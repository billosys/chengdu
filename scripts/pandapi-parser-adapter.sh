#!/usr/bin/env bash
# chengdu-owned managed-process adapter for the inherited pandaPIparser binary.
set -euo pipefail

SELF_PATH="${BASH_SOURCE[0]}"
SELF_DIR="$(cd "$(dirname "$SELF_PATH")" && pwd)"
INVOKED_NAME="$(basename "$0")"
LEGACY_BIN="$SELF_DIR/pandaPIparser.legacy"

if [ ! -x "$LEGACY_BIN" ]; then
  echo "pandapi-parser: internal_error: missing legacy parser binary: $LEGACY_BIN" >&2
  exit 60
fi

status_exit_code() {
  case "$1" in
    ok) echo 0 ;;
    cli_usage_error) echo 10 ;;
    input_unavailable) echo 20 ;;
    output_unavailable) echo 21 ;;
    input_invalid) echo 22 ;;
    legacy_surface) echo 31 ;;
    *) echo 60 ;;
  esac
}

status_class() {
  case "$1" in
    ok) echo "success" ;;
    cli_usage_error|input_unavailable|output_unavailable) echo "caller_error" ;;
    input_invalid) echo "input_model_error" ;;
    legacy_surface) echo "policy_surface_failure" ;;
    *) echo "internal_defect" ;;
  esac
}

status_disposition() {
  case "$1" in
    legacy_surface) echo "legacy" ;;
    *) echo "supported" ;;
  esac
}

status_surface() {
  case "$1" in
    legacy_surface) echo "parser_legacy_helper" ;;
    *) echo "normal_parse" ;;
  esac
}

emit_status() {
  local target="$1" status="$2" extra="${3:-}"
  if [ "$target" = "none" ]; then
    return 0
  fi

  local exit_code class disposition surface line
  exit_code="$(status_exit_code "$status")"
  class="$(status_class "$status")"
  disposition="$(status_disposition "$status")"
  surface="$(status_surface "$status")"
  line="PANDAPI_STATUS	status=$status	component=parser	surface=$surface	surface_disposition=$disposition	exit_code=$exit_code	class=$class"
  if [ -n "$extra" ]; then
    line="$line	$extra"
  fi

  if [ "$target" = "stdout" ]; then
    printf '%s\n' "$line"
  else
    printf '%s\n' "$line" >&2
  fi
}

finish() {
  local status="$1" status_target="$2" extra="${3:-}"
  emit_status "$status_target" "$status" "$extra"
  exit "$(status_exit_code "$status")"
}

usage_error() {
  local message="$1" status_target="$2"
  printf 'pandapi-parser: %s\n' "$message" >&2
  finish "cli_usage_error" "$status_target" "partial_output_policy=absent"
}

legacy_surface() {
  local message="$1" status_target="$2"
  printf 'pandapi-parser: %s\n' "$message" >&2
  finish "legacy_surface" "$status_target" "partial_output_policy=absent"
}

print_help() {
  cat <<'EOF'
Usage: pandapi-parser [COMMON] [--output OUT.htn|-] DOMAIN.hddl PROBLEM.hddl

Supported surface:
  Normal HDDL domain/problem parsing to pandaPI .htn output.

Common options:
  --output PATH|-        Write the parser artifact to PATH, or to stdout with -.
  --status[=stderr]     Emit one final PANDAPI_STATUS record on stderr.
  --status=stdout       Emit final status on stdout only when stdout is otherwise empty.
  --supervised          Suppress inherited human prose and ANSI behavior.
  --quiet               Suppress non-fatal human diagnostics.
  --verbose             Permit additional human diagnostics on stderr.
  --color=auto|always|never
  --no-color, --no-colour
  --help                Show this help.
  --version             Show version fields.
  --provenance          Show provenance fields.

Compatibility:
  pandaPIparser remains executable for 0.3.0. Inherited helper modes are not
  supported managed-process surfaces through pandapi-parser.
EOF
}

print_version() {
  printf 'canonical_command=pandapi-parser\n'
  printf 'invoked_command=%s\n' "$INVOKED_NAME"
  printf 'component=parser\n'
  printf 'chengdu_version=0.3.0\n'
  printf 'managed_process_contract=0.3.0\n'
  printf 'upstream_project=pandaPIparser\n'
  printf 'source_prefix=pandaPI/pandaPIparser\n'
}

print_provenance() {
  print_version
  printf 'legacy_binary=pandaPIparser.legacy\n'
  printf 'license=LICENSE\n'
  printf 'notice=NOTICE\n'
  printf 'third_party_licenses=THIRD-PARTY-LICENSES\n'
  if [ -f "$SELF_DIR/provenance.txt" ]; then
    awk '
      $0=="component=pandaPIparser"{in_block=1}
      in_block && /^chengdu_commit=/ {print}
      in_block && /^upstream_sha=/ {print}
      in_block && /^import_commit=/ {print}
      in_block && /^compiler=/ {print}
      in_block && /^---$/ {exit}
    ' "$SELF_DIR/provenance.txt"
  fi
}

is_legacy_surface_option() {
  case "$1" in
    -c|--panda-converter|-p|--properties|-v|--verify|-V|--vverify|-W|--vvverify|\
    -l|--lenient|-o|--verify-no-order|-E|--verification-encoding|\
    -S|--shop|-1|--shop1|-H|--hpdl|-R|--hppdl|-h|--hddl|-P|--processed-hddl|\
    -i|--internal-hddl|--no-domain-constants|--remove-method-preconditions|\
    -m|--goal-action|-g|--encode-disjunctive-preconditions-in-htn|-D|\
    --keep-conditional-effects|-k|--linear-conditional-effect|-L|\
    --no-split-parameters|-s|--debug|-d)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

managed_mode=0
if [ "$INVOKED_NAME" = "pandapi-parser" ]; then
  managed_mode=1
fi

for arg in "$@"; do
  case "$arg" in
    --output|--output=*|--status|--status=*|--supervised|--quiet|--verbose|\
    --color|--color=*|--no-color|--no-colour|--help|--version|--provenance)
      managed_mode=1
      ;;
  esac
done

if [ "$managed_mode" -eq 0 ]; then
  exec "$LEGACY_BIN" "$@"
fi

status_target="none"
output_path=""
output_given=0
supervised=0
quiet=0
verbose=0
color_mode="auto"
color_disabled=0
info_command="none"
positional_output_alias=0
operands=()

set_info_command() {
  local command="$1"
  if [ "$info_command" != "none" ]; then
    usage_error "only one informational command may be selected" "$status_target"
  fi
  info_command="$command"
}

while [ "$#" -gt 0 ]; do
  case "$1" in
    --help)
      set_info_command "help"
      shift
      ;;
    --version)
      set_info_command "version"
      shift
      ;;
    --provenance)
      set_info_command "provenance"
      shift
      ;;
    --status)
      status_target="stderr"
      shift
      ;;
    --status=stderr)
      status_target="stderr"
      shift
      ;;
    --status=stdout)
      status_target="stdout"
      shift
      ;;
    --status=*)
      usage_error "invalid --status value: ${1#*=}" "$status_target"
      ;;
    --output)
      if [ "$output_given" -eq 1 ]; then
        usage_error "--output specified more than once" "$status_target"
      fi
      output_path="${2:-}"
      if [ -z "$output_path" ]; then
        usage_error "--output requires a path or -" "$status_target"
      fi
      output_given=1
      shift 2
      ;;
    --output=*)
      if [ "$output_given" -eq 1 ]; then
        usage_error "--output specified more than once" "$status_target"
      fi
      output_path="${1#*=}"
      if [ -z "$output_path" ]; then
        usage_error "--output requires a path or -" "$status_target"
      fi
      output_given=1
      shift
      ;;
    --supervised)
      supervised=1
      shift
      ;;
    --quiet)
      quiet=1
      shift
      ;;
    --verbose)
      verbose=1
      shift
      ;;
    --color)
      color_mode="${2:-}"
      if [ -z "$color_mode" ]; then
        usage_error "--color requires auto, always, or never" "$status_target"
      fi
      shift 2
      ;;
    --color=*)
      color_mode="${1#*=}"
      shift
      ;;
    --no-color|--no-colour)
      color_disabled=1
      color_mode="never"
      shift
      ;;
    --)
      shift
      while [ "$#" -gt 0 ]; do
        operands+=("$1")
        shift
      done
      ;;
    -*)
      if is_legacy_surface_option "$1"; then
        legacy_surface "legacy parser helper/output surface is not supported by the managed parser CLI" "$status_target"
      fi
      usage_error "unknown option: $1" "$status_target"
      ;;
    *)
      operands+=("$1")
      shift
      ;;
  esac
done

case "$color_mode" in
  auto|always|never) ;;
  *)
    usage_error "invalid --color value: $color_mode" "$status_target"
    ;;
esac

if [ "$info_command" != "none" ]; then
  if [ "${#operands[@]}" -ne 0 ] || [ "$output_given" -ne 0 ]; then
    usage_error "informational commands do not accept input or output operands" "$status_target"
  fi
  if [ "$status_target" = "stdout" ]; then
    usage_error "--status=stdout conflicts with informational stdout" "$status_target"
  fi
  case "$info_command" in
    help) print_help ;;
    version) print_version ;;
    provenance) print_provenance ;;
  esac
  finish "ok" "$status_target"
fi

if [ "${#operands[@]}" -lt 2 ]; then
  usage_error "DOMAIN.hddl and PROBLEM.hddl are required" "$status_target"
fi

if [ "$output_given" -eq 1 ] && [ "${#operands[@]}" -gt 2 ]; then
  usage_error "--output conflicts with positional output compatibility" "$status_target"
fi

if [ "$output_given" -eq 0 ]; then
  if [ "${#operands[@]}" -eq 3 ]; then
    positional_output_alias=1
    output_path="${operands[2]}"
  elif [ "${#operands[@]}" -eq 2 ]; then
    output_path="-"
  else
    usage_error "too many positional arguments" "$status_target"
  fi
else
  if [ "${#operands[@]}" -ne 2 ]; then
    usage_error "exactly DOMAIN.hddl and PROBLEM.hddl are required with --output" "$status_target"
  fi
fi

if [ "$status_target" = "stdout" ] && [ "$output_path" = "-" ]; then
  usage_error "--status=stdout conflicts with stdout artifact output" "$status_target"
fi

if [ "$color_mode" = "always" ] && [ "$color_disabled" -eq 0 ]; then
  if [ "$supervised" -eq 1 ] || [ "$status_target" != "none" ] || [ "$output_path" = "-" ]; then
    usage_error "--color=always conflicts with supervised or machine-owned streams" "$status_target"
  fi
fi

if [ "${NO_COLOR:-}" != "" ]; then
  color_disabled=1
  color_mode="never"
fi

domain="${operands[0]}"
problem="${operands[1]}"

if [ ! -r "$domain" ]; then
  [ "$quiet" -eq 1 ] || printf 'pandapi-parser: cannot read domain: %s\n' "$domain" >&2
  finish "input_unavailable" "$status_target" "path_role=domain	operation=open	partial_output_policy=absent"
fi

if [ ! -r "$problem" ]; then
  [ "$quiet" -eq 1 ] || printf 'pandapi-parser: cannot read problem: %s\n' "$problem" >&2
  finish "input_unavailable" "$status_target" "path_role=problem	operation=open	partial_output_policy=absent"
fi

check_missing_includes() {
  local source="$1" include_name include_path
  while IFS= read -r include_name; do
    [ -n "$include_name" ] || continue
    include_path="$include_name.hddl"
    if [ ! -r "$include_path" ]; then
      [ "$quiet" -eq 1 ] || printf 'pandapi-parser: cannot read include: %s\n' "$include_path" >&2
      finish "input_unavailable" "$status_target" "path_role=include	operation=open	include_stack=$include_name	partial_output_policy=absent"
    fi
  done < <(sed -n 's/.*(:include[[:space:]][[:space:]]*\([^()[:space:]]*\)[[:space:]]*).*/\1/p' "$source")
}

check_missing_includes "$domain"
check_missing_includes "$problem"

if [ "$output_path" != "-" ]; then
  output_parent="$(dirname "$output_path")"
  if [ ! -d "$output_parent" ] || [ -d "$output_path" ]; then
    [ "$quiet" -eq 1 ] || printf 'pandapi-parser: cannot write output: %s\n' "$output_path" >&2
    finish "output_unavailable" "$status_target" "path_role=output	operation=open	partial_output_policy=absent"
  fi
  probe="$output_parent/.pandapi-parser-write-test.$$"
  if ! ( : > "$probe" ) 2>/dev/null; then
    rm -f "$probe" 2>/dev/null || true
    [ "$quiet" -eq 1 ] || printf 'pandapi-parser: cannot write output: %s\n' "$output_path" >&2
    finish "output_unavailable" "$status_target" "path_role=output	operation=open	partial_output_policy=absent"
  fi
  rm -f "$probe"
fi

work_dir="$(mktemp -d)"
cleanup() {
  rm -rf "$work_dir"
}
trap cleanup EXIT

legacy_out="$work_dir/parser.htn"
legacy_stdout="$work_dir/stdout.txt"
legacy_stderr="$work_dir/stderr.txt"

set +e
"$LEGACY_BIN" -C "$domain" "$problem" "$legacy_out" >"$legacy_stdout" 2>"$legacy_stderr"
legacy_rc=$?
set -e

if [ "$legacy_rc" -ne 0 ]; then
  if [ "$output_path" != "-" ]; then
    rm -f "$output_path" 2>/dev/null || true
  fi
  if grep -q "I can't open" "$legacy_stdout" "$legacy_stderr" 2>/dev/null; then
    [ "$quiet" -eq 1 ] || printf 'pandapi-parser: parser input unavailable\n' >&2
    finish "input_unavailable" "$status_target" "partial_output_policy=absent"
  fi
  if [ "$verbose" -eq 1 ] && [ "$supervised" -eq 0 ]; then
    cat "$legacy_stderr" >&2
  else
    [ "$quiet" -eq 1 ] || printf 'pandapi-parser: parser input invalid\n' >&2
  fi
  finish "input_invalid" "$status_target" "partial_output_policy=discarded"
fi

if [ ! -s "$legacy_out" ]; then
  [ "$quiet" -eq 1 ] || printf 'pandapi-parser: legacy parser produced no output artifact\n' >&2
  finish "internal_error" "$status_target" "partial_output_policy=unknown"
fi

if [ "$output_path" = "-" ]; then
  cat "$legacy_out"
else
  tmp_output="$output_path.tmp.$$"
  rm -f "$tmp_output"
  if ! cp "$legacy_out" "$tmp_output" 2>/dev/null || ! mv "$tmp_output" "$output_path" 2>/dev/null; then
    rm -f "$tmp_output" "$output_path" 2>/dev/null || true
    [ "$quiet" -eq 1 ] || printf 'pandapi-parser: cannot finalize output: %s\n' "$output_path" >&2
    finish "output_unavailable" "$status_target" "path_role=output	operation=finalize	partial_output_policy=discarded"
  fi
fi

if [ "$verbose" -eq 1 ] && [ "$supervised" -eq 0 ]; then
  cat "$legacy_stderr" >&2
fi

extra="partial_output_policy=complete"
if [ "$output_path" = "-" ]; then
  extra="$extra	artifact=stdout"
elif [ "$positional_output_alias" -eq 1 ]; then
  extra="$extra	artifact=file	positional_output_alias=true"
else
  extra="$extra	artifact=file"
fi
finish "ok" "$status_target" "$extra"
