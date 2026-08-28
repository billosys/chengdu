# chengdu arc04 - shared-runtime-substrate - arc plan

> Plan-of-record for arc04, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc begins after Arc03
> `managed-process-contract` is closed.

## 1. Capability statement

Roadmap line: *Introduce the shared C/C++ runtime/build substrate selected by
the design, with tests and no behavior changes beyond wiring.*

Expanded: this arc creates the chengdu-owned runtime substrate that Arc05 will
use when migrating `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`
onto the accepted managed-process contract. The substrate owns shared status
types, result mapping, diagnostics/process I/O, tagged status writing/parsing,
TTY/color policy, output finalization, provenance assembly, CLI policy
wrappers, and test harness support. It also establishes the build and test
shape for adopting fmt, CLI11, Catch2, `tl::expected`, and any conditional
child-process adapter behind local facades rather than leaking third-party APIs
through planner internals.

Arc04 is implementation work, but it is not per-binary contract adoption.
Planner behavior must remain unchanged except where a slice explicitly wires
the new substrate behind tests without changing observed command behavior.
Arc05 owns user-visible binary migration and `pandapi-*` entry points. Arc06
owns release proof, license packaging proof, wolong verification, and release
documentation.

## 2. Design principles

1. **Chengdu-owned boundary first.** Shared code lives outside `pandaPI/` so
   upstream history remains legible and downstream process policy is owned by
   chengdu.
2. **Narrow interfaces before adoption.** Each substrate facility is exposed
   through `pandapi::runtime` interfaces. fmt, CLI11, Catch2, `tl::expected`,
   and any child-process library remain hidden behind local facades or
   test-only targets.
3. **Standard library baseline.** Prefer C++17 standard-library types for
   ownership, strings, paths, streams, chrono/timeouts, and containers before
   adding dependencies.
4. **No behavior drift.** Arc04 can add targets, helpers, and tests. It must
   not change parser, grounder, or engine stdout/stderr, exits, options,
   artifacts, release assets, or wolong behavior.
5. **Test the substrate before migration.** Catch2 seam tests and process
   fixture harness pieces land before Arc05 uses the helpers in binaries.
6. **Compatibility by construction.** The runtime must support canonical
   `pandapi-*` commands and inherited `pandaPI*` names, but Arc05 decides where
   those command surfaces are wired into each binary.
7. **C++ Core Guidelines pressure.** New code should follow RAII, explicit
   interfaces, self-contained headers, no global mutable state, no raw
   ownership transfer, no header ODR hazards, and systematic error handling.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `runtime-build-skeleton` | Create the chengdu-owned `pandapi-runtime/` source/build/test skeleton and prove it builds without touching inherited planner behavior. | all later Arc04 runtime code; Arc05 link strategy |
| slice02 | `status-result-core` | Implement `ProcessStatus`, exit-code mapping, payload-class vocabulary, and a local status/result facade, with seam tests and no binary adoption. | diagnostics, status output, CLI parse mapping, Arc05 exits |
| slice03 | `diagnostics-status-io` | Implement diagnostics/process I/O facade, stdout/stderr ownership helpers, tagged-text `PANDAPI_STATUS` writer/parser, flushing, and partial-output primitives, with fmt behind the facade if adopted. | Arc05 stream/status adoption; Arc06 behavior evidence |
| slice04 | `cli-tty-provenance-core` | Implement shared CLI policy wrappers, TTY/color policy, help/version/provenance field assembly, and CLI11 adapter gates without replacing per-binary CLIs yet. | Arc05 command migration; Arc06 provenance docs |
| slice05 | `fixture-and-seam-test-substrate` | Build the process fixture harness, normalization rules, safe timeout/resource/signal probes, and Catch2 seam-test integration for runtime helpers. | Arc05 conformance gates; Arc06 CI evidence |
| slice06 | `substrate-integration-readiness` | Wire the runtime build into existing build scripts as an inert dependency/probe, document adoption boundaries, and produce the Arc05 handoff without changing binary behavior. | Arc05 detailed planning; project ledger P4 |

## 4. Dependencies

**Consumes:**

- Arc03 closing report:
  [`../arc03-managed-process-contract/closing-report.md`](../arc03-managed-process-contract/closing-report.md)
- Arc03 final managed-process contract:
  [`../arc03-managed-process-contract/managed-process-contract.md`](../arc03-managed-process-contract/managed-process-contract.md)
- Arc02 closing report:
  [`../arc02-cpp-library-research/closing-report.md`](../arc02-cpp-library-research/closing-report.md)
- Arc02 combined library recommendations:
  [`../arc02-cpp-library-research/combined-library-recommendations.md`](../arc02-cpp-library-research/combined-library-recommendations.md)
- Current build scripts under `scripts/` and inherited build systems under
  `pandaPI/`, as implementation constraints.
- C++ Core Guidelines substrate, especially API/interface design, RAII,
  error-handling strategy, project structure, and standard-library guidance.

**Leaves for arc05:** a buildable `pandapi-runtime` substrate, tested status
and result helpers, diagnostics/status I/O facade, CLI/TTY/provenance helpers,
fixture harness, dependency facades, and clear per-binary adoption instructions.
Arc05 must migrate parser, grounder, and engine one binary at a time and prove
the user-visible managed-process contract at the executable boundary.

**Leaves for arc06:** dependency license/provenance facts that were introduced
in Arc04, proof that test-only dependencies remain out of release assets,
behavior-change table inputs for helpers actually wired in Arc05, and CI/release
checks that prove the new substrate is packaged correctly.

## 5. Current status

- **slice01 runtime-build-skeleton - closed and CDC-verified.** Slice set:
  [`slice01-runtime-build-skeleton/slice-doc.md`](slice01-runtime-build-skeleton/slice-doc.md),
  [`slice01-runtime-build-skeleton/ledger.md`](slice01-runtime-build-skeleton/ledger.md),
  [`slice01-runtime-build-skeleton/cc-prompt.md`](slice01-runtime-build-skeleton/cc-prompt.md).
  CDC verification:
  [`slice01-runtime-build-skeleton/cdc-verification.md`](slice01-runtime-build-skeleton/cdc-verification.md).
- **slice02 status-result-core - closed and CDC-verified.** Slice set:
  [`slice02-status-result-core/slice-doc.md`](slice02-status-result-core/slice-doc.md),
  [`slice02-status-result-core/ledger.md`](slice02-status-result-core/ledger.md),
  [`slice02-status-result-core/cc-prompt.md`](slice02-status-result-core/cc-prompt.md).
  CDC verification:
  [`slice02-status-result-core/cdc-verification.md`](slice02-status-result-core/cdc-verification.md).
- **slice03 diagnostics-status-io - closed and CDC-verified.** Slice set:
  [`slice03-diagnostics-status-io/slice-doc.md`](slice03-diagnostics-status-io/slice-doc.md),
  [`slice03-diagnostics-status-io/ledger.md`](slice03-diagnostics-status-io/ledger.md),
  [`slice03-diagnostics-status-io/cc-prompt.md`](slice03-diagnostics-status-io/cc-prompt.md).
  CDC verification:
  [`slice03-diagnostics-status-io/cdc-verification.md`](slice03-diagnostics-status-io/cdc-verification.md).
- **slice04 cli-tty-provenance-core - closed and CDC-verified.** Slice set:
  [`slice04-cli-tty-provenance-core/slice-doc.md`](slice04-cli-tty-provenance-core/slice-doc.md),
  [`slice04-cli-tty-provenance-core/ledger.md`](slice04-cli-tty-provenance-core/ledger.md),
  [`slice04-cli-tty-provenance-core/cc-prompt.md`](slice04-cli-tty-provenance-core/cc-prompt.md).
  CDC verification:
  [`slice04-cli-tty-provenance-core/cdc-verification.md`](slice04-cli-tty-provenance-core/cdc-verification.md).
- **slice05 fixture-and-seam-test-substrate - closed and CDC-verified.** Slice set:
  [`slice05-fixture-and-seam-test-substrate/slice-doc.md`](slice05-fixture-and-seam-test-substrate/slice-doc.md),
  [`slice05-fixture-and-seam-test-substrate/ledger.md`](slice05-fixture-and-seam-test-substrate/ledger.md),
  [`slice05-fixture-and-seam-test-substrate/cc-prompt.md`](slice05-fixture-and-seam-test-substrate/cc-prompt.md).
  CDC verification:
  [`slice05-fixture-and-seam-test-substrate/cdc-verification.md`](slice05-fixture-and-seam-test-substrate/cdc-verification.md).
- **slice06 substrate-integration-readiness - closed and CDC-verified.** Slice set:
  [`slice06-substrate-integration-readiness/slice-doc.md`](slice06-substrate-integration-readiness/slice-doc.md),
  [`slice06-substrate-integration-readiness/ledger.md`](slice06-substrate-integration-readiness/ledger.md),
  [`slice06-substrate-integration-readiness/cc-prompt.md`](slice06-substrate-integration-readiness/cc-prompt.md).
  CDC verification:
  [`slice06-substrate-integration-readiness/cdc-verification.md`](slice06-substrate-integration-readiness/cdc-verification.md).

## 6. Planned Runtime Artifact Paths

Arc04 introduces chengdu-owned runtime artifacts outside `pandaPI/`:

| Artifact | Planned path |
|----------|--------------|
| Runtime source root | `pandapi-runtime/` |
| Public headers | `pandapi-runtime/include/pandapi/runtime/` |
| Runtime implementation | `pandapi-runtime/src/` |
| Runtime seam tests | `pandapi-runtime/tests/` |
| Runtime CMake helpers | `pandapi-runtime/cmake/` |
| Runtime build/test entrypoint | `scripts/build-runtime.sh` |

The planned C++ namespace is `pandapi::runtime`.

## 7. Arc ledger

Composition rows verifying the capability; open here and close per-row in this
arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Every planned Arc04 slice is closed and CDC-verified, with no missing slice from the breakdown. | reproduced |
| A2 | The final runtime source/build layout creates a chengdu-owned substrate outside `pandaPI/`, with self-contained headers, implementation sources, tests, and build entrypoints that can be consumed by later binary adoption. | reproduced |
| A3 | The status/result substrate implements the Arc03 status vocabulary, exit-code mapping, payload classes, and local result facade without leaking `tl::expected` or other third-party APIs into planner internals. | reproduced |
| A4 | The diagnostics/status I/O substrate implements stdout/stderr ownership, tagged-text `PANDAPI_STATUS`, buffering/flushing, partial-output primitives, and fmt usage only behind the approved facade. | reproduced |
| A5 | The CLI/TTY/provenance substrate implements option-policy helpers, CLI11 gates, no-color/no-colour/`NO_COLOR` policy, TTY detection, and provenance field assembly without replacing per-binary command behavior in Arc04. | reproduced |
| A6 | The fixture/seam test substrate provides black-box process fixture support, normalization, safe timeout/resource/signal probes, and Catch2 seam-test integration where adopted, with test-only dependencies fenced from release outputs. | reproduced |
| A7 | Existing parser, grounder, engine builds and smoke probes remain behaviorally unchanged through Arc04 except for explicitly documented inert wiring; Arc05 remains the owner of user-visible command migration. | reproduced |
| A8 | Arc05 and Arc06 can be planned from the Arc04 close without silent drops: per-binary adoption inputs, dependency/license obligations, release-packaging constraints, CI gates, and wolong proof obligations are routed. | reproduced |

## 8. Open questions and risks

- **OQ1 - runtime root chosen for now.** Slice01 should create
  `pandapi-runtime/` as a sibling of `pandaPI/`. If implementation finds a
  better build-compatible spelling, it must bubble that up before later slices
  depend on it.
- **OQ2 - dependency import timing.** Slice01 must not vendor external
  dependencies. fmt, CLI11, Catch2, `tl::expected`, and reproc++ enter only in
  the slice that owns their facade or test integration, with license and
  packaging notes.
- **OQ3 - CMake versus inherited builds.** The runtime can use CMake for its
  own target even though parser/grounder still use Makefiles. Cross-component
  binary wiring waits until Arc05 unless slice06 proves inert build integration
  is necessary.
- **OQ4 - no behavior-change proof.** Because Arc04 adds substrate before
  adoption, its tests must prove helpers and existing binaries separately.
  Passing helper tests does not imply managed-process conformance until Arc05.
- **OQ5 - child-process adapter remains conditional.** reproc++ should not
  enter unless a supported boundary needs it. Arc03 currently leaves H2 and
  translation out of the supported default path.

## 9. Version history

- **v2.1 - 2026-08-09.** Marked slice06
  substrate-integration-readiness closed and CDC-verified. Surfaced by:
  slice06 CDC verification of commit
  `f5f04a9de0ddc93840d375adb9c993d2c6b65dd9`. Why: Arc04 now has an accepted
  readiness handoff, inert build-all probe, Arc02 dependency-state map, Arc03
  coverage map, Arc05 adoption/fixture inputs, Arc06 release/wolong/license
  obligations, and preserved no-binary-adoption/no-release-change boundaries;
  Arc04 can proceed to arc-level CDC close.
- **v2.0 - 2026-08-09.** Opened slice06
  substrate-integration-readiness. Surfaced by: slice05 CDC verification. Why:
  Arc04 now has accepted runtime build, status/result, diagnostics/status I/O,
  CLI/TTY/provenance, and fixture/seam-test substrate evidence, so the final
  implementation slice can synthesize integration readiness, add an inert
  all-build probe, route Arc02 dependency state, preserve the Slice05
  stdin/full-duplex caveat, and hand Arc05/Arc06 concrete adoption and release
  proof inputs without changing binary behavior.
- **v1.9 - 2026-08-09.** Marked slice05
  fixture-and-seam-test-substrate closed and CDC-verified. Surfaced by:
  slice05 CDC verification of commit
  `4741113b744a5b2e7c39c9ee8d16ad0dea7ada56`. Why: Arc04 now has tested
  fixture records, normalization helpers, process observation, wait-status and
  signal mapping, safe timeout/resource/signal policy vocabulary, structured
  comparison, final tagged-status matching, and optional local Catch2 seam
  integration without binary adoption or release-shape change. Slice06 can
  synthesize integration readiness and Arc05 handoff constraints from accepted
  substrate evidence.
- **v1.8 - 2026-08-09.** Opened slice05
  fixture-and-seam-test-substrate. Surfaced by: slice04 CDC verification. Why:
  Arc04 now has accepted status/result, status I/O, CLI policy, TTY/color, and
  provenance helpers, so the runtime can add fixture records, normalization,
  process observation, safe timeout/signal/resource policy, and test-only
  Catch2 gates before Arc05 begins executable conformance migration.
- **v1.7 - 2026-08-09.** Marked slice04 cli-tty-provenance-core closed and
  CDC-verified after Iteration 01. Surfaced by: slice04 CDC verification of
  commit `56966f989888f39c20030b0c99ca32c4a5d3f8d8`. Why: Arc04 now has
  tested common CLI policy, command identity, informational-command,
  TTY/color, ANSI legality, and required version/provenance helpers without
  binary adoption or CLI11 import; slice05 can build fixture and seam-test
  substrate on accepted runtime policy inputs.
- **v1.6 - 2026-08-09.** Opened slice04 cli-tty-provenance-core. Surfaced by:
  slice03 CDC verification. Why: Arc04 now has accepted status/result and
  diagnostics/status I/O helpers, so the runtime can add common CLI policy,
  TTY/color policy, command identity, and provenance/version assembly while
  preserving the CLI11 gate and no-binary-adoption boundary.
- **v1.5 - 2026-08-09.** Marked slice03 diagnostics-status-io closed and
  CDC-verified after Iteration 01. Surfaced by: slice03 CDC verification of
  commit `ece82e0e311ab2e63c922a6b679bb3ae647fdbec`. Why: Arc04 now has a
  tested standard-library-only status I/O facade, tagged `PANDAPI_STATUS`
  writer/parser, stdout/stderr status-stream legality, final flush,
  partial-output policy, and overflow-safe numeric parsing; slice04 can build
  CLI/TTY/provenance policy on accepted helpers without binary adoption.
- **v1.4 - 2026-08-09.** Opened slice03 diagnostics-status-io. Surfaced by:
  slice02 CDC verification. Why: Arc04 now has accepted `ProcessStatus`,
  Arc03 status/exit mapping, status-class vocabulary, and a local
  standard-library-only `StatusResult<T>` facade, so the diagnostics/status
  I/O layer can implement tagged-text `PANDAPI_STATUS`, stdout/stderr
  ownership, flushing, and partial-output primitives while preserving the fmt
  dependency gate and no-binary-adoption boundary.
- **v1.3 - 2026-08-09.** Marked slice02 status-result-core closed and
  CDC-verified. Surfaced by: slice02 CDC verification of commit
  `e277c0a568269d18d870789f8e6b60ada479c7a9`. Why: Arc04 now has tested
  `ProcessStatus`, Arc03 status/exit mapping, status-class vocabulary, and a
  local standard-library-only `StatusResult<T>` facade; slice03 can implement
  diagnostics/status I/O against those APIs.
- **v1.2 - 2026-08-09.** Opened slice02 status-result-core. Surfaced by:
  slice01 CDC verification. Why: Arc04 now has an accepted runtime root,
  namespace, CMake target, CTest hook, and dependency-gate baseline, so the
  Arc03 status taxonomy can become tested C++ runtime code without binary
  adoption.
- **v1.1 - 2026-08-09.** Marked slice01 runtime-build-skeleton closed and
  CDC-verified. Surfaced by: slice01 CDC verification of commit
  `2732865d78b5a4dc7330eac7f4ce1164627a3bc2`. Why: the chengdu-owned runtime
  root, namespace, static CMake target, smoke test, and build entrypoint are
  accepted; slice02 can now add the status/result substrate without changing
  binary behavior.
- **v1.0 - 2026-08-09.** Initial Arc04 plan and slice01 open set. Source:
  Arc03 close, final managed-process contract, Arc02 dependency portfolio, and
  current build-script inspection. Why: the project can now move from accepted
  process-contract design into a chengdu-owned shared C++ runtime/build
  substrate without beginning per-binary behavior migration.
