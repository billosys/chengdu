# chengdu arc04 / slice01 - runtime-build-skeleton - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Create the first buildable chengdu-owned runtime substrate skeleton:

`pandapi-runtime/`

The skeleton establishes source layout, C++ namespace, build/test entrypoint,
and documentation for later Arc04 helpers. It must be intentionally small:
enough to prove the new substrate can build and test on the local platform, but
not enough to change parser, grounder, engine, release assets, or wolong-facing
behavior.

## 2. Scope

**In:**

- Create `pandapi-runtime/` as a sibling of `pandaPI/`.
- Create public include, source, test, and CMake helper directories.
- Add a minimal `pandapi::runtime` library target with self-contained headers
  and at least one tiny test/smoke executable.
- Add `scripts/build-runtime.sh` as the runtime build/test entrypoint.
- Document the runtime boundary, source layout, C++17 floor, no-behavior-change
  rule, Arc03 inputs, Arc02 dependency gates, and later-slice ownership.
- Prove the runtime skeleton builds and tests.
- Prove existing parser, grounder, and engine smoke behavior is not changed by
  this slice.

**Out:**

- No edits under `pandaPI/`.
- No parser, grounder, or engine behavior changes.
- No `pandapi-*` binary aliases or wrappers.
- No status taxonomy implementation beyond harmless placeholder/skeleton code.
- No diagnostics/status I/O facade, CLI11 wrapper, fmt integration, Catch2
  import, `tl::expected` import, reproc++ adapter, provenance assembler, or
  process fixture harness.
- No workflow, README, release asset, `release-manifest.txt`, `vendor.env`,
  `pins.env`, `dist`, or `build` changes committed.
- No slice close artifacts at open time other than CC's eventual
  `closing-report.md`; leave `cdc-verification.md` for CDC.

## 3. Required Decisions

CC must decide and record:

- the exact minimal CMake shape for `pandapi-runtime`;
- whether the skeleton produces a static library, object library, or both;
- the minimal smoke/seam test shape that does not require external
  dependencies yet;
- how `scripts/build-runtime.sh` chooses a build directory without polluting
  tracked source;
- how the runtime README names later-slice responsibilities without implying
  those helpers already exist;
- whether any existing build-script convention needs to be mirrored or avoided.

## 4. Constraints

- Use C++17; do not require C++20 or C++23.
- Keep headers self-contained and include-guarded or `#pragma once` consistently.
- Use namespace `pandapi::runtime`.
- Follow C++ Core Guidelines pressure for new code: explicit interfaces,
  standard-library-first design, RAII, no raw ownership transfer, no mutable
  globals, no header object definitions, no global `using namespace`, and no
  hidden third-party APIs.
- Do not add external dependencies in this slice.
- Do not make existing binary builds depend on the runtime skeleton unless the
  dependency is explicitly inert and the existing smoke gates still pass.
- Do not write close-set files before the slice is complete.

## 5. Verification Approach

CC verifies by building and testing `pandapi-runtime`, checking the new source
layout and documentation with `rg`, running existing parser/grounder/engine
build and negative smoke gates, and confirming no protected behavior/release
paths changed. CDC will rerun the ledger commands and inspect the diff.

## 6. Exit Criteria

The ledger reaches final status. `pandapi-runtime/` exists, builds, and tests
through `scripts/build-runtime.sh`; the runtime skeleton is documented and
standard-library-only; existing binary smoke behavior remains unchanged; and no
inherited planner source or release-shape path is modified.
