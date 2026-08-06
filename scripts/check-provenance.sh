#!/usr/bin/env bash
# Validates dist/<platform>/provenance.txt against pins.env: every
# component's SHA matches its pin, the grounder's patch list matches the
# exact per-platform expected set (order-independent), and every
# component's compiler field is non-empty. Retires slice02's W-4/W-5
# residue — "CC attested the provenance file" — mechanically: no CI run
# is green unless the provenance is actually right.
#
# Usage: check-provenance.sh [PROVENANCE_FILE]
#   Defaults to dist/<platform>/provenance.txt. An explicit path is
#   accepted so a tamper test can point this at a hand-edited copy
#   without disturbing a real build's output.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
FILE="${1:-$REPO_ROOT/dist/$PLATFORM/provenance.txt}"

# shellcheck source=/dev/null
. "$REPO_ROOT/pins.env"

if [ ! -f "$FILE" ]; then
  echo "check-provenance.sh: FAIL: $FILE not found — run the build scripts first" >&2
  exit 1
fi

case "$PLATFORM" in
  linux-x86_64)
    GROUNDER_PATCHES_EXPECTED="0002-makefile.patch,bliss-0.73-cxx11-string-literal.patch"
    ;;
  macos-arm64)
    GROUNDER_PATCHES_EXPECTED="0002-makefile.patch,0001-Removed-non-macos-call-in-unused-function.patch,0001-boruvka-endian.patch,bliss-0.73-cxx11-string-literal.patch"
    ;;
esac

FAIL_COUNT=0

fail() {
  echo "check-provenance.sh: FAIL: $1" >&2
  FAIL_COUNT=$((FAIL_COUNT + 1))
}

# sorted_csv <csv> — comma-separated list, sorted, for an
# order-independent comparison of patch lists.
sorted_csv() {
  printf '%s' "$1" | tr ',' '\n' | sort | tr '\n' ',' | sed 's/,$//'
}

# get_field <block> <key> — extract key=value from a provenance block.
get_field() {
  printf '%s\n' "$1" | sed -n "s/^$2=//p"
}

# get_block <name> — the provenance block for one component, from
# "component=<name>" through its trailing "---" line, inclusive.
get_block() {
  awk -v c="component=$1" 'BEGIN{f=0} $0==c{f=1} f{print} f && /^---$/{exit}' "$FILE"
}

check_component() {
  local name="$1" expected_sha="$2" expected_patches="$3"
  local block sha patches compiler

  block="$(get_block "$name")"
  if [ -z "$block" ]; then
    fail "$name: no provenance block found in $FILE"
    return
  fi

  sha="$(get_field "$block" sha)"
  patches="$(get_field "$block" patches)"
  compiler="$(get_field "$block" compiler)"

  if [ "$sha" != "$expected_sha" ]; then
    fail "$name: sha mismatch — provenance has '$sha', pins.env has '$expected_sha'"
  fi

  if [ "$(sorted_csv "$patches")" != "$(sorted_csv "$expected_patches")" ]; then
    fail "$name: patches mismatch on $PLATFORM — provenance has '$patches', expected '$expected_patches'"
  fi

  if [ -z "$compiler" ]; then
    fail "$name: compiler field is empty"
  fi
}

check_component "pandaPIparser" "$PARSER_SHA" "none"
check_component "pandaPIgrounder" "$GROUNDER_SHA" "$GROUNDER_PATCHES_EXPECTED"
check_component "pandaPIengine" "$ENGINE_SHA" "none"

if [ "$FAIL_COUNT" -gt 0 ]; then
  echo "check-provenance.sh: $FAIL_COUNT check(s) failed against $FILE" >&2
  exit 1
fi

echo "check-provenance.sh: OK: all 3 components verified against pins.env ($PLATFORM)"
