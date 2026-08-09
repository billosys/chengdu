# chengdu arc02 - cpp-library-research - arc plan

> Plan-of-record for arc02, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc was inserted by
> operator direction on 2026-08-09 so dependency choices are researched before
> the managed-process design becomes load-bearing.

## 1. Capability statement

Roadmap line: *Research open source C++ libraries against the parser, grounder,
and engine audit findings; produce per-component assessments and a final
combined dependency recommendation set.*

Expanded: this arc turns audit findings into library decisions. It starts with
the accepted parser audit, then repeats the same workflow for the grounder and
engine once their audit reports land. The final slice compares all three
component reports and recommends which open source C++ libraries chengdu should
adopt, pilot, hold, or reject for 0.3.0 and later.

The intent is not to add fashionable dependencies. The intent is to stop
rebuilding infrastructure poorly when small, battle-tested C++ libraries can
retire real defect classes: CLI parsing, status propagation, diagnostic
formatting, JSON/event output, tests, RAII/resource wrappers, and possibly
parser-generation strategy.

## 2. Reusable workflow

Use this workflow for every component-level research slice.

1. **Read the audit as the problem statement.** Extract concrete defect classes,
   exact finding IDs, source locations, and clean checks. Do not start from a
   library wish list.
2. **Map defect classes to capability buckets.** Typical buckets are CLI
   parsing/help/version, error/status propagation, diagnostics/logging/color,
   filesystem/path/resource handling, structured event output, tests/fixtures,
   parser/lexer technology, containers/ownership, and build/tooling.
3. **Apply the standard-library-first screen.** Prefer C++17 standard library
   facilities where they materially solve the defect class: `std::filesystem`,
   `std::unique_ptr`, `std::string_view`, `std::vector`, `std::array`,
   `std::optional`, `std::variant`, `std::error_code`, and stream/file RAII.
   Reach for third-party libraries only when they replace substantial
   infrastructure or establish a cross-component contract.
4. **Generate candidates from the bucket, not the brand.** For each bucket,
   include at least one "no new dependency" option and one credible external
   option when such a library exists.
5. **Collect current candidate evidence.** Record role, license, C++ standard,
   integration modes, platform support, package availability, latest visible
   release or maintenance signal, transitive dependencies, security/update
   surface, and documentation quality.
6. **Score against chengdu constraints.** Use a 0-3 score for each axis:
   audit fit, maintainability impact, runtime/performance impact, license and
   release fit, build/packaging fit, migration risk, and cross-component reuse.
7. **Assign a disposition.** Use:
   - `adopt` when the library is a good fit for a near-term implementation
     slice.
   - `pilot` when the fit is promising but needs a contained spike.
   - `hold` when it may be valuable after 0.3.0 or after other audit evidence.
   - `reject` when the candidate does not solve enough real chengdu pain or
     carries unacceptable cost.
8. **Record non-adoptions explicitly.** A rejected parser generator or logging
   framework is useful evidence. Future sessions should see why it was not
   chosen.
9. **Bubble up cross-component questions.** Anything that cannot be decided from
   one component becomes an input to the final combined recommendation slice.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `parser-library-research` | Define the reusable workflow and assess C++ library candidates against `pandaPIparser` audit findings. | slice04 combined recommendations; Arc03 design inputs |
| slice02 | `grounder-library-research` | Apply the same workflow to the grounder audit report once accepted. | slice04 combined recommendations; Arc03/Arc04 dependency choices |
| slice03 | `engine-library-research` | Apply the same workflow to the engine audit report once accepted. | slice04 combined recommendations; Arc03/Arc05 dependency choices |
| slice04 | `combined-library-recommendations` | Compare all three component reports and produce final open source C++ library recommendations for 0.3.0 and later. | Arc03 managed-process contract; Arc04 shared runtime substrate |

## 4. Dependencies

**Consumes:** Arc01 parser/grounder/engine audit reports; the C++ Core
Guidelines substrate exposed through `cpp-guidelines`; current upstream
candidate documentation/release metadata; local build/release constraints under
`scripts/`, `.github/`, and `pandaPI/`; wolong frozen fetch-spec constraint.

**Leaves for arc03:** a recommended dependency portfolio, rejected-candidate
rationales, library-specific migration risks, and explicit questions that must
be resolved before writing the managed-process design.

## 4.1 Current status

- **slice01 parser-library-research - attested.** Report:
  [`slice01-parser-library-research/closing-report.md`](slice01-parser-library-research/closing-report.md).
- **slice02 grounder-library-research - attested.** Report:
  [`slice02-grounder-library-research/closing-report.md`](slice02-grounder-library-research/closing-report.md).
- **slice03 engine-library-research - attested.** Report:
  [`slice03-engine-library-research/closing-report.md`](slice03-engine-library-research/closing-report.md).
- **slice04 combined-library-recommendations - closed and CDC-verified.**
  Verification:
  [`slice04-combined-library-recommendations/cdc-verification.md`](slice04-combined-library-recommendations/cdc-verification.md).
  Accepted report:
  [`combined-library-recommendations.md`](combined-library-recommendations.md).

## 5. Candidate scoring rubric

| Score | Meaning |
|-------|---------|
| 3 | Strong fit: directly retires a real defect class with low adoption cost. |
| 2 | Plausible fit: useful, but needs a pilot or cross-component confirmation. |
| 1 | Weak fit: solves only narrow pain or carries notable cost. |
| 0 | Bad fit: does not solve the audit problem or violates a constraint. |

Final disposition should not be a raw sum alone. A library with a perfect
feature score but unacceptable license, release-packaging risk, or migration
blast radius is not adopted.

## 6. Arc ledger

Composition rows verifying the capability; open here and close per-row in this
arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | The reusable workflow is recorded in this arc plan and is concrete enough for a future session to repeat against grounder and engine audits without inventing new criteria. | reproduced |
| A2 | Parser, grounder, and engine each receive a component-level library-research slice that maps audit findings to candidate libraries and dispositions. | reproduced |
| A3 | Every adopted or piloted candidate records license, current maintenance/release evidence, supported integration mode, build/packaging impact, and the audit defect classes it addresses. | reproduced |
| A4 | The final combined recommendation report compares all three components and distinguishes 0.3.0 adoption candidates from later research or rejected options. | reproduced |
| A5 | Arc03 can be planned from the combined recommendation report without silent drops: every accepted dependency choice is either adopted into a later design/implementation slice, deferred with re-entry condition, or rejected/no-op with rationale. | reproduced |

## 7. Open questions and risks

- **OQ1 - dependency budget.** This arc may recommend more useful libraries than
  0.3.0 should absorb. The final report must separate "adopt for 0.3.0" from
  "pilot later".
- **OQ2 - build-system transition.** Many candidates are easiest with CMake,
  Conan, vcpkg, or package-manager installs, while pandaPI components currently
  build through makefiles and chengdu shell wrappers. Each recommendation must
  name the integration path it assumes.
- **OQ3 - parser rewrite temptation.** Parser-generator replacement may be
  valuable, but it is a larger semantic migration than adopting CLI, formatting,
  result, or test libraries. Do not recommend it for 0.3.0 unless evidence from
  all three audits makes the migration cost justified.
- **OQ4 - license and vendoring.** chengdu is public and ships binaries.
  Dependency licenses, notices, source provenance, and release-asset packaging
  are part of the recommendation, not a later clerical step.

## 8. Version history

- **v1.4 - 2026-08-09.** Updated slice04 status after the combined library
  recommendations closed and CDC verification accepted it. Surfaced by:
  slice04 CDC verification. Why: Arc03 can now plan from the accepted
  dependency portfolio, while the arc close still needs an explicit decision
  on whether the attested component-level research slices require independent
  CDC verification.
- **v1.3 - 2026-08-09.** Opened slice04 combined-library-recommendations with
  explicit architecture-impact and supported-surface criteria. Surfaced by:
  operator guidance after the engine library-research slice landed. Why: final
  dependency choices need to be sequenced with Arc03/Arc04 architecture, not
  treated as isolated package picks.
- **v1.2 - 2026-08-09.** Added slice03 engine-library-research status after
  applying the reusable workflow to the accepted engine audit. Surfaced by:
  operator direction after `audit-results-pandapi-engine.md` landed. Why:
  engine evidence completes the component-level library research set and adds
  supported-surface containment for translation/SAT/BDD/CUDD as a final-report
  input.
- **v1.1 - 2026-08-09.** Added slice02 grounder-library-research status after
  applying the reusable workflow to the accepted grounder audit. Surfaced by:
  operator direction after `audit-results-pandapi-grounder.md` landed. Why:
  grounder evidence strengthens shared CLI/status/stdio/test candidates and
  adds dependency-process containment as a research input.
- **v1.0 - 2026-08-09.** Initial arc plan. Source: operator direction to create
  a new arc for open source C++ library research after the parser audit and
  before the managed-process design.
