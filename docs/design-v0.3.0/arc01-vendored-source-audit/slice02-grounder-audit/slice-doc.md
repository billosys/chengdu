# chengdu arc01 / slice02 - grounder-audit - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Audit `pandaPI/pandaPIgrounder` as a vendored C/C++ command-line tool and as
the integration owner for its former submodule dependencies. Produce
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
with concrete findings, clean checks, and recommendations that can be compared
against the parser and engine reports during synthesis.

## 2. Scope

**In:**

- Grounder-owned source, headers, generated-code inputs, `build.sh`,
  `src/Makefile`, docs, public CLI behavior implied by `options.ggo` and
  `main.cpp`, and any local tests/fixtures if present.
- Grounder-owned integration with `cpddl` and `h2-fd-preprocessor`, including
  build coupling, invocation assumptions, headers, generated artifacts, vendored
  provenance, and release-packaging consequences.
- C++ Core Guidelines-derived audit substrate from
  `/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/`, starting with
  `SKILL.md` and `guides/11-anti-patterns.md`, then grounder-relevant topic
  guides.
- Correctness/runtime-safety risks visible from code, especially parser/grounder
  data ingestion, global state, mutable shared state, resource handling, and
  malformed-input behavior.
- Error handling, exit/status behavior, stdout/stderr/TTY/color discipline,
  buffering/flushing under pipes, and non-interactive assumptions.
- CLI/API design issues relevant to `pandapi-*` namespacing, help/version
  output, machine-readable mode, and wolong-style supervision.
- Duplicate code, shared-code candidates, common headers/helpers, and library
  opportunities that should be compared with parser/engine later.

**Out:**

- No grounder source cleanup or behavior changes.
- No deep standalone audit of `cpddl` or `h2-fd-preprocessor` internals beyond
  findings needed to assess grounder-owned integration risk.
- No makefile/script/workflow/release edits.
- No final 0.3.0 managed-process design decisions.

## 3. Constraints

- Diagnosis only: the committed diff may add planning/report artifacts, but
  must not change `pandaPI/`, `scripts/`, `.github/`, release assets, or build
  outputs.
- Distinguish grounder-owned findings from dependency-internal findings. If a
  deeper dependency audit is warranted, route it as a recommendation with a
  re-entry condition rather than silently expanding this slice.
- Every finding must cite at least one exact file/line. Broad advice without a
  cited location does not belong in the report.
- Use the C++ knowledge pack as the source-quality substrate, but do not apply
  it mechanically. The grounder's current build system, generated-code inputs,
  vendored dependencies, legacy constraints, and 0.3.0 process-contract goals
  decide which guideline violations become findings.
- If a build/test/probe is blocked by sandbox, missing dependencies, or
  platform assumptions, record the command and blocker rather than inventing a
  result.

## 4. Verification approach

CC verifies by producing the grounder report, citing exact source lines,
listing source/dependency inventory evidence, recording clean checks, and
updating the ledger Evidence column with report sections and relevant command
output. CDC verifies by checking the report path, row count, cited files/lines,
dependency-boundary treatment, absence of source edits, and enough sampled
findings/clean checks to confirm the report is evidence-backed.

## 5. Exit criteria

The ledger reaches final status. The grounder audit report exists at the
planned arc-local path, uses the required categories and severity scale, cites
concrete grounder source locations, distinguishes dependency integration risk
from dependency-internal audit scope, maps findings to C++ Core Guidelines rule
IDs where applicable, includes at least five clean checks, names
cross-codebase synthesis inputs, and leaves the worktree with no grounder
source or build/release changes.
