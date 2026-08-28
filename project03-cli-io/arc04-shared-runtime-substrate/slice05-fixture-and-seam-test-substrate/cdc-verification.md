# CDC Verification: fixture-and-seam-test-substrate

Date: 2026-08-09
Branch: `release/0.3.x`
Role: CDC
Verified commit: `4741113b744a5b2e7c39c9ee8d16ad0dea7ada56`

## Verdict

Accepted.

Arc04 slice05 now has a CDC-verified fixture and seam-test substrate under
`pandapi::runtime`. The accepted implementation adds typed fixture records,
normalization helpers, structured comparison results, final
`PANDAPI_STATUS` matching through the accepted Slice03 parser, local
Linux/macOS process observation, wait-status/signal mapping, CI-safe
harness-timeout coverage, and an optional local Catch2 seam gate without
binary adoption or release-shape changes.

This verification does not assert parser, grounder, or engine managed-process
conformance. The substrate is accepted as reusable Arc05 test infrastructure.

## Source and Scope Checks

| Check | Result | Evidence |
|-------|--------|----------|
| Worktree clean before CDC docs | accepted | `git status --short --branch` showed only `## release/0.3.x`. |
| Commit scope | accepted | `git show --stat --oneline --summary 4741113b...` showed the slice ledger/closing report, runtime CMake/README, fixture/normalization/process headers and sources, and fixture tests only. |
| Commit trailers | accepted | `git show --format=fuller --name-only --no-renames 4741113b...` showed both required co-author trailers. |
| No preexisting CDC verification | accepted | `test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md` exited nonzero before this file was created. |
| Protected source/release paths | accepted | `git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release` produced no output during CDC verification. |

## Independent Verification

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | accepted | Slice open set documents fixture/seam-test scope, Arc03 matrix inputs, black-box ownership, Catch2 gate, no-binary-adoption boundary, and Core Guidelines pressure. |
| F-2 | accepted | Runtime README documents fixture records, seam tests, process observation, normalization, Catch2, and no binary adoption. |
| F-3 | accepted | Public headers exist for `fixture.hpp`, `normalization.hpp`, and `process_fixture.hpp`, with `FixtureSpec`, `NormalizationRules`, `ProcessObservation`, and `StatusResult` vocabulary. |
| F-4 | accepted | Fixture types cover command identity, argv, environment, stdin, stream roles, artifact expectations, expected exit, wait status, final status, normalization, and safety policy. |
| F-5 | accepted | `run_process_fixture` captures stdout, stderr, exit code, wait status, timeout disposition, and cleanup state without reproc++ or Boost.Process. |
| F-6 | accepted | Probe vocabulary distinguishes harness timeout, binary timeout, resource limit, quarantine, manual, and CI-safe/default policy. |
| F-7 | accepted | Wait-status helpers cover exited/signaled state, SIGINT, SIGTERM, SIGKILL, and supervisor `128 + signal_number` mapping. |
| F-8 | accepted | Final status matching delegates to `parse_status_record`; no human diagnostic regex parser was introduced. |
| F-9 | accepted | Normalization helpers cover path tokens, LF endings, ANSI rejection/stripping, duration, timestamp, build metadata, errno, and signal predicates. |
| F-10 | accepted | Comparison helpers distinguish golden fields from semantic predicates and return structured expected/actual mismatch records. |
| F-11 | accepted | Stream and artifact helpers encode output roles, artifact states, and ANSI-forbidden machine streams. |
| F-12 | accepted | Runtime tests exercise fixture construction, normalization, final status matching, comparison failures, process success/failure, and a bounded harness timeout. |
| F-13 | accepted | Runtime CMake builds the new sources and registers fixture substrate tests. |
| F-14 | accepted | Catch2 integration is optional, local, test-only, absent from public headers/sources, and not fetched by the default build. |
| F-15 | accepted | Arc02 dependency-gate search found no CLI11, fmt, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL, benchmark, SAT/BDD/CUDD, H2/cpddl, or parser-generator import. |
| F-16 | accepted | Release packaging/manifests do not mention Catch2, fixture test paths, or runtime test material; tracked `release`, `dist`, and `build` diffs were empty. |
| F-17 | accepted | Public-header hazard search found no global `using namespace`, mutable namespace-scope definitions, raw ownership-transfer pointer APIs, explicit allocation/deallocation, or `namespace std` additions. |
| F-18 | accepted | No parser, grounder, engine, `dist`, `build`, or `release` adoption diff exists, and no `pandapi-*` release entry points were added. |
| F-19 | accepted | `./scripts/build-runtime.sh` passed on macos-arm64 with 5/5 CTests; optional Catch2 was absent and skipped. |
| F-20 | accepted | `./scripts/build-parser.sh`, `./scripts/build-grounder.sh`, and `./scripts/build-engine.sh` passed on macos-arm64 with inherited warnings only. |
| F-21 | accepted | `./scripts/smoke-test.sh --negative` passed: 4 passed, 0 failed. |
| F-22 | accepted | Protected-path diff check produced no output. |
| F-23 | accepted | CC closing report is present and contains capability verdict, implementation decisions, verification, ledger walk, silent-drop check, and Bubble-up to Arc04. |
| F-24 | accepted | CC did not create `cdc-verification.md`. |

CDC also reproduced the ledger row count and open-row checks:

```text
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md
24
rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md
```

The open-row search returned no output.

## Build and Smoke Gates

```text
./scripts/build-runtime.sh
```

Result:

```text
100% tests passed, 0 tests failed out of 5
build-runtime.sh: OK: /Users/oubiwann/lab/billosys/chengdu/build/runtime/macos-arm64
```

The default runtime build reported Catch2 absent and skipped the optional
test-only fixture seam gate.

```text
./scripts/build-parser.sh && ./scripts/build-grounder.sh && ./scripts/build-engine.sh && ./scripts/smoke-test.sh --negative
```

Result:

```text
build-parser.sh: OK: /Users/oubiwann/lab/billosys/chengdu/dist/macos-arm64/pandaPIparser
build-engine.sh: OK: /Users/oubiwann/lab/billosys/chengdu/dist/macos-arm64/pandaPIengine
smoke-test.sh: SUMMARY: 4 passed, 0 failed (negative mode, macos-arm64)
```

The combined command exited 0. The grounder build also completed successfully;
the command output included inherited compiler warnings from vendored source.

## Code Review Notes

No blocking findings remain.

The local process runner uses RAII file-descriptor ownership in the
implementation, hides POSIX handles from public headers, drains stdout and
stderr with nonblocking reads while polling `waitpid`, and records timeout
cleanup state. That avoids the primary pipe deadlock risk for planner-style
fixtures that produce output while the harness observes them.

One non-blocking Arc05 caveat is worth preserving: `stdin` is represented in
the fixture model, but the current runner writes stdin synchronously before
entering its stdout/stderr drain loop. That is acceptable for this substrate
slice and for the current zero/small-stdin planner fixtures, but Arc05 should
not treat the helper as a general full-duplex process transport for large
stdin payloads without adding a focused regression or moving stdin writes into
the same nonblocking observation loop.

Arc04 still has the intended no-binary-adoption boundary. This verification
accepts reusable fixture, normalization, process-observation, and optional
Catch2 seam infrastructure only.

## Bubble-up to Arc04

Arc04 can proceed to slice06 from these accepted inputs:

- typed fixture records for command, environment, stdin, streams, artifacts,
  exits, wait status, final status, normalization, and safety policy;
- normalization helpers for paths, line endings, ANSI handling, durations,
  timestamps, build metadata, errno, and signals;
- structured golden and semantic comparison results;
- final status matching through the accepted tagged status parser;
- local Linux/macOS process observation for CI-safe harness tests;
- wait-status and signal helpers that preserve supervisor-visible
  `128 + signal_number` mapping;
- optional local Catch2 seam integration that remains test-only and absent
  from release packaging;
- confirmed no adoption into parser, grounder, engine, command names,
  workflows, release assets, or wolong-facing surfaces.

Slice06 should synthesize integration readiness and Arc05 handoff constraints,
including the stdin/full-duplex caveat and the continued separation between
fixture substrate proof and executable managed-process conformance.
