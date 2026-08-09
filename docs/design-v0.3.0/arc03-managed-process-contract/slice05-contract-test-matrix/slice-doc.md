# chengdu arc03 / slice05 - contract-test-matrix - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Produce the Arc03 contract test matrix report:

`docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`

This report turns the accepted Arc03 design slices into testable behavior. It
must define the golden process fixtures, acceptance probes, expected
observations, and harness ownership boundaries that Arc04 and Arc05 will use
to prove the managed-process contract for parser, grounder, and engine.

Slice05 is design-only. It defines what must be tested and how observations
are classified; it does not create fixture files, implement a harness, add
Catch2, change build scripts, or modify planner source.

## 2. Scope

**In:**

- Accepted slice01 supported-surface classification:
  - parser normal HDDL parse is supported;
  - grounder normal `.htn` grounding is supported;
  - engine normal search is supported;
  - parser helper/verifier, grounder `cpddl`/FAM, and engine interactive are
    legacy;
  - grounder H2 and engine translation are experimental;
  - engine SAT is unsupported;
  - engine BDD/CUDD is future.
- Accepted slice02 status vocabulary, numeric exit codes, payload classes,
  timeout/resource/signal semantics, dependency and child-process failure
  semantics.
- Accepted slice03 stdout/stderr/status, buffering/flushing, partial-output,
  color, TTY, quiet, verbosity, and non-supported-surface fencing rules.
- Accepted slice04 canonical/inherited command names, help/version/provenance,
  CLI parse-error, `--supervised`, `--status`, `--status=stderr|stdout`,
  `--output`, `--color`, `--no-color`, `--no-colour`, `NO_COLOR`, and CLI11
  golden-output gating decisions.
- A process fixture model for both human CLI and supervised-process
  invocation, including command line, environment, stdin/stdout/stderr,
  artifacts, exit status, signal observation, timeout/resource policy, and
  final `PANDAPI_STATUS` parsing.
- A golden-output policy that distinguishes stable machine/status fields from
  non-normative human diagnostic prose.
- A coverage matrix for parser, grounder, and engine positive and negative
  behavior:
  - canonical and inherited command names;
  - help, version, and provenance;
  - missing args, unknown options, malformed values, option conflicts, and
    unsupported status-stream placement;
  - missing input and unavailable output;
  - malformed or invalid input;
  - valid file-backed and stdout/pipe-backed artifact behavior;
  - engine solved and `domain_no_plan` behavior;
  - timeout, resource, interrupt, and signal termination expectations;
  - color, no-color/no-colour, `NO_COLOR`, TTY, and no-TTY behavior;
  - visible legacy, experimental, unsupported, and future surfaces.
- The split between black-box process fixtures and Catch2 seam tests, including
  what belongs in Arc04 substrate tests versus Arc05 per-binary adoption tests.
- Later-arc routing for Arc04 harness/substrate work, Arc05 per-binary
  adoption gates, and Arc06 release/CI/wolong evidence.

**Out:**

- No planner source changes, dependency vendoring, build-system edits, scripts,
  workflows, release assets, generated fixture files, `vendor.env`, or
  `pins.env`.
- No implementation of the process test harness.
- No adoption or integration of Catch2, CLI11, fmt, `tl::expected`, reproc++,
  JSON, or any other dependency.
- No changes to status names, numeric exit codes, tagged status field schema,
  stdout/stderr ownership, command names, option spellings, supported surfaces,
  release asset shape, or wolong migration policy.
- No final `managed-process-contract.md`; slice06 owns synthesis.
- No requirement that every future probe be runnable before Arc04/Arc05. This
  slice may classify probes as design-required, Arc04 harness-required, Arc05
  per-binary-required, Arc06 release-required, or deferred with re-entry
  conditions.

## 3. Required Decisions

The report must decide, at design level:

- the canonical structure of a process fixture/probe record;
- which fixture dimensions are stable enough for golden comparison and which
  are semantic predicates only;
- how to normalize environment-sensitive observations such as paths, duration,
  compiler/build metadata, timestamps, signal names, errno names, and TTY
  detection;
- how to parse and validate the final `PANDAPI_STATUS` line without relying on
  human diagnostic prose;
- how to detect stdout ownership violations, stderr diagnostic expectations,
  ANSI leakage, buffering/flushing failures, and partial-output policy;
- the minimum parser, grounder, and engine fixture set required before Arc05
  can claim conformance;
- the minimum compatibility probes for inherited `pandaPI*` names and retained
  positional output forms;
- the negative probe set for CLI usage errors, missing files, unavailable
  outputs, invalid inputs, unsupported/legacy/experimental/future surfaces,
  timeouts, resource limits, interrupts, and signal termination;
- which checks are black-box process fixtures and which are Catch2 seam tests;
- which tests gate CLI11 adoption, fmt facade behavior, `tl::expected` status
  substrate use, and any conditional reproc++ child-process adapter;
- which fixture obligations move to Arc04, Arc05, and Arc06.

## 4. Constraints

- Tests serve the accepted product contract. Do not expand the supported
  surface to make a fixture easier to write.
- Golden output must not freeze non-normative human diagnostic prose unless
  the report explicitly identifies the text as stable contract text.
- Machine assertions must prefer exit code and tagged status fields over
  diagnostic regex.
- stdout ownership is a hard invariant: artifact, status stream, or empty.
- `--status=stdout` conflict behavior must be represented explicitly.
- TTY/color probes must not require interactive behavior from legacy surfaces.
- Timeout, resource, and signal probes must be safe for CI and may be specified
  as harness obligations with bounded timings and cleanup rules.
- Catch2 remains test-only and process fixtures remain black-box; the report
  must not imply Catch2 becomes a runtime dependency or that subprocess tests
  require reproc++.
- Preserve wolong's frozen fetch/install shape and the 0.3.0 compatibility
  transition from slice04.

## 5. Verification Approach

CC verifies by producing the contract test matrix report, updating the ledger,
and writing a closing report with a row-by-row walk and Bubble-up to Arc03.
CDC verifies row count, report existence, prior-slice coverage, fixture schema,
positive and negative probe coverage, stdout/stderr/status/color/signal
coverage, Arc02 dependency gates, later-arc routing, and protected-path
cleanliness.

## 6. Exit Criteria

The ledger reaches final status. The report exists at the planned path, names
the accepted Arc03 prior slices as inputs, defines a reusable process fixture
schema, enumerates parser/grounder/engine acceptance probes, covers both human
CLI and supervised-process behavior, preserves Arc02 dependency gates, splits
black-box process fixtures from Catch2 seam tests, routes all implementation
and release obligations to later arcs, and leaves source/build/release paths
untouched.
