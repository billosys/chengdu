# chengdu arc02 / slice03 - engine-library-research - closing report

Date: 2026-08-09

## 1. Summary

The engine audit strengthens the parser/grounder dependency direction and adds
two engine-specific boundaries that must stay explicit: translation child
processes and optional SAT/BDD/CUDD support. The strongest near-term candidates
for 0.3.0 remain shared process-infrastructure libraries, not planner-algorithm
or performance-container libraries.

For engine planning, the recommended shape is:

- **Standard-library modernization** as the baseline: scoped streams,
  `std::filesystem`, `std::chrono`, `std::unique_ptr`, `std::vector`, local
  hashers, checked model-reading helpers, and explicit supported-surface flags.
- **CLI11** as the leading CLI replacement candidate for the future
  `pandapi-engine` command surface, gated by golden help/status/stdout/stderr
  tests.
- **fmt** as the formatting primitive underneath one shared chengdu
  diagnostics/process facade.
- **tl::expected** as a shared status/result pilot candidate now that all three
  components show deep exit/status-propagation defects.
- **Catch2** as the C++ test framework candidate for engine-local seams plus a
  process fixture runner for binary contracts.
- **reproc++** only as a conditional pilot if translation mode remains in the
  supported managed-process contract and cannot be removed or isolated with a
  small local Linux/macOS child-process wrapper.

The audit does not justify enabling or replacing CUDD, importing Abseil as a
foundation library, replacing hash containers, or adding a threading framework
for SAT. Those may become real later, but only after supported-surface and
measurement gates.

### Supported-Surface Containment

The engine differs from parser and grounder because several risky behaviors are
optional or legacy-adjacent: interactive mode, translation mode, SAT, and BDD.
That changes the library decision. A library can make a risky path cleaner, but
it can also accidentally make the path feel first-class before the product
contract has accepted it.

The preferred sequence is:

1. In Arc03, classify each path as supported, unsupported, hidden/experimental,
   or split into a separate future deliverable.
2. For unsupported or experimental paths, fence the CLI and documentation first.
   Do not adopt a runtime dependency merely to improve a path outside 0.3.0.
3. For supported translation mode, remove shell-string construction and fixed
   filenames behind an explicit child-process/output-directory/status boundary.
4. Only then decide whether the boundary needs `reproc++`, Boost.Process, or a
   small local POSIX wrapper for the Linux/macOS release matrix.
5. For supported SAT/BDD, open separate dependency/concurrency slices. Do not
   smuggle CUDD, real-time signal scheduling, or worker-thread `exit()` into the
   default engine contract through this research slice.

This keeps the library recommendations focused on PANDA framework value rather
than making optional inherited infrastructure bigger and shinier.

## 2. Audit Mapping

| Audit finding | Defect class | Candidate bucket |
|---------------|--------------|------------------|
| E-001 | unchecked model parsing and release-disabled asserts | checked parser helpers; `tl::expected`; GSL-style bounds; tests |
| E-002 | deep helper `exit()` calls | status/result model; process facade; `tl::expected` |
| E-003 | incomplete exit taxonomy; timeout shares unsolvable | status enum; process facade; event/status output |
| E-004 | stdout mixes metadata/progress/statistics/status/plan data | stream router; diagnostics facade; optional JSON Lines event mode |
| E-005 | ANSI color lacks TTY/no-color policy | shared color/diagnostic policy; CLI parser |
| E-006 | inherited binary identity/version | CLI parser; version/provenance facade |
| E-007 | interactive mode reachable from public CLI | CLI parser; TTY policy; process contract tests |
| E-008 | timeout/signal split between polling and wrapper kill | `std::chrono`; signal/resource policy; process fixture tests |
| E-009 | optional SAT path uses process signals, threads, and exits | supported-surface decision; no near-term library; possible future concurrency slice |
| E-010 | no engine-local tests or fixture oracle | Catch2; process fixture runner |
| E-011 | warnings visible but nonfatal | build/tooling policy; test target; no runtime library |
| E-012 | raw-pointer-heavy model/search/heuristic ownership | standard-library RAII and containers; GSL narrow pilot only if needed |
| E-013 | public headers leak namespace policy/specializations | standard-library local hashers; header hygiene; no broad dependency |
| E-014 | process-policy duplication across parser/engine | shared process facade; CLI11; fmt; `tl::expected`; Catch2 |
| E-015 | translation mode shells out with fixed filenames and weak cleanup | `std::filesystem`; child-process containment; conditional `reproc++` pilot |
| E-016 | optional bundled CUDD/BDD build needs separate decision | CUDD supported-surface audit; hold dependency changes |

## 3. Standard-library Baseline

Disposition: **adopt as default design posture**.

Engine findings E-001, E-008, E-012, E-013, E-015, and E-016 first need ordinary
C++17 discipline:

- Use scoped `std::ifstream` and ordinary stack objects at the process boundary
  rather than raw-owned stream pointers.
- Convert model/search/heuristic outer ownership to `std::unique_ptr`,
  `std::vector`, and scoped owner objects before deeper representation changes.
- Use `std::filesystem::path` for input/output validation, translation output
  directories, fixed-file replacement, and future temp-workspace ownership.
- Use `std::chrono` types for timeout accounting and reporting even before
  signal/resource handling is fully redesigned.
- Replace release-disabled parse assertions with checked extraction helpers
  that validate counts, indexes, and section markers before allocation or
  indexing.
- Remove public-header `using namespace` leakage and keep hash specializations
  out of `std` unless they are standard-permitted specializations for local
  types and narrowly documented.

Why this matters: the C++ Core Guidelines recommend library use, but also prefer
the standard library when it is enough. Engine has large defect classes that
should not wait on dependency adoption.

## 4. Candidate Assessment

### CLI11

Disposition: **adopt or pilot for Arc03/Arc05 CLI redesign**.

Audit fit: E-003, E-005, E-006, E-007, E-014, plus parser/grounder CLI findings.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ command-line parser with help, validation, option groups, positional arguments, parse-error handling, subcommands, and direct typed values. |
| License | BSD-3-Clause style license; current license text identifies CLI11 2.7.2 and redistribution conditions. |
| Maintenance | Current GitHub releases show `v2.7.2` released 2026-08-02; recent 2.7.x notes include parsing, help, type-conversion, and precompiled-build work. |
| Integration | Header-only by default; single-header inclusion and precompiled mode are available. |
| Build/packaging | Low runtime packaging cost if vendored/pinned with license and NOTICE updates. |
| Risk | Replacing `gengetopt` changes generated help, parse errors, defaults, and option normalization. Golden CLI tests must land with the pilot. |

Recommendation: engine completes the three-component case for CLI11. It should
be the default shared CLI candidate for `pandapi-parser`, `pandapi-grounder`,
and `pandapi-engine`, unless Arc03 deliberately keeps `gengetopt` for the first
contract slice to reduce migration risk.

### fmt

Disposition: **adopt as the formatting primitive for diagnostics and structured
messages**.

Audit fit: E-004, E-005, E-006, E-014, plus parser/grounder diagnostics
findings.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Type-safe formatting and direct printing alternative to iostream formatting and C stdio. |
| License | MIT. |
| Maintenance | Current GitHub releases show `12.2.0` as latest and `12.1.0` with active late-2025 maintenance. |
| Integration | Small C++ library with CMake/package support and optional header-only mode. |
| Build/packaging | Good fit for static release assets if pinned and noticed. |
| Risk | fmt does not define stream ownership, event schemas, or color policy; it must sit below a chengdu facade. |

Recommendation: adopt fmt beneath one diagnostics/process facade. Do not spread
direct `fmt::print` calls through planner internals before stream ownership is
decided.

### tl::expected

Disposition: **pilot as the shared status/result propagation candidate**.

Audit fit: E-001, E-002, E-003, E-008, E-014, E-015, plus parser/grounder deep
exit and parse/status findings.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | `std::expected`-style result values for C++11/14/17 with functional-style extensions. |
| License | CC0-1.0/public-domain style waiver. |
| Maintenance | Project remains the canonical small pre-C++23 `expected` option; package metadata should be pinned before vendoring. |
| Integration | Header-only/single-header style is suitable for a contained pilot. |
| Build/packaging | Very low runtime footprint if vendored with provenance. |
| Risk | Can leak into public headers before the process taxonomy is stable; C++23 `std::expected` is the eventual standard-library destination. |

Recommendation: engine tips this from "promising" to "pilot in shared runtime".
The first surface should be narrow, such as `expected<EngineResult,
EngineError>` or component-specific aliases behind the chengdu process facade.

### Catch2

Disposition: **adopt or pilot for C++ unit tests and fixture-backed seams**.

Audit fit: E-001, E-003, E-004, E-007, E-008, E-010, E-011, plus parser/grounder
test gaps.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ unit/TDD/BDD test framework for C++14, C++17, and later. |
| License | BSL-1.0. |
| Maintenance | Current GitHub releases show latest `v3.15.3` on 2026-07-26. |
| Integration | v3 is a compiled test dependency; works best once engine code has testable seams. |
| Build/packaging | Test-only dependency, but likely requires CMake/test-target work. It should not affect release assets. |
| Risk | Does not replace process-level fixture tests for exit status, stdout/stderr, timeout, signal, help, and version behavior. |

Recommendation: adopt/pilot Catch2 as the C++ test framework, paired with a
chengdu process fixture runner. Engine especially needs both: unit seams for
checked SAS/model parsing and process probes for managed-command behavior.

### reproc++

Disposition: **conditional pilot for translation child-process containment**.

Audit fit: E-015; possibly E-008 if external supervision remains relevant.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Cross-platform C99/C++11 process library for starting, stopping, and communicating with external programs. |
| License | MIT. |
| Maintenance | Conan Center lists `reproc/14.2.7`; upstream README documents CMake integration, package-manager options, C and C++ libraries, stream communication, wait/terminate/kill behavior, and error-code handling. |
| Integration | C library plus C++ wrapper; CMake-oriented, but small enough for a contained adapter pilot. |
| Build/packaging | Adds runtime/library packaging if adopted; object-library or vendored-static integration may avoid separate shipped dylibs. |
| Risk | Adopting it implicitly blesses translation mode unless Arc03 first accepts that path into the 0.3.0 contract. |

Recommendation: do not adopt for the default engine path. If translation mode is
supported in 0.3.0, pilot `reproc++` against a small adapter that captures
stdout/stderr, supplies explicit input/output paths, maps child status, and
enforces timeout/cleanup. If translation is deferred or hidden, defer reproc++.

### Boost.Process

Disposition: **hold/reject for 0.3.0 engine cleanup unless Boost is adopted for
another reason**.

Audit fit: E-015; possible E-008.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Portable process creation/management library in Boost. |
| License | Boost Software License. |
| Maintenance | Current Boost page lists Boost.Process in Boost 1.91.0, C++11, added in Boost 1.64.0. |
| Integration | Pulls broad Boost dependencies including Asio, Filesystem, Optional, System, and more. |
| Build/packaging | License is excellent, but dependency footprint is too large for a single optional translation boundary. |
| Risk | Forces a broader Boost policy before chengdu has evidence that a broad Boost dependency is worth it. |

Recommendation: do not adopt for engine first. Keep it as a fallback if a later
Arc04 build-system decision already admits Boost.

### nlohmann/json

Disposition: **hold; pilot only if Arc03 chooses JSON Lines for machine events**.

Audit fit: E-003, E-004, E-006, E-014.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | JSON for Modern C++ with single-header integration and broad package-manager support. |
| License | MIT. |
| Maintenance | Official release page shows `v3.12.0` on 2025-04-11 with backward-compatible fixes/features; README describes single-header C++11 integration and extensive testing/fuzzing. |
| Integration | Single `json.hpp`, C++11, CMake/package-manager support. |
| Build/packaging | Acceptable if JSON Lines is selected as the supervised-process event format. |
| Risk | Premature before Arc03 defines the event/status contract; JSON dependency should not lead the design. |

Recommendation: engine reinforces the hold. Adopt only if Arc03 chooses typed
JSON Lines for machine-readable status/progress/events.

### Microsoft GSL

Disposition: **hold/pilot only for narrow checked-boundary helpers**.

Audit fit: E-001, E-012, E-013.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Guidelines Support Library implementation with `not_null`, `span`, `narrow`, checked helpers, and Core Guidelines-aligned vocabulary. |
| License | MIT. |
| Maintenance | Microsoft announced GSL `v4.2.0` in 2025, including span iterator performance and modern compiler support updates. |
| Integration | Header-only, generally assumes C++14 support; overlaps with C++17 standard-library facilities. |
| Build/packaging | Low runtime cost, but introduces another vocabulary family that can spread through headers. |
| Risk | Engine public headers already have hygiene problems; adding GSL types globally before API cleanup could widen churn. |

Recommendation: do not adopt globally. Consider `gsl::narrow`, `gsl::not_null`,
or `gsl::span` only behind local checked reader/API boundaries if standard
library and local helpers are insufficient.

### Abseil

Disposition: **hold/reject for 0.3.0 engine cleanup**.

Audit fit: E-002, E-003, E-008, possible E-012/E-013.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Broad C++17 library collection including `absl::Status`, `StatusOr`, strings, time, synchronization, and hash containers. |
| License | Apache-2.0. |
| Maintenance | Current releases show Abseil LTS `20260526.0`, with status helpers and clock/test-time utilities among new items; release notes also raise compiler-floor implications. |
| Integration | CMake/Bazel/package-manager friendly, but much broader than chengdu's immediate process-runtime needs. |
| Build/packaging | Large dependency surface and Apache notice obligations. |
| Risk | Solves several classes by importing an ecosystem rather than a focused library. Compiler-floor and build-policy effects need a separate decision. |

Recommendation: hold. Engine is the strongest argument for Abseil so far, but
not strong enough for 0.3.0. Prefer a focused `tl::expected` pilot, local
`std::chrono` resource policy, and measured container work.

### tsl::robin_map / ankerl::unordered_dense

Disposition: **hold until profiling proves a container bottleneck**.

Audit fit: E-012/E-013 only indirectly; possible future performance work.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Faster hash map/set alternatives to `std::unordered_map` in specific workloads. |
| License | `tsl::robin_map` is MIT; `ankerl::unordered_dense` is MIT. |
| Maintenance | Both projects document C++ header/library integration and performance-focused hash containers. |
| Integration | Header-only or small source dependency, but touches algorithmic data structures and iterator/reference assumptions. |
| Build/packaging | Low packaging cost if vendored, but high behavioral risk without benchmarks. |
| Risk | C++ Core Guidelines `Per.6` applies: do not make performance claims without measurements. |

Recommendation: do not adopt from the audit alone. First add fixture workloads
and profiling. Replace containers only where measured representative engine
workloads prove the standard containers are the bottleneck.

### google/benchmark

Disposition: **hold/pilot as a development-only measurement dependency**.

Audit fit: E-008, E-012, E-016, future container/search performance work.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ microbenchmark support library. |
| License | Apache-2.0. |
| Maintenance | Project README describes a stable benchmarking library; it can be used from C++11 but requires C++17 to build. |
| Integration | CMake-oriented development/test dependency, not runtime. |
| Build/packaging | Should not affect release assets if kept in dev/test targets. |
| Risk | Microbenchmarks can distract from end-to-end planner workloads; engine first needs representative fixtures and status/stream tests. |

Recommendation: hold until engine contract fixtures exist and a specific hot
path is named. Then pilot as a dev-only tool for model parsing, heuristic setup,
search queue operations, or container experiments.

### CUDD

Disposition: **hold as a separate optional-dependency decision; reject enabling
or replacing for 0.3.0 by default**.

Audit fit: E-016.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Decision diagram package used by the optional BDD/symbolic-search path. |
| License | Current maintained fork source and the bundled engine tree need a dedicated license/provenance check before support expansion. |
| Maintenance | Upstream fork exists, but engine default build currently disables BDD and does not ship CUDD-linked binaries. |
| Integration | Autotools-style C dependency with CMake `ExternalProject` in engine optional path. |
| Build/packaging | Significant CI, license, security/update, and release-asset implications if enabled. |
| Risk | Enabling BDD changes supported planner capability and build matrix, not just maintainability infrastructure. |

Recommendation: keep BDD/CUDD out of the default 0.3.0 dependency portfolio.
If symbolic search becomes supported, open a dedicated dependency audit and
Linux/macOS build proof slice.

## 5. Recommendation Portfolio

### Adopt

- Standard-library RAII/path/checking/status-boundary modernization as the
  engine baseline.
- fmt as the formatting primitive inside a shared diagnostics/process facade.

### Adopt or Pilot

- CLI11 for the future `pandapi-engine` CLI redesign, gated by golden
  help/status/stdout/stderr tests and a compatibility decision for generated
  `gengetopt` behavior.
- Catch2 for C++ unit tests once checked model-reader, process facade, or
  shared runtime seams exist.

### Pilot

- `tl::expected` for shared status/result propagation after the taxonomy is
  defined.
- `reproc++` only if Arc03 keeps translation mode inside the 0.3.0 supported
  managed-process contract and a small local wrapper is not enough.

### Hold

- nlohmann/json until Arc03 chooses a machine-readable event/status format.
- Microsoft GSL until local checked parsing/API helpers reveal a narrow need.
- google/benchmark until fixture baselines and a specific hot path exist.
- `tsl::robin_map`/`ankerl::unordered_dense` until profiling proves standard
  containers are a bottleneck.
- Abseil unless the combined report deliberately chooses a broader foundation
  library strategy.
- CUDD/BDD support until a dedicated optional-dependency audit and build proof
  slice is opened.

### Reject for 0.3.0 Engine Cleanup

- Boost.Process as the first process-containment dependency; reproc++ or a
  small Linux/macOS wrapper is a better first pilot if translation is supported.
- Any SAT threading/signal framework adoption from this audit alone.
- Any allocator or hash-container replacement without representative
  measurements.
- Enabling or upgrading CUDD as part of the default engine build.

## 6. Bubble-up to the Arc

This slice delivered its assigned piece: it applied the Arc02 reusable workflow
to the accepted engine audit and produced a comparable library-research report.

What the arc plan should preserve for slice04:

- All three component reports now support `CLI11`, `fmt`, `tl::expected`, and
  `Catch2` as the strongest shared candidates.
- Engine confirms that `tl::expected` should be piloted in the shared
  status/result substrate, not only component-local code.
- Engine adds a stronger supported-surface gate: optional translation, SAT, and
  BDD/CUDD paths must be classified before a library recommendation can make
  them part of 0.3.0.
- Process containment remains conditional. Grounder may need it for H2; engine
  may need it for translation. The final report should separate those two
  reasons and avoid adopting a process library unless at least one supported
  path requires it.
- Performance libraries remain behind measurement. Engine is performance
  sensitive, but the audit did not provide profile evidence.

Silent-drop diff: no requested engine research category was dropped. Final
cross-codebase recommendation remains deferred to slice04, which now has all
three component reports.

## 7. Ledger Walk

| Row | Final status | Evidence |
|-----|--------------|----------|
| F-1 | done | Section 2 maps engine findings E-001 through E-016 to candidate buckets. |
| F-2 | done | Section 4 records license, maintenance/release evidence, integration, build/packaging, risk, and disposition for adopted/piloted candidates. |
| F-3 | done | Section 5 separates adopt, adopt/pilot, pilot, hold, and reject decisions. |
| F-4 | done | Sections 1, 4, 5, and 6 cover translation/SAT/BDD/CUDD boundaries, `reproc++`, Boost.Process, child-process containment, and dependency-internal scope limits. |
| F-5 | done | Section 8 records upstream source links used for candidate assessment. |
| F-6 | done | This slice changed Arc02 planning/research docs and the project/arc status surfaces only; verification command recorded in `ledger.md`. |

## 8. Sources

- CLI11: <https://github.com/CLIUtils/CLI11>
- CLI11 releases: <https://github.com/CLIUtils/CLI11/releases>
- CLI11 license: <https://raw.githubusercontent.com/CLIUtils/CLI11/main/LICENSE>
- fmt: <https://github.com/fmtlib/fmt>
- fmt releases: <https://github.com/fmtlib/fmt/releases>
- tl::expected: <https://github.com/TartanLlama/expected>
- Catch2: <https://github.com/catchorg/Catch2>
- Catch2 releases: <https://github.com/catchorg/Catch2/releases>
- reproc: <https://github.com/DaanDeMeyer/reproc>
- reproc Conan Center recipe: <https://conan.io/center/recipes/reproc>
- Boost.Process: <https://www.boost.org/library/latest/process/>
- Boost.Process source: <https://github.com/boostorg/process>
- nlohmann/json: <https://github.com/nlohmann/json>
- nlohmann/json releases: <https://json.nlohmann.me/home/releases/>
- Microsoft GSL: <https://github.com/microsoft/GSL>
- Microsoft GSL license: <https://raw.githubusercontent.com/microsoft/GSL/main/LICENSE>
- Microsoft GSL 4.2.0 announcement: <https://devblogs.microsoft.com/cppblog/announcing-guidelines-support-library-v4-2-0/>
- Abseil: <https://github.com/abseil/abseil-cpp>
- Abseil releases: <https://github.com/abseil/abseil-cpp/releases>
- robin-map: <https://github.com/Tessil/robin-map>
- ankerl::unordered_dense: <https://github.com/martinus/unordered_dense>
- google/benchmark: <https://github.com/google/benchmark>
- CUDD maintained fork: <https://github.com/ivmai/cudd>
