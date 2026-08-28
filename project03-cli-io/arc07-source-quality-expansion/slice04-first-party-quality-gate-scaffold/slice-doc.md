# Arc07 Slice04: first-party-quality-gate-scaffold

Status: open
Opened: 2026-08-12

## Goal

Add the Make-backed first-party source-quality gate scaffold that later
component burndown slices will use for parser, grounder, engine, runtime, and
Chengdu-owned generators/templates.

This slice is about honest target shape, report placement, source-class
routing, and re-entry conditions. It should not clear the warning/static/
coverage/unit-test debt itself, and it should not set release thresholds before
component baselines exist.

## Background

Arc07 Slice01 classified the source-quality surface. Slice02 added selectors,
compile databases, and profile/source mapping. Slice03 normalized first-party
maintained C/C++ source names so selectors and reports can rely on stable
paths.

Slice04 now turns that runway into named Make gate families and durable
scaffold evidence. The target families should make the next burndown slices
obvious:

- formatting;
- static analysis;
- coverage;
- unit/seam tests;
- warning inventory and policy;
- generated-warning root-cause/correctness triage;
- sanitizer source-class triage.

The key discipline is honesty. A target may run a real gate, produce a
report-only baseline, or state that a component is not yet enforceable with a
re-entry condition. It must not be a dummy green target that looks like quality
coverage without exercising or reporting anything meaningful.

## In Scope

- Add or revise Make-backed target families for first-party source-quality
  gates, with help text. Expected aggregate targets:
  - `make source-quality-gate-report`
  - `make format-check-first-party`
  - `make static-analysis-first-party`
  - `make coverage-first-party`
  - `make warning-inventory-first-party`
  - `make generated-warning-triage`
  - `make test-unit`
  - `make sanitize-first-party`
- Add component forms or durable report rows for parser, grounder, engine, and
  runtime. If a component target is not yet executable, record why and the
  re-entry condition instead of silently passing.
- Reuse the Slice02/03 source-quality selectors, compile database outputs,
  profile/source maps, and naming checks where practical.
- Write a durable scaffold report, expected at:
  `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`.
- In the report, map every gate family by component and source class:
  first-party maintained, first-party generated, first-party
  generator/template, vendored third-party, dependency-internal,
  third-party generated, copied build artifact, and generated build output.
- Add generated-warning triage records or a report schema that requires:
  generated file, generator/template or skeleton, ownership class, warning
  reason, semantic correctness impact, chosen remedy, and re-entry condition.
- Document optional IPC corpus placement. Existing `make test-corpus
  CORPUS_DIR=...` may remain optional and external-corpus dependent, but the
  scaffold must not confuse it with unit coverage or with a release-grade IPC
  benchmark gate unless a later ledger explicitly accepts that policy.
- Preserve existing runtime gates while routing parser, grounder, and engine
  first-party work into later component burndown slices.

## Out Of Scope

- No parser, grounder, engine, runtime, or generator behavior rewrites except
  tiny helper/report plumbing required by the scaffold.
- No warning fixes, static-analysis fixes, coverage-floor enforcement, or
  unit-test implementation beyond minimal target/report scaffolding.
- No new public CLI behavior, supported-surface policy, or optional/fenced
  feature promotion.
- No CI workflow changes unless a new workflow already has an existing Make
  target and the ledger explicitly accepts the wiring. This slice is local
  scaffold first.
- No public README/tutorial, Arc08, Arc09, release-package, publication, or
  wolong migration edits.
- No new `scripts/` entrypoint.
- No new external dependency pin unless the slice stops and requests an
  amendment. Catch2/test-dependency posture may be reported, but dependency
  adoption belongs in a later accepted slice.
- No blanket warning suppressions, global `-Werror`, release coverage floor,
  or threshold policy.
- No third-party/dependency-internal source promotion into first-party
  obligations.

## Required Target Shape

The scaffold should make these targets discoverable through `make help`:

- `source-quality-gate-report`
- `format-check-first-party`
- `static-analysis-first-party`
- `coverage-first-party`
- `warning-inventory-first-party`
- `generated-warning-triage`
- `test-unit`
- `sanitize-first-party`

Component-specific forms are encouraged when they can be honest now. If CC
chooses report-only rows instead of component targets for this slice, the
durable report must list the exact component target names intended for later
burndown slices and explain why they are not executable yet.

Every new target must do one of three honest things:

- run an existing real gate;
- produce a generated/durable evidence report from current source-quality
  selectors, compile databases, build logs, profile maps, or sanitizer output;
- fail or skip with a clear missing-tool/precondition message and documented
  re-entry condition.

No target may pass only because it echoes that the work is future work.

## Verification Approach

Primary Slice04 verification:

- `make help`
- `make source-quality-gate-report`
- `make format-check-first-party`
- `make static-analysis-first-party`
- `make coverage-first-party`
- `make warning-inventory-first-party`
- `make generated-warning-triage`
- `make test-unit`
- `make sanitize-first-party`

Existing source-quality runway verification:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-first-party`

Regression checks:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Run `make check` if practical. If a narrower set is used, the closing report
must state exactly what ran and why `make check` was not used.

## Exit Criteria

- The expected scaffold targets are Make-backed, listed in `make help`, and
  have non-vacuous behavior.
- A durable scaffold report exists and maps gate family, component,
  source-class scope, current status, evidence path, and re-entry condition.
- Generated-warning triage is an explicit target/report surface with the
  required root-cause and correctness fields.
- Parser, grounder, engine, runtime, generated, dependency, and third-party
  boundaries remain source-class routed.
- Optional IPC corpus testing is documented as optional external-corpus smoke,
  not as a unit/coverage substitute or silent release gate.
- Existing source-quality, behavior, safety, static-analysis, format,
  provenance, and whitespace gates still pass.
- No public behavior, CI workflow, release, README/tutorial, Arc08, Arc09,
  dependency, warning-fail-budget, or coverage-floor policy expands.
- Ledger rows close with evidence and Bubble-up states the recommended Slice05
  component burndown order or any required split/remediation.
