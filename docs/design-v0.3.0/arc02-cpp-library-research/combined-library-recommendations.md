# chengdu arc02 - combined C++ library recommendations

Date: 2026-08-09
Research HEAD: `757df527`
Branch: `release/0.3.x`

## 1. Executive Summary

Arc02 supports a small, focused portfolio for 0.3.0. The recurring defects are
not algorithm-library defects; they are process-contract, ownership, status,
diagnostic, test, and build-boundary defects across `pandaPIparser`,
`pandaPIgrounder`, and `pandaPIengine`.

Recommended portfolio:

- Standard-library baseline: adopt as the default modernization rule for all
  three binaries.
- fmt: adopt as the formatting primitive under one chengdu diagnostics/process
  facade.
- CLI11: adopt as the selected CLI replacement candidate, but land through a
  golden-test pilot before replacing generated `gengetopt` behavior.
- Catch2: adopt as the C++ seam-test framework when Arc04 introduces the shared
  runtime substrate and component-local test seams.
- tl::expected: pilot behind the shared status/result facade after Arc03 defines
  the process taxonomy.
- reproc++: conditional pilot only if Arc03 keeps H2 subprocess containment or
  engine translation mode inside the supported 0.3.0 surface and a local
  Linux/macOS wrapper is not enough.
- nlohmann/json: hold until Arc03 chooses a machine-readable event format.
- Microsoft GSL: hold globally; allow only a narrow helper pilot if local
  checked parsing and standard-library types prove insufficient.
- Abseil: reject as a 0.3.0 foundation library despite current maintenance.
- Boost.Process: reject as the first process-containment dependency; keep only
  as a fallback if a later Boost policy exists for other reasons.
- Performance containers and google/benchmark: hold until representative
  measurements identify a specific bottleneck.
- Parser-generator alternatives and optional SAT/BDD/CUDD/H2/cpddl work: hold
  behind supported-surface and dedicated dependency-audit decisions.

## 2. Inputs and Evidence Basis

Component library-research inputs:

- `slice01-parser-library-research/closing-report.md`
- `slice02-grounder-library-research/closing-report.md`
- `slice03-engine-library-research/closing-report.md`

Accepted Arc01 audit inputs:

- `../arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `../arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
- `../arc01-vendored-source-audit/audit-results-pandapi-engine.md`
- CDC verification reports for parser, grounder, and engine under the
  corresponding `slice0*-*-audit/cdc-verification.md` paths.

C++ substrate:

- `cpp-guidelines` skill, especially `11-anti-patterns.md`,
  `05-resource-management.md`, `06-error-handling.md`,
  `09-performance.md`, `12-project-structure-and-tooling.md`, and
  `13-standard-library.md`.
- Applicable rule pressure: `SL.1`, `SL.2`, `SL.3`, `SL.con.2`,
  `SL.con.3`, `SL.io.2`, `R.1`, `R.3`, `R.5`, `R.11`, `R.20`, `R.21`,
  `E.1`, `E.4`, `E.6`, `E.27`, `E.28`, `SF.7`, and `Per.6`.

Current metadata refresh:

- `curl -L https://api.github.com/repos/CLIUtils/CLI11/releases/latest`
- `curl -L https://api.github.com/repos/fmtlib/fmt/releases/latest`
- `curl -L https://api.github.com/repos/TartanLlama/expected/releases/latest`
- `curl -L https://api.github.com/repos/catchorg/Catch2/releases/latest`
- `curl -L https://api.github.com/repos/DaanDeMeyer/reproc/releases/latest`
- `curl -L https://api.github.com/repos/nlohmann/json/releases/latest`
- `curl -L https://api.github.com/repos/microsoft/GSL/releases/latest`
- `curl -L https://api.github.com/repos/abseil/abseil-cpp/releases/latest`
- `curl -L https://api.github.com/repos/google/benchmark/releases/latest`
- `curl -L https://api.github.com/repos/Tessil/robin-map/releases/latest`
- `curl -L https://api.github.com/repos/martinus/unordered_dense/releases/latest`
- `curl -L` license fetches for CLI11, fmt, reproc, GSL, nlohmann/json,
  Abseil, and google/benchmark.
- Boost.Process metadata refreshed from the official Boost 1.91.0 library page.

Local build/release constraints checked:

- Linux and macOS only for 0.3.0.
- Current wrappers build parser/grounder with makefile-era flows and engine with
  CMake through `scripts/build-*.sh`.
- Release assets remain `pandapi-<tag>-<platform>.tar.gz`, `SHA256SUMS`,
  `release-manifest.txt`, `THIRD-PARTY-LICENSES`, and notes.
- The wolong fetch spec remains frozen; dependency choices must not silently
  change release asset names, checksum shape, manifest shape, or install path.

## 3. Cross-Component Defect Map

| Defect class | Parser | Grounder | Engine | Library pressure |
|--------------|--------|----------|--------|------------------|
| CLI validation, help, version, namespacing | P-005, P-008 | G-005, G-010, G-011 | E-003, E-005, E-006, E-007 | CLI11 plus golden process tests |
| stdout/stderr, ANSI, diagnostics, progress | P-006, P-007, P-014 | G-007, G-008, G-009 | E-004, E-005, E-014 | fmt under shared process facade |
| deep exits and status/result propagation | P-001, P-004, P-009 | G-002, G-003, G-004, G-006, G-019 | E-001, E-002, E-003, E-008, E-014, E-015 | local status enum, tl::expected pilot |
| resource ownership and file/path handling | P-002, P-012, P-015 | G-006, G-016, G-018 | E-012, E-015 | C++17 RAII, `std::filesystem`, smart ownership |
| header/source hygiene | P-013 | G-017 | E-013 | standard library, local hashers, no broad dependency |
| test gaps and warning posture | P-010, P-011 | G-014, G-015 | E-010, E-011 | Catch2 plus process fixture runner |
| dependency/process boundary | none primary | G-008, G-013, G-019 | E-015 | adapter first; conditional reproc++ |
| optional capability dependencies | parser generator held | H2/cpddl boundary | SAT/BDD/CUDD/translation | Arc03 supported-surface decision first |
| performance/container questions | no measurement basis | G-017, G-019 | E-012, E-016 | hold containers; benchmark only after fixtures |

## 4. Standard-Library Baseline

Disposition: **Adopt for 0.3.0.**

Audit findings addressed: P-002, P-003, P-009, P-012, P-013, P-015; G-001,
G-002, G-006, G-008, G-016, G-017, G-018, G-019; E-001, E-008, E-012, E-013,
E-015, E-016.

Baseline:

- Use `std::filesystem::path` for input, output, include, temp, and translation
  paths.
- Use scoped `std::ifstream`/`std::ofstream` and RAII wrappers for non-standard
  handles instead of raw `FILE*` or raw owning stream pointers.
- Use `std::unique_ptr`, values, `std::vector`, and local owner objects before
  introducing external ownership libraries.
- Use `std::chrono` for timeout and resource-policy accounting.
- Use `std::string`, `std::string_view`, local parsing helpers, explicit
  `enum class` status values, and checked access where malformed input can cross
  trust boundaries.
- Use local hashers instead of injecting project specializations into
  namespace `std`.

Rationale: `SL.1` supports library use, but `SL.2` prefers the standard library
when it solves the defect class. Most resource, bounds, string, path, and
ownership findings are first-order C++17 cleanup. A third-party library should
not be used to postpone those corrections.

Build/packaging impact: no new runtime dependency. Arc04 should create shared
runtime headers/sources that express these choices once, then Arc05 migrates
each binary behind the shared facade.

Rollback/defer option: no rollback risk beyond individual refactors. If one
component has an ABI or generated-code constraint, isolate that exception behind
the smallest adapter and document it.

## 5. Adopt for 0.3.0

### fmt

Disposition: **adopt for 0.3.0 as an implementation dependency under a shared
facade.**

| Field | Recommendation |
|-------|----------------|
| Audit fit | P-006, P-007, P-014; G-001, G-007, G-009, G-010, G-011; E-004, E-005, E-006, E-014. |
| License | MIT. |
| Maintenance/Release | GitHub API reports latest release `12.2.0`, published 2026-06-16; repository metadata shows active pushes on 2026-08-09. |
| Source links | <https://github.com/fmtlib/fmt>, <https://github.com/fmtlib/fmt/releases/tag/12.2.0> |
| Integration | Prefer compiled/static or object-library integration through Arc04; header-only remains a fallback if the build transition is delayed. |
| Build/packaging | Runtime source must be pinned and included in `THIRD-PARTY-LICENSES`; static linking avoids new shipped dylibs. |
| Migration risk | Low-medium. It changes formatting calls, not behavior by itself. Risk appears if direct `fmt::print` calls bypass the process facade. |
| Arc ownership | Arc03 defines stream/color/event rules; Arc04 adds `process_io`/`diagnostics`; Arc05 migrates each binary. |

Recommendation: use fmt only through a chengdu diagnostics/process facade that
owns stdout, stderr, verbosity, color, flushing, and event formatting. Direct
planner-internal calls to stdout/stderr should remain disallowed.

### CLI11

Disposition: **adopt as the selected CLI parser for 0.3.0, with first use
implemented as a golden-test pilot.**

| Field | Recommendation |
|-------|----------------|
| Audit fit | P-005, P-006, P-008, P-014; G-005, G-010, G-011; E-003, E-005, E-006, E-007, E-014. |
| License | BSD-3-Clause style CLI11 license. |
| Maintenance/Release | GitHub API reports latest release `v2.7.2`, published 2026-08-02; release assets include source archives and `CLI11.hpp`. |
| Source links | <https://github.com/CLIUtils/CLI11>, <https://github.com/CLIUtils/CLI11/releases/tag/v2.7.2>, <https://cliutils.github.io/CLI11/book/chapters/installation.html> |
| Integration | Start with the single-header or header-only mode; graduate to a normal CMake target if Arc04 moves the shared substrate to CMake. |
| Build/packaging | Add BSD license text to `THIRD-PARTY-LICENSES`; no runtime dylib if header-only or statically compiled. |
| Migration risk | Medium-high. Replacing `gengetopt` can change help formatting, parse errors, defaults, aliases, and status behavior. |
| Arc ownership | Arc03 decides CLI contract and compatibility; Arc04 wraps common CLI policy; Arc05 migrates parser, grounder, and engine. |

Recommendation: CLI11 should replace `gengetopt` only after Arc03 defines the
accepted `pandapi-*` option contract and Arc04/Arc05 add golden tests for
`--help`, `--version`, missing args, malformed args, stdout/stderr separation,
and compatibility aliases.

### Catch2

Disposition: **adopt as the C++ unit/seam test framework, test-only.**

| Field | Recommendation |
|-------|----------------|
| Audit fit | P-001, P-003, P-005, P-010, P-011; G-001, G-002, G-003, G-005, G-014, G-015; E-001, E-003, E-004, E-007, E-008, E-010, E-011. |
| License | Boost Software License 1.0. |
| Maintenance/Release | GitHub API reports latest release `v3.15.3`, published 2026-07-26; release assets include amalgamated source/header files. |
| Source links | <https://github.com/catchorg/Catch2>, <https://github.com/catchorg/Catch2/releases/tag/v3.15.3> |
| Integration | Test-only compiled library or amalgamated test source. Use for library seams, not for binary process-contract tests. |
| Build/packaging | Should not ship in release tarballs. Requires test-target work in Arc04 and possibly CMake/test-runner consolidation. |
| Migration risk | Low for runtime behavior; medium for build-system churn because current primary components lack local C++ test targets. |
| Arc ownership | Arc04 adds shared-runtime tests; Arc05 adds component seam tests; Arc06 keeps release packaging free of test artifacts. |

Recommendation: pair Catch2 with a separate process fixture runner. Catch2
should prove local parsing/status/RAII seams; process fixtures should prove
exit code, stdout, stderr, files, timeout, signal, help, and version behavior.

## 6. Pilot for 0.3.0

### tl::expected

Disposition: **pilot in Arc04 behind the shared status/result facade.**

| Field | Recommendation |
|-------|----------------|
| Audit fit | P-001, P-003, P-004, P-009, P-015; G-002, G-003, G-004, G-006, G-019; E-001, E-002, E-003, E-008, E-014, E-015. |
| License | CC0-1.0. |
| Maintenance/Release | GitHub API reports latest release `v1.3.1`, published 2025-09-01; repository metadata describes C++11/14/17 `std::expected`-style support. |
| Source links | <https://github.com/TartanLlama/expected>, <https://github.com/TartanLlama/expected/releases/tag/v1.3.1> |
| Integration | Header-only/single-header style behind chengdu aliases such as `StatusResult<T>`; do not expose directly across every planner header. |
| Build/packaging | Very small source/license footprint if vendored or imported under a future dependency policy. |
| Migration risk | Medium. It can spread through public headers before Arc03 stabilizes the status taxonomy. C++23 `std::expected` is the eventual destination. |
| Arc ownership | Arc03 defines statuses and payloads; Arc04 pilots result plumbing; Arc05 adopts only at component boundaries that need it. |

Recommendation: first define `ProcessStatus`, component error payloads, and
conversion to exit/event output. Then pilot `tl::expected` behind local aliases.
If the pilot becomes noisy or leaks into algorithm internals, fall back to small
local result structs until the C++ standard floor changes.

### reproc++

Disposition: **conditional pilot only for supported child-process boundaries.**

| Field | Recommendation |
|-------|----------------|
| Audit fit | G-008, G-013, G-019; E-015, and possibly E-008 for timeout cleanup. |
| License | MIT. |
| Maintenance/Release | GitHub API reports latest release `v14.2.7`, published 2026-04-15; Conan Center reports `reproc/14.2.7` published 2026-04-28 with Linux, macOS, and macOS Apple Silicon packages. |
| Source links | <https://github.com/DaanDeMeyer/reproc>, <https://github.com/DaanDeMeyer/reproc/releases/tag/v14.2.7>, <https://conan.io/center/recipes/reproc> |
| Integration | C library plus C++ wrapper; CMake-oriented; should be isolated in one adapter target if piloted. |
| Build/packaging | Adds runtime source/license and possible static-library build work; do not ship extra dynamic libraries. |
| Migration risk | High if adopted before Arc03 classifies H2 and translation mode; low-medium if fenced behind one adapter. |
| Arc ownership | Arc03 decides supported surface; Arc04 may add a child-process adapter; Arc05 uses it only in binaries whose supported path needs it. |

Recommendation: prefer in-process adapters or a small local Linux/macOS wrapper
first. Pilot reproc++ only if a supported path needs robust subprocess capture,
termination, timeout, and cleanup beyond that wrapper.

## 7. Hold

### nlohmann/json

Disposition: **hold; pilot only if Arc03 selects JSON Lines or another JSON
machine-event contract.**

Metadata: MIT license; GitHub API reports latest release `v3.12.0`, published
2025-04-11, with release assets including `json.hpp`. Source:
<https://github.com/nlohmann/json> and
<https://github.com/nlohmann/json/releases/tag/v3.12.0>.

Audit fit: P-006, P-008; G-007, G-010, G-011, G-019; E-003, E-004, E-006,
E-014.

Reason to hold: the event format is an Arc03 product-contract decision. JSON
availability should not decide whether chengdu emits JSON Lines, tagged text,
or another machine-readable surface.

### Microsoft GSL

Disposition: **hold globally; allow only a narrow helper pilot.**

Metadata: MIT license text fetched from the repository; GitHub API reports
latest release `v4.2.2`, published 2026-05-20. Source:
<https://github.com/microsoft/GSL> and
<https://github.com/microsoft/GSL/releases/tag/v4.2.2>.

Audit fit: P-003, P-012; G-002, G-016, G-018; E-001, E-012, E-013.

Reason to hold: the standard library plus local checked helpers should solve
most boundary issues without introducing another vocabulary type family into
already messy public headers. Re-enter only for `gsl::narrow`, `gsl::span`,
`gsl::not_null`, or `gsl::finally` after a concrete Arc04 implementation point
proves local helpers are worse.

### Performance Containers

Disposition: **hold until representative profiling proves a bottleneck.**

Candidates refreshed:

- `tsl::robin_map`: MIT-family project, latest GitHub release `v1.4.1`,
  published 2025-11-02. Source: <https://github.com/Tessil/robin-map>.
- `ankerl::unordered_dense`: MIT project, latest GitHub release `v4.9.0`,
  published 2026-08-07. Source: <https://github.com/martinus/unordered_dense>.

Audit fit: G-017, G-019; E-012, E-013, E-016.

Reason to hold: no accepted audit contains representative timing evidence.
`Per.6` applies directly: do not make performance claims without measurements.
First fix illegal `std::hash` specializations, add fixture workloads, and then
measure parser/grounder/engine hot paths.

### google/benchmark

Disposition: **hold as a development-only pilot after process fixtures exist.**

Metadata: Apache-2.0 license; GitHub API reports latest release `v1.9.5`,
published 2026-01-21. Source: <https://github.com/google/benchmark> and
<https://github.com/google/benchmark/releases/tag/v1.9.5>.

Audit fit: G-019; E-008, E-012, E-016.

Reason to hold: chengdu first needs representative end-to-end fixtures and
process-contract tests. A microbenchmark framework is useful only after a
specific hot path is named.

### Parser-Generator Alternatives

Disposition: **hold/reject for 0.3.0 managed-process cleanup.**

Candidates:

- Existing Bison C++ skeleton/variant path: pilot later, because it keeps the
  current generator family but still changes generated parser shape.
- ANTLR4 C++ target: hold as later parser rewrite research.
- PEGTL and lexy: hold as later rewrite research.

Audit fit: P-002, P-004, P-009, P-012, P-015.

Reason to hold: parser-generator replacement is semantic parser work, not a
shared process-runtime library decision. 0.3.0 should first establish parser
status boundaries, test oracles, and process behavior.

### Optional SAT, BDD, CUDD, H2, and cpddl Decisions

Disposition: **hold behind supported-surface and dependency-audit slices.**

Specific decisions:

- SAT: do not add a threading, signal, or SAT support library for 0.3.0 from
  current evidence. Engine finding E-009 is a supported-surface and concurrency
  audit issue first.
- BDD/CUDD: do not enable, upgrade, or replace CUDD in the default engine build.
  CUDD metadata shows a BSD-3-Clause maintained source lineage, but E-016 still
  requires a dedicated optional-dependency license/build/security audit before
  support expansion.
- H2/cpddl: do not let reproc++ or any adapter library classify H2/cpddl as
  supported product surface. Grounder findings G-008 and G-013 require an
  explicit dependency boundary and possibly separate dependency audits.
- Translation mode: do not adopt a process library solely to preserve the
  engine translation path. Arc03 must first classify the path as supported,
  unsupported, hidden/experimental, or future work.
- Interactive mode: no library adoption. Arc03 must classify TTY-only behavior
  and disable or fence it for managed-process invocation.

## 8. Reject for 0.3.0

### Abseil

Disposition: **reject as a 0.3.0 foundation library; hold only as a later
architecture option.**

Metadata: Apache-2.0; GitHub API reports Abseil LTS `20260526.0`, published
2026-06-01. The official repository describes Abseil as a C++17 collection that
augments the standard library and includes status, strings, containers, time,
types, synchronization, and other components. The LTS release raises a GCC 10
floor. Source: <https://github.com/abseil/abseil-cpp> and
<https://github.com/abseil/abseil-cpp/releases/tag/20260526.0>.

Audit fit: G-004, G-017, G-019; E-002, E-003, E-008, E-012, E-013.

Reason to reject for 0.3.0: Abseil can solve several classes, but it does so by
importing a broad ecosystem. chengdu's current evidence supports a narrower
portfolio: standard library, fmt, CLI11, Catch2, a small expected pilot, and a
conditional process adapter.

### Boost.Process

Disposition: **reject as the first process-containment dependency for 0.3.0.**

Metadata: Boost.Process is in Boost 1.91.0, C++11, Boost Software License 1.0,
and the official library page lists broad dependencies including Asio,
Filesystem, Optional, and System. Source:
<https://www.boost.org/library/latest/process/>.

Audit fit: G-008, G-013, G-019; E-015.

Reason to reject for 0.3.0: the license is excellent, but the dependency
footprint is too large for the current H2/translation containment question.
Use a local wrapper first; if that is insufficient, pilot reproc++ before
importing Boost.

### Boost.Outcome and spdlog

Disposition: **reject for the 0.3.0 default portfolio; hold only by exception.**

Reason: Boost.Outcome overlaps the `tl::expected` pilot with more conceptual
surface. spdlog overlaps the proposed fmt-backed diagnostics facade and can
make stdout/stderr discipline worse if logging sinks are not tightly wrapped.
Neither solves enough cross-component pain to justify another dependency now.

## 9. Architecture Impact

### Arc03 Managed-Process Design

Arc03 should design the contract before implementation dependencies decide it.
The combined portfolio affects Arc03 as follows:

- Define status taxonomy and payloads first; `tl::expected` is only a carrier.
- Define stdout, stderr, event, color, TTY, buffering, and flushing policy;
  fmt is only formatting machinery.
- Define CLI command names, compatibility aliases, help/version/provenance, and
  parse-error behavior before CLI11 migration.
- Classify translation, interactive, SAT, BDD, CUDD, H2, and cpddl before
  reproc++, JSON, CUDD, or dependency-internal work becomes a commitment.

### Arc04 Shared Runtime Substrate

Arc04 should introduce a small shared substrate, not a broad framework:

- `process_status` and exit-code/event mapping.
- `process_io` or `diagnostics` owning fmt, stdout/stderr routing, ANSI/TTY,
  flushing, and verbosity.
- `cli_common` wrapping CLI11 after golden contract tests exist.
- `result` aliases around `tl::expected` only after Arc03 names the taxonomy.
- RAII filesystem/temp/stream helpers using the standard library.
- Optional `child_process` adapter only if a supported path needs it.

This is the large architectural recommendation: chengdu should add a shared
runtime substrate before per-binary migration. The alternative is three
independent rewrites that repeat the current process-policy duplication.

### Arc05 Per-Binary Adoption

Arc05 should migrate each binary incrementally:

1. Add process fixture tests for current behavior and accepted behavior.
2. Put parser, grounder, and engine behind the shared status and I/O facade.
3. Introduce CLI11 one binary at a time, preserving accepted compatibility
   aliases or documenting operator-approved breaks.
4. Keep algorithmic code separate from process contract code.
5. Leave optional paths fenced until Arc03 has accepted their product surface.

### NOTICE, License, and Release Packaging

Any adopted dependency must update `THIRD-PARTY-LICENSES`, release manifests,
and source provenance before release. Static/object integration is preferred so
the tarball shape remains `pandaPIparser`, `pandaPIgrounder`,
`pandaPIengine`, `provenance.txt`, and `fixtures/`.

Runtime dependencies selected for 0.3.0:

- fmt: MIT notice required.
- CLI11: BSD-3-Clause style notice required if adopted.

Test-only dependencies:

- Catch2: BSL-1.0 notice in source/dev docs; should not enter release tarballs.

Pilot dependencies:

- tl::expected: CC0 provenance if vendored.
- reproc++: MIT notice and static-link/provenance plan if adopted after pilot.

Held/rejected dependencies should not affect release packaging.

### Rollback and Deferral

- fmt rollback: local to diagnostics facade if direct calls are forbidden.
- CLI11 rollback: preserve golden tests and keep `gengetopt` wrapper until the
  new CLI behavior matches accepted outputs.
- Catch2 rollback: test-only; can be replaced with another test framework
  without release impact if no production headers depend on it.
- tl::expected rollback: keep aliases local; replace alias target with local
  result struct if needed.
- reproc++ rollback: keep all use behind one child-process adapter; replace
  with local POSIX implementation if the dependency cost exceeds value.
- JSON, GSL, Abseil, Boost, performance containers, CUDD: defer by not adding
  them until their re-entry criteria are met.

## 10. Final Recommendation Matrix

| Candidate | Final disposition | Reason |
|-----------|-------------------|--------|
| Standard-library modernization | Adopt for 0.3.0 | Directly addresses ownership, path, string, chrono, bounds, and hash hygiene with no dependency cost. |
| CLI11 | Adopt for 0.3.0 through pilot | Best cross-component fit for CLI/help/version/namespacing defects; requires golden tests before replacing `gengetopt`. |
| fmt | Adopt for 0.3.0 | Best focused primitive for diagnostics and structured messages under a shared facade. |
| tl::expected | Pilot for 0.3.0 | All three components need typed propagation, but taxonomy must stabilize first. |
| Catch2 | Adopt test-only | Best C++ seam-test fit; pair with process fixture runner. |
| nlohmann/json | Hold | Event format is Arc03's decision, not a library-led choice. |
| reproc++ | Conditional pilot | Useful only for supported H2/translation child-process boundaries. |
| Boost.Process | Reject for 0.3.0 | Too much Boost surface for the current containment problem. |
| Microsoft GSL | Hold/narrow pilot | Useful vocabulary, but standard library plus local helpers should come first. |
| Abseil | Reject for 0.3.0 | Broad foundation library and compiler/build implications exceed current evidence. |
| Performance containers | Hold | No representative measurements. |
| google/benchmark | Hold/dev pilot later | Useful only after fixture baselines and named hot paths exist. |
| Parser-generator alternatives | Hold/reject for 0.3.0 | Parser rewrite is semantic work, not process-runtime cleanup. |
| SAT/BDD/CUDD | Hold/reject default | Optional capability and dependency-audit decisions, not default release dependencies. |
| H2/cpddl | Hold dependency-internal work | Requires supported-surface and boundary classification first. |

## 11. Sources

- CLI11: <https://github.com/CLIUtils/CLI11>
- CLI11 release `v2.7.2`: <https://github.com/CLIUtils/CLI11/releases/tag/v2.7.2>
- CLI11 installation docs: <https://cliutils.github.io/CLI11/book/chapters/installation.html>
- fmt: <https://github.com/fmtlib/fmt>
- fmt release `12.2.0`: <https://github.com/fmtlib/fmt/releases/tag/12.2.0>
- tl::expected: <https://github.com/TartanLlama/expected>
- tl::expected release `v1.3.1`: <https://github.com/TartanLlama/expected/releases/tag/v1.3.1>
- Catch2: <https://github.com/catchorg/Catch2>
- Catch2 release `v3.15.3`: <https://github.com/catchorg/Catch2/releases/tag/v3.15.3>
- reproc: <https://github.com/DaanDeMeyer/reproc>
- reproc release `v14.2.7`: <https://github.com/DaanDeMeyer/reproc/releases/tag/v14.2.7>
- reproc Conan Center recipe: <https://conan.io/center/recipes/reproc>
- nlohmann/json: <https://github.com/nlohmann/json>
- nlohmann/json release `v3.12.0`: <https://github.com/nlohmann/json/releases/tag/v3.12.0>
- Microsoft GSL: <https://github.com/microsoft/GSL>
- Microsoft GSL release `v4.2.2`: <https://github.com/microsoft/GSL/releases/tag/v4.2.2>
- Abseil: <https://github.com/abseil/abseil-cpp>
- Abseil release `20260526.0`: <https://github.com/abseil/abseil-cpp/releases/tag/20260526.0>
- Boost.Process: <https://www.boost.org/library/latest/process/>
- google/benchmark: <https://github.com/google/benchmark>
- google/benchmark release `v1.9.5`: <https://github.com/google/benchmark/releases/tag/v1.9.5>
- robin-map: <https://github.com/Tessil/robin-map>
- unordered_dense: <https://github.com/martinus/unordered_dense>
- CUDD maintained source lineage: <https://github.com/cuddorg/cudd>
- CUDD fork metadata checked: <https://github.com/ivmai/cudd>
