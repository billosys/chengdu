# CDC Verification - Arc06 Slice03 - coverage-gate

Verifier: CDC
Verified on: 2026-08-11
Implementation commit reviewed: `dffa6baf`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc06 Slice03 is CDC-verified.

The slice delivered a Make-backed Clang source-based coverage gate for
`pandaPI/runtime`, with deterministic local reports under
`build/coverage/runtime/<platform>/report/`. The coverage claim is scoped to
owned runtime source and tests; inherited planner, generated, nested
third-party, `build`, and `dist` paths are excluded from the owned-code signal.

The adoption-seam coverage deferral is valid. Coverage for
`pandapi_parser_native.cpp`, `pandapi_grounder_native.cpp`, and
`pandapi_engine_native.cpp` is explicitly deferred until managed-fixture
profile collection can isolate those owned files from copied inherited build
trees, generated sources, and third-party code.

## Source and Scope Checks

- `git show --stat --oneline --name-status dffa6baf` confirmed the commit is
  limited to Make coverage wiring, Arc06 Slice03/inventory docs, and owned
  runtime docs/source.
- `git show --format=fuller --no-patch dffa6baf` confirmed both required
  co-author trailers are present.
- The runtime helper fix in `pandaPI/runtime/src/cli_policy.cpp` computes the
  inherited-command flag before moving the invoked command string. This is
  within owned runtime/process-policy scope and is exercised by runtime CTest
  under `make coverage` plus the normal `make test` managed-contract gates.
- An adapted committed-path boundary check confirmed no touched top-level
  README, release package/publish, release workflow, Arc07, or Arc08 paths.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | `make help | rg -n "coverage"` lists `make coverage` under Quality. |
| F-2 | reproduced | Coverage implementation grep found Clang source-coverage flags and LLVM tooling terms in `Makefile`, `mk/coverage.mk`, runtime docs, and Slice03 docs. |
| F-3 | reproduced | `make coverage` passed: built instrumented runtime, ran 5/5 runtime CTest cases, merged profiles, and emitted `runtime-coverage-summary.txt` and `runtime-coverage.txt`. |
| F-4 | reproduced | Coverage detail and active docs contain `pandaPI/runtime` paths; Slice03 docs and runtime README avoid the retired runtime path spelling. |
| F-5 | reproduced | Slice03 docs and `pandaPI/runtime/README.md` document generated, inherited, third-party, build, dist, and excluded-path policy. |
| F-6 | reproduced as deferred | Adoption-seam coverage deferral names the affected native files, technical reason, and re-entry condition in Slice03 docs and the Arc06 inventory. |
| F-7 | reproduced | `make coverage` writes under ignored `build/coverage/runtime/<platform>/...`; `git status --short -- build dist` stayed empty. |
| F-8 | reproduced | `make safety-checks && make actionlint` passed; no workflow change was made. |
| F-9 | reproduced | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` passed. |
| F-10 | reproduced | Adapted post-commit boundary check found no touched top-level README, release package/publish, release workflow, Arc07, or Arc08 surfaces. |

## Commands Reproduced

```text
make coverage
make help | rg -n "coverage"
coverage implementation grep for Clang/LLVM terms
coverage report/runtime path grep and retired-path grep
coverage exclusion-policy grep
adoption-seam deferral grep
git status --short -- build dist
make safety-checks && make actionlint
commit path-boundary check for dffa6baf
make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check
```

Observed results:

- `make coverage`: passed, using `/usr/bin/clang++`,
  `/Library/Developer/CommandLineTools/usr/bin/llvm-profdata`, and
  `/Library/Developer/CommandLineTools/usr/bin/llvm-cov`.
- Runtime CTest under coverage: 5 passed, 0 failed.
- Coverage summary total: 74.09% line coverage over the included owned runtime
  source and tests.
- Managed fixtures during `make test`: parser 298/0, grounder 269/0, engine
  312/0, pipeline 129/0.
- Positive smoke: 3/0. Negative smoke: 4/0.
- Provenance check passed for all three components against `vendor.env` and
  Git state on `macos-arm64`.

## Code Review Notes

No blocking findings remain.

The `mk/coverage.mk` implementation is appropriately Make-backed and fails
explicitly if `clang++`, `llvm-profdata`, or `llvm-cov` cannot be resolved from
`PATH` or `xcrun`. Generated reports remain local build artifacts.

The coverage report is honest about scope. It reports runtime files as report
sections; any `pandaPI/engine` text observed in the detail report is fixture
string data inside runtime tests, not included engine source coverage.

The runtime helper bug fix is acceptable in Slice03 because coverage exposed a
defect in owned runtime process-policy code, and the normal managed-contract
fixtures still pass after the fix. It does not edit parser, grounder, or engine
product source.

## Bubble-up to Arc06

Slice03 delivered the coverage evidence assigned by the Arc06 plan. The runtime
coverage signal is useful enough for Slice04 static-analysis planning because
`pandaPI/runtime` now has a coverage build tree, `compile_commands.json`, and
source/test evidence showing where owned runtime tests are thin.

Remaining gaps are disclosed and routed:

- adoption-seam coverage remains deferred until canonical `pandapi-*` managed
  fixture profiles can isolate owned native files from inherited/generated and
  third-party build volume;
- coverage is local evidence in this slice, not yet a CI job;
- optional Catch2 seam coverage remains dependent on a locally installed
  Catch2 CMake package.

Arc06 should now open Slice04 `static-analysis-gate` with no additional
sequencing change.
