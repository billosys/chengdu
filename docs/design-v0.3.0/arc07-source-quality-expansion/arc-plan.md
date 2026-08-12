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
problems should be fixed there rather than hidden in generated output.

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

## 3. Slice Breakdown

| Slice | Slug | Scope | Load-bearing for |
|-------|------|-------|------------------|
| slice01 | `source-classification-inventory` | Classify every source/build/generated path by source class; map existing gates and gaps; produce recommendations for restructuring, target shape, test framework/dependency posture, and likely slice order. | all later Arc07 work |
| slice02 | `source-layout-and-build-surface-normalization` | Conditional restructuring slice. Apply only the path/build/compile-database/profile-isolation changes accepted after Slice01 review, so first-party parser/grounder/engine files can be analyzed without third-party/generated noise. | coverage/static-analysis expansion |
| slice03 | `first-party-quality-gate-scaffold` | Add or revise Make-backed aggregate/per-component target scaffolding for first-party format, static analysis, coverage, unit tests, and warning policy without yet forcing unrealistic thresholds. | component quality slices |
| slice04+ | `component-source-quality-burndown` | A series of component or subsystem slices, opened one at a time after Slice01/Slice02/Slice03 determine the real cut lines. Expected areas include parser first-party source, grounder first-party source, engine first-party source, Chengdu-owned generators/templates, and shared runtime follow-up. | release readiness |
| final | `source-quality-synthesis` | Compose the final source-quality evidence, thresholds, release blockers, remaining budgets, and Arc08/Arc09 handoff. | docs and release prep |

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

- **slice01 source-classification-inventory - open.** Slice set:
  [`slice01-source-classification-inventory/slice-doc.md`](slice01-source-classification-inventory/slice-doc.md),
  [`slice01-source-classification-inventory/ledger.md`](slice01-source-classification-inventory/ledger.md),
  [`slice01-source-classification-inventory/cc-prompt.md`](slice01-source-classification-inventory/cc-prompt.md).

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
| A4 | Chengdu-owned generators/templates are identified, and generated-code warnings or formatting problems are routed to generator/template fixes where applicable. | reproduced |
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
  should be fixed at the generator/template; generated warnings from
  third-party skeletons may be excluded or suppressed with rationale.
- **OQ5 - component sequencing.** Engine, grounder, and parser may not have
  equal testability. Slice01 should recommend the safest component order.

## 9. Version History

- **v1.0 - 2026-08-12.** Opened Arc07 source-quality-expansion and Slice01
  source-classification-inventory. Surfaced by: operator correction after
  Arc06 close. Why: all in-tree pandaPI product code is Chengdu-owned in the
  fork/product sense, and first-party parser, grounder, and engine internals
  need coverage, static analysis, formatting, warning policy, and unit-test
  planning before release.
