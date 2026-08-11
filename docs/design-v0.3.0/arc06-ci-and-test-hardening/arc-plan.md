# chengdu arc06 - ci-and-test-hardening - arc plan

> Plan-of-record for arc06, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc begins after Arc05
> `binary-contract-adoption` is closed and after the post-Arc05 source layout
> normalization commits moved product source under `pandaPI/`.

## 1. Capability statement

Roadmap line: *Complete the remaining local and CI proof for the new behavior:
expanded process fixtures, coverage, heavier sanitizer/static-analysis gates,
and pre-release CI evidence.*

Expanded: this arc turns Arc05's local primary-binary conformance into a
release-grade quality gate suite. Arc05 proved that `pandapi-parser`,
`pandapi-grounder`, and `pandapi-engine` conform to the accepted
managed-process contract on the current local surface. Arc06 must make the
proof harder to fool: inventory fixture coverage against the Arc03 matrix,
expand process fixtures for missing supported and failure-path obligations,
add coverage evidence for chengdu-owned runtime/process-policy code, add
heavier static and sanitizer gates where the current toolchains support them,
and wire the resulting proof through Make-backed local and CI entry points.

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
   ASan/UBSan-on-binaries, LSan, and TSan may start as local, non-blocking, or
   platform-limited gates if the toolchain or inherited-source noise requires
   triage. Blocking status needs evidence and a clean ownership policy.
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
| slice05 | `binary-sanitizer-gates` | Run ASan/UBSan/LSan where supported against runtime and representative `pandapi-*` process fixtures without turning inherited third-party noise into silent failure. | sanitizer confidence; fixture workload validation |
| slice06 | `tsan-and-ci-synthesis` | Add or explicitly defer TSan based on representative concurrency/process-observation workload, then compose the final local/CI hardening evidence and hand off to Arc07/Arc08. | project ledger P6; Arc07 docs; Arc08 release prep |

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

- **slice01 fixture-gap-inventory - open.** Slice set:
  [`slice01-fixture-gap-inventory/slice-doc.md`](slice01-fixture-gap-inventory/slice-doc.md),
  [`slice01-fixture-gap-inventory/ledger.md`](slice01-fixture-gap-inventory/ledger.md),
  [`slice01-fixture-gap-inventory/cc-prompt.md`](slice01-fixture-gap-inventory/cc-prompt.md).
- **slice02 process-fixture-expansion - planned.** Opens after Slice01 maps
  the missing fixture obligations and chooses the first expansion batch.
- **slice03 coverage-gate - planned.** Opens after fixture workload is
  representative enough that coverage means something.
- **slice04 static-analysis-gate - planned.**
- **slice05 binary-sanitizer-gates - planned.**
- **slice06 tsan-and-ci-synthesis - planned.**

## 6. Planned implementation surface

Arc06 may touch these surfaces as slices open:

| Surface | Intended ownership |
|---------|--------------------|
| `fixtures/contract/` | Fixture records, inventory reports, golden/semantic comparison notes, and gap evidence. |
| `tests/contract/run`, `tests/smoke/run` | Harness behavior only through Make-backed entry points; process-fixture expansion and observation hardening. |
| `pandaPI/runtime/` | Runtime tests, coverage instrumentation support, process-observation helpers, and owned C++ quality gates. |
| `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine` | Only narrow testability or instrumentation changes owned by an Arc06 slice; no optional-surface expansion. |
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
| A7 | ASan/UBSan/LSan gates run through Make where supported and include representative process-fixture workloads, with ownership triage for inherited or third-party findings. | reproduced |
| A8 | TSan is either added as a meaningful Make/CI gate over representative concurrency/process-observation workload or explicitly deferred with a concrete re-entry condition. | reproduced |
| A9 | GitHub Actions and local CI-equivalent targets use Make entry points for every new gate and pass `make actionlint` plus workflow Make-entrypoint safety checks. | reproduced |
| A10 | Arc06 closes without changing release asset shape, wolong installation path, inherited optional-surface support, or public tutorial/docs scope. | reproduced |
| A11 | Arc07 and Arc08 can be planned from Arc06 close without silent drops: docs/tutorial evidence, behavior-change inputs, release gate status, coverage/static/sanitizer disposition, and remaining risks are routed. | reproduced |

## 8. Open questions and risks

- **OQ1 - coverage scope.** The useful first metric is owned runtime and
  process-policy coverage, not inherited planner algorithm coverage. Slice03
  must define exclusions before presenting percentages.
- **OQ2 - TSan workload.** TSan only finds races that are exercised. Slice06
  should not add a symbolic TSan job if process-observation concurrency is not
  representative enough.
- **OQ3 - sanitizer ownership.** ASan/UBSan/LSan findings in chengdu-owned
  runtime/adoption code should block. Findings in inherited third-party code
  require explicit triage rather than blanket suppression or accidental
  promotion to release blockers.
- **OQ4 - CI time budget.** Expanded fixtures and sanitizer builds can make CI
  noisy or slow. Arc06 should separate always-on gates from scheduled or
  pre-release gates when evidence supports the split.
- **OQ5 - post-Arc05 layout drift.** Active source paths are
  `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine`, and
  `pandaPI/runtime`. Historical Arc03/Arc04/Arc05 evidence may mention older
  paths; Arc06 implementation must use the current tree.

## 9. Version history

- **v1.0 - 2026-08-11.** Opened Arc06 with Slice01
  fixture-gap-inventory. Surfaced by: Arc05 close and post-Arc05 source layout
  normalization. Why: the binaries now conform locally through canonical
  native `pandapi-*` commands, so the next risk is not another adoption slice
  but release-grade CI/test proof: fixture gaps, coverage, heavier static and
  sanitizer gates, and CI evidence.
