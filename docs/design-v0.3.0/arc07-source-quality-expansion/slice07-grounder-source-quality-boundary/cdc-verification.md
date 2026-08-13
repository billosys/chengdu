# CDC Verification: Arc07 Slice07 grounder-source-quality-boundary

Date: 2026-08-13

Verified commit:
`b8fe92a8dc76e8ab162007324c388bd863765921`

CDC verdict: verified closed.

Slice07 successfully converts the grounder source-quality scaffold into
Make-backed component evidence while keeping the first-party/dependency
boundary explicit. The new targets select maintained grounder source, exclude
generated `cmdline.*`, cpddl, H2, nested third-party source, copied build
roots, `dist`, `release`, and build output from first-party interpretation,
and preserve the managed grounder and managed pipeline contracts.

CDC found no blocking issues. One wording nuance is recorded: the ledger row
for unit/seam evidence mentions CTest in its grep terms, while the implemented
grounder seam is a direct compiled executable and the generated evidence says
CTest is not required for this smallest seam. That is acceptable for this
slice because the requirement is direct maintained-source seam evidence, not a
specific test harness framework.

## Scope Review

The reviewed commit changed only expected Slice07 surfaces:

- Arc07 Slice07 docs and durable source-quality reports.
- `mk/config.mk`, `mk/help.mk`, and `mk/source-quality.mk`.
- `tools/source-quality/surface`.
- `pandaPI/grounder/src/rss.c` and `rss.h`.
- `pandaPI/grounder/tests/top_sort_seam_test.cpp`.

CDC confirmed no `.github/`, README, release tooling, Arc08, Arc09, parser,
or engine scope was changed. The commit includes the required co-author
trailers.

## Reproduced Evidence

CDC reproduced the grounder component target family:

- `make help | rg 'format-check-grounder|static-analysis-grounder|coverage-grounder|warning-inventory-grounder|test-unit-grounder|sanitize-grounder'`
- `make compile-db-grounder`
- `make format-check-grounder`
- `make static-analysis-grounder`
- `make coverage-grounder`
- `make warning-inventory-grounder`
- `make test-unit-grounder`
- `make sanitize-grounder`

Observed grounder component results:

- `compile-db-grounder` selected 22 first-party grounder translation units.
- The selected list contained no `cmdline.*`, cpddl, H2, nested third-party,
  copied build, `dist`, or `release` paths.
- `format-check-grounder` passed as an executable reported baseline and
  recorded 43 inherited drift entries.
- `static-analysis-grounder` passed with clang-tidy exit status 0.
- `coverage-grounder` passed with a topsort seam baseline of 81.13% region
  coverage and no release coverage floor.
- `warning-inventory-grounder` passed with first-party maintained warning
  count 0 and 23 dependency-class evidence lines.
- The `rss.c` warning grep found no remaining `rss.c` warnings.
- `test-unit-grounder` passed through the direct `topsort` seam test.
- `sanitize-grounder` passed the `topsort` seam under ASan/UBSan.

CDC reproduced managed behavior evidence:

- `make build-grounder`
- `make test-contract-grounder-managed`
- `make test-contract-pipeline-managed`

Observed managed behavior results:

- `pandapi-grounder` rebuilt successfully.
- Managed grounder fixtures passed with 269 passed, 0 failed.
- Managed pipeline fixtures passed with 129 passed, 0 failed.

CDC reproduced the source-quality runway:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-first-party`
- `make warning-inventory-first-party`

Observed source-quality runway result:

- Aggregate warning inventory passed with `Warning lines: 50` in CDC's run.

CDC reproduced the regression chain:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Observed regression results:

- Safety checks, actionlint, runtime static analysis, current owned runtime
  formatting, full test, provenance, and whitespace checks passed.
- `make test` passed runtime tests, baseline fixtures, managed parser,
  grounder, engine, and pipeline fixtures, plus positive and negative smoke.
- `make check` was not run as an aggregate because the constituent gates were
  reproduced directly, avoiding the known macOS aggregate provenance/noise
  risk.

## Ledger Review

CDC reviewed all F-1 through F-16 evidence rows and agrees they are closed:

- F-1 through F-2: target exposure and first-party selector boundaries hold.
- F-3 through F-7: format, static analysis, coverage, warning inventory, and
  unit/seam evidence exist with source-class routing.
- F-8: `rss.c` is resolved as first-party maintained copied-origin source,
  and its strict-prototype warnings are fixed.
- F-9: generated grounder `cmdline.*` remains gengetopt output; no CLI11
  migration or public option behavior change was made.
- F-10: grounder sanitizer seam evidence exists, with macOS LSan and TSan
  limits still explicitly routed.
- F-11 through F-12: managed grounder and pipeline behavior did not regress.
- F-13 through F-14: source-quality runway and regression gates passed.
- F-15: scope stayed inside the grounder source-quality boundary.
- F-16: the closing report walks every row and bubbles up the next Arc07
  recommendation.

## Bubble-up to Arc07

Arc07 may mark Slice07 closed and CDC-verified.

CDC accepts the closing-report recommendation that the next slice should be
source-quality synthesis. Runtime, engine, parser generated-warning, and
grounder boundary evidence are now sufficient to compose the Arc07 posture,
name remaining re-entry work, and decide which remaining parser maintained
source, generated-code, dependency-audit, coverage-floor, and warning/static
analysis policies must be release blockers versus later budgets.
