# chengdu arc02 / slice04 - combined-library-recommendations - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Compare the parser, grounder, and engine library-research reports and produce
the final Arc02 recommendation portfolio:

`docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

The portfolio must distinguish libraries to adopt for 0.3.0, candidates to
pilot, candidates to hold for later, and candidates rejected for this release,
while making the architecture consequences of each choice explicit.

## 2. Scope

**In:**

- Arc02 slice01, slice02, and slice03 closing reports.
- Accepted Arc01 audit reports and CDC verification reports as the defect
  source for library decisions.
- Current upstream source, release, license, integration, and maintenance
  evidence for adopt/pilot candidates.
- Standard-library-first baseline decisions.
- Cross-component recommendation matrix for CLI11, fmt, tl::expected, Catch2,
  nlohmann/json, reproc++, Boost.Process, Microsoft GSL, Abseil, performance
  containers, google/benchmark, parser generators, and optional CUDD/BDD/SAT
  paths.
- Architecture impact: dependency footprint, build-system implications,
  shared runtime substrate design pressure, release packaging, NOTICE/license
  work, and migration sequencing.

**Out:**

- No dependency vendoring.
- No planner source changes.
- No build-system implementation.
- No final managed-process contract text; Arc03 owns the contract design.

## 3. Constraints

- Refresh current public metadata for adopt/pilot candidates before making a
  final recommendation. Do not trust older per-component research blindly.
- Prefer the standard library where it solves the defect class cleanly.
- A big architectural recommendation is allowed when it has the best evidence,
  but it must include sequencing, blast radius, rollback/defer conditions, and
  which later arc owns each step.
- Do not let library availability decide supported product surface. Arc03 must
  classify translation, interactive, SAT, BDD, CUDD, H2, and cpddl boundaries
  before optional-path dependencies become design commitments.
- Every adoption must name license/NOTICE impact and release-packaging impact.

## 4. Verification Approach

CC verifies by producing the combined recommendation report, recording current
source evidence, updating the ledger, and writing a closing report. CDC verifies
candidate metadata, source links, row count, disposition coverage, and
protected-path cleanliness before the arc can close.

## 5. Exit Criteria

The ledger reaches final status. The combined recommendation report exists,
maps every adopt/pilot/hold/reject decision to audit findings and current
library evidence, identifies architecture consequences, and leaves source,
build, release, and dependency-vendoring paths untouched.
