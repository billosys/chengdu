# Arc03 Slice02 - Status, Exit, and Signal Taxonomy

## 1. Executive Summary

This report defines the common process-status vocabulary, numeric exit-code
mapping, timeout/resource/signal semantics, and dependency/child-process
failure semantics for the 0.3.0 managed-process contract.

Evidence inputs:

- Arc01 `audit-synthesis-pandapi.md` and accepted parser, grounder, and engine
  audit reports;
- Arc02 `combined-library-recommendations.md`;
- slice01 `supported-surface-classification.md` and
  `slice01-supported-surface-classification/cdc-verification.md`.

The taxonomy applies to the three supported 0.3.0 surfaces:

- parser normal HDDL parse;
- grounder normal `.htn` grounding;
- engine normal search.

It also gives deterministic treatment to visible non-supported surfaces:
parser verifier/output helper paths, grounder H2, grounder `cpddl`/FAM,
engine interactive mode, engine translation mode, engine SAT, and engine
BDD/CUDD.

The design principle is simple: a supervisor must be able to classify the
process outcome from a documented status name and exit code, never from
diagnostic prose. This slice names payload fields and status classes, but it
does not choose stdout/stderr routing, event serialization, JSON Lines, color,
TTY behavior, CLI option spelling, or test harness implementation.

## 2. Evidence Base

### Arc01

Arc01 identifies the recurring status defects:

- Parser `P-004` and `P-005`: deep parser/verifier exits and CLI validation
  that reports an error but continues.
- Grounder `G-004`, `G-005`, and `G-006`: deep `exit(-1)`/`_exit(0)`, missing
  positional arguments treated as stdin/stdout execution, and output streams
  opened after parse failure.
- Engine `E-002`, `E-003`, `E-008`, and `E-009`: deep exits, collapsed exit
  taxonomy, timeout/signal gaps, and optional SAT signal/thread/direct-exit
  behavior.
- Parser `P-001` through `P-003`, grounder `G-002`/`G-003`, and engine
  `E-001`: malformed input and release-disabled asserts need explicit input
  or validation statuses.
- Grounder `G-019` and engine `E-008`: timeout/resource outcomes need stable
  status semantics.
- Grounder `G-008`/`G-013` and engine `E-015`: H2/cpddl and translation
  dependency or child-process failures must not collapse into generic errors.

Arc01's synthesis recommends a shared process facade and status enum owned by
Arc03/Arc04.

### Arc02

Arc02 makes dependency gating load-bearing:

- `tl::expected` may carry status values later, but this taxonomy defines the
  semantics first.
- reproc++ is conditional and cannot become a default requirement because
  slice01 classified H2 and translation as experimental, not supported.
- nlohmann/json remains held until slice03 chooses an event format.
- CLI11 waits for slice04 CLI naming, parse-error, help/version, and migration
  decisions.
- fmt belongs behind a diagnostics/process facade after slice03 defines stream
  ownership.
- Catch2 is test-only and process fixtures remain separate until slice05/Arc04.
- Abseil and Boost.Process remain rejected as 0.3.0 foundation choices; GSL,
  performance containers, google/benchmark, parser-generator replacement,
  SAT/BDD/CUDD enablement, H2/cpddl dependency-internal work, and broad
  optional dependency audits remain held unless their re-entry criteria are
  met.

### slice01

slice01 accepted this surface boundary:

| Surface | Disposition |
|---------|-------------|
| parser normal HDDL parse | supported |
| parser verifier/output helper paths | legacy |
| grounder normal `.htn` grounding | supported |
| grounder H2 | experimental |
| grounder `cpddl`/FAM | legacy |
| engine normal search | supported |
| engine interactive mode | legacy |
| engine translation mode | experimental |
| engine SAT | unsupported |
| engine BDD/CUDD | future |

This report does not expand that support set.

## 3. Status Vocabulary

Every process outcome has:

- `status`: the stable status name below;
- `exit_code`: the numeric process exit code;
- `class`: one of success, expected domain outcome, caller error,
  input/model error, policy failure, external/dependency failure, or internal
  defect;
- `component`: parser, grounder, or engine;
- `surface`: the invoked product surface;
- `surface_disposition`: supported, legacy, experimental, unsupported, or
  future;
- a human-facing summary requirement;
- machine-facing payload fields, without committing to JSON or any other
  serialization.

### 3.1 Cross-Binary Status Table

| Status | Meaning | Applicable binaries/surfaces | Exit code | Class | Human-facing summary requirement | Machine-facing payload fields | Later consumers |
|--------|---------|-------------------------------|-----------|-------|----------------------------------|-------------------------------|-----------------|
| `ok` | The requested operation completed and produced the expected artifact or output. | parser normal HDDL parse; grounder normal `.htn`; engine normal search when a plan is found; human help/version if slice04 keeps them as successful informational commands. | `0` | success | Say the operation completed; do not require callers to parse this prose. | `status`, `component`, `surface`, `surface_disposition`, `artifact_role`, `output_path`, `duration_ms`. | slice03 output ownership; slice05 positive probes; Arc04 status/result substrate; Arc05 adoption. |
| `domain_no_plan` | The model was processed successfully and the planner/grounder can prove no usable plan or reachable goal exists. | engine normal search unsolvable; grounder normal `.htn` unreachable goal if reachable in default path. | `2` | expected domain outcome | Say the instance is unsolvable or unreachable using component vocabulary. | `status`, `component`, `surface`, `outcome_kind`, `proof_kind`, `duration_ms`, `bounds`. | slice03 status/event payloads; slice05 unsolvable/unreachable probes; Arc05 engine/grounder adoption. |
| `cli_usage_error` | The caller supplied invalid command-line shape, missing required arguments, mutually exclusive options, too many inputs, or malformed option values. | all binaries and visible surfaces. | `10` | caller error | Say the invocation is invalid and point to help, without continuing into input parsing. | `status`, `component`, `surface`, `argument_name`, `option_name`, `usage_rule`, `received_count`, `expected_count`. | slice04 parse-error behavior and CLI11 gate; slice05 missing-arg/malformed-arg probes; Arc04 CLI facade. |
| `input_unavailable` | Required input cannot be opened, read, or resolved, including missing HDDL includes. | parser normal HDDL parse; grounder normal `.htn`; engine normal search. | `20` | caller error | Name the input role and path that could not be read. | `status`, `component`, `surface`, `path`, `path_role`, `operation`, `errno_name`, `include_stack`. | slice03 diagnostics; slice05 missing file/include probes; Arc04 filesystem helpers. |
| `output_unavailable` | Required output path, temp path, or artifact destination cannot be created, opened, written, flushed, renamed, or cleaned up. | parser normal HDDL parse; grounder normal `.htn`; engine normal search and experimental translation if visible. | `21` | caller error | Name the output role and operation that failed. | `status`, `component`, `surface`, `path`, `path_role`, `operation`, `errno_name`, `partial_output_policy`. | slice03 stream/file policy; slice05 output-path probes; Arc04 RAII filesystem/temp helpers. |
| `input_invalid` | Input was read but is syntactically malformed, semantically invalid, truncated, internally inconsistent, or violates checked model bounds. | parser normal HDDL parse; grounder normal `.htn`; engine normal search. | `22` | input/model error | Say the input/model is invalid and include a stable location if available. | `status`, `component`, `surface`, `path`, `path_role`, `location`, `rule`, `token`, `model_section`, `finding_refs`. | slice03 payload fields; slice05 malformed-input probes; Arc04 checked parsing helpers; Arc05 per-binary validation. |
| `unsupported_feature` | The request uses a feature, mode, build option, or model construct rejected from the 0.3.0 contract. | engine SAT; unsupported planner/model features on supported paths; any hidden unsupported visible mode. | `30` | policy failure | Say the feature is unsupported in chengdu 0.3.0. | `status`, `component`, `surface`, `feature`, `required_build_flag`, `reentry_condition`, `finding_refs`. | slice04 help/version disclaimers; slice05 negative probes; Arc05 option fencing; Arc06 behavior-change table. |
| `legacy_surface` | A legacy surface was invoked where managed-process supervision expected the supported contract. | parser verifier/output helper paths; grounder `cpddl`/FAM; engine interactive mode under supervised or no-TTY invocation. | `31` | policy failure | Say the surface is legacy and outside the supervised contract. | `status`, `component`, `surface`, `surface_disposition`, `mode`, `tty_required`, `reentry_condition`. | slice03 TTY/stdio policy; slice04 CLI labeling; slice05 legacy negative/compat probes; Arc06 migration notes. |
| `experimental_surface` | An experimental surface was invoked without the final opt-in contract, or the experimental path failed within its fenced behavior. | grounder H2; engine translation. | `32` | policy failure | Say the surface is experimental and not a supported default contract. | `status`, `component`, `surface`, `surface_disposition`, `opt_in_flag`, `dependency_name`, `child_process_role`, `reentry_condition`. | slice03 stream/dependency fencing; slice04 experimental help text; slice05 opt-in probes; Arc04 conditional adapters. |
| `future_surface` | A dormant or future surface was requested but is not active in the 0.3.0 default binary contract. | engine BDD/CUDD if reachable; future optional dependency surfaces. | `33` | policy failure | Say the surface is future work and unavailable in this release. | `status`, `component`, `surface`, `surface_disposition`, `feature`, `build_flag`, `reentry_condition`. | slice04 hidden/future option handling; slice05 negative probes only if reachable; Arc06 release notes. |
| `timeout` | The operation exceeded a configured time limit owned by the binary or accepted supervisor contract. | engine normal search; grounder normal `.htn` if resource policy is adopted; parser only if a later contract adds parser time limits. | `40` | external/dependency failure | Say the operation timed out and name the limit. | `status`, `component`, `surface`, `timeout_ms`, `elapsed_ms`, `limit_owner`, `partial_output_policy`. | slice03 final status/event fields; slice05 timeout probes; Arc04 chrono/cancellation helpers; Arc05 engine adoption. |
| `resource_limit` | The operation exceeded a memory, node, file, disk, recursion, or other configured resource policy. | grounder normal `.htn`; engine normal search; parser normal HDDL parse where applicable. | `41` | external/dependency failure | Say which resource policy stopped execution. | `status`, `component`, `surface`, `resource_kind`, `limit_value`, `observed_value`, `limit_owner`, `partial_output_policy`. | slice03 payload fields; slice05 resource probes where feasible; Arc04 resource helpers; Arc06 behavior docs. |
| `interrupted` | The process handled an interrupt or termination request and exited through the status mapper. | all supported paths; visible legacy/experimental paths only if they can be safely intercepted. | `42` | external/dependency failure | Say execution was interrupted and whether cleanup completed. | `status`, `component`, `surface`, `signal_name`, `signal_number`, `cleanup_completed`, `partial_output_policy`. | slice03 flushing/event policy; slice05 signal probes; Arc04 signal/cancellation boundary; Arc05 adoption. |
| `dependency_failure` | An in-process dependency or dependency-internal adapter failed in a way that is not ordinary invalid user input. | grounder H2/cpddl/FAM; future BDD/CUDD only if reachable; any later accepted dependency boundary. | `50` | external/dependency failure | Name the dependency boundary and failure class. | `status`, `component`, `surface`, `dependency_name`, `dependency_role`, `dependency_status`, `boundary`, `finding_refs`. | slice03 dependency diagnostics; slice05 opt-in dependency probes; Arc04 adapter boundaries; Arc06 NOTICE/release docs if adopted. |
| `child_process_failure` | A child process required by a visible experimental or later accepted path failed, was killed, timed out, or returned an unmapped status. | engine translation; possible future H2 subprocess containment. | `51` | external/dependency failure | Name the child role and whether it exited, signaled, or timed out. | `status`, `component`, `surface`, `child_role`, `child_exit_code`, `child_signal`, `child_status`, `timeout_ms`, `stderr_available`. | slice03 child output/event policy; slice05 opt-in child-process probes; Arc04 local wrapper/reproc++ gate. |
| `internal_error` | A program invariant failed, an unexpected exception escaped, a deep helper reached an impossible state, or an unclassified defect was caught at the top boundary. | all binaries and surfaces. | `60` | internal defect | Say the tool hit an internal defect and include a stable diagnostic code. | `status`, `component`, `surface`, `diagnostic_code`, `source_boundary`, `finding_refs`, `crash_kind`. | slice03 fatal diagnostics; slice05 internal-error probes where safe; Arc04 status/result substrate; Arc05 deep-exit replacement. |
| `signal_terminated` | The OS terminated the process by a signal before the binary could emit `interrupted`. | all binaries; mostly observed by external supervisors, not emitted by the binary. | `128 + signal_number` | external/dependency failure | No binary-owned summary is guaranteed. Supervisors classify from process wait status. | `status`, `component`, `surface`, `signal_name`, `signal_number`, `wait_status`, `core_dumped`. | slice05 signal probes; Arc04 signal handling design; Arc06 supervisor docs. |

## 4. Numeric Exit-Code Table and Rationale

| Exit code | Status family | Rationale |
|-----------|---------------|-----------|
| `0` | `ok` | Conventional success. This code is reserved for actual successful completion or explicitly successful informational CLI commands. |
| `1` | reserved compatibility fallback | New 0.3.0 contract paths should not emit generic `1`. It is reserved for legacy compatibility wrappers or unmapped pre-contract failures during migration. |
| `2` | `domain_no_plan` | Preserves the 0.2.0 direction that proven unsolvable is not generic failure, while separating it from timeout/resource failure. |
| `10` | `cli_usage_error` | Caller supplied invalid invocation shape before input/model processing began. |
| `20` | `input_unavailable` | Input path, include, or input stream cannot be opened or read. |
| `21` | `output_unavailable` | Output or temp artifact cannot be opened, written, flushed, renamed, or cleaned up. |
| `22` | `input_invalid` | Input was available but invalid as HDDL, `.htn`, SAS/model, or component model data. |
| `30` | `unsupported_feature` | The requested feature is rejected from the 0.3.0 contract. |
| `31` | `legacy_surface` | A legacy surface was invoked where deterministic supervised behavior was expected. |
| `32` | `experimental_surface` | An experimental surface was invoked or failed under its fenced opt-in behavior. |
| `33` | `future_surface` | A dormant/future surface was requested but is unavailable in this release. |
| `40` | `timeout` | Time limit reached; deliberately distinct from `domain_no_plan`. |
| `41` | `resource_limit` | Non-time resource policy reached; deliberately distinct from malformed input and internal defect. |
| `42` | `interrupted` | Binary handled a termination request and exited through the status mapper. |
| `50` | `dependency_failure` | In-process dependency/adapted dependency boundary failed. |
| `51` | `child_process_failure` | Child process failed, timed out, was signaled, or returned an unmapped status. |
| `60` | `internal_error` | Program defect, invariant failure, unexpected exception, or unclassified top-boundary failure. |
| `64`-`78` | reserved | Reserved for possible future BSD `sysexits` compatibility aliases. Do not use inside the primary 0.3.0 contract without an Arc03/Arc04 compatibility decision. |
| `128 + N` | `signal_terminated` | Conventional supervisor-visible encoding for process death by signal `N` when the binary does not handle and map the signal itself. |

Rationale:

- Exit codes are grouped by meaning: success/domain outcome, caller/input,
  policy/surface, runtime/resource, dependency/child, and internal defect.
- `domain_no_plan` is not failure in the same class as malformed input,
  timeout, or internal defect. It gets a stable nonzero code because callers
  often need to branch on "processed successfully, no plan exists".
- Diagnostic prose is non-normative. A supervisor can first classify by
  numeric exit code, then refine by the stable status name and payload fields
  once slice03 chooses the machine-facing serialization.
- Deep `exit(-1)` behavior observed in Arc01 must not survive as status `255`;
  `255` is intentionally not assigned.
- If a legacy wrapper must temporarily preserve an inherited code, it must map
  the internal status to the documented code at the chengdu boundary or record
  the inherited code as compatibility behavior in slice04/slice05.

## 5. Per-Binary and Surface Application Notes

### 5.1 Parser Normal HDDL Parse

Supported surface: parser normal HDDL parse.

Required status coverage:

- `ok`: domain/problem parsed and selected output artifact produced.
- `cli_usage_error`: missing required domain/problem, incompatible output
  options, malformed option values, or invalid invocation shape. `P-005`
  requires this status to be terminal.
- `input_unavailable`: missing domain/problem file or missing include file.
  `P-002` requires include-open failure to classify before parser recursion.
- `output_unavailable`: output file cannot be opened, written, flushed, or
  finalized.
- `input_invalid`: HDDL parse error, malformed verifier/helper input if routed
  through the normal parse boundary, or release-disabled validation that should
  be runtime validation (`P-001`, `P-003`).
- `legacy_surface`: parser verifier/output helper invocation under supervised
  mode, unless a later slice accepts a helper-specific contract.
- `internal_error`: former deep exits from `P-004` after all known user/model
  cases have been classified.

Parser status payloads should include `input_role` (`domain`, `problem`, or
`include`), `path`, `location`, `rule`, and `include_stack` where applicable.
slice03 owns whether those fields appear as events, stderr diagnostics, or a
separate machine stream.

### 5.2 Grounder Normal `.htn` Grounding

Supported surface: grounder normal `.htn` to planner-output path.

Required status coverage:

- `ok`: `.htn` input grounded and planner artifact produced.
- `domain_no_plan`: unreachable goal or no useful grounded output if the
  supported normal path can prove that outcome. `G-004` requires this to stop
  using `_exit(0)`.
- `cli_usage_error`: missing required input unless `-` is explicitly accepted
  by slice04; malformed option values (`G-005`).
- `input_unavailable`: `.htn` input cannot be opened/read.
- `output_unavailable`: output file cannot be opened/written or would be opened
  after a failed parse (`G-006`).
- `input_invalid`: malformed `.htn`, invalid counts/indexes/cross-references,
  or release-disabled validation (`G-002`, `G-003`).
- `legacy_surface`: explicit `cpddl`/FAM advanced behavior under managed
  invocation unless later accepted.
- `experimental_surface`: H2 invocation before a final opt-in contract exists.
- `dependency_failure`: cpddl/FAM/H2 in-process dependency boundary failure
  that is not user input invalidity.
- `timeout` and `resource_limit`: if a binary-owned or accepted supervisor
  limit is applied to grounding (`G-019`).
- `internal_error`: former deep exits after known invalid/unsupported cases are
  classified (`G-004`).

Grounder payloads should distinguish `input_invalid` from `dependency_failure`:
bad `.htn` is a caller/model problem; H2/cpddl internals failing after accepted
input is a dependency-boundary problem.

### 5.3 Engine Normal Search

Supported surface: engine normal search.

Required status coverage:

- `ok`: plan found and emitted according to slice03 output policy.
- `domain_no_plan`: proven unsolvable or unreachable result. This status is
  distinct from timeout and resource exhaustion, addressing `E-003`.
- `cli_usage_error`: command-line parse errors, too many input files,
  unsupported selected planner configuration, or unknown heuristic before model
  processing.
- `input_unavailable`: engine input cannot be opened/read.
- `input_invalid`: malformed/truncated SAS/model data, invalid counts/indexes,
  or release-disabled validation that should be runtime validation (`E-001`).
- `timeout`: configured engine time limit reached (`E-008`).
- `resource_limit`: configured memory/node/other resource limit reached.
- `interrupted`: SIGINT/SIGTERM handled and mapped by the binary.
- `signal_terminated`: supervisor observes an unhandled signal death.
- `legacy_surface`: interactive mode under supervised or no-TTY invocation
  (`E-007`).
- `experimental_surface`: translation mode before a final opt-in contract
  exists (`E-015`).
- `unsupported_feature`: SAT mode if reachable in the default binary (`E-009`).
- `future_surface`: BDD/CUDD if reachable in the default binary (`E-016`).
- `child_process_failure`: translation child process failure if translation
  remains visible.
- `internal_error`: former deep exits, impossible invariants, unexpected
  exceptions, or unclassified top-boundary failures (`E-002`).

Engine payloads must allow a supervisor to distinguish solved, unsolvable,
timeout, interrupted, malformed input, unsupported mode, and internal defect
without parsing `Status:` or any other human text from stdout.

### 5.4 Visible Non-Supported Surfaces

| Surface | slice01 disposition | Status treatment |
|---------|--------------------|------------------|
| parser verifier/output helper paths | legacy | `legacy_surface` under managed invocation; if intentionally invoked by a human compatibility path, final behavior is slice04/slice05 compatibility scope, not a supported supervised contract. |
| grounder H2 | experimental | `experimental_surface` unless a later opt-in contract accepts best-effort execution; dependency failures under that path use `dependency_failure`. |
| grounder `cpddl`/FAM | legacy | `legacy_surface` for user-facing advanced behavior under supervised mode; `dependency_failure` for dependency-boundary failures after accepted input. |
| engine interactive mode | legacy | `legacy_surface` under supervised/no-TTY invocation; only a later TTY-specific design can re-enter it. |
| engine translation mode | experimental | `experimental_surface` for invocation; `child_process_failure`, `output_unavailable`, or `dependency_failure` for fenced opt-in failures if it remains visible. |
| engine SAT | unsupported | `unsupported_feature`; signal/thread/direct-exit behavior from `E-009` must not be exposed as supported. |
| engine BDD/CUDD | future | `future_surface` if reachable; otherwise no default status obligation beyond hiding it. |

## 6. Timeout, Resource, Signal, and Interruption Semantics

### 6.1 Timeout

`timeout` means execution exceeded a configured wall-clock or CPU-time limit.
It is not `domain_no_plan`, even if the current engine historically reused
exit code `2` for both. The payload must identify `timeout_ms`, `elapsed_ms`,
and `limit_owner` (`binary`, `supervisor`, or `compat_wrapper`).

For 0.3.0, engine normal search is the primary timeout owner. Grounder timeout
is allowed only if the later contract defines a binary-owned or supervisor
owned limit. Parser timeout is future unless a later contract names a parser
limit.

### 6.2 Resource Policy

`resource_limit` means a non-time resource policy stopped execution: memory,
node count, recursion depth, file count, disk space, or another named resource.
It differs from `input_invalid`: valid but large input may hit a policy limit,
while malformed input fails validation. It differs from `internal_error`:
resource limit is expected policy behavior, not a defect.

Grounder `G-019` requires at least a documented mapping for externally imposed
resource termination. If the binary cannot observe OOM directly, the supervisor
may classify OS kill or exit status at the wrapper boundary, but the contract
must still produce stable `resource_limit` semantics where observable.

### 6.3 Interruptions and Signals

`interrupted` is binary-owned: the process caught an interruption request,
performed the cleanup and flush behavior that slice03 defines, and returned
through the status mapper with exit code `42`.

`signal_terminated` is supervisor-owned: the process died by signal before it
could return a status. Supervisors classify it as `128 + signal_number`, such
as `130` for SIGINT and `143` for SIGTERM. The binary should try to convert
ordinary SIGINT/SIGTERM into `interrupted` on supported paths, but cannot
guarantee that for SIGKILL or hard crashes.

SAT signal behavior from `E-009` is excluded from the supported 0.3.0 contract.
It must not reserve process-global real-time signals in a default supported
path.

### 6.4 Partial Output

Timeout, resource limit, interrupted, dependency failure, child-process
failure, and internal error must carry `partial_output_policy`: at minimum
`none`, `discarded`, `retained`, `unknown`, or a later slice03 equivalent.
This slice does not decide stream/file cleanup behavior; it names the semantic
field required so callers can avoid trusting partial artifacts by accident.

## 7. Dependency and Child-Process Failure Semantics

`dependency_failure` applies when an in-process dependency or dependency
adapter fails after input and invocation are otherwise accepted. It covers
grounder H2/cpddl/FAM boundary failures and future optional dependency
boundaries. It does not cover malformed caller input.

`child_process_failure` applies when a subprocess is part of a visible
experimental or later accepted path and the child exits unsuccessfully, dies by
signal, times out, cannot be started, or emits an unmapped status. It currently
matters only for engine translation and possible future H2 containment.

Because H2 and translation are experimental rather than supported, these
statuses do not make reproc++ or any child-process dependency part of the
default 0.3.0 supported path. Arc04 may build a local wrapper or conditional
adapter only if a later accepted contract needs it.

Required child/dependency payload fields:

- `dependency_name` or `child_role`;
- `boundary` (`in_process`, `adapter`, `subprocess`);
- `operation`;
- `dependency_status`, `child_exit_code`, or `child_signal` where applicable;
- `timeout_ms` and `elapsed_ms` for child timeout;
- `partial_output_policy`;
- `finding_refs`, such as `G-008`, `G-013`, or `E-015`.

## 8. Arc02 Dependency-Gating Implications

- `tl::expected`: This report defines `status`, `class`, `exit_code`, and
  payload semantics that a future `StatusResult<T>` may carry. It does not
  design the C++ API or expose `tl::expected`.
- reproc++: Remains conditional. Since H2 and translation are experimental,
  status semantics for `child_process_failure` are enough for now; no default
  supported path requires a process library.
- nlohmann/json and JSON: Held for slice03. This report names payload fields
  and event/status classes but does not choose JSON Lines, tagged text, or
  another serialization.
- CLI11: Held for slice04. This report defines `cli_usage_error`; slice04 owns
  option grammar, aliases, help/version text, `pandapi-*` migration, and
  golden CLI compatibility.
- fmt: Held behind slice03/Arc04 diagnostics facade. This report does not
  specify formatting calls, logging APIs, stdout/stderr routing, or color.
- Catch2 and process fixtures: Catch2 remains test-only. slice05 and Arc04
  must prove process behavior with process fixtures; unit tests alone are not
  process-contract proof.
- Abseil and Boost.Process: Remain rejected as 0.3.0 foundations.
- Held candidates: GSL, performance containers, google/benchmark,
  parser-generator alternatives, SAT/BDD/CUDD enablement, H2/cpddl
  dependency-internal work, and broad optional dependency audits stay held
  behind their explicit re-entry conditions.

## 9. Later Slice and Arc Inputs

### slice03 - stdio, event, TTY, color, buffering

slice03 must decide where human summaries and machine-facing status payloads
are emitted. It must ensure stdout is owned by the data artifact or selected
machine-event stream, and that statuses above do not rely on diagnostic prose.
It must also define partial-output policy, flushing on `interrupted`, color
suppression, and TTY behavior for `legacy_surface` interactive mode.

### slice04 - CLI naming, version, parse errors, migration

slice04 must define how `cli_usage_error`, `unsupported_feature`,
`legacy_surface`, `experimental_surface`, and `future_surface` appear in help,
version/provenance text, parse-error output, compatibility aliases, and
`pandapi-*` migration. CLI11 remains a candidate only after this surface and
golden outputs are fixed.

### slice05 - contract test matrix

slice05 must include positive and negative probes for each status family that
is reachable in the default binaries:

- successful parser/grounder/engine paths;
- engine unsolvable and any grounder unreachable outcome;
- missing args, malformed args, missing input, bad include, bad output path;
- malformed HDDL, `.htn`, and engine model input;
- timeout, resource, SIGINT/SIGTERM/interrupted, and unhandled signal behavior
  where feasible;
- visible legacy, experimental, unsupported, and future surfaces;
- dependency and child-process failures only for visible opt-in paths.

Process fixtures, not Catch2 alone, must assert exit code, status name, stdout,
stderr, files, signals, and partial-output behavior.

### Arc04 - shared status/result substrate

Arc04 should implement a small shared substrate:

- `process_status` enum matching this vocabulary;
- exit-code mapper matching this table;
- payload structs or adapters carrying the named fields;
- top-level `main()` mapping that replaces deep exits;
- optional `StatusResult<T>` or `tl::expected` alias after the local status
  types are stable;
- RAII filesystem/temp helpers and signal/cancellation boundaries where
  required.

### Arc05 - per-binary adoption

Arc05 must migrate parser, grounder, and engine one binary at a time. It must
keep normal supported paths conformant while fencing legacy, experimental,
unsupported, and future paths. Library availability must not change a status
classification.

### Arc06 - release docs and behavior-change table

Arc06 must document behavior changes for exit codes, status names, supported
versus non-supported surfaces, signal/timeout/resource behavior, dependency
gates, and any compatibility wrappers or aliases. Release docs must preserve
wolong's fetch/install shape unless the operator accepts a breaking migration.

## 10. Final Recommendation

Adopt this taxonomy as the status basis for the rest of Arc03:

- supported paths use `ok`, `domain_no_plan`, caller/input/output errors,
  timeout/resource/interrupted, dependency/child failures where applicable, and
  `internal_error`;
- visible non-supported paths map to `legacy_surface`,
  `experimental_surface`, `unsupported_feature`, or `future_surface` according
  to slice01;
- `domain_no_plan` remains separate from timeout/resource failure;
- unhandled signal death uses supervisor-visible `128 + signal_number`, while
  graceful binary-owned interruption uses `interrupted`;
- Arc04 may implement these semantics with a local status/result substrate and
  only then pilot `tl::expected` behind that facade.

This keeps the contract semantic, testable, and independent of diagnostic
prose, while preserving Arc02's dependency gates for the implementation arcs.
