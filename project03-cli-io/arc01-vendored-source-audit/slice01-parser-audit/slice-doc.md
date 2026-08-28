# chengdu arc01 / slice01 - parser-audit - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Audit `pandaPI/pandaPIparser` as a vendored C/C++ command-line tool that must
become both a high-quality CLI and a high-quality supervised process. Produce
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
with concrete findings, clean checks, and recommendations that later slices can
compare against the grounder and engine reports.

## 2. Scope

**In:**

- Parser source, headers, generated-code inputs, makefile/build surface,
  tests/fixtures where present, and public CLI behavior implied by
  `options.ggo` and `main.cpp`.
- C++ Core Guidelines-derived audit substrate from
  `/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/`, starting with
  `SKILL.md` and `guides/11-anti-patterns.md`, then parser-relevant topic
  guides.
- Correctness/runtime-safety risks visible from code and existing fixtures.
- Error handling, exit/status behavior, stdout/stderr/TTY/color discipline,
  buffering/flushing under pipes, and non-interactive assumptions.
- CLI/API design issues relevant to `pandapi-*` namespacing, help/version
  output, machine-readable mode, and wolong-style supervision.
- Duplicate code, shared-code candidates, common headers/helpers, and library
  opportunities that should be compared with grounder/engine later.
- The report structure, severity scale, and category vocabulary needed for
  later synthesis.

**Out:**

- No parser source cleanup or behavior changes.
- No makefile/script/workflow/release edits.
- No grounder or engine audit beyond noting obvious cross-references needed
  for synthesis.
- No final 0.3.0 managed-process design decisions.

## 3. Constraints

- Diagnosis only: the committed diff may add planning/report artifacts, but
  must not change `pandaPI/`, `scripts/`, `.github/`, release assets, or build
  outputs.
- Every finding must cite at least one exact file/line. Broad advice without a
  cited location does not belong in the report.
- Do not soft-pedal severity. Use Blocker, High, Medium, or Low, with the same
  definitions used by the arc.
- Use the C++ knowledge pack as the source-quality substrate, but do not apply
  it mechanically. The parser's current build system, compiler flags,
  generated-code inputs, legacy constraints, and 0.3.0 process-contract goals
  decide which guideline violations become findings.
- If a finding cannot be verified without running a build/test that is blocked
  by sandbox or missing dependencies, record the command and the blocker
  rather than inventing a result.

## 4. Verification approach

CC verifies by producing the parser report, citing exact source lines, listing
source inventory evidence, recording clean checks, and updating the ledger
Evidence column with report sections and relevant command output. CDC verifies
by checking the report path, row count, cited files/lines, absence of source
edits, and enough sampled findings/clean checks to confirm the report is
evidence-backed.

## 5. Exit criteria

The ledger reaches final status. The parser audit report exists at the planned
arc-local path, uses the required categories and severity scale, cites concrete
parser source locations, maps findings to C++ Core Guidelines rule IDs where
applicable, includes at least five clean checks, names cross-codebase synthesis
inputs, and leaves the worktree with no parser source or build/release changes.
