#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

"$SCRIPT_DIR/build-runtime.sh"
"$SCRIPT_DIR/build-parser.sh"
"$SCRIPT_DIR/build-grounder.sh"
"$SCRIPT_DIR/build-engine.sh"
