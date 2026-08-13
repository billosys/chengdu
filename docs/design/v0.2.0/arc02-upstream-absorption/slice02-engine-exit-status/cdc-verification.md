# CDC verification - Arc02 Slice02 engine-exit-status

Verified by CDC on 2026-08-08 for `release/0.2.x`.

Verdict: **Accepted.** Slice02 is independently verified. The engine exit-status
absorption is narrow, credited, and reproduced by local macos-arm64 gates.

## Scope Checked

CDC verified CC's close from slice-open commit `12533ac0` through
`1ea60387`:

- `4da38819` - `engine: propagate search exit status`
- `1ea60387` - `docs: close engine exit-status slice`

The implementation diff is limited to seven files:

```text
pandaPI/pandaPIengine/src/SearchEngine.cpp
pandaPI/pandaPIengine/src/sat/sat_planner.cpp
pandaPI/pandaPIengine/src/sat/sat_planner.h
pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h
pandaPI/pandaPIengine/src/translation/translationController.cpp
pandaPI/pandaPIengine/src/translation/translationController.h
scripts/smoke-test.sh
```

Diff size for the implementation commit: 28 insertions, 23 deletions. No parser
PR #21 work, grounder work, release publication, wolong release-shape change,
or 0.3.0 status/API redesign was present.

## Reproduced Gates

Commands run at HEAD `1ea60387` on macos-arm64:

| Gate | CDC result |
|------|------------|
| `./scripts/build-parser.sh` | reproduced: exit 0; existing compiler warnings only |
| `./scripts/build-grounder.sh` | reproduced: exit 0; existing third-party/compiler warnings only |
| `./scripts/build-engine.sh` | reproduced: exit 0; Slice02 engine source compiled |
| `./scripts/check-provenance.sh` | reproduced: exit 0; all three components verified against `vendor.env` and Git state |
| `./scripts/smoke-test.sh` | reproduced: exit 0; 5 passed, 0 failed; solve gate `exit 0, Status: Solved` |
| `./scripts/smoke-test.sh --negative` | reproduced: exit 0; 4 passed, 0 failed; unsolvable gate `engine exit 2, Status: Proven unsolvable` |
| `./scripts/package-release.sh v0.2.0-slice02-smoke` | reproduced: exit 0; macos-arm64 dry-run package, manifest, checksums, licenses, and notes built |
| `bash -n scripts/*.sh` | reproduced: exit 0 |
| `/bin/bash -n scripts/*.sh` | reproduced: exit 0 |
| `shellcheck scripts/*.sh` | reproduced: exit 0 |
| `actionlint .github/workflows/*.yml` | reproduced: exit 0 |
| `git diff --check` | reproduced: exit 0 |

CDC also reproduced the upstream evidence row with GitHub API calls:

- PR #14 remains open, non-draft, `mergeable=false`,
  `mergeable_state=dirty`, head
  `304048392e2b3ee53aca7f93d88b5a35230f638b`, 4 commits, 7 files,
  +387/-376.
- All PR #14 commits are authored by Robert P. Goldman
  `<rpgoldman@sift.net>`.
- Issue #12 remains open and still records the intended search-failure exit
  code `2`; comments still tie the work to PR #14 and preserve the SAT solver
  10/20 ordinary-state guidance.

## Ledger Verification

| Row | CDC disposition |
|-----|-----------------|
| F-1 | reproduced: branch is `release/0.2.x`; Slice01 close and CDC verification files exist; tracked worktree was clean before this CDC file. |
| F-2 | reproduced: upstream PR #14 / issue #12 metadata and authorship matched CC's report. |
| F-3 | reproduced: PR is still dirty; implementation commit is a small manual port and does not carry generated `.gitignore` or formatting churn. |
| F-4 | reproduced: source inspection shows `main` returns `searchRes`; progression solved returns `0`; timeout/proven-unsolvable defaults to `2`; smoke tests prove both solved and unsolvable outcomes. |
| F-5 | reproduced with caveat: SAT return signatures compile and source inspection confirms `solve_with_sat_planner` now returns `int`; non-10/20 solver states remain on existing `exit(2)` paths. Normal release smoke does not exercise SAT. |
| F-6 | reproduced: negative smoke expects and receives exit `2` plus `Status: Proven unsolvable`, reported as `UNSOLVABLE`. |
| F-7 | reproduced: positive smoke still solves with exit `0` and verifies true. |
| F-8 | reproduced: parser, grounder, engine, provenance, positive smoke, and negative smoke all passed at current HEAD. |
| F-9 | reproduced: package dry-run passed without publishing a release. |
| F-10 | reproduced: bash syntax, shellcheck, actionlint, and diff hygiene passed. |
| F-11 | reproduced: changed files are limited to engine exit-status source, smoke harness, and slice close docs. |
| F-12 | reproduced: implementation commit includes Robert P. Goldman credit, upstream PR/issue URLs, and required Codex/Billo co-author trailers. Native author preservation was not possible because the PR did not apply cleanly. |
| F-13 | reproduced: no nested `.git` under `pandaPI/`, no `160000` gitlinks under `pandaPI/`, one worktree, tracked status clean before CDC file. |
| F-14 | reproduced: `closing-report.md` walks all 14 rows and includes Bubble-up. CC did not write this CDC verification file. |

## Bubble-up Check

Delivered-as-assigned: yes. The slice delivers Arc02's assigned engine #14/#12
piece: the engine now distinguishes solved search from search failure via exit
status while keeping the existing smoke/status-line classification.

Silent-drop diff: complete. Dropped items are explicitly named as PR formatting
churn, generated `.gitignore` additions, unrelated upstream patch content, and
out-of-scope Arc02 items. The implemented source scope matches the slice plan.

Arc/project plan impact: no required plan update before Slice03. The existing
Arc02 queue still points next to parser PR #21, then release publication.

## Observations

- CC's stale generated `dist/macos-arm64/provenance.txt` incident did not
  reproduce at CDC HEAD after rebuilding all three components; provenance
  passed against the current docs-close commit.
- `git remote -v` shows the existing `origin` URL as
  `git@github.com:oubiwann/chengdu.git`, while standing project instructions
  name `billosys/chengdu` as repo home. This was not introduced by Slice02 and
  did not affect the local verification, but it is worth reconciling before
  release publication checks rely on remote URLs.
