# Slice 01 closing report: upstream-triage

Closed by: CC
Date: 2026-08-08
Slice doc: [`slice-doc.md`](slice-doc.md)
Ledger: [`ledger.md`](ledger.md)
Resume baseline after footer rewrite:
`14fe461c32521bc360062c7ddf9403d03884970b`
Verdict: proposed closed; CDC verification still required.

Rows: 12. Done: 12. Deferred: 0. No-op: 0.

## Ledger walk

| Row | Result | Evidence |
|-----|--------|----------|
| F-1 | Done / attested. | Work resumed on `release/0.2.x` at clean baseline `14fe461c32521bc360062c7ddf9403d03884970b`; `arc01-vendor-identity/closing-report.md` and `arc02-upstream-absorption/arc-plan.md` existed. Arc02 open commit in rewritten history is `8e282675dff0ad3998c53b438bacc453ff064b24`. |
| F-2 | Done / attested. | GitHub API refresh on 2026-08-08 found the same current upstream sets at close as at triage: engine PRs #15/#14/#9 and issues #17/#16/#12/#10/#6/#3; parser PR #21 and issues #29/#28/#25/#24/#23/#16/#15/#11/#10/#1; grounder PR #7 and issues #12/#11/#10/#8/#5/#3/#2. |
| F-3 | Done / attested. | Planned PR metadata was captured for engine #14/#15/#9, parser #21, and grounder #7: state, draft flag, title, URL, head/base SHA, commits/authors, and changed-file summaries. Arc plan v1.1 carries the summarized table. |
| F-4 | Done / attested. | A throwaway worktree fetched PR heads and ran patch applicability checks with `git diff --binary <base> <head> \| git apply --check --directory=<prefix> -`. Engine #14 conflicted in engine source files; engine #15, engine #9, parser #21, and grounder #7 applied cleanly, with whitespace warnings noted for all except parser #21. The temporary worktree was removed before close. |
| F-5 | Done / attested. | Candidate issue bodies/comments were inspected. Parser #25's gist verified successfully with exit 0 on the current macOS parser, so the segfault did not reproduce. Parser #15 timed out at 15s from its gist, and parser #29 timed out at 30s from its attached files. Parser #28 had no attached inputs. Engine #12 is covered by PR #14; grounder #8 has no reproducer. |
| F-6 | Done / attested. | Broader issue classifications cover every current open issue number from the three repos. Grouping is explicit by repo/topic in arc-plan v1.1 and in this report's silent-drop section. |
| F-7 | Done / attested. | Final dispositions exist for all planned PRs/issues and broader issue groups. Deferrals name re-entry conditions: engine #9 needs a verifier/SAT capability slice with licensing and CI scope; grounder #8 needs a grounding-stage unreachable-goal fixture and default-vs-opt-in policy; parser #15/#29 need parser-normalization design and bounded output/time expectations; engine #16 needs an exact fixture plus timeout/resource target. |
| F-8 | Done / attested. | `arc-plan.md` is updated to v1.1 with the selected absorption queue, broader issue classifications, revised slice breakdown, updated risks/open questions, and a Version History entry naming slice01 and why. |
| F-9 | Done / attested. | `project-plan.md` did not change. Triage resolved the already-planned Arc02 queue and did not alter the project DoD, v0.2.0 boundary, platform matrix, or wolong release contract. |
| F-10 | Done / attested. | The intended committed diff is docs/planning only: `arc-plan.md`, this slice `ledger.md`, and this `closing-report.md`. No planner source, scripts, workflows, README, release assets, `dist/`, or `build/` paths are modified. |
| F-11 | Done / attested. | Final cleanup removed `/tmp/chengdu-arc02-triage`; no temporary branches or remotes were added. Final verification runs `git diff --check`, status, worktree listing, branch listing, and remote listing. |
| F-12 | Done / attested. | This close report walks all 12 ledger rows and includes the required Bubble-up to the arc section. |

## Final disposition table

| Item | Disposition | Rationale / re-entry |
|------|-------------|----------------------|
| engine PR #14 / issue #12 | absorb | Bounded exit-status work is directly in v0.2.0 scope, but conflicts require a port/adaptation in slice02 rather than mechanical cherry-pick. |
| engine PR #15 | no-op | Convenience driver script is outside wolong's frozen binary-release contract and chengdu's existing wrapper/release shape. |
| engine PR #9 | defer | Large SAT verifier subsystem changes build, test, and license surface. Re-entry: a 0.3.0/backlog verifier/SAT slice with licensing and CI scope. |
| parser PR #21 | absorb | Clean one-commit makefile/install-target fix; suitable for slice03 with upstream author credit and PR trailer. |
| grounder PR #7 / issue #5 | no-op | Draft Docker/workflow/script work is outside v0.2.0's release contract. |
| grounder issue #8 | defer | No reproducer or PR; status-policy decision is 0.3.0 material. Re-entry: grounding-stage unreachable-goal fixture and default-vs-opt-in decision. |
| parser issue #25 | no-op | Referenced gist did not reproduce; parser verification exited 0 and reported true. |
| parser issue #15 | defer | Reproduced as bounded long-running parse behavior. Re-entry: parser-normalization design for static antecedents / disjunction / forall handling. |
| parser issue #28 | no-op | No input files attached; likely resource-related but not reproducible from available evidence. |
| parser issue #29 | defer | Reproduced as bounded long-running parse/expansion behavior. Re-entry: same parser-normalization design as #15 plus output-size/time guard. |

## Broader issue sweep

| Repo | Issues | Disposition |
|------|--------|-------------|
| engine | #17 PDDL support/examples; #10 multiple plans | No-op for v0.2.0: support/API expansion. |
| engine | #16 infinite planning time | Deferred to performance/resource backlog; re-entry needs exact fixture and timeout/resource target. |
| engine | #6 Windows build; #3 optional SAT dependency build | No-op for v0.2.0: outside supported matrix and SAT-off release path. |
| parser | #24 verifier diagnostic/scoping; #23 task-index diagnostic | No-op for v0.2.0: unreproduced/diagnostic behavior, related to #25. |
| parser | #16 build after `make clean`; #10 macOS flex/bison build | No-op: current in-tree chengdu build path already covers these build concerns. |
| parser | #11 case sensitivity; #1 MIT relicensing | No-op: language semantics and licensing requests outside v0.2.0. |
| grounder | #12 bliss Mac build; #2 boruvka Mac build | No-op: arc01's in-tree grounder/cpddl/boruvka/bliss build path passes macOS. |
| grounder | #11 and #10 parser-output/diagnostic failures | No-op for v0.2.0: upstream comments say parser safety checks were added; re-enter only with a failing current fixture. |
| grounder | #3 Windows/cpddl build | No-op for v0.2.0: outside supported matrix. |

## Bubble-up to the arc

**Delivered as assigned.** Yes. Against arc-plan slice01, this slice refreshed
current upstream state, dry-ran PR applicability, attempted issue reproduction
where upstream evidence allowed it, classified all planned candidates and the
broader open issue lists, and updated `arc-plan.md` to v1.1 before slice02 is
planned.

**What the arc plan did not anticipate.** The provisional five-slice arc was
larger than the evidence justified. Slice01 narrowed v0.2.0 source absorption
to two items: engine #14/#12 and parser #21. Engine #9, parser #15/#29, and
grounder #8 are real findings, but they require 0.3.0/backlog design rather
than release-hardening absorption. The arc plan now has four slices: triage,
engine exit status, parser makefile PR, and release publication.

**Silent-drop diff.** Scope-as-specified was: all planned PRs, all planned
issues, broader open issue lists, PR applicability checks, issue reproduction
attempts, final dispositions with re-entry conditions, and arc-plan v1.1.
Scope-as-delivered includes all of that. No planned candidate or current open
issue is silently dropped; out-of-scope and unreproduced items are explicit
no-ops or deferrals.

**Project-plan change.** No project-plan change is required. The triage
changes Arc02's internal queue and slice breakdown, but it does not change the
project DoD, v0.2.0 boundary, platform matrix, or wolong's frozen release
asset contract.
