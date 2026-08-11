# Arc06 Slice02: process-fixture-expansion

## Goal

Expand the black-box process fixture workload from strong per-component
managed-contract coverage into the first representative Arc06 integration
workload: managed pipeline composition, pairwise artifact composition,
positive supervised invocation, and any deterministic output-finalization
failure coverage that can be proved portably.

This slice should make later coverage and sanitizer gates meaningful by giving
them more than minimal component fixtures to execute.

## Scope

In scope:

- Add managed pipeline fixture support under the Make public interface.
- Add a managed parse-ground-solve pipeline fixture using canonical
  `pandapi-*` commands and `--status=stderr`.
- Add pairwise artifact-composition fixtures for parser stdout captured into
  grounder input and grounder stdout captured into engine input, where the CLI
  surface supports the composition without inventing stdin behavior.
- Add positive `--supervised` fixtures for parser, grounder, and engine.
- Add deterministic output-finalization failure fixtures if the harness can
  prove them portably; otherwise record a precise deferral and re-entry
  condition.
- Update fixture records and inventory/routing notes so Slice03 coverage knows
  what workload it can rely on.

Out of scope:

- No parser, grounder, or engine product behavior changes except for narrowly
  justified testability repairs required to expose already-accepted behavior.
- No new supported optional surfaces.
- No coverage, `clang-tidy`, Clang Static Analyzer, sanitizer, LSan, or TSan
  gate implementation yet.
- No release package, checksum, manifest, wolong, README, tutorial, or public
  migration edits.
- No direct CI workflow calls to `tests/contract/run`; add or use Make targets.

## Verification Approach

Verification must run through Make targets:

- Managed component fixture targets still pass.
- A managed pipeline fixture target exists and passes.
- Pairwise and supervised fixture cases are listable and selectable by the
  public Make/harness surface.
- Fixture records document the new cases and their comparison policy.
- If output-finalization failure cannot be added portably, the deferral is
  documented in the ledger and closing report with a re-entry condition.

## Exit Criteria

- Arc06 has managed pipeline and representative process-composition evidence.
- The fixture workload is broad enough for Slice03 coverage and Slice05
  sanitizer gates to run against more than per-component minimal paths.
- All new process-fixture behavior is wired through Make-backed local and CI
  surfaces.
- Existing contract, smoke, quality, and provenance gates still pass.

## Handoff

At close, the slice closing report must include a **Bubble-up to the arc**
section that states whether the expanded workload is representative enough for
Slice03 coverage work, and names any fixture gaps that remain for Slice05 or
Slice06.
