# CC assignment - chengdu arc01 / slice01 - subtree-import

You are CC, the implementing context for one slice, working in the
`chengdu` repo (`~/lab/billosys/chengdu`) on `release/0.2.x`. This file is
your assignment; read it fully before writing anything.

## Read order (before any git operation)

1. `/CLAUDE.md` - standing project contracts.
2. `docs/design-v0.2.0/project-plan.md` - project DoD and fork policy.
3. `docs/design-v0.2.0/arc01-vendor-identity/arc-plan.md` - what this
   slice is load-bearing for.
4. `docs/design-v0.2.0/arc01-vendor-identity/slice01-subtree-import/slice-doc.md`
   - goal, scope, constraints, exit criteria.
5. `docs/design-v0.2.0/arc01-vendor-identity/slice01-subtree-import/ledger.md`
   - the specification of done. Eleven rows; every one reaches a final
   status before this slice advances. Follow the CC protocol in the
   collaboration framework's `LEDGER-DISCIPLINE.md` Section A.
6. `pins.env` - historical import-point record for parser, grounder, and
   engine. The former submodule SHAs are in the slice-doc identity table.

## The work

Import the pandaPI source tree into chengdu with full history and no source
edits:

- `pandaPI/pandaPIparser` from
  `https://github.com/panda-planner-dev/pandaPIparser.git`
  at `88c0995c72c5ce2685e9546ec153a3545977ed81`.
- `pandaPI/pandaPIgrounder` from
  `https://github.com/panda-planner-dev/pandaPIgrounder.git`
  at `b04ff7bcca710ae80a1ce73b8c027f0224565afc`.
- `pandaPI/pandaPIengine` from
  `https://github.com/panda-planner-dev/pandaPIengine.git`
  at `810f04388667db5e3e4f114e960a4efbb43b1ac0`.
- `pandaPI/pandaPIgrounder/cpddl` from
  `https://gitlab.com/danfis/cpddl.git`
  at `8c142bab211200213746c75be71810526e834d4a`.
- `pandaPI/pandaPIgrounder/h2-fd-preprocessor` from
  `https://github.com/galvusdamor/h2-fd-preprocessor.git`
  at `e335433bfc53171a1c215bd49ba39cc5fb1d485b`.

Use `git subtree add --prefix=<prefix> <repo> <sha>` without `--squash`.
If you use temporary remotes, remove them before close or record why they
remain. Preserve the grounder's identity before replacing its gitlinks: record
the grounder import commit, verify its `cpddl` and `h2-fd-preprocessor`
gitlinks, then replace those paths with the matching subtree imports.

## Constraints

- Do not edit imported source files in this slice. Compatibility patches land
  later, in slice03.
- Do not rewire scripts, workflows, release packaging, README install paths,
  `pins.env`, `patches/`, fixtures, or license artifacts in this slice.
- Do not use `--squash`.
- Do not leave nested `.git` directories or gitlinks in `pandaPI/`.
- If a subtree import goes wrong, stop and report the state. Do not rewrite
  history or run destructive git commands unless the operator explicitly
  approves the recovery.

## Protocol

- Work against the ledger; update Evidence with strength `attested` as each
  row lands.
- For every import, record source URL, SHA, import commit, and the normalized
  tree check used.
- A wrong or impossible ledger row is an amendment request to the operator,
  never quietly ignored.
- At close: write `closing-report.md` in this slice directory - per-row walk
  for all 11 rows, then a **Bubble-up to the arc** section answering the
  three questions in `PROJECT-MANAGEMENT.md` Part IV:
  delivered-as-assigned, what the arc-plan did not anticipate, and the
  silent-drop diff.
- Do not write `cdc-verification.md`; that is CDC's independent file.

## Definition of done

All 11 ledger rows at final status with `attested` evidence, `pandaPI/`
present as real in-tree source directories, full histories reachable from
chengdu history, imported trees identity-verified against the pins, and the
0.1.0 build/release path untouched for slice02 to repoint deliberately.
