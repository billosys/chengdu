#!/usr/bin/env bash
# Mechanizes PANDA Runbook §4.3: build pandaPIengine via cmake + make.
# Note: `cmake ../src`, not `cmake ..` — the CMakeLists lives in src/.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
SRC_DIR="$REPO_ROOT/upstream/pandaPIengine"
BUILD_DIR="$SRC_DIR/build"
DIST_DIR="$REPO_ROOT/dist/$PLATFORM"

if [ ! -d "$SRC_DIR" ]; then
  echo "build-engine.sh: $SRC_DIR missing — run scripts/fetch-upstream.sh first" >&2
  exit 1
fi

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

SHA="$(git -C "$SRC_DIR" rev-parse HEAD)"
COMPILER="$(resolve_compiler_id c++)"
append_provenance "$DIST_DIR" "pandaPIengine" "$SHA" "none" "$COMPILER"

echo "build-engine.sh: OK: $DIST_DIR/pandaPIengine"
