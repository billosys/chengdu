#!/usr/bin/env bash
# Records each macOS binary's minimum-OS deployment target
# (LC_BUILD_VERSION's minos, via otool -l) into
# dist/macos-arm64/provenance.txt as an appended observation block.
# Observation only — not yet a compat-floor policy; that decision is
# arc02's (release packaging), with this as its input.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
DIST_DIR="$REPO_ROOT/dist/macos-arm64"
PROVENANCE="$DIST_DIR/provenance.txt"

if [ ! -d "$DIST_DIR" ]; then
  echo "record-min-os.sh: $DIST_DIR missing — run the macOS build scripts first" >&2
  exit 1
fi

{
  echo "min-os-macos-arm64:"
  for bin in pandaPIparser pandaPIgrounder pandaPIengine; do
    path="$DIST_DIR/$bin"
    if [ ! -x "$path" ]; then
      echo "record-min-os.sh: $path missing or not executable" >&2
      exit 1
    fi
    minos="$(otool -l "$path" | awk '/LC_BUILD_VERSION/{f=1} f && /minos/{print $2; exit}')"
    echo "$bin=$minos"
  done
  echo "---"
} >> "$PROVENANCE"

echo "record-min-os.sh: OK: recorded min-OS observation in $PROVENANCE"
