# Contract Test Matrix

Date: 2026-08-09

Arc: `arc03-managed-process-contract`

## Summary

This report converts the accepted Arc03 managed-process design into a
testable matrix for Arc04, Arc05, Arc06, and slice06. It is a design artifact:
it defines fixture obligations, expected observations, comparison policy, and
ownership boundaries. It does not add fixture data, implement a harness, adopt
dependencies, or modify planner source.

Evidence inputs:

- Arc01 vendored-source audit close:
  `arc01-vendored-source-audit/closing-report.md` and
  `arc01-vendored-source-audit/audit-synthesis-pandapi.md`;
- Arc02 C++ library research close:
  `arc02-cpp-library-research/closing-report.md` and
  `arc02-cpp-library-research/combined-library-recommendations.md`;
- Arc03 slice01:
  `supported-surface-classification.md` and
  `slice01-supported-surface-classification/cdc-verification.md`;
- Arc03 slice02:
  `status-exit-signal-taxonomy.md` and
  `slice02-status-exit-signal-taxonomy/cdc-verification.md`;
- Arc03 slice03:
  `stdio-event-tty-contract.md` and
  `slice03-stdio-event-tty-contract/cdc-verification.md`;
- Arc03 slice04:
  `cli-naming-version-migration.md` and
  `slice04-cli-naming-version-migration/cdc-verification.md`.

The matrix preserves the accepted 0.3.0 boundary: supported parser normal HDDL
parse, supported grounder normal `.htn` grounding, and supported engine normal
search. Parser verifier/output helpers, grounder `cpddl`/FAM, and engine
interactive remain legacy surfaces. Grounder H2 and engine translation remain
experimental surfaces. Engine SAT remains unsupported. Engine BDD/CUDD remains
future surface. No fixture may expand these surfaces into supported 0.3.0
behavior.

## Fixture Schema

Each black-box process fixture or acceptance probe is a structured record. The
canonical schema is:

```yaml
id: arc03.process.<component>.<case>
owner: Arc04 | Arc05 | Arc06
kind: black-box-process | catch2-seam | release-verification
component: parser | grounder | engine
surface: supported | legacy | experimental | unsupported | future
surface_disposition: supported | legacy_surface | experimental_surface | unsupported_feature | future_surface
command: pandapi-parser | pandapi-grounder | pandapi-engine | pandaPIparser | pandaPIgrounder | pandaPIengine
argv:
  - command-name-as-invoked
  - option-or-positional
environment:
  PANDAPI_TEST_ROOT: normalized-fixture-root
  NO_COLOR: unset | set
stdin:
  mode: empty | pipe-artifact | invalid-bytes
  bytes: fixture-reference-or-empty
tty:
  stdin: tty | no-TTY
  stdout: tty | pipe | file | no-TTY
  stderr: tty | pipe | file | no-TTY
color:
  option: auto | always | never | no-color | no-colour | unset
  expected_ansi: stderr-human-only | none
timeout_policy:
  mode: none | harness-timeout | binary-timeout
  bound_ms: ci-safe-bound
resource_policy:
  mode: none | harness-limit | binary-limit
  bound: ci-safe-bound
working_directory: fixture-workdir
input_artifacts:
  - path: normalized/path
    role: domain | problem | htn | engine-input | include
    availability: present | missing | unreadable | malformed
output_artifacts:
  - path: normalized/path | "-"
    role: htn | sas | plan | status | none
    expected_state: absent | complete | retained-partial | discarded-partial
stdout:
  role: empty | artifact | informational | tagged-status
  comparison: golden | semantic | empty
stderr:
  role: empty | human-diagnostics | progress | warning
  comparison: semantic | empty
exit:
  status_name: ok | domain_no_plan | cli_usage_error | input_unavailable | output_unavailable | input_invalid | unsupported_feature | legacy_surface | experimental_surface | future_surface | timeout | resource_limit | interrupted | dependency_failure | child_process_failure | internal_error | signal_terminated
  exit_code: 0 | 2 | 10 | 20 | 21 | 22 | 30 | 31 | 32 | 33 | 40 | 41 | 42 | 50 | 51 | 60 | 128 + signal_number
wait_status:
  observed: exited | signaled
  signal: none | SIGINT | SIGTERM | SIGKILL | platform-normalized-signal
final_status:
  observation: absent | required | forbidden
  stream: stderr | stdout
  tag: PANDAPI_STATUS
  required_fields:
    - status
    - component
    - surface
    - surface_disposition
    - exit_code
    - class
  optional_fields:
    - artifact
    - duration_ms
    - path
    - path_role
    - operation
    - errno
    - location
    - rule
    - token
    - model_section
    - outcome
    - proof
    - timeout_ms
    - resource
    - signal
    - dependency
    - child
    - partial_output_policy
    - finding_refs
normalization:
  paths: normalize-to-fixture-root
  durations: predicate-only
  build_metadata: field-presence-or-pattern
  timestamps: wildcard-or-rfc3339-predicate
  errno: symbolic-name-preferred
  signals: platform-normalized
  line_endings: lf
  ansi: forbidden-unless-stderr-human-tty-color
safety:
  ci: safe | quarantine | manual
  cleanup: remove-temp-artifacts | preserve-for-debug
```

The fixture schema deliberately observes command, argv, environment, stdin,
stdout, stderr, artifact paths, exit code, signal and wait status,
timeout/resource policy, TTY/color mode, and final `PANDAPI_STATUS` behavior in
one place. Arc04 owns the reusable harness representation. Arc05 owns
per-binary conformance fixtures. Arc06 owns release-package and wolong
verification fixtures.

## Golden-Output Policy

Golden-output tests freeze stable contract fields, not incidental prose.

Stable fields eligible for golden comparison:

- command name and argv shape;
- exit code and status name;
- `PANDAPI_STATUS` tag spelling, tab delimiter, field names, required fields,
  status vocabulary, numeric exit code, component, surface, and
  surface_disposition;
- stdout ownership role: empty, artifact, informational, or tagged-status;
- stderr ownership role: human diagnostics, warning, progress, or empty;
- help/usage skeleton headings and option names once slice06 freezes the final
  managed-process contract;
- `--version` and `--provenance` field names;
- absence of default inherited-name deprecation warning;
- absence of ANSI escape sequences in stdout artifacts, tagged status records,
  supervised output, machine output, files, pipes, and no-TTY output.

Semantic predicates, not byte-for-byte golden text:

- non-normative human diagnostic prose;
- paths, after normalization to fixture root;
- durations and timeout intervals;
- compiler names, compiler versions, platform names, and build metadata;
- timestamps and build timestamp fields;
- errno text, with symbolic errno names preferred and numeric errno accepted
  only after normalization;
- signal names and wait status wording;
- upstream planner progress/statistics text on stderr;
- model-specific plan text when the algorithm is not required to be stable.

Normalization rules:

- Paths are normalized to `$FIXTURE_ROOT`, `$BUILD_ROOT`, `$TMPDIR`, or
  `$RELEASE_ROOT` tokens before comparison.
- Durations must satisfy predicates such as present, numeric, non-negative, and
  under the fixture's CI-safe bound.
- Build metadata is compared by field presence and conservative patterns, not
  exact host-specific text.
- Timestamps are either wildcarded or checked by RFC3339-like predicate.
- Errno values normalize to symbolic names where available; platform-specific
  numeric values may be accepted only through a fixture-local map.
- Signals normalize to `SIGINT`, `SIGTERM`, `SIGKILL`, or
  `platform-normalized-signal`.
- Line endings normalize to LF.
- ANSI escape detection is explicit and fails any stdout artifact, tagged
  status, supervised stream, machine stream, file, pipe, or no-TTY output.

Tests must not rely on regex archaeology over diagnostic prose. A diagnostic
regex is allowed only to assert the presence of a stable error class token or a
stable option name. Process managers classify through exit code and tagged
status fields, not through human text.

## Command Names

Every supported binary has canonical and inherited-name probes.

| Probe | Commands | Expected observation |
|-------|----------|----------------------|
| Canonical invocation | `pandapi-parser`, `pandapi-grounder`, `pandapi-engine` | argv[0] is accepted; status component matches parser, grounder, or engine; supported normal paths are available. |
| Inherited invocation | `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine` | argv[0] is accepted as 0.3.0 compatibility; no default deprecation warning is emitted; `--version`/`--provenance` record canonical command and invoked compatibility name. |
| Compatibility output positionals | inherited parser and grounder positional output forms | accepted as compatibility aliases where slice04 allowed them; conflicts with `--output` are `cli_usage_error`. |

The compatibility decision is observable, but it is not a deprecation campaign.
No default warning is required merely because the inherited `pandaPI*` name was
used. Any future deprecation warning is a behavior-change/release obligation
for Arc06 and must not appear silently.

## Informational Commands

For each canonical and inherited command, the matrix requires probes for
`--help`, `--version`, and `--provenance`.

| Probe | Expected stdout | Expected stderr | Exit/status | Processing rule |
|-------|-----------------|-----------------|-------------|-----------------|
| `--help` | informational help text owns stdout | empty or human diagnostic-free | `ok` / exit 0 | no input/model processing |
| `--version` | version fields own stdout | empty | `ok` / exit 0 | no input/model processing |
| `--provenance` | stable provenance fields own stdout | empty | `ok` / exit 0 | no input/model processing |

Required stable provenance fields are: canonical command, invoked compatibility
name when applicable, component, chengdu release version, managed-process
contract version `0.3.0`, upstream pandaPI project, upstream commit, source
prefix, chengdu build commit, platform, compiler, license pointer, and NOTICE
pointer. `--provenance` additionally exposes import commit, build timestamp if
available, dependency license/provenance fields, and THIRD-PARTY-LICENSES
pointer.

Status-stream interactions:

- `--status` and `--status=stderr` may add one final `PANDAPI_STATUS` on
  stderr after the informational stdout is complete.
- `--status=stdout` conflicts with help, version, and provenance because stdout
  already carries informational output. The expected result is
  `cli_usage_error` / exit 10 and no input/model processing.

## CLI Usage Errors

The required usage-error probes apply to parser, grounder, and engine, using
both canonical and inherited entry points where feasible.

| Probe | Example shape | Expected result |
|-------|---------------|-----------------|
| Missing required arguments | `pandapi-parser DOMAIN.hddl` or no positional input | `cli_usage_error`, exit 10, no input/model processing, no artifact |
| Unknown option | `--unknown-option` | `cli_usage_error`, exit 10, no input/model processing |
| Malformed option value | `--color=wat` or malformed enum | `cli_usage_error`, exit 10 |
| Too many positionals | extra non-option operands beyond accepted compatibility forms | `cli_usage_error`, exit 10 |
| `--output` conflict | both `--output OUT` and positional output alias | `cli_usage_error`, exit 10, no artifact |
| `--status=stdout` conflict | stdout is artifact sink, help/version/provenance output, or existing engine default artifact output | `cli_usage_error`, exit 10 |

CLI parse-error fixtures must prove that no domain, problem, `.htn`, or engine
input file is opened for semantic processing after the parse failure. A
sentinel unreadable or malformed input may be used to prove the parser stopped
at CLI classification, but the assertion remains on absence of input/model
processing and absence of output artifact.

## Supported Positive Matrix

Parser positive probes:

| ID | Invocation | Expected observation |
|----|------------|----------------------|
| parser positive file-backed | `pandapi-parser [COMMON] --output OUT.htn DOMAIN.hddl PROBLEM.hddl` | valid HDDL input produces complete `.htn` artifact; stdout empty; stderr owns human diagnostics only; final `PANDAPI_STATUS` if enabled is `ok` / 0. |
| parser positive stdout-backed | `pandapi-parser [COMMON] --output - DOMAIN.hddl PROBLEM.hddl` | stdout owns `.htn` artifact; stderr may carry human diagnostics and final status; no status on stdout. |
| parser inherited compatibility | `pandaPIparser DOMAIN.hddl PROBLEM.hddl OUT.htn` | compatibility positional output remains accepted; no default deprecation warning. |

Grounder positive probes:

| ID | Invocation | Expected observation |
|----|------------|----------------------|
| grounder positive file-backed | `pandapi-grounder [COMMON] --output OUT.sas INPUT.htn` | valid `.htn` produces complete grounder artifact; stdout empty; status `ok` / 0 when enabled. |
| grounder positive stdout-backed | `pandapi-grounder [COMMON] --output - INPUT.htn` | stdout owns grounder artifact; stderr owns diagnostics/status; no ANSI or diagnostic text in stdout. |
| grounder inherited compatibility | `pandaPIgrounder INPUT.htn OUT.sas` | compatibility positional output remains accepted; no default deprecation warning. |

Engine positive probes:

| ID | Invocation | Expected observation |
|----|------------|----------------------|
| engine positive file-backed | `pandapi-engine [COMMON] --output PLAN INPUT.sas` | solved supported search produces complete plan artifact; stdout empty; status `ok` / 0 when enabled. |
| engine positive stdout-backed | `pandapi-engine [COMMON] INPUT.sas` with stdout pipe or file redirection | stdout owns plan artifact by existing shell-redirection behavior; stderr owns diagnostics/status. |
| engine inherited compatibility | `pandaPIengine INPUT.sas` | compatibility name remains accepted; no default deprecation warning. |

Pipeline composition probes:

- parser-to-grounder composition: parser stdout or file artifact is accepted by
  grounder as valid `.htn`.
- grounder-to-engine composition: grounder stdout or file artifact is accepted
  by engine as valid engine input.
- parser-to-grounder-to-engine composition: all three supported positive paths
  can compose with status records enabled on stderr, while stdout carries only
  the selected artifact at each stage.

Pipeline tests are semantic acceptance probes, not proof that every planner
algorithm output is byte-stable.

## Input And Output Failures

All three supported components require negative fixtures for unavailable
inputs, unavailable outputs, and invalid inputs.

Input unavailable probes:

- missing parser domain, missing parser problem, and missing include file;
- unreadable parser domain/problem/include where the platform can create that
  condition safely;
- missing or unreadable grounder `.htn`;
- missing or unreadable engine input.

Expected status: `input_unavailable` / exit 20. Required final status fields
include path, path_role, operation, and errno when available.

Output unavailable probes:

- output parent directory does not exist;
- output path is a directory;
- output path is not writable where the platform can create that condition
  safely;
- write, flush, and finalize failures where Arc04 can supply a deterministic
  harness device or wrapper;
- temp-and-rename failure once Arc04/Arc05 implement file finalization.

Expected status: `output_unavailable` / exit 21. Fixtures must assert the
partial_output_policy: absent, retained-partial, or discarded-partial.

Input invalid probes:

- malformed HDDL domain/problem for parser;
- malformed `.htn` for grounder;
- malformed engine input for engine;
- model section or token errors that are syntactically present but invalid for
  the target component.

Expected status: `input_invalid` / exit 22. These fixtures are distinct from
missing input, unavailable output, timeout, resource_limit, dependency_failure,
and internal_error.

## Engine Outcome Matrix

The engine requires a solved fixture and a no-plan fixture.

| Probe | Expected status | Distinction |
|-------|-----------------|-------------|
| solved supported search | `ok` / 0 | complete plan artifact or stdout plan artifact; status outcome indicates solved. |
| unsolvable supported search | `domain_no_plan` / 2 | valid input and completed search prove no plan or unreachable goal; no malformed input classification. |
| malformed engine input | `input_invalid` / 22 | parser/reader rejects the model before search. |
| exceeded timeout | `timeout` / 40 | bounded by binary or harness timeout policy. |
| exceeded resource limit | `resource_limit` / 41 | bounded by memory, node, or other resource policy. |
| missing dependency | `dependency_failure` / 50 | required optional external dependency absent or failed to initialize. |
| invariant violation | `internal_error` / 60 | no accepted user or model classification applies. |

`domain_no_plan` is a successful classification of a valid search result, not
a synonym for malformed input, timeout, resource exhaustion, dependency
failure, child-process failure, or internal error.

## Timeout, Resource, Interrupt, And Signal Probes

Timeout and resource fixtures must be CI-safe. They may be Arc04 harness
obligations until deterministic limits exist in the binaries.

| Probe | Owner | Expected observation |
|-------|-------|----------------------|
| binary-owned timeout | Arc05 when timeout option exists | final `PANDAPI_STATUS` required, status `timeout`, exit 40, cleanup complete, partial_output_policy recorded. |
| harness-owned timeout | Arc04 harness | supervisor observes process kill; final status optional only if binary had a chance to emit it; cleanup and artifact policy asserted by harness. |
| binary-owned resource limit | Arc05 when resource option exists | final status `resource_limit`, exit 41, resource field present, partial_output_policy recorded. |
| interrupt | Arc05/Arc04 | handled SIGINT/SIGTERM maps to `interrupted`, exit 42, final status required where binary-owned. |
| unhandled signal termination | Arc04 supervisor | wait status is signaled; process-manager-visible exit code is `128 + signal_number`; final status is not required if termination prevents flush. |

All timeout/resource/interrupt/signal_terminated fixtures must specify cleanup,
safe CI bounds, artifact retention policy, and whether the observed exit is
binary-owned or supervisor-visible. `SIGKILL` should be quarantine/manual
unless the harness can make it deterministic without flaking CI.

## Stream, Status, And Artifact Assertions

stdout ownership is a hard invariant. For each fixture, stdout must be exactly
one of:

- data artifact stream;
- tagged status stream, only when `--status=stdout` is legal;
- informational help/version/provenance output;
- empty.

stderr owns human diagnostics, warnings, progress, statistics, summaries, and
status when `--status` or `--status=stderr` is enabled. Human diagnostics are
non-normative unless a fixture explicitly identifies stable contract text.

Final status rules:

- the final tagged status line starts with `PANDAPI_STATUS`;
- the line is emitted after artifact disposition is known and before exit;
- stdout artifacts are flushed and closed before final status is emitted on
  stderr;
- file artifacts record complete, absent, retained-partial, or
  discarded-partial state;
- status fields are parsed as tagged key/value text, not JSON Lines;
- `nlohmann/json` remains held because the accepted contract is tagged text
  status-only.

## Color And TTY Matrix

Color probes assert human presentation without leaking ANSI escapes into
machine streams.

| Probe | Expected observation |
|-------|----------------------|
| `--color=auto` with human stderr TTY | ANSI may appear only in human stderr diagnostics. |
| `--color=auto` without stderr TTY | no ANSI escape sequences. |
| `--color=always` | ANSI may appear only in human stderr diagnostics; stdout artifacts, tagged status, supervised output, machine output, files, pipes, and no-TTY streams stay ANSI-free. |
| `--color=never` | no ANSI escape sequences. |
| `--no-color` | canonical disable spelling; no ANSI escape sequences. |
| `--no-colour` | compatibility disable spelling; no ANSI escape sequences. |
| `NO_COLOR` set | no ANSI escape sequences unless an accepted future contract explicitly overrides it. |
| `--supervised` | quiet process-manager behavior; no ANSI in stdout, tagged status, or supervised stderr. |

TTY/color probes must not require interactive legacy engine behavior. They are
process fixtures over supported commands and visible option handling.

## Visible Non-Supported Surfaces

The matrix requires probes for visible legacy, experimental, unsupported, and
future surfaces without expanding them into supported 0.3.0 surfaces.

| Surface | Disposition | Expected probe behavior |
|---------|-------------|-------------------------|
| parser verifier | `legacy_surface` / 31 | visible path is fenced; not a supported positive parser fixture. |
| parser output helper | `legacy_surface` / 31 | visible path is fenced; no expansion to supported surface. |
| grounder H2 | `experimental_surface` / 32 | visible experimental path is identified as experimental; not required for positive support. |
| grounder `cpddl`/FAM | `legacy_surface` / 31 | legacy path is fenced; no 0.3.0 support claim. |
| engine interactive | `legacy_surface` / 31 | visible interactive path is fenced; no TTY/color probe depends on interactive behavior. |
| engine translation | `experimental_surface` / 32 | visible translation path is fenced; child-process failures remain distinct when invoked by future adapters. |
| engine SAT | `unsupported_feature` / 30 | unsupported path reports unsupported_feature, not internal_error. |
| engine BDD/CUDD | `future_surface` / 33 | future path reports future_surface when visible; hidden future options must not appear in default help. |

Hidden options, if absent from the CLI, are asserted as absent from default
help and absent from supported positive fixtures. Visible options must produce
the accepted status disposition and may include a non-normative diagnostic on
stderr.

## Black-Box Versus Catch2

Black-box process fixtures own:

- executable discovery and command-name compatibility;
- argv, environment, stdin, stdout, stderr, TTY, and color observation;
- exit code, wait status, signal, timeout, and resource observation;
- file artifacts, stdout artifacts, pipes, flush/finalize, and cleanup;
- final `PANDAPI_STATUS` placement and parseability;
- release-package executable behavior under Arc06.

Catch2 seam tests own:

- status enum to exit-code mapping;
- tagged status writer/parser escaping and field-order policy;
- diagnostics routing helper behavior;
- color policy helper behavior;
- CLI parse facade behavior, including any CLI11 wrapper;
- `tl::expected` or status/result substrate behavior behind a local facade;
- output finalization helpers and partial_output_policy mapping;
- provenance field assembly;
- signal-to-status mapper code where the binary handles a signal.

Catch2 remains test-only. A Catch2 seam test cannot replace black-box process
fixtures for command execution, stdout/stderr ownership, or release-package
behavior. Subprocess tests do not require reproc++; reproc++ remains
conditional on a supported child-process boundary that actually needs it.

## Dependency Gates

Arc02 dependency decisions are preserved:

- CLI11 remains conditional on golden-output/process tests and must not be
  adopted before Arc04/Arc05 can prove the CLI contract.
- fmt remains behind a diagnostics/process facade so formatting does not leak
  into machine output or status output.
- `nlohmann/json` remains held because the contract uses tagged text status,
  not JSON Lines.
- Catch2 is test-only and paired with black-box process fixtures.
- `tl::expected` remains behind the status/result substrate and cannot define
  the external process contract directly.
- reproc++ remains conditional and is not required for subprocess tests unless
  a future supported child-process boundary needs it.
- Abseil and Boost.Process remain rejected for 0.3.0.

No dependency is adopted by this slice.

## Routing To Later Work

slice06 consumes this report to synthesize
`managed-process-contract.md`. It must route every fixture obligation, probe
obligation, deferred optional surface, compatibility decision, dependency gate,
and behavior-change/release obligation without silent drop.

Arc04 consumes:

- fixture schema implementation;
- black-box process harness;
- status parser and matcher;
- normalization helpers;
- Catch2 seam-test substrate;
- safe CI timeout/resource/signal harness behavior.

Arc05 consumes:

- parser, grounder, and engine per-binary adoption gates;
- canonical and inherited command behavior;
- supported positive and negative fixtures;
- final status emission and stream ownership;
- color/TTY/no-color/no-colour/NO_COLOR behavior;
- non-supported surface fencing in executable behavior.

Arc06 consumes:

- release package executable probes;
- wolong fetch/verify/extract/run compatibility;
- release-manifest, SHA256SUMS, license, NOTICE, and THIRD-PARTY-LICENSES
  verification;
- behavior-change documentation for any future deprecation, alias removal, or
  diagnostic wording that becomes normative;
- CI publication evidence.

Deferred optional surfaces remain deferred until a future arc explicitly opens
them. Compatibility decisions, including inherited `pandaPI*` names and
positional output aliases, stay active for 0.3.0 unless a later accepted
contract changes them with release notice.
