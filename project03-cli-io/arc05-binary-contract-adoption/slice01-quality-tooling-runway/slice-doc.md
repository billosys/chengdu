# chengdu arc05 / slice01 - quality-tooling-runway - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Put the pre-adoption C++ quality runway in place before parser, grounder, or
engine behavior changes begin.

This slice turns the accepted `ci-notes.md` conclusions into narrow,
executable repo gates: owned-source formatting, runtime sanitizer proof,
runtime/build CI coverage, and explicit documentation of what still waits
for later Arc05 or Arc06. It must make future binary adoption safer without
quietly performing binary adoption itself.

## 2. Scope

**In:**

- Add a top-level `.clang-format` style contract for chengdu-owned C/C++
  source and tests.
- Add an owned-source formatting check script. The script must scope itself to
  chengdu-owned runtime source/tests and any explicitly owned future adapter
  surface, not all vendored `pandaPI/` source.
- Prefer `clang-format` on `PATH`, but support `xcrun clang-format` on macOS
  where the Xcode toolchain provides the binary outside the shell `PATH`.
- Add a runtime ASan/UBSan script, likely `scripts/sanitize-runtime.sh`, that
  configures `pandapi-runtime/` with Clang sanitizer flags, builds it, and
  runs CTest.
- Keep sanitizer outputs under a dedicated generated build directory.
- Preserve or generate a CMake `compile_commands.json` path for
  `pandapi-runtime/`, enough to make future `clang-tidy` adoption concrete.
- Record the local `clang-tidy` state. If `clang-tidy` is unavailable in the
  default local/CI toolchain, document the install/toolchain gate rather than
  inventing a brittle check.
- Update CI so normal build gates include the runtime build and the inert
  all-build probe where appropriate.
- Add CI coverage for the new owned-source formatting and runtime sanitizer
  gates if tool availability is reliable on the selected runners.
- Update `pandapi-runtime/README.md` or a narrow docs note if needed to explain
  the new developer gates and their scope.
- Preserve `ci-notes.md` as design input rather than replacing it with the
  implementation.
- Close the slice ledger and write the CC closing report.

**Out:**

- No edits under `pandaPI/`.
- No parser, grounder, or engine adoption of `pandapi::runtime`.
- No `pandapi-*` wrapper, symlink, copied binary, renamed binary, release
  entry point, package change, or inherited-name migration.
- No CLI11 import, CLI parser migration, `gengetopt` replacement, or
  per-binary option parser change.
- No fmt, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL,
  google/benchmark, SAT/BDD/CUDD, H2/cpddl dependency-internal work, optional
  dependency audit, parser-generator work, or package-manager bootstrap.
- No `FetchContent`, `ExternalProject`, `curl`, `git clone`, or network
  download in the default build.
- No coverage threshold or `llvm-cov` gate unless the implementation proves a
  very small runtime-only report is lower risk than deferring coverage to
  fixture-backed binary adoption. Default expectation: plan coverage now,
  implement later.
- No TSan gate. TSan waits for representative subprocess, timeout/signal, and
  stream-draining fixture workloads.
- No release-package dry-run, checksum, manifest, license/NOTICE, test-only
  dependency exclusion, wolong fetch/install/migration, package, publish, or
  release workflow changes. Arc06 owns those.
- No Arc05 closing report. Arc close happens only after all Arc05 slices are
  implemented and CDC-verified.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- the exact `.clang-format` base style and any deviations chosen for the
  current `pandapi-runtime/` code shape;
- the exact owned-source file set checked by the format script and why
  vendored `pandaPI/` source is excluded;
- the sanitizer build directory, build type, compiler selection, and ASan/UBSan
  flags;
- whether sanitizer checks run on Linux only, macOS only, or both in CI, and
  why that runner set is reliable;
- how `compile_commands.json` is produced or preserved for
  `pandapi-runtime/`;
- whether `clang-tidy` is implemented now, documented as a non-blocking local
  command, or deferred behind a tool-install gate;
- whether `build-reusable.yml` should call `scripts/build-runtime.sh` and
  `scripts/build-all.sh`, or whether one subsumes the other in CI;
- which `ci-notes.md` recommendations are deliberately deferred to later
  Arc05 or Arc06.

If CC discovers that a useful tool gate requires changing parser, grounder,
engine, release packaging, or wolong-facing behavior, stop and record an
amendment request instead of widening this slice.

## 4. Constraints

- Use the existing `docs/design-v0.3.0/` project, arc, and slice layout.
- Keep this slice a quality runway, not binary adoption.
- Prefer small scripts that compose existing build entrypoints rather than
  duplicating platform/build logic.
- Keep generated outputs under `build/` or another ignored generated location.
- Treat chengdu-owned code and inherited vendored code differently. Formatting
  and sanitizer failures in owned runtime/adapters may block; inherited
  vendored warning/sanitizer volume must be triaged by ownership.
- Preserve C++ Core Guidelines pressure: self-contained headers, no header ODR
  hazards, no global mutable state, no raw ownership transfer, no hidden
  resource leaks, no unbounded arithmetic/UB hazards, and tool-supported
  enforcement where practical.

## 5. Verification Approach

CC verifies the slice by running the new format and sanitizer scripts, the
runtime build, the all-build probe, positive and negative smoke tests, and
repo hygiene checks. CI changes should be syntax-checked with actionlint if
available through the existing workflow path or local install.

CDC will rerun the ledger commands, inspect the diff, verify that the new
gates are scoped to owned source, and confirm that parser, grounder, engine,
release packaging, package/publish scripts, and wolong-facing surfaces did
not change.

## 6. Exit Criteria

The ledger reaches final status. The repo has an accepted pre-adoption quality
runway: owned-source formatting, runtime sanitizer proof, runtime/build CI
coverage, and a documented path for compile commands and clang-tidy. Deferred
coverage, TSan, release-package, license/NOTICE, and wolong proof obligations
are explicit. No parser, grounder, engine, release, package, publish, or
wolong-facing behavior changes occur in this slice.
