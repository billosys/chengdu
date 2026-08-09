# chengdu arc03 / slice03 - stdio-event-tty-contract - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Produce the Arc03 stdio, event, buffering, TTY, color, diagnostics, progress,
quiet-mode, and partial-output contract report:

`docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`

This report decides how the statuses and payload fields accepted in slice02 are
observed by human CLI users and by supervising processes. It must make stdout
ownership explicit for every supported normal path, decide where diagnostics
and progress belong, decide whether machine-readable event output exists and in
what format class, and define TTY/color/buffering rules that prevent accidental
ANSI leakage or pipe ambiguity.

## 2. Scope

**In:**

- Accepted slice01 surface classifications from
  `../supported-surface-classification.md`.
- Accepted slice02 status vocabulary, exit-code table, payload fields, and
  partial-output requirements from `../status-exit-signal-taxonomy.md`.
- Arc01 process I/O evidence:
  - parser stdout contamination and ANSI leakage: `P-006`, `P-007`, plus
    shared helper candidate `P-014`;
  - grounder stdout mixing, quiet-mode global stream mutation, ANSI output, and
    output-open-after-failure hazards: `G-006`, `G-007`, `G-008`, `G-009`;
  - engine stdout mixing, ANSI color, timeout/signal cleanup, translation
    subprocess output, and diagnostic facade candidates: `E-004`, `E-005`,
    `E-008`, `E-014`, `E-015`.
- Arc02 dependency gates:
  - fmt is an implementation primitive only under a future diagnostics/process
    facade;
  - nlohmann/json re-enters only if this slice chooses JSON Lines or another
    JSON event/status format;
  - CLI11 still belongs to slice04;
  - Catch2/process fixtures belong to slice05 and Arc04;
  - reproc++ remains conditional and must not be justified by stream policy
    alone;
  - Abseil and Boost.Process remain rejected as 0.3.0 foundations.
- Ownership of stdout, stderr, optional event output, status payloads,
  diagnostic summaries, progress/statistics, color, quiet/no-progress behavior,
  buffering/flushing, partial output, and TTY/no-TTY behavior.
- Supported normal paths:
  - parser normal HDDL parse;
  - grounder normal `.htn` grounding;
  - engine normal search.
- Visible non-supported surfaces where stream/event/TTY behavior must be fenced:
  parser helper/verifier paths, grounder H2, grounder `cpddl`/FAM, engine
  interactive, engine translation, engine SAT, and engine BDD/CUDD.
- Later-slice inputs for slice04 CLI naming/version/migration and slice05
  contract tests, plus Arc04 diagnostics/process I/O substrate inputs.

**Out:**

- No new status names or numeric exit-code changes; slice02 owns the taxonomy.
- No final CLI option spelling, command names, aliases, help text, version text,
  or CLI11 migration plan; slice04 owns those. This slice may define semantic
  modes that slice04 later maps onto flags.
- No final contract test matrix, fixture inventory, Catch2 harness design, or
  process runner implementation; slice05 and Arc04 own those.
- No dependency vendoring, build changes, planner source changes, scripts,
  workflows, release assets, or packaging changes.
- No expansion of supported product surfaces beyond slice01.

## 3. Required Decisions

The report must decide, at design level:

- for each supported normal path, what stdout contains on success and on every
  non-success status;
- what stderr contains for human diagnostics, progress, warnings, summaries,
  and fatal errors;
- whether machine-readable events are supported in 0.3.0, and if so whether
  they are JSON Lines, tagged text, status-only side-channel output, or another
  documented format class;
- where slice02 status names and payload fields appear in human mode,
  supervised mode, and any event mode;
- whether nlohmann/json re-enters for 0.3.0 event/status output or remains held;
- how fmt is constrained behind a future diagnostics/process I/O facade;
- pipe versus TTY behavior, including default color, NO_COLOR/no-color
  semantics at policy level, and suppression of ANSI escape sequences in
  supervised/machine modes;
- buffering and flushing policy for stdout, stderr, event output, successful
  artifacts, `domain_no_plan`, timeout, resource limit, interrupted,
  dependency/child-process failure, and internal error;
- quiet and progress behavior for human CLI and supervised invocation;
- partial-output policy semantics for files and streams;
- stream fencing for legacy, experimental, unsupported, and future surfaces.

## 4. Constraints

- stdout is owned. A path may use stdout for one selected data artifact, one
  selected machine-event stream, or intentionally nothing; it must not mix
  human diagnostics/progress/statistics with machine data.
- Diagnostic prose is non-normative. It may help humans, but supervisors must
  classify outcomes from slice02 status and exit semantics, plus any selected
  machine payload/event format.
- Do not let nlohmann/json availability decide event mode. Choose the event
  mode from product/consumer needs, then decide whether JSON is justified.
- Do not let fmt become direct scattered output. It remains an implementation
  candidate behind a future shared diagnostics/process I/O facade.
- Keep interactive mode legacy and TTY-gated. Do not make it a managed-process
  protocol by accident.
- Preserve wolong's frozen fetch/install shape unless the operator explicitly
  accepts a breaking migration.

## 5. Verification Approach

CC verifies by producing the stdio/event/TTY contract report, updating the
ledger, and writing a closing report with a row-by-row walk and bubble-up to
Arc03. CDC verifies row count, stdout/stderr/event ownership, status payload
routing, Arc02 dependency gates, non-supported-surface fencing, later-slice
routing, and protected-path cleanliness.

## 6. Exit Criteria

The ledger reaches final status. The report exists at the planned path, defines
stdout/stderr/event ownership for supported and visible non-supported surfaces,
decides the event format class and nlohmann/json disposition, defines color,
TTY, buffering, flushing, quiet/progress, and partial-output policy, preserves
Arc02 gates, identifies later slice/arc consumers, and leaves source/build/
release paths untouched.
