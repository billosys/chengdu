# CDC Verification: Arc07 Slice02 source-layout-and-build-surface-normalization

Status: CDC verified closed
Date: 2026-08-12
Verified commit: `88b4e2535de5ee98c05287b859c30a2983aec189`

## Verdict

Slice02 is verified closed. The implementation adds the Make-backed
source-quality normalization runway requested by the ledger without expanding
public behavior, CI workflow policy, release packaging, README/tutorial docs,
or quality thresholds.

CDC accepts CC's proposed-done closure with one plan amendment: after Slice02
landed, the operator and CDC identified first-party C/C++ file and directory
naming normalization as prerequisite work before the quality gate scaffold
bakes in selector paths. This is not a Slice02 defect. It changes the next
Arc07 slice order: source naming normalization should run before
`first-party-quality-gate-scaffold`.

## Scope Review

The verified commit changed only the expected Make/source-quality and Slice02
evidence surfaces:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `tools/source-quality/surface`
- `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/ledger.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/closing-report.md`

No parser, grounder, engine, runtime, public README/tutorial, CI workflow,
release/package/publication, Arc08, or Arc09 surface changed in the
implementation commit.

## Reproduced Commands

New Slice02 target evidence:

- `make help`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-parser`
- `make compile-db-grounder`
- `make compile-db-engine`
- `make compile-db-first-party`

Regression evidence:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make build`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Boundary and ledger evidence:

- `git show --check 88b4e253`
- `git diff-tree --no-commit-id --name-only -r 88b4e253 -- .github README.md release tools/release docs/design-v0.3.0/arc08-pandapi-tutorial-docs docs/design-v0.3.0/arc09-release-prep-publication`
- `git show --format= --unified=0 -G 'coverage floor|release blocker coverage|fail.*warning|fail.*coverage|supported public|wolong' 88b4e253 -- Makefile mk tools`
- `rg -n -- "F-1|F-2|F-3|F-4|F-5|F-6|F-7|F-8|F-9|F-10|F-11|F-12|F-13|F-14|Bubble-up to Arc07|Slice03|remediation|split" docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/closing-report.md`

`make check` was not rerun as a single aggregate target during CDC
verification because the constituent gates were reproduced directly, including
`make build`, `make test`, and `make provenance-check`. This avoids adding
noise from the known macOS provenance mutation path while still reproducing
the behavior, static, format, safety, and provenance evidence.

## Ledger Walk

| Row | CDC status | Evidence |
|-----|------------|----------|
| F-1 | verified | `Makefile`, `mk/config.mk`, `mk/source-quality.mk`, and `tools/source-quality/surface` expose selector classes for first-party, generated, generator/template, third-party, dependency, copied build, and build output routing. |
| F-2 | verified | `make help` documents `source-quality-surface`; `make source-quality-surface` writes non-empty reports under `build/source-quality/macos-arm64/`. |
| F-3 | verified | `source-quality-surface.md` separately names parser, grounder, engine, and runtime first-party maintained roots. |
| F-4 | verified | Make/helper/report evidence encodes Flex, Bison, gengetopt, `hddl.y`, `hddl-token.l`, `options.ggo`, and generated output routing. |
| F-5 | verified | Make/helper/report evidence encodes cpddl, H2, nested third-party, and CUDD exclusions separately from first-party selectors. |
| F-6 | verified | Make/helper/report evidence records copied build roots, generated build output, and `source-map.tsv` mapping. |
| F-7 | verified | `make compile-db-parser` writes `compile_commands.json`, `first-party-selected.txt`, and `filter-report.md`; generated evidence selected 19 parser entries. |
| F-8 | verified | `make compile-db-grounder` writes `compile_commands.json`, `first-party-selected.txt`, and `filter-report.md`; generated evidence selected 22 grounder entries. |
| F-9 | verified | `make compile-db-engine` writes `compile_commands.json`, `first-party-selected.txt`, and `filter-report.md`; generated evidence selected 52 engine entries. |
| F-10 | verified | `make compile-db-first-party` runs the component compile database targets and writes aggregate first-party selected evidence. |
| F-11 | verified | `make source-quality-profile-map` writes component-specific profile/source mapping, including parser, grounder, engine, runtime, `LLVM_PROFILE_FILE`, and canonical `pandaPI/` roots. |
| F-12 | verified | Post-commit protected-path and threshold/public-surface greps were empty. |
| F-13 | verified | Safety, actionlint, static analysis, format, build, test, provenance, and whitespace checks passed. |
| F-14 | verified with amendment | CC's closing report walks all rows and includes Bubble-up to Arc07. CDC amends the next-slice recommendation: first-party source naming normalization should precede the quality gate scaffold. |

## Bubble-up

Arc07 can treat Slice02 as closed and CDC-verified. The delivered artifacts
provide the selector, compile database, and profile/source map runway needed
for broader first-party quality gates.

The next slice should be inserted as
`first-party-source-naming-normalization`. It should define and apply the
accepted lower snake case naming policy for first-party maintained C/C++ files
and directories, use `git mv` carefully for case-only changes, exclude
third-party/generated/dependency paths unless explicitly accepted, and update
Make/source-quality selectors, include paths, build files, tests, and docs
needed by the rename. The prior `first-party-quality-gate-scaffold` slice
should follow after path names are normalized.
