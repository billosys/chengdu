#!/usr/bin/env bash
# Build and test pandapi-runtime with ASan/UBSan instrumentation.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

find_tool() {
  local name="$1"
  if command -v "$name" >/dev/null 2>&1; then
    command -v "$name"
    return 0
  fi
  if command -v xcrun >/dev/null 2>&1; then
    xcrun --find "$name" 2>/dev/null || true
  fi
}

CXX_BIN="${CXX:-$(find_tool clang++)}"
if [ -z "$CXX_BIN" ]; then
  echo "sanitize-runtime.sh: clang++ is required for ASan/UBSan" >&2
  exit 1
fi

PLATFORM="$(detect_platform)"
SRC_DIR="$REPO_ROOT/pandapi-runtime"
BUILD_DIR="$REPO_ROOT/build/runtime-sanitize/$PLATFORM"
SANITIZER_FLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined"

echo "sanitize-runtime.sh: building pandapi-runtime sanitizer gate for $PLATFORM"
mkdir -p "$BUILD_DIR"
cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_CXX_COMPILER="$CXX_BIN" \
  -DCMAKE_CXX_FLAGS="$SANITIZER_FLAGS" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build "$BUILD_DIR" --parallel
( cd "$BUILD_DIR" && ctest --output-on-failure -C RelWithDebInfo )

echo "sanitize-runtime.sh: OK: $BUILD_DIR"
