# Arc07 Plan: source-quality-expansion

Status: active
Opened: 2026-08-12

## 1. Capability

Arc07 expands source-quality gates from the initial Arc06 runtime-first runway
to the whole first-party maintained pandaPI fork. For `v0.3.0`, parser,
grounder, engine, runtime, and any Chengdu-maintained generators/templates are
all Chengdu-owned product source. Public contract tests remain black-box at
the `pandapi-*` CLI boundary, but unit tests, formatting, static analysis,
coverage, warning policy, and sanitizer triage must cover the code we write
or maintain, not only the shared runtime substrate.

This arc does not make third-party code a Chengdu testing obligation.
Vendored third-party source, third-party generated output, nested dependency
internals, and copied build artifacts must be identified, excluded, or reported
separately. If generated code comes from Chengdu-owned generators or templates,
the generator/template is the maintenance target; warning or formatting
problems should be fixed there rather than hidden in generated output. Generated
warning cleanup must begin with root-cause and correctness analysis: identify
why the generated code emits the warning, whether the generated code is
semantically correct, which generator/template/skeleton owns the emitted code,
and whether the proper remedy is a generator/template fix, an input grammar or
option change, a compiler-flag boundary, or a documented third-party exclusion.

## 2. Policy Corrections

Arc06 closed correctly for the policy in force at the time, but its wording
can imply that only `pandaPI/runtime` is Chengdu-owned. Arc07 supersedes that
ambiguity:

1. All in-tree pandaPI product code is Chengdu-owned in the fork/product
   sense.
2. Quality gates distinguish source class, not ownership of the release:
   first-party maintained source, first-party generated source,
   first-party generators/templates, vendored third-party source,
   third-party generated source, and copied build artifacts.
3. First-party maintained parser, grounder, engine, and runtime code require
   formatting, static analysis, coverage, unit-test strategy, warning policy,
   and sanitizer triage.
4. Third-party code is excluded from Chengdu coverage/static-analysis/unit-test
   obligations unless a later dependency audit explicitly takes ownership of a
   boundary.
5. Public integration/contract tests stay focused on accepted APIs and process
   behavior; unit tests may and should exercise internal first-party code.
6. Generated-warning triage must classify each warning by generated file,
   generator or skeleton, source-class ownership, root cause, correctness
   impact, chosen remedy, and re-entry condition before it is closed,
   suppressed, or deferred.

## 3. Slice Breakdown

| Slice | Slug | Scope | Load-bearing for |
|-------|------|-------|------------------|
| slice01 | `source-classification-inventory` | Classify every source/build/generated path by source class; map existing gates and gaps; produce recommendations for restructuring, target shape, test framework/dependency posture, and likely slice order. | all later Arc07 work |
| slice02 | `source-layout-and-build-surface-normalization` | Conditional restructuring slice. Apply only the path/build/compile-database/profile-isolation changes accepted after Slice01 review, so first-party parser/grounder/engine files can be analyzed without third-party/generated noise. | coverage/static-analysis expansion |
| slice03 | `first-party-source-naming-normalization` | Closed and CDC-verified. Normalized first-party maintained C/C++ file and directory names to the accepted lower snake case policy before gate selectors, compile databases, coverage maps, and static-analysis paths become enforcement surfaces. Excluded third-party, dependency-internal, and generated paths unless a ledger row explicitly accepts them. | stable source-quality selectors |
| slice04 | `first-party-quality-gate-scaffold` | Closed and CDC-verified. Added Make-backed aggregate scaffold targets and durable status/re-entry evidence for first-party format, static analysis, coverage, unit tests, warning policy, sanitizer policy, optional IPC corpus placement, and generated-warning triage without forcing premature thresholds. | component quality slices |
| slice05 | `engine-first-party-quality-burndown` | Closed and CDC-verified. Converted Slice04 engine report-backed rows into Make-backed executable engine component gates; added focused `visited_list` seam evidence; closed the primary engine warning and duplicate link-library noise; preserved CUDD/generated/copy/build-output exclusions and public behavior. | parser/grounder component burndown |
| slice06 | `parser-generated-warning-root-cause` | Closed and CDC-verified. Made parser generated-warning evidence component-specific, root-caused Flex scanner helper warnings, fixed them through the owned scanner input, and dispositioned parser gengetopt output without CLI11 migration or public behavior change. | parser/grounder component burndown |
| slice07 | `grounder-source-quality-boundary` | Closed and CDC-verified. Converted grounder scaffold rows into Make-backed component gates; made the first-party/dependency boundary executable; resolved `rss.c` as first-party maintained copied-origin source with warnings fixed; kept cpddl, H2, nested third-party, generated, copied-build, and build-output surfaces separately reported. | release readiness |
| slice08 | `source-quality-synthesis` | Open. Compose the final source-quality evidence, thresholds, release blockers, remaining budgets, and Arc08/Arc09 handoff; propose Arc07 closure only if the completed slices honestly satisfy A1-A9. | docs and release prep |
| slice09+ | `component-source-quality-burndown` | Future re-entry work after synthesis. Expected later work may include parser maintained-source burndown, parser/grounder gengetopt and CLI11 replacement, stricter generated-code policy, dependency audits where accepted, coverage floors, or warning/static-analysis thresholds. | release readiness |

Slice01 is intentionally the only fully-opened slice at arc start. Its report
may amend this breakdown before Slice02 opens.

## 4. Dependencies And Inputs

Consumes:

- Arc01 parser, grounder, engine audit reports and synthesis.
- Arc02 dependency and library research, especially dependency-boundary
  decisions.
- Arc03 supported-surface and status/process contract decisions.
- Arc04 shared runtime substrate and current CMake/CTest shape.
- Arc05 canonical native `pandapi-*` binary adoption.
- Arc06 fixture, coverage, static-analysis, warning, sanitizer, and TSan
  evidence.

Leaves for later arcs:

- Arc08 tutorial/docs should explain the final source classes and dependency
  posture without exposing private planning ambiguity.
- Arc09 release prep should enforce only the source-quality thresholds Arc07
  actually lands or explicitly marks as release blockers.

## 5. Current Status

- **slice01 source-classification-inventory - closed and CDC-verified.**
  Slice set:
  [`slice01-source-classification-inventory/slice-doc.md`](slice01-source-classification-inventory/slice-doc.md),
  [`slice01-source-classification-inventory/ledger.md`](slice01-source-classification-inventory/ledger.md),
  [`slice01-source-classification-inventory/cc-prompt.md`](slice01-source-classification-inventory/cc-prompt.md),
  [`slice01-source-classification-inventory/closing-report.md`](slice01-source-classification-inventory/closing-report.md),
  [`slice01-source-classification-inventory/cdc-verification.md`](slice01-source-classification-inventory/cdc-verification.md).
  Source classification report:
  [`source-classification-inventory.md`](source-classification-inventory.md).
- **slice02 source-layout-and-build-surface-normalization - closed and
  CDC-verified.** Slice set:
  [`slice02-source-layout-and-build-surface-normalization/slice-doc.md`](slice02-source-layout-and-build-surface-normalization/slice-doc.md),
  [`slice02-source-layout-and-build-surface-normalization/ledger.md`](slice02-source-layout-and-build-surface-normalization/ledger.md),
  [`slice02-source-layout-and-build-surface-normalization/cc-prompt.md`](slice02-source-layout-and-build-surface-normalization/cc-prompt.md),
  [`slice02-source-layout-and-build-surface-normalization/closing-report.md`](slice02-source-layout-and-build-surface-normalization/closing-report.md),
  [`slice02-source-layout-and-build-surface-normalization/cdc-verification.md`](slice02-source-layout-and-build-surface-normalization/cdc-verification.md).
  Slice02 delivered Make-backed selectors, compile databases, profile/source
  mapping, generated-code policy, copied-build mapping, and
  third-party/dependency exclusions before broad gate expansion.
- **slice03 first-party-source-naming-normalization - closed and
  CDC-verified.** Slice set:
  [`slice03-first-party-source-naming-normalization/slice-doc.md`](slice03-first-party-source-naming-normalization/slice-doc.md),
  [`slice03-first-party-source-naming-normalization/ledger.md`](slice03-first-party-source-naming-normalization/ledger.md),
  [`slice03-first-party-source-naming-normalization/cc-prompt.md`](slice03-first-party-source-naming-normalization/cc-prompt.md),
  [`slice03-first-party-source-naming-normalization/closing-report.md`](slice03-first-party-source-naming-normalization/closing-report.md),
  [`slice03-first-party-source-naming-normalization/cdc-verification.md`](slice03-first-party-source-naming-normalization/cdc-verification.md).
  Slice03 defined and applied the accepted lower snake case naming policy for
  first-party maintained C/C++ files and directories before quality gate
  scaffolding relies on the normalized path surface.
- **slice04 first-party-quality-gate-scaffold - closed and CDC-verified.**
  Slice04 added Make-backed gate scaffolding for first-party format, static
  analysis, coverage, unit/seam tests, warning policy, sanitizer policy,
  optional IPC corpus placement, and generated warning triage records before
  component burndown slices start enforcing or clearing findings. Slice set:
  [`slice04-first-party-quality-gate-scaffold/slice-doc.md`](slice04-first-party-quality-gate-scaffold/slice-doc.md),
  [`slice04-first-party-quality-gate-scaffold/ledger.md`](slice04-first-party-quality-gate-scaffold/ledger.md),
  [`slice04-first-party-quality-gate-scaffold/cc-prompt.md`](slice04-first-party-quality-gate-scaffold/cc-prompt.md),
  [`slice04-first-party-quality-gate-scaffold/closing-report.md`](slice04-first-party-quality-gate-scaffold/closing-report.md),
  [`slice04-first-party-quality-gate-scaffold/cdc-verification.md`](slice04-first-party-quality-gate-scaffold/cdc-verification.md).
  Gate scaffold report:
  [`source-quality-gate-scaffold.md`](source-quality-gate-scaffold.md).
- **slice05 engine-first-party-quality-burndown - closed and
  CDC-verified.** Slice05 converted Slice04 engine report-backed rows into
  Make-backed executable engine component gates, added focused seam evidence
  for the `visited_list` payload representation, closed the primary engine
  warning budget, and eliminated duplicate link-library warning noise without
  changing public behavior. `format-check-engine` and
  `static-analysis-engine` are accepted as reported baselines, not strict
  zero-drift or zero-finding gates. Slice set:
  [`slice05-engine-first-party-quality-burndown/slice-doc.md`](slice05-engine-first-party-quality-burndown/slice-doc.md),
  [`slice05-engine-first-party-quality-burndown/ledger.md`](slice05-engine-first-party-quality-burndown/ledger.md),
  [`slice05-engine-first-party-quality-burndown/cc-prompt.md`](slice05-engine-first-party-quality-burndown/cc-prompt.md),
  [`slice05-engine-first-party-quality-burndown/closing-report.md`](slice05-engine-first-party-quality-burndown/closing-report.md),
  [`slice05-engine-first-party-quality-burndown/cdc-verification.md`](slice05-engine-first-party-quality-burndown/cdc-verification.md).
  Parser generated-warning/root-cause work is the recommended next scoped
  ledger; grounder first-party source and dependency-boundary follow-up remain
  later Arc07 work.
- **slice06 parser-generated-warning-root-cause - closed and CDC-verified.**
  Slice06 turned the parser generated-warning scaffold into component-specific
  root-cause, correctness, remedy, and re-entry evidence. It closed the Flex
  `yyunput`/`yyinput` scanner warnings through the owned scanner input,
  dispositioned parser gengetopt output without migrating to CLI11, and
  preserved managed parser behavior. Slice set:
  [`slice06-parser-generated-warning-root-cause/slice-doc.md`](slice06-parser-generated-warning-root-cause/slice-doc.md),
  [`slice06-parser-generated-warning-root-cause/ledger.md`](slice06-parser-generated-warning-root-cause/ledger.md),
  [`slice06-parser-generated-warning-root-cause/cc-prompt.md`](slice06-parser-generated-warning-root-cause/cc-prompt.md),
  [`slice06-parser-generated-warning-root-cause/closing-report.md`](slice06-parser-generated-warning-root-cause/closing-report.md),
  [`slice06-parser-generated-warning-root-cause/cdc-verification.md`](slice06-parser-generated-warning-root-cause/cdc-verification.md).
  Parser generated-warning evidence:
  [`parser-generated-warning-root-cause.md`](parser-generated-warning-root-cause.md).
  Parser Flex helper warnings are closed; parser gengetopt/option replacement,
  CLI11 migration, and stricter generated-code policy remain separate re-entry
  work. Grounder first-party source and dependency-boundary follow-up is the
  recommended next scoped ledger.
- **slice07 grounder-source-quality-boundary - closed and CDC-verified.**
  Slice07 converted the grounder report-backed scaffold into Make-backed
  component source-quality targets and made the first-party/dependency
  boundary executable before stricter grounder claims are made. It resolved
  `rss.c` as first-party maintained copied-origin source, fixed its
  strict-prototype warnings, kept cpddl/H2/nested third-party/generated/copied
  surfaces separately reported unless a dependency audit accepts ownership,
  and preserved managed grounder/pipeline behavior. Slice set:
  [`slice07-grounder-source-quality-boundary/slice-doc.md`](slice07-grounder-source-quality-boundary/slice-doc.md),
  [`slice07-grounder-source-quality-boundary/ledger.md`](slice07-grounder-source-quality-boundary/ledger.md),
  [`slice07-grounder-source-quality-boundary/cc-prompt.md`](slice07-grounder-source-quality-boundary/cc-prompt.md),
  [`slice07-grounder-source-quality-boundary/closing-report.md`](slice07-grounder-source-quality-boundary/closing-report.md),
  [`slice07-grounder-source-quality-boundary/cdc-verification.md`](slice07-grounder-source-quality-boundary/cdc-verification.md).
  Source-quality synthesis is the recommended next scoped ledger.
- **slice08 source-quality-synthesis - open.** Slice08 composes the completed
  Arc07 source-quality evidence, reconciles arc ledger rows A1-A9, classifies
  remaining work as release blockers, measured baselines, accepted
  deferred/post-0.3.0 budgets, or out-of-first-party-scope dependency
  boundaries, and proposes Arc07 closure only if the evidence honestly
  composes. Slice set:
  [`slice08-source-quality-synthesis/slice-doc.md`](slice08-source-quality-synthesis/slice-doc.md),
  [`slice08-source-quality-synthesis/ledger.md`](slice08-source-quality-synthesis/ledger.md),
  [`slice08-source-quality-synthesis/cc-prompt.md`](slice08-source-quality-synthesis/cc-prompt.md).

## 6. Planned Implementation Surface

Arc07 may touch these surfaces as slices open:

| Surface | Intended ownership |
|---------|--------------------|
| `pandaPI/parser/` | first-party parser source, generated parser artifacts, generator/template classification, build/test hooks |
| `pandaPI/grounder/` | first-party grounder source, nested dependency classification, generator/template classification, build/test hooks |
| `pandaPI/engine/` | first-party engine source, optional/fenced source classification, generated command parser artifacts, build/test hooks |
| `pandaPI/runtime/` | existing runtime gates plus any support needed for component unit/seam tests |
| `fixtures/`, `tests/` | unit/seam tests, process fixtures only where needed to exercise source-quality gates |
| `mk/`, `Makefile` | canonical Make targets for first-party source-quality gates |
| `docs/design-v0.3.0/arc07-source-quality-expansion/` | plans, source-classification report, recommendations, closing evidence |

Arc07 should not edit release publication assets, final release notes, wolong
installation docs, or public tutorial prose except to route handoff evidence.

## 7. Arc Ledger

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Every source path under parser, grounder, engine, and runtime is classified as first-party maintained, first-party generated, first-party generator/template, vendored third-party, third-party generated, copied build artifact, generated build output, or other explicitly routed class. | reproduced |
| A2 | The plan uses source-class language rather than ambiguous "owned runtime versus inherited planner" wording for active quality decisions. | reproduced |
| A3 | First-party maintained parser, grounder, engine, and runtime code have Make-backed formatting, static-analysis, coverage, unit-test, warning, and sanitizer policy, or explicit release-blocking follow-up rows. | reproduced |
| A4 | Chengdu-owned generators/templates are identified, and generated-code warnings or formatting problems are triaged by root cause, correctness impact, generator/skeleton ownership, remedy, and re-entry condition before being routed to generator/template fixes or documented third-party exclusions. | reproduced |
| A5 | Vendored third-party and third-party generated code are excluded from first-party coverage/static-analysis/unit-test obligations or reported separately with rationale. | reproduced |
| A6 | Public integration/process-contract tests remain scoped to accepted `pandapi-*` behavior while unit/seam tests cover internal first-party code. | reproduced |
| A7 | Any required source/build restructuring is performed before coverage/static-analysis thresholds rely on the new layout. | reproduced |
| A8 | Release blocker policy is explicit: zero first-party warnings/static-analysis findings where required, sanitizer findings triaged by source class, and coverage thresholds recorded as measured baselines or release floors. | reproduced |
| A9 | Arc08 documentation and Arc09 release prep can proceed from Arc07 without silent drops around source-quality scope, dependency exclusions, or remaining budgets. | reproduced |

## 8. Open Questions And Risks

- **OQ1 - classification before enforcement.** Do not set global thresholds
  until Slice01 classifies source paths and identifies which build products are
  first-party versus generated or third-party.
- **OQ2 - restructuring may be required.** Parser, grounder, and engine build
  flows may need compile-database/profile isolation before component coverage
  and static analysis produce honest signal.
- **OQ3 - Catch2 posture.** Unit testing internals may require promoting
  Catch2 from optional local discovery to a pinned test dependency. Slice01
  should recommend, but not silently implement, that decision.
- **OQ4 - generated code.** Generated warnings from Chengdu-owned generators
  should be fixed at the generator/template after root-cause and correctness
  analysis. Each generated warning needs a triage record naming the generated
  file, generator/template or third-party skeleton, ownership class, why the
  warning is emitted, whether generated semantics are correct, chosen remedy,
  and re-entry condition. Generated warnings from third-party skeletons may be
  excluded or suppressed only with rationale.
- **OQ5 - component sequencing.** Engine, grounder, and parser may not have
  equal testability. Slice01 should recommend the safest component order.

## 9. Version History

- **v1.15 - 2026-08-13.** Opened Slice08
  source-quality-synthesis. Surfaced by: Slice07 CDC verification and
  closing-report bubble-up. Why: runtime, engine, parser generated-warning,
  and grounder boundary evidence now need one composition pass to determine
  whether Arc07 can honestly close, which remaining source-quality items are
  release blockers, and what exact Arc08/Arc09 handoff public docs and release
  prep should consume.
- **v1.14 - 2026-08-13.** Marked Slice07
  grounder-source-quality-boundary closed and CDC-verified, and selected
  source-quality synthesis as the next scoped ledger. Surfaced by: Slice07
  CDC verification. Why: grounder now has Make-backed component source-quality
  targets, first-party maintained warning count 0, `rss.c` resolved and
  warning-clean, direct topsort seam evidence, ASan/UBSan seam evidence, and
  managed grounder/pipeline regression proof while cpddl, H2, nested
  third-party, generated, copied-build, and build-output classes remain
  separately routed.
- **v1.13 - 2026-08-13.** Opened Slice07
  grounder-source-quality-boundary. Surfaced by: Slice06 CDC verification.
  Why: parser generated Flex warnings are closed, leaving grounder as the
  next component whose first-party source-quality gates must become executable
  without silently absorbing cpddl, H2, nested third-party, generated,
  copied-build, or build-output warnings into first-party obligations. The
  slice also resolves the `rss.c` classification ambiguity before warning
  budgets become stricter.
- **v1.12 - 2026-08-13.** Marked Slice06
  parser-generated-warning-root-cause closed and CDC-verified. Surfaced by:
  Slice06 CDC verification. Why: the parser Flex `yyunput`/`yyinput` warnings
  are now closed through owned scanner input, generated parser outputs remain
  untouched, managed parser/pipeline behavior still passes, and parser
  gengetopt/CLI11 work is explicitly routed to later re-entry rather than
  hidden in the generated-warning closure.
- **v1.11 - 2026-08-13.** Opened Slice06
  parser-generated-warning-root-cause. Surfaced by: Slice05 CDC verification.
  Why: engine component gates now have an executable pattern, and the
  remaining parser warning budget is generated scanner output that requires
  Arc07's stricter root-cause/correctness/remedy discipline before generated
  warnings can become strict or explicitly excluded.
- **v1.10 - 2026-08-12.** Marked Slice05
  engine-first-party-quality-burndown closed and CDC-verified, and selected
  parser generated-warning/root-cause work as the next scoped ledger. Surfaced
  by: Slice05 CDC verification. Why: Arc07 now has an executable engine target
  pattern with source-class fences, seam evidence, sanitizer/coverage/warning
  baselines, and public contract regression proof, so the remaining generated
  warning budget can move to parser root-cause analysis without pretending the
  engine baselines are strict global thresholds.
- **v1.9 - 2026-08-12.** Opened Slice05
  engine-first-party-quality-burndown. Surfaced by: Slice04 CDC verification.
  Why: the source-quality scaffold selected engine as the first component
  burndown because its remaining primary warning budget is narrow, risk-bearing,
  and tied to internal seam evidence needed before stronger engine gates can be
  made executable.
- **v1.8 - 2026-08-12.** Marked Slice04
  first-party-quality-gate-scaffold closed and CDC-verified, and selected
  engine first-party maintained source as the first Slice05 component
  burndown. Surfaced by: Slice04 CDC verification. Why: the project now has
  named Make-backed scaffold targets and durable re-entry evidence for the
  first-party source-quality gate families, so Arc07 can begin clearing
  component findings without premature global thresholds.
- **v1.7 - 2026-08-12.** Opened Slice04
  first-party-quality-gate-scaffold. Surfaced by: Slice03 CDC verification and
  generated-warning policy clarification. Why: first-party source-quality
  component burndown needs named Make gate families, durable status/re-entry
  reports, generated-warning triage records, and optional IPC corpus placement
  before later slices begin enforcing or clearing findings.
- **v1.6 - 2026-08-12.** Made generated-warning root-cause and correctness
  triage explicit before Arc07 quality-gate scaffolding and component
  burndown. Surfaced by: operator clarification after Slice03 CDC
  verification. Why: generated-warning work must validate whether generated
  code is correct and fix Chengdu-owned generators/templates at the source,
  not merely hide emitted warnings.
- **v1.5 - 2026-08-12.** Marked Slice03
  first-party-source-naming-normalization closed and CDC-verified. Surfaced by:
  Slice03 CDC verification. Why: first-party maintained parser, grounder, and
  engine source names are now normalized and Make-checkable, so Slice04
  first-party-quality-gate-scaffold can open next.
- **v1.4 - 2026-08-12.** Opened Slice03
  first-party-source-naming-normalization. Surfaced by: Slice02 CDC
  verification and operator acceptance of the naming-normalization insertion.
  Why: first-party source names need one documented, Make-checkable convention
  before quality gate scaffolding turns paths into enforcement surfaces.
- **v1.3 - 2026-08-12.** Marked Slice02
  source-layout-and-build-surface-normalization closed and CDC-verified, and
  inserted Slice03 first-party-source-naming-normalization before the quality
  gate scaffold. Surfaced by: Slice02 CDC verification plus operator naming
  policy discussion. Why: first-party path names should be normalized before
  Arc07 gate selectors, compile databases, profile maps, static-analysis
  paths, and coverage surfaces become enforcement anchors.
- **v1.2 - 2026-08-12.** Opened Slice02
  source-layout-and-build-surface-normalization. Surfaced by: operator
  acceptance of Slice01's recommendation. Why: first-party quality gate
  expansion needs Make-backed selectors, compile databases, profile/source
  mapping, and dependency/generated exclusions before thresholds.
- **v1.1 - 2026-08-12.** Marked Slice01 source-classification-inventory
  closed and CDC-verified. Surfaced by: Slice01 CDC verification. Why:
  classification evidence accepts source-layout/build-surface normalization as
  the next work before first-party quality gates expand.
- **v1.0 - 2026-08-12.** Opened Arc07 source-quality-expansion and Slice01
  source-classification-inventory. Surfaced by: operator correction after
  Arc06 close. Why: all in-tree pandaPI product code is Chengdu-owned in the
  fork/product sense, and first-party parser, grounder, and engine internals
  need coverage, static analysis, formatting, warning policy, and unit-test
  planning before release.
