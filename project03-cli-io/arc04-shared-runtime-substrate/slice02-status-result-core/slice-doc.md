# chengdu arc04 / slice02 - status-result-core - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Implement the first real shared runtime semantics in `pandapi-runtime/`: the
Arc03 process-status vocabulary, exit-code mapping, payload-class vocabulary,
and local status/result facade.

This slice turns the accepted Arc03 status taxonomy from prose into tested C++17
types under `pandapi::runtime`, without adopting the runtime in
`pandaPIparser`, `pandaPIgrounder`, or `pandaPIengine` yet.

## 2. Scope

**In:**

- Add public runtime headers for status and result semantics under
  `pandapi-runtime/include/pandapi/runtime/`.
- Define `ProcessStatus` or an equivalent local status object as the shared
  boundary type for later Arc05 binary adoption.
- Implement exact Arc03 status names and exit-code mappings:
  `ok`/0, `domain_no_plan`/2, `cli_usage_error`/10,
  `input_unavailable`/20, `output_unavailable`/21, `input_invalid`/22,
  `unsupported_feature`/30, `legacy_surface`/31,
  `experimental_surface`/32, `future_surface`/33, `timeout`/40,
  `resource_limit`/41, `interrupted`/42, `dependency_failure`/50,
  `child_process_failure`/51, `internal_error`/60, and
  supervisor-observed `signal_terminated` as `128 + signal_number`.
- Represent payload classes, components, surface disposition, artifact/output
  roles, and status names with strongly typed C++ interfaces rather than
  stringly typed call sites.
- Add a local `StatusResult` facade for value-or-status return paths.
- Keep `tl::expected` out of public API and out of direct imports unless CC
  explicitly bubbles up an Arc02 dependency-adoption amendment before
  implementing.
- Add plain C++ seam tests that prove every status maps to the accepted name,
  class, and exit code, including signal termination mapping.
- Update the runtime README and CMake target/test setup.
- Prove `pandapi-runtime` still builds through `scripts/build-runtime.sh`.
- Prove existing parser, grounder, and engine behavior is not changed by this
  slice.

**Out:**

- No edits under `pandaPI/`.
- No adoption of these helpers by parser, grounder, or engine binaries.
- No `pandapi-*` binary wrappers or compatibility-name changes.
- No diagnostics/status I/O facade, tagged `PANDAPI_STATUS` writer/parser,
  TTY/color policy, CLI11 wrapper, fmt integration, Catch2 import, reproc++
  adapter, provenance assembler, or process fixture harness.
- No changes to stdout, stderr, CLI parsing, exit codes, generated artifacts,
  release assets, wolong behavior, or existing build outputs.
- No external dependency import, vendoring, fetching, or package-manager setup.
- No close-set files at open time other than CC's eventual
  `closing-report.md`; leave `cdc-verification.md` for CDC.

## 3. Required Decisions

CC must decide and record:

- the header split for status and result APIs;
- the exact C++ enum/type names for status, status class, component, surface
  disposition, and payload roles;
- whether `StatusResult` is implemented as a small local value type, a
  `std::variant` wrapper, or another C++17 standard-library-only design;
- how a status with dynamic exit mapping, especially `signal_terminated`,
  carries the signal number without making ordinary statuses ambiguous;
- which functions are `noexcept` because they only map enum values to names,
  classes, or numeric codes;
- how seam tests prove the taxonomy exhaustively enough that later Arc05
  binary adoption can trust the helpers.

## 4. Constraints

- Use C++17 and the existing `pandapi::runtime` namespace.
- Follow C++ Core Guidelines pressure:
  - explicit and strongly typed interfaces (`I.1`, `I.4`);
  - systematic error handling strategy (`E.1`, `E.4`, `E.27`);
  - no global mutable state (`I.2`, `R.6`);
  - no raw ownership transfer (`I.11`, `R.11`);
  - self-contained headers with no global `using namespace` (`SF.2`, `SF.7`);
  - `noexcept` on functions that must not throw (`F.6`, `E.12`).
- Use standard-library types first.
- Do not expose third-party dependency types in public headers.
- Do not hide behavior changes in this substrate slice. Runtime tests can prove
  the new helpers, but parser/grounder/engine conformance waits for Arc05.

## 5. Verification Approach

CC verifies by adding seam tests to `pandapi-runtime/tests/`, updating
`pandapi-runtime/CMakeLists.txt`, running `./scripts/build-runtime.sh`, and
running the existing parser/grounder/engine build plus negative smoke gates.
CDC will rerun the ledger commands, inspect the diff, and verify that all
status names and exit codes come from the accepted Arc03 contract.

## 6. Exit Criteria

The ledger reaches final status. `pandapi-runtime` exposes tested status and
result APIs that encode the Arc03 status vocabulary and exit-code mapping;
`StatusResult` is local and standard-library-only; no third-party APIs leak;
no inherited planner source or release-shape path changes; and existing binary
build/smoke behavior remains unchanged.
