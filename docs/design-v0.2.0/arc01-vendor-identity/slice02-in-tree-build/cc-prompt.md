# CC assignment - chengdu arc01 / slice02 - in-tree-build

You are CC, the implementing context for one slice, working in the
`chengdu` repo (`~/lab/billosys/chengdu`) on `release/0.2.x`. This file is
your assignment; read it fully before writing anything.

## Read order (before any code)

1. `/CLAUDE.md` - standing project contracts.
2. `docs/design-v0.2.0/project-plan.md` - project DoD and fork policy.
3. `docs/design-v0.2.0/arc01-vendor-identity/arc-plan.md` - arc capability,
   current layout, and OQ2's slice02 build-copy decision.
4. `docs/design-v0.2.0/arc01-vendor-identity/slice01-subtree-import/cdc-verification.md`
   - the closed import baseline you are building from.
5. `docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build/slice-doc.md`
   - goal, scope, constraints, exit criteria.
6. `docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build/ledger.md`
   - the specification of done. Twelve rows; every one reaches a final
   status before this slice advances. Follow the CC protocol in the
   collaboration framework's `LEDGER-DISCIPLINE.md` Section A.

## The work

Repoint the build from fetched `upstream/` checkouts to vendored `pandaPI/`
source, without source edits:

- Build scripts copy from `pandaPI/pandaPIparser`,
  `pandaPI/pandaPIgrounder`, and `pandaPI/pandaPIengine` into ignored
  disposable build directories, then compile from those copies.
- Patch application remains a bridge in this slice, but only inside the
  disposable grounder copy. Do not apply patches directly in `pandaPI/`.
- Dist output remains `dist/<platform>/`, and `provenance.txt` remains
  compatible with `scripts/check-provenance.sh`.
- GitHub Actions and README source-build commands stop running
  `scripts/fetch-upstream.sh`. Release install commands and wolong's fetch
  spec do not change.

## Constraints

- No planner-source network fetch in active build paths. `actions/checkout`
  and action/tool installers are fine; cloning pandaPI/cpddl/h2/etc. during
  build is not.
- Keep bash 3.2 compatibility.
- Preserve the 0.1.0 behavior oracle: positive smoke, negative smoke,
  provenance, packaging shape.
- Keep this slice out of patch dissolution. `patches/` can remain an active
  build input until slice03; do not convert patch files into source commits
  here.
- If local tools such as `shellcheck` or `actionlint` are unavailable, record
  the exact limitation in the ledger/closing report and perform the strongest
  static inspection available. Do not mark a row as reproduced by a tool you
  did not run.

## Protocol

- Work against the ledger; update Evidence with strength `attested` as each
  row lands.
- A wrong or impossible ledger row is an amendment request to the operator,
  never quietly ignored.
- At close: write `closing-report.md` in this slice directory - per-row walk
  for all 12 rows, then a **Bubble-up to the arc** section answering:
  delivered-as-assigned, what the arc-plan did not anticipate, and the
  silent-drop diff.
- Do not write `cdc-verification.md`; that is CDC's independent file.

## Definition of done

All 12 ledger rows at final status with `attested` evidence, source builds
originating from `pandaPI/`, no active build path running `fetch-upstream.sh`,
`pandaPI/` clean after builds, local smoke/provenance/package gates passing,
and Slice 3 set up to dissolve patches as normal in-tree commits.
