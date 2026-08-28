# Slice 01 Closing Report - runtime-build-skeleton

Closer: CC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Close commit: this report's adding commit
CDC verification: pending

## Capability Verdict

Delivered. Slice01 creates a first buildable chengdu-owned runtime substrate at
`pandapi-runtime/`, outside `pandaPI/`, with a C++17 static library target,
public header, implementation source, CTest-registered smoke test, and
`scripts/build-runtime.sh` entrypoint.

The slice is intentionally inert. It does not add `pandapi-*` wrappers, change
parser/grounder/engine builds, import external dependencies, edit inherited
planner source, or touch release-shape paths.

## Implementation Decisions

- CMake shape: standalone `pandapi-runtime/CMakeLists.txt`, static library
  target `pandapi_runtime`, and alias target `pandapi::runtime`.
- C++ floor: C++17, standard-library-only, with public include root
  `pandapi-runtime/include`.
- Public API: minimal `pandapi::runtime::runtime_name()` and
  `pandapi::runtime::runtime_abi_version()` declarations to prove a compiled
  target and linked caller, without implementing future Arc03 helpers.
- Test shape: plain C++ smoke executable registered with CTest, avoiding Catch2
  until the slice that owns test-framework adoption.
- Build output: `scripts/build-runtime.sh` configures and builds under ignored
  `build/runtime/$PLATFORM`.
- README boundary: documents scope, non-goals, source layout, no behavior
  change, Arc03 future facility ownership, and Arc02 dependency gates without
  implying those helpers already exist.

## Verification

Commands run by CC:

```bash
./scripts/build-runtime.sh
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
```

Results:

- `./scripts/build-runtime.sh`: passed on `macos-arm64`; CMake built
  `libpandapi_runtime.a` and `pandapi_runtime_smoke`; CTest reported 1/1 tests
  passed.
- `./scripts/build-parser.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/build-grounder.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/build-engine.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/smoke-test.sh --negative`: passed on `macos-arm64`; 4 passed, 0
  failed.
- `git diff --check`: passed with no output.
- Protected-path diff: no output.

## Ledger Walk

Rows verified: 12. Done: 12. Deferred: 0. No-op: 0.

| Row | CC disposition |
|-----|----------------|
| F-1 | Done. `pandapi-runtime/README.md` documents scope, non-goals, source layout, C++17, no behavior change, Arc03 future helpers, and Arc02 dependency gates. |
| F-2 | Done. Planned public header, source, tests, and CMake-helper directories exist under `pandapi-runtime/`; no `pandaPI/` files changed. |
| F-3 | Done. CMake defines `pandapi_runtime`, public includes, C++17 properties, smoke executable, and CTest registration. |
| F-4 | Done. Header, implementation, and test source use `pandapi::runtime`; the header is self-contained and declaration-only. |
| F-5 | Done. `scripts/build-runtime.sh` is executable, builds out of source under `build/runtime/$PLATFORM`, and runs CTest. |
| F-6 | Done. Runtime build/test entrypoint passed. |
| F-7 | Done. No dependency fetch, package discovery, or direct external include appears in runtime source/build surfaces. The verify command was scoped away from README prose because F-1 requires documenting dependency gates by name. |
| F-8 | Done. Runtime public headers avoid global using directives, mutable namespace-scope state, object definitions, raw ownership, allocation, and deallocation patterns. |
| F-9 | Done. Generated runtime artifacts remain in ignored build output and are not staged. |
| F-10 | Done. Existing parser, grounder, and engine build scripts passed. |
| F-11 | Done. Existing negative smoke behavior passed. |
| F-12 | Done. Protected behavior/release-shape path diff produced no output. |

## Silent-Drop Check

No requested deliverable was dropped:

- `pandapi-runtime/README.md`: added.
- `pandapi-runtime/CMakeLists.txt`: added.
- `pandapi-runtime/include/pandapi/runtime/runtime.hpp`: added.
- `pandapi-runtime/src/runtime.cpp`: added.
- `pandapi-runtime/tests/runtime_smoke.cpp`: added.
- `pandapi-runtime/cmake/`: tracked with `cmake/README.md`.
- `scripts/build-runtime.sh`: added and executable.
- `ledger.md`: closed.
- `closing-report.md`: added.

## Bubble-up to Arc04

Slice01 leaves Arc04 with a working runtime source/build/test root for later
substrate slices. The next slices can add status/result, diagnostics/status
I/O, CLI/TTY/provenance, and fixture/seam-test facilities without first
solving build layout.

Open planning cautions for later slices:

- Keep third-party APIs behind local `pandapi::runtime` facades.
- Do not couple parser, grounder, or engine behavior to the runtime until the
  adoption slice owns and tests that boundary.
- Preserve release and wolong paths until Arc06 packaging proof.

## What Worked

- The standalone static-library shape kept slice01 small and buildable.
- A standard-library-only smoke test proved linkability without pulling in the
  future Catch2 dependency.
- Reusing `lib-platform.sh` kept platform naming aligned with existing build
  scripts.
