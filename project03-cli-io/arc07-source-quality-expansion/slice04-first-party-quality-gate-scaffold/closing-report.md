# Arc07 Slice04 Closing Report: first-party-quality-gate-scaffold

Status: CC proposed done
Date: 2026-08-12
Branch: `release/0.3.x`

## Scope Delivered

Slice04 adds Make-backed first-party source-quality scaffold targets and a
durable review report without changing parser, grounder, engine, runtime,
public CLI behavior, CI workflows, release tooling, README/tutorial docs,
dependencies, warning budgets, static thresholds, or coverage floors.

Delivered targets:

- `source-quality-gate-report`
- `format-check-first-party`
- `static-analysis-first-party`
- `coverage-first-party`
- `warning-inventory-first-party`
- `generated-warning-triage`
- `test-unit`
- `sanitize-first-party`

Durable report:

- `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | `make help` lists every scaffold target; `rg` found the target wiring in Make-backed surfaces. |
| F-2 | done | `source-quality-gate-scaffold.md` exists and contains format, static analysis, coverage, unit, warning, generated-warning, sanitizer, parser, grounder, engine, runtime, source class, and re-entry terms. |
| F-3 | done | The scaffold reuses `SOURCE_QUALITY`, `source-quality`, `compile-db-first-party`, `source-quality-profile-map`, `source-quality-naming-check`, first-party maintained, `GENERATED`, `THIRD_PARTY`, `DEPENDENCY`, `COPIED_BUILD`, and `BUILD_OUTPUT` surfaces. |
| F-4 | done | `make format-check-first-party` passed. Runtime format runs the existing real gate; parser, grounder, and engine rows record component re-entry and generated/third-party/dependency exclusions. |
| F-5 | done | `make static-analysis-first-party` passed with runtime `clang-tidy` 20.1.6 and component compile database evidence. No blanket suppression was added. |
| F-6 | done | `make coverage-first-party` passed and printed the runtime coverage summary. The report records parser, grounder, and engine baseline placement and states no release floor. |
| F-7 | done | `make test-unit` passed with 5 runtime CTest tests. The report keeps CTest/Catch2 seam posture separate from process contract fixture evidence. |
| F-8 | done | `make warning-inventory-first-party` passed and captured 56 warning lines, preserving generated/dependency/third-party budgets and first-party warning re-entry. |
| F-9 | done | `make generated-warning-triage` passed, and the durable report records generated file, generator, skeleton, ownership class, root cause, correctness, chosen remedy, and re-entry fields. |
| F-10 | done | `make sanitize-first-party` passed with 5 runtime ASan/UBSan CTest tests and reports binary sanitizer source-class re-entry for parser, grounder, and engine. |
| F-11 | done | The durable report records `make test-corpus CORPUS_DIR=...` as optional IPC external corpus smoke, not unit coverage or a release-grade benchmark gate. |
| F-12 | done | `make source-quality-surface && make source-quality-profile-map && make source-quality-naming-check && make compile-db-first-party` passed. |
| F-13 | done | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` passed. Aggregate `make check` was not run because on macOS it includes `record-min-os-if-macos`, which appends provenance observations and can introduce known provenance mutation/noise; the row's constituent gates were reproduced directly. |
| F-14 | done | The cached boundary check passed after staging: no `.github`, README, release, `tools/release`, Arc08, Arc09, dependency, threshold, or public-surface expansion was staged. |
| F-15 | done | This closing report walks F-1 through F-15 and includes Bubble-up to Arc07 with Slice05 component order and split/remediation disposition. |

## Verification

New scaffold targets:

- `make help` passed.
- `make source-quality-gate-report` passed.
- `make format-check-first-party` passed.
- `make static-analysis-first-party` passed.
- `make coverage-first-party` passed.
- `make warning-inventory-first-party` passed.
- `make generated-warning-triage` passed.
- `make test-unit` passed.
- `make sanitize-first-party` passed.

Existing source-quality runway:

- `make source-quality-surface` passed.
- `make source-quality-profile-map` passed.
- `make source-quality-naming-check` passed.
- `make compile-db-first-party` passed.

Regression gates:

- `make safety-checks` passed.
- `make actionlint` passed.
- `make static-analysis` passed.
- `make format-check` passed.
- `make test` passed.
- `make provenance-check` passed.
- `git diff --check` passed.
- `git diff --cached --check` passed.

`make check` was not run as an aggregate because the macOS aggregate includes
`record-min-os-if-macos`, which appends minimum-OS provenance observations and
can create known mutation/noise. The narrower Make-backed gates above are the
constituents needed for this slice and were reproduced directly.

## Bubble-up to Arc07

Recommended Slice05 component order:

1. Engine first-party maintained source. Start here because the remaining
   primary warning budget is narrow, source-owned, and tied to
   `visited_list`/search/data-structure seams that will unlock safer warning
   and sanitizer remediation.
2. Parser first-party maintained plus generated-warning triage. Parser primary
   warnings are clear, but Flex/gengetopt generated-warning policy needs
   generator/skeleton decisions before strict enforcement.
3. Grounder first-party maintained source. Primary warnings are clear, but
   cpddl/H2/dependency noise makes source-class routing the main risk.
4. Runtime follow-up only where needed. Runtime already has executable format,
   static-analysis, coverage, unit, and sanitizer gates; revisit for component
   target splits or integration friction found by the first component slices.

No required split/remediation was found before Slice05. The next slice should
convert one report-backed component gate family into executable evidence at a
time rather than turning all scaffold rows into enforcement in one step.
