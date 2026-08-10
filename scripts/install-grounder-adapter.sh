#!/usr/bin/env bash
# Install the chengdu grounder adapter around a built inherited pandaPIgrounder.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -ne 1 ]; then
  echo "install-grounder-adapter.sh: usage: install-grounder-adapter.sh DIST_DIR" >&2
  exit 1
fi

DIST_DIR="$1"
RAW="$DIST_DIR/pandaPIgrounder"
LEGACY="$DIST_DIR/pandaPIgrounder.legacy"
ADAPTER="$SCRIPT_DIR/pandapi-grounder-adapter.sh"

if [ ! -d "$DIST_DIR" ]; then
  echo "install-grounder-adapter.sh: missing dist directory: $DIST_DIR" >&2
  exit 1
fi

if [ ! -x "$ADAPTER" ]; then
  echo "install-grounder-adapter.sh: missing adapter: $ADAPTER" >&2
  exit 1
fi

if [ ! -x "$LEGACY" ]; then
  if [ ! -x "$RAW" ]; then
    echo "install-grounder-adapter.sh: missing built grounder: $RAW" >&2
    exit 1
  fi
  mv "$RAW" "$LEGACY"
fi

cp "$ADAPTER" "$DIST_DIR/pandapi-grounder"
cp "$ADAPTER" "$DIST_DIR/pandaPIgrounder"
chmod +x "$LEGACY" "$DIST_DIR/pandapi-grounder" "$DIST_DIR/pandaPIgrounder"

echo "install-grounder-adapter.sh: OK: $DIST_DIR/pandapi-grounder and $DIST_DIR/pandaPIgrounder"
