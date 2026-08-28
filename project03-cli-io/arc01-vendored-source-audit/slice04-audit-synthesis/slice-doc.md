# chengdu arc01 / slice04 - audit-synthesis - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Synthesize the parser, grounder, and engine audit reports into one
cross-codebase recommendation document:

`docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`

The synthesis must identify shared defect classes, duplicate-code/shared-code
opportunities, architecture options, implementation sequencing, and the
specific inputs Arc02 and Arc03 need before dependency and managed-process
decisions become load-bearing.

## 2. Scope

**In:**

- Accepted Arc01 audit reports for parser, grounder, and engine.
- CDC verification reports for all three audits.
- Cross-codebase comparison of error/status handling, stdout/stderr and
  TTY/color policy, CLI naming/help/version behavior, resource/signal/timeout
  behavior, build/test posture, ownership/lifetime risks, generated-code
  posture, and dependency-boundary risks.
- Shared runtime/header/library candidates that appear in two or more
  components.
- Architecture options ranging from conservative contract wrappers through a
  larger shared C++ runtime substrate, with explicit costs, risks, and
  re-entry conditions.
- Inputs to Arc02 combined library recommendations and Arc03 managed-process
  contract design.

**Out:**

- No planner source changes.
- No dependency vendoring or build-system rewrites.
- No final library adoption decision; Arc02 slice04 owns the combined
  dependency portfolio.
- No final managed-process contract; Arc03 owns the accepted design.

## 3. Constraints

- Treat all three accepted audit reports as evidence, not as prose to summarize
  loosely. Cross-codebase claims must cite audit finding IDs and report
  sections.
- Be willing to recommend a large architectural move if it is the clearest path
  to a maintainable 0.3.0, but make the cost and sequencing explicit.
- Distinguish algorithmic planner internals from process-contract/runtime
  substrate. The synthesis may recommend boundaries before refactors.
- Preserve wolong's frozen fetch/install contract unless a later operator
  decision explicitly accepts a breaking transition.
- Keep optional paths explicit: parser verifier/output helpers, grounder H2/
  cpddl integration, engine translation/interactive/SAT/BDD/CUDD paths must not
  silently become part of the 0.3.0 supported process contract.

## 4. Verification Approach

CC verifies by producing the synthesis report, updating this slice ledger with
finding/report evidence, and writing a closing report with a row-by-row walk
plus bubble-up to Arc01. CDC verifies by checking row count, cited audit IDs,
scope coverage, protected-path cleanliness, and whether the synthesis provides
actionable inputs for Arc02 and Arc03.

## 5. Exit Criteria

The ledger reaches final status. The synthesis report exists at the planned
path, compares all three audits, identifies shared and component-specific
recommendations, includes architecture options with tradeoffs, names
implementation sequencing candidates, and leaves source/build/release paths
untouched.
