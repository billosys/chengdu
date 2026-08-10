#!/usr/bin/env bash
# Run Arc05 black-box process fixtures against built planner binaries.
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: run-contract-fixtures.sh --baseline [--component parser|grounder|engine|pipeline|all] [--preserve]
       run-contract-fixtures.sh --contract --component parser [--case CASE] [--preserve]
       run-contract-fixtures.sh [--baseline|--contract --component parser] --list

  --baseline   run current inherited-binary baseline fixtures.
  --contract   run managed-process contract fixtures.
  --component  select a fixture component; default: all.
  --case       select one contract fixture case.
  --list       list selected fixture IDs.
  --preserve   keep the temporary work directory for debugging.
EOF
}

MODE=""
COMPONENT="all"
CASE_ID="all"
PRESERVE=0
LIST=0

while [ "$#" -gt 0 ]; do
  case "$1" in
    --baseline)
      MODE="baseline"
      shift
      ;;
    --contract)
      MODE="contract"
      shift
      ;;
    --component)
      COMPONENT="${2:-}"
      shift 2
      ;;
    --component=*)
      COMPONENT="${1#*=}"
      shift
      ;;
    --case)
      CASE_ID="${2:-}"
      shift 2
      ;;
    --case=*)
      CASE_ID="${1#*=}"
      shift
      ;;
    --list)
      LIST=1
      shift
      ;;
    --preserve)
      PRESERVE=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "run-contract-fixtures.sh: unknown argument: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

case "$COMPONENT" in
  all|parser|grounder|engine|pipeline) ;;
  *)
    echo "run-contract-fixtures.sh: invalid --component: $COMPONENT" >&2
    usage >&2
    exit 1
    ;;
esac

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib-platform.sh
. "$SCRIPT_DIR/lib-platform.sh"

PLATFORM="$(detect_platform)"
DIST_DIR="$REPO_ROOT/dist/$PLATFORM"
FIXTURES_DIR="$REPO_ROOT/fixtures"

PC="$DIST_DIR/pandapi-parser"
P="$DIST_DIR/pandaPIparser"
G="$DIST_DIR/pandaPIgrounder"
E="$DIST_DIR/pandaPIengine"

list_baseline_fixtures() {
  cat <<'EOF'
parser parser-success-minimal
parser parser-missing-file
parser parser-broken-syntax
parser parser-broken-reference
grounder grounder-success-minimal
grounder grounder-missing-input
engine engine-solved-minimal
engine engine-unsolvable
pipeline pipeline-minimal
EOF
}

list_contract_fixtures() {
  cat <<'EOF'
parser parser-canonical-file-success
parser parser-canonical-stdout-success
parser parser-inherited-compat-success
parser parser-info-commands
parser parser-cli-usage-errors
parser parser-input-unavailable
parser parser-input-invalid
parser parser-output-unavailable
parser parser-final-status-fields
parser parser-status-stream-legality
parser parser-color-policy
parser parser-legacy-surface-fencing
EOF
}

if [ "$LIST" -eq 1 ]; then
  if [ "$MODE" = "contract" ]; then
    list_contract_fixtures
  else
    list_baseline_fixtures
  fi
  exit 0
fi

if [ "$MODE" != "baseline" ] && [ "$MODE" != "contract" ]; then
  echo "run-contract-fixtures.sh: --baseline or --contract is required unless --list is used" >&2
  usage >&2
  exit 1
fi

if [ "$MODE" = "baseline" ]; then
  for bin in "$P" "$G" "$E"; do
    if [ ! -x "$bin" ]; then
      echo "run-contract-fixtures.sh: missing binary: $bin; run ./scripts/build-all.sh first" >&2
      exit 1
    fi
  done
fi

if [ "$MODE" = "contract" ]; then
  if [ "$COMPONENT" != "parser" ]; then
    echo "run-contract-fixtures.sh: only parser contract fixtures exist in this slice" >&2
    exit 1
  fi
  for bin in "$PC" "$P"; do
    if [ ! -x "$bin" ]; then
      echo "run-contract-fixtures.sh: missing binary: $bin; run ./scripts/build-parser.sh first" >&2
      exit 1
    fi
  done
fi

WORK="$(mktemp -d)"
cleanup() {
  if [ "$PRESERVE" -eq 1 ]; then
    echo "run-contract-fixtures.sh: preserved work directory: $WORK"
  else
    rm -rf "$WORK"
  fi
}
trap cleanup EXIT

PASS_COUNT=0
FAIL_COUNT=0
OBS_ID=""
OBS_OUT=""
OBS_ERR=""
OBS_RC=0
ENGINE_TIMEOUT_SECS=60

selected() {
  local component="$1"
  [ "$COMPONENT" = "all" ] || [ "$COMPONENT" = "$component" ]
}

pass() {
  echo "run-contract-fixtures.sh: PASS: $1"
  PASS_COUNT=$((PASS_COUNT + 1))
}

fail() {
  echo "run-contract-fixtures.sh: FAIL: $1" >&2
  FAIL_COUNT=$((FAIL_COUNT + 1))
}

strip_ansi_file() {
  sed $'s/\x1b\\[[0-9;]*m//g' "$1"
}

run_capture() {
  OBS_ID="$1"
  shift
  OBS_OUT="$WORK/${OBS_ID}.stdout"
  OBS_ERR="$WORK/${OBS_ID}.stderr"
  set +e
  "$@" >"$OBS_OUT" 2>"$OBS_ERR"
  OBS_RC=$?
  set -e
}

run_capture_timeout() {
  OBS_ID="$1"
  local timeout="$2"
  shift 2
  OBS_OUT="$WORK/${OBS_ID}.stdout"
  OBS_ERR="$WORK/${OBS_ID}.stderr"
  "$@" >"$OBS_OUT" 2>"$OBS_ERR" &
  local pid=$!
  ( sleep "$timeout"; kill -9 "$pid" 2>/dev/null ) &
  local watcher=$!
  set +e
  wait "$pid"
  OBS_RC=$?
  kill "$watcher" 2>/dev/null
  wait "$watcher" 2>/dev/null
  set -e
  if [ "$OBS_RC" -eq 137 ]; then
    OBS_RC=124
    printf 'harness timeout after %ss\n' "$timeout" >>"$OBS_ERR"
  fi
}

expect_exit() {
  local id="$1" expected="$2"
  if [ "$OBS_RC" -eq "$expected" ]; then
    pass "$id exit=$expected"
  else
    fail "$id expected exit $expected, got $OBS_RC"
  fi
}

expect_stdout_contains() {
  local id="$1" expected="$2"
  if strip_ansi_file "$OBS_OUT" | grep -q -- "$expected"; then
    pass "$id stdout contains $expected"
  else
    fail "$id stdout missing $expected"
  fi
}

expect_stderr_contains() {
  local id="$1" expected="$2"
  if strip_ansi_file "$OBS_ERR" | grep -q -- "$expected"; then
    pass "$id stderr contains $expected"
  else
    fail "$id stderr missing $expected"
  fi
}

expect_stderr_not_contains() {
  local id="$1" unexpected="$2"
  if strip_ansi_file "$OBS_ERR" | grep -q -- "$unexpected"; then
    fail "$id stderr unexpectedly contains $unexpected"
  else
    pass "$id stderr does not contain $unexpected"
  fi
}

expect_stderr_empty() {
  local id="$1"
  if [ ! -s "$OBS_ERR" ]; then
    pass "$id stderr empty"
  else
    fail "$id expected empty stderr"
  fi
}

expect_artifact_complete() {
  local id="$1" artifact="$2"
  if [ -s "$artifact" ]; then
    pass "$id artifact complete: ${artifact#"$WORK"/}"
  else
    fail "$id expected complete artifact: ${artifact#"$WORK"/}"
  fi
}

expect_artifact_absent() {
  local id="$1" artifact="$2"
  if [ ! -e "$artifact" ]; then
    pass "$id artifact absent: ${artifact#"$WORK"/}"
  else
    fail "$id expected absent artifact: ${artifact#"$WORK"/}"
  fi
}

expect_artifact_retained() {
  local id="$1" artifact="$2"
  if [ -s "$artifact" ]; then
    pass "$id artifact retained after failure: ${artifact#"$WORK"/}"
  else
    fail "$id expected retained artifact after failure: ${artifact#"$WORK"/}"
  fi
}

expect_final_status_absent() {
  local id="$1"
  if grep -q 'PANDAPI_STATUS' "$OBS_OUT" "$OBS_ERR"; then
    fail "$id baseline final_status expected absent"
  else
    pass "$id baseline final_status absent"
  fi
}

contains_ansi_file() {
  LC_ALL=C grep -q "$(printf '\033')" "$1"
}

expect_no_ansi() {
  local id="$1" file="$2" label="$3"
  if contains_ansi_file "$file"; then
    fail "$id $label contains ANSI"
  else
    pass "$id $label ANSI-free"
  fi
}

expect_stdout_empty() {
  local id="$1"
  if [ ! -s "$OBS_OUT" ]; then
    pass "$id stdout empty"
  else
    fail "$id expected empty stdout"
  fi
}

expect_stdout_not_contains() {
  local id="$1" unexpected="$2"
  if strip_ansi_file "$OBS_OUT" | grep -q -- "$unexpected"; then
    fail "$id stdout unexpectedly contains $unexpected"
  else
    pass "$id stdout does not contain $unexpected"
  fi
}

status_lines() {
  local stream="$1"
  if [ "$stream" = "stdout" ]; then
    grep '^PANDAPI_STATUS	' "$OBS_OUT" 2>/dev/null || true
  else
    grep '^PANDAPI_STATUS	' "$OBS_ERR" 2>/dev/null || true
  fi
}

status_field() {
  local line="$1" field="$2"
  printf '%s\n' "$line" | tr '\t' '\n' | sed -n "s/^$field=//p"
}

expect_status_field() {
  local id="$1" line="$2" field="$3" expected="$4"
  local actual
  actual="$(status_field "$line" "$field")"
  if [ "$actual" = "$expected" ]; then
    pass "$id status $field=$expected"
  else
    fail "$id expected status $field=$expected, got ${actual:-<missing>}"
  fi
}

expect_final_status() {
  local id="$1" stream="$2" status="$3" exit_code="$4" class="$5" disposition="${6:-supported}"
  local lines count line
  lines="$(status_lines "$stream")"
  count="$(printf '%s\n' "$lines" | sed '/^$/d' | wc -l | tr -d ' ')"
  if [ "$count" = "1" ]; then
    pass "$id final_status exactly once on $stream"
  else
    fail "$id expected one final_status on $stream, got $count"
    return
  fi
  line="$lines"
  expect_status_field "$id" "$line" "status" "$status"
  expect_status_field "$id" "$line" "component" "parser"
  expect_status_field "$id" "$line" "surface" "$( [ "$disposition" = "legacy" ] && echo parser_legacy_helper || echo normal_parse )"
  expect_status_field "$id" "$line" "surface_disposition" "$disposition"
  expect_status_field "$id" "$line" "exit_code" "$exit_code"
  expect_status_field "$id" "$line" "class" "$class"
  if [ "$stream" = "stdout" ]; then
    expect_no_ansi "$id" "$OBS_OUT" "stdout status"
  else
    expect_no_ansi "$id" "$OBS_ERR" "stderr status"
  fi
}

expect_no_status_on_stream() {
  local id="$1" stream="$2"
  local file
  if [ "$stream" = "stdout" ]; then
    file="$OBS_OUT"
  else
    file="$OBS_ERR"
  fi
  if grep -q '^PANDAPI_STATUS	' "$file"; then
    fail "$id unexpected final_status on $stream"
  else
    pass "$id no final_status on $stream"
  fi
}

contract_case_selected() {
  local id="$1"
  [ "$CASE_ID" = "all" ] || [ "$CASE_ID" = "$id" ]
}

run_parser_contract_case() {
  local id="$1"
  shift
  if contract_case_selected "$id"; then
    "$@"
  fi
}

prepare_minimal_htn() {
  local htn="$1"
  run_capture "setup-minimal-htn" \
    "$P" -C "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$htn"
  if [ "$OBS_RC" -ne 0 ] || [ ! -s "$htn" ]; then
    fail "setup minimal htn expected parser exit 0 and complete artifact"
    return 1
  fi
  return 0
}

prepare_minimal_sas() {
  local htn="$WORK/minimal.htn"
  local sas="$1"
  prepare_minimal_htn "$htn" || return 1
  run_capture "setup-minimal-sas" "$G" "$htn" "$sas"
  if [ "$OBS_RC" -ne 0 ] || [ ! -s "$sas" ]; then
    fail "setup minimal sas expected grounder exit 0 and complete artifact"
    return 1
  fi
  return 0
}

prepare_unsolvable_sas() {
  local htn="$WORK/unsolvable.htn"
  local sas="$1"
  run_capture "setup-unsolvable-htn" \
    "$P" -C "$FIXTURES_DIR/unsolvable/domain.hddl" "$FIXTURES_DIR/unsolvable/problem.hddl" "$htn"
  if [ "$OBS_RC" -ne 0 ] || [ ! -s "$htn" ]; then
    fail "setup unsolvable htn expected parser exit 0 and complete artifact"
    return 1
  fi
  run_capture "setup-unsolvable-sas" "$G" "$htn" "$sas"
  if [ "$OBS_RC" -ne 0 ] || [ ! -s "$sas" ]; then
    fail "setup unsolvable sas expected grounder exit 0 and complete artifact"
    return 1
  fi
  return 0
}

run_parser_fixtures() {
  local id artifact

  id="parser-success-minimal"
  artifact="$WORK/parser-success-minimal.htn"
  run_capture "$id" "$P" -C "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$artifact"
  expect_exit "$id" 0
  expect_artifact_complete "$id" "$artifact"
  expect_final_status_absent "$id"

  id="parser-missing-file"
  artifact="$WORK/parser-missing-file.htn"
  run_capture "$id" "$P" -C "$WORK/missing-domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$artifact"
  expect_exit "$id" 2
  expect_stdout_contains "$id" "I can't open"
  expect_artifact_absent "$id" "$artifact"
  expect_final_status_absent "$id"

  id="parser-broken-syntax"
  artifact="$WORK/parser-broken-syntax.htn"
  run_capture "$id" "$P" -C "$FIXTURES_DIR/broken-syntax/domain.hddl" "$FIXTURES_DIR/broken-syntax/problem.hddl" "$artifact"
  expect_exit "$id" 255
  expect_stderr_contains "$id" "Parse error in file"
  expect_artifact_absent "$id" "$artifact"
  expect_final_status_absent "$id"

  id="parser-broken-reference"
  artifact="$WORK/parser-broken-reference.htn"
  run_capture "$id" "$P" -C "$FIXTURES_DIR/broken-reference/domain.hddl" "$FIXTURES_DIR/broken-reference/problem.hddl" "$artifact"
  expect_exit "$id" 255
  expect_stderr_contains "$id" "not declared"
  expect_stderr_not_contains "$id" "Parse error in file"
  expect_artifact_retained "$id" "$artifact"
  expect_final_status_absent "$id"
}

run_grounder_fixtures() {
  local id htn sas

  id="grounder-success-minimal"
  htn="$WORK/grounder-success-minimal.htn"
  sas="$WORK/grounder-success-minimal.sas"
  if prepare_minimal_htn "$htn"; then
    run_capture "$id" "$G" "$htn" "$sas"
    expect_exit "$id" 0
    expect_artifact_complete "$id" "$sas"
    expect_final_status_absent "$id"
  fi

  id="grounder-missing-input"
  sas="$WORK/grounder-missing-input.sas"
  run_capture "$id" "$G" "$WORK/missing.htn" "$sas"
  expect_exit "$id" 1
  expect_stderr_contains "$id" "Unable to open input file"
  expect_artifact_absent "$id" "$sas"
  expect_final_status_absent "$id"
}

run_engine_fixtures() {
  local id sas

  id="engine-solved-minimal"
  sas="$WORK/engine-solved-minimal.sas"
  if prepare_minimal_sas "$sas"; then
    run_capture_timeout "$id" "$ENGINE_TIMEOUT_SECS" "$E" "$sas"
    expect_exit "$id" 0
    expect_stdout_contains "$id" "^- Status: Solved$"
    expect_stderr_empty "$id"
    expect_final_status_absent "$id"
  fi

  id="engine-unsolvable"
  sas="$WORK/engine-unsolvable.sas"
  if prepare_unsolvable_sas "$sas"; then
    run_capture_timeout "$id" "$ENGINE_TIMEOUT_SECS" "$E" "$sas"
    expect_exit "$id" 2
    expect_stdout_contains "$id" "^- Status: Proven unsolvable$"
    expect_stderr_empty "$id"
    expect_final_status_absent "$id"
  fi
}

run_pipeline_fixtures() {
  local id htn sas
  id="pipeline-minimal"
  htn="$WORK/pipeline-minimal.htn"
  sas="$WORK/pipeline-minimal.sas"

  run_capture "$id-parser" "$P" -C "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$htn"
  expect_exit "$id parser" 0
  expect_artifact_complete "$id parser" "$htn"
  if [ "$OBS_RC" -ne 0 ]; then
    return
  fi

  run_capture "$id-grounder" "$G" "$htn" "$sas"
  expect_exit "$id grounder" 0
  expect_artifact_complete "$id grounder" "$sas"
  if [ "$OBS_RC" -ne 0 ]; then
    return
  fi

  run_capture_timeout "$id-engine" "$ENGINE_TIMEOUT_SECS" "$E" "$sas"
  expect_exit "$id engine" 0
  expect_stdout_contains "$id engine" "^- Status: Solved$"
  expect_final_status_absent "$id engine"
}

parser_contract_canonical_file_success() {
  local id="parser-canonical-file-success" artifact="$WORK/parser-canonical-file-success.htn"
  run_capture "$id" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id" 0
  expect_artifact_complete "$id" "$artifact"
  expect_stdout_empty "$id"
  expect_final_status "$id" "stderr" "ok" "0" "success"
}

parser_contract_canonical_stdout_success() {
  local id="parser-canonical-stdout-success" artifact="$WORK/parser-canonical-stdout-success.htn"
  run_capture "$id" "$PC" --status=stderr --output - "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id" 0
  if [ -s "$OBS_OUT" ]; then
    pass "$id stdout artifact complete"
  else
    fail "$id expected stdout artifact"
  fi
  expect_stdout_not_contains "$id" "PANDAPI_STATUS"
  expect_stdout_not_contains "$id" "pandaPIparser is configured"
  expect_artifact_absent "$id" "$artifact"
  expect_no_ansi "$id" "$OBS_OUT" "stdout artifact"
  expect_final_status "$id" "stderr" "ok" "0" "success"
}

parser_contract_inherited_compat_success() {
  local id="parser-inherited-compat-success" artifact="$WORK/parser-inherited-compat-success.htn"
  run_capture "$id" "$P" --status=stderr "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$artifact"
  expect_exit "$id" 0
  expect_artifact_complete "$id" "$artifact"
  expect_stdout_empty "$id"
  expect_stderr_not_contains "$id" "deprecation"
  expect_final_status "$id" "stderr" "ok" "0" "success"

  local legacy_artifact="$WORK/parser-inherited-legacy-positional.htn"
  run_capture "$id-legacy-positional" "$P" -C "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$legacy_artifact"
  expect_exit "$id legacy positional" 0
  expect_artifact_complete "$id legacy positional" "$legacy_artifact"
  expect_final_status_absent "$id legacy positional"
}

parser_contract_info_commands() {
  local id="parser-info-commands"
  run_capture "$id-help" "$PC" --help
  expect_exit "$id help" 0
  expect_stdout_contains "$id help" "Usage: pandapi-parser"
  expect_stdout_contains "$id help" "pandaPIparser remains executable"
  expect_stderr_empty "$id help"
  expect_no_status_on_stream "$id help" "stdout"
  expect_no_status_on_stream "$id help" "stderr"

  run_capture "$id-version" "$PC" --version
  expect_exit "$id version" 0
  expect_stdout_contains "$id version" "canonical_command=pandapi-parser"
  expect_stdout_contains "$id version" "component=parser"
  expect_stderr_empty "$id version"

  run_capture "$id-provenance" "$P" --provenance
  expect_exit "$id provenance" 0
  expect_stdout_contains "$id provenance" "canonical_command=pandapi-parser"
  expect_stdout_contains "$id provenance" "invoked_command=pandaPIparser"
  expect_stdout_contains "$id provenance" "legacy_binary=pandaPIparser.legacy"
  expect_stderr_empty "$id provenance"
}

parser_contract_cli_usage_errors() {
  local id="parser-cli-usage-errors" artifact="$WORK/parser-cli-usage-errors.htn"

  run_capture "$id-missing-args" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl"
  expect_exit "$id missing args" 10
  expect_artifact_absent "$id missing args" "$artifact"
  expect_final_status "$id missing args" "stderr" "cli_usage_error" "10" "caller_error"

  run_capture "$id-unknown-option" "$PC" --status=stderr --unknown-option "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id unknown option" 10
  expect_final_status "$id unknown option" "stderr" "cli_usage_error" "10" "caller_error"

  run_capture "$id-malformed-color" "$PC" --status=stderr --color=wat "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id malformed color" 10
  expect_final_status "$id malformed color" "stderr" "cli_usage_error" "10" "caller_error"

  run_capture "$id-too-many-positionals" "$PC" --status=stderr "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$WORK/a.htn" "$WORK/extra.htn"
  expect_exit "$id too many positionals" 10
  expect_final_status "$id too many positionals" "stderr" "cli_usage_error" "10" "caller_error"

  run_capture "$id-output-conflict" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$WORK/positional.htn"
  expect_exit "$id output conflict" 10
  expect_artifact_absent "$id output conflict" "$artifact"
  expect_final_status "$id output conflict" "stderr" "cli_usage_error" "10" "caller_error"

  run_capture "$id-status-stdout-artifact" "$PC" --status=stdout --output - "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id status stdout artifact conflict" 10
  expect_final_status "$id status stdout artifact conflict" "stdout" "cli_usage_error" "10" "caller_error"

  run_capture "$id-status-stdout-info" "$PC" --status=stdout --help
  expect_exit "$id status stdout info conflict" 10
  expect_final_status "$id status stdout info conflict" "stdout" "cli_usage_error" "10" "caller_error"
}

parser_contract_input_unavailable() {
  local id="parser-input-unavailable" artifact="$WORK/parser-input-unavailable.htn" include_domain="$WORK/parser-missing-include-domain.hddl"
  run_capture "$id-domain" "$PC" --status=stderr --output "$artifact" "$WORK/missing-domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id missing domain" 20
  expect_artifact_absent "$id missing domain" "$artifact"
  expect_stdout_empty "$id missing domain"
  expect_final_status "$id missing domain" "stderr" "input_unavailable" "20" "caller_error"

  run_capture "$id-problem" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl" "$WORK/missing-problem.hddl"
  expect_exit "$id missing problem" 20
  expect_artifact_absent "$id missing problem" "$artifact"
  expect_stdout_empty "$id missing problem"
  expect_final_status "$id missing problem" "stderr" "input_unavailable" "20" "caller_error"

  awk 'NR==3{print "  (:include missing_include)"} {print}' "$FIXTURES_DIR/minimal/domain.hddl" > "$include_domain"
  run_capture "$id-include" "$PC" --status=stderr --output "$artifact" "$include_domain" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id missing include" 20
  expect_artifact_absent "$id missing include" "$artifact"
  expect_stdout_empty "$id missing include"
  expect_final_status "$id missing include" "stderr" "input_unavailable" "20" "caller_error"
}

parser_contract_input_invalid() {
  local id="parser-input-invalid" artifact="$WORK/parser-input-invalid.htn"
  run_capture "$id-syntax" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/broken-syntax/domain.hddl" "$FIXTURES_DIR/broken-syntax/problem.hddl"
  expect_exit "$id syntax" 22
  expect_artifact_absent "$id syntax" "$artifact"
  expect_stdout_empty "$id syntax"
  expect_final_status "$id syntax" "stderr" "input_invalid" "22" "input_model_error"

  run_capture "$id-reference" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/broken-reference/domain.hddl" "$FIXTURES_DIR/broken-reference/problem.hddl"
  expect_exit "$id broken reference" 22
  expect_artifact_absent "$id broken reference" "$artifact"
  expect_stdout_empty "$id broken reference"
  expect_final_status "$id broken reference" "stderr" "input_invalid" "22" "input_model_error"
}

parser_contract_output_unavailable() {
  local id="parser-output-unavailable"
  run_capture "$id-missing-parent" "$PC" --status=stderr --output "$WORK/no-such-parent/out.htn" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id missing parent" 21
  expect_stdout_empty "$id missing parent"
  expect_final_status "$id missing parent" "stderr" "output_unavailable" "21" "caller_error"

  mkdir -p "$WORK/output-dir"
  run_capture "$id-output-directory" "$PC" --status=stderr --output "$WORK/output-dir" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id output directory" 21
  expect_stdout_empty "$id output directory"
  expect_final_status "$id output directory" "stderr" "output_unavailable" "21" "caller_error"
}

parser_contract_final_status_fields() {
  local id="parser-final-status-fields" artifact="$WORK/parser-final-status-fields.htn"
  run_capture "$id" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id" 0
  expect_final_status "$id" "stderr" "ok" "0" "success"
  local line
  line="$(status_lines stderr)"
  expect_status_field "$id" "$line" "partial_output_policy" "complete"
  expect_status_field "$id" "$line" "artifact" "file"
}

parser_contract_status_stream_legality() {
  local id="parser-status-stream-legality" artifact="$WORK/parser-status-stream-legality.htn"
  run_capture "$id-file-stderr" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id file stderr" 0
  expect_final_status "$id file stderr" "stderr" "ok" "0" "success"

  run_capture "$id-stdout-artifact-stderr" "$PC" --status=stderr --output - "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id stdout artifact stderr" 0
  expect_final_status "$id stdout artifact stderr" "stderr" "ok" "0" "success"

  run_capture "$id-file-stdout" "$PC" --status=stdout --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id file stdout" 0
  expect_final_status "$id file stdout" "stdout" "ok" "0" "success"
  expect_no_status_on_stream "$id file stdout" "stderr"

  run_capture "$id-illegal-stdout" "$PC" --status=stdout --output - "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id illegal stdout status" 10
  expect_final_status "$id illegal stdout status" "stdout" "cli_usage_error" "10" "caller_error"
}

parser_contract_color_policy() {
  local id="parser-color-policy" artifact="$WORK/parser-color-policy.htn"
  run_capture "$id-no-color" "$PC" --status=stderr --no-color --output "$artifact" "$FIXTURES_DIR/broken-syntax/domain.hddl" "$FIXTURES_DIR/broken-syntax/problem.hddl"
  expect_exit "$id no-color" 22
  expect_no_ansi "$id no-color" "$OBS_ERR" "stderr"
  expect_final_status "$id no-color" "stderr" "input_invalid" "22" "input_model_error"

  run_capture "$id-no-colour" "$PC" --status=stderr --no-colour --output "$artifact" "$FIXTURES_DIR/broken-syntax/domain.hddl" "$FIXTURES_DIR/broken-syntax/problem.hddl"
  expect_exit "$id no-colour" 22
  expect_no_ansi "$id no-colour" "$OBS_ERR" "stderr"
  expect_final_status "$id no-colour" "stderr" "input_invalid" "22" "input_model_error"

  NO_COLOR=1 run_capture "$id-no-color-env" "$PC" --status=stderr --output "$artifact" "$FIXTURES_DIR/broken-syntax/domain.hddl" "$FIXTURES_DIR/broken-syntax/problem.hddl"
  expect_exit "$id NO_COLOR" 22
  expect_no_ansi "$id NO_COLOR" "$OBS_ERR" "stderr"
  expect_final_status "$id NO_COLOR" "stderr" "input_invalid" "22" "input_model_error"

  run_capture "$id-color-always-status" "$PC" --status=stderr --color=always --output "$artifact" "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl"
  expect_exit "$id color always status" 10
  expect_final_status "$id color always status" "stderr" "cli_usage_error" "10" "caller_error"
}

parser_contract_legacy_surface_fencing() {
  local id="parser-legacy-surface-fencing"
  run_capture "$id-verifier" "$PC" --status=stderr --verify "$FIXTURES_DIR/minimal/domain.hddl" "$FIXTURES_DIR/minimal/problem.hddl" "$WORK/plan.txt"
  expect_exit "$id parser verifier" 31
  expect_final_status "$id parser verifier" "stderr" "legacy_surface" "31" "policy_surface_failure" "legacy"

  run_capture "$id-output-helper" "$PC" --status=stderr --panda-converter "$WORK/plan.raw" "$WORK/plan.txt"
  expect_exit "$id output helper" 31
  expect_final_status "$id output helper" "stderr" "legacy_surface" "31" "policy_surface_failure" "legacy"
}

run_parser_contract_fixtures() {
  run_parser_contract_case "parser-canonical-file-success" parser_contract_canonical_file_success
  run_parser_contract_case "parser-canonical-stdout-success" parser_contract_canonical_stdout_success
  run_parser_contract_case "parser-inherited-compat-success" parser_contract_inherited_compat_success
  run_parser_contract_case "parser-info-commands" parser_contract_info_commands
  run_parser_contract_case "parser-cli-usage-errors" parser_contract_cli_usage_errors
  run_parser_contract_case "parser-input-unavailable" parser_contract_input_unavailable
  run_parser_contract_case "parser-input-invalid" parser_contract_input_invalid
  run_parser_contract_case "parser-output-unavailable" parser_contract_output_unavailable
  run_parser_contract_case "parser-final-status-fields" parser_contract_final_status_fields
  run_parser_contract_case "parser-status-stream-legality" parser_contract_status_stream_legality
  run_parser_contract_case "parser-color-policy" parser_contract_color_policy
  run_parser_contract_case "parser-legacy-surface-fencing" parser_contract_legacy_surface_fencing
}

if [ "$MODE" = "contract" ]; then
  echo "run-contract-fixtures.sh: platform=$PLATFORM mode=contract component=$COMPONENT case=$CASE_ID"
  run_parser_contract_fixtures
  echo "run-contract-fixtures.sh: SUMMARY: $PASS_COUNT passed, $FAIL_COUNT failed (contract, $COMPONENT, $PLATFORM)"
  if [ "$FAIL_COUNT" -ne 0 ]; then
    exit 1
  fi
  exit 0
fi

echo "run-contract-fixtures.sh: platform=$PLATFORM mode=baseline component=$COMPONENT"

if selected parser; then
  run_parser_fixtures
fi
if selected grounder; then
  run_grounder_fixtures
fi
if selected engine; then
  run_engine_fixtures
fi
if selected pipeline; then
  run_pipeline_fixtures
fi

echo "run-contract-fixtures.sh: SUMMARY: $PASS_COUNT passed, $FAIL_COUNT failed (baseline, $COMPONENT, $PLATFORM)"
if [ "$FAIL_COUNT" -ne 0 ]; then
  exit 1
fi
