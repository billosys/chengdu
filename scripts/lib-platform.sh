# shellcheck shell=bash
# Shared helpers for the chengdu build/smoke scripts. Sourced, not
# executed — no shebang, no set -e (the sourcing script owns that).
# bash 3.2 compatible: no associative arrays, no mapfile.

detect_platform() {
  local os arch
  os="$(uname -s)"
  arch="$(uname -m)"
  case "$os-$arch" in
    Linux-x86_64)
      echo "linux-x86_64"
      ;;
    Darwin-arm64)
      echo "macos-arm64"
      ;;
    *)
      echo "chengdu: unsupported platform: $os-$arch (support matrix: linux-x86_64, macos-arm64)" >&2
      return 1
      ;;
  esac
}

# resolve_compiler_id <compiler-binary> — first line of --version, for
# provenance. Falls back to the bare binary name if --version fails.
resolve_compiler_id() {
  local bin="$1"
  if command -v "$bin" >/dev/null 2>&1; then
    "$bin" --version 2>&1 | head -1
  else
    echo "$bin (not found on PATH)"
  fi
}

# append_provenance <dist_dir> <component> <sha> <patches_csv> <compiler_id>
append_provenance() {
  local dist_dir="$1" component="$2" sha="$3" patches="$4" compiler="$5"
  mkdir -p "$dist_dir"
  {
    echo "component=$component"
    echo "sha=$sha"
    echo "patches=$patches"
    echo "compiler=$compiler"
    echo "---"
  } >> "$dist_dir/provenance.txt"
}
