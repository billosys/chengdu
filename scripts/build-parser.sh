#!/usr/bin/env bash
# Mechanizes PANDA Runbook §4.1: build pandaPIparser and land it in
# dist/<platform>/. No patches apply to the parser.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
SRC_DIR="$REPO_ROOT/upstream/pandaPIparser"
DIST_DIR="$REPO_ROOT/dist/$PLATFORM"

if [ ! -d "$SRC_DIR" ]; then
  echo "build-parser.sh: $SRC_DIR missing — run scripts/fetch-upstream.sh first" >&2
  exit 1
fi

echo "build-parser.sh: building pandaPIparser for $PLATFORM"
( cd "$SRC_DIR" && make -j )

if [ ! -x "$SRC_DIR/pandaPIparser" ]; then
  echo "build-parser.sh: build did not produce an executable pandaPIparser" >&2
  exit 1
fi

mkdir -p "$DIST_DIR"
cp "$SRC_DIR/pandaPIparser" "$DIST_DIR/pandaPIparser"

SHA="$(git -C "$SRC_DIR" rev-parse HEAD)"
COMPILER="$(resolve_compiler_id g++)"
append_provenance "$DIST_DIR" "pandaPIparser" "$SHA" "none" "$COMPILER"

echo "build-parser.sh: OK: $DIST_DIR/pandaPIparser"
