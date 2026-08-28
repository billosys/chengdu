# Arc06 Slice03: coverage-gate

## Goal

Add scoped Clang source-based coverage reporting through Make for the
chengdu-owned runtime and any stable chengdu-owned process-policy/adoption
seams that can be instrumented without turning inherited planner or third-party
code into vanity percentages.

This slice starts from the Slice02 representative workload: runtime CTest,
managed parser/grounder/engine fixtures, managed pipeline fixtures, and smoke
tests. The first required result is useful owned-code coverage evidence, not a
large percentage over inherited algorithmic source.

## In Scope

- Add Make target(s) for coverage wayfinding and execution. The expected public
  entrypoint is `make coverage`; helper targets may be added under `mk/*` if
  useful.
- Use Clang source-based coverage where available:
  `-fprofile-instr-generate`, `-fcoverage-mapping`, `llvm-profdata`, and
  `llvm-cov`.
- Produce a deterministic local report for `pandaPI/runtime` CTest coverage.
- Include chengdu-owned adoption seams such as native `pandapi_*_native.cpp`
  files only where instrumentation and profile collection are reliable in this
  slice.
- If adoption-seam process-binary coverage is not reliable yet, document the
  exact deferral, the technical reason, and the re-entry condition.
- Define and document coverage exclusions for generated code, inherited
  planner algorithm code, nested third-party code, generated parser/lexer
  sources, build trees, `dist/`, and historical design docs.
- Keep any CI wiring Make-backed. A workflow may call only an existing or newly
  added Make target.
- Update `pandaPI/runtime/README.md` and Arc06 inventory/planning docs only as
  needed to describe the active coverage target, report location, exclusions,
  and remaining gaps.

## Out of Scope

- No parser, grounder, or engine product behavior changes.
- No warning burndown, `clang-tidy`, Clang Static Analyzer, ASan, UBSan, LSan,
  TSan, release packaging, README migration prose, Arc07 tutorial work, or
  Arc08 publication work.
- No hard coverage threshold unless the slice can justify it from current
  evidence. This slice establishes meaningful scoped reporting first.
- No direct CI or documentation instruction to call `./scripts/*` when a Make
  target exists.

## Verification Approach

- Make wayfinding: `make help` must list the coverage entrypoint(s).
- Runtime coverage: the coverage target must rebuild the runtime with Clang
  coverage flags, run the runtime CTest workload, merge profiles, and emit a
  report.
- Scope policy: the report and docs must distinguish included owned code from
  excluded inherited/generated/third-party code.
- Optional adoption-seam coverage: if landed, managed fixtures or smoke tests
  must generate profiles under canonical `pandapi-*` binaries; if deferred, the
  deferral must be explicit and routed.
- Regression proof: the existing Make quality and behavior gates must still
  pass.

## Active Coverage Scope

`make coverage` is the active public entrypoint. It builds `pandaPI/runtime`
with Clang source-based coverage flags, runs the runtime CTest workload, merges
profiles with `llvm-profdata`, and writes generated reports under:

- `build/coverage/runtime/<platform>/report/runtime-coverage-summary.txt`
- `build/coverage/runtime/<platform>/report/runtime-coverage.txt`

The report is owned-runtime evidence. It includes source and tests under
`pandaPI/runtime/` and excludes generated output, inherited planner source,
nested third-party source, `build/`, `dist/`, release packaging output,
historical workbench output, and upstream checkout material from the ownership
claim. Those excluded paths are not presented as chengdu-owned process-policy
coverage.

Adoption seam coverage is deferred for `pandapi_parser_native.cpp`,
`pandapi_grounder_native.cpp`, and `pandapi_engine_native.cpp`. The technical
reason is that the canonical parser, grounder, and engine builds still compile
through copied inherited build trees with generated parser/lexer sources,
nested third-party code, and component-specific Make/CMake flows; collecting
and merging managed-fixture profiles there would mix owned adoption shims with
inherited planner volume and generated code in this slice. Re-entry: add
adoption seam coverage after the native binary build path can instrument those
owned files directly, collect profiles from managed fixtures under canonical
`pandapi-*` binaries, and exclude or separately report inherited/generated and
third-party paths.

## Exit Criteria

- `make coverage` or the accepted coverage entrypoint passes locally and emits
  a report with a stable path.
- The report contains coverage for `pandaPI/runtime` owned source and tests.
- Coverage exclusions and any adoption-seam deferrals are documented.
- New CI workflow behavior, if any, calls Make targets only.
- Slice ledger rows close with evidence, the closing report names any
  deferrals, and CDC can reproduce the coverage target and the existing gates.

## Bubble-up Expectation

The closing report must say whether the coverage signal is strong enough for
Slice04 static-analysis planning and Slice06 sanitizer workload selection. It
must also route any uncovered owned process-policy seams, toolchain
unavailability, or noisy inherited-source coverage separately so Arc06 does not
overclaim release readiness.
