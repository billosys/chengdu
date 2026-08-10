#!/usr/bin/env bash
# Run Arc05 black-box baseline process fixtures against built inherited binaries.
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: run-contract-fixtures.sh --baseline [--component parser|grounder|engine|pipeline|all] [--preserve]
       run-contract-fixtures.sh --list

  --baseline   run current inherited-binary baseline fixtures.
  --component  select a fixture component; default: all.
  --list       list baseline fixture IDs.
  --preserve   keep the temporary work directory for debugging.
EOF
}

MODE=""
COMPONENT="all"
PRESERVE=0

while [ "$#" -gt 0 ]; do
  case "$1" in
    --baseline)
      MODE="baseline"
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
    --list)
      MODE="list"
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

P="$DIST_DIR/pandaPIparser"
G="$DIST_DIR/pandaPIgrounder"
E="$DIST_DIR/pandaPIengine"

list_fixtures() {
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

if [ "$MODE" = "list" ]; then
  list_fixtures
  exit 0
fi

if [ "$MODE" != "baseline" ]; then
  echo "run-contract-fixtures.sh: --baseline is required unless --list is used" >&2
  usage >&2
  exit 1
fi

for bin in "$P" "$G" "$E"; do
  if [ ! -x "$bin" ]; then
    echo "run-contract-fixtures.sh: missing binary: $bin; run ./scripts/build-all.sh first" >&2
    exit 1
  fi
done

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
    pass "$id artifact complete: ${artifact#$WORK/}"
  else
    fail "$id expected complete artifact: ${artifact#$WORK/}"
  fi
}

expect_artifact_absent() {
  local id="$1" artifact="$2"
  if [ ! -e "$artifact" ]; then
    pass "$id artifact absent: ${artifact#$WORK/}"
  else
    fail "$id expected absent artifact: ${artifact#$WORK/}"
  fi
}

expect_artifact_retained() {
  local id="$1" artifact="$2"
  if [ -s "$artifact" ]; then
    pass "$id artifact retained after failure: ${artifact#$WORK/}"
  else
    fail "$id expected retained artifact after failure: ${artifact#$WORK/}"
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
