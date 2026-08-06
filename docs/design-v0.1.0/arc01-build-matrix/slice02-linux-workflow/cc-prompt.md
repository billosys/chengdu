# CC assignment — chengdu arc01 / slice02 — linux-workflow

You are CC, the implementing context for one slice, working in the
`chengdu` repo (`~/lab/billosys/chengdu`). Read this fully before writing
anything.

## Read order (before any code)

1. `/CLAUDE.md` — standing contracts.
2. `docs/design-v0.1.0/arc01-build-matrix/arc-plan.md` — **v1.2**; note
   OQ1's amendment (finding CDC-1) and OQ3.
3. `…/slice02-linux-workflow/slice-doc.md` — goal, scope, constraints.
4. `…/slice02-linux-workflow/ledger.md` — the specification of done: 9
   rows. Follow `LEDGER-DISCIPLINE.md` Section A's CC protocol.
5. `…/slice01-substrate/cdc-verification.md` — the three findings this
   slice inherits (CDC-1 → row W-8, CDC-3 → row W-7, plus the re-entry
   rows W-4/W-6).

## The work

`.github/workflows/build.yml` (ubuntu-22.04 + ubuntu-24.04 matrix calling
the slice01 scripts; artifact upload; `readme-verbatim` job; actionlint
gate), the CDC-3 fix in `smoke-test.sh`, the fetch-only fallback guard in
`build-grounder.sh` + docs, README badge/CI section.

## Constraints that bite

- **No build logic in YAML** — the workflow calls `scripts/*.sh`. If you
  need a branch, put it in a script where shellcheck sees it.
- Pin action majors (`@v4`), mirror the README exactly in the
  `readme-verbatim` job (byte-identical commands — W-6 is verified by
  diff), and keep every script edit bash-3.2-safe and shellcheck-clean.
- Iterating a workflow means pushing and watching real runs. Use
  `workflow_dispatch` + a working branch to avoid churning `main`; the
  linked green run in evidence must be on the branch the slice lands
  from. Runs count toward the five-iteration budget only when a full
  matrix run fails for a reason a local check should have caught —
  runner-environment discovery iterations are expected and cheap.
- W-3 (gitlab reachability) closes with a recorded answer EITHER WAY. If
  runners cannot reach gitlab.com, stop, record the failure log in the
  ledger row, and raise it to the operator — that outcome re-opens the
  fallback disposition and is bigger than this slice.

## Protocol

Per `LEDGER-DISCIPLINE.md` Section A: evidence (`attested`) filled per row
as it lands, run URLs linked; amendment requests over silent deviation; at
close, `closing-report.md` with the full 9-row walk plus the Part IV
bubble-up (delivered-as-assigned / what the arc-plan didn't anticipate /
silent-drop diff). CDC (the Cowork session) verifies independently:
run-log review, artifact download + container execution, actionlint,
re-walk. `cdc-verification.md` is not yours to write.

## Definition of done

All 9 rows at final status with attested evidence including linked green
runs. W-2 through W-6 all cite the SAME run where possible — one run, one
story.
