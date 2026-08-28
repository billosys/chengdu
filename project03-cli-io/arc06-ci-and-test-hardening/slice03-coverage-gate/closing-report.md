# Arc06 Slice03 CC Closing Report

## Capability Verdict

Proposed done, with adoption-seam process-binary coverage explicitly deferred.
Arc06 now has a Make-backed Clang source-based coverage target for
`pandaPI/runtime` that builds with coverage instrumentation, runs runtime
CTest, merges profiles, and emits ignored local reports under
`build/coverage/runtime/<platform>/report/`.

The coverage run also exposed a runtime helper defect hidden by the existing
Release CTest build: `command_identity_for` computed inherited compatibility
after moving the invoked command string. The fix preserves the intended
compatibility metadata and does not change parser, grounder, or engine product
behavior.

## Implementation Decisions

- Added `make coverage` and listed it in `make help`.
- Added `mk/coverage.mk` to resolve `clang++`, `llvm-profdata`, and `llvm-cov`
  through PATH or `xcrun`, fail explicitly when required tools are unavailable,
  and use `-fprofile-instr-generate` plus `-fcoverage-mapping`.
- Generated runtime reports at:
  `build/coverage/runtime/<platform>/report/runtime-coverage-summary.txt` and
  `build/coverage/runtime/<platform>/report/runtime-coverage.txt`.
- Documented the owned coverage scope and exclusions for generated output,
  inherited planner source, nested third-party source, `build`, `dist`,
  release output, upstream checkout material, and historical workbench output.
- Deferred `pandapi_parser_native.cpp`, `pandapi_grounder_native.cpp`, and
  `pandapi_engine_native.cpp` coverage until managed-fixture profile
  collection can isolate owned adoption files from copied inherited build
  trees, generated sources, and third-party code.
- Updated the Arc06 fixture inventory and `pandaPI/runtime/README.md` with the
  report path, exclusions, and adoption-seam re-entry condition.

## Verification

- `make coverage` passed; runtime CTest ran 5/5 tests and emitted both reports.
- `make help | rg -n "coverage"` passed.
- Coverage implementation grep for Clang/LLVM coverage terms passed.
- Runtime path and retired-path grep passed.
- Exclusion-policy grep passed.
- Adoption-seam deferral grep passed.
- Generated report hygiene check passed: `git status --short -- build dist`
  stayed empty.
- `make safety-checks` passed.
- `make actionlint` passed.
- `make static-analysis` passed.
- `make format-check` passed.
- `make test` passed.
- `make provenance-check` passed.
- `git diff --check` and `git diff --cached --check` passed after staging.

## Ledger Walk

- F-1 proposed-done: Make exposes and lists `make coverage`.
- F-2 proposed-done: coverage uses Clang source-based coverage and explicit
  LLVM tool checks.
- F-3 proposed-done: `make coverage` builds instrumented runtime CMake output,
  runs CTest, merges profiles, and emits deterministic local report paths.
- F-4 proposed-done: the detailed report includes `pandaPI/runtime` source and
  test paths and active Slice03 docs avoid the retired path spelling.
- F-5 proposed-done: generated, inherited, third-party, `build`, and `dist`
  paths are excluded from the owned-code coverage claim by written policy.
- F-6 proposed-deferred: native adoption-seam coverage is deferred with a
  technical reason and re-entry condition.
- F-7 proposed-done: generated coverage reports remain under ignored `build/`
  output and are not committed.
- F-8 proposed-done: no workflow was changed, and workflow Make-entrypoint
  safety plus actionlint still pass.
- F-9 proposed-done: quality and behavior gates pass after coverage wiring.
- F-10 proposed-done: the slice stays in Arc06 coverage scope and avoids
  release packaging/publication, README migration prose, Arc07, and Arc08
  surfaces.

## Silent-Drop Check

No parser, grounder, or engine product behavior changed. No warning burndown,
`clang-tidy`, Clang Static Analyzer, sanitizer expansion, LSan, TSan, release
package, checksum, manifest, wolong, Arc07 tutorial, or Arc08 publication
surface was added. No inherited planner or third-party coverage percentage is
presented as owned runtime/process-policy evidence.

## Bubble-up to the arc

The runtime coverage signal is strong enough for Slice04 static-analysis
planning: `pandaPI/runtime` now has a dedicated coverage CMake build tree,
`compile_commands.json`, and source/test evidence showing where runtime helper
tests are thin. It is also useful for Slice06 sanitizer workload selection
because runtime CTest now runs under both coverage and the existing ASan/UBSan
gate, but process-binary sanitizer work must still use managed fixtures rather
than inheriting the runtime-only signal.

Remaining coverage gaps before Arc06 close:

- Adoption-seam coverage needs isolated profile collection for canonical
  `pandapi-*` managed fixtures over owned native files.
- The coverage target is local evidence only in this slice; any CI wiring
  should call `make coverage` directly and keep report artifacts generated.
- Optional Catch2 fixture coverage depends on a locally installed Catch2 CMake
  package; the default report remains valid when Catch2 is absent.
