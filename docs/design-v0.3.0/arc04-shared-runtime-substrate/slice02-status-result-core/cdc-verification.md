# CDC Verification: status-result-core

Verifier: CDC
Branch: `release/0.3.x`
Verified on: 2026-08-09
Verified commit: `e277c0a568269d18d870789f8e6b60ada479c7a9`
CC closing report:
[`closing-report.md`](closing-report.md)

## Verdict

Accepted. Slice02 is CDC-verified.

The committed slice turns the accepted Arc03 status taxonomy into tested
C++17 runtime code under `pandapi::runtime`. It adds strongly typed status,
status-class, component, surface-disposition, and output-role vocabulary;
`ProcessStatus`; dynamic `signal_terminated` exit mapping; and a local
standard-library-only `StatusResult<T>` facade. No parser, grounder, or engine
binary adopts these helpers in this slice.

## Commit and Scope Check

- `git status --short --branch`: clean on `release/0.3.x` before CDC docs.
- `git show --name-status --format=full e277c0a568269d18d870789f8e6b60ada479c7a9`:
  commit includes the required `Codex` and `Billo AI` co-author trailers.
- Commit files are limited to the slice close set and `pandapi-runtime/`.
- `git diff --check e277c0a568269d18d870789f8e6b60ada479c7a9^..e277c0a568269d18d870789f8e6b60ada479c7a9`:
  passed with no output.
- `git diff --name-only e277c0a568269d18d870789f8e6b60ada479c7a9^..e277c0a568269d18d870789f8e6b60ada479c7a9 -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release`:
  produced no output.

## Independent Verification

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | accepted | Slice open set still documents the Arc03 scope, no-binary-adoption boundary, Arc02 dependency gate, and C++ Core Guidelines pressure. |
| F-2 | accepted | `pandapi-runtime/README.md` documents the implemented status/result core, `ProcessStatus`, `StatusResult`, exit-code mapping, no binary adoption, and dependency gate. |
| F-3 | accepted | `status.hpp` defines `StatusCode`, `StatusClass`, `Component`, `SurfaceDisposition`, `ProcessStatus`, and `pandapi::runtime`. |
| F-4 | accepted | Status vocabulary includes all Arc03 statuses: `Ok`, `DomainNoPlan`, `CliUsageError`, `InputUnavailable`, `OutputUnavailable`, `InputInvalid`, `UnsupportedFeature`, `LegacySurface`, `ExperimentalSurface`, `FutureSurface`, `Timeout`, `ResourceLimit`, `Interrupted`, `DependencyFailure`, `ChildProcessFailure`, `InternalError`, and `SignalTerminated`. |
| F-5 | accepted | Runtime mapping APIs expose `status_name`, `status_class`, `exit_code`, `signal_number`, and signal exit-code handling without diagnostic prose parsing. |
| F-6 | accepted | Tests assert exact non-signal Arc03 status-name and exit-code mappings from `ok`/0 through `internal_error`/60. |
| F-7 | accepted | `ProcessStatus::signal_terminated(15, ...)` is tested as `signal_terminated` with exit `128 + signal_number`, specifically `143` for signal 15. |
| F-8 | accepted | `result.hpp` defines local `StatusResult<T>` success/failure paths backed by standard-library types and `ProcessStatus`, with no `tl::expected` exposure. |
| F-9 | accepted | Dependency-gate search found no fetch/discovery/direct include or vocabulary leak for fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, or GSL in runtime source/build/test surfaces. |
| F-10 | accepted | Public headers avoid global `using namespace`, mutable namespace-scope state, object definitions, raw ownership transfer, and explicit allocation/deallocation patterns. |
| F-11 | accepted | Non-failing mapping/accessor APIs are declared and defined with `noexcept`; runtime tests exercise the public headers. |
| F-12 | accepted | `pandapi-runtime/CMakeLists.txt` builds `src/status.cpp` and registers `pandapi_status_result_smoke`. |
| F-13 | accepted | `./scripts/build-runtime.sh` passed on `macos-arm64`; CTest reported 2/2 tests passed. |
| F-14 | accepted | `./scripts/build-parser.sh && ./scripts/build-grounder.sh && ./scripts/build-engine.sh` passed on `macos-arm64`; inherited warnings only. |
| F-15 | accepted | `./scripts/smoke-test.sh --negative` passed on `macos-arm64`: 4 passed, 0 failed. |
| F-16 | accepted | `git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release` produced no output. |
| F-17 | accepted | CC `closing-report.md` exists and includes capability verdict, implementation decisions, verification, ledger walk, silent-drop check, and Arc04 bubble-up. |
| F-18 | accepted | CC did not create `cdc-verification.md`; CDC created this file independently after reproducing the evidence. |

Additional close-set checks:

- Ledger row count: `18`.
- Open ledger row check: no `| open |` rows remain.
- CC closing report row walk covers all 18 rows and records no deferrals or
  no-ops.

## Code Review Notes

No blocking findings.

Carry forward two non-blocking notes:

- `ProcessStatus::signal_terminated(...)` is the correct dynamic constructor
  and is tested. Because `StatusCode::SignalTerminated` is still accepted by
  `ProcessStatus::from_code(...)`, later adoption work should either avoid
  that construction path by convention or add a guard/helper before binary
  signal mapping becomes public behavior.
- `pandapi-runtime/README.md` now has a correct Slice02 status/result section,
  but its earlier Arc03-input prose still speaks in slice01 skeleton terms.
  The next runtime documentation touch should distinguish "implemented today"
  from "future Arc04 helper" more cleanly.

Neither note weakens this slice's delivered capability because the helpers are
not yet adopted by parser, grounder, or engine, and the tested public path for
dynamic signal termination is present.

## Bubble-up

Arc04 can proceed to slice03. The accepted substrate now includes:

- `StatusCode` and exact Arc03 status-name mapping;
- `StatusClass` and status-class name mapping;
- `Component`, `SurfaceDisposition`, and `OutputRole` vocabulary;
- `ProcessStatus` with dynamic signal exit mapping;
- `StatusResult<T>` as the local standard-library-only facade;
- CTest seam coverage for status/result behavior.

Slice03 should consume these APIs when implementing diagnostics/status I/O and
tagged `PANDAPI_STATUS` writer/parser behavior. Keep the dependency gates in
force: fmt belongs only behind the diagnostics/process facade, and no binary
adoption belongs in slice03 unless its ledger explicitly expands.
