#!/usr/bin/env bash
# Mechanizes PANDA Runbook §4.2 + §2-macOS: build pandaPIgrounder.
#
# Two-stage build (cpddl third-party deps + libpddl.a, then the grounder's
# own src/), with the only script-level trap handled explicitly:
#   - grounder src/ hardcodes g++-11 on Darwin via a plain '=' assignment,
#     which environment variables cannot override — only command-line
#     make variables can, so the compiler override happens there, not
#     via export.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
SRC_ROOT="$(prepare_build_source_copy "pandaPIgrounder")"
CPDDL_DIR="$SRC_ROOT/cpddl"
DIST_DIR="$REPO_ROOT/dist/$PLATFORM"

# shellcheck source=/dev/null
. "$REPO_ROOT/pins.env"

if [ "$PLATFORM" = "macos-arm64" ]; then
  : "${GROUNDER_CC:=cc}"
  : "${GROUNDER_CXX:=c++}"
else
  : "${GROUNDER_CC:=gcc}"
  : "${GROUNDER_CXX:=g++}"
fi

echo "build-grounder.sh: building pandaPIgrounder for $PLATFORM (CC=$GROUNDER_CC CXX=$GROUNDER_CXX)"

# --- cpddl third-party deps ---
( cd "$CPDDL_DIR" && CC="$GROUNDER_CC" CXX="$GROUNDER_CXX" make boruvka opts bliss lpsolve )

# --- libpddl.a ---
( cd "$CPDDL_DIR" && CC="$GROUNDER_CC" CXX="$GROUNDER_CXX" make )

# --- grounder itself: command-line vars, not env, to beat the plain '=' Darwin default ---
( cd "$SRC_ROOT/src" && make -j CXX="$GROUNDER_CXX" CC="$GROUNDER_CC" )

if [ ! -x "$SRC_ROOT/pandaPIgrounder" ]; then
  echo "build-grounder.sh: build did not produce an executable pandaPIgrounder" >&2
  exit 1
fi

mkdir -p "$DIST_DIR"
cp "$SRC_ROOT/pandaPIgrounder" "$DIST_DIR/pandaPIgrounder"

COMPILER="$(resolve_compiler_id "$GROUNDER_CXX")"
append_provenance "$DIST_DIR" "pandaPIgrounder" "$GROUNDER_SHA" "none" "$COMPILER"

echo "build-grounder.sh: OK: $DIST_DIR/pandaPIgrounder"
