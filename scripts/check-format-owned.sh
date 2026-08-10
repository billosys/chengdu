#!/usr/bin/env bash
# Check formatting for chengdu-owned runtime C++ source and tests.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CLANG_FORMAT="${CLANG_FORMAT:-}"
if [ -z "$CLANG_FORMAT" ]; then
  if command -v clang-format >/dev/null 2>&1; then
    CLANG_FORMAT="$(command -v clang-format)"
  elif command -v xcrun >/dev/null 2>&1; then
    CLANG_FORMAT="$(xcrun --find clang-format 2>/dev/null || true)"
  fi
fi

if [ -z "$CLANG_FORMAT" ] || [ ! -x "$CLANG_FORMAT" ]; then
  echo "check-format-owned.sh: clang-format not found on PATH or through xcrun" >&2
  exit 1
fi

files=()
while IFS= read -r -d '' file; do
  files+=("$file")
done < <(
  find \
    "$REPO_ROOT/pandapi-runtime/include" \
    "$REPO_ROOT/pandapi-runtime/src" \
    "$REPO_ROOT/pandapi-runtime/tests" \
    -type f \
    \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \) \
    -print0
)

if [ "${#files[@]}" -eq 0 ]; then
  echo "check-format-owned.sh: no owned C++ files found"
  exit 0
fi

if "$CLANG_FORMAT" --help 2>&1 | grep -q -- '--dry-run'; then
  "$CLANG_FORMAT" --dry-run --Werror "${files[@]}"
else
  failed=0
  for file in "${files[@]}"; do
    if ! "$CLANG_FORMAT" "$file" | diff -u "$file" - >/dev/null; then
      echo "check-format-owned.sh: format drift: ${file#"$REPO_ROOT"/}" >&2
      failed=1
    fi
  done
  if [ "$failed" -ne 0 ]; then
    exit 1
  fi
fi

echo "check-format-owned.sh: OK: ${#files[@]} files"
