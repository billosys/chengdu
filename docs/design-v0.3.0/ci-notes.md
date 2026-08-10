# Chengdu 0.3.0 CI and C++ Tooling Notes

Date: 2026-08-09
Status: design note

This note records the current testing, coverage, formatting, static-analysis,
and sanitizer recommendations for the 0.3.0 managed-process work. It is not an
implementation plan by itself. Arc05 and Arc06 still own executable
conformance, CI hardening, release packaging proof, and wolong migration
evidence.

## 1. Summary Recommendation

Chengdu should use a layered C++ quality stack:

1. CMake/CTest as the test runner for C++ test executables.
2. Catch2 as the optional test-only C++ seam/unit framework.
3. Chengdu black-box process fixtures as the integration/contract test
   framework for executable behavior.
4. Clang source-based coverage through `llvm-cov` and `llvm-profdata` as the
   primary coverage path, with `gcov`/`lcov` only as a GCC fallback.
5. `clang-format` for formatting, initially scoped to chengdu-owned source and
   touched lines rather than bulk-reformatting vendored upstream code.
6. `clang-tidy` plus Clang Static Analyzer checks for static bug finding and
   C++ Core Guidelines enforcement.
7. Clang sanitizers in CI: ASan/UBSan first, LSan with ASan where supported,
   and TSan on concurrency/process-observation paths once fixture coverage is
   representative.

The major distinction is important: Catch2 proves in-process C++ seams, while
process fixtures prove product behavior. Unit tests must not become a proxy for
managed-process conformance.

## 2. Current Project Inputs

The 0.3.0 project plan defines the release goal as a managed-process contract
for `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`: clear command
contracts, namespaced binaries, disciplined stdout/stderr, documented
exit/status semantics, bounded failure behavior, and maintainable C/C++ source.

The current plan state is:

- Arc01 audit work is closed and CDC-verified.
- Arc02 C++ library research is closed and CDC-verified.
- Arc03 managed-process contract is closed and CDC-verified.
- Arc04 shared-runtime substrate is closed and CDC-verified.
- Arc05 and Arc06 remain roadmap only.

Arc02 already selected the relevant dependency posture:

- Standard-library modernization is the baseline.
- `fmt`, `CLI11`, and Catch2 are accepted only through their owning gates.
- Catch2 is test-only and must be paired with process fixtures.
- `tl::expected` and reproc++ are pilots behind explicit facades/decisions.
- nlohmann/json, GSL, performance containers, google/benchmark, parser
  generators, optional SAT/BDD/CUDD/H2/cpddl work, and optional dependency
  audits remain held until their re-entry criteria are met.
- Abseil and Boost.Process are rejected as 0.3.0 foundation dependencies.

The current `pandapi-runtime/` state lines up with that posture. It provides a
C++17 standard-library-first runtime substrate, CMake/CTest smoke tests,
fixture and normalization helpers, local Linux/macOS process observation, and
an optional Catch2 seam gate that is registered only when Catch2 is already
available locally. The runtime does not fetch Catch2 and does not expose it
from public headers.

## 3. Unit and Seam Testing

### 3.1 Framework Choice

Use Catch2 for C++ seam/unit tests in Chengdu.

This is both a local project decision and a good fit for the codebase:

- Arc02 explicitly adopted Catch2 as the C++ unit/seam test framework,
  test-only.
- Catch2 integrates naturally with CMake through exported targets such as
  `Catch2::Catch2` and `Catch2::Catch2WithMain`.
- Catch2 also provides CMake helpers for registering `TEST_CASE`s with CTest.
- The runtime already has an optional `find_package(Catch2 QUIET CONFIG)` gate.
- The repo does not need GoogleTest's stronger mocking ecosystem for the
  current runtime seam tests.

There is no evidence that Bjarne Stroustrup or the C++ Core Guidelines
canonize one unit-test framework. The Guidelines emphasize modern C++,
resource safety, static checking, gradual adoption, and mechanical enforcement
through tools. For testing frameworks, the wider C++ community commonly uses
GoogleTest, Catch2, doctest, and Boost.Test. GoogleTest is the de facto large
organization/default choice when a project needs a mature xUnit framework plus
mocking. Catch2 is a strong fit when the project wants low ceremony, readable
assertions, CMake integration, and local seam coverage.

For Chengdu, the better local answer is Catch2.

### 3.2 Scope of Catch2 Tests

Catch2 should cover:

- status-name, status-class, and exit-code mapping;
- `StatusResult<T>` value/status behavior;
- `PANDAPI_STATUS` serialization and parsing;
- status-stream legality and final flushing;
- color and TTY policy;
- CLI policy helpers and option-conflict classification;
- command identity and provenance formatting;
- path, line-ending, ANSI, duration, timestamp, errno, and signal
  normalization helpers;
- fixture construction and comparison helpers;
- small process-observation helper seams where local in-process assertions are
  useful.

Catch2 should not cover:

- final executable behavior as a substitute for process fixtures;
- release asset shape;
- wolong fetch/install compatibility;
- inherited `pandaPI*` versus canonical `pandapi-*` command behavior;
- stdout/stderr/artifact ownership at the binary boundary, except through
  helper-level contracts.

Those are black-box fixture responsibilities.

## 4. Integration and Contract Testing

There is no single best generic "C++ integration testing framework" for this
project. The product boundary is process behavior, so Chengdu's integration
test framework should be the local black-box process fixture substrate, run by
CTest or a CI script.

Process fixtures should observe the whole command:

- command name and argv shape;
- environment;
- stdin mode;
- stdout role and comparison mode;
- stderr role and comparison mode;
- expected artifacts and partial-output policy;
- exit status and status name;
- wait status and signal behavior;
- final `PANDAPI_STATUS` fields;
- timeout/resource policy;
- TTY and color mode;
- path, duration, timestamp, errno, signal, line-ending, and ANSI
  normalization.

Stable machine contract fields can be golden-tested:

- command and argv shape;
- exit code and status name;
- `PANDAPI_STATUS` tag spelling, required fields, status vocabulary, component,
  surface, surface disposition, numeric exit code, and class;
- stdout/stderr ownership roles;
- help/usage skeleton and option names once finalized;
- version/provenance field names;
- absence of default inherited-name deprecation warning;
- absence of ANSI leakage in machine streams, files, pipes, no-TTY output, and
  supervised output.

Human diagnostic prose, normalized paths, durations, compiler/platform fields,
timestamps, errno text, signal wording, and planner-specific plan text should
use semantic predicates rather than byte-for-byte goldens.

## 5. Coverage Tooling

Use Clang source-based coverage as the primary coverage path:

- compile with `-fprofile-instr-generate -fcoverage-mapping`;
- run the instrumented tests with `LLVM_PROFILE_FILE` set;
- merge raw profiles with `llvm-profdata merge`;
- report with `llvm-cov report`, `llvm-cov show`, and optionally
  `llvm-cov export`.

This should cover both:

- C++ seam tests for `pandapi-runtime` and any later component seams;
- process fixtures that execute instrumented `pandapi-*` binaries.

Coverage should be reported with explicit scope boundaries. Recommended first
scope:

- include chengdu-owned runtime code under `pandapi-runtime/include`,
  `pandapi-runtime/src`, and `pandapi-runtime/tests`;
- include later Arc05 chengdu-owned adapter/facade code;
- report vendored planner files separately if they are instrumented;
- exclude generated code and vendored third-party internals from quality
  thresholds unless a later slice explicitly owns their cleanup.

The goal is not to create a vanity percentage over inherited code. The useful
metric is whether Chengdu-owned process-policy code and touched adoption seams
have meaningful line, branch, and negative-path coverage.

`gcov`/`lcov` remain acceptable fallback tooling for GCC-centered Linux jobs,
but `llvm-cov` should be the preferred path because Chengdu's sanitizer/static
analysis recommendations are also Clang-oriented.

## 6. Formatting

Use `clang-format`.

Current local check: there is no top-level `.clang-format` or `_clang-format`
file yet. Adding one should be its own small change because it creates a new
style contract.

Recommended policy:

- Add a top-level `.clang-format` before large Arc05 C++ edits.
- Apply formatting first to chengdu-owned source and tests.
- Avoid bulk-reformatting vendored upstream planner source under `pandaPI/`.
- For vendored planner files touched during behavior migration, format only the
  changed region where practical, or explicitly record when a file-wide format
  is intentional.
- Use `clang-format --dry-run --Werror` as a CI gate for owned source once the
  config lands.
- Prefer `git clang-format` or equivalent diff-scoped formatting for patch
  review.

This avoids hiding semantic process-contract changes inside a formatting-only
diff.

## 7. Static Analysis

Use `clang-tidy` as the main static-analysis driver.

Recommended check families for owned C++:

- `clang-analyzer-*` for Clang Static Analyzer checks;
- `bugprone-*` for common C++ mistakes;
- `cppcoreguidelines-*` for Core Guidelines pressure;
- `modernize-*` where compatible with the C++17 floor;
- `performance-*` for low-risk performance diagnostics;
- `readability-*` selectively, only after formatting/naming policy exists;
- `concurrency-*` where shared state or threading appears.

Adoption requirements:

- Generate or preserve a reliable `compile_commands.json` for CMake targets.
- Keep Makefile-era parser/grounder coverage separate until their build shape
  can feed Clang tooling cleanly.
- Start with chengdu-owned runtime and adapter code.
- Treat vendored upstream warning volume as triage input, not as an immediate
  blanket failure gate.
- Use suppressions only with narrow, written rationale.

The C++ Core Guidelines supporting-tool appendix names `clang-tidy` and
Microsoft CppCoreCheck as guideline-supporting tools. On Chengdu's Linux/macOS
matrix, `clang-tidy` is the practical portable choice.

## 8. Sanitizers and Dynamic Bug Finding

Sanitizers should be CI gates for instrumented test runs, not production or
release binaries.

### 8.1 ASan and UBSan

Run AddressSanitizer and UndefinedBehaviorSanitizer first.

ASan catches memory errors such as:

- heap, stack, and global out-of-bounds accesses;
- use-after-free;
- use-after-return and use-after-scope, depending on flags/options;
- double free and invalid free;
- leaks through LeakSanitizer where supported.

UBSan catches many undefined-behavior classes such as invalid shifts, signed
integer overflow where enabled, invalid enum values, nullability issues, and
other runtime UB checks depending on flags.

Recommended CI shape:

```text
CC=clang CXX=clang++ \
cmake -S pandapi-runtime -B build/runtime-asan \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_CXX_FLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build/runtime-asan
ctest --test-dir build/runtime-asan --output-on-failure
```

Once Arc05 binaries are instrumentable through the same build substrate, run
black-box process fixtures against ASan/UBSan binaries as well.

### 8.2 LSan

LeakSanitizer should usually ride with ASan:

- Linux: leak detection is typically available by default with ASan.
- macOS: set `ASAN_OPTIONS=detect_leaks=1` if the local toolchain supports it.
- Standalone `-fsanitize=leak` is useful when ASan overhead is too broad.

Leak findings in test-only dependencies or inherited third-party code should be
triaged by ownership. Leaks in chengdu-owned runtime/adapters should block.

### 8.3 TSan

Use ThreadSanitizer for data-race detection.

TSan is the right tool for races, but it is expensive and workload-dependent.
The Core Guidelines note that dynamic race tools are best applied to
integration tests, canary pushes, or multithreaded unit tests, and that they
only find races exercised in a given run.

Recommended Chengdu policy:

- Add a TSan job after process fixtures cover meaningful subprocess,
  timeout/signal, and stream-draining paths.
- Keep it separate from ASan/UBSan; TSan is normally not combined with ASan.
- Run it on Linux and macOS where supported by the CI toolchain.
- Start with `pandapi-runtime` process-observation tests and any future
  child-process adapter.
- Treat data-race reports in chengdu-owned code as blockers.

### 8.4 MSan and Valgrind

MemorySanitizer detects uninitialized memory reads, but it is higher friction
because it works best when all code, including dependencies and the standard
library path, is instrumented. Hold it unless Arc05/Arc06 finds a specific
uninitialized-memory defect class that ASan/UBSan and static analysis do not
cover well enough.

Valgrind is useful on Linux as a supplemental leak/memory checker when
compiler sanitizers cannot be used, but it should not be the primary
Linux/macOS recommendation for Chengdu. Prefer compiler sanitizers first.

## 9. Proposed CI Gate Shape

### 9.1 Always-on pull/push gates

Recommended once the scripts/config exist:

- actionlint for GitHub workflow syntax;
- README/verbatim docs gates already used by the project;
- `./scripts/build-runtime.sh`;
- `./scripts/build-parser.sh`;
- `./scripts/build-grounder.sh`;
- `./scripts/build-engine.sh`;
- `./scripts/smoke-test.sh`;
- `./scripts/smoke-test.sh --negative`;
- CTest for `pandapi-runtime`;
- `clang-format --dry-run --Werror` for owned C/C++ source after
  `.clang-format` lands.

### 9.2 Standard C++ quality gates

Recommended after Arc05 planning decides exact script ownership:

- `clang-tidy` for `pandapi-runtime` and chengdu-owned adapter/facade code;
- ASan/UBSan build and CTest run;
- ASan/UBSan process fixtures once binaries are migrated;
- `llvm-cov` coverage report for runtime and adapter tests.

These can start as non-blocking or scheduled while legacy warning/noise is
triaged, then graduate to blocking for owned code.

### 9.3 Scheduled or heavier gates

Recommended for nightly or pre-release gates:

- TSan process-observation and child-process tests;
- coverage aggregation across seam tests and black-box fixtures;
- release package dry-run;
- `THIRD-PARTY-LICENSES`, NOTICE, manifest, checksum, and provenance checks;
- wolong fetch/install/migration verification.

## 10. Decision Table

| Area | Recommended tool | Status for Chengdu 0.3.0 | Notes |
|------|------------------|---------------------------|-------|
| C++ unit/seam tests | Catch2 | accepted test-only | Optional local gate today; no fetch/vendor by default. |
| Test runner | CTest | active | Runtime CMake already registers smoke/seam executables. |
| Integration/contract tests | chengdu process fixtures | accepted substrate | Owns executable behavior proof. |
| Coverage | `llvm-cov` + `llvm-profdata` | recommended | Primary path for Clang builds. |
| GCC coverage fallback | `gcov`/`lcov` | fallback | Useful if Linux compiler matrix needs GCC reports. |
| Formatting | `clang-format` | recommended | Add config before enforcing. |
| Static analysis | `clang-tidy` | recommended | Start with owned runtime/adapter code. |
| Static bug finding | Clang Static Analyzer | recommended through `clang-tidy`/`scan-build` | Good for path-sensitive bug checks. |
| Race detection | ThreadSanitizer | recommended heavier gate | Best once relevant process/concurrency fixtures exist. |
| Memory errors | AddressSanitizer | recommended standard gate | Pair with UBSan. |
| Leaks | LeakSanitizer | recommended with ASan where supported | Ownership triage matters. |
| Undefined behavior | UndefinedBehaviorSanitizer | recommended standard gate | Useful for inherited C/C++ modernization. |
| Uninitialized reads | MemorySanitizer | hold | High friction; use only with a specific need. |
| Linux memory fallback | Valgrind | supplemental | Not the primary Linux/macOS strategy. |

## 11. External References Checked

- C++ Core Guidelines:
  <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>
- Catch2 CMake integration:
  <https://catch2-temp.readthedocs.io/en/latest/cmake-integration.html>
- GoogleTest user guide:
  <https://google.github.io/googletest/>
- CMake `add_test`:
  <https://cmake.org/cmake/help/latest/command/add_test.html>
- Clang source-based code coverage:
  <https://clang.llvm.org/docs/SourceBasedCodeCoverage.html>
- `clang-format`:
  <https://clang.llvm.org/docs/ClangFormat.html>
- `clang-tidy`:
  <https://clang.llvm.org/extra/clang-tidy/index.html>
- Clang Static Analyzer:
  <https://clang.llvm.org/docs/ClangStaticAnalyzer.html>
- ThreadSanitizer:
  <https://clang.llvm.org/docs/ThreadSanitizer.html>
- AddressSanitizer:
  <https://clang.llvm.org/docs/AddressSanitizer.html>
- LeakSanitizer:
  <https://clang.llvm.org/docs/LeakSanitizer.html>
- MemorySanitizer:
  <https://clang.llvm.org/docs/MemorySanitizer.html>

## 12. Follow-on Work

Future slices can turn this note into implementation by adding:

- `.clang-format` and an owned-source format check;
- `.clang-tidy` plus a generated `compile_commands.json` path;
- `scripts/coverage-runtime.sh` or equivalent `llvm-cov` gate;
- `scripts/sanitize-runtime.sh` for ASan/UBSan;
- a TSan job once the process fixture workload justifies it;
- Arc05 process fixtures for parser, grounder, and engine conformance;
- Arc06 release packaging checks proving test-only dependencies are excluded.
