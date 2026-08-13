# Slice 02 closing report - engine-exit-status

Closed by CC on 2026-08-08 for `release/0.2.x`.

## Summary

Delivered as assigned. Upstream pandaPIengine PR #14 / issue #12 was refreshed,
found still open and conflicting, and absorbed as a bounded manual port in
implementation commit `4da38819ae0d556a9a8c22972c1954e98b846c03`.

The engine now returns the selected planner result instead of unconditionally
returning `0` at the end of `main`. Progression search returns `0` when solved
and `2` for timeout / proven-unsolvable. The smoke harness now expects the
unsolvable fixture to exit `2` while preserving its explicit `UNSOLVABLE`
classification.

No parser PR #21 work, 0.3.0 status/API redesign, release publication, active
patch-file flow, or wolong release-shape change was introduced.

## Upstream Refresh

Implementation-time refresh on 2026-08-08:

| Item | Evidence |
|------|----------|
| PR | `panda-planner-dev/pandaPIengine#14`, "Set exit status from search." |
| State | open, non-draft |
| Head | `304048392e2b3ee53aca7f93d88b5a35230f638b` |
| Base | `17e8d98cbbaed38ee25b136e8ead08332c673275` on `master` |
| Merge state | `mergeable=false`, `mergeable_state=dirty`, `rebaseable=false` |
| Size | 4 commits, 7 files, +387/-376 |
| Author | Robert P. Goldman `<rpgoldman@sift.net>` on all four PR commits |
| Issue | `panda-planner-dev/pandaPIengine#12`, open |
| Intent | Search failure should use a designated nonzero code, specifically `2`; ordinary SAT solver states are 10 and 20, with other states treated as errors unless explicit termination modes are in play. |

PR #14 diff was saved to `/tmp/chengdu-pr14.patch`. The preferred direct
application check failed:

```text
git apply --check --directory=pandaPI/pandaPIengine /tmp/chengdu-pr14.patch
```

Conflicts remained in `SearchEngine.cpp`, `sat_planner.cpp`, and
`sat_planner.h`, so the slice used a manual bounded port. PR formatting churn,
generated `.gitignore` additions, and unrelated files were not carried over.

## Implementation

Commit:
`4da38819ae0d556a9a8c22972c1954e98b846c03` -
`engine: propagate search exit status`.

Functional diff:

| Path | Change |
|------|--------|
| `pandaPI/pandaPIengine/src/SearchEngine.cpp` | Adds `searchRes`, assigns progression/SAT/translation results, returns `searchRes`, and keeps unavailable SAT/BDD support nonzero. |
| `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h` | Changes `search` to return `int`; solved returns `0`, timeout/proven-unsolvable returns `2`. |
| `pandaPI/pandaPIengine/src/sat/sat_planner.cpp` / `.h` | Changes SAT planner entry points to return `int`; success returns `0`; non-10/20 solver states keep existing `exit(2)` error behavior. |
| `pandaPI/pandaPIengine/src/translation/translationController.cpp` / `.h` | Returns planner status to `main` instead of calling `exit()` directly on the ported paths. |
| `scripts/smoke-test.sh` | Negative unsolvable fixture now expects engine exit `2` plus `Status: Proven unsolvable`. |

Implementation commit trailers include:

```text
Upstream-author: Robert P. Goldman <rpgoldman@sift.net>
Upstream-PR: https://github.com/panda-planner-dev/pandaPIengine/pull/14
Upstream-Issue: https://github.com/panda-planner-dev/pandaPIengine/issues/12
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```

## Verification

Local gates run on macos-arm64 after the implementation change:

| Gate | Result |
|------|--------|
| `./scripts/build-parser.sh` | pass; existing compiler warnings only |
| `./scripts/build-grounder.sh` | pass; existing compiler warnings only |
| `./scripts/build-engine.sh` | pass; existing compiler warnings only |
| `./scripts/check-provenance.sh` | pass after clearing stale ignored `dist/macos-arm64/provenance.txt` and rebuilding all three artifacts |
| `./scripts/smoke-test.sh` | pass: 5 passed, 0 failed; solve gate `exit 0, Status: Solved` |
| `./scripts/smoke-test.sh --negative` | pass: 4 passed, 0 failed; unsolvable gate `engine exit 2, Status: Proven unsolvable` |
| `./scripts/package-release.sh v0.2.0-slice02-smoke` | pass; built macos-arm64 dry-run package, manifest, checksums, licenses, and notes |
| `bash -n scripts/*.sh` | pass |
| `/bin/bash -n scripts/*.sh` | pass |
| `shellcheck scripts/*.sh` | pass |
| `actionlint .github/workflows/*.yml` | pass |
| `git diff --check` | pass after correcting three indentation lines |

The stale provenance issue was in ignored generated output, not tracked source.
Because provenance records the current Git `HEAD`, CDC should refresh generated
`dist/` output before rerunning provenance/package gates after this docs-close
commit.

## Ledger Walk

| Row | Close |
|-----|-------|
| F-1 | Done. Slice started on `release/0.2.x`; Slice01 close and CDC verification files were present. |
| F-2 | Done. PR #14 and issue #12 were refreshed with state, head, author, merge/conflict, and issue intent. |
| F-3 | Done. Direct PR diff application failed; bounded manual port was used and disclosed. |
| F-4 | Done. `main` now returns search status; progression solved is `0`, timeout/proven-unsolvable is `2`; smoke verifies both positive and unsolvable outcomes. |
| F-5 | Done. SAT planner return signatures compile and preserve non-10/20 solver-state error handling. |
| F-6 | Done. Negative smoke expects exit `2` plus `Status: Proven unsolvable` and reports `UNSOLVABLE`. |
| F-7 | Done. Positive smoke still solves, converts, and verifies true with engine exit `0`. |
| F-8 | Done. Parser, grounder, engine, provenance, and smoke gates passed locally. |
| F-9 | Done. Package dry-run passed without release publication. |
| F-10 | Done. Shell, workflow, shellcheck, actionlint, and diff hygiene passed. |
| F-11 | Done. Diff is scoped to engine exit-status source, smoke harness, and slice close docs. |
| F-12 | Done. Manual-port commit credits Robert P. Goldman, PR #14, issue #12, and required co-authors. |
| F-13 | Done. No gitlinks, nested `.git`, temporary worktree, or extra remote was introduced; generated outputs remain ignored. |
| F-14 | Done. This report and the ledger include row-by-row closure and Bubble-up. |

## Bubble-up

Delivered-as-assigned: yes.

Semantic surprise: positive source behavior was straightforward, but
`dist/macos-arm64/provenance.txt` can accumulate stale ignored blocks if builds
append onto an existing generated file. Clearing it and rebuilding restored the
provenance gate. This is a generated-output hygiene issue for CDC/CI awareness,
not a slice-source change.

Arc/project docs changed: no parent arc or project-plan change was needed.
The current active behavior now matches the already-opened Slice02 plan.
Historical references outside this slice were preserved.

Silent-drop diff: dropped PR #14 formatting churn, generated `.gitignore`
additions, and unrelated upstream patch content. Ported only exit-status
behavior and traceable return plumbing.

CDC handoff: rerun practical gates from a clean generated output state, confirm
the negative unsolvable fixture exits `2`, inspect commit trailers, and write
`cdc-verification.md`.
