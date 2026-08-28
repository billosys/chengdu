# Closing Report: stdio-event-tty-contract

Date: 2026-08-09

Branch: `release/0.3.x`

Primary slice commit: `e465f42736db9d443b196621d06436c6e644ee2f`
(`Close Arc03 stdio event tty contract`)

Corrective close-set commit: this report was added after CDC identified that
the CC closing report artifact was missing from the primary slice commit.

## Result

Slice 03 is CC-complete after adding this report. The slice delivered the
stdio/event/TTY contract report, closed all 12 ledger rows, preserved the
design-only boundary, and left CDC verification independent.

The main artifact is:

`docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`

## Delivered Scope

The slice report defines:

- invocation/output modes for human CLI, supervised/process-manager, and
  machine-event use without deciding final CLI option spelling;
- stdout ownership for parser normal HDDL parse, grounder normal `.htn`
  grounding, and engine normal search;
- stderr ownership for diagnostics, warnings, progress, statistics, and human
  summaries while making diagnostic prose non-normative for supervisors;
- 0.3.0 machine-readable status output as tagged text status-only records, not
  JSON Lines;
- slice02 status payload field routing into final tagged status records;
- buffering, flushing, and partial-output policy;
- ANSI/color, TTY, no-TTY, quiet, verbosity, progress, and statistics policy;
- stream/event/TTY fencing for legacy, experimental, unsupported, and future
  surfaces from slice01;
- preservation of Arc02 dependency gates.

## Ledger Walk

The opening ledger contained 12 rows and all are closed in
`slice03-stdio-event-tty-contract/ledger.md`.

| Row | Status | CC evidence |
|-----|--------|-------------|
| F-1 | closed | The contract report exists and names Arc01, Arc02, slice01, slice02, `audit-synthesis-pandapi`, `combined-library-recommendations`, `supported-surface-classification`, and `status-exit-signal-taxonomy` as evidence inputs. |
| F-2 | closed | The report defines human CLI, supervised/process-manager, and machine-event modes while routing final CLI option and flag spelling to slice04. |
| F-3 | closed | The report defines stdout as a single artifact/event/empty channel and covers parser normal HDDL parse, grounder normal `.htn` grounding, engine normal search, `domain_no_plan`, success, and non-success behavior without diagnostic/progress/statistics mixing. |
| F-4 | closed | The report assigns diagnostics, summaries, warnings, progress, statistics, fatal, and `internal_error` human prose to stderr and marks diagnostic prose non-normative for supervisors. |
| F-5 | closed | The report selects machine-readable tagged text, status-only output for 0.3.0, lists required slice02 payload fields, defines ordering/flush expectations, and keeps `nlohmann/json` held unless a later event-format decision selects JSON. |
| F-6 | closed | The report defines buffering, flushing, and `partial_output_policy` behavior for pipes, TTYs, files, event output, `timeout`, `resource_limit`, `interrupted`, `dependency_failure`, `child_process_failure`, and `internal_error`. |
| F-7 | closed | The report defines ANSI/color and TTY policy: auto color only for suitable human TTY stderr, no ANSI in supervised/machine output, no-color/no-colour and `NO_COLOR`, plus no-TTY interactive fencing. |
| F-8 | closed | The report defines quiet, verbosity, progress, and statistics behavior for human and supervised invocation and requires H2 `std::cout` stream mutation containment. |
| F-9 | closed | The report fences parser verifier/output helpers, grounder H2, grounder `cpddl`/FAM, engine interactive, engine translation, engine SAT, and engine BDD/CUDD. |
| F-10 | closed | The report preserves Arc02 dependency gates for `fmt`, `nlohmann/json`, `CLI11`, `Catch2`, `reproc++`, Abseil, and Boost.Process. |
| F-11 | closed | The report identifies slice04, slice05, Arc04, Arc05, and Arc06 consumers for CLI mapping, contract tests, shared diagnostics/process I/O, per-binary migration, release docs, license/NOTICE, and behavior-change entries. |
| F-12 | closed | The protected-path check returned no paths, confirming no planner source, dependency source, scripts, workflows, release assets, build outputs, or vendored dependency files changed in the design-only slice. |

## Verification Evidence

Row count:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md
```

Result: `12`.

Open-row check:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md
```

Result: no output.

Protected-path check:

```bash
git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Whitespace check before the primary slice commit:

```bash
git diff --cached --check
```

Result: no output.

Primary slice commit content:

```bash
git show --stat --oneline --name-status e465f42736db9d443b196621d06436c6e644ee2f
```

Result:

```text
e465f427 Close Arc03 stdio event tty contract
M	docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md
A	docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md
```

## Silent-Drop Diff

Scope requested but intentionally not delivered in this slice:

- final command names, option names, aliases, help text, version text, and
  CLI11 migration: deferred to slice04;
- final contract test matrix and fixture implementation: deferred to slice05
  and Arc04;
- planner source changes, dependency source changes, scripts, workflows,
  release assets, build outputs, `vendor.env`, and `pins.env`: explicitly out
  of scope for this design-only slice;
- JSON Lines event stream and `nlohmann/json` adoption: held because the
  accepted 0.3.0 event/status format is tagged text status-only output;
- implementation of diagnostics/process I/O facade: routed to Arc04;
- per-binary parser, grounder, and engine migration: routed to Arc05;
- release docs, license/NOTICE, and behavior-change tables: routed to Arc06.

No requested in-scope ledger row remains open.

## Bubble-Up To Arc03

Slice 03 gives Arc03 a normative stream/status/TTY contract:

- stdout is a single selected artifact sink, event/status sink, or empty;
- stderr is the human diagnostic/progress/statistics/summary channel;
- supervisors must use exit codes and tagged status records rather than
  parsing diagnostic prose;
- 0.3.0 supports tagged text status-only records and keeps JSON dependencies
  held;
- legacy, experimental, unsupported, and future surfaces are fenced before
  they can contaminate supported stream semantics.

Arc03 follow-up routing:

- Slice04 must map these semantic modes to CLI spelling, aliases, help,
  version text, and color/status controls.
- Slice05 must convert the contract into process-level tests and fixtures.
- Arc04 must provide shared diagnostics/process I/O helpers for formatting,
  status escaping, buffering/flushing, TTY detection, and color policy.
- Arc05 must migrate parser, grounder, and engine behavior to the accepted
  contract.
- Arc06 must document release behavior changes and any license/NOTICE effects
  from dependencies actually adopted.

## CDC Handoff

CDC verification remains independent. The current CDC file records the missing
closing report as a blocker against the primary slice commit; this corrective
close-set artifact resolves that specific CC omission and should be rerun by
CDC without treating this report as CDC evidence.
