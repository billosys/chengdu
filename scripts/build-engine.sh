#!/usr/bin/env bash
# Mechanizes PANDA Runbook §4.3: build pandaPIengine via cmake + make.
# Note: `cmake ../src`, not `cmake ..` — the CMakeLists lives in src/.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
SRC_DIR="$(prepare_build_source_copy "pandaPIengine")"
BUILD_DIR="$SRC_DIR/build"
DIST_DIR="$REPO_ROOT/dist/$PLATFORM"

# shellcheck source=/dev/null
. "$REPO_ROOT/vendor.env"

echo "build-engine.sh: building pandaPIengine for $PLATFORM"
mkdir -p "$BUILD_DIR"
( cd "$BUILD_DIR" && cmake ../src -DCMAKE_BUILD_TYPE=Release )
( cd "$BUILD_DIR" && make -j )

if [ ! -x "$BUILD_DIR/pandaPIengine" ]; then
  echo "build-engine.sh: build did not produce an executable pandaPIengine" >&2
  exit 1
fi

mkdir -p "$DIST_DIR"
cp "$BUILD_DIR/pandaPIengine" "$DIST_DIR/pandaPIengine"

COMPILER="$(resolve_compiler_id c++)"
append_provenance "$DIST_DIR" "pandaPIengine" "ENGINE" "$COMPILER"

echo "build-engine.sh: OK: $DIST_DIR/pandaPIengine"
