# Arc07 Slice08 Closing Report: source-quality-synthesis

Status: CC proposed done
Platform evidence: macos-arm64

## Summary

Slice08 creates the final Arc07 source-quality synthesis and proposes Arc07
closure. The synthesis separates strict release blocker evidence, accepted
measured baselines, accepted deferred/post-0.3.0 budgets, and out-of-first-party
scope dependency boundaries. It does not change implementation source, Make/CI,
public docs, release tooling, wolong paths, dependency pins, CLI11 migration,
or global thresholds.

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | closed | `source-quality-synthesis.md` links Slice01 through Slice07 closing reports and CDC verification files. |
| F-2 | closed | The synthesis walks A1 through A9 and marks every row satisfied; no blocked row remains. |
| F-3 | closed | The synthesis records first-party maintained, first-party generated, first-party generator/template, vendored third-party, dependency-internal, third-party generated, copied build artifact, generated build output, `rss.c`, and copied-origin policy. |
| F-4 | closed | The component matrix covers parser, grounder, engine, and runtime across format, static analysis, coverage, unit/seam, warning, sanitizer, generated-warning, and process-contract evidence. |
| F-5 | closed | The release posture separates release blocker evidence, measured baseline, deferred/post-0.3.0 budget, out of first-party scope, and re-entry items. |
| F-6 | closed | `make warning-inventory-first-party` passed with `Warning lines: 50`; warning policy is reconciled by first-party maintained warning, generated warning, dependency, third-party, aggregate, and release posture. |
| F-7 | closed | `make static-analysis`, `make format-check`, `make format-check-engine`, `make static-analysis-engine`, `make format-check-grounder`, and `make static-analysis-grounder` passed or recorded accepted baselines; the synthesis names drift/finding counts and avoids blanket suppression claims. |
| F-8 | closed | `make coverage`, `make coverage-engine`, and `make coverage-grounder` passed; the synthesis records runtime, engine, and grounder baselines and says no release floor is accepted. |
| F-9 | closed | `make test-unit`, `make test-unit-engine`, and `make test-unit-grounder` passed; the synthesis distinguishes CTest/direct seams from black-box process-contract fixtures and names parser remaining internal-test work. |
| F-10 | closed | `make test-runtime-sanitize`, `make test-binary-sanitize`, `make sanitize-engine`, and `make sanitize-grounder` passed; the synthesis records ASan, UBSan, LSan, TSan, binary sanitizer, and re-entry posture. |
| F-11 | closed | `make generated-warning-triage-parser` and `make generated-warning-triage` passed; Flex, gengetopt, `options.ggo`, `cmdline.*`, CLI11, third-party skeleton, root cause, correctness, chosen remedy, and re-entry are reconciled. |
| F-12 | closed | Dependency-boundary posture is explicit for cpddl, H2, h2-fd-preprocessor, boruvka, opts, lpsolve, CUDD, RSS, dependency audit, out-of-first-party scope, and re-entry. |
| F-13 | closed | Arc08 and Arc09 handoffs name tutorial, README/public docs, release prep, publication, wolong, source-quality release gates, and handoff responsibilities without editing those surfaces. |
| F-14 | closed | Arc07 closing report exists and walks A1-A9, Slice01-Slice08, Bubble-up to the project, and CC proposed done. |
| F-15 | closed | Source-quality runway, safety, actionlint, static-analysis, format, test, provenance, and whitespace gates were reproduced directly; `make check` aggregate was skipped for the macOS provenance/noise reason. |
| F-16 | closed | Staged boundary checks passed: no implementation source, CI workflow, public README/tutorial/release/wolong, dependency pin, CLI11 migration, global threshold, Make, tools, fixtures, or tests scope is included. |
| F-17 | closed | This closing report walks F-1 through F-17 and includes Bubble-up to Arc07 with arc closure. |

## Verification

Commands run before writing docs:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-first-party`
- `make generated-warning-triage-parser`
- `make generated-warning-triage`
- `make warning-inventory-first-party`
- `make coverage`
- `make coverage-engine`
- `make coverage-grounder`
- `make test-unit`
- `make test-unit-engine`
- `make test-unit-grounder`
- `make test-runtime-sanitize`
- `make test-binary-sanitize`
- `make sanitize-engine`
- `make sanitize-grounder`
- `make static-analysis`
- `make format-check`
- `make format-check-engine`
- `make static-analysis-engine`
- `make format-check-grounder`
- `make static-analysis-grounder`
- `make safety-checks`
- `make actionlint`
- `make test`
- `make provenance-check`

Observed evidence:

- Warning inventory: `Warning lines: 50`.
- Runtime coverage: 74.09% line coverage.
- Engine coverage: 66.67% line coverage.
- Grounder coverage: 63.79% line coverage and 81.13% region coverage.
- Binary sanitizer managed fixtures: 1008 passed, 0 failed.
- Full `make test`: runtime CTest 5/0, baseline 38/0, managed parser 298/0,
  managed grounder 269/0, managed engine 312/0, managed pipeline 129/0,
  positive smoke 3/0, negative smoke 4/0.
- Provenance check: all three components verified on macos-arm64.

`make check` was not run as an aggregate. The required constituent gates were
run directly because the macOS aggregate can append minimum-OS provenance
observations and create known provenance mutation/noise.

## Bubble-up to Arc07

Arc07 can close as CC proposed done. The arc-level closing report has been
written, and no next required slice is needed before CDC review. Future work is
explicit re-entry: parser maintained-source burndown, gengetopt/CLI11
replacement, strict generated-code policy, dependency audits, coverage floors,
warning/static thresholds, Linux LSan, and TSan.
