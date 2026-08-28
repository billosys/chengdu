# chengdu arc01 / slice03 - engine-audit - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Audit `pandaPI/pandaPIengine` as a vendored C/C++ command-line planner that
must become both a high-quality CLI and a high-quality supervised process.
Produce
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
with concrete findings, clean checks, and recommendations focused on managed
process behavior, resource safety, status semantics, and maintainable source
quality.

## 2. Scope

**In:**

- Engine source, headers, `src/CMakeLists.txt`, `src/options.ggo`,
  `problemSolver.sh`, README/docs, public CLI behavior, and any local
  tests/fixtures if present.
- C++ Core Guidelines-derived audit substrate from
  `/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/`, starting with
  `SKILL.md` and `guides/11-anti-patterns.md`, then engine-relevant topic
  guides.
- Correctness/runtime-safety risks visible from code, especially search
  lifecycle, global flags, pointer/reference ownership, memory growth, and
  malformed-input behavior.
- Error handling, exit/status behavior, stdout/stderr/TTY/color discipline,
  buffering/flushing under pipes, signal/resource behavior, and
  non-interactive assumptions.
- CLI/API design issues relevant to `pandapi-*` namespacing, help/version
  output, machine-readable mode, and wolong-style supervision.
- Duplicate code, shared-code candidates, common headers/helpers, and library
  opportunities that should be compared with parser/grounder later.

**Out:**

- No engine source cleanup or behavior changes.
- No makefile/script/workflow/release edits.
- No solver-algorithm research or search-performance redesign beyond concrete
  risk findings visible from the current source.
- No final 0.3.0 managed-process design decisions.

## 3. Constraints

- Diagnosis only: the committed diff may add planning/report artifacts, but
  must not change `pandaPI/`, `scripts/`, `.github/`, release assets, or build
  outputs.
- Every finding must cite at least one exact file/line. Broad advice without a
  cited location does not belong in the report.
- Use the C++ knowledge pack as the source-quality substrate, but do not apply
  it mechanically. The engine's current CMake surface, generated-code inputs,
  legacy constraints, and 0.3.0 process-contract goals decide which guideline
  violations become findings.
- Treat search/resource behavior as process-contract relevant: timeout,
  memory, signal, and unsolved/unsolvable classification issues should be
  reported even when they are not C++ style findings.
- If a build/test/probe is blocked by sandbox, missing dependencies, or
  platform assumptions, record the command and blocker rather than inventing a
  result.

## 4. Verification approach

CC verifies by producing the engine report, citing exact source lines, listing
source/build inventory evidence, recording clean checks, and updating the
ledger Evidence column with report sections and relevant command output. CDC
verifies by checking the report path, row count, cited files/lines, absence of
source edits, and enough sampled findings/clean checks to confirm the report
is evidence-backed.

## 5. Exit criteria

The ledger reaches final status. The engine audit report exists at the planned
arc-local path, uses the required categories and severity scale, cites concrete
engine source locations, maps findings to C++ Core Guidelines rule IDs where
applicable, includes at least five clean checks, names cross-codebase synthesis
inputs, and leaves the worktree with no engine source or build/release changes.
