# chengdu arc04 / slice06 - substrate-integration-readiness - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Complete the last Arc04 implementation slice by turning the accepted runtime
substrate into an explicit integration-readiness and Arc05 handoff package.

This slice does not migrate parser, grounder, or engine behavior. It proves
that the chengdu-owned runtime is buildable as a first-class inert substrate,
records exactly what Arc04 delivered, and gives Arc05 and Arc06 a concrete
map for per-binary adoption, conformance fixtures, dependency gates, release
proof, and known caveats.

## 2. Scope

**In:**

- Add an Arc04 readiness report at
  `docs/design-v0.3.0/arc04-shared-runtime-substrate/substrate-integration-readiness.md`.
- Inventory the accepted Arc04 runtime surface:
  - runtime layout, CMake target, script entrypoint, and tests;
  - status/result core;
  - diagnostics/status I/O;
  - CLI/TTY/provenance helpers;
  - fixture, normalization, process observation, and optional Catch2 seam
    substrate.
- Map that surface back to Arc03 managed-process contract requirements and
  forward to Arc05 per-binary adoption.
- Map Arc02 dependency decisions to the actual Arc04 state:
  - standard-library baseline adopted;
  - fmt not imported unless behind an accepted facade;
  - CLI11 still gated for Arc05;
  - Catch2 optional/local/test-only;
  - `tl::expected` and reproc++ still pilots, not imported;
  - nlohmann/json, Abseil, Boost.Process, and held optional/runtime-expansion
    libraries remain out.
- Add an inert `scripts/build-all.sh` build probe. The intended shape is a
  build-only helper that runs the runtime build before the existing parser,
  grounder, and engine builds without changing any existing build script's
  semantics.
- Update `pandapi-runtime/README.md` only if needed to document the final
  Arc04 integration-readiness surface and build probe.
- Record the Slice05 CDC caveat that `run_process_fixture` is not yet a
  general full-duplex stdin transport for large stdin payloads.
- Record Arc05 handoff recommendations:
  - per-binary sequencing;
  - canonical `pandapi-*` and inherited `pandaPI*` compatibility obligations;
  - supported versus legacy/experimental/unsupported/future surface fences;
  - contract fixture categories and minimum positive/negative gates.
- Record Arc06 handoff recommendations:
  - release shape preservation;
  - wolong compatibility proof;
  - behavior-change table inputs;
  - license/NOTICE and test-only dependency exclusion proof;
  - CI/release gate implications.

**Out:**

- No edits under `pandaPI/`.
- No parser, grounder, or engine adoption of `pandapi::runtime`.
- No `pandapi-*` wrapper, symlink, copied binary, renamed binary, package
  entry point, or release asset work.
- No CLI11 import, CLI migration, `gengetopt` replacement, or per-binary
  option parser change.
- No fmt, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL,
  google/benchmark, SAT/BDD/CUDD, H2/cpddl dependency-internal work, optional
  dependency audit, or parser-generator work.
- No `FetchContent`, `ExternalProject`, package-manager bootstrap, `curl`,
  `git clone`, or network download in the default build.
- No workflow, top-level README, release asset, manifest, `vendor.env`,
  `pins.env`, `dist`, `build`, or `release` changes.
- No Arc04 closing report. Arc close happens only after Slice06 is implemented
  and CDC-verified.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- the exact semantics of the new inert `scripts/build-all.sh` build probe and
  why it does not alter existing build/release shape;
- whether `pandapi-runtime/README.md` needs a final integration-readiness
  section beyond the durable readiness report;
- the Arc05 adoption order and why that order minimizes behavior drift;
- which runtime facilities are ready for immediate Arc05 use and which need
  focused hardening before adoption;
- where the Slice05 stdin/full-duplex caveat re-enters;
- which Arc02 dependency gates remain closed, pilot-only, or test-only after
  Arc04.

If CC discovers that closing Arc04 requires a source, release, workflow, or
per-binary behavior change in this slice, stop and record an amendment request
instead of silently widening scope.

## 4. Constraints

- Use the existing `docs/design-v0.3.0/` project, arc, and slice layout.
- Preserve the Arc04 no-binary-adoption boundary.
- Keep any build-script change inert and build-only. It may call existing build
  scripts; it must not package, publish, install, rename, copy, or wrap
  binaries.
- Keep runtime dependency decisions aligned with Arc02 and Arc03. A dependency
  cannot enter because it is useful; it can enter only through its accepted
  owning gate.
- Treat Arc04 helper tests as substrate proof, not executable conformance
  proof. Arc05 owns black-box binary contract fixtures.
- Preserve C++ Core Guidelines pressure for any touched C++ or build-facing
  code: clear interfaces, RAII/resource safety, no global mutable state, no
  raw ownership transfer, no header ODR hazards, no unbounded casts or magic
  constants, and standard-library-first vocabulary.

## 5. Verification Approach

CC verifies the slice by writing the readiness report, making any inert
build-probe update, rerunning the runtime and inherited binary build gates,
rerunning negative smoke, and proving the release/workflow/inherited-source
boundary stayed clean.

CDC will rerun the ledger commands, inspect the diff, and verify that the
handoff report actually composes Arc04 into Arc05 and Arc06 planning inputs
without treating helper tests as executable managed-process conformance.

## 6. Exit Criteria

The ledger reaches final status. Arc04 has a durable readiness report and
build evidence showing that the shared runtime substrate is complete enough
to plan Arc05. The report records adopted runtime surfaces, dependency gates,
per-binary adoption inputs, release and wolong proof obligations, and known
caveats. No user-visible parser, grounder, engine, release, workflow, or
wolong-facing behavior changes occur in this slice.
