# chengdu arc02 / slice01 - upstream-triage - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Turn Arc02's upstream PR/issue input list into an evidence-backed absorption
queue before any planner source changes land. At slice close, every known
candidate PR and issue has a current upstream snapshot, an applicability or
reproduction attempt, a disposition (`absorb`, `defer`, or `no-op`) with
rationale, and a re-entry condition for every deferral. The arc plan is updated
with that queue before the next implementation slice is planned.

## 2. Scope

**In:**

- Refresh primary-source GitHub state for the three upstream repositories:
  `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`. Capture open PRs,
  open issues, PR draft/open state, head/base SHAs, authors, titles, and URLs.
- Inspect all planned PR candidates: engine #14/#15/#9, parser #21, grounder
  #7. Fetch PR heads for local inspection, list their commits/authors, and
  dry-run applicability against the matching in-tree `pandaPI/` subtree in a
  throwaway worktree or otherwise reversible state.
- Inspect all planned issue candidates: engine #12, grounder #8, parser #25,
  #15, #28, and #29. Attempt reproduction from upstream issue evidence where
  the issue body provides enough input; otherwise record the exact missing
  evidence and a re-entry condition.
- Review the broader open issue lists for the three repos and classify each
  issue as candidate, covered, out-of-scope, duplicate/related, unreproducible
  from available evidence, or deferred to 0.3.0/project backlog.
- Update `../arc-plan.md` with a v1.1 change that records the selected
  absorption queue and any required slice breakdown changes before slice02 is
  planned.
- If triage changes project roadmap scope or the `v0.2.0` boundary, update
  `../../project-plan.md` with a tracked version-history entry; otherwise state
  why no project-plan change is required in the closing report.

**Out:**

- No planner source absorption or issue fixes. This slice may dry-run
  cherry-picks in throwaway state, but the committed diff is planning/docs
  only.
- No final release publication.
- No upstream courtesy issue filing yet; this slice can note needed content,
  but release-linked announcements belong to the release slice.
- No 0.3.0 CLI/API redesign, machine-readable status redesign, or systematic
  stdout/stderr contract.

## 3. Constraints

- End with the main worktree clean except for the intended docs commits.
- Do not leave temporary remotes, branches, worktrees, generated source
  changes, build outputs, or PR trial state tracked in the repository.
- Do not mark a candidate `absorb` unless its applicability is evidenced or
  the exact required remediation is bounded enough for one later slice.
- Do not mark a candidate `no-op` merely because it is inconvenient. Name the
  reason: superseded by chengdu, outside release contract, draft/not intended,
  duplicate, not reproducible from available evidence, or 0.3.0 material.
- Preserve author-credit requirements in the plan for later absorption:
  cherry-pick with native author metadata when possible; use commit trailers
  for upstream PR/issue URLs and the required assistant co-authors.

## 4. Verification approach

CC verifies by capturing current GitHub API output, local PR metadata, dry-run
applicability results, issue reproduction attempts, the final candidate
disposition table, and the arc-plan update diff. CDC verifies by rechecking the
row count, re-running compact API/listing commands where practical, inspecting
that no source files changed, and confirming the arc-plan v1.1 queue is
sufficient to plan slice02 without silent drops.

## 5. Exit criteria

The ledger rows reach final status. The committed diff is docs-only. Arc02 has
a current, explicit absorption queue; every candidate has absorb/defer/no-op
status with rationale; every deferral has a re-entry condition; the broader
open upstream issue lists have been reviewed; and slice02 can be planned from
the updated arc plan instead of from stale roadmap notes.

## 6. Candidate inputs at slice open

Open PRs verified at arc02 open:

| Repo | PR | State | Head SHA |
|------|----|-------|----------|
| `pandaPIengine` | [#14](https://github.com/panda-planner-dev/pandaPIengine/pull/14) | open | `304048392e2b3ee53aca7f93d88b5a35230f638b` |
| `pandaPIengine` | [#15](https://github.com/panda-planner-dev/pandaPIengine/pull/15) | open | `bb8a4c7acf393eee97a910d9551af6fc30e9480a` |
| `pandaPIengine` | [#9](https://github.com/panda-planner-dev/pandaPIengine/pull/9) | open | `68747acbcaa611818331a49ebe79801346bb86df` |
| `pandaPIparser` | [#21](https://github.com/panda-planner-dev/pandaPIparser/pull/21) | open | `9735763ffd694d95c287766a447deba980d7deaa` |
| `pandaPIgrounder` | [#7](https://github.com/panda-planner-dev/pandaPIgrounder/pull/7) | open draft | `f289c5c8033785eb5845842143726d86834bc8fe` |

Planned issue candidates: engine #12; grounder #8; parser #25, #15, #28, and
#29. Slice01 must also review the broader open issue lists current at execution
time.
