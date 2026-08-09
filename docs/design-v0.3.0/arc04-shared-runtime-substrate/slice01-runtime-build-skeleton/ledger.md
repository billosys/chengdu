# Slice 01: runtime-build-skeleton

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The chengdu-owned runtime root exists at `pandapi-runtime/` with README documentation naming scope, non-goals, Arc03 inputs, Arc02 dependency gates, source layout, and no behavior changes. | `test -f pandapi-runtime/README.md && rg -n "Scope|Non-Goals|Arc03|Arc02|Dependency Gates|Source Layout|no behavior|pandapi::runtime|status/result|diagnostics/process|CLI parser|PANDAPI_STATUS|TTY/color|output finalization|provenance|fixture harness|fmt|CLI11|Catch2|tl::expected|reproc" pandapi-runtime/README.md` | serious | Arc03 Arc04 handoff; Arc02 gates | open | | |
| F-2 | The runtime skeleton creates the planned public header, source, test, and CMake-helper directories without placing files under `pandaPI/`. | `test -d pandapi-runtime/include/pandapi/runtime && test -d pandapi-runtime/src && test -d pandapi-runtime/tests && test -d pandapi-runtime/cmake` | serious | arc04 artifact paths | open | | |
| F-3 | The runtime CMake project defines a C++17 `pandapi_runtime` target, exposes public headers, creates a smoke/seam test target, and registers that test. | `test -f pandapi-runtime/CMakeLists.txt && rg -n "cmake_minimum_required|project\\(|CXX_STANDARD 17|pandapi_runtime|add_library|target_include_directories|add_executable|enable_testing|add_test" pandapi-runtime/CMakeLists.txt` | correctness-grade | build substrate | open | | |
| F-4 | The skeleton includes a self-contained public runtime header, an implementation source, and a test source using namespace `pandapi::runtime`. | `test -f pandapi-runtime/include/pandapi/runtime/runtime.hpp && test -f pandapi-runtime/src/runtime.cpp && test -f pandapi-runtime/tests/runtime_smoke.cpp && rg -n "namespace pandapi::runtime|#pragma once|std::string|runtime" pandapi-runtime/include/pandapi/runtime/runtime.hpp pandapi-runtime/src/runtime.cpp pandapi-runtime/tests/runtime_smoke.cpp` | correctness-grade | C++ Core Guidelines SF/I/R | open | | |
| F-5 | `scripts/build-runtime.sh` exists, is executable, builds the runtime skeleton out of source, and runs the registered tests. | `test -x scripts/build-runtime.sh && rg -n "pandapi-runtime|cmake|ctest|build/runtime|CMAKE_BUILD_TYPE|Release" scripts/build-runtime.sh` | serious | build entrypoint | open | | |
| F-6 | The runtime skeleton builds and tests successfully through the new entrypoint. | `./scripts/build-runtime.sh` | serious | executable substrate proof | open | | |
| F-7 | Slice01 does not vendor or fetch external dependencies and does not introduce direct fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json, Boost, or Abseil usage. | `! rg -n "FetchContent|ExternalProject|find_package\\((fmt|CLI11|Catch2|tl|reproc|nlohmann|Boost|absl)|#include <fmt|#include \"CLI11|#include <catch2|expected.hpp|reproc" pandapi-runtime scripts/build-runtime.sh` | serious | Arc02 dependency gates | open | | |
| F-8 | New C++ headers avoid global `using namespace`, mutable namespace-scope state, non-inline object definitions, and raw ownership transfer. | `! rg -n "using namespace|extern [^;]*=|^[A-Za-z_:<>]+ \\*|new |delete |malloc\\(|free\\(" pandapi-runtime/include` | correctness-grade | C++ Core Guidelines I.2/I.11/SF.2/SF.7/R.11 | open | | |
| F-9 | Runtime build artifacts stay outside tracked source status, and no generated runtime build output is staged. | `! git status --short --ignored pandapi-runtime scripts/build-runtime.sh | rg -n "^[AM?].*pandapi-runtime/(build|cmake-build|CMakeFiles|CMakeCache|Testing)|^[AM?].*\\.o$|^[AM?].*\\.a$"` | correctness-grade | build hygiene | open | | |
| F-10 | Existing parser, grounder, and engine builds still pass after the runtime skeleton lands. | `./scripts/build-parser.sh && ./scripts/build-grounder.sh && ./scripts/build-engine.sh` | serious | no behavior drift | open | | |
| F-11 | Existing negative smoke behavior still passes after the runtime skeleton lands. | `./scripts/smoke-test.sh --negative` | serious | no behavior drift | open | | |
| F-12 | The slice does not edit inherited planner source, workflows, README, release assets, manifest, `vendor.env`, `pins.env`, `dist`, `build`, or `release` paths. | `git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release` | serious | Arc04 no-behavior/release-shape boundary | open | | |

## What Worked

_(Filled in at slice close. Patterns, practices, or decisions that made this
slice close cleanly and should be preserved or generalised.)_

## Closure

Closed at commit TBD on TBD. CDC verification: pending. Total rows: 12. Done:
0. Deferred: 0. No-op: 0.
