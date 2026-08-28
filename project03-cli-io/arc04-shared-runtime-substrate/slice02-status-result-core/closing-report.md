# Slice 02 Closing Report - status-result-core

Closer: CC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Close commit: this report's adding commit
CDC verification: pending

## Capability Verdict

Delivered. Slice02 turns the accepted Arc03 status taxonomy into C++17 runtime
code under `pandapi::runtime` without adopting the helpers in parser, grounder,
or engine.

The runtime now has strongly typed status, class, component, surface
disposition, and output-role vocabulary; a `ProcessStatus` object with dynamic
`signal_terminated` support; stable status-name, class, and exit-code mapping;
and a local `StatusResult<T>` value-or-status facade. The implementation
remains standard-library-only and does not import `tl::expected` or any other
third-party dependency.

## Implementation Decisions

- Header split: `status.hpp` owns process-status vocabulary and mapping API;
  `result.hpp` owns the local `StatusResult<T>` facade.
- Type names: `StatusCode`, `StatusClass`, `Component`,
  `SurfaceDisposition`, `OutputRole`, and `ProcessStatus`.
- Result facade: `StatusResult<T>` wraps `std::variant<T, ProcessStatus>`.
  This keeps Arc02's `tl::expected` pilot gate closed while giving later
  helpers one local return type.
- Signal semantics: ordinary statuses are constructed with
  `ProcessStatus::from_code(...)`; supervisor-observed signal death is
  constructed with `ProcessStatus::signal_terminated(signal_number, ...)`.
  `exit_code(status)` returns `128 + signal_number` only for that dynamic
  status.
- Mapping functions: pure name/class/exit mapping functions and
  `ProcessStatus` accessors are `noexcept`.
- Tests: `status_result_smoke.cpp` exhaustively checks all non-signal Arc03
  statuses, the dynamic signal mapping, component/disposition names, and both
  success and failure `StatusResult<T>` paths.

## Verification

Commands run by CC:

```bash
./scripts/build-runtime.sh
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/cdc-verification.md
```

Results:

- `./scripts/build-runtime.sh`: passed on `macos-arm64`; CTest reported 2/2
  tests passed.
- `./scripts/build-parser.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/build-grounder.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/build-engine.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/smoke-test.sh --negative`: passed on `macos-arm64`; 4 passed, 0
  failed.
- `git diff --check`: passed with no output.
- `git diff --cached --check`: passed with no output.
- Protected-path diff: no output.
- Ledger row count: 18.
- Open-row check: passed with no output.
- CDC verification file check: passed; no CDC file was created.

## Ledger Walk

Rows verified: 18. Done: 18. Deferred: 0. No-op: 0.

| Row | CC disposition |
|-----|----------------|
| F-1 | Done. The open slice doc already documents the status/result scope, no-binary-adoption boundary, Arc02 dependency gate, and C++ Core Guidelines pressure. |
| F-2 | Done. `pandapi-runtime/README.md` now documents the implemented status/result core, no binary adoption, and continuing dependency gate. |
| F-3 | Done. `status.hpp` defines strongly typed `StatusCode`, `StatusClass`, `Component`, `SurfaceDisposition`, and `ProcessStatus` under `pandapi::runtime`. |
| F-4 | Done. The status vocabulary includes all Arc03 statuses, including `SignalTerminated`. |
| F-5 | Done. Public mapping APIs expose status names, status classes, dynamic signal number, and numeric exit codes. |
| F-6 | Done. `status_result_smoke.cpp` asserts every non-signal Arc03 status name and exit-code pair. |
| F-7 | Done. `signal_terminated` is represented and tested as `128 + signal_number`, with signal 15 mapping to 143. |
| F-8 | Done. `result.hpp` provides local `StatusResult<T>` success/failure paths without exposing `tl::expected`. |
| F-9 | Done. Runtime source/build/test surfaces contain no external dependency fetch, discovery, direct include, or third-party vocabulary leak. |
| F-10 | Done. Public headers avoid global using directives, mutable namespace-scope state, object definitions, raw ownership transfer, and explicit allocation/deallocation. |
| F-11 | Done. Non-failing mapping/accessor functions are marked `noexcept` and exercised through public headers. |
| F-12 | Done. CMake builds `src/status.cpp` and registers `pandapi_status_result_smoke`. |
| F-13 | Done. Runtime build/test entrypoint passed. |
| F-14 | Done. Existing parser, grounder, and engine build scripts passed. |
| F-15 | Done. Existing negative smoke behavior passed. |
| F-16 | Done. Protected planner/workflow/release-shape diff produced no output. |
| F-17 | Done. This closing report includes the required verdict, decisions, verification, ledger walk, silent-drop check, and Arc04 bubble-up. |
| F-18 | Done. No `cdc-verification.md` was created by CC. |

## Silent-Drop Check

Scope-as-specified versus scope-as-delivered:

- Public status headers: delivered with `status.hpp`.
- Strongly typed status, class, component, and surface-disposition vocabulary:
  delivered.
- Exact Arc03 status names and exit-code mapping: delivered and tested.
- Supervisor-observed `signal_terminated` as `128 + signal_number`: delivered
  and tested.
- Local `StatusResult<T>` facade: delivered with standard-library
  implementation.
- Seam tests for accepted names, classes, and exit codes: delivered through
  `pandapi_status_result_smoke`.
- README and CMake updates: delivered.
- No parser/grounder/engine adoption: preserved.
- No third-party dependency import: preserved.
- No CDC verification file: preserved.

No specified deliverable was dropped. No deferrals or no-ops were recorded.

## Bubble-up to Arc04

Slice02 delivers the Arc04 slice-breakdown item: `ProcessStatus`, exit-code
mapping, payload-class vocabulary, and local status/result facade, with seam
tests and no binary adoption.

Slice03 planning does not need a scope change. It can consume the status and
result APIs when implementing diagnostics/process I/O and tagged
`PANDAPI_STATUS` writer/parser behavior. The main carry-forward constraint is
that `StatusClass` names in code are slightly more explicit than the prose
table's broad "external/dependency failure" family so later status-line fields
can distinguish timeout/resource/interruption, dependency/child failures, and
supervisor-owned signal termination without diagnostic prose.

## What Worked

- Keeping mapping code in `status.cpp` kept public headers self-contained and
  declaration-oriented.
- The dynamic signal constructor made the `signal_terminated` special case
  explicit without weakening ordinary status mapping.
- Plain C++ tests kept Catch2 deferred to its owning slice while still proving
  the full status table.
