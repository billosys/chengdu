# Arc06 Slice04: static-analysis-gate

## Goal

Add or graduate C++ static-analysis gates for owned runtime/process-policy code
where compile databases are reliable, with Make-backed entrypoints and a
written ownership/suppression policy.

This slice starts from Slice03's runtime coverage build evidence and the normal
runtime `compile_commands.json`. Its purpose is to turn C++ static analysis
from a future intention into a concrete, reproducible local gate without
pretending inherited planner, generated, or third-party source has the same
ownership profile as chengdu-owned runtime code.

## In Scope

- Add Make target(s) for C++ static analysis, listed in `make help`.
- Prefer `clang-tidy` for the first gate over owned runtime source and tests
  under `pandaPI/runtime/`, using a reliable runtime compile database.
- Add Clang Static Analyzer support only if it can run reproducibly in this
  slice without duplicating large inherited build volume or creating a
  platform-only illusion.
- Define which paths are included, excluded, or deferred:
  `pandaPI/runtime/` is the required first scope; `pandapi_*_native.cpp`
  adoption seams may be included only if their compile commands are reliable;
  inherited planner, generated, and nested third-party code must be excluded or
  separately classified.
- Document tool resolution, missing-tool behavior, suppression policy, and
  warning ownership.
- Keep new CI behavior Make-backed. If a workflow is changed, it must call the
  Make target rather than `clang-tidy` or analyzer commands directly.

## Out of Scope

- No compiler-warning burndown; that is Slice05.
- No sanitizer, LSan, TSan, release package, README migration prose, Arc07
  tutorial work, or Arc08 publication work.
- No product behavior changes in parser, grounder, or engine.
- No blanket inherited-source cleanup, bulk formatting, or third-party
  suppression churn.
- No hard "zero findings forever" promise without an explicit owned-code
  baseline and suppression policy.

## Verification Approach

- `make help` must expose the C++ static-analysis entrypoint(s).
- The gate must either pass locally or fail/skip explicitly with a documented
  missing-tool/platform reason and re-entry condition.
- Path policy must be grep-verifiable and must distinguish owned runtime code
  from inherited/generated/third-party code.
- If suppressions are added, they must be narrow, justified, and documented.
- Existing Make quality and behavior gates must still pass.

## Exit Criteria

- A contributor can discover and run the static-analysis target from Make.
- Owned runtime C++ source is analyzed through a reliable compile database or a
  missing-tool/platform deferral is explicitly recorded.
- Any adoption-seam inclusion or deferral is documented with a re-entry
  condition.
- CI wiring, if added, calls Make targets only.
- Slice ledger rows close with evidence, and CDC can reproduce the target or
  the explicit deferral plus the existing regression gates.

## Bubble-up Expectation

The closing report must say whether the static-analysis signal is strong enough
to inform Slice05 compiler-warning burndown and Slice06 sanitizer ownership. It
must also route any analyzer/toolchain gaps so Arc06 can close without
overclaiming static-analysis coverage.
