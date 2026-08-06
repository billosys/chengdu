#!/usr/bin/env bash
# Validates dist/<platform>/provenance.txt against pins.env: every
# component's SHA matches its pin, the grounder's patch list matches the
# exact per-platform expected set (order-independent), and every
# component's compiler field is non-empty. Retires slice02's W-4/W-5
# residue — "CC attested the provenance file" — mechanically: no CI run
# is green unless the provenance is actually right.
#
# Usage: check-provenance.sh [--platform linux-x86_64|macos-arm64] [PROVENANCE_FILE]
#   Defaults to dist/<platform>/provenance.txt, PLATFORM auto-detected
#   from the running machine. --platform overrides detection — needed
#   by package-release.sh, which validates both platforms' provenance
#   files from a single packaging runner (auto-detection would check
#   the other platform's file against the wrong expected patch set).
#   An explicit FILE path is accepted so a tamper test can point this at
#   a hand-edited copy without disturbing a real build's output.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM_OVERRIDE=""
while [ "$#" -gt 0 ]; do
  case "$1" in
    --platform)
      PLATFORM_OVERRIDE="${2:-}"
      shift 2
      ;;
    --platform=*)
      PLATFORM_OVERRIDE="${1#*=}"
      shift
      ;;
    *)
      break
      ;;
  esac
done

if [ -n "$PLATFORM_OVERRIDE" ]; then
  case "$PLATFORM_OVERRIDE" in
    linux-x86_64|macos-arm64)
      PLATFORM="$PLATFORM_OVERRIDE"
      ;;
    *)
      echo "check-provenance.sh: FAIL: invalid --platform '$PLATFORM_OVERRIDE' (want linux-x86_64 or macos-arm64)" >&2
      exit 1
      ;;
  esac
else
  PLATFORM="$(detect_platform)"
fi

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

check_component() {
  local name="$1" expected_sha="$2" expected_patches="$3"
  local block sha patches compiler

  block="$(provenance_get_block "$FILE" "$name")"
  if [ -z "$block" ]; then
    fail "$name: no provenance block found in $FILE"
    return
  fi

  sha="$(provenance_get_field "$block" sha)"
  patches="$(provenance_get_field "$block" patches)"
  compiler="$(provenance_get_field "$block" compiler)"

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
