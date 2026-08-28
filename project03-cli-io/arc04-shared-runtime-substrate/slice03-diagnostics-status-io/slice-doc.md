# chengdu arc04 / slice03 - diagnostics-status-io - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Implement the shared runtime facade for Arc03 diagnostics/status process I/O:
stdout/stderr ownership helpers, a tagged-text `PANDAPI_STATUS` writer/parser,
final-status flushing, and partial-output policy primitives.

This slice turns the accepted Arc03 stdout/stderr/status contract into tested
C++17 helpers under `pandapi::runtime`, using the Slice02 `ProcessStatus` and
`StatusResult<T>` APIs as inputs. It still does not adopt the helpers in
`pandaPIparser`, `pandaPIgrounder`, or `pandaPIengine`.

## 2. Scope

**In:**

- Add public runtime APIs for status I/O under
  `pandapi-runtime/include/pandapi/runtime/`.
- Represent a final status record that combines:
  - `ProcessStatus`;
  - component and surface disposition from Slice02;
  - a surface name such as `normal_hddl_parse`, `normal_htn_grounding`, or
    `normal_search`;
  - optional conditional fields such as duration, path roles, operation,
    errno, signal details, cleanup, finding refs, and partial-output policy.
- Implement a writer for exactly one tagged-text final status line using the
  Arc03 prefix:

  ```text
  PANDAPI_STATUS\tstatus=ok\tcomponent=engine\tsurface=normal_search\tsurface_disposition=supported\texit_code=0\tclass=success\tduration_ms=37
  ```

- Implement a parser for tagged status records that returns `StatusResult` or
  an equivalent local value-or-status result and rejects malformed input.
- Ensure emitted values are single-line and escaped or rejected where necessary
  so status records cannot contain embedded newlines, tabs, ANSI escapes, or
  placeholder prose.
- Implement status-stream selection helpers that encode Arc03 stdout/stderr
  ownership:
  - stderr is always a legal status stream;
  - stdout is legal only when stdout is not carrying a data artifact or human
    informational output;
  - no helper may mix diagnostics/progress/statistics with stdout artifacts.
- Implement final-status flushing behavior at the facade level.
- Implement partial-output policy vocabulary for at least absent, retained,
  discarded, complete, and unknown outcomes.
- Update runtime README and CMake/test registration.
- Fix skeleton-era README wording that still says these helpers do not exist.
- Add standard-library-only seam tests for status record writing, parsing,
  stream legality, escaping/rejection, final flush behavior, and
  partial-output vocabulary.
- Prove `pandapi-runtime` still builds through `scripts/build-runtime.sh`.
- Prove existing parser, grounder, and engine behavior is not changed by this
  slice.

**Out:**

- No edits under `pandaPI/`.
- No adoption of these helpers by parser, grounder, or engine binaries.
- No `pandapi-*` binary wrappers or inherited-name migration work.
- No CLI11 wrapper, CLI parse policy, TTY/color policy, provenance assembler,
  Catch2 import, reproc++ adapter, process fixture harness, or Arc05
  executable contract migration.
- No JSON Lines event stream and no `nlohmann/json` re-entry.
- No external dependency import, vendoring, fetching, or package-manager setup.
- No direct fmt usage in this slice. The API should be shaped so fmt can later
  sit behind the diagnostics/process facade, but dependency import waits for a
  dedicated approved gate or explicit bubble-up.
- No changes to stdout, stderr, CLI parsing, exit codes, generated artifacts,
  release assets, wolong behavior, or existing build outputs.
- No close-set files at open time other than CC's eventual
  `closing-report.md`; leave `cdc-verification.md` for CDC.

## 3. Required Decisions

CC must decide and record:

- the exact header/source split for status I/O APIs;
- whether the main value type is named `StatusRecord`, `FinalStatusRecord`,
  `StatusLine`, or another local type;
- how optional fields are represented without making placeholder prose part of
  the machine contract;
- whether malformed tagged text maps to `input_invalid`, `internal_error`, or
  a more precise local parse status when the parser is used only by tests and
  supervisors;
- how surface names are typed or validated while leaving per-binary surface
  adoption to Arc05;
- how to test final flushing without relying on inherited binaries;
- how the facade remains standard-library-only while preserving the Arc02 fmt
  adoption path behind a later diagnostics facade.

## 4. Constraints

- Use C++17 and the existing `pandapi::runtime` namespace.
- Build on Slice02 `ProcessStatus`, `StatusCode`, `StatusClass`, `Component`,
  `SurfaceDisposition`, `OutputRole`, and `StatusResult<T>`.
- Follow C++ Core Guidelines pressure:
  - explicit and strongly typed interfaces (`I.1`, `I.4`);
  - systematic error handling strategy (`E.1`, `E.4`, `E.27`);
  - no global mutable state (`I.2`, `R.6`);
  - no raw ownership transfer (`I.11`, `R.11`);
  - self-contained headers with no global `using namespace` (`SF.2`, `SF.7`);
  - RAII/resource discipline around streams and flushing (`R.1`, `R.5`);
  - `noexcept` on pure mapping functions that must not throw (`F.6`, `E.12`).
- Use standard-library types first.
- Do not expose third-party dependency types in public headers.
- Do not hide behavior changes in this substrate slice. Runtime tests can prove
  helpers, but parser/grounder/engine conformance waits for Arc05.

## 5. Verification Approach

CC verifies by adding seam tests to `pandapi-runtime/tests/`, updating
`pandapi-runtime/CMakeLists.txt`, running `./scripts/build-runtime.sh`, and
running the existing parser/grounder/engine build plus negative smoke gates.
CDC will rerun the ledger commands, inspect the diff, and verify that the
status I/O helpers reproduce the accepted Arc03 tagged-text, stream ownership,
flushing, and partial-output requirements without changing inherited binaries.

## 6. Exit Criteria

The ledger reaches final status. `pandapi-runtime` exposes tested diagnostics
and status I/O APIs that can emit and parse final `PANDAPI_STATUS` records,
encode stdout/stderr ownership, flush final status output, represent partial
output policy, and remain standard-library-only with no binary adoption. No
inherited planner source or release-shape paths change.
