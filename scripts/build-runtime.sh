#!/usr/bin/env bash
# Build and test the chengdu-owned pandapi-runtime skeleton out of source.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
SRC_DIR="$REPO_ROOT/pandapi-runtime"
BUILD_DIR="$REPO_ROOT/build/runtime/$PLATFORM"

echo "build-runtime.sh: building pandapi-runtime for $PLATFORM"
mkdir -p "$BUILD_DIR"
cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build "$BUILD_DIR" --parallel
( cd "$BUILD_DIR" && ctest --output-on-failure -C Release )

echo "build-runtime.sh: OK: $BUILD_DIR"
