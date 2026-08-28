# Arc07 Source-Quality Synthesis

Status: Slice08 implementation report
Platform evidence: macos-arm64

## Verdict

Arc07 composes. The completed slices classify parser, grounder, engine,
runtime, generators/templates, generated output, copied build roots, build
output, vendored third-party source, dependency-internal source, and
third-party generated output. The arc now has explicit first-party
source-quality policy and Make-backed evidence without claiming a global
zero-debt state.

Arc07 can be proposed closed. The remaining work is classified below as strict
release blocker evidence, accepted measured baseline, accepted deferred or
post-0.3.0 budget, or out of first-party scope unless a later dependency audit
accepts ownership.

## Child Evidence

| Slice | Evidence |
|-------|----------|
| Slice01 source-classification-inventory | [`source-classification-inventory.md`](source-classification-inventory.md), [`slice01-source-classification-inventory/closing-report.md`](slice01-source-classification-inventory/closing-report.md), [`slice01-source-classification-inventory/cdc-verification.md`](slice01-source-classification-inventory/cdc-verification.md) |
| Slice02 source-layout-and-build-surface-normalization | [`slice02-source-layout-and-build-surface-normalization/closing-report.md`](slice02-source-layout-and-build-surface-normalization/closing-report.md), [`slice02-source-layout-and-build-surface-normalization/cdc-verification.md`](slice02-source-layout-and-build-surface-normalization/cdc-verification.md) |
| Slice03 first-party-source-naming-normalization | [`source-naming-normalization.md`](source-naming-normalization.md), [`slice03-first-party-source-naming-normalization/closing-report.md`](slice03-first-party-source-naming-normalization/closing-report.md), [`slice03-first-party-source-naming-normalization/cdc-verification.md`](slice03-first-party-source-naming-normalization/cdc-verification.md) |
| Slice04 first-party-quality-gate-scaffold | [`source-quality-gate-scaffold.md`](source-quality-gate-scaffold.md), [`slice04-first-party-quality-gate-scaffold/closing-report.md`](slice04-first-party-quality-gate-scaffold/closing-report.md), [`slice04-first-party-quality-gate-scaffold/cdc-verification.md`](slice04-first-party-quality-gate-scaffold/cdc-verification.md) |
| Slice05 engine-first-party-quality-burndown | [`slice05-engine-first-party-quality-burndown/closing-report.md`](slice05-engine-first-party-quality-burndown/closing-report.md), [`slice05-engine-first-party-quality-burndown/cdc-verification.md`](slice05-engine-first-party-quality-burndown/cdc-verification.md) |
| Slice06 parser-generated-warning-root-cause | [`parser-generated-warning-root-cause.md`](parser-generated-warning-root-cause.md), [`slice06-parser-generated-warning-root-cause/closing-report.md`](slice06-parser-generated-warning-root-cause/closing-report.md), [`slice06-parser-generated-warning-root-cause/cdc-verification.md`](slice06-parser-generated-warning-root-cause/cdc-verification.md) |
| Slice07 grounder-source-quality-boundary | [`slice07-grounder-source-quality-boundary/closing-report.md`](slice07-grounder-source-quality-boundary/closing-report.md), [`slice07-grounder-source-quality-boundary/cdc-verification.md`](slice07-grounder-source-quality-boundary/cdc-verification.md) |

## Arc Ledger Reconciliation

| Row | Status | Synthesis |
|-----|--------|-----------|
| A1 | satisfied | Slice01 classified the source tree; Slice02 encoded selectors and compile/profile maps; Slice07 resolved the `rss.c` copied-origin ambiguity. No blocked path class remains unnamed. |
| A2 | satisfied | Active policy now uses source-class language, not ambiguous "runtime-owned versus inherited planner" language. |
| A3 | satisfied | Runtime has strict executable format, static-analysis, coverage, unit, warning, and sanitizer gates. Engine and grounder have Make-backed component evidence and baselines. Parser has Make-backed selectors plus generated-warning closure, with maintained-source burndown classified as post-0.3.0 re-entry rather than a silent drop. |
| A4 | satisfied | Flex, Bison, gengetopt, `options.ggo`, scanner grammar input, generated `cmdline.*`, and generated skeleton boundaries are routed through root cause, correctness, chosen remedy, and re-entry evidence. |
| A5 | satisfied | cpddl, H2, boruvka, opts, lpsolve, CUDD, third-party generated output, copied build artifacts, and build output are excluded from first-party obligations or separately reported with rationale. |
| A6 | satisfied | Managed `pandapi-*` process fixtures remain black-box contract evidence. Runtime CTest, engine `visited_list`, and grounder `topsort` are internal unit/seam evidence. |
| A7 | satisfied | Slice02 selectors, compile databases, and profile/source maps landed before Slice04+ gate scaffolding and component baseline claims. |
| A8 | satisfied | Release posture is explicit: strict gates are named, coverage is measured baseline with no release floor, aggregate warning count is evidence not a global threshold, and sanitizer evidence is source-class triaged. No silent drop remains. |
| A9 | satisfied | Arc08 and Arc09 handoffs below name public-doc and release-prep responsibilities without editing those surfaces in this slice. |

No A-row is blocked.

## Source-Class Policy

| Class | Final Arc07 posture |
|-------|---------------------|
| first-party maintained | Parser, grounder, engine, runtime, first-party adapters, build/test hooks, and maintained copied-origin code such as `rss.c` are Chengdu source-quality obligations. Gates may be strict, measured baselines, or scoped re-entry depending on component evidence. |
| first-party generated | Generated output from Chengdu-maintained inputs is not hand-edited. Defects are fixed at the generator/template or input when the finding belongs there. |
| first-party generator/template | `hddl.y`, `hddl-token.l`, `options.ggo`, Make/CMake inputs, and wrapper logic are reviewable source. Slice06 fixed Flex helper warnings through `hddl-token.l`. |
| vendored third-party | cpddl, H2, nested boruvka, opts, lpsolve, and CUDD stay out of first-party format/static-analysis/coverage/unit obligations unless a dependency audit accepts the boundary. |
| dependency-internal | Dependency code compiled by a first-party integration is separately reported; findings are not first-party release blockers by default. |
| third-party generated | Third-party Flex/Bison or skeleton output inside dependencies is separately reported and requires dependency-audit re-entry before cleanup obligations. |
| copied build artifact | `build/<platform>/source/*` is evidence/copy volume only; diagnostics map back to canonical `pandaPI/<component>` roots before policy decisions. |
| generated build output | `build/`, `dist/`, coverage, sanitizer, CMake, CTest, and release output are evidence, not source-quality owners. |

## Component Gate Matrix

| Gate | Parser | Grounder | Engine | Runtime |
|------|--------|----------|--------|---------|
| format | Selector/report-backed; maintained-source strict formatting remains post-0.3.0 re-entry. | `format-check-grounder` executable baseline, 43 drift entries, no broad rewrite. | `format-check-engine` executable baseline, 126 drift entries, no broad rewrite. | `format-check` strict; 25 owned files passed. |
| static analysis | Compile database exists; strict parser clang-tidy waits for generated-output and maintained-source burndown policy. | `static-analysis-grounder` passed with clang-tidy exit status 0. | `static-analysis-engine` reported clang-tidy status 1 and 98 finding lines as accepted baseline. | `static-analysis` strict; 14 runtime TUs passed clang-analyzer checks. |
| coverage | No maintained-source parser baseline accepted yet; re-entry requires unit/seam or mapped managed profiles. | `coverage-grounder` topsort seam baseline: 81.13% region, 63.79% line, no release floor. | `coverage-engine` visited_list seam baseline: 66.67% line, no release floor. | `make coverage` runtime baseline: 74.09% line, no release floor. |
| unit/seam | Managed fixtures are black-box only; parser internal unit/seam work remains re-entry. | `test-unit-grounder` direct topsort seam passed; CTest/Catch2 not required for that seam. | `test-unit-engine` CTest visited_list payload seam passed. | `test-unit` runs runtime CTest: 5 passed, 0 failed. |
| warning | Primary parser warnings clear; Flex generated `yyunput`/`yyinput` warnings closed; gengetopt remains re-entry. | First-party maintained warning count 0; dependency/third-party warning budget separate. | Primary visited_list warning closed; duplicate link-library noise not observed; other analyzer/format debt remains baseline. | Runtime warning posture remains strict through current gates. |
| sanitizer | Binary ASan/UBSan managed fixtures cover parser process behavior; parser internal sanitizer seam remains re-entry. | `sanitize-grounder` topsort seam passed; binary sanitizer source-class triage remains release evidence. | `sanitize-engine` visited_list seam passed; binary sanitizer source-class triage remains release evidence. | `test-runtime-sanitize` passed 5 runtime ASan/UBSan tests. |
| generated-warning | Flex warning budget closed through scanner input; gengetopt/CLI11 remains re-entry. | `cmdline.*` gengetopt output and dependency generated warnings are separately routed. | `cmdline.*` gengetopt output routed to option-source or CLI11 re-entry. | Not a generated-output component. |
| process-contract | Managed parser fixtures passed 298/0 in the reproduced `make test` run. | Managed grounder fixtures passed 269/0. | Managed engine fixtures passed 312/0. | Runtime CTest plus binary fixture harness support; managed pipeline passed 129/0. |

## Release Posture

### Strict release blocker evidence

- `make source-quality-surface`, `make source-quality-profile-map`,
  `make source-quality-naming-check`, and `make compile-db-first-party` must
  continue to pass before source-quality release signoff.
- `make warning-inventory-first-party` must pass and must not reintroduce
  closed first-party warning items. Current reproduced aggregate evidence is
  `Warning lines: 50`; that count is evidence, not a global failure threshold.
- Runtime strict gates remain `make static-analysis`, `make format-check`,
  `make test-unit`, `make test-runtime-sanitize`, and `make coverage` as
  measured coverage evidence.
- Component seam gates that now exist must continue to pass when those
  surfaces change: `make test-unit-engine`, `make sanitize-engine`,
  `make test-unit-grounder`, and `make sanitize-grounder`.
- Behavior and release-candidate source-quality verification must include
  `make test`, `make provenance-check`, `make safety-checks`, and
  `make actionlint`.
- Binary ASan/UBSan evidence is a pre-release gate through
  `make test-binary-sanitize`; the reproduced run passed managed fixtures
  1008/0.

### Accepted measured baselines

- Runtime coverage: 74.09% line coverage, no release floor.
- Engine coverage: visited_list seam total 66.67% line coverage, no release
  floor.
- Grounder coverage: topsort seam total 63.79% line and 81.13% region
  coverage, no release floor.
- Engine format baseline: 126 drift entries.
- Grounder format baseline: 43 drift entries.
- Engine static-analysis baseline: clang-tidy status 1 with 98 finding lines.
- Grounder static-analysis baseline: clang-tidy status 0.
- Aggregate warning inventory: 50 warning lines, with dependency/third-party
  warning classes separately reported.

### Accepted deferred/post-0.3.0 budgets

- Parser maintained-source component burndown: strict parser formatting,
  parser clang-tidy, parser coverage baseline, parser unit/seam tests, and
  parser internal sanitizer seam remain re-entry work. Reason: Arc07 closed
  parser generated warnings and established selectors, but did not accept a
  maintained-source parser burndown slice.
- Parser, grounder, and engine gengetopt replacement or CLI11 migration remain
  re-entry. Reason: generated human diagnostic prose is not final
  managed-process contract evidence, and Arc07 did not perform option-parser
  replacement.
- Stricter generated-code policy remains re-entry. Reason: Arc07 requires root
  cause and correctness triage before suppression, strict enforcement, or
  generator/template fixes.
- Global coverage floors, global warning fail budgets, global `-Werror`, and
  global static-analysis thresholds remain re-entry. Reason: current evidence
  contains measured baselines and component footholds, not release floors.
- Catch2 pinning remains re-entry. Reason: runtime and engine use CTest-style
  evidence, grounder uses a direct seam test, and no slice accepted a pinned
  test dependency.
- Linux LeakSanitizer remains Arc09/pre-release re-entry. macOS runs use
  `detect_leaks=0`; release prep should reproduce binary sanitizer on a
  LeakSanitizer-capable Linux/toolchain with `detect_leaks=1`.
- TSan remains deferred. Re-entry requires accepted supported multithreaded
  Chengdu-owned process observation, concurrent child-process stream draining,
  or promoted supported planner concurrency.

### Out of first-party scope unless audited

- cpddl and H2 remain dependency-internal/vendored third-party surfaces.
- boruvka, opts, and lpsolve remain nested third-party or third-party
  generated dependency surfaces.
- CUDD remains vendored third-party and is fenced with BDD/CUDD future-surface
  behavior.
- `rss.c` is not out of scope today. It is first-party maintained
  copied-origin RSS utility source for grounder gates until a later dependency
  audit changes that boundary.

## Generated-Code Policy

Generated-code closure requires root cause, correctness impact, ownership,
chosen remedy, and re-entry evidence. Slice06 is the accepted example:
`hddl-token.cpp` Flex helper warnings were root-caused to unused skeleton
helpers, the scanner rules were checked for no `input()` or `unput()` use, and
the owned `hddl-token.l` input gained `%option noinput` and `%option nounput`.

`options.ggo` and generated `cmdline.c`/`cmdline.h` for parser, grounder, and
engine are separate from that Flex closure. The proper re-entry is option
source cleanup or the accepted CLI11 migration path, not hand-editing generated
output or treating gengetopt human prose as final managed-process contract
evidence. Third-party skeleton output inside dependencies remains a
dependency-boundary issue.

## Arc08 Handoff

Arc08 public docs and tutorial work should describe:

- all in-tree pandaPI product source as Chengdu-owned in the fork/product
  sense;
- source-quality obligations by source class, not by old runtime-versus-planner
  wording;
- canonical `pandapi-*` behavior as proven by managed black-box fixtures;
- generated-code policy in user-facing terms: maintain generator inputs,
  avoid hand-editing generated output, and avoid treating generated diagnostic
  prose as managed-process contract evidence;
- third-party/dependency boundaries for cpddl, H2, nested dependencies, and
  CUDD without claiming Chengdu cleanup obligations that Arc07 did not accept;
- remaining budgets as known engineering posture, not hidden defects.

Arc08 should not turn coverage baselines into public quality guarantees or
claim global zero-warning status.

## Arc09 Handoff

Arc09 release prep should verify:

- `make source-quality-surface`, `make source-quality-profile-map`,
  `make source-quality-naming-check`, `make compile-db-first-party`,
  `make warning-inventory-first-party`, `make generated-warning-triage-parser`,
  and `make generated-warning-triage`;
- runtime, engine, and grounder unit/seam and sanitizer evidence;
- `make coverage`, `make coverage-engine`, and `make coverage-grounder` as
  measured baselines, not floors;
- `make test`, `make provenance-check`, `make safety-checks`, `make actionlint`,
  `make static-analysis`, and `make format-check`;
- `make test-binary-sanitize` before publication, plus Linux LSan re-entry
  where available;
- package assets, checksums, manifests, license/NOTICE obligations, test-only
  dependency exclusion, wolong fetch/install/migration, and publication
  idempotency.

Arc09 should enforce only the thresholds Arc07 actually landed: strict runtime
format/static-analysis and passing source-quality/behavior/sanitizer gates,
not a new global coverage floor or global zero-warning policy.

## Verification Snapshot

Reproduced for Slice08:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-first-party`
- `make generated-warning-triage-parser`
- `make generated-warning-triage`
- `make warning-inventory-first-party` (`Warning lines: 50`)
- `make coverage` (runtime line coverage 74.09%)
- `make coverage-engine` (engine line coverage 66.67%)
- `make coverage-grounder` (grounder line coverage 63.79%, region coverage
  81.13%)
- `make test-unit`
- `make test-unit-engine`
- `make test-unit-grounder`
- `make test-runtime-sanitize`
- `make test-binary-sanitize` (managed fixtures 1008/0)
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

`make check` was not run as the aggregate because on macOS the aggregate can
append minimum-OS provenance observations and create known provenance
mutation/noise. The constituent behavior, source-quality, static, format,
safety, actionlint, provenance, sanitizer, coverage, and whitespace gates were
run directly.
