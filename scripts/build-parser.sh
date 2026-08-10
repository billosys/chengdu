#!/usr/bin/env bash
# Mechanizes PANDA Runbook §4.1: build pandaPIparser and land it in
# dist/<platform>/. No patches apply to the parser.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
SRC_DIR="$(prepare_build_source_copy "pandaPIparser")"
DIST_DIR="$REPO_ROOT/dist/$PLATFORM"

# shellcheck source=/dev/null
. "$REPO_ROOT/vendor.env"

echo "build-parser.sh: building pandaPIparser for $PLATFORM"
( cd "$SRC_DIR" && make -j )

if [ ! -x "$SRC_DIR/pandaPIparser" ]; then
  echo "build-parser.sh: build did not produce an executable pandaPIparser" >&2
  exit 1
fi

mkdir -p "$DIST_DIR"
cp "$SRC_DIR/pandaPIparser" "$DIST_DIR/pandaPIparser"
"$REPO_ROOT/scripts/install-parser-adapter.sh" "$DIST_DIR"

COMPILER="$(resolve_compiler_id g++)"
append_provenance "$DIST_DIR" "pandaPIparser" "PARSER" "$COMPILER"

echo "build-parser.sh: OK: $DIST_DIR/pandapi-parser and $DIST_DIR/pandaPIparser"
