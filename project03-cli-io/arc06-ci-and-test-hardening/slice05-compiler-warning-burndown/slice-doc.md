# Arc06 Slice05: compiler-warning-burndown

## Goal

Inventory current C/C++ compiler warnings across parser, grounder, engine,
runtime, generated code, and nested third-party code; fix low-risk primary
hand-written warnings where doing so does not change planner semantics; and
document any remaining warning budget, suppression, or release re-entry
criteria before sanitizer evidence is treated as release-quality.

This slice follows Slice04 because `make static-analysis-cpp` now distinguishes
owned runtime analyzer signal from inherited planner noise. Slice05 applies the
same honesty discipline to compiler warnings: primary chengdu-owned or
maintained fork code should be cleaned up where low-risk, while generated and
third-party warnings must be isolated rather than silently normalized.

## In Scope

- Add a reproducible warning-inventory workflow behind Make if one is needed
  for repeatable capture or review.
- Inventory warnings observed while building `pandapi-parser`,
  `pandapi-grounder`, `pandapi-engine`, and `pandaPI/runtime` on the supported
  local platform.
- Classify warnings by ownership tier:
  primary hand-written parser/grounder/engine/runtime source, generated source,
  nested third-party/dependency-internal source, copied build-tree artifacts,
  and toolchain/platform-only noise.
- Fix low-risk warnings in primary hand-written source when the fix is
  behavior-preserving and covered by existing contract/smoke tests.
- Document warnings that remain, including reason, owner, release risk, and
  re-entry condition.
- Add narrow warning flags, suppression, or budget checks only where the
  ownership classification makes them honest and maintainable.

## Out of Scope

- No algorithm redesign, planner feature changes, optional-surface expansion,
  or semantic cleanup that cannot be covered by existing fixtures.
- No broad formatting churn or generated-source regeneration unless a warning
  fix requires it and the generator path is documented.
- No blanket `-Werror` over generated or nested third-party code.
- No sanitizer, LSan, TSan, coverage, release-package, Arc07 tutorial, or
  Arc08 publication work.
- No adoption-seam static-analysis expansion unless reliable compile commands
  already exist as a byproduct of the warning work.

## Verification Approach

- `make help` must list any new warning target.
- The warning inventory must be reproducible from Make or from a documented
  exact command sequence.
- Primary-source warning fixes must be traceable to specific warning classes
  and must keep parser, grounder, engine, runtime, managed contract, smoke, and
  provenance gates passing.
- Remaining warnings must be classified by ownership tier and must not be
  buried in generic "inherited warnings" prose.
- Any suppression or warning budget must be narrow, documented, and
  grep-verifiable.

## Exit Criteria

- Current warning classes are inventoried and classified.
- Low-risk primary hand-written warning fixes are applied or explicitly
  budgeted with rationale.
- Generated and third-party warnings are isolated with release re-entry
  conditions.
- Existing quality and behavior gates still pass.
- Slice ledger rows close with evidence, and CDC can reproduce the inventory,
  fixes, deferrals, and regression gates.

## Bubble-up Expectation

The closing report must say whether the warning state is clean enough for
Slice06 binary sanitizer gates to produce meaningful release-readiness signal.
It must also identify any warning classes that Arc08 release publication must
mention or gate before `v0.3.0`.
