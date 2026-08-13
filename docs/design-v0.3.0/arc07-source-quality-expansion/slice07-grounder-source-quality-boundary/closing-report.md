# Arc07 Slice07 Closing Report: grounder-source-quality-boundary

Status: CC proposed done
Platform: macos-arm64

## Summary

Slice07 adds Make-backed grounder component source-quality targets and resolves
the `rss.c` ownership ambiguity. `rss.c` remains selected as first-party
maintained copied-origin RSS utility source, and its strict-prototype warnings
are fixed locally. Generated `cmdline.*`, cpddl, H2, nested third-party source,
copied build roots, and build output remain excluded from first-party
maintained gate interpretation.

The new maintained seam is `pandaPI/grounder/tests/top_sort_seam_test.cpp`,
which exercises `util.cpp::topsort` directly. Managed process fixtures remain
black-box contract evidence and are not counted as unit/seam coverage.

## Verification

Grounder component evidence passed:

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

Cross-surface evidence passed:

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

`make check` was not run as an aggregate. The requested constituent Make gates
above were run directly; this avoids the known macOS aggregate provenance/noise
risk while preserving the slice-specific evidence surface.

## Ledger Walk

- F-1 closed: `make help` and Make/tool greps expose `format-check-grounder`,
  `static-analysis-grounder`, `coverage-grounder`,
  `warning-inventory-grounder`, `test-unit-grounder`, and
  `sanitize-grounder`.
- F-2 closed: `make compile-db-grounder` writes an explicit
  `first-party-selected.txt`; the negative grep found no `cmdline.*`, cpddl,
  H2, nested third-party, copied build, dist, or release paths.
- F-3 closed: `make format-check-grounder` checks 45 grounder source/test
  files and records 43 inherited drift entries without rewriting broad
  inherited formatting.
- F-4 closed: `make static-analysis-grounder` runs clang-tidy/analyzer over
  selected first-party grounder TUs with exit status 0. Build-only generated
  gengetopt/cpddl/boruvka headers live under `build/source-quality`.
- F-5 closed: `make test-unit-grounder` runs the topsort maintained seam
  directly.
- F-6 closed: `make coverage-grounder` writes an 81.13% region coverage
  baseline for the topsort seam and sets no release floor.
- F-7 closed: `make warning-inventory-grounder` reports first-party maintained
  warning count 0 and keeps dependency-class warning evidence separate.
- F-8 closed: `rss.c` is first-party maintained copied-origin source; the
  warning grep found no remaining `rss.c` warnings.
- F-9 closed: generated `cmdline.c`/`cmdline.h` remain gengetopt output from
  `options.ggo`; no option-parser replacement or public CLI behavior change
  was made.
- F-10 closed: `make sanitize-grounder` runs the topsort seam under ASan/UBSan
  and records macOS LSan plus TSan/binary re-entry.
- F-11 closed: `make build-grounder` and `make test-contract-grounder-managed`
  passed; managed grounder fixtures reported 269 passed, 0 failed.
- F-12 closed: `make test-contract-pipeline-managed` passed; managed pipeline
  fixtures reported 129 passed, 0 failed.
- F-13 closed: the source-quality runway targets passed after adding
  `pandaPI/grounder/tests` to first-party classification.
- F-14 closed: safety, actionlint, static analysis, format, test, and
  provenance gates passed; whitespace checks run after staging.
- F-15 closed: no parser, engine, CI, public docs, release, dependency,
  global-threshold, or optional/fenced behavior scope was expanded.
- F-16 closed: this report walks every row and includes Bubble-up to Arc07.

## Bubble-up to Arc07

Recommended next slice: source-quality synthesis. Runtime, engine, parser
generated-warning, and grounder boundary targets now have enough Make-backed
evidence to summarize the Arc07 posture and name remaining re-entry work.

Do not start a cpddl/H2/RSS dependency audit from this slice unless CDC finds
that the classification evidence is insufficient. Do not start stricter
generated-code work until the synthesis accepts a generated-output policy.
Parser maintained-source burndown remains a valid future direction, but this
slice did not require it.
