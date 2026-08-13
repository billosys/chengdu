# CC assignment - chengdu arc02 / slice01 - upstream-triage

You are CC, the implementing context for one slice, working in the `chengdu`
repo (`~/lab/billosys/chengdu`) on `release/0.2.x`. This file is your
assignment; read it fully before writing anything.

## Read order (before any work)

1. `/AGENTS.md` - standing project contracts, branch/release policy, commit
   footer convention, and wolong fetch-spec constraint.
2. `docs/design-v0.2.0/project-plan.md` - 0.2.0 DoD, arc roadmap, and Arc02
   boundary.
3. `docs/design-v0.2.0/arc01-vendor-identity/closing-report.md` - the closed
   in-tree fork baseline you are triaging from.
4. `docs/design-v0.2.0/arc02-upstream-absorption/arc-plan.md` - arc capability,
   current upstream snapshot, provisional slice breakdown, open questions, and
   arc ledger.
5. `docs/design-v0.2.0/arc02-upstream-absorption/slice01-upstream-triage/slice-doc.md`
   - goal, scope, constraints, exit criteria.
6. `docs/design-v0.2.0/arc02-upstream-absorption/slice01-upstream-triage/ledger.md`
   - the specification of done. Twelve rows; every one reaches final status
   before this slice advances. Follow the CC protocol in the collaboration
   framework's `LEDGER-DISCIPLINE.md` Section A.

## The work

Triage Arc02 before source absorption:

- Refresh current upstream PR and issue state for:
  - `panda-planner-dev/pandaPIengine`
  - `panda-planner-dev/pandaPIparser`
  - `panda-planner-dev/pandaPIgrounder`
- For PR candidates engine #14/#15/#9, parser #21, and grounder #7:
  - capture state, draft flag, URL, title, head/base SHA, commits/authors, and
    changed-file summary;
  - fetch PR heads for local inspection;
  - dry-run applicability against the matching in-tree subtree using a
    throwaway worktree or fully reversible state;
  - classify each as `absorb`, `defer`, or `no-op`.
- For issue candidates engine #12, grounder #8, and parser #25/#15/#28/#29:
  - inspect issue bodies and comments for reproducer inputs;
  - attempt reproduction where evidence is sufficient;
  - record exact blockers when evidence is insufficient;
  - classify each as `absorb`, `defer`, or `no-op`.
- Review the broader open issue lists for all three repos. Do not silently drop
  an issue just because it was not in the project plan's short candidate list.
- Update `docs/design-v0.2.0/arc02-upstream-absorption/arc-plan.md` to v1.1
  with the selected absorption queue and any slice breakdown changes. If the
  project roadmap itself changes, update `docs/design-v0.2.0/project-plan.md`
  with a tracked version-history entry; otherwise state no project-plan change
  is required in the close report.

## Constraints

- This is a docs/planning slice. Do not commit planner source, scripts,
  workflows, release assets, generated build outputs, or trial cherry-pick
  state.
- Network access to GitHub is expected for triage evidence. If unavailable,
  mark the affected rows blocked/deferred with concrete re-entry conditions;
  do not invent current upstream state.
- A PR conflict or draft status is not automatically a rejection. It is
  evidence to route.
- A no-op disposition needs a real reason: superseded by chengdu, outside the
  release contract, duplicate, unavailable reproducer, draft/out-of-scope, or
  0.3.0 material.
- A deferral needs a re-entry condition.
- Later absorption commits must preserve author credit and include upstream
  PR/issue URL trailers plus the required assistant co-author trailers; this
  slice should preserve that requirement in the queue.

## Protocol

- Work against the ledger; update Evidence with strength `attested` as each
  row lands.
- At close: write `closing-report.md` in this slice directory - per-row walk
  for all 12 rows, then a **Bubble-up to the arc** section answering:
  delivered-as-assigned, what the arc-plan did not anticipate, and the
  silent-drop diff.
- Do not write `cdc-verification.md`; that is CDC's independent file.

## Definition of done

All 12 ledger rows at final status with `attested` evidence, no source changes
committed, current upstream state captured, every candidate classified with
rationale, deferrals carrying re-entry conditions, the arc plan updated to v1.1
with the absorption queue, and slice02 ready to be planned from evidence rather
than stale roadmap notes.
