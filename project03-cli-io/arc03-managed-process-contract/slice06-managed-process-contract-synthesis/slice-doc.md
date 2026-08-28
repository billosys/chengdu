# chengdu arc03 / slice06 - managed-process-contract-synthesis - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Produce the final Arc03 managed-process contract:

`docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`

This report synthesizes the five accepted Arc03 design reports into one
normative contract for 0.3.0 and routes implementation, test, adoption, and
release obligations to Arc04, Arc05, and Arc06. It is the final design slice in
Arc03.

Slice06 is design-only. It writes the final contract and closes the synthesis
ledger; it does not modify planner source, add dependencies, implement tests,
change build scripts, or create release assets.

## 2. Scope

**In:**

- Accepted Arc03 slice01 supported-surface classification.
- Accepted Arc03 slice02 status, exit, and signal taxonomy.
- Accepted Arc03 slice03 stdio, event, buffering, partial-output, ANSI/color,
  TTY, quiet, and verbosity contract.
- Accepted Arc03 slice04 CLI naming, compatibility, help/version/provenance,
  parse-error, `--supervised`, `--status`, and wolong migration contract.
- Accepted Arc03 slice05 contract test matrix.
- Arc01 audit synthesis and Arc02 combined library recommendations as
  load-bearing background.
- A final normative 0.3.0 contract covering:
  - supported, legacy, experimental, unsupported, and future surfaces;
  - canonical and inherited command names;
  - CLI and supervised-process modes;
  - status names, numeric exit codes, payload classes, and signal semantics;
  - stdout, stderr, tagged status, buffering/flushing, and partial outputs;
  - help, version, provenance, parse errors, color, and TTY behavior;
  - fixture/test obligations and comparison policy;
  - Arc02 dependency gates and shared-runtime substrate requirements;
  - Arc04, Arc05, and Arc06 handoff obligations;
  - behavior-change, release-doc, license/NOTICE, and wolong verification
    inputs;
  - explicit deferrals and re-entry conditions for non-supported surfaces and
    held dependencies.

**Out:**

- No planner source changes, dependency vendoring, build-system edits, scripts,
  workflows, release assets, generated fixtures, `vendor.env`, or `pins.env`.
- No implementation of shared runtime substrate, process harness, CLI wrapper,
  status/result types, diagnostics facade, output finalization, signal
  handling, or per-binary migrations.
- No dependency adoption or build-policy change.
- No final Arc04/Arc05/Arc06 detailed plans. This slice may route inputs to
  those arcs but must not pre-write their full plans.
- No Arc03 arc closing report. After this slice is closed and CDC-verified,
  CDC or the operator can close Arc03 at the arc level.

## 3. Required Decisions

The final contract must decide and record, without contradiction:

- what is normative versus informative in the prior slice reports;
- whether the supported surface remains parser normal HDDL parse, grounder
  normal `.htn` grounding, and engine normal search;
- the final command names and compatibility transition for inherited
  `pandaPI*` entry points;
- the final status vocabulary and numeric exit code table;
- how process managers observe outcomes without parsing human diagnostic prose;
- exact stdout/stderr/status stream ownership rules;
- final tagged status format and required fields;
- final help/version/provenance, parse-error, color, TTY, quiet, and
  supervised behavior;
- which fixture/test obligations are normative gates for Arc04 and Arc05;
- how Arc02 choices enter or stay out of implementation;
- what Arc04 must build as shared substrate;
- what Arc05 must migrate per binary;
- what Arc06 must verify and document for release;
- what remains deferred, held, rejected, legacy, experimental, unsupported, or
  future work.

## 4. Constraints

- Do not soften earlier accepted decisions to make synthesis easier. If there
  is a real contradiction, name it and route a required amendment rather than
  silently choosing one source.
- Treat `managed-process-contract.md` as the normative Arc03 output. Earlier
  reports remain evidence, but this synthesis must be the contract Arc04-Arc06
  can implement against.
- Preserve wolong's frozen fetch/install shape and slice04 compatibility
  transition.
- Preserve tagged text status-only output for 0.3.0; do not reopen JSON Lines
  or `nlohmann/json`.
- Preserve the Arc02 dependency portfolio: standard-library modernization as
  baseline; fmt, CLI11, and Catch2 only through accepted gates; `tl::expected`
  and reproc++ as pilots behind explicit facades/conditions; held and rejected
  libraries remain held or rejected.
- Preserve the design-only boundary.

## 5. Verification Approach

CC verifies by producing `managed-process-contract.md`, updating the ledger,
and writing a closing report with a row-by-row walk and Bubble-up to Arc03.
CDC verifies row count, synthesis completeness, contradiction handling,
normative/informative separation, Arc04/Arc05/Arc06 routing, dependency gates,
protected-path cleanliness, and whether Arc03 is ready for arc-level close.

## 6. Exit Criteria

The ledger reaches final status. The final managed-process contract exists,
absorbs all five accepted Arc03 design reports, preserves Arc01/Arc02 inputs,
defines the complete normative 0.3.0 process contract, routes implementation
and release obligations to later arcs, records deferrals and re-entry
conditions, and leaves source/build/release paths untouched.
