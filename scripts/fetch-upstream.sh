#!/usr/bin/env bash
# Clone the three pandaPI components at the SHAs pinned in pins.env into
# upstream/, or (--source pandadealer) clone the vendored IPC 2023
# competition snapshot as a fallback when the environment cannot reach
# gitlab.com for the cpddl submodule. Verifies every checkout against its
# pin and fails loudly — never silently — on any mismatch.
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: fetch-upstream.sh [--source canonical|pandadealer]

  --source canonical    (default) clone pandaPIparser, pandaPIgrounder
                         (with submodules), pandaPIengine at their pinned
                         SHAs from pins.env.
  --source pandadealer  clone the ipc2023-htn/PandaDealer vendored
                         snapshot instead, and symlink upstream/pandaPI*
                         to its 00-parser/01-grounder/02-planner subdirs.

                         FETCH-ONLY: the vendored snapshot ships no patch
                         files and its grounder/engine sources don't build
                         cleanly on a modern Linux/GCC toolchain (found
                         2026-08-05: pre-final concepts-TS syntax GCC-13
                         rejects, and a missing <cstdint> include).
                         build-grounder.sh refuses to run against it.
                         Use this mode to inspect or diff the competition
                         snapshot, not to produce binaries — canonical
                         mode is the only buildable source today.
EOF
}

SOURCE_MODE=canonical
while [ "$#" -gt 0 ]; do
  case "$1" in
    --source)
      SOURCE_MODE="${2:-}"
      shift 2
      ;;
    --source=*)
      SOURCE_MODE="${1#*=}"
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "fetch-upstream.sh: unknown argument: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

case "$SOURCE_MODE" in
  canonical|pandadealer) ;;
  *)
    echo "fetch-upstream.sh: --source must be 'canonical' or 'pandadealer' (got: $SOURCE_MODE)" >&2
    exit 1
    ;;
esac

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
UPSTREAM_DIR="$REPO_ROOT/upstream"

# shellcheck source=/dev/null
. "$REPO_ROOT/pins.env"

fail() {
  echo "fetch-upstream.sh: FAIL: $*" >&2
  exit 1
}

verify_sha() {
  local dir="$1" expected="$2" label="$3" actual
  actual="$(git -C "$dir" rev-parse HEAD)"
  if [ "$actual" != "$expected" ]; then
    fail "$label: expected $expected, got $actual (pin mismatch — refusing to continue)"
  fi
  echo "fetch-upstream.sh: OK: $label at $actual"
}

fetch_canonical() {
  rm -rf "$UPSTREAM_DIR/pandaPIparser" "$UPSTREAM_DIR/pandaPIgrounder" \
    "$UPSTREAM_DIR/pandaPIengine" "$UPSTREAM_DIR/PandaDealer"
  mkdir -p "$UPSTREAM_DIR"

  echo "fetch-upstream.sh: cloning pandaPIparser @ $PARSER_SHA"
  git clone --quiet "$PARSER_URL" "$UPSTREAM_DIR/pandaPIparser"
  git -C "$UPSTREAM_DIR/pandaPIparser" checkout --quiet "$PARSER_SHA"
  verify_sha "$UPSTREAM_DIR/pandaPIparser" "$PARSER_SHA" "pandaPIparser"

  echo "fetch-upstream.sh: cloning pandaPIgrounder @ $GROUNDER_SHA (with submodules)"
  git clone --quiet "$GROUNDER_URL" "$UPSTREAM_DIR/pandaPIgrounder"
  git -C "$UPSTREAM_DIR/pandaPIgrounder" checkout --quiet "$GROUNDER_SHA"
  git -C "$UPSTREAM_DIR/pandaPIgrounder" submodule update --init --recursive
  verify_sha "$UPSTREAM_DIR/pandaPIgrounder" "$GROUNDER_SHA" "pandaPIgrounder"

  local sub sub_dir
  for sub in cpddl h2-fd-preprocessor; do
    sub_dir="$UPSTREAM_DIR/pandaPIgrounder/$sub"
    if [ ! -d "$sub_dir" ] || [ -z "$(ls -A "$sub_dir" 2>/dev/null)" ]; then
      fail "submodule $sub not initialized under pandaPIgrounder"
    fi
    echo "fetch-upstream.sh: OK: submodule $sub initialized"
  done

  echo "fetch-upstream.sh: cloning pandaPIengine @ $ENGINE_SHA"
  git clone --quiet "$ENGINE_URL" "$UPSTREAM_DIR/pandaPIengine"
  git -C "$UPSTREAM_DIR/pandaPIengine" checkout --quiet "$ENGINE_SHA"
  verify_sha "$UPSTREAM_DIR/pandaPIengine" "$ENGINE_SHA" "pandaPIengine"
}

fetch_pandadealer() {
  rm -rf "$UPSTREAM_DIR/PandaDealer" "$UPSTREAM_DIR/pandaPIparser" \
    "$UPSTREAM_DIR/pandaPIgrounder" "$UPSTREAM_DIR/pandaPIengine"
  mkdir -p "$UPSTREAM_DIR"

  echo "fetch-upstream.sh: cloning PandaDealer @ $PANDADEALER_SHA (vendored fallback)"
  git clone --quiet "$PANDADEALER_URL" "$UPSTREAM_DIR/PandaDealer"
  git -C "$UPSTREAM_DIR/PandaDealer" checkout --quiet "$PANDADEALER_SHA"
  verify_sha "$UPSTREAM_DIR/PandaDealer" "$PANDADEALER_SHA" "PandaDealer"

  local pair name sub
  for pair in "pandaPIparser:00-parser" "pandaPIgrounder:01-grounder" "pandaPIengine:02-planner"; do
    name="${pair%%:*}"
    sub="${pair##*:}"
    if [ ! -d "$UPSTREAM_DIR/PandaDealer/$sub" ]; then
      fail "PandaDealer layout missing expected dir: $sub"
    fi
    ln -s "PandaDealer/$sub" "$UPSTREAM_DIR/$name"
    echo "fetch-upstream.sh: OK: upstream/$name -> PandaDealer/$sub"
  done
}

case "$SOURCE_MODE" in
  canonical) fetch_canonical ;;
  pandadealer) fetch_pandadealer ;;
esac

echo "fetch-upstream.sh: done ($SOURCE_MODE)"
