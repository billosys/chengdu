# chengdu arc06 - ci-and-test-hardening - arc plan

> Plan-of-record for arc06, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc begins after Arc05
> `binary-contract-adoption` is closed and after the post-Arc05 source layout
> normalization commits moved product source under `pandaPI/`.

## 1. Capability statement

Roadmap line: *Complete the remaining local and CI proof for the new behavior:
expanded process fixtures, coverage, compiler-warning cleanup, heavier
sanitizer/static-analysis gates, and pre-release CI evidence.*

Expanded: this arc turns Arc05's local primary-binary conformance into a
release-grade quality gate suite. Arc05 proved that `pandapi-parser`,
`pandapi-grounder`, and `pandapi-engine` conform to the accepted
managed-process contract on the current local surface. Arc06 must make the
proof harder to fool: inventory fixture coverage against the Arc03 matrix,
expand process fixtures for missing supported and failure-path obligations,
add coverage evidence for chengdu-owned runtime/process-policy code, add
heavier static and sanitizer gates where the current toolchains support them,
burn down or explicitly disposition inherited C/C++ compiler warnings before
release, and wire the resulting proof through Make-backed local and CI entry
points.

Arc06 is not a documentation/tutorial arc and not the release-publication arc.
Arc07 owns the public tutorial, README, architecture, dependency rationale,
behavior-change table, and migration prose. Arc08 owns release asset shape,
checksums, manifests, package dry-runs, license/NOTICE proof, test-only
dependency exclusion, wolong migration proof, and publication.

## 2. Design principles

1. **Make remains the public interface.** All new CI and GitHub Actions jobs
   must call Make targets. If a new gate needs behavior that lacks a target,
   add the target before wiring CI.
2. **Inventory before expansion.** Do not add another pile of fixtures until
   the current fixture set is mapped to the Arc03 matrix and its gaps are
   explicitly routed.
3. **Product behavior stays black-box.** Catch2/CTest seam tests support
   runtime helper confidence, but parser, grounder, engine, and pipeline
   behavior must be proved by process fixtures and smoke tests.
4. **Coverage has scoped meaning.** Arc06 should measure chengdu-owned
   runtime/process-policy code and touched adoption seams first. It must not
   create vanity percentages over inherited planner or third-party code.
5. **Heavier gates graduate honestly.** `clang-tidy`, Clang Static Analyzer,
   compiler-warning gates, ASan/UBSan-on-binaries, LSan, and TSan may start as
   local, non-blocking, or platform-limited gates if the toolchain or
   inherited-source noise requires triage. Blocking status needs evidence and
   a clean ownership policy.
6. **No optional-surface expansion.** Adding fixture or sanitizer coverage must
   not promote inherited optional surfaces into supported 0.3.0 behavior.
7. **No release overclaim.** Passing Arc06 means the local and CI quality gates
   are ready for release preparation. It does not mean `v0.3.0` is packaged,
   licensed, checksummed, wolong-verified, or published.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `fixture-gap-inventory` | Inventory current baseline/managed fixtures, Make/CI gates, and Arc03 matrix gaps; add missing Make list targets if needed, with no binary behavior change. | all fixture expansion and CI hardening |
| slice02 | `process-fixture-expansion` | Add the highest-value missing process fixtures for supported parser, grounder, engine, and pipeline behavior, including supervised pipe/stream cases that are safe in CI. | coverage/sanitizer workload quality; Arc07 examples; Arc08 release confidence |
| slice03 | `coverage-gate` | Add Clang source-based coverage targets and reporting for `pandaPI/runtime` and chengdu-owned process-policy/adoption seams, with explicit exclusions for generated and inherited third-party code. | CI evidence; release readiness |
| slice04 | `static-analysis-gate` | Add or graduate `clang-tidy`/Clang Static Analyzer gates for owned C++ source where compile databases are reliable, with written suppression and ownership policy. | pre-release confidence; future cleanup arcs |
| slice05 | `compiler-warning-burndown` | Inventory current parser, grounder, engine, runtime, generated-code, and nested third-party compiler warnings; fix primary hand-written warnings where low-risk; document suppressions and warning budgets before release. | release confidence; static-analysis ownership; sanitizer signal quality |
| slice06 | `binary-sanitizer-gates` | Run ASan/UBSan/LSan where supported against runtime and representative `pandapi-*` process fixtures without turning inherited third-party noise into silent failure. | sanitizer confidence; fixture workload validation |
| slice07 | `tsan-and-ci-synthesis` | Add or explicitly defer TSan based on representative concurrency/process-observation workload, then compose the final local/CI hardening evidence and hand off to Arc07/Arc08. | project ledger P6; Arc07 docs; Arc08 release prep |

## 4. Dependencies

**Consumes:**

- Arc03 final managed-process contract:
  [`../arc03-managed-process-contract/managed-process-contract.md`](../arc03-managed-process-contract/managed-process-contract.md)
- Arc03 contract-test matrix:
  [`../arc03-managed-process-contract/contract-test-matrix.md`](../arc03-managed-process-contract/contract-test-matrix.md)
- Arc04 fixture and runtime substrate, now located under `pandaPI/runtime/`:
  [`../arc04-shared-runtime-substrate/closing-report.md`](../arc04-shared-runtime-substrate/closing-report.md)
- Arc05 native binary conformance and downstream handoff:
  [`../arc05-binary-contract-adoption/closing-report.md`](../arc05-binary-contract-adoption/closing-report.md)
- CI/tooling design note:
  [`../ci-notes.md`](../ci-notes.md)
- Active Makefile convention from `AGENTS.md`: Make is the entry point for
  build, test, check, CI, and release operations; new CI jobs call Make
  targets, not project scripts.

**Leaves for later arcs:** Arc07 receives the final behavior-change evidence,
CLI examples, known fixture boundaries, and architecture/dependency facts it
needs for public docs. Arc08 receives CI-hardening evidence, coverage/static
analysis/sanitizer disposition, pre-release gate status, and any release-risk
deferrals that must be checked before package publication.

## 5. Current status

- **slice01 fixture-gap-inventory - closed and CDC-verified.** Slice set:
  [`slice01-fixture-gap-inventory/slice-doc.md`](slice01-fixture-gap-inventory/slice-doc.md),
  [`slice01-fixture-gap-inventory/ledger.md`](slice01-fixture-gap-inventory/ledger.md),
  [`slice01-fixture-gap-inventory/cc-prompt.md`](slice01-fixture-gap-inventory/cc-prompt.md),
  [`slice01-fixture-gap-inventory/closing-report.md`](slice01-fixture-gap-inventory/closing-report.md),
  [`slice01-fixture-gap-inventory/cdc-verification.md`](slice01-fixture-gap-inventory/cdc-verification.md).
- **slice02 process-fixture-expansion - closed and CDC-verified.** Slice set:
  [`slice02-process-fixture-expansion/slice-doc.md`](slice02-process-fixture-expansion/slice-doc.md),
  [`slice02-process-fixture-expansion/ledger.md`](slice02-process-fixture-expansion/ledger.md),
  [`slice02-process-fixture-expansion/cc-prompt.md`](slice02-process-fixture-expansion/cc-prompt.md),
  [`slice02-process-fixture-expansion/closing-report.md`](slice02-process-fixture-expansion/closing-report.md),
  [`slice02-process-fixture-expansion/cdc-verification.md`](slice02-process-fixture-expansion/cdc-verification.md).
- **slice03 coverage-gate - closed and CDC-verified.** Slice set:
  [`slice03-coverage-gate/slice-doc.md`](slice03-coverage-gate/slice-doc.md),
  [`slice03-coverage-gate/ledger.md`](slice03-coverage-gate/ledger.md),
  [`slice03-coverage-gate/cc-prompt.md`](slice03-coverage-gate/cc-prompt.md),
  [`slice03-coverage-gate/closing-report.md`](slice03-coverage-gate/closing-report.md),
  [`slice03-coverage-gate/cdc-verification.md`](slice03-coverage-gate/cdc-verification.md).
- **slice04 static-analysis-gate - closed and CDC-verified.** Slice set:
  [`slice04-static-analysis-gate/slice-doc.md`](slice04-static-analysis-gate/slice-doc.md),
  [`slice04-static-analysis-gate/ledger.md`](slice04-static-analysis-gate/ledger.md),
  [`slice04-static-analysis-gate/cc-prompt.md`](slice04-static-analysis-gate/cc-prompt.md),
  [`slice04-static-analysis-gate/closing-report.md`](slice04-static-analysis-gate/closing-report.md),
  [`slice04-static-analysis-gate/cdc-verification.md`](slice04-static-analysis-gate/cdc-verification.md).
- **slice05 compiler-warning-burndown - closed and CDC-verified.** Slice set:
  [`slice05-compiler-warning-burndown/slice-doc.md`](slice05-compiler-warning-burndown/slice-doc.md),
  [`slice05-compiler-warning-burndown/ledger.md`](slice05-compiler-warning-burndown/ledger.md),
  [`slice05-compiler-warning-burndown/cc-prompt.md`](slice05-compiler-warning-burndown/cc-prompt.md),
  [`slice05-compiler-warning-burndown/closing-report.md`](slice05-compiler-warning-burndown/closing-report.md),
  [`slice05-compiler-warning-burndown/cdc-verification.md`](slice05-compiler-warning-burndown/cdc-verification.md).
- **slice06 binary-sanitizer-gates - closed and CDC-verified.** Slice set:
  [`slice06-binary-sanitizer-gates/slice-doc.md`](slice06-binary-sanitizer-gates/slice-doc.md),
  [`slice06-binary-sanitizer-gates/ledger.md`](slice06-binary-sanitizer-gates/ledger.md),
  [`slice06-binary-sanitizer-gates/cc-prompt.md`](slice06-binary-sanitizer-gates/cc-prompt.md),
  [`slice06-binary-sanitizer-gates/closing-report.md`](slice06-binary-sanitizer-gates/closing-report.md),
  [`slice06-binary-sanitizer-gates/cdc-verification.md`](slice06-binary-sanitizer-gates/cdc-verification.md).
- **slice07 tsan-and-ci-synthesis - open.** Slice set:
  [`slice07-tsan-and-ci-synthesis/slice-doc.md`](slice07-tsan-and-ci-synthesis/slice-doc.md),
  [`slice07-tsan-and-ci-synthesis/ledger.md`](slice07-tsan-and-ci-synthesis/ledger.md),
  [`slice07-tsan-and-ci-synthesis/cc-prompt.md`](slice07-tsan-and-ci-synthesis/cc-prompt.md).

## 6. Planned implementation surface

Arc06 may touch these surfaces as slices open:

| Surface | Intended ownership |
|---------|--------------------|
| `fixtures/contract/` | Fixture records, inventory reports, golden/semantic comparison notes, and gap evidence. |
| `tests/contract/run`, `tests/smoke/run` | Harness behavior only through Make-backed entry points; process-fixture expansion and observation hardening. |
| `pandaPI/runtime/` | Runtime tests, coverage instrumentation support, process-observation helpers, and owned C++ quality gates. |
| `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine` | Only narrow testability, instrumentation, or compiler-warning-burndown changes owned by an Arc06 slice; no optional-surface expansion. |
| `mk/`, `Makefile` | Canonical local and CI targets for fixture listing, coverage, static analysis, sanitizers, and CI aggregates. |
| `.github/workflows/` | CI jobs that invoke Make targets only. |
| `docs/design-v0.3.0/arc06-ci-and-test-hardening/` | Arc06 plans, reports, fixture inventory, and closing evidence. |
| `dist/`, `build/` | Generated output only. |

Arc06 should not edit release package assets, final release notes, wolong
migration docs, or public tutorial material except to hand off evidence.

## 7. Arc ledger

Composition rows verifying the capability; open here and close per-row in this
arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Every planned Arc06 slice is closed and CDC-verified, with no missing slice from the breakdown. | reproduced |
| A2 | Current baseline and managed process fixtures are inventoried against the Arc03 matrix, with every missing/deferred obligation routed to an Arc06 slice, Arc07 docs, Arc08 release prep, or a no-op rationale. | reproduced |
| A3 | Expanded fixtures prove supported parser, grounder, engine, and pipeline behavior through black-box process observations under canonical `pandapi-*` commands, including positive and negative paths. | reproduced |
| A4 | CLI and pipe-supervised invocation are both represented by CI-safe fixture or smoke workloads; the arc does not rely on human CLI smoke alone as process-manager proof. | reproduced |
| A5 | Coverage evidence exists for chengdu-owned runtime/process-policy and touched adoption seams, with generated, inherited planner, and third-party code excluded or reported separately by policy. | reproduced |
| A6 | Static-analysis gates run through Make and cover owned C++ source where compile databases are reliable; any unavailable toolchain or inherited-source noise is explicitly dispositioned. | reproduced |
| A7 | Compiler warnings from parser, grounder, engine, runtime, generated code, and nested third-party code are inventoried by ownership tier; primary hand-written warnings are fixed or explicitly budgeted; generated and third-party warnings are isolated, suppressed, or routed with release re-entry criteria. | reproduced |
| A8 | ASan/UBSan/LSan gates run through Make where supported and include representative process-fixture workloads, with ownership triage for inherited or third-party findings. | reproduced |
| A9 | TSan is either added as a meaningful Make/CI gate over representative concurrency/process-observation workload or explicitly deferred with a concrete re-entry condition. | reproduced |
| A10 | GitHub Actions and local CI-equivalent targets use Make entry points for every new gate and pass `make actionlint` plus workflow Make-entrypoint safety checks. | reproduced |
| A11 | Arc06 closes without changing release asset shape, wolong installation path, inherited optional-surface support, or public tutorial/docs scope. | reproduced |
| A12 | Arc07 and Arc08 can be planned from Arc06 close without silent drops: docs/tutorial evidence, behavior-change inputs, release gate status, coverage/static/sanitizer/warning disposition, and remaining risks are routed. | reproduced |

## 8. Open questions and risks

- **OQ1 - coverage scope.** The useful first metric is owned runtime and
  process-policy coverage, not inherited planner algorithm coverage. Slice03
  must define exclusions before presenting percentages.
- **OQ2 - TSan workload.** TSan only finds races that are exercised. Slice07
  should not add a symbolic TSan job if process-observation concurrency is not
  representative enough.
- **OQ3 - sanitizer and warning ownership.** ASan/UBSan/LSan findings and
  compiler warnings in chengdu-owned runtime/adoption code should block unless
  explicitly budgeted. Findings or warnings in inherited generated code and
  third-party code require explicit triage rather than blanket suppression or
  accidental promotion to release blockers.
- **OQ4 - CI time budget.** Expanded fixtures and sanitizer builds can make CI
  noisy or slow. Arc06 should separate always-on gates from scheduled or
  pre-release gates when evidence supports the split.
- **OQ5 - post-Arc05 layout drift.** Active source paths are
  `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine`, and
  `pandaPI/runtime`. Historical Arc03/Arc04/Arc05 evidence may mention older
  paths; Arc06 implementation must use the current tree.

## 9. Version history

- **v1.8 - 2026-08-12.** Marked Slice06 binary-sanitizer-gates closed and
  CDC-verified, and opened Slice07 tsan-and-ci-synthesis. Surfaced by: CDC
  reproduction of CC commit `95f98412`. Why: `make test-binary-sanitize`
  now runs ASan/UBSan over an isolated canonical `pandapi-*` sanitizer
  distribution and the full managed fixture workload, while macOS LSan remains
  an explicit Linux/toolchain re-entry. The remaining Arc06 work is TSan
  disposition plus final CI and downstream handoff synthesis.

- **v1.7 - 2026-08-12.** Marked Slice06 binary-sanitizer-gates CC proposed
  done. Surfaced by: CC sanitizer implementation and verification. Why:
  `make test-binary-sanitize` now builds an isolated ASan/UBSan distribution
  for `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`, runs managed
  parser/grounder/engine/pipeline fixtures through that distribution, fixes
  promoted engine sanitizer findings, and documents macOS LSan as a precise
  skip with Linux/toolchain re-entry.

- **v1.6 - 2026-08-12.** Marked Slice05 compiler-warning-burndown closed and
  CDC-verified, and opened Slice06 binary-sanitizer-gates. Surfaced by: CDC
  reproduction of CC commits `fc8b671d` and `85a396ff`. Why: warning classes
  are now inventoried by ownership tier, low-risk primary warnings were burned
  down, and remaining warning budgets are specific enough for ASan/UBSan/LSan
  findings to be triaged honestly against canonical `pandapi-*` process
  fixtures.

- **v1.5 - 2026-08-11.** Marked Slice04 static-analysis-gate closed and
  CDC-verified, and opened Slice05 compiler-warning-burndown. Surfaced by:
  CDC reproduction of CC commit `04491682`. Why: owned runtime C++ static
  analysis now runs through Make with a reliable Debug compile database and
  documented exclusions, while parser, grounder, and engine builds still emit
  inherited warning debt. Slice05 must inventory, classify, fix, budget, or
  defer those warnings before sanitizer and release gates treat build signal as
  release-quality.

- **v1.4 - 2026-08-11.** Marked Slice03 coverage-gate closed and
  CDC-verified, and opened Slice04 static-analysis-gate. Surfaced by: CDC
  reproduction of CC commit `dffa6baf`. Why: `make coverage` now produces
  scoped owned-runtime coverage evidence under `build/coverage/runtime/`, with
  inherited/generated/third-party exclusions and explicit adoption-seam
  deferral. The next release-readiness risk is Make-backed C++ static analysis
  over owned runtime/process-policy code, with toolchain, suppression, and path
  ownership documented before compiler-warning burndown.

- **v1.3 - 2026-08-11.** Marked Slice02 process-fixture-expansion closed and
  CDC-verified, and opened Slice03 coverage-gate. Surfaced by: CDC
  reproduction of CC commit `89991120` on top of warning-burndown planning
  commit `ca7cccae`. Why: managed parser, grounder, engine, and pipeline
  fixtures now reproduce as a representative workload, so coverage can measure
  owned runtime/process-policy code without pretending inherited planner
  algorithm coverage is the primary signal. Slice02's closing-report references
  to later sanitizer/timeout slice numbers are superseded by v1.2's inserted
  compiler-warning-burndown slice.

- **v1.2 - 2026-08-11.** Inserted Slice05
  compiler-warning-burndown and renumbered binary-sanitizer and TSan synthesis
  work to Slice06 and Slice07. Surfaced by: operator release-readiness review
  after Arc06 Slice02 implementation. Why: Arc01 recorded parser `P-011`,
  grounder `G-015`, and engine `E-011` compiler-warning debt, but Arc06 only
  had static-analysis and sanitizer disposition slices. The release needs an
  explicit warning inventory, burndown, suppression, and budget gate before
  Arc08 publication.

- **v1.1 - 2026-08-11.** Marked Slice01 fixture-gap-inventory closed and
  CDC-verified, and opened Slice02 process-fixture-expansion. Surfaced by:
  CDC reproduction of CC commit `91cc50b6`. Why: the inventory mapped current
  baseline and managed fixture coverage, confirmed 40 managed component
  fixture cases and no managed pipeline cases, and recommended the first
  fixture-expansion batch before coverage, sanitizer, static-analysis, and
  TSan work.

- **v1.0 - 2026-08-11.** Opened Arc06 with Slice01
  fixture-gap-inventory. Surfaced by: Arc05 close and post-Arc05 source layout
  normalization. Why: the binaries now conform locally through canonical
  native `pandapi-*` commands, so the next risk is not another adoption slice
  but release-grade CI/test proof: fixture gaps, coverage, heavier static and
  sanitizer gates, and CI evidence.
