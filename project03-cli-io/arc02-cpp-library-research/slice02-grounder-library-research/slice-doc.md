# chengdu arc02 / slice02 - grounder-library-research - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Use the accepted grounder audit report as evidence to identify, assess, and
recommend open source C++ libraries that could materially improve
`pandaPIgrounder` maintainability, process behavior, dependency-boundary
control, testability, and performance posture without prematurely changing
grounding algorithms or vendored dependency internals.

## 2. Scope

**In:**

- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
  as the primary problem statement.
- Existing Arc02 reusable workflow and parser-library report for comparable
  categories and disposition vocabulary.
- Grounder-specific candidate categories for CLI parsing, status/error
  propagation, diagnostics/formatting/logging, dependency process containment,
  fixture tests, resource/performance measurement, safer bounds helpers, and
  hash/container modernization.
- Current public documentation and release metadata for candidate C++
  libraries.
- A grounder-specific recommendation report in `closing-report.md`.

**Out:**

- No grounder source changes.
- No dependency vendoring, build-system rewrites, or test implementation.
- No deep standalone recommendation for `cpddl` or `h2-fd-preprocessor`
  internals beyond grounder-owned integration boundaries and re-entry
  conditions.
- No final all-codebase recommendation until engine research completes.

## 3. Constraints

- Prefer C++17 standard-library facilities when they solve the defect class
  cleanly.
- Third-party candidates must map to concrete grounder audit findings and clear
  license, build, packaging, maintenance, and security/update checks before an
  `adopt` or `pilot` disposition.
- Do not recommend performance containers or allocators without measurement.
  Grounder is performance-sensitive, but the audit did not establish a
  benchmark baseline.
- Dependency-boundary recommendations must distinguish in-process adapters from
  subprocess containment and must not silently expand this slice into a deep
  audit of `cpddl` or H2.

## 4. Verification approach

Verify by checking that `closing-report.md` contains an audit-finding mapping,
candidate matrix with dispositions, current upstream source links, explicit
held/rejected candidates, cross-codebase bubble-up notes, and a no-source-change
guard that ignores unrelated engine-audit work already present in the tree.

## 5. Exit criteria

The ledger reaches final status. The grounder report identifies adopt/pilot/
hold/reject candidates, maps them to grounder findings, records source evidence,
and strengthens or changes the Arc02 combined-recommendation inputs without
touching grounder source, dependency source, scripts, workflows, or release
assets.
