# CDC Verification: Arc07 Slice03 first-party-source-naming-normalization

Status: CDC verified closed
Date: 2026-08-12
Verified commit: `e584a64f3b3ce46001780260c6702694b83fc8f6`

## Verdict

Slice03 is verified closed. The implementation normalizes first-party
maintained parser, grounder, and engine source file and directory basenames to
the accepted lower snake case policy, adds Make-backed naming report/check
targets, and keeps generated, dependency-internal, third-party, copied-build,
and build-output paths outside the naming scope.

CDC accepts CC's proposed-done closure with no amendment. Slice04
`first-party-quality-gate-scaffold` should open next.

## Scope Review

The verified commit changed the expected Slice03 surfaces:

- Make-backed source-quality naming target plumbing in `mk/` and
  `tools/source-quality/surface`.
- Durable Arc07 naming report:
  `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`.
- First-party parser, grounder, and engine source path renames plus required
  include/build reference updates.
- Arc07 source-classification references that named renamed files.
- Slice03 ledger and CC closing report.

No CI workflow, public README/tutorial, release/package/publication, Arc08, or
Arc09 surface changed. Excluded dependency roots were untouched.

## Reproduced Commands

Naming and source-quality evidence:

- `make help`
- `make source-quality-naming-report`
- `make source-quality-naming-check`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-first-party`

Component evidence:

- `make build-parser`
- `make test-contract-parser-managed`
- `make build-grounder`
- `make test-contract-grounder-managed`
- `make build-engine`
- `make test-contract-engine-managed`

Regression evidence:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make build`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Boundary and ledger evidence:

- `git log -1 --format='%H%n%s%n%b'`
- `git show --check e584a64f`
- `git show --summary --find-renames e584a64f`
- `git show --summary --find-renames e584a64f | rg 'delete mode|create mode' | rg 'pandaPI/(parser|grounder|engine|runtime)'`
- `git diff-tree --no-commit-id --name-only -r e584a64f -- pandaPI/grounder/cpddl pandaPI/grounder/h2-fd-preprocessor pandaPI/engine/src/symbolic_search/cudd-3.0.0`
- `git diff-tree --no-commit-id --name-only -r e584a64f -- .github README.md release tools/release docs/design-v0.3.0/arc08-pandapi-tutorial-docs docs/design-v0.3.0/arc09-release-prep-publication`
- `git show --format= --unified=0 -G 'coverage floor|release blocker coverage|fail.*warning|fail.*coverage|supported public|wolong|GitHub Actions' e584a64f -- Makefile mk tools`
- `rg -n -- 'F-1|F-2|F-3|F-4|F-5|F-6|F-7|F-8|F-9|F-10|F-11|F-12|Bubble-up to Arc07|Slice04|remediation|split' docs/design-v0.3.0/arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/closing-report.md`

`make check` was not rerun as a single aggregate target during CDC
verification because the constituent gates were reproduced directly, including
`make build`, `make test`, and `make provenance-check`. This preserves the same
behavior, static, format, safety, actionlint, provenance, and whitespace
evidence while avoiding extra noise from the known macOS aggregate provenance
race.

## Observed Results

- `make source-quality-naming-check` passed with zero naming violations.
- Managed parser contract fixtures passed: `298 passed, 0 failed`.
- Managed grounder contract fixtures passed: `269 passed, 0 failed`.
- Managed engine contract fixtures passed: `312 passed, 0 failed`.
- `make test` passed, including managed pipeline fixtures
  `129 passed, 0 failed`, positive smoke `3 passed, 0 failed`, and negative
  smoke `4 passed, 0 failed`.
- `make provenance-check` passed for all three components.
- `git diff --check` and `git diff --cached --check` passed.
- Final pre-verification worktree was clean.

Expected inherited warning-budget items remain visible during C/C++ builds,
including generated/dependency warnings and the existing engine
`visited_list.cpp` payload cast warning. CDC does not treat those as Slice03
regressions.

## Ledger Verification

| Row | CDC status | Evidence |
|-----|------------|----------|
| F-1 | verified | `make help` lists `source-quality-naming-report` and `source-quality-naming-check`; `mk/source-quality.mk`, `mk/help.mk`, and `tools/source-quality/surface` contain the target/helper wiring. |
| F-2 | verified | The durable naming report exists and contains the required policy, exclusion, and rename-map terms. |
| F-3 | verified | `git show --summary --find-renames e584a64f` records parser, grounder, and engine source renames; no first-party source-root create/delete churn was found. |
| F-4 | verified | `make source-quality-naming-check` passed. |
| F-5 | verified | `make build-parser` and `make test-contract-parser-managed` passed; parser managed fixtures reported `298 passed, 0 failed`. |
| F-6 | verified | `make build-grounder` and `make test-contract-grounder-managed` passed; grounder managed fixtures reported `269 passed, 0 failed`. |
| F-7 | verified | `make build-engine` and `make test-contract-engine-managed` passed; engine managed fixtures reported `312 passed, 0 failed`. |
| F-8 | verified | Dependency exclusion roots had no changes, and source-quality evidence still records generated, third-party, dependency, copied-build, and build-output classes. |
| F-9 | verified | `make source-quality-surface`, `make source-quality-profile-map`, and `make compile-db-first-party` passed after the rename set. |
| F-10 | verified | Safety, actionlint, static analysis, format, build, test, provenance, and whitespace checks passed. |
| F-11 | verified | Post-commit boundary greps found no CI workflow, public README/tutorial, release/package/publication, Arc08, Arc09, quality-threshold, or public-behavior expansion changes. |
| F-12 | verified | CC's closing report walks all rows and bubbles up Slice04 as the next work with no required split/remediation. |

## Bubble-up

Arc07 can treat Slice03 as closed and CDC-verified. The first-party maintained
C/C++ naming surface is now Make-checkable and clean, with parser, grounder,
and engine build/contract evidence reproduced after the renames.

Slice04 `first-party-quality-gate-scaffold` should open next. It should build
on the Slice02 selectors/profile maps and the Slice03 naming gate, while still
avoiding premature coverage floors, warning failure budgets, README/tutorial
changes, release/publication work, and public behavior changes unless its own
ledger explicitly accepts them.
