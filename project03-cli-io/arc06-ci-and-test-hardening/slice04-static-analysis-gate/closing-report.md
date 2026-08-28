# Arc06 Slice04 CC Closing Report

## Capability Verdict

Proposed done, with adoption-seam static analysis explicitly deferred. Arc06
now has a Make-backed C++ static-analysis target for owned runtime code:
`make static-analysis-cpp`. The target configures a Debug runtime CMake build,
uses its `compile_commands.json`, and runs `clang-tidy` with
`clang-analyzer-*` over runtime source/test translation units present in that
database.

## Implementation Decisions

- Added `make static-analysis-cpp` and listed it in `make help`.
- Integrated `static-analysis-cpp` into the existing `make static-analysis`
  aggregate after shell syntax and shellcheck.
- Resolved `clang-tidy` through `CLANG_TIDY`, PATH, `xcrun`, and common
  Homebrew LLVM locations; this host used
  `/opt/homebrew/opt/llvm/bin/clang-tidy` 20.1.6.
- Used a dedicated Debug compile database under
  `build/static-analysis/runtime/<platform>/build/` so assert-based runtime
  smoke tests remain meaningful to the analyzer.
- Kept the first blocking C++ check family narrow:
  `CLANG_TIDY_CHECKS=-*,clang-analyzer-*`.
- Documented included runtime source/tests/reached headers, excluded
  inherited/generated/third-party/build/dist paths, no-suppression policy,
  missing-tool behavior, standalone `scan-build` deferral, and adoption-seam
  re-entry conditions.

## Verification

- `make help` passed and lists `make static-analysis-cpp`.
- `make static-analysis-cpp` passed over 14 runtime translation units.
- `make build-runtime` passed and emitted the normal runtime compile database.
- Ledger greps for clang-tidy, compile commands, path policy, suppressions,
  analyzer disposition, and adoption-seam routing passed.
- `make safety-checks` passed.
- `make actionlint` passed.
- `make static-analysis` passed.
- `make format-check` passed.
- `make test` passed.
- `make provenance-check` passed.
- `git diff --check` and `git diff --cached --check` passed after staging.

## Ledger Walk

- F-1 proposed-done: Make exposes and lists `make static-analysis-cpp`.
- F-2 proposed-done: runtime compile databases are generated, and the C++
  target uses the dedicated Debug runtime compile database for analysis.
- F-3 proposed-done: `clang-tidy` runs locally; the target also has explicit
  missing-tool SKIP behavior with a re-entry condition.
- F-4 proposed-done: Clang Static Analyzer checks are active through
  `clang-tidy`'s `clang-analyzer-*` family; standalone `scan-build` is
  deferred with a reason and re-entry condition.
- F-5 proposed-done: included, excluded, and deferred path ownership is
  documented for runtime, inherited planner, generated, third-party, `build`,
  and `dist` surfaces.
- F-6 proposed-done: no suppressions or `NOLINT` annotations were added, and
  the future suppression policy is documented.
- F-7 proposed-deferred: adoption-seam static analysis for
  `pandapi_parser_native.cpp`, `pandapi_grounder_native.cpp`, and
  `pandapi_engine_native.cpp` waits for reliable owned-file compile commands
  from canonical `pandapi-*` builds.
- F-8 proposed-done: no workflow changed, and Make-entrypoint safety plus
  actionlint pass.
- F-9 proposed-done: existing quality and behavior gates pass after the new
  C++ static-analysis wiring.
- F-10 proposed-done: the slice stays inside Arc06 static-analysis scope and
  does not touch parser, grounder, engine, release, Arc07, or Arc08 surfaces.

## Silent-Drop Check

No compiler-warning burndown was performed. No sanitizer, LSan, TSan, release
package, publication, README migration prose, Arc07 tutorial, or Arc08
release-prep work was added. No parser, grounder, or engine product behavior
changed. No inherited planner, generated, or third-party source was bulk
formatted or broadly suppressed.

## Bubble-up to the arc

The static-analysis signal is strong enough to inform Slice05 warning
inventory and burndown for `pandaPI/runtime`: owned runtime source/tests now
have a reproducible Make target, Debug compile database, and analyzer pass.
It is not a substitute for Slice05's compiler-warning inventory across parser,
grounder, engine, runtime, generated code, and nested third-party code.

Remaining gaps:

- Adoption-seam static analysis is deferred until canonical `pandapi-*` builds
  expose reliable compile commands for `pandapi_parser_native.cpp`,
  `pandapi_grounder_native.cpp`, and `pandapi_engine_native.cpp` without
  mixing inherited/generated and third-party findings into the owned signal.
- Standalone `scan-build` is deferred unless Slice05 or Slice06 needs a
  distinct path-sensitive report beyond the `clang-tidy` analyzer pass.
- Optional Catch2 static analysis is dependent on a local Catch2 CMake package;
  when Catch2 is absent, its optional test translation unit is not in the
  compile database and is not part of the gate.
