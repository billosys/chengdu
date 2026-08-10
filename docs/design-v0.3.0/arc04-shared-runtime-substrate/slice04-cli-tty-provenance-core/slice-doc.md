# chengdu arc04 / slice04 - cli-tty-provenance-core - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Implement the shared runtime core for common CLI policy, TTY/color decisions,
command identity, and version/provenance field assembly.

This slice turns the accepted Arc03 command naming, invocation-mode,
parse-error, color/TTY, help/version, and provenance contracts into tested
C++17 helpers under `pandapi::runtime`. It builds on the Slice02
status/result APIs and the Slice03 status I/O helpers, but it still does not
replace parser, grounder, or engine CLI parsing and does not add `pandapi-*`
entry points.

## 2. Scope

**In:**

- Add public runtime APIs under `pandapi-runtime/include/pandapi/runtime/` for:
  - common CLI policy values;
  - command identity for canonical `pandapi-*` names and inherited
    `pandaPI*` compatibility names;
  - output/status conflict validation;
  - `--help`, `--version`, and `--provenance` informational-command policy;
  - TTY/color policy, including `--color`, `--no-color`, `--no-colour`, and
    `NO_COLOR`;
  - version/provenance field assembly.
- Implement typed C++17 value types for:
  - invocation mode: human CLI versus supervised/process-manager;
  - status target: none, stderr, stdout;
  - output target: file path, stdout, or absent;
  - color mode: auto, always, never;
  - terminal observation: tty, not tty, unknown;
  - canonical command identity, invoked command name, and compatibility status.
- Implement validation helpers that return the local `StatusResult<T>` facade
  or a value/status equivalent and map invalid CLI policy to
  `cli_usage_error` without touching input/model processing.
- Encode Arc03 option conflicts at the runtime-policy level, including:
  - `--status=stdout` when stdout is already a data artifact or human
    informational output;
  - mutually exclusive `--output` and inherited positional-output alias;
  - malformed enum values;
  - `--color=always` with supervised or machine-parsed output.
- Implement color policy so disabling controls win:
  `--color=never`, `--no-color`, `--no-colour`, and `NO_COLOR` disable ANSI
  everywhere, including when another control tries to enable color.
- Implement ANSI legality helpers: ANSI is allowed only for human stderr on a
  suitable TTY and is forbidden for stdout artifacts, tagged status records,
  supervised output, machine output, files, pipes, and no-TTY output.
- Implement provenance assembly from caller-supplied fields. The runtime must
  not shell out to git or infer build metadata by itself.
- Include stable version/provenance field names required by Arc03:
  canonical command, invoked compatibility name where applicable, component,
  chengdu version, managed-process contract version, upstream pandaPI project,
  upstream commit, chengdu source prefix, chengdu build commit, platform,
  compiler, license pointer, and NOTICE pointer.
- Update runtime README and CMake/test registration.
- Add standard-library-only seam tests for CLI policy conflicts, command
  identity, informational-command roles, color/TTY policy, provenance field
  assembly, and CLI11 dependency gates.
- Prove `pandapi-runtime` still builds through `scripts/build-runtime.sh`.
- Prove existing parser, grounder, and engine behavior is not changed by this
  slice.

**Out:**

- No edits under `pandaPI/`.
- No adoption of these helpers by parser, grounder, or engine binaries.
- No `pandapi-*` wrappers, symlinks, copied binaries, renamed binaries, or
  inherited-name migration work.
- No actual replacement of `gengetopt` or per-binary CLI parsing.
- No CLI11 import, vendoring, fetching, package-manager setup, `find_package`,
  or direct include in this slice. This slice may create the local policy
  facade that a later CLI11 adapter must target.
- No fmt, Catch2, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, or
  GSL import.
- No process fixture harness or golden-output runner; slice05 owns that test
  substrate.
- No release asset, manifest, checksum, wolong, workflow, or top-level README
  changes.
- No changes to stdout, stderr, CLI parsing, exit codes, generated artifacts,
  release assets, wolong behavior, or existing build outputs.
- No close-set files at open time other than CC's eventual
  `closing-report.md`; leave `cdc-verification.md` for CDC.

## 3. Required Decisions

CC must decide and record:

- the exact header/source split for common CLI policy, TTY/color policy, and
  provenance APIs;
- whether policy helpers live in one cohesive module or a few small modules;
- the exact type names for invocation mode, status target, output target,
  color mode, terminal observation, and command identity;
- how to represent caller-supplied provenance fields without making unknown
  placeholder prose part of the stable contract;
- how validation reports the specific usage rule, option name, or conflict
  while returning `cli_usage_error`;
- how to keep TTY detection testable without depending on the current shell's
  interactive state;
- how the local facade makes CLI11 adoption possible later without importing
  or exposing CLI11 now.

## 4. Constraints

- Use C++17 and the existing `pandapi::runtime` namespace.
- Build on Slice02 `ProcessStatus`, `StatusCode`, `StatusClass`,
  `Component`, `SurfaceDisposition`, `OutputRole`, and `StatusResult<T>`.
- Build on Slice03 `StatusStream`, `status_stream_allowed`, and status I/O
  semantics instead of reinterpreting stdout/stderr policy.
- Follow C++ Core Guidelines pressure:
  - explicit and strongly typed interfaces (`I.1`, `I.4`);
  - low-ambiguity function parameters and result structs (`I.23`, `I.24`,
    `F.20`, `F.21`);
  - systematic error handling strategy (`E.1`, `E.4`, `E.27`);
  - no global mutable state (`I.2`, `R.6`);
  - no raw ownership transfer (`I.11`, `R.11`);
  - no unbounded casts, enum/int confusion, or magic constants (`ES.45`,
    `ES.46`, `ES.48`);
  - self-contained headers with no global `using namespace` (`SF.2`, `SF.7`,
    `SF.11`);
  - standard-library string and stream vocabulary (`SL.str.1`, `SL.str.2`,
    `SL.io.2`, `SL.io.3`);
  - `noexcept` on pure mapping functions that must not throw (`F.6`, `E.12`).
- Use standard-library types first.
- Do not expose third-party dependency types in public headers.
- Do not hide behavior changes in this substrate slice. Runtime tests can
  prove helpers, but parser/grounder/engine conformance waits for Arc05.

## 5. Verification Approach

CC verifies by adding seam tests to `pandapi-runtime/tests/`, updating
`pandapi-runtime/CMakeLists.txt`, running `./scripts/build-runtime.sh`, and
running the existing parser/grounder/engine build plus negative smoke gates.
CDC will rerun the ledger commands, inspect the diff, and verify that the CLI,
TTY/color, command identity, and provenance helpers reproduce the accepted
Arc03 rules without changing inherited binaries or importing CLI11 early.

## 6. Exit Criteria

The ledger reaches final status. `pandapi-runtime` exposes tested common CLI
policy, TTY/color policy, command identity, and provenance assembly APIs that
can be consumed by Arc05 binary adoption. The implementation remains
standard-library-only, preserves the CLI11 gate for later golden/process
tests, and makes no inherited planner or release-shape behavior change.
