# CC assignment - chengdu arc02 / slice02 - engine-exit-status

You are CC, the implementing context for one source-changing slice, working in
the `chengdu` repo (`~/lab/billosys/chengdu`) on `release/0.2.x`. This file is
your assignment; read it fully before writing anything.

## Read order (before any work)

1. `/AGENTS.md` - standing project contracts, release policy, commit footer
   convention, and wolong fetch-spec constraint.
2. `docs/design-v0.2.0/project-plan.md` - 0.2.0 DoD, Arc02 boundary, and
   project ledger P3/P5.
3. `docs/design-v0.2.0/arc02-upstream-absorption/arc-plan.md` - current
   absorption queue and Arc02 ledger.
4. `docs/design-v0.2.0/arc02-upstream-absorption/slice01-upstream-triage/closing-report.md`
   and `docs/design-v0.2.0/arc02-upstream-absorption/slice01-upstream-triage/cdc-verification.md`
   - the verified triage basis for this slice.
5. `docs/design-v0.2.0/arc02-upstream-absorption/slice02-engine-exit-status/slice-doc.md`
   - goal, scope, constraints, upstream snapshot, and exit criteria.
6. `docs/design-v0.2.0/arc02-upstream-absorption/slice02-engine-exit-status/ledger.md`
   - the specification of done. Fourteen rows; every one reaches final status
   before this slice advances. Follow the CC protocol in the collaboration
   framework's `LEDGER-DISCIPLINE.md` Section A.

## The work

Absorb pandaPIengine PR
[#14](https://github.com/panda-planner-dev/pandaPIengine/pull/14) and issue
[#12](https://github.com/panda-planner-dev/pandaPIengine/issues/12) as a
bounded exit-status change:

- Refresh upstream PR #14 and issue #12 at the start of implementation. Record
  state, head SHA, author/commit metadata, issue comments, and applicability
  result in the ledger/closing report. Do not rely only on the slice-open
  snapshot if GitHub is reachable.
- Try the project-preferred absorption path first in reversible state:
  `git cherry-pick -Xsubtree=pandaPI/pandaPIengine ...` or an equivalent PR
  diff dry-run. If conflicts remain, perform a manual bounded port of the PR's
  functional behavior and disclose that choice.
- Port behavior, not formatting churn. PR #14 is known to conflict in:
  - `pandaPI/pandaPIengine/src/SearchEngine.cpp`
  - `pandaPI/pandaPIengine/src/sat/sat_planner.cpp`
  - `pandaPI/pandaPIengine/src/sat/sat_planner.h`
- Make process exit status reflect search outcome:
  - solved / successful search exits `0`;
  - search failure / proven-unsolvable exits `2`;
  - malformed input, usage/configuration errors, and internal errors remain
    nonzero and are not collapsed into success.
- Preserve SAT planner return plumbing clearly enough that the engine builds
  and issue #12's maintainer guidance is not lost: SAT solver returns 10/20
  are the ordinary solver states; other states are errors unless a termination
  mode is explicitly in play.
- Update `scripts/smoke-test.sh --negative` so the unsolvable fixture expects
  exit `2` plus `Status: Proven unsolvable`, and reports it as
  `UNSOLVABLE`, not success and not generic failure.
- Keep positive smoke unchanged in meaning: the minimal fixture solves with
  engine exit `0`, converts, and verifies true.
- Update active README/docs text that describes the current unsolvable engine
  behavior if it still says exit `0`. Preserve historical docs under
  `docs/design-v0.1.0/` and 0.3.0 planning notes as historical records.
- Close the ledger and write `closing-report.md` with a row-by-row walk and a
  Bubble-up to the arc/project.

## Credit and commits

Preserve upstream credit:

- If a PR commit can be cherry-picked cleanly with native author metadata, use
  that path.
- If conflicts require a manual port, credit Robert P. Goldman
  `<rpgoldman@sift.net>` in the implementation commit body/trailers and include:
  - `Upstream-PR: https://github.com/panda-planner-dev/pandaPIengine/pull/14`
  - `Upstream-Issue: https://github.com/panda-planner-dev/pandaPIengine/issues/12`
- Assistant-authored commits must also include:
  - `Co-authored-by: Codex <noreply@openai.com>`
  - `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

A clean split is preferred if it stays natural: one source/smoke implementation
commit, then one docs-close commit.

## Constraints

- No parser PR #21 work; that is slice03.
- No engine PR #9 SAT verifier, engine PR #15 wrapper script, grounder PR #7
  Docker work, grounder issue #8, parser expansion/resource fixes, release
  publication, or upstream courtesy filing.
- No 0.3.0 CLI/API/status redesign. Do not add machine-readable status output,
  a new wolong API, stdout/stderr restructuring, or a general outcome enum.
- Do not revive upstream source fetches, active patch files, or clone paths.
- Do not change wolong's frozen release asset shape.
- Keep shell script changes bash-3.2 compatible.
- Do not write `cdc-verification.md`; CDC writes that after independent
  verification.

## Verification commands

Run the strongest practical local gate and record exact results:

```bash
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/check-provenance.sh
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
./scripts/package-release.sh v0.2.0-slice02-smoke
bash -n scripts/*.sh
/bin/bash -n scripts/*.sh
shellcheck scripts/*.sh
actionlint .github/workflows/*.yml
git diff --check
git status --short --branch
```

If a local tool is unavailable, record the exact missing tool/version issue and
which later CDC/CI gate must cover it. Do not mark a row done silently.

## Definition of done

All 14 ledger rows at final status with concrete evidence; engine search
success exits `0`; search failure / proven-unsolvable exits `2`; positive and
negative smoke pass; provenance and packaging still pass; source diff is
bounded to the engine exit-status work; commits credit PR #14 / issue #12 and
Robert P. Goldman; worktree is clean; `closing-report.md` exists; no
`cdc-verification.md` is written by CC.
