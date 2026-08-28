# CC assignment - chengdu arc03 / slice03 - stdio-event-tty-contract

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc03-managed-process-contract/arc-plan.md`
4. `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/slice-doc.md`
5. `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md`
6. Accepted Arc03 slice01 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/cdc-verification.md`
7. Accepted Arc03 slice02 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/cdc-verification.md`
8. Arc01 close and synthesis:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/closing-report.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
9. Arc01 component reports as needed for stream/TTY details:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
10. Arc02 close and combined recommendations:
   - `docs/design-v0.3.0/arc02-cpp-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

## The work

Produce:

`docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`

This is the third Arc03 design artifact. It must define how humans and
process supervisors observe the statuses and payloads accepted by slice02,
while preserving the supported surface accepted by slice01.

The contract must cover:

- stdout ownership for parser normal HDDL parse;
- stdout ownership for grounder normal `.htn` grounding;
- stdout ownership for engine normal search;
- stderr ownership for diagnostics, summaries, warnings, progress, statistics,
  and fatal/internal-error summaries;
- event or machine-readable status output, if supported;
- status payload field routing from slice02;
- buffering, flushing, and partial-output policy;
- ANSI/color and TTY/no-TTY behavior;
- quiet, verbosity, progress, and statistics behavior;
- stream/event/TTY fencing for visible legacy, experimental, unsupported, and
  future surfaces.

## Decisions you must make

Decide whether 0.3.0 supports machine-readable event/status output. If yes,
name the format class: JSON Lines, tagged text, status-only side channel, or
another documented class. If no, explain what supervised callers can rely on in
0.3.0 and what re-entry condition would add events later.

Decide nlohmann/json's Arc02 disposition for this contract. If the event format
is JSON, describe why nlohmann/json re-enters as an implementation candidate
for Arc04. If the event format is not JSON or events are deferred, keep
nlohmann/json held.

Define policy-level mode names if useful, but do not decide final flag spelling
or CLI option grammar. Slice04 owns names, flags, aliases, help/version text,
and CLI11 migration.

## Arc02 constraints you must preserve

Arc02 findings are load-bearing:

- fmt may be used only behind a future diagnostics/process I/O facade. Do not
  specify direct `fmt::print` calls in planner code.
- nlohmann/json may re-enter only if this slice chooses a JSON event/status
  format. Otherwise it remains held.
- CLI11 remains slice04-owned.
- Catch2 and process fixtures remain slice05/Arc04-owned.
- reproc++ remains conditional and must not be justified only by stream policy
  for H2 or translation.
- `tl::expected` remains behind the status/result substrate from slice02/Arc04.
- Abseil and Boost.Process remain rejected as 0.3.0 foundation choices.

## Constraints

- Do not edit planner source, dependency source, scripts, workflows, release
  assets, build outputs, `vendor.env`, or `pins.env`.
- Do not adopt dependencies or change build policy.
- Do not change status names or numeric exit codes; slice02 owns those.
- Do not decide final command names, option names, aliases, help/version text,
  or CLI11 migration; route those inputs to slice04.
- Do not write the final contract test matrix; route test inputs to slice05.
- Preserve wolong's frozen fetch/install shape unless the operator explicitly
  accepts a breaking migration.

## Suggested report shape

1. Executive summary.
2. Evidence base: Arc01, Arc02, slice01, and slice02.
3. Output/invocation modes.
4. stdout ownership table.
5. stderr diagnostics/progress/statistics policy.
6. Machine event/status output decision and status payload routing.
7. Buffering, flushing, and partial-output policy.
8. ANSI/color and TTY/no-TTY policy.
9. Quiet, verbosity, progress, and statistics behavior.
10. Non-supported surface stream/event/TTY fencing.
11. Arc02 dependency-gating implications.
12. Later slice and arc inputs.
13. Final recommendation.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc03, and leave `cdc-verification.md` for CDC.
