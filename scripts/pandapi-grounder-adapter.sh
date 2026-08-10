#!/usr/bin/env bash
# chengdu-owned managed-process adapter for the inherited pandaPIgrounder binary.
set -euo pipefail

SELF_PATH="${BASH_SOURCE[0]}"
SELF_DIR="$(cd "$(dirname "$SELF_PATH")" && pwd)"
INVOKED_NAME="$(basename "$0")"
LEGACY_BIN="$SELF_DIR/pandaPIgrounder.legacy"

if [ ! -x "$LEGACY_BIN" ]; then
  echo "pandapi-grounder: internal_error: missing legacy grounder binary: $LEGACY_BIN" >&2
  exit 60
fi

status_exit_code() {
  case "$1" in
    ok) echo 0 ;;
    domain_no_plan) echo 2 ;;
    cli_usage_error) echo 10 ;;
    input_unavailable) echo 20 ;;
    output_unavailable) echo 21 ;;
    input_invalid) echo 22 ;;
    legacy_surface) echo 31 ;;
    experimental_surface) echo 32 ;;
    *) echo 60 ;;
  esac
}

status_class() {
  case "$1" in
    ok) echo "success" ;;
    domain_no_plan) echo "expected_domain_outcome" ;;
    cli_usage_error|input_unavailable|output_unavailable) echo "caller_error" ;;
    input_invalid) echo "input_model_error" ;;
    legacy_surface|experimental_surface) echo "policy_surface_failure" ;;
    *) echo "internal_defect" ;;
  esac
}

status_disposition() {
  case "$1" in
    legacy_surface) echo "legacy" ;;
    experimental_surface) echo "experimental" ;;
    *) echo "supported" ;;
  esac
}

status_surface() {
  case "$1" in
    legacy_surface) echo "grounder_cpddl_fam" ;;
    experimental_surface) echo "grounder_h2" ;;
    *) echo "normal_grounding" ;;
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
  line="PANDAPI_STATUS	status=$status	component=grounder	surface=$surface	surface_disposition=$disposition	exit_code=$exit_code	class=$class"
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
  printf 'pandapi-grounder: %s\n' "$message" >&2
  finish "cli_usage_error" "$status_target" "partial_output_policy=absent"
}

policy_surface() {
  local status="$1" message="$2" status_target="$3"
  printf 'pandapi-grounder: %s\n' "$message" >&2
  finish "$status" "$status_target" "partial_output_policy=absent"
}

print_help() {
  cat <<'EOF'
Usage: pandapi-grounder [COMMON] [--output OUT.sas|-] INPUT.htn

Supported surface:
  Normal parser-generated .htn grounding to pandaPI .sas output.

Common options:
  --output PATH|-        Write the grounder artifact to PATH, or to stdout with -.
  --status[=stderr]     Emit one final PANDAPI_STATUS record on stderr.
  --status=stdout       Emit final status on stdout only when stdout is otherwise empty.
  --supervised          Suppress inherited human prose, progress, statistics, and ANSI behavior.
  --quiet               Suppress non-fatal human diagnostics.
  --verbose             Permit additional human diagnostics on stderr.
  --color=auto|always|never
  --no-color, --no-colour
  --help                Show this help.
  --version             Show version fields.
  --provenance          Show provenance fields.

Compatibility:
  pandaPIgrounder remains executable for 0.3.0. H2 is experimental, and
  cpddl/FAM inference remains a legacy surface; neither is supported normal
  grounding behavior through pandapi-grounder.
EOF
}

print_version() {
  printf 'canonical_command=pandapi-grounder\n'
  printf 'invoked_command=%s\n' "$INVOKED_NAME"
  printf 'component=grounder\n'
  printf 'chengdu_version=0.3.0\n'
  printf 'managed_process_contract=0.3.0\n'
  printf 'upstream_project=pandaPIgrounder\n'
  printf 'source_prefix=pandaPI/pandaPIgrounder\n'
}

print_provenance() {
  print_version
  printf 'legacy_binary=pandaPIgrounder.legacy\n'
  printf 'license=LICENSE\n'
  printf 'notice=NOTICE\n'
  printf 'third_party_licenses=THIRD-PARTY-LICENSES\n'
  printf 'dependency_surface_h2=experimental_surface\n'
  printf 'dependency_surface_cpddl_fam=legacy_surface\n'
  if [ -f "$SELF_DIR/provenance.txt" ]; then
    awk '
      $0=="component=pandaPIgrounder"{in_block=1}
      in_block && /^chengdu_commit=/ {print}
      in_block && /^upstream_sha=/ {print}
      in_block && /^import_commit=/ {print}
      in_block && /^compiler=/ {print}
      in_block && /^---$/ {exit}
    ' "$SELF_DIR/provenance.txt"
  fi
}

managed_mode=0
if [ "$INVOKED_NAME" = "pandapi-grounder" ]; then
  managed_mode=1
fi

for arg in "$@"; do
  case "$arg" in
    --output|--output=*|--status|--status=*|--supervised|--quiet|--verbose|\
    --color|--color=*|--no-color|--no-colour|--help|--version|--provenance|\
    --h2|-2|--invariants|-i|--plan|--plan=*|\
    --output-domain|-O|--sasplus|-s|--hddl|-H|--no-output|-g)
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
surface_fence="none"
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
    --h2|-2)
      surface_fence="experimental_surface"
      shift
      ;;
    --invariants|-i)
      surface_fence="legacy_surface"
      shift
      ;;
    --plan)
      surface_fence="legacy_surface"
      if [ -z "${2:-}" ]; then
        usage_error "--plan requires a path" "$status_target"
      fi
      shift 2
      ;;
    --plan=*)
      surface_fence="legacy_surface"
      shift
      ;;
    --output-domain|-O|--sasplus|-s|--hddl|-H|--no-output|-g)
      surface_fence="legacy_surface"
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

if [ "$surface_fence" = "experimental_surface" ]; then
  policy_surface "experimental_surface" "H2 is experimental and is not supported normal grounding behavior" "$status_target"
fi
if [ "$surface_fence" = "legacy_surface" ]; then
  policy_surface "legacy_surface" "legacy cpddl/FAM or non-normal grounder surface is outside the managed grounder CLI" "$status_target"
fi

if [ "${#operands[@]}" -lt 1 ]; then
  usage_error "INPUT.htn is required" "$status_target"
fi

if [ "$output_given" -eq 1 ] && [ "${#operands[@]}" -gt 1 ]; then
  usage_error "--output conflicts with positional output compatibility" "$status_target"
fi

if [ "$output_given" -eq 0 ]; then
  if [ "${#operands[@]}" -eq 2 ]; then
    positional_output_alias=1
    output_path="${operands[1]}"
  elif [ "${#operands[@]}" -eq 1 ]; then
    output_path="-"
  else
    usage_error "too many positional arguments" "$status_target"
  fi
else
  if [ "${#operands[@]}" -ne 1 ]; then
    usage_error "exactly INPUT.htn is required with --output" "$status_target"
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

input_path="${operands[0]}"

if [ ! -r "$input_path" ]; then
  [ "$quiet" -eq 1 ] || printf 'pandapi-grounder: cannot read input: %s\n' "$input_path" >&2
  finish "input_unavailable" "$status_target" "path_role=htn	operation=open	partial_output_policy=absent"
fi

if [ "$output_path" != "-" ]; then
  output_parent="$(dirname "$output_path")"
  if [ ! -d "$output_parent" ] || [ -d "$output_path" ]; then
    [ "$quiet" -eq 1 ] || printf 'pandapi-grounder: cannot write output: %s\n' "$output_path" >&2
    finish "output_unavailable" "$status_target" "path_role=output	operation=open	partial_output_policy=absent"
  fi
  probe="$output_parent/.pandapi-grounder-write-test.$$"
  if ! ( : > "$probe" ) 2>/dev/null; then
    rm -f "$probe" 2>/dev/null || true
    [ "$quiet" -eq 1 ] || printf 'pandapi-grounder: cannot write output: %s\n' "$output_path" >&2
    finish "output_unavailable" "$status_target" "path_role=output	operation=open	partial_output_policy=absent"
  fi
  rm -f "$probe"
fi

work_dir="$(mktemp -d)"
# shellcheck disable=SC2329 # Invoked by the EXIT trap below.
cleanup() {
  rm -rf "$work_dir"
}
trap cleanup EXIT

legacy_out="$work_dir/grounder.sas"
legacy_stdout="$work_dir/stdout.txt"
legacy_stderr="$work_dir/stderr.txt"

set +e
"$LEGACY_BIN" --quiet "$input_path" "$legacy_out" >"$legacy_stdout" 2>"$legacy_stderr"
legacy_rc=$?
set -e

if [ "$legacy_rc" -ne 0 ]; then
  if grep -q "Unable to open input file" "$legacy_stderr" 2>/dev/null; then
    [ "$quiet" -eq 1 ] || printf 'pandapi-grounder: input unavailable\n' >&2
    finish "input_unavailable" "$status_target" "path_role=htn	operation=open	partial_output_policy=absent"
  fi
  if [ "$verbose" -eq 1 ] && [ "$supervised" -eq 0 ]; then
    cat "$legacy_stderr" >&2
  else
    [ "$quiet" -eq 1 ] || printf 'pandapi-grounder: input invalid\n' >&2
  fi
  finish "input_invalid" "$status_target" "partial_output_policy=discarded"
fi

if [ ! -s "$legacy_out" ]; then
  [ "$quiet" -eq 1 ] || printf 'pandapi-grounder: legacy grounder produced no output artifact\n' >&2
  finish "internal_error" "$status_target" "partial_output_policy=unknown"
fi

if [ "$output_path" = "-" ]; then
  cat "$legacy_out"
else
  tmp_output="$output_path.tmp.$$"
  rm -f "$tmp_output"
  if ! cp "$legacy_out" "$tmp_output" 2>/dev/null || ! mv "$tmp_output" "$output_path" 2>/dev/null; then
    rm -f "$tmp_output" "$output_path" 2>/dev/null || true
    [ "$quiet" -eq 1 ] || printf 'pandapi-grounder: cannot finalize output: %s\n' "$output_path" >&2
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
