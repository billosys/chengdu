# Arc07 Closing Report: source-quality-expansion

Status: CC proposed done
Platform evidence: macos-arm64

## Summary

Arc07 is proposed done. Slices01-07 are closed and CDC-verified; Slice08
composes their evidence in
[`source-quality-synthesis.md`](source-quality-synthesis.md). The arc corrects
the runtime-only framing by classifying parser, grounder, engine, runtime, and
Chengdu-owned generators/templates as first-party obligations by source class,
while keeping generated output, copied build roots, vendored third-party
source, dependency-internal source, and third-party generated output
separately routed.

Arc07 does not claim a global zero-debt state. It lands strict gates where the
evidence supports them, records measured baselines where thresholds are not
accepted, and routes remaining parser maintained-source, generated-code,
dependency-audit, coverage-floor, warning/static-threshold, Linux LSan, and
TSan work to explicit re-entry.

## Slice Walk

| Slice | Status | Evidence |
|-------|--------|----------|
| Slice01 | closed and CDC-verified | Source-classification inventory and gap map. |
| Slice02 | closed and CDC-verified | Source-quality selectors, compile databases, profile/source mapping, generated/dependency exclusions. |
| Slice03 | closed and CDC-verified | Lower snake case first-party maintained C/C++ file and directory naming policy and Make check. |
| Slice04 | closed and CDC-verified | First-party quality gate scaffold and re-entry map. |
| Slice05 | closed and CDC-verified | Engine component targets, visited_list seam, warning closure, and reported baselines. |
| Slice06 | closed and CDC-verified | Parser Flex generated-warning root cause and scanner-input fix; gengetopt re-entry preserved. |
| Slice07 | closed and CDC-verified | Grounder component targets, `rss.c` copied-origin disposition, topsort seam, and dependency-boundary evidence. |
| Slice08 | CC proposed done | Final synthesis, release posture, Arc08/Arc09 handoff, and this arc close report. |

## Arc Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| A1 | done | Source classes are recorded in `source-classification-inventory.md`, encoded through Slice02 source-quality surfaces, and composed in `source-quality-synthesis.md`. |
| A2 | done | The arc plan and synthesis use first-party maintained/generated/generator/template, vendored third-party, dependency-internal, copied build, and generated build-output language. |
| A3 | done | The synthesis component matrix records Make-backed format, static-analysis, coverage, unit/seam, warning, and sanitizer policy for parser, grounder, engine, and runtime, including explicit parser re-entry. |
| A4 | done | Generated-warning policy is root-cause/correctness/remedy/re-entry based; Slice06 closes parser Flex warnings and preserves gengetopt/CLI11 re-entry. |
| A5 | done | cpddl, H2, boruvka, opts, lpsolve, CUDD, third-party generated output, copied build roots, and build output are excluded or separately reported; `rss.c` is first-party maintained copied-origin today. |
| A6 | done | Managed fixtures remain black-box process-contract proof; runtime CTest, engine visited_list, and grounder topsort are internal seam evidence. |
| A7 | done | Slice02 restructuring landed before Slice04+ source-quality claims relied on selectors, compile databases, or profile/source maps. |
| A8 | done | Release blocker policy is explicit in the synthesis: strict gates, measured baselines, post-0.3.0 budgets, and out-of-first-party-scope boundaries are separated. |
| A9 | done | Arc08 and Arc09 handoffs are concrete and do not edit public docs, release tooling, wolong paths, or package surfaces in this arc close. |

## Verification

Reproduced in Slice08 before edits:

- Source-quality runway: `make source-quality-surface`,
  `make source-quality-profile-map`, `make source-quality-naming-check`,
  `make compile-db-first-party`.
- Warning/generated evidence: `make warning-inventory-first-party`
  (`Warning lines: 50`), `make generated-warning-triage-parser`,
  `make generated-warning-triage`.
- Coverage/unit/sanitizer evidence: `make coverage`, `make coverage-engine`,
  `make coverage-grounder`, `make test-unit`, `make test-unit-engine`,
  `make test-unit-grounder`, `make test-runtime-sanitize`,
  `make test-binary-sanitize`, `make sanitize-engine`,
  `make sanitize-grounder`.
- Static/format evidence: `make static-analysis`, `make format-check`,
  `make format-check-engine`, `make static-analysis-engine`,
  `make format-check-grounder`, `make static-analysis-grounder`.
- Regression evidence: `make safety-checks`, `make actionlint`, `make test`,
  `make provenance-check`.

`make test` passed runtime CTest 5/0, baseline fixtures 38/0, managed parser
298/0, managed grounder 269/0, managed engine 312/0, managed pipeline 129/0,
positive smoke 3/0, and negative smoke 4/0. `make test-binary-sanitize` passed
managed fixtures 1008/0. `make check` was not run as the aggregate because on
macOS it can append minimum-OS provenance observations and create known
provenance mutation/noise; the constituent gates above were reproduced
directly.

## Bubble-up to the project

Arc07 can be marked CC proposed done, awaiting CDC verification. Arc08 may use
the synthesis for public tutorial/docs language, especially source-class
posture, canonical `pandapi-*` behavior, generated-code policy, and dependency
boundaries. Arc09 may use it for release-prep gate selection, but should not
invent new global coverage floors, global zero-warning policy, global
`-Werror`, CLI11 migration, or dependency-audit obligations without a new
ledger.
