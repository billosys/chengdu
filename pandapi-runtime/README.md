# pandapi-runtime

`pandapi-runtime/` is the chengdu-owned C++ runtime substrate for the
`pandapi-*` managed-process work planned in Arc04. It lives outside `pandaPI/`
so upstream planner history remains separate from chengdu process, build, and
test policy.

## Scope

- Provide the shared `pandapi::runtime` namespace for later Arc04 helpers.
- Build as a C++17, standard-library-only library target.
- Keep public headers self-contained and implementation code under `src/`.
- Provide local smoke/seam tests for runtime code before any binary adoption.
- Support later parser, grounder, and engine migration without changing their
  current command behavior in this slice.

## Non-Goals

- No edits to inherited planner source under `pandaPI/`.
- No `pandapi-parser`, `pandapi-grounder`, or `pandapi-engine` wrappers.
- No changes to stdout, stderr, exit status, CLI parsing, provenance, release
  assets, wolong behavior, or existing build outputs.
- No external dependency import, vendoring, fetching, or hidden facade
  implementation in slice01.

## Source Layout

```text
pandapi-runtime/
  CMakeLists.txt
  cmake/
  include/pandapi/runtime/runtime.hpp
  src/runtime.cpp
  tests/runtime_smoke.cpp
```

The current CMake shape produces one static library target,
`pandapi_runtime`, with an alias target `pandapi::runtime`. The smoke test is a
small executable registered with CTest. Build output is generated under
`build/runtime/` by `scripts/build-runtime.sh`.

## C++ Floor

The runtime uses C++17. New code should prefer standard-library ownership,
paths, strings, streams, chrono/timeouts, containers, and scoped resource
management before considering third-party dependencies.

## Arc03 Inputs

Arc03 defines the managed-process contract this runtime will eventually
support. Later Arc04 slices will add shared facilities for:

- status/result types and exit-code mapping;
- diagnostics/process I/O and stdout/stderr ownership;
- tagged `PANDAPI_STATUS` writing and parsing;
- TTY/color policy;
- output finalization and partial-output policy;
- provenance/version field assembly;
- CLI parser wrapper policy;
- fixture harness support and seam tests.

Those helpers do not exist in this skeleton yet. Slice01 only establishes the
buildable runtime boundary.

## Arc02 Dependency Gates

Arc02 selected the standard library as the baseline and placed every external
candidate behind explicit gates:

- `fmt` may enter only behind a chengdu diagnostics/process facade.
- `CLI11` may enter only through the CLI parser wrapper after golden process
  tests cover help, version, provenance, parse errors, compatibility names,
  color controls, supervised mode, status output, and output/status conflicts.
- `Catch2` is test-only and must be paired with black-box process fixtures.
- `tl::expected` is only a pilot behind the local status/result substrate.
- `reproc++` is only a conditional pilot behind one child-process adapter if a
  supported boundary later requires it.
- `nlohmann/json` remains held because the selected status format is tagged
  text, not JSON Lines.
- Abseil and Boost.Process remain rejected as 0.3.0 foundation dependencies.

## No Behavior Change

Arc04 slice01 is inert with respect to the inherited binaries. Building or
testing this runtime must not change parser, grounder, or engine command names,
arguments, stdout/stderr behavior, exit codes, generated artifacts, release
shape, or wolong-facing behavior.
