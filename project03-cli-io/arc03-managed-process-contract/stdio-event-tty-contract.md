# Arc03 Slice 03: stdio, Event, and TTY Contract

## Purpose

This report defines how 0.3.0 callers observe supported pandaPI execution through standard streams, machine-readable status records, buffering, and TTY behavior. It preserves the slice01 supported surface and routes the slice02 status taxonomy and payload fields without changing status names, numeric exit codes, command names, option names, dependency policy, tests, or release assets.

Evidence inputs:

- Arc01 vendored source audit synthesis: `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`.
- Arc01 component reports for parser, grounder, and engine stream/color/TTY findings.
- Arc02 C++ library research close: `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`.
- Arc03 slice01 `supported-surface-classification.md` and slice01 CDC verification.
- Arc03 slice02 `status-exit-signal-taxonomy.md` and slice02 CDC verification.

## Invocation And Output Modes

0.3.0 has three semantic invocation/output modes. Slice04 owns final CLI option names, aliases, help text, version text, and flag spelling. This slice defines only the behavior those options must select.

| Mode | Intended caller | Stdout | Stderr | Status observation |
|---|---|---|---|---|
| Human CLI mode | Terminal user | Selected data artifact only, or empty when output is file-backed or absent. | Human diagnostics, warnings, progress, statistics, and final human summary. | Exit code plus non-normative human prose. |
| Supervised/process-manager mode | Script, daemon, workflow runner, process manager | Selected data artifact only, or empty. | Human prose is quiet by default except warnings/fatal summaries; machine-readable tagged status may be enabled. | Exit code plus tagged status line when selected. |
| Machine-event mode | Supervisor that needs parseable observation without diagnostic regex | Event/status stream, or selected data artifact, but never both on stdout. | If stdout carries data, the tagged status side channel is stderr and human prose must be suppressed or kept unprefixed. | Exit code plus tagged status records. |

No mode may mix diagnostics, progress, statistics, or human summaries into stdout when stdout is carrying a parser, grounder, or engine data artifact.

## Stdout Ownership

Stdout is owned by a single selected artifact role for the invocation. If the selected artifact is written to a file, stdout is empty unless machine-event mode explicitly selects stdout for tagged status records.

| Supported surface | Success stdout | `domain_no_plan` stdout | Other non-success stdout |
|---|---|---|---|
| Parser normal HDDL parse | Parser-owned normal output only when stdout is selected as the parser artifact sink. No `Parsing ...`, diagnostics, progress, color, statistics, or status summary. | Not a parser status. | Empty unless a machine-event stream is explicitly assigned to stdout. |
| Grounder normal `.htn` grounding | Grounder-owned grounded `.htn` artifact only when stdout is selected as the grounder artifact sink. No option dump, H2 progress, dependency chatter, diagnostics, warnings, color, or statistics. | Not a grounder status. | Empty unless a machine-event stream is explicitly assigned to stdout. |
| Engine normal search | Plan/search result artifact only when stdout is selected as the engine artifact sink. | Empty by default: `domain_no_plan` is a classified outcome, not a plan artifact. A later contract may define an explicit no-plan artifact, but prose must not appear on stdout. | Empty unless a machine-event stream is explicitly assigned to stdout. |

The Arc01 findings that motivate this rule are parser stdout contamination (`P-006`), grounder stdout configuration/progress mixing (`G-007`), and engine stdout diagnostics/progress/statistics/status mixing (`E-004`). The 0.3.0 contract is that stdout is data, event/status, or empty; it is never a mixed human channel.

## Stderr Ownership

Stderr is the human diagnostic channel. It owns:

- diagnostics for invalid input, unavailable paths, unsupported/legacy/experimental/future surfaces, dependency failures, child process failures, fatal errors, and internal errors;
- warnings and compatibility notes;
- progress and statistics when enabled for human use;
- final human-readable status summaries.

Diagnostic prose on stderr is non-normative for supervisors. Supervisors must use exit codes and the tagged status side channel when they need machine-readable classification. Wording, punctuation, line wrapping, and ordering of untagged diagnostic prose may change without changing the process contract.

Fatal/internal-error summaries on stderr should include enough human context to triage the failure, but they must not replace the slice02 payload fields in machine-readable status records.

## Machine-Readable Status Decision

0.3.0 supports machine-readable status output as a status-only tagged text side channel. It does not adopt JSON Lines for this release. The format class is line-delimited tagged text with tab-separated `key=value` fields:

```text
PANDAPI_STATUS	status=ok	component=engine	surface=normal_search	surface_disposition=supported	exit_code=0	class=success	duration_ms=37
```

The tag name and field names are contract concepts here; slice04 owns the final user-facing option spelling that enables the stream.

Because the selected format is tagged text rather than JSON, `nlohmann/json` remains held for 0.3.0. It may re-enter only if a later accepted event-format decision selects JSON Lines or another JSON machine-event/status contract.

### Required Status Fields

Every tagged final status record must include these slice02 payload fields:

- `status`
- `component`
- `surface`
- `surface_disposition`
- `exit_code`
- `class`

When known, the record also carries scalar slice02 fields relevant to the outcome:

- normal artifact fields: `artifact_role`, `output_path`, `duration_ms`;
- file/path errors: `path`, `path_role`, `operation`, `errno_name`;
- validation errors: `location`, `rule`, `token`, `model_section`;
- plan outcomes: `outcome_kind`, `proof_kind`;
- timeout/resource outcomes: `timeout_ms`, `elapsed_ms`, `limit_owner`, `resource_kind`, `limit_value`, `observed_value`;
- termination outcomes: `signal_name`, `signal_number`, `cleanup_completed`;
- dependency/child outcomes: `dependency_name`, `dependency_role`, `dependency_status`, `boundary`, `child_role`, `child_exit_code`, `child_signal`, `child_status`;
- partial data outcomes: `partial_output_policy`;
- audit traceability: `finding_refs`.

Values must be single-line, escaped where necessary, and parseable without inspecting untagged diagnostic prose. Unknown fields are omitted, not emitted with placeholder prose.

### Ordering And Flush

The final `PANDAPI_STATUS` line is emitted after output artifact disposition is known and before process exit. It must be flushed before returning from `main` or before an owned graceful `interrupted` exit. No status records may be emitted after the final status line.

0.3.0 does not require a full event telemetry stream. If later slices add lifecycle or progress events, they must use the same tagged text class or explicitly reopen the event-format decision. Any optional event records must be ordered before the final status record and flushed on each line.

## Buffering, Flushing, And Partial Output

Stdout data artifacts may use normal file or pipe buffering, but supported binaries must flush and close the selected artifact sink before emitting the final status record and exiting. Stderr diagnostics and status records are line-oriented; progress/status updates should flush at line boundaries when visible to a TTY or supervisor.

Files should be written through an implementation-owned output policy that can classify final artifact state as complete, absent, retained-partial, discarded-partial, or unknown. Where feasible, file artifacts should use a temporary path plus commit/rename pattern so non-success statuses do not leave ambiguous outputs. This is an implementation rule for Arc04/Arc05; this slice only requires the status payload to report `partial_output_policy`.

| Outcome | Output disposition |
|---|---|
| `ok` | Selected artifact is complete, flushed, and closed before final status. |
| `domain_no_plan` | Engine plan artifact is absent by default; final status records the classified no-plan outcome. |
| `cli_usage_error`, `input_unavailable`, `output_unavailable`, `input_invalid` | Stdout data is empty; file artifacts are absent or explicitly reported as partial/discarded. |
| `unsupported_feature`, `legacy_surface`, `experimental_surface`, `future_surface` | Supported artifact contract is not entered; stdout data is empty unless machine-event mode owns stdout. |
| `timeout`, `resource_limit` | The process should attempt best-effort flush of diagnostics/status and report whether partial output was retained or discarded. |
| `interrupted` | Binary-owned graceful interruption attempts cleanup, flushes final tagged status, and reports `cleanup_completed`. |
| `dependency_failure`, `child_process_failure` | Child/dependency output must not be forwarded to stdout unless it is the selected data artifact for a supported boundary. Status fields describe the failing boundary. |
| `internal_error` | Stdout data is empty unless a complete artifact was already committed; final status reports partial-output policy and internal classification. |

For TTYs, stdout remains artifact-owned; human progress belongs on stderr. For pipes and files, progress/statistics are disabled unless explicitly selected for human stderr or machine status/event observation.

## ANSI, Color, And TTY Policy

ANSI escape sequences are permitted only in human stderr output when color is enabled and stderr is a suitable TTY. Color is never permitted in:

- stdout data artifacts;
- tagged status/event records;
- supervised/process-manager machine records;
- no-TTY output;
- files or pipes used for machine parsing.

The default policy is auto color for human TTY stderr only. A no-color/no-colour control and the `NO_COLOR` environment convention must disable ANSI output everywhere. This contract closes the parser `--no-colour` gap from `P-007`, grounder debug/output-domain color issue from `G-009`, and engine unconditional color issue from `E-005`.

Interactive engine behavior remains legacy. If a legacy interactive surface is reached without a TTY, the process must not silently enter an interactive prompt; it should classify the request as `legacy_surface` or another slice02 status chosen by the surrounding invocation contract.

## Quiet, Verbosity, Progress, And Statistics

Quiet and verbosity affect human stderr only. They do not change exit codes, final tagged status fields, or stdout artifact ownership.

- Human default: concise diagnostics and final summaries on stderr; progress only when useful and preferably only when stderr is a TTY; statistics only when requested or when they are already part of an accepted human mode.
- Quiet: suppresses human progress, option dumps, banners, and statistics. It must not suppress fatal diagnostics, final tagged status records, or required exit classification.
- Verbose/debug: may add human diagnostics and statistics to stderr, never to stdout data artifacts or tagged status records unless those fields are explicitly part of the machine status schema.
- Supervised/process-manager default: quiet human prose, no progress spinner, no unstructured statistics. Supervisors use exit status and tagged status records.

Grounder H2 quiet-mode stream mutation from `G-008` must be contained. H2 is experimental, and its implementation must not globally poison `std::cout` for the parent process. If H2 remains reachable, output redirection must be scoped, restored, and kept out of supported stdout. If that cannot be guaranteed, H2 invocation is fenced as `experimental_surface` or `dependency_failure` rather than allowed to mutate stream state.

## Non-Supported Surface Fencing

Slice01 dispositions remain authoritative. Stream/event/TTY behavior for non-supported surfaces is fenced before the supported artifact contract is entered.

| Surface | Disposition | Stream/event/TTY fence |
|---|---|---|
| Parser verifier paths | legacy | No supported stdout artifact guarantee. Reject or classify as `legacy_surface`; diagnostics on stderr; optional tagged status record only. |
| Parser output helper paths | legacy | Same legacy fence; do not let helper chatter define stdout semantics for supported parser parse. |
| Grounder H2 | experimental | No stdout progress/config leakage into supported mode; scoped stream mutation only; otherwise classify as `experimental_surface` or dependency-related failure. |
| Grounder `cpddl`/FAM | legacy | No 0.3.0 supported event/TTY contract; child/dependency output is not normalized except through a final tagged status record. |
| Engine interactive | legacy | Requires explicit legacy handling; no silent prompt in no-TTY supervised mode. |
| Engine translation | experimental | Child process and temporary-file chatter are not part of the supported stdout contract; classify/fence unless a later slice accepts it. |
| Engine SAT | unsupported | Reject as `unsupported_feature`; stderr diagnostic plus optional tagged status. |
| Engine BDD/CUDD | future | Reject as `future_surface`; stderr diagnostic plus optional tagged status. |

## Arc02 Dependency Gates

This contract preserves the Arc02 dependency decisions:

- `fmt` may be used only behind a diagnostics/process I/O facade or equivalent shared substrate. Direct planner-internal stdout/stderr formatting remains disallowed.
- `nlohmann/json` follows the event-format decision and is held because 0.3.0 selects tagged text status-only output, not JSON Lines.
- `CLI11` remains slice04-owned for option mapping and help behavior.
- `Catch2` and process fixture work remain slice05/Arc04-owned for contract tests.
- `reproc++` remains conditional and may be justified only by an accepted supported child-process boundary, not by experimental H2 or translation behavior alone.
- Abseil and Boost.Process remain rejected as 0.3.0 foundations.

## Later Consumers

This report is an input to:

- Arc03 slice04 for CLI option mapping, option names, aliases, help, version, no-color/no-colour control, and machine-status enablement spelling.
- Arc03 slice05 for the final contract test matrix and process fixture expectations.
- Arc04 for the shared diagnostics/process I/O substrate, buffering, flushing, color, TTY detection, and status line escaping.
- Arc05 for per-binary migration of parser, grounder, and engine behavior.
- Arc06 for release docs, license/NOTICE review of any dependency actually adopted, and behavior-change table entries documenting stdout/stderr/status compatibility changes.

## Final Recommendation

Adopt the 0.3.0 managed-process contract as: stdout is a single artifact or event sink, stderr is human diagnostics, and machine observation uses exit codes plus a tagged text final status side channel. Keep JSON out of the release unless a later accepted event-format decision requires it. Fence all legacy, experimental, unsupported, and future surfaces before they can contaminate the supported parser, grounder, or engine stream contract.
