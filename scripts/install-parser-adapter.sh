#!/usr/bin/env bash
# Install the chengdu parser adapter around a built inherited pandaPIparser.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -ne 1 ]; then
  echo "install-parser-adapter.sh: usage: install-parser-adapter.sh DIST_DIR" >&2
  exit 1
fi

DIST_DIR="$1"
RAW="$DIST_DIR/pandaPIparser"
LEGACY="$DIST_DIR/pandaPIparser.legacy"
ADAPTER="$SCRIPT_DIR/pandapi-parser-adapter.sh"

if [ ! -d "$DIST_DIR" ]; then
  echo "install-parser-adapter.sh: missing dist directory: $DIST_DIR" >&2
  exit 1
fi

if [ ! -x "$ADAPTER" ]; then
  echo "install-parser-adapter.sh: missing adapter: $ADAPTER" >&2
  exit 1
fi

if [ ! -x "$LEGACY" ]; then
  if [ ! -x "$RAW" ]; then
    echo "install-parser-adapter.sh: missing built parser: $RAW" >&2
    exit 1
  fi
  mv "$RAW" "$LEGACY"
fi

cp "$ADAPTER" "$DIST_DIR/pandapi-parser"
cp "$ADAPTER" "$DIST_DIR/pandaPIparser"
chmod +x "$LEGACY" "$DIST_DIR/pandapi-parser" "$DIST_DIR/pandaPIparser"

echo "install-parser-adapter.sh: OK: $DIST_DIR/pandapi-parser and $DIST_DIR/pandaPIparser"
