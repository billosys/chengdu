# chengdu arc02 / slice02 - grounder-library-research - closing report

Date: 2026-08-09

## 1. Summary

The grounder audit confirms the parser report's main dependency direction and
adds one important grounder-specific track: dependency-boundary containment.
For near-term 0.3.0 planning, the strongest candidates are:

- **Standard-library modernization** as the baseline: scoped streams,
  `std::filesystem`, checked parsing helpers, explicit local hashers,
  `std::chrono`-based limit bookkeeping, and visible mutable working models.
- **CLI11** as the leading replacement candidate for `gengetopt` and the
  inherited optional-stdin/stdout argument behavior.
- **fmt** as the formatting primitive underneath one shared chengdu
  diagnostics/process facade.
- **tl::expected** as a stronger cross-codebase pilot candidate now that both
  parser and grounder show deep `exit()`/status-propagation defects.
- **Catch2** as the C++ test framework candidate, plus process fixture tests
  around parser-generated `.htn` inputs.
- **reproc++** as a grounder-specific pilot candidate if H2 cannot be kept
  in-process behind a safe adapter and explicit output sink.

The grounder audit does not justify performance container or allocator adoption
yet. It justifies measurement infrastructure and warning/test gates first.

### Dependency-Process Containment

The grounder has a dependency-boundary problem that the parser does not:
grounder-owned code compiles H2 internals directly, copies cpddl internals, and
suppresses H2 output by mutating global `std::cout` state. That makes dependency
behavior part of the grounder process contract without giving chengdu a clean
place to classify dependency failure, timeout, stdout/stderr output, or cleanup.

The preferred sequence is:

1. Try a narrow in-process adapter first, with explicit input/output/error
   channels and no global stream poisoning.
2. If the dependency cannot provide that boundary, pilot subprocess containment:
   run the dependency through a small grounder-owned adapter that captures
   stdout/stderr, maps exit status, enforces timeout/resource policy, and returns
   a typed result to `main()`.
3. Only after that pilot decide whether the adapter needs a library such as
   `reproc++` or whether a local Linux/macOS wrapper is enough for 0.3.0.

This is not a recommendation to split every dependency into a subprocess. It is
a recommendation to make the dependency/process boundary explicit where the
audit already shows uncontrolled process-surface leakage.

## 2. Audit Mapping

| Audit finding | Defect class | Candidate bucket |
|---------------|--------------|------------------|
| G-001 | string-literal pointer arithmetic in H2 integration | standard library; test oracle; warning gate |
| G-002 | unchecked counts/indexes from `.htn` stream | checked parser helpers; GSL-style bounds; status/result model; tests |
| G-003 | release-disabled validation asserts | status/result model; test oracle |
| G-004 | deep `exit(-1)`/`_exit(0)` from helpers | status/result model; process facade |
| G-005 | missing args become stdin/stdout execution | CLI parser; process fixture tests |
| G-006 | output opened after parse failure | RAII streams; status/result model; CLI facade |
| G-007 | stdout mixes config/progress/data | diagnostics/process facade; formatting/logging |
| G-008 | H2 output suppressed by poisoning `std::cout` | RAII stream guard; dependency adapter; subprocess containment |
| G-009 | debug color has no TTY/no-color policy | diagnostics/color facade |
| G-010 | inherited binary name/version | CLI parser; version/provenance facade |
| G-011 | help text/status policy gaps | CLI parser; process contract tests |
| G-012 | stale upstream `build.sh` path | build/tooling policy; no runtime library |
| G-013 | dependency internals compiled/copied through grounder | dependency adapter; subprocess containment; build boundary |
| G-014 | no primary grounder test/fixture posture | C++ test framework; process fixture runner |
| G-015 | warning policy lets primary warnings through | build/tooling policy; test framework |
| G-016 | raw `new` streams leak | standard-library RAII |
| G-017 | `std::hash` specializations inside `std` | explicit local hashers; possible hash-container pilot later |
| G-018 | `const_cast` mutation of input | visible mutable working model; API boundary |
| G-019 | resource limits documented but not enforced | process contract; `std::chrono`; benchmark/resource probes |

## 3. Standard-library Baseline

Disposition: **adopt as default design posture**.

Grounder findings G-001, G-002, G-006, G-008, G-016, G-017, G-018, and G-019
first need ordinary C++17 discipline:

- Use scoped `std::ifstream`/`std::ofstream` and references instead of raw
  owning stream pointers.
- Use `std::filesystem::path` for input/output validation and explicit `-`
  stdin/stdout handling.
- Use `std::string("var") + std::to_string(...)` rather than pointer arithmetic
  on string literals.
- Use checked parsing helpers for counts, indexes, and cross references before
  indexing vectors.
- Move `std::hash` extensions out of namespace `std` into local hasher types.
- Make mutation explicit by passing mutable working models or constructing a
  transformed model value.
- Use `std::chrono` types for future timeout/resource accounting, even if the
  first enforcement layer is an external supervisor.

Why this matters: the C++ Core Guidelines standard-library rules support
library use, but prefer the standard library where it solves the problem. The
grounder has several defects that disappear through baseline C++17 without
adding release-packaging risk.

## 4. Candidate Assessment

### CLI11

Disposition: **adopt or pilot for Arc03/Arc05 CLI redesign**.

Audit fit: G-005, G-010, G-011, plus parser P-005/P-008.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ command-line parser with help, validation, option groups, positional arguments, parse-error handling, and generated exit behavior. |
| License | BSD-3-Clause. |
| Maintenance | Current Homebrew formula shows stable `2.6.2`; project README describes header-only/single-file inclusion and no dependencies beyond C++11. |
| Integration | Header-only or single-header; suitable for makefile-era pilot before a larger build-system decision. |
| Build/packaging | Low runtime packaging cost if vendored/pinned with license and NOTICE updates. |
| Risk | Replacing `gengetopt` changes help formatting, default handling, and parse errors. Golden CLI tests must land first or with the pilot. |

Recommendation: grounder strengthens the parser recommendation. CLI11 should be
the default candidate for a shared `pandapi-*` CLI surface unless the engine
audit reveals a blocker.

### fmt

Disposition: **adopt as the formatting primitive for diagnostics and structured
messages**.

Audit fit: G-001, G-007, G-009, G-010, G-011, plus parser P-006/P-007/P-014.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Type-safe formatting and direct printing alternative to C stdio and iostream formatting. |
| License | MIT. |
| Maintenance | Current GitHub metadata reports release `12.1.0` on 2025-10-29 and broad package-manager support. |
| Integration | Small C++ library, optional header-only mode, no external runtime dependency. |
| Build/packaging | Good fit for static release assets if pinned and noticed. |
| Risk | fmt does not itself define stdout/stderr/color policy; it must sit below a chengdu facade. |

Recommendation: adopt fmt underneath a shared diagnostics/process facade. For
grounder specifically, use it to avoid hand-built string/ANSI messages and to
make the H2 variable-name bug class harder to write.

### tl::expected

Disposition: **pilot as a shared status/result propagation candidate**.

Audit fit: G-002, G-003, G-004, G-006, G-019, plus parser P-001/P-004/P-009.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Single-header implementation of `std::expected`-style result values for C++11/14/17. |
| License | CC0-1.0/public-domain style waiver. |
| Maintenance | Project releases include `v1.3.1`; package availability exists through common C++ package ecosystems. |
| Integration | Header-only/single-header style makes a contained pilot easy. |
| Build/packaging | Very low runtime footprint if vendored with provenance. |
| Risk | Could leak into public headers before chengdu's error taxonomy is stable. C++23 `std::expected` is the eventual standard-library path. |

Recommendation: parser plus grounder evidence is now enough to justify a shared
pilot. Keep the first API narrow, e.g. `expected<GroundedModel, GrounderError>`
behind a process facade, and do not commit all implementation internals to
`tl::expected` until engine evidence is known.

### Catch2

Disposition: **adopt or pilot for C++ unit tests and fixture-backed seams**.

Audit fit: G-001, G-002, G-003, G-005, G-014, G-015, plus parser P-010.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ unit/TDD/BDD test framework for C++14/17 and later. |
| License | BSL-1.0. |
| Maintenance | Current GitHub releases page shows latest `v3.15.3` on 2026-07-26. |
| Integration | v3 is a compiled test dependency; good for C++ library seams, not a runtime dependency. |
| Build/packaging | Test-only dependency, but likely requires a build-system/test-target improvement. |
| Risk | Does not replace process-level fixture tests. Grounder still needs binary probes for status/stdout/stderr/output files. |

Recommendation: adopt/pilot Catch2 once shared runtime or parser/grounder
library seams exist. Pair it with a process fixture runner that feeds
parser-generated `.htn` into the grounder and asserts status, stdout, stderr,
and output-file behavior.

### reproc++

Disposition: **pilot for H2 subprocess containment if an in-process adapter is
not enough**.

Audit fit: G-008, G-013, G-019.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Cross-platform C99/C++11 process library for starting, stopping, and communicating with external programs. |
| License | MIT. |
| Maintenance | Conan Center lists `reproc/14.2.7` published 2026-04-28 with Linux, Windows, macOS, macOS Apple Silicon, and Windows ARM64 packages; Bazel Central Registry lists `14.2.5`. |
| Integration | C library plus C++ wrapper; CMake-oriented, but small enough for a contained adapter pilot. |
| Build/packaging | Adds runtime/library packaging if adopted; may be avoidable if a small POSIX-only wrapper is enough for Linux/macOS. |
| Risk | Subprocess H2 integration changes failure modes, performance, and deployment assumptions. It is not a free replacement for a stable in-process API. |

Recommendation: first try a grounder-owned H2 adapter with explicit output sinks
or RAII stream suppression. If H2 cannot be controlled in-process, pilot
`reproc++` as the smallest credible process-capture library before considering
Boost.Process.

### Boost.Process

Disposition: **hold/reject for 0.3.0 unless Boost is adopted for another reason**.

Audit fit: G-008, G-013, G-019.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Portable process creation/management library, added to Boost in 1.64. |
| License | Boost Software License. |
| Maintenance | Boost latest library page shows Process in current Boost releases and lists broad Boost dependencies including Asio, Filesystem, Optional, and System. |
| Integration | C++11, but pulls a large Boost surface relative to the single H2 containment problem. |
| Build/packaging | License is excellent, but dependency footprint is likely too high for a small chengdu runtime addition. |
| Risk | Could force broader build-system/package policy decisions before 0.3.0 needs them. |

Recommendation: keep as a fallback if reproc++ is rejected and a broader Boost
adoption becomes justified elsewhere. Do not adopt for grounder first.

### nlohmann/json

Disposition: **hold; pilot only for the final machine-readable event/status
contract**.

Audit fit: G-007, G-010, G-011, G-019 and parser P-006/P-008.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | JSON for Modern C++, including JSON Lines documentation and single-header integration. |
| License | MIT. |
| Maintenance | Current project docs and GitHub releases show `v3.12.0` released 2025-04-11. |
| Integration | Single `json.hpp`, C++11, CMake/package-manager support. |
| Build/packaging | Acceptable if JSON Lines is selected as the supervised-process event format. |
| Risk | Premature before Arc03 chooses the event/status contract. |

Recommendation: grounder reinforces the parser hold. Revisit in Arc03 if
machine-readable events/status are JSON Lines.

### Microsoft GSL

Disposition: **hold/pilot only for checked-boundary helpers after local parsing
helpers are designed**.

Audit fit: G-002, G-016, G-018.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Guidelines Support Library implementation with `not_null`, `span`, `narrow`, `finally`, and checked helpers aligned with C++ Core Guidelines. |
| License | MIT. |
| Maintenance | Microsoft C++ blog announced GSL `v4.2.0` in 2025; later package/release metadata shows `v4.2.1` available. |
| Integration | Header-only, but overlaps with standard-library facilities and local checked parsing helpers. |
| Build/packaging | Low runtime cost, but adds another vocabulary type family to public APIs. |
| Risk | GSL adoption can sprawl into headers without first defining chengdu's local API boundaries. |

Recommendation: do not adopt globally now. Consider a small pilot for
`gsl::narrow`, `gsl::not_null`, or `gsl::at` only if local checked parsing
helpers are insufficient.

### Abseil

Disposition: **hold/reject for 0.3.0 grounder cleanup**.

Audit fit: G-004, G-017, possible G-019.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Broad C++17 library collection including `absl::Status`, `StatusOr`, strings, synchronization, and flat hash containers. |
| License | Apache-2.0. |
| Maintenance | Current release evidence shows Abseil LTS `20260526.0`; project README describes extensive Google production use and LTS releases. |
| Integration | CMake/Bazel/package-manager friendly, but much broader than chengdu's immediate needs. |
| Build/packaging | Large dependency surface and Apache notice obligations. |
| Risk | Solves several classes, but by importing an ecosystem rather than a focused library. This is premature before the engine audit and build-system decision. |

Recommendation: hold as a later "foundation library" option only if engine
evidence shows a broad need for Status/StatusOr plus flat hash containers. For
now, prefer `tl::expected`, local hashers, and measured container pilots.

### tsl::robin_map / ankerl::unordered_dense

Disposition: **hold until profiling proves container lookup is a bottleneck**.

Audit fit: G-017, possible G-019.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Faster hash map/set alternatives to `std::unordered_map` in specific workloads. |
| License | `tsl::robin_map` is MIT; `ankerl::unordered_dense` is commonly distributed as MIT/BSL-family permissive code, but exact license/version must be verified before adoption. |
| Maintenance | `tsl::robin_map` documentation describes C++11 header-only integration and package availability. |
| Integration | Header-only candidate, but touches algorithmic data structures. |
| Build/packaging | Low packaging cost if vendored, but high behavioral risk without benchmarks. |
| Risk | C++ Core Guidelines performance rule `Per.6` applies: do not make performance claims without measurements. |

Recommendation: fix `std::hash` misuse with explicit local hashers first.
Profile representative grounder workloads before adopting any replacement hash
container.

### google/benchmark

Disposition: **hold/pilot as a development-only measurement dependency**.

Audit fit: G-019 and future container/grounding performance work.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ microbenchmark support library. |
| License | Apache-2.0. |
| Maintenance | GitHub page shows latest release `v1.9.5` on 2026-01-21; docs say the library requires C++17 to build. |
| Integration | CMake-oriented test/development dependency, not runtime. |
| Build/packaging | Should not affect release assets if kept in dev/test targets. |
| Risk | Microbenchmarks can distract from end-to-end planner workloads; grounder first needs fixture/probe baselines. |

Recommendation: do not adopt before grounder process fixtures exist. Use it only
for contained microbenchmarks once a specific hot path is identified.

## 5. Recommendation Portfolio

### Adopt

- Standard-library RAII/path/checking/local-hasher modernization as the grounder
  baseline.
- fmt as the formatting primitive inside a shared diagnostics/process facade.

### Adopt or Pilot

- CLI11 for the future `pandapi-grounder` CLI redesign, gated by golden
  help/status/stdout/stderr tests.
- Catch2 for C++ unit tests once shared runtime or grounder library seams exist.

### Pilot

- `tl::expected` for shared status/result propagation after the taxonomy is
  defined.
- `reproc++` only if a safe in-process H2 adapter/output-sink boundary is not
  practical.

### Hold

- nlohmann/json until Arc03 chooses a machine-readable event/status format.
- Microsoft GSL until local checked parsing/API helpers reveal a narrow need.
- google/benchmark until fixture baselines and a specific hot path exist.
- `tsl::robin_map`/`ankerl::unordered_dense` until profiling proves standard
  containers are a bottleneck.
- Abseil unless the engine audit creates a broad foundation-library case.

### Reject for 0.3.0 Grounder Cleanup

- Boost.Process as the first process-containment dependency; reproc++ or a small
  local Linux/macOS wrapper is a better first pilot.
- Any allocator or hash-container replacement without representative
  measurements.

## 6. Bubble-up to the Arc

This slice delivered its assigned piece: it applied the Arc02 reusable workflow
to the accepted grounder audit and produced a comparable library-research report.

What the arc plan should preserve for later slices:

- Parser and grounder now both support `CLI11`, `fmt`, `tl::expected`, and
  `Catch2` as strong shared candidates.
- Grounder adds `reproc++` as a conditional dependency-boundary pilot, not a
  general process-library adoption.
- The final combined report must separate process-contract libraries from
  algorithm/performance libraries. The former are nearing enough evidence; the
  latter still need measurement.
- Dependency-internal `cpddl` and H2 work remains out of scope unless the final
  synthesis routes explicit dependency audits.

Silent-drop diff: no requested grounder research category was dropped. Final
cross-codebase recommendation remains deferred to slice04 because engine library
research is not complete.

## 7. Ledger Walk

| Row | Final status | Evidence |
|-----|--------------|----------|
| F-1 | done | Section 2 maps grounder findings G-001 through G-019 to candidate buckets. |
| F-2 | done | Section 4 records license, maintenance/release evidence, integration, build/packaging, risk, and disposition for adopted/piloted candidates. |
| F-3 | done | Section 5 separates adopt, adopt/pilot, pilot, hold, and reject decisions. |
| F-4 | done | Sections 4 and 6 cover H2/cpddl boundary options, `reproc++`, Boost.Process, adapter-first sequencing, and dependency-internal scope limits. |
| F-5 | done | Section 8 records upstream source links used for candidate assessment. |
| F-6 | done | This slice changed Arc02 planning/research docs only; verification command recorded in `ledger.md`. |

## 8. Sources

- CLI11: <https://github.com/CLIUtils/CLI11>
- CLI11 Homebrew formula: <https://formulae.brew.sh/formula/cli11>
- fmt: <https://github.com/fmtlib/fmt>
- tl::expected: <https://github.com/TartanLlama/expected>
- Catch2: <https://github.com/catchorg/Catch2>
- Catch2 releases: <https://github.com/catchorg/Catch2/releases>
- reproc: <https://github.com/DaanDeMeyer/reproc>
- reproc Conan Center recipe: <https://conan.io/center/recipes/reproc>
- reproc Bazel Central Registry: <https://registry.bazel.build/modules/reproc>
- Boost.Process: <https://www.boost.org/library/latest/process/>
- nlohmann/json: <https://github.com/nlohmann/json>
- nlohmann/json releases: <https://json.nlohmann.me/home/releases/>
- Microsoft GSL: <https://github.com/microsoft/GSL>
- Microsoft GSL 4.2.0 announcement: <https://devblogs.microsoft.com/cppblog/announcing-guidelines-support-library-v4-2-0/>
- Abseil: <https://github.com/abseil/abseil-cpp>
- Abseil LTS release evidence: <https://newreleases.io/project/github/abseil/abseil-cpp/release/20260526.0>
- robin-map: <https://tessil.github.io/robin-map/>
- google/benchmark: <https://github.com/google/benchmark>
