# CC Prompt: Arc07 Slice07 - grounder-source-quality-boundary

You are implementing Arc07 Slice07 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`
6. `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`
7. `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md`
8. `docs/design-v0.3.0/arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/cdc-verification.md`
9. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/warning-inventory.md`
10. `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
11. `docs/design-v0.3.0/arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/slice-doc.md`
12. `docs/design-v0.3.0/arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/ledger.md`

Also inspect current Make/tool/source surfaces before editing:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `mk/checks.mk`
- `mk/format.mk`
- `mk/coverage.mk`
- `mk/tests.mk`
- `mk/build.mk`
- `tools/source-quality/surface`
- `pandaPI/grounder/src/Makefile`
- `pandaPI/grounder/src/options.ggo`
- `pandaPI/grounder/src/pandapi_grounder_native.cpp`
- `pandaPI/grounder/src/rss.c`
- nearby grounder model/output/parser/helper files only as needed for the
  smallest honest unit/seam test.

## Policy

This is a grounder source-quality boundary slice, not a full dependency audit
and not a broad grounder modernization slice.

The current warning budget after Slice06 is mostly dependency/dependency-
internal grounder build noise:

- cpddl boruvka GNU null-pointer arithmetic;
- macOS semaphore deprecation warnings;
- opts strict-prototype warnings;
- lpsolve `-O9` and parentheses warnings;
- H2 `h2_mutexes.cc` unused counters;
- RSS strict-prototype warnings.

The tricky part is RSS: `rss.c` lives under `pandaPI/grounder/src`, while
earlier warning-budget wording grouped RSS with dependency-internal warning
classes. Resolve that explicitly. If RSS is first-party maintained source, fix
or budget it as first-party warning debt. If it is copied/dependency-internal
source, update the source-class report, selectors, and warning evidence so the
exclusion is honest and reproducible.

Do not edit cpddl, H2, boruvka, opts, lpsolve, or other dependency code only
to lower the warning count. Dependency cleanup requires a separate audit or an
accepted support-surface decision.

## Task

Implement Slice07:

1. Add Make targets and help text:
   - `make format-check-grounder`
   - `make static-analysis-grounder`
   - `make coverage-grounder`
   - `make warning-inventory-grounder`
   - `make test-unit-grounder`
   - `make sanitize-grounder`
2. Reuse `compile-db-grounder`, `source-quality-profile-map`,
   `source-quality-naming-check`, `compile-db-first-party`, and existing
   source-class selectors.
3. Make grounder first-party selected files explicit and ensure generated,
   dependency, third-party, copied-build, and build-output paths are excluded
   from first-party maintained gate interpretation.
4. Resolve the `rss.c` source-class and warning-budget ambiguity.
5. Add the smallest useful grounder unit/seam test surface for maintained
   grounder logic. Do not count process-contract fixtures as unit tests.
6. Add honest grounder format/static-analysis/coverage/warning/unit/sanitizer
   reports with source-class routing and re-entry conditions.
7. Preserve managed grounder behavior and managed pipeline composition.
8. Update durable Arc07 reports, including
   `source-classification-inventory.md` and `source-quality-gate-scaffold.md`
   if the RSS or dependency-boundary classification changes.

## Scope Constraints

Allowed surfaces:

- `Makefile`
- `mk/`
- `tools/source-quality/`
- focused grounder build/test files under `pandaPI/grounder/`
- focused first-party grounder source edits only if needed to fix a warning
  or expose a small seam safely
- Arc07 source-quality reports
- Arc07 Slice07 docs/ledger/closing report

Do not change:

- parser or engine source/build/test behavior;
- generated grounder `cmdline.c` or `cmdline.h`;
- cpddl, H2, boruvka, opts, lpsolve, or nested third-party source unless the
  slice stops for an amendment first;
- public `pandapi-*` CLI behavior;
- grounder CLI11 migration or option parser replacement;
- CI workflows unless a ledger row is amended to accept one;
- public README/tutorial docs;
- Arc08 or Arc09 docs;
- release/package/publication tooling;
- wolong migration guidance;
- dependency pins/fetch behavior;
- global `-Werror`, global warning fail policy, coverage floors, or
  static-analysis thresholds;
- optional/fenced H2 or FAM public support.

Do not add a new `scripts/` entrypoint. All new automation must be reachable
through Make.

## Required Verification

Grounder component evidence:

```text
make help
make compile-db-grounder
make format-check-grounder
make static-analysis-grounder
make coverage-grounder
make warning-inventory-grounder
make test-unit-grounder
make sanitize-grounder
make build-grounder
make test-contract-grounder-managed
make test-contract-pipeline-managed
```

Boundary/source-class evidence:

```text
rg -n -- "cpddl|h2-fd-preprocessor|third-party|cmdline|rss\\.c|first-party maintained|dependency-internal|chosen disposition|re-entry" docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md build/source-quality/*/*grounder*.md
! rg -n -- "cmdline\\.c|cmdline\\.h|cpddl|h2-fd-preprocessor|third-party|build/.*/source|dist/|release/" build/source-quality/*/compile-db/grounder/first-party-selected.txt
```

If any `rss.c` warnings remain, the report must show their chosen source
class and re-entry:

```text
if rg -n -- "rss\\.c.*warning:|src/rss\\.c:.*warning:" build/warnings/*/build.log build/warnings/*/warning-inventory.txt; then
  rg -n -- "rss\\.c|RSS|first-party|dependency-internal|copied|re-entry" docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md build/source-quality/*/warning-inventory-grounder*.md
fi
```

Cross-surface regression evidence:

```text
make source-quality-surface
make source-quality-profile-map
make source-quality-naming-check
make compile-db-first-party
make warning-inventory-first-party
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

Run `make check` if practical. If you do not run it, say exactly why and list
the narrower Make-backed gates that substitute for this slice.

Also run the ledger boundary checks:

- no parser/engine changes;
- no `.github`, README, release, `tools/release`, Arc08, or Arc09 changes;
- no dependency pinning or fetch behavior;
- no global coverage floor, global warning failure policy, or global
  static-analysis threshold;
- no grounder CLI11 migration or public CLI behavior expansion;
- no optional/fenced H2 or FAM support promotion.

## Stop Conditions

Stop and request an amendment if:

- grounder component targets cannot be made honest without including cpddl,
  H2, nested third-party, generated output, copied build roots, or build
  output as first-party maintained source;
- `rss.c` cannot be classified cleanly without a dependency audit or source
  history decision;
- any first-party grounder warning cannot be fixed safely in this slice;
- `test-unit-grounder` would only wrap managed process fixtures rather than
  exercising an internal maintained seam;
- grounder coverage cannot produce a meaningful baseline/report without
  mixing dependency/generated/copied paths as first-party source;
- static analysis cannot run or report a baseline without blanket
  suppressions;
- sanitizer evidence cannot map findings back to source class;
- any parser, engine, CI, public docs, release, dependency pinning, global
  threshold, CLI11 migration, or public behavior change appears necessary;
- optional/fenced H2 or FAM behavior would be promoted as supported public
  behavior.

## Closing Requirements

Before committing:

- Close every ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/closing-report.md`.
- Include `Bubble-up to Arc07`, explicitly stating whether the next slice
  should proceed to parser maintained-source burndown, stricter generated-code
  work, a cpddl/H2/RSS dependency audit, or Arc07 synthesis.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
