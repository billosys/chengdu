#!/usr/bin/env bash
# Mechanizes PANDA Runbook §4.2 + §2-macOS: build pandaPIgrounder.
#
# Two-stage build (cpddl third-party deps + libpddl.a, then the grounder's
# own src/), with three traps handled explicitly:
#   - bliss: cpddl's own "bliss" make target rm-rf's and re-unzips
#     third-party/bliss on every invocation, so it is built manually here
#     (extract once, patch, build directly) — a single code path applied
#     on both platforms, since the __DATE__ fix is valid C++ everywhere.
#   - boruvka macOS patches: ship inside the pandaPIgrounder checkout
#     itself (not vendored under patches/) and only apply on macOS,
#     matching upstream intent. They are applied here explicitly because
#     the in-tree boruvka source is already present before cpddl's
#     Makefile-driven auto-apply (added by 0002-makefile.patch) can fire.
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
PATCHES_DIR="$REPO_ROOT/patches"

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

APPLIED_PATCHES="0002-makefile.patch"

# --- apply the upstream cpddl makefile patch (both platforms) ---
if ( cd "$CPDDL_DIR" && patch -p1 --dry-run --silent < ../0002-makefile.patch ) >/dev/null 2>&1; then
  echo "build-grounder.sh: applying patch: 0002-makefile.patch"
  ( cd "$CPDDL_DIR" && patch -p1 --silent < ../0002-makefile.patch )
elif ( cd "$CPDDL_DIR" && patch -p1 --reverse --dry-run --silent < ../0002-makefile.patch ) >/dev/null 2>&1; then
  echo "build-grounder.sh: patch already applied, skipping: 0002-makefile.patch"
else
  echo "build-grounder.sh: FAIL: 0002-makefile.patch neither applies nor is already applied" >&2
  exit 1
fi

# --- macOS-only boruvka patches, applied from the grounder checkout ---
if [ "$PLATFORM" = "macos-arm64" ]; then
  BORUVKA_DIR="$CPDDL_DIR/third-party/boruvka"
  for p in 0001-Removed-non-macos-call-in-unused-function.patch 0001-boruvka-endian.patch; do
    if ( cd "$BORUVKA_DIR" && patch -p1 --dry-run --silent < "$SRC_ROOT/$p" ) >/dev/null 2>&1; then
      echo "build-grounder.sh: applying patch: $p"
      ( cd "$BORUVKA_DIR" && patch -p1 --silent < "$SRC_ROOT/$p" )
      APPLIED_PATCHES="$APPLIED_PATCHES,$p"
    else
      echo "build-grounder.sh: FAIL: $p does not apply cleanly to boruvka" >&2
      exit 1
    fi
  done
fi

# --- bliss: manual extract + patch, bypassing the rm-rf/unzip make target ---
rm -rf "$CPDDL_DIR/third-party/bliss"
( cd "$CPDDL_DIR/third-party" && unzip -q bliss-0.73.zip && mv bliss-0.73 bliss )
( cd "$CPDDL_DIR/third-party/bliss" && patch -p1 --silent < ../bliss-0.73-memleak.patch )
echo "build-grounder.sh: applying patch: bliss-0.73-cxx11-string-literal.patch"
( cd "$CPDDL_DIR/third-party/bliss" && patch -p1 --silent < "$PATCHES_DIR/bliss-0.73-cxx11-string-literal.patch" )
APPLIED_PATCHES="$APPLIED_PATCHES,bliss-0.73-cxx11-string-literal.patch"
( cd "$CPDDL_DIR" && make CC="$GROUNDER_CXX" -C third-party/bliss )

# --- cpddl third-party deps (bliss target is now a no-op: libbliss.a exists) ---
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
append_provenance "$DIST_DIR" "pandaPIgrounder" "$GROUNDER_SHA" "$APPLIED_PATCHES" "$COMPILER"

echo "build-grounder.sh: OK: $DIST_DIR/pandaPIgrounder"
