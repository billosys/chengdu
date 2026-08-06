#!/usr/bin/env bash
# Packages a chengdu release from already-built dist/<platform>/ trees:
# per-platform tarballs, a SHA256SUMS covering them, an aggregated
# release-manifest.txt, and rendered release notes. Reuses
# check-provenance.sh as a pre-package gate, per platform explicitly —
# this script may run on a runner that matches neither, one, or both of
# the platforms it packages, so provenance is never auto-detected here.
#
# Usage: package-release.sh TAG
#   Packages every dist/<platform>/ tree present under dist/ (both in
#   CI; a subset is fine for local/CDC reproduction). Output lands in
#   release/, which is removed and recreated on each run.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

TAG="${1:-}"
if [ -z "$TAG" ]; then
  echo "package-release.sh: usage: package-release.sh TAG" >&2
  exit 1
fi

OUT_DIR="$REPO_ROOT/release"
DIST_ROOT="$REPO_ROOT/dist"
TEMPLATE="$REPO_ROOT/.github/release-notes.md.in"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

# --- discover which platform dist trees are present ---
PLATFORMS=""
for p in linux-x86_64 macos-arm64; do
  if [ -f "$DIST_ROOT/$p/provenance.txt" ]; then
    PLATFORMS="$PLATFORMS $p"
  fi
done
PLATFORMS="${PLATFORMS# }"

if [ -z "$PLATFORMS" ]; then
  echo "package-release.sh: FAIL: no dist/<platform>/provenance.txt found under $DIST_ROOT" >&2
  exit 1
fi
echo "package-release.sh: packaging for:$PLATFORMS"

# --- pre-package gate: every present platform's provenance must be
# right, checked explicitly per platform since detection can't be
# trusted to match the packaging runner. ---
for p in $PLATFORMS; do
  echo "package-release.sh: checking provenance for $p"
  "$SCRIPT_DIR/check-provenance.sh" --platform "$p" "$DIST_ROOT/$p/provenance.txt"
done

# --- tarballs: pandapi-<tag>-<platform>.tar.gz, 3 binaries + provenance.txt
# + fixtures/ (the byte-verified minimal pair, so an installed consumer
# can run a real --verify offline, per project ledger P4) ---
FIXTURES_DIR="$REPO_ROOT/fixtures/minimal"
for f in domain.hddl problem.hddl; do
  if [ ! -f "$FIXTURES_DIR/$f" ]; then
    echo "package-release.sh: FAIL: $FIXTURES_DIR/$f missing" >&2
    exit 1
  fi
done

for p in $PLATFORMS; do
  for bin in pandaPIparser pandaPIgrounder pandaPIengine; do
    if [ ! -x "$DIST_ROOT/$p/$bin" ]; then
      echo "package-release.sh: FAIL: $DIST_ROOT/$p/$bin missing or not executable" >&2
      exit 1
    fi
  done
  tarball="pandapi-$TAG-$p.tar.gz"
  stage="$OUT_DIR/.stage-$p"
  rm -rf "$stage"
  mkdir -p "$stage/fixtures"
  cp "$DIST_ROOT/$p/pandaPIparser" "$DIST_ROOT/$p/pandaPIgrounder" "$DIST_ROOT/$p/pandaPIengine" "$DIST_ROOT/$p/provenance.txt" "$stage/"
  cp "$FIXTURES_DIR/domain.hddl" "$FIXTURES_DIR/problem.hddl" "$stage/fixtures/"
  tar czf "$OUT_DIR/$tarball" -C "$stage" pandaPIparser pandaPIgrounder pandaPIengine provenance.txt fixtures
  rm -rf "$stage"
  echo "package-release.sh: built $tarball (incl. fixtures/)"
done

# --- THIRD-PARTY-LICENSES: assembled from the committed licenses/
# directory, per docs/license-audit-v0.1.0.md. Fails loudly if any
# audited component's license text is missing — the release must never
# ship silently short of what the audit found. ---
LICENSES_DIR="$REPO_ROOT/licenses"
THIRD_PARTY="$OUT_DIR/THIRD-PARTY-LICENSES"
: > "$THIRD_PARTY"
for entry in \
  "pandaPIparser:BSD 3-Clause:pandaPIparser-BSD-3-Clause.txt" \
  "pandaPIgrounder:BSD 3-Clause:pandaPIgrounder-BSD-3-Clause.txt" \
  "pandaPIengine:BSD 3-Clause:pandaPIengine-BSD-3-Clause.txt" \
  "cpddl:BSD 3-Clause:cpddl-BSD-3-Clause.txt" \
  "boruvka:BSD 3-Clause:boruvka-BSD-3-Clause.txt" \
  "h2-fd-preprocessor (linked into pandaPIgrounder):GPL-3.0:h2-fd-preprocessor-GPL-3.0.txt"
do
  name="${entry%%:*}"
  rest="${entry#*:}"
  license="${rest%%:*}"
  file="${rest#*:}"
  path="$LICENSES_DIR/$file"
  if [ ! -f "$path" ]; then
    echo "package-release.sh: FAIL: $path missing — licenses/ is out of sync with docs/license-audit-v0.1.0.md" >&2
    exit 1
  fi
  {
    echo "================================================================"
    echo "$name — $license"
    echo "================================================================"
    echo
    cat "$path"
    echo
  } >> "$THIRD_PARTY"
done
echo "package-release.sh: built THIRD-PARTY-LICENSES (6 components)"

# --- aggregated manifest: tag + chengdu commit + every platform's full provenance ---
MANIFEST="$OUT_DIR/release-manifest.txt"
{
  echo "tag=$TAG"
  echo "chengdu_commit=$(git -C "$REPO_ROOT" rev-parse HEAD)"
  echo "==="
  for p in $PLATFORMS; do
    echo "platform=$p"
    cat "$DIST_ROOT/$p/provenance.txt"
  done
} > "$MANIFEST"
echo "package-release.sh: built release-manifest.txt"

# --- checksums: every asset except the checksum file itself ---
(
  cd "$OUT_DIR"
  sha256sum -- *.tar.gz release-manifest.txt THIRD-PARTY-LICENSES > SHA256SUMS
)
echo "package-release.sh: built SHA256SUMS"

# --- release notes, rendered from the template ---
NOTES="$OUT_DIR/notes.md"
COMMIT="$(git -C "$REPO_ROOT" rev-parse HEAD)"
COMMIT_SHORT="$(git -C "$REPO_ROOT" rev-parse --short HEAD)"

render_provenance_table() {
  local p comp block sha patches count
  for p in $PLATFORMS; do
    for comp in pandaPIparser pandaPIgrounder pandaPIengine; do
      block="$(provenance_get_block "$DIST_ROOT/$p/provenance.txt" "$comp")"
      sha="$(provenance_get_field "$block" sha)"
      patches="$(provenance_get_field "$block" patches)"
      if [ "$patches" = "none" ]; then
        count=0
      else
        # NF over comma-separated fields, not `wc -l` on tr'd newlines —
        # wc -l undercounts by one when the last field has no trailing
        # newline (found in local testing: 4 patches counted as 3).
        count="$(printf '%s' "$patches" | awk -F',' '{print NF}')"
      fi
      # shellcheck disable=SC2016  # single-quoted on purpose: the backticks are literal markdown code-formatting, not command substitution, and %s placeholders are printf's, not shell expansion
      printf '| %s | %s | `%s` | %s |\n' "$p" "$comp" "$sha" "$count"
    done
  done
}

PROVENANCE_TABLE="$(render_provenance_table)"

sed \
  -e "s|@@TAG@@|$TAG|g" \
  -e "s|@@COMMIT@@|$COMMIT|g" \
  -e "s|@@COMMIT_SHORT@@|$COMMIT_SHORT|g" \
  "$TEMPLATE" > "$NOTES"

# The provenance table is multi-line, so splice it in with awk rather
# than fight sed's single-line substitution semantics.
awk -v table="$PROVENANCE_TABLE" '{ if ($0 == "@@PROVENANCE_TABLE@@") print table; else print }' \
  "$NOTES" > "$NOTES.tmp" && mv "$NOTES.tmp" "$NOTES"
echo "package-release.sh: rendered notes.md"

# --- prerelease marker: a semver hyphen suffix denotes a pre-release ---
case "$TAG" in
  v*-*)
    echo "true" > "$OUT_DIR/PRERELEASE"
    ;;
  *)
    echo "false" > "$OUT_DIR/PRERELEASE"
    ;;
esac

echo "package-release.sh: OK: release/ ready for $TAG ($PLATFORMS)"
