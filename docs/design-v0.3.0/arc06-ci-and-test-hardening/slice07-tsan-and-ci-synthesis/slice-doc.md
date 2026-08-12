# Arc06 Slice07: tsan-and-ci-synthesis

## Goal

Decide whether ThreadSanitizer can provide meaningful evidence for the current
chengdu workload, add a Make-backed TSan gate only if it exercises real
concurrency or process-observation risk, and synthesize the final Arc06
local/CI hardening evidence for Arc07 documentation and Arc08 release
preparation.

This is the final planned Arc06 slice. Its job is not to add another expensive
symbolic gate. TSan only finds races that the workload actually exercises, so
the slice must either wire a meaningful gate or explicitly defer TSan with a
concrete workload/toolchain re-entry condition.

## In Scope

- Review current concurrency/process-observation surfaces:
  runtime helpers, managed fixture supervision, shell harness behavior, parser,
  grounder, engine, and nested dependency execution paths.
- Decide whether TSan has a representative workload today.
- If meaningful, add Make-backed TSan target(s), listed in `make help`, and run
  them against the selected workload.
- If not meaningful, document the exact reason, owner, and re-entry condition;
  a clear deferral is acceptable.
- Decide CI placement for heavy gates already produced by Arc06:
  coverage, static analysis, warning inventory, runtime sanitizer, binary
  sanitizer, and any TSan disposition.
- Update the Arc06 fixture/gate inventory so local, always-on CI, scheduled CI,
  pre-release, and deferred gates are explicit.
- Prepare final Arc06 synthesis notes for Arc07 and Arc08: behavior examples,
  warning/sanitizer/static/coverage disposition, Linux LSan re-entry, and
  release-risk gates.

## Out of Scope

- No Arc07 tutorial, README, behavior-change table, or public documentation
  prose beyond handoff inputs.
- No Arc08 release packaging, checksums, license/NOTICE, wolong migration, or
  publication work.
- No broad planner algorithm redesign or dependency audit.
- No optional-surface expansion for H2, cpddl/FAM, SAT, BDD/CUDD,
  translation, or interactive behavior.
- No attempt to combine TSan with ASan/UBSan; TSan must be a separate gate if
  added.

## Verification Approach

- `make help` must list any new TSan target, or the docs must explicitly say no
  TSan target was added and why.
- Any TSan target must run through Make, not direct scripts or ad hoc compiler
  commands in CI.
- The chosen TSan disposition must reference the workload it does or does not
  exercise.
- Existing Arc06 gates must still pass or be explicitly routed with evidence.
- The fixture/gate inventory and Arc06 closing handoff must identify which
  gates are local-only, always-on CI, scheduled CI, pre-release, or deferred.
- CI/GitHub Actions changes, if any, must invoke Make targets only and pass
  workflow safety checks plus `make actionlint`.

## Exit Criteria

- TSan is added as a meaningful Make-backed gate or explicitly deferred with a
  concrete re-entry condition.
- CI placement for coverage, static analysis, warning inventory, runtime
  sanitizer, binary sanitizer, and TSan is documented.
- Arc06's final evidence is synthesized for Arc07 and Arc08 without claiming
  release publication is complete.
- Slice ledger rows close with evidence, and CDC can reproduce the TSan
  disposition, CI synthesis, boundary checks, and regression gates.

## Bubble-up Expectation

The closing report must say whether Arc06 can close. If yes, it must identify
the exact Arc07 documentation inputs and Arc08 release-prep gates that remain.
If no, it must name the missing Arc06 gate or evidence row and provide the next
slice or iteration prompt.
