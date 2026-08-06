#!/usr/bin/env bash
# Publishes a packaged chengdu release via `gh release create`, from
# package-release.sh's output. Requires `gh` authenticated with
# `contents: write` on the target repo (in CI: GH_TOKEN set to the
# workflow's own token).
#
# Usage: publish-release.sh TAG [RELEASE_DIR]
#   RELEASE_DIR defaults to ./release (package-release.sh's own default
#   output location).
set -euo pipefail

TAG="${1:-}"
RELEASE_DIR="${2:-release}"

if [ -z "$TAG" ]; then
  echo "publish-release.sh: usage: publish-release.sh TAG [RELEASE_DIR]" >&2
  exit 1
fi

if [ ! -f "$RELEASE_DIR/PRERELEASE" ]; then
  echo "publish-release.sh: FAIL: $RELEASE_DIR/PRERELEASE not found — run package-release.sh first" >&2
  exit 1
fi

PRERELEASE_ARGS=()
if [ "$(cat "$RELEASE_DIR/PRERELEASE")" = "true" ]; then
  PRERELEASE_ARGS+=(--prerelease)
fi

gh release create "$TAG" \
  --title "$TAG" \
  --notes-file "$RELEASE_DIR/notes.md" \
  "${PRERELEASE_ARGS[@]}" \
  "$RELEASE_DIR"/pandapi-*.tar.gz "$RELEASE_DIR/SHA256SUMS" "$RELEASE_DIR/release-manifest.txt"

echo "publish-release.sh: OK: published $TAG"
