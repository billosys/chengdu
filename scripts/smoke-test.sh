#!/usr/bin/env bash
# Mechanizes PANDA Runbook §5-§6 as executable gates against in-repo
# fixtures. Two modes:
#   (default)   the five-gate positive chain on fixtures/minimal —
#               parse, ground, solve (Status: Solved), convert, verify
#               (result: true) — with a per-gate PASS summary.
#   --negative  four distinct negative outcomes: missing file (parser
#               exit 2), broken-syntax fixture (exit 255, "Parse error
#               in file"), broken-reference fixture (exit 255, a
#               different — semantic — message), and the unsolvable
#               fixture (engine exit 0 but "Status: Proven unsolvable",
#               which must be classified as UNSOLVABLE, never as
#               success and never as a generic failure).
#
# Nonzero exit whenever any gate deviates from its expected outcome, in
# either mode. --corpus DIR runs the Transport pfile01 loop when an
# ipc2023-domains checkout is present at DIR; optional, not part of the
# required gate.
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: smoke-test.sh [--negative] [--corpus DIR]

  (default)   run the positive gate chain against fixtures/minimal.
  --negative  run the four negative-gate fixtures instead.
  --corpus DIR  additionally run the Transport pfile01 loop from an
                ipc2023-domains checkout at DIR (positive mode only).
EOF
}

MODE=positive
CORPUS_DIR=""
while [ "$#" -gt 0 ]; do
  case "$1" in
    --negative)
      MODE=negative
      shift
      ;;
    --corpus)
      CORPUS_DIR="${2:-}"
      shift 2
      ;;
    --corpus=*)
      CORPUS_DIR="${1#*=}"
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "smoke-test.sh: unknown argument: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

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

for bin in "$P" "$G" "$E"; do
  if [ ! -x "$bin" ]; then
    echo "smoke-test.sh: missing binary: $bin — run the build scripts first" >&2
    exit 1
  fi
done

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

PASS_COUNT=0
FAIL_COUNT=0

gate_pass() {
  echo "smoke-test.sh: PASS: $1"
  PASS_COUNT=$((PASS_COUNT + 1))
}

gate_fail() {
  echo "smoke-test.sh: FAIL: $1" >&2
  FAIL_COUNT=$((FAIL_COUNT + 1))
}

# strip_ansi <string> — pandaPIparser's --no-colour flag does not
# reliably suppress ANSI SGR codes on every error path (observed: the
# syntax-error message stays colored even with -C), which breaks
# substring matching on split-up text like "Parse\x1b[0merror". Strip
# unconditionally rather than depend on -C's coverage.
strip_ansi() {
  printf '%s' "$1" | sed $'s/\x1b\\[[0-9;]*m//g'
}

# run_and_capture <cmd...> — sets OUT (ANSI-stripped) and RC, without
# tripping set -e on an intentionally-nonzero exit (the negative
# fixtures rely on this).
OUT=""
RC=0
run_and_capture() {
  set +e
  OUT="$("$@" 2>&1)"
  RC=$?
  set -e
  OUT="$(strip_ansi "$OUT")"
}

run_positive() {
  local fdir="$FIXTURES_DIR/minimal"
  local htn="$WORK/dp.htn" sas="$WORK/dp.sas" raw="$WORK/plan.raw" plan="$WORK/plan.txt"

  run_and_capture "$P" -C "$fdir/domain.hddl" "$fdir/problem.hddl" "$htn"
  if [ "$RC" -eq 0 ]; then
    gate_pass "parse (exit 0)"
  else
    gate_fail "parse: expected exit 0, got $RC: $OUT"
  fi

  run_and_capture "$G" "$htn" "$sas"
  if [ "$RC" -eq 0 ]; then
    gate_pass "ground (exit 0)"
  else
    gate_fail "ground: expected exit 0, got $RC: $OUT"
  fi

  "$E" "$sas" > "$raw" 2>&1
  RC=$?
  if [ "$RC" -eq 0 ] && grep -q '^- Status: Solved$' "$raw"; then
    gate_pass "solve (exit 0, Status: Solved)"
  else
    gate_fail "solve: expected exit 0 + 'Status: Solved', got exit $RC: $(grep 'Status:' "$raw" || echo 'no Status line')"
  fi

  run_and_capture "$P" -c "$raw" "$plan"
  if [ "$RC" -eq 0 ]; then
    gate_pass "convert (exit 0)"
  else
    gate_fail "convert: expected exit 0, got $RC: $OUT"
  fi

  run_and_capture "$P" -C --verify "$fdir/domain.hddl" "$fdir/problem.hddl" "$plan"
  if [ "$RC" -eq 0 ] && printf '%s' "$OUT" | grep -q 'Plan verification result: true'; then
    gate_pass "verify (exit 0, Plan verification result: true)"
  else
    gate_fail "verify: expected exit 0 + 'Plan verification result: true', got exit $RC"
  fi
}

run_negative() {
  # a. missing file -> parser exit 2, "I can't open"
  run_and_capture "$P" -C "$WORK/does-not-exist.hddl" "$FIXTURES_DIR/minimal/problem.hddl" /dev/null
  if [ "$RC" -eq 2 ] && printf '%s' "$OUT" | grep -q "I can't open"; then
    gate_pass "negative: missing file -> exit 2, \"I can't open\""
  else
    gate_fail "negative: missing file: expected exit 2 + \"I can't open\", got exit $RC"
  fi

  # b. broken-syntax -> exit 255, "Parse error in file"
  run_and_capture "$P" -C "$FIXTURES_DIR/broken-syntax/domain.hddl" "$FIXTURES_DIR/broken-syntax/problem.hddl" /dev/null
  if [ "$RC" -eq 255 ] && printf '%s' "$OUT" | grep -q "Parse error in file"; then
    gate_pass "negative: broken-syntax -> exit 255, \"Parse error in file\""
  else
    gate_fail "negative: broken-syntax: expected exit 255 + \"Parse error in file\", got exit $RC"
  fi

  # c. broken-reference -> exit 255, a DIFFERENT (semantic) message —
  # must not collapse into the same reported reason as broken-syntax.
  run_and_capture "$P" -C "$FIXTURES_DIR/broken-reference/domain.hddl" "$FIXTURES_DIR/broken-reference/problem.hddl" /dev/null
  if [ "$RC" -eq 255 ] && printf '%s' "$OUT" | grep -q "not declared" && ! printf '%s' "$OUT" | grep -q "Parse error in file"; then
    gate_pass "negative: broken-reference -> exit 255, semantic \"not declared\" (distinct from syntax path)"
  else
    gate_fail "negative: broken-reference: expected exit 255 + \"not declared\" without \"Parse error in file\", got exit $RC"
  fi

  # d. unsolvable -> engine exit 0 but Status: Proven unsolvable —
  # classified UNSOLVABLE, never success, never a generic failure.
  local htn="$WORK/uns.htn" sas="$WORK/uns.sas" raw="$WORK/uns.raw"
  "$P" -C "$FIXTURES_DIR/unsolvable/domain.hddl" "$FIXTURES_DIR/unsolvable/problem.hddl" "$htn" >/dev/null 2>&1
  "$G" "$htn" "$sas" >/dev/null 2>&1
  "$E" "$sas" > "$raw" 2>&1
  RC=$?
  if [ "$RC" -eq 0 ] && grep -q '^- Status: Proven unsolvable$' "$raw"; then
    gate_pass "negative: unsolvable -> engine exit 0, Status: Proven unsolvable (UNSOLVABLE, not success, not failure)"
  else
    gate_fail "negative: unsolvable: expected exit 0 + 'Status: Proven unsolvable', got exit $RC: $(grep 'Status:' "$raw" || echo 'no Status line')"
  fi
}

run_corpus() {
  local dir="$1"
  local dom="$dir/total-order/Transport/domain.hddl"
  local prob="$dir/total-order/Transport/pfile01.hddl"

  if [ ! -f "$dom" ] || [ ! -f "$prob" ]; then
    echo "smoke-test.sh: --corpus $dir missing total-order/Transport/{domain,pfile01}.hddl — skipping" >&2
    return 0
  fi

  local t="$WORK/t.htn" tsas="$WORK/t.sas" tout="$WORK/t.out" tplan="$WORK/t.plan"
  "$P" -C "$dom" "$prob" "$t" >/dev/null 2>&1
  "$G" "$t" "$tsas" >/dev/null 2>&1
  "$E" "$tsas" > "$tout" 2>&1

  if grep -q '^- Status: Solved$' "$tout"; then
    gate_pass "corpus: Transport pfile01 solved"
  else
    gate_fail "corpus: Transport pfile01 did not solve"
  fi

  run_and_capture "$P" -c "$tout" "$tplan"
  if [ "$RC" -ne 0 ]; then
    gate_fail "corpus: Transport pfile01 convert failed: $OUT"
    return 0
  fi

  run_and_capture "$P" -C --verify "$dom" "$prob" "$tplan"
  if [ "$RC" -eq 0 ] && printf '%s' "$OUT" | grep -q 'Plan verification result: true'; then
    gate_pass "corpus: Transport pfile01 verified"
  else
    gate_fail "corpus: Transport pfile01 verify failed, got exit $RC"
  fi
}

echo "smoke-test.sh: platform=$PLATFORM mode=$MODE"

case "$MODE" in
  positive)
    run_positive
    if [ -n "$CORPUS_DIR" ]; then
      run_corpus "$CORPUS_DIR"
    fi
    ;;
  negative)
    run_negative
    ;;
esac

echo "smoke-test.sh: SUMMARY: $PASS_COUNT passed, $FAIL_COUNT failed ($MODE mode, $PLATFORM)"

if [ "$FAIL_COUNT" -gt 0 ]; then
  exit 1
fi
exit 0
