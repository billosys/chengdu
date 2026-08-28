# Arc07 Slice01: source-classification-inventory

Status: open
Opened: 2026-08-12

## Goal

Create a complete source-classification inventory for the current Chengdu
pandaPI tree and turn it into a recommendation report for the rest of Arc07.
The report must make the next decisions discussable: whether restructuring is
needed, which code classes receive which gates, which generated warnings are
ours to fix, which third-party surfaces are excluded, and how to sequence
first-party coverage/static-analysis/unit-test expansion.

## Background

Arc06 established useful quality gates, but its coverage/static-analysis
claims were runtime-first. That was an honest starting point, not an adequate
release endpoint. The operator has now clarified the release policy:

- parser, grounder, engine, and runtime are all Chengdu-owned product source;
- first-party maintained code needs formatting, static analysis, coverage,
  unit-test strategy, warning policy, and sanitizer triage;
- generated code from Chengdu-owned generators/templates should be fixed at
  the generator/template;
- generated code from third-party skeletons may be excluded or suppressed with
  rationale;
- vendored third-party/dependency-internal code is not a Chengdu
  coverage/static-analysis/unit-test obligation unless a later dependency
  audit accepts that boundary.

## In Scope

- Classify paths under `pandaPI/parser/`, `pandaPI/grounder/`,
  `pandaPI/engine/`, and `pandaPI/runtime/`.
- Identify generated source and, where possible, the generator/template owner.
- Identify vendored third-party and dependency-internal paths.
- Identify copied build-tree artifact paths and generated build outputs that
  should never become committed source-quality scope.
- Map current Make targets to each source class: format, static analysis,
  coverage, warning inventory, sanitizer, unit/seam tests, contract tests.
- Produce a recommendation report at
  `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`.
- Recommend whether Slice02 should restructure paths/builds before tooling
  expansion.
- Recommend the likely component/order for the long source-quality burndown.

## Out Of Scope

- No parser, grounder, engine, runtime, generator, Make, CI, or release
  behavior changes unless a tiny docs-only correction is needed to keep this
  inventory honest.
- No new coverage/static-analysis/format/unit-test targets.
- No source-code cleanup or warning fixes.
- No release publication, wolong migration, README/tutorial prose, or public
  docs changes.
- No promotion of optional legacy/experimental/unsupported/future public
  surfaces.

## Required Report Shape

The report must include:

1. Source-class taxonomy with definitions.
2. Path inventory table by component and class.
3. Generated-code ownership table: generated file, generator/template,
   owner/class, warning/format policy.
4. Third-party/dependency exclusion table with rationale.
5. Existing gate map by component/source class.
6. Gap map for coverage, static analysis, formatting, warnings, sanitizers,
   and unit/seam tests.
7. Recommendation section:
   - whether restructuring is needed in Slice02;
   - proposed Make target families;
   - proposed test dependency posture, including Catch2;
   - proposed warning/static-analysis/format policy;
   - proposed coverage baseline/floor strategy;
   - proposed component slice order.
8. Explicit operator-decision section: items CDC/operator should discuss
   before CC starts Slice02.

## Verification Approach

This is an inventory/report slice. Verification should use repository
inspection rather than behavioral test expansion:

- `rg --files` and targeted `find`/`file` checks for source classes.
- Existing Make target listing through `make help`.
- Existing gate checks where cheap and relevant: `make safety-checks`,
  `make actionlint`, `make static-analysis`, `make format-check`, and
  `make warning-inventory`.
- Whitespace checks with `git diff --check` and `git diff --cached --check`.
- Boundary checks proving no production source, Make/CI, release, README,
  Arc08 docs, or Arc09 release-prep surface changed.

If a full gate is intentionally skipped because the slice is docs-only, the
closing report must say exactly what was skipped and why.

## Exit Criteria

- Source classification report exists and is specific enough to plan Slice02.
- Every parser/grounder/engine/runtime source class is either classified or
  explicitly listed as unknown with a re-entry command to resolve it.
- The report does not use ambiguous "owned runtime versus inherited planner"
  language for active quality policy.
- Remaining decisions are explicit and discussable, not hidden deferrals.
- Ledger rows close with evidence, and CDC can reproduce the inventory.
