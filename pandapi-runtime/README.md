# pandapi-runtime

`pandapi-runtime/` is the chengdu-owned C++ runtime substrate for the
`pandapi-*` managed-process work planned in Arc04. It lives outside `pandaPI/`
so upstream planner history remains separate from chengdu process, build, and
test policy.

## Scope

- Provide the shared `pandapi::runtime` namespace for later Arc04 helpers.
- Build as a C++17, standard-library-only library target.
- Keep public headers self-contained and implementation code under `src/`.
- Provide local smoke/seam tests for runtime code before any binary adoption.
- Support later parser, grounder, and engine migration without changing their
  current command behavior in this slice.

## Non-Goals

- No edits to inherited planner source under `pandaPI/`.
- No `pandapi-parser`, `pandapi-grounder`, or `pandapi-engine` wrappers.
- No changes to stdout, stderr, exit status, CLI parsing, provenance, release
  assets, wolong behavior, or existing build outputs.
- No external dependency import, vendoring, fetching, or hidden facade
  implementation in slice01.

## Source Layout

```text
pandapi-runtime/
  CMakeLists.txt
  cmake/
  include/pandapi/runtime/cli_policy.hpp
  include/pandapi/runtime/fixture.hpp
  include/pandapi/runtime/normalization.hpp
  include/pandapi/runtime/process_fixture.hpp
  include/pandapi/runtime/provenance.hpp
  include/pandapi/runtime/result.hpp
  include/pandapi/runtime/runtime.hpp
  include/pandapi/runtime/status.hpp
  include/pandapi/runtime/status_io.hpp
  include/pandapi/runtime/tty.hpp
  src/cli_policy.cpp
  src/fixture.cpp
  src/normalization.cpp
  src/process_fixture.cpp
  src/provenance.cpp
  src/runtime.cpp
  src/status.cpp
  src/status_io.cpp
  src/tty.cpp
  tests/cli_tty_provenance_smoke.cpp
  tests/fixture_catch2_smoke.cpp
  tests/fixture_substrate_smoke.cpp
  tests/runtime_smoke.cpp
  tests/status_result_smoke.cpp
  tests/status_io_smoke.cpp
```

The current CMake shape produces one static library target,
`pandapi_runtime`, with an alias target `pandapi::runtime`. The smoke test is a
small executable registered with CTest. Build output is generated under
`build/runtime/` by `scripts/build-runtime.sh`.

## C++ Floor

The runtime uses C++17. New code should prefer standard-library ownership,
paths, strings, streams, chrono/timeouts, containers, and scoped resource
management before considering third-party dependencies.

## Arc03 Inputs

Arc03 defines the managed-process contract this runtime will eventually
support at the parser, grounder, and engine executable boundary. The runtime
now provides the inert substrate for status/result mapping, Diagnostics/status
I/O, and CLI/TTY/provenance policy while leaving binary adoption to later
Arc04/Arc05 slices.

Later Arc04 slices still own output finalization, fixture harness support, and
any approved test-only dependency integration.

## Status/result core

Slice02 implements the first Arc03 status semantics as standard-library-only
C++17 runtime APIs:

- `StatusCode` names every Arc03 status, including `signal_terminated`.
- `StatusClass` represents the payload class vocabulary used by process
  managers.
- `Component` and `SurfaceDisposition` provide typed vocabulary for later
  parser, grounder, and engine adoption.
- `ProcessStatus` carries a status code plus component, surface disposition,
  and the dynamic signal number needed for supervisor-observed
  `signal_terminated`.
- `status_name`, `status_class`, and `exit_code` implement the stable Arc03
  exit-code mapping without requiring diagnostic prose parsing.
- `StatusResult<T>` is a local value-or-status facade for helper returns.

This is no binary adoption: parser, grounder, and engine do not link to or call
these helpers yet, and their stdout/stderr behavior, CLI shape, current exit
codes, artifacts, release assets, and wolong-facing behavior remain unchanged.

## Diagnostics/status I/O

Slice03 implements a standard-library-only Diagnostics/status I/O facade for
the Arc03 tagged status contract:

- `StatusRecord` builds one final machine status record from `ProcessStatus`
  plus a validated surface name.
- `StatusStream` and `status_stream_allowed` encode stdout/stderr ownership:
  stderr is always legal for status, while stdout is legal only when stdout is
  empty or already assigned to tagged status, not when it carries a data
  artifact or human informational output.
- `serialize_status_record` and `write_status_record` emit one single-line
  `PANDAPI_STATUS` record with required fields `status`, `component`,
  `surface`, `surface_disposition`, `exit_code`, and `class`.
- `write_status_record` flushes the selected stream before returning so final
  status is visible before process exit.
- `parse_status_record` accepts tagged status records and returns the local
  `StatusResult<StatusRecord>` facade with `input_invalid` on malformed
  machine text.
- field escaping keeps newline and tab content parseable without raw control
  characters in the emitted record, rejects ANSI escape sequences, and rejects
  placeholder prose values instead of making them part of the machine contract.
- `PartialOutputPolicy` covers `absent`, `retained`, `discarded`, `complete`,
  and `unknown` partial-output outcomes.

This is still no binary adoption: the helper can write and parse
`PANDAPI_STATUS`, but parser, grounder, and engine do not yet call it, no
`pandapi-*` wrappers are introduced, and current stdout/stderr behavior is not
changed.

## CLI/TTY/provenance Core

Slice04 implements standard-library-only helpers for the shared command and
stream policy that later per-binary work will consume.

- `cli_policy.hpp` defines common CLI policy values for human versus
  supervised invocation, `--status=stderr|stdout`, `--output` destinations,
  inherited positional output alias conflicts, and `--help`, `--version`, and
  `--provenance` informational commands. Validation reports malformed enum
  values and common option conflicts as `cli_usage_error` / exit `10` through
  `StatusResult<T>` before any input/model processing.
- `CommandIdentity` records command identity for canonical `pandapi-parser`,
  `pandapi-grounder`, and `pandapi-engine` names plus inherited compatibility
  names `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`. Compatibility
  invocation is identified for version/provenance output, but there is no
  default deprecation warning and no binary adoption in this slice.
- `tty.hpp` defines TTY/color policy with injected terminal observations, so
  tests use an observed terminal value instead of depending on the current
  shell. `--color=auto`, `--color=always`, `--color=never`, `--no-color`,
  `--no-colour`, and `NO_COLOR` are represented by `ColorControls`; disabling
  controls win and disable ANSI everywhere.
- `allows_color` permits ANSI only for human stderr on a suitable TTY. It
  rejects ANSI for stdout data artifacts, tagged status records, supervised
  output, machine output, file output, pipe output, and no-TTY output.
- `provenance.hpp` defines `ProvenanceRecord` and field-oriented
  `format_version` / `format_provenance` helpers. Callers supply
  `canonical_command`, `component`, `chengdu_version`,
  `contract_version`, `upstream_project`, `upstream_commit`, `source_prefix`,
  `build_commit`, `platform`, `compiler`, `license`, and `NOTICE` fields. The
  optional `invoked_command` field records inherited compatibility invocation
  when it differs from the canonical command. The runtime does not shell out to
  git or infer build metadata. Required version fields must be present;
  optional detailed provenance-only fields are omitted when absent, and unknown
  or placeholder values are rejected when supplied as placeholder prose so later
  golden tests see stable field names.

This is a local facade for future CLI11 adoption; CLI11 is still not imported,
vendored, fetched, discovered with `find_package`, included, or exposed by
the runtime API. Parser, grounder, and engine command behavior, stdout/stderr,
exit codes, generated artifacts, release shape, and wolong-facing behavior are
unchanged.

## Fixture and Seam-Test Substrate

Slice05 implements reusable fixture and seam-test substrate APIs for later
Arc05 executable conformance work. This is still no binary adoption.

- `fixture.hpp` defines typed fixture records for command identity, argv,
  environment, stdin, stdout/stderr role expectations, output artifact state,
  expected exit/status, wait status, final status policy, normalization rules,
  and safe timeout/resource/signal probe policy. The vocabulary keeps golden
  fields, semantic predicates, harness-owned timeouts, binary-owned timeouts,
  resource-limit probes, quarantine probes, and manual probes explicit.
- `normalization.hpp` provides comparison helpers for stable golden fields and
  semantic predicates. It normalizes path tokens such as `$FixtureRoot`,
  `$BuildRoot`, `$TempRoot`, and `$ReleaseRoot`, converts line endings to LF,
  detects or strips ANSI escapes, checks duration/timestamp/build metadata
  predicates, normalizes errno and signal values, and parses final
  `PANDAPI_STATUS` lines through the accepted Slice03 status parser rather than
  diagnostic regex matching.
- `process_fixture.hpp` provides a local Linux/macOS process observation
  substrate for harness tests. It captures stdout, stderr, exit code, wait
  status, supervisor-observed signal termination, harness timeout disposition,
  and cleanup state using local POSIX shims hidden in the implementation.
- Runtime tests exercise typed fixture construction, normalization,
  structured comparison failures, final status matching, process observation
  success/failure, and a bounded CI-safe harness timeout with a tiny local
  helper command. These tests do not invoke parser, grounder, or engine as
  managed-process conformance proof.
- Catch2 integration is optional and test-only. `CMakeLists.txt` looks for an
  already installed local Catch2 package and registers `fixture_catch2_smoke`
  only when available. It never fetches Catch2, never exposes Catch2 from
  public runtime headers, and the default runtime build continues to pass when
  Catch2 is absent.

The fixture substrate is intended for black-box process fixtures and seam tests
that Arc05 will own. It does not add `pandapi-*` executable entry points,
wrappers, symlinks, copied binaries, release packaging, or planner behavior
changes.

## Integration readiness

Arc04 integration readiness leaves the runtime ready for Arc05 planning as an
inert substrate, not as an executable contract adoption. The current accepted
surface is:

- `pandapi_runtime` / `pandapi::runtime` as the CMake target for shared runtime
  helpers;
- `scripts/build-runtime.sh` for runtime CMake build and CTest execution;
- `scripts/build-all.sh` for a build-only local probe that runs runtime,
  parser, grounder, and engine builds in that order;
- status/result, diagnostics/status I/O, CLI/TTY/provenance, fixture,
  normalization, and process observation headers under
  `include/pandapi/runtime/`;
- smoke and seam tests under `pandapi-runtime/tests/`.

Arc05 still owns parser, grounder, and engine adoption. That includes
canonical `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` entry
points, inherited `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`
compatibility, black-box contract fixtures, and the managed-process behavior
change from inherited outputs to the accepted Arc03 contract.

Arc06 still owns release shape, wolong proof, license and NOTICE updates,
test-only dependency exclusion, and CI/release gate evidence. Arc04 performs
no binary adoption.

## Arc02 Dependency Gates

Arc02 selected the standard library as the baseline and placed every external
candidate behind explicit gates:

- `fmt` may enter only behind a chengdu diagnostics/process facade.
- `CLI11` may enter only through the CLI parser wrapper after golden process
  tests cover help, version, provenance, parse errors, compatibility names,
  color controls, supervised mode, status output, and output/status conflicts.
- `Catch2` is test-only and must be paired with black-box process fixtures.
- `tl::expected` is only a pilot behind the local status/result substrate.
- `reproc++` is only a conditional pilot behind one child-process adapter if a
  supported boundary later requires it.
- `nlohmann/json` remains held because the selected status format is tagged
  text, not JSON Lines.
- Abseil and Boost.Process remain rejected as 0.3.0 foundation dependencies.

The slice02 and slice03 dependency gates keep `tl::expected`, fmt, CLI11,
Catch2, reproc++, nlohmann/json, Boost, Abseil, and GSL out of the public API
and out of direct imports. The local `StatusResult<T>` and Diagnostics/status
I/O facades can later hide approved pilots, but this runtime core remains
standard-library-only today.

## No Behavior Change

Arc04 slice01 is inert with respect to the inherited binaries. Building or
testing this runtime must not change parser, grounder, or engine command names,
arguments, stdout/stderr behavior, exit codes, generated artifacts, release
shape, or wolong-facing behavior.
