# CDC verification - chengdu arc02 / slice01 - upstream-triage

Verified by CDC on 2026-08-08, against `e3264eb7` on `release/0.2.x`.
Role: independent ledger reproduction and bubble-up check, per
`LEDGER-DISCIPLINE.md` Section A and `PROJECT-MANAGEMENT.md` Part IV.

## Verdict

**Accepted.** Rows F-1 through F-12 reproduce or check cleanly. The slice
delivered its assigned Arc02 piece: it refreshed upstream PR/issue state,
classified the candidate queue, reproduced PR applicability against the in-tree
subtrees, updated the arc plan to v1.1, and left the committed diff limited to
planning documents.

CDC note on F-5: I reproduced the upstream issue-body inspection and confirmed
the presence or absence of reproducers that supports the dispositions. I did
not rerun the parser timeout fixtures from scratch after CC removed the
temporary triage workspace. The row is accepted because the closure decision is
the routing of issues into absorb/defer/no-op, and the upstream evidence plus
CC's documented local attempts are sufficient for that triage result.

## Row reproduction

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Current branch is `release/0.2.x`; `git status --short --branch` is clean against origin; `docs/design-v0.2.0/arc01-vendor-identity/closing-report.md` and `docs/design-v0.2.0/arc02-upstream-absorption/arc-plan.md` both exist. |
| F-2 | reproduced | Fresh GitHub API checks returned PR sets engine `[9,14,15]`, parser `[21]`, grounder `[7]`; issue sets engine `[3,6,10,12,16,17]`, parser `[1,10,11,15,16,23,24,25,28,29]`, grounder `[2,3,5,8,10,11,12]`, matching the closing report and arc-plan v1.1. |
| F-3 | reproduced | `gh api pulls/<n>` spot-checks matched arc-plan v1.1 metadata: engine #14 is open/non-draft at head `304048392e2b3ee53aca7f93d88b5a35230f638b`, `mergeable_state=dirty`, 4 commits and 7 changed files; engine #15 is clean, 1 commit, 1 changed file; engine #9 is clean, 13 commits, 39 changed files; parser #21 is clean, 1 commit, 1 changed file; grounder #7 is open draft, clean, 6 commits, 3 changed files. |
| F-4 | reproduced | Downloaded PR diffs with `curl -L` and ran `git apply --check --directory=<prefix>` against the in-tree subtrees. Engine #14 failed exactly in `SearchEngine.cpp`, `sat_planner.cpp`, and `sat_planner.h`; engine #15, engine #9, parser #21, and grounder #7 all passed. |
| F-5 | reproduced with caveat | Issue-body API inspection confirms the routing basis: engine #12 describes exit status and points toward PR #14; grounder #8 requests unreachable-goal status but has no fixture; parser #25 links a gist; parser #15 links a gist and describes a hang; parser #28 has command/output but no input files; parser #29 links attached files and describes expansion. CC's closing report records local parser attempts and timeout/no-op outcomes. |
| F-6 | reproduced | The broader issue sweep in arc-plan v1.1 covers every current open issue number returned by the API. The closing report repeats the grouping table for engine, parser, and grounder, with no unmatched open issue numbers. |
| F-7 | reproduced | Arc-plan v1.1 and the closing report contain final dispositions for every planned PR/issue and broader issue group. Deferrals include concrete re-entry conditions for engine #9, grounder #8, parser #15/#29, and engine #16. No-op rows state topic-specific rationales rather than "not needed." |
| F-8 | reproduced | `arc-plan.md` is updated to v1.1 with the absorption queue narrowed to engine #14/#12 and parser #21, a four-slice Arc02 breakdown, OQ updates, broader issue classifications, and a v1.1 Version History entry naming slice01 and why. |
| F-9 | reproduced | `git diff --name-only 14fe461c32521bc360062c7ddf9403d03884970b..HEAD` does not include `project-plan.md`; the closing report explains no project-plan change is required because triage only changes Arc02's internal queue and not the project DoD, v0.2.0 boundary, platform matrix, or wolong release contract. |
| F-10 | reproduced | The slice diff from `14fe461c32521bc360062c7ddf9403d03884970b..HEAD` contains only `arc-plan.md`, this slice's `ledger.md`, and this slice's `closing-report.md`. `git status --short -- pandaPI scripts .github README.md release dist build` returned empty. |
| F-11 | reproduced | `git diff --check 14fe461c32521bc360062c7ddf9403d03884970b..HEAD` passed. `git status --short` was empty before writing this CDC file. `git worktree list` shows only the main worktree; `git branch --list '*arc02*triage*'` returned empty; `/tmp/chengdu-arc02-triage` is absent. |
| F-12 | reproduced | `closing-report.md` exists, walks rows F-1 through F-12, and includes a Bubble-up to the arc section covering delivered-as-assigned, new findings, silent-drop diff, and project-plan change decision. |

## Additional checks

- Commit `e3264eb7` contains the required current co-author trailers from
  `AGENTS.md`: `Codex <noreply@openai.com>` and Billo AI.
- The PR applicability reproduction used downloaded `.diff` files and
  `git apply --check`, so no trial PR source changes were applied to the main
  worktree.
- The branch was clean before this verification file was added.

## Bubble-up check

**Delivered as assigned:** yes. Slice01 performed the Arc02 triage gate the
arc plan assigned it: current upstream state was refreshed, PR applicability
was checked, issue evidence was inspected, all planned and broader candidates
were classified, and the arc plan was updated before slice02 planning.

**Silent-drop diff:** complete. The final tables cover all planned PRs and
issues plus every current open issue number from the three upstream
repositories. Absorptions, deferrals, and no-ops are explicit, and deferrals
carry re-entry conditions.

**Arc-plan change required:** already completed. Slice01 changed Arc02's
internal implementation queue and slice breakdown; `arc-plan.md` v1.1 records
that change and why.

**Project-plan change required:** no. The triage result narrows Arc02's
internal queue but does not alter the v0.2.0 definition of done, support
matrix, or wolong release asset contract.

**Next planning target:** slice02 `engine-exit-status`, scoped to a bounded
port/absorption of engine PR #14 / issue #12 with upstream credit and no 0.3.0
CLI/API redesign.
