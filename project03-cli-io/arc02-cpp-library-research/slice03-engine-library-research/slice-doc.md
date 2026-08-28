# chengdu arc02 / slice03 - engine-library-research - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Use the accepted engine audit report as evidence to identify, assess, and
recommend open source C++ libraries that could materially improve
`pandaPIengine` maintainability, managed-process behavior, testability,
resource handling, optional child-process paths, and performance posture
without prematurely changing planner algorithms or widening the 0.3.0 supported
surface.

## 2. Scope

**In:**

- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
  as the primary problem statement.
- Existing Arc02 reusable workflow and parser/grounder reports for comparable
  categories and disposition vocabulary.
- Engine-specific candidate categories for CLI parsing, status/error
  propagation, diagnostics/formatting/logging, stream/event output,
  fixture/unit tests, child-process containment for translation mode, resource
  timeout/signal posture, checked model parsing, CUDD/BDD dependency posture,
  and performance measurement.
- Current public documentation and release metadata for candidate C++
  libraries.
- An engine-specific recommendation report in `closing-report.md`.

**Out:**

- No engine source changes.
- No dependency vendoring, build-system rewrites, or test implementation.
- No deep standalone audit of bundled CUDD, optional SAT code, symbolic search,
  or translation solver behavior.
- No final all-codebase recommendation until slice04 compares parser, grounder,
  and engine together.

## 3. Constraints

- Prefer C++17 standard-library facilities when they solve the defect class
  cleanly: scoped streams, `std::filesystem`, `std::chrono`, smart pointers,
  containers, local hashers, and explicit result/status values.
- Third-party candidates must map to concrete engine audit findings and clear
  license, build, packaging, maintenance, and security/update checks before an
  `adopt` or `pilot` disposition.
- Do not recommend performance containers, allocators, threading frameworks, or
  SAT/BDD dependency changes without measurement and a supported-surface
  decision.
- Treat translation, SAT, BDD, and interactive mode as contract-boundary
  decisions. A library recommendation must not silently make an optional or
  legacy path part of the 0.3.0 supported managed-process surface.

## 4. Verification approach

Verify by checking that `closing-report.md` contains an audit-finding mapping,
candidate matrix with dispositions, current upstream source links, explicit
held/rejected candidates, cross-codebase bubble-up notes, and a no-source-change
guard that ignores unrelated Arc01 audit work already present in the tree.

## 5. Exit criteria

The ledger reaches final status. The engine report identifies adopt/pilot/
hold/reject candidates, maps them to engine findings, records source evidence,
and strengthens or changes the Arc02 combined-recommendation inputs without
touching engine source, dependency source, scripts, workflows, release assets,
or tracked build outputs.
