# Arc07 Slice07: grounder-source-quality-boundary

Status: open
Opened: 2026-08-13

## Goal

Convert the grounder source-quality scaffold into honest grounder component
gates while making the grounder dependency boundary executable and durable.

This slice follows Slice06's parser generated-warning cleanup. The current
grounder primary warning budget is believed to be clear, but the build still
reports warnings from cpddl, nested cpddl third-party code, H2, and `rss.c`.
Slice07 must prove which of those are first-party maintained grounder source
and which are dependency/dependency-internal evidence before any warning,
static-analysis, coverage, sanitizer, or unit-test claim is allowed to stand.

## Background

Arc07 currently classifies grounder source as:

- first-party maintained: `pandaPI/grounder/src/*.cpp`, `*.h`, and `rss.c`,
  excluding generated outputs;
- first-party generator/template: `pandaPI/grounder/src/options.ggo`;
- generated build output: `pandaPI/grounder/src/cmdline.c` and
  `cmdline.h`;
- vendored third-party / dependency-internal:
  `pandaPI/grounder/cpddl/` and
  `pandaPI/grounder/h2-fd-preprocessor/`;
- vendored third-party / third-party generated:
  `pandaPI/grounder/cpddl/third-party/*`, including lp_solve generated
  files.

Arc06 Slice05 recorded the current grounder warning classes:

- cpddl boruvka GNU null-pointer arithmetic warnings;
- macOS semaphore deprecation warnings;
- opts strict-prototype warnings;
- lpsolve `-O9` and parentheses warnings;
- RSS strict-prototype warnings;
- H2 `h2_mutexes.cc` unused counter warnings.

That record also says primary grounder warnings are clear after earlier fixes.
Slice07 must reconcile the remaining ambiguity around `rss.c`: if `rss.c` is
first-party maintained source, its warnings are first-party debt; if it is a
copied or dependency-internal source file, the source-class inventory and gate
selectors must say so explicitly.

## In Scope

- Add Make-backed grounder component targets where they can be honest:
  - `make format-check-grounder`
  - `make static-analysis-grounder`
  - `make coverage-grounder`
  - `make warning-inventory-grounder`
  - `make test-unit-grounder`
  - `make sanitize-grounder`
- Reuse `compile-db-grounder`, `source-quality-profile-map`,
  `source-quality-naming-check`, `compile-db-first-party`, and existing
  source-class selectors.
- Make the grounder first-party maintained selection explicit and
  reproducible.
- Keep generated `cmdline.c`/`cmdline.h`, cpddl, H2, nested cpddl
  third-party source, copied build roots, and build output excluded from
  first-party maintained gates unless a ledger row explicitly accepts separate
  evidence for them.
- Resolve the `rss.c` classification and warning-budget status.
- Add the smallest useful grounder unit/seam test surface for maintained
  grounder logic, preferably near model/output/parser/helper code that can be
  exercised without turning process-contract fixtures into unit tests.
- Report or baseline grounder static-analysis, coverage, warning, unit, and
  sanitizer evidence by source class.
- Preserve managed grounder and managed pipeline process-contract behavior.

## Out Of Scope

- Parser or engine source-quality burndown.
- Full cpddl, H2, boruvka, opts, lpsolve, or RSS dependency audit.
- Editing dependency-internal or third-party code only to reduce the global
  warning count.
- Hand-editing generated grounder `cmdline.c` or `cmdline.h`.
- Grounder CLI11 migration or option parser replacement.
- Broad grounder algorithm modernization, H2/FAM support expansion, or
  public-surface promotion for optional/fenced behavior.
- Global `-Werror`, global zero-warning release policy, global coverage
  floors, or broad static-analysis thresholds.
- CI workflow edits unless the slice first adds the required Make target and
  the ledger explicitly accepts a workflow edit. This slice should not need CI
  wiring.
- Public README/tutorial, Arc08, Arc09, release/package/publication, wolong
  migration, or dependency-pinning changes.
- New non-test external dependencies without an amendment. Catch2 may remain
  optional/local if used; do not silently pin or fetch it.

## Required Target Shape

Each grounder target must be discoverable through `make help` and must be
non-vacuous:

- `format-check-grounder` runs clang-format check over selected grounder
  first-party maintained source, or records a baseline report with concrete
  selected files and drift evidence.
- `static-analysis-grounder` runs clang-tidy/clang-analyzer over selected
  grounder first-party translation units from `compile-db-grounder`, excluding
  generated and dependency paths without blanket suppressions.
- `coverage-grounder` produces a grounder first-party coverage baseline from
  unit/seam or mapped managed-fixture profiles. It must not set a release
  floor in this slice.
- `warning-inventory-grounder` reports first-party grounder warning status
  separately from cpddl, H2, nested third-party, generated, copied-build, and
  build-output warning classes.
- `test-unit-grounder` runs new grounder unit/seam tests and reports them
  separately from process-contract fixtures.
- `sanitize-grounder` runs grounder sanitizer evidence or records a precise
  platform/toolchain limitation, with findings routed by source class.

If CC finds that any target cannot be made honest inside this slice, stop and
request an amendment rather than adding an empty green target.

## Verification Approach

Grounder component evidence:

- `make help`
- `make compile-db-grounder`
- `make format-check-grounder`
- `make static-analysis-grounder`
- `make coverage-grounder`
- `make warning-inventory-grounder`
- `make test-unit-grounder`
- `make sanitize-grounder`
- `make build-grounder`
- `make test-contract-grounder-managed`
- `make test-contract-pipeline-managed`

Boundary/source-class evidence:

- Grep generated reports for `cpddl`, `h2-fd-preprocessor`, `third-party`,
  `cmdline`, `rss.c`, `first-party maintained`, `dependency-internal`,
  `chosen disposition`, and `re-entry`.
- Grep selected grounder translation units to prove dependency/generated paths
  are excluded from first-party maintained gates.
- Grep warning reports to prove first-party grounder warnings are either
  absent or fixed, and remaining warnings are dependency/generated/third-party
  classes with explicit re-entry.

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

- Grounder component source-quality targets are Make-backed, listed in
  `make help`, and non-vacuous.
- Grounder first-party maintained source selection is explicit and excludes
  generated `cmdline.*`, cpddl, H2, nested third-party, copied build roots,
  and build output from first-party maintained gate interpretation.
- The `rss.c` classification is resolved and reflected in selectors,
  reports, and warning-budget evidence.
- Grounder warning inventory separates first-party warnings from dependency,
  generated, third-party, copied-build, and build-output warnings.
- Any first-party grounder warning observed by this slice is fixed or the
  slice stops for amendment with evidence that it cannot be fixed safely here.
- Grounder unit/seam evidence exists and remains distinct from process
  contract fixtures.
- Grounder static-analysis, format, coverage, warning, unit, and sanitizer
  reports are durable and source-class routed.
- Managed grounder and managed pipeline process-contract fixtures still pass.
- No parser/engine, public behavior, CI, release, README/tutorial, Arc08,
  Arc09, dependency, global threshold, or optional/fenced behavior scope
  expands.
- Ledger rows close with evidence and Bubble-up states whether Arc07 should
  proceed to parser maintained-source burndown, stricter generated-code work,
  a dependency audit, or Arc07 synthesis.
