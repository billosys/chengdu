# CC assignment - chengdu arc04 / slice01 - runtime-build-skeleton

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
4. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice01-runtime-build-skeleton/slice-doc.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice01-runtime-build-skeleton/ledger.md`
6. Arc03 contract and close:
   - `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/closing-report.md`
7. Arc02 dependency portfolio:
   - `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/closing-report.md`
8. Current build scripts as implementation constraints:
   - `scripts/build-parser.sh`
   - `scripts/build-grounder.sh`
   - `scripts/build-engine.sh`
   - `scripts/smoke-test.sh`
   - `scripts/lib-platform.sh`

## The work

Create the first buildable chengdu-owned runtime substrate skeleton:

`pandapi-runtime/`

The skeleton should be small and boring in the best possible way. It proves
where shared runtime code will live, how it builds, how tests run, and how
later Arc04 slices should add helpers. It must not change parser, grounder, or
engine behavior.

Expected deliverables:

- `pandapi-runtime/README.md`
- `pandapi-runtime/CMakeLists.txt`
- `pandapi-runtime/include/pandapi/runtime/runtime.hpp`
- `pandapi-runtime/src/runtime.cpp`
- `pandapi-runtime/tests/runtime_smoke.cpp`
- `pandapi-runtime/cmake/` if useful for the build layout
- `scripts/build-runtime.sh`
- updated `ledger.md`
- `closing-report.md`

## Required behavior

The runtime README must document:

- the `pandapi-runtime/` scope and non-goals;
- the planned `pandapi::runtime` namespace;
- the C++17 floor;
- the source layout;
- the no-behavior-change rule for Arc04;
- the Arc03 facilities this runtime will eventually own: status/result,
  diagnostics/process I/O, tagged `PANDAPI_STATUS`, TTY/color policy, output
  finalization, provenance, CLI parser wrapper, fixture harness, and seam
  tests;
- the Arc02 dependency gates for fmt, CLI11, Catch2, `tl::expected`, reproc++,
  nlohmann/json, Abseil, and Boost.Process.

The C++ skeleton should:

- use namespace `pandapi::runtime`;
- be self-contained and standard-library-only;
- build with C++17;
- avoid global `using namespace`, mutable globals, raw ownership transfer,
  naked `new`/`delete`, `malloc`/`free`, and header object definitions;
- include a tiny smoke/seam test that exercises the compiled runtime target
  without testing future helpers that do not exist yet.

The build script should:

- follow the style of the existing `scripts/build-*.sh` wrappers;
- build out of source under an ignored/generated build location;
- run the runtime tests with `ctest`;
- fail loudly on compile or test failure.

## Constraints

- Do not edit anything under `pandaPI/`.
- Do not add `pandapi-*` wrappers, aliases, or release asset changes.
- Do not import fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json,
  Boost, Abseil, or any other third-party dependency in this slice.
- Do not update `.github/`, `README.md`, `release-manifest.txt`, `vendor.env`,
  `pins.env`, `dist/`, `build/`, or `release/`.
- Do not implement the status taxonomy, diagnostics facade, tagged status
  writer/parser, CLI parser wrapper, TTY/color helper, provenance assembler,
  output finalizer, child-process adapter, or process fixture harness yet.
- Do not write `cdc-verification.md`; CDC owns independent verification.

## Verification

Run every ledger command and record evidence in `ledger.md`.

Required commands include:

```bash
./scripts/build-runtime.sh
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
```

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row ledger walk and
Bubble-up to Arc04, then commit the slice.

The bubble-up should answer:

- whether `pandapi-runtime/` is the right root for later slices;
- whether the CMake/script shape needs an Arc04 plan change;
- whether any dependency import should be pulled earlier or kept in its owning
  later slice;
- whether Arc04 slice02 can proceed as planned.
