# chengdu arc02 / slice01 - parser-library-research - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Use the accepted parser audit report as evidence to identify, assess, and
recommend open source C++ libraries that could materially improve
`pandaPIparser` maintainability, process behavior, performance, and future
shared-runtime design without distracting chengdu from PANDA's planning value.

## 2. Scope

**In:**

- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
  as the primary problem statement.
- Local parser source samples needed to confirm audit-to-candidate mapping.
- Current public documentation and release metadata for candidate C++ libraries.
- Candidate categories for CLI parsing, status/error propagation,
  diagnostics/formatting/logging, structured event output, tests, parser/lexer
  technology, and standard-library modernization.
- A parser-specific recommendation report in `closing-report.md`.

**Out:**

- No parser source changes.
- No dependency vendoring or build-system changes.
- No final all-codebase recommendation until grounder and engine research
  slices complete.
- No commitment to a parser-generator rewrite.

## 3. Constraints

- Prefer standard-library facilities when they solve the defect class cleanly.
- Third-party candidates must clear license, release/build, supported-platform,
  packaging, maintenance, and security/update checks before an `adopt` or
  `pilot` disposition.
- Every recommendation must map back to concrete parser audit findings.
- Parser-only evidence cannot decide a cross-codebase shared dependency by
  itself; unresolved cross-codebase questions bubble up to slice04.

## 4. Verification approach

Verify by checking that `closing-report.md` contains the reusable workflow
application, a candidate matrix with dispositions, links to current upstream
sources, mappings to parser audit findings, explicit rejected/held candidates,
and no source/build/release changes outside planning/research artifacts.

## 5. Exit criteria

The ledger reaches final status. The parser report identifies adopt/pilot/hold/
reject candidates, maps them to parser findings, records source evidence, and
leaves future grounder/engine research with a reusable workflow.
