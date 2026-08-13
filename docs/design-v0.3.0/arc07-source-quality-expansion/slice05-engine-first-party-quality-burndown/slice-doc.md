# Arc07 Slice05: engine-first-party-quality-burndown

Status: open
Opened: 2026-08-12

## Goal

Convert the Slice04 report-backed engine source-quality scaffold into the
first executable component burndown for engine first-party maintained source.

This slice starts with engine because Slice04 CDC verification accepted that
order: the remaining primary warning budget is narrow but risk-bearing, and
the engine needs a small internal seam foothold before warning, sanitizer,
static-analysis, and coverage evidence can become stronger than process
contract tests alone.

## Background

Arc07 Slice01 classified parser, grounder, engine, runtime, generated,
dependency, third-party, copied-build, and build-output surfaces. Slice02 added
source-quality selectors, profile/source maps, and component compile database
evidence. Slice03 normalized first-party maintained source path names. Slice04
added the source-quality gate scaffold and recorded the engine re-entry
conditions:

- `format-check-engine` should cover engine first-party maintained source while
  excluding CUDD and generated `cmdline.*` output.
- `static-analysis-engine` should run clang-tidy over selected engine
  translation units from `compile-db-engine`, again excluding generated and
  third-party surfaces.
- `coverage-engine` needs honest first-party baseline placement and copied
  build path mapping before any floor exists.
- `test-unit-engine` should add CTest or Catch2 seam tests, starting near
  `visited_list`/search/data-structure risk.
- `warning-inventory-engine` should burn down or explicitly disposition the
  engine primary `visited_list.cpp` payload-cast warning and separately
  classify duplicate link-library noise.
- `sanitize-engine` should map binary sanitizer findings back to engine
  first-party, generated, dependency, or copied-build classes.

The known engine warning budget from Arc06 is:

- primary first-party maintained warning:
  `pandaPI/engine/src/visited_list.cpp` integer-to-pointer payload cast;
- build-system/toolchain warning:
  duplicate link-library warning for `heuristics/libheuristics.a` and
  `heuristics/rc_heuristics/libhrc.a`.

## In Scope

- Add Make-backed engine component targets where they can be honest:
  - `make format-check-engine`
  - `make static-analysis-engine`
  - `make coverage-engine`
  - `make warning-inventory-engine`
  - `make test-unit-engine`
  - `make sanitize-engine`
- Reuse `compile-db-engine`, `source-quality-profile-map`,
  `source-quality-naming-check`, and the existing source-class selectors.
- Keep generated `cmdline.c`/`cmdline.h`, vendored CUDD, copied build trees,
  and build output excluded from first-party maintained engine gates unless a
  row explicitly accepts separate evidence for them.
- Add a small engine unit/seam test surface for the `visited_list` payload
  representation or the nearest safe data-structure seam needed to change that
  warning honestly.
- Fix the engine primary `visited_list.cpp` integer-to-pointer payload-cast
  warning only after the seam evidence makes the representation change safe.
- Investigate and either fix or explicitly classify the duplicate
  link-library warning without changing supported CLI behavior.
- Update source-quality reports so engine component target status, warning
  budget disposition, coverage baseline placement, sanitizer mapping, and
  re-entry conditions are durable.
- Preserve managed engine process-contract behavior.

## Out Of Scope

- Parser or grounder source-quality burndown.
- Parser/grounder generated-warning fixes or CLI11 migration.
- Full engine algorithm modernization, heuristic rewrites, SAT/translation/BDD
  support expansion, or optional/fenced public behavior promotion.
- Vendored CUDD analysis, formatting, coverage, warning cleanup, or sanitizer
  obligations.
- Global coverage floors, global zero-warning release policy, global
  `-Werror`, or broad static-analysis thresholds.
- CI workflow changes unless the slice first adds the required Make target and
  the ledger explicitly accepts a workflow edit. This slice should not need CI
  wiring.
- Public README/tutorial, Arc08, Arc09, release/package/publication, wolong
  migration, or dependency-pinning changes.
- New non-test external dependencies without an amendment. Catch2 may remain
  optional/local if used; do not silently pin or fetch it.

## Required Target Shape

Each engine target must be discoverable through `make help` and must be
non-vacuous:

- `format-check-engine` runs clang-format check over selected engine
  first-party maintained source or fails with concrete selection evidence.
- `static-analysis-engine` runs clang-tidy/clang-analyzer over selected engine
  translation units from `compile-db-engine`, or records a tool/precondition
  skip with a re-entry condition matching the runtime analyzer behavior.
- `coverage-engine` produces an engine first-party coverage baseline or an
  explicitly named baseline report from managed/unit profiles. It must not set
  a release floor in this slice.
- `warning-inventory-engine` reports engine warning classes and must show the
  primary `visited_list.cpp` budget closed, or stop for an amendment if closure
  is not safe.
- `test-unit-engine` runs the new engine seam/unit tests and reports them
  separately from process-contract fixtures.
- `sanitize-engine` runs existing binary sanitizer evidence for engine and
  maps findings to source classes, or records an explicit platform/toolchain
  limitation with a re-entry condition.

If CC finds that any target cannot be made honest inside this slice, stop and
request an amendment rather than adding an empty green target.

## Verification Approach

Engine component evidence:

- `make help`
- `make compile-db-engine`
- `make format-check-engine`
- `make static-analysis-engine`
- `make coverage-engine`
- `make warning-inventory-engine`
- `make test-unit-engine`
- `make sanitize-engine`
- `make build-engine`
- `make test-contract-engine-managed`

Cross-surface regression evidence:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-first-party`
- `make warning-inventory-first-party`
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

- Engine component source-quality targets are Make-backed, listed in
  `make help`, and non-vacuous.
- Engine first-party maintained source selection is explicit and excludes
  generated `cmdline.*`, vendored CUDD, copied build trees, and build output.
- Engine unit/seam evidence exists for the `visited_list` payload
  representation or a documented nearest safe seam.
- The engine primary `visited_list.cpp` payload-cast warning is fixed or the
  slice stops for amendment with evidence that it cannot be fixed safely here.
- Duplicate engine link-library warning noise is fixed or explicitly
  classified with a durable re-entry condition.
- Engine static-analysis, format, coverage, warning, unit, and sanitizer
  reports are durable and source-class routed.
- Managed engine process-contract fixtures and the full project test chain
  still pass.
- No parser/grounder, public behavior, CI, release, README/tutorial, Arc08,
  Arc09, dependency, global threshold, or optional/fenced behavior scope expands.
- Ledger rows close with evidence and Bubble-up states whether the next Arc07
  slice should proceed to parser generated-warning/root-cause work or whether
  engine remediation remains.
