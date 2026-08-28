# CC assignment — chengdu arc01 / slice01 — substrate

You are CC, the implementing context for one slice, working in the
`chengdu` repo (`~/lab/billosys/chengdu`). This file is your assignment;
read it fully before writing anything.

## Read order (before any code)

1. `/AGENTS.md` — standing project contracts.
2. `docs/design-v0.1.0/arc01-build-matrix/arc-plan.md` — what this slice is
   load-bearing for.
3. `docs/design-v0.1.0/arc01-build-matrix/slice01-substrate/slice-doc.md` —
   goal, scope, constraints, exit criteria.
4. `docs/design-v0.1.0/arc01-build-matrix/slice01-substrate/ledger.md` —
   **the specification of done.** Eleven rows; every one reaches a final
   status before this slice advances. Follow the CC protocol in the
   collaboration framework's `LEDGER-DISCIPLINE.md` Section A.
5. The PANDA Runbook — `~/Dropbox/Duncan's Research/LLMs/PANDA Runbook -
   Setup, Validation, and Execution.md`. This is your build spec; its Linux
   path is evidence-grade *reproduced*, its macOS path field-confirmed.
   The essential exit-code contract is inlined in slice-doc §6 if the
   runbook is unavailable.

## The work

Mechanize the runbook: `pins.env`, `scripts/fetch-upstream.sh`, the bliss
patch as `patches/bliss-0.73-cxx11-string-literal.patch`, three
`scripts/build-*.sh`, `scripts/smoke-test.sh` with fixtures, provenance
byproduct, README + .gitignore. Scope in/out is slice-doc §2 — the out list
is disclosed deferral, not an invitation.

## Constraints (from slice-doc §3, restated because they bite)

- bash, `set -euo pipefail`, **stock macOS bash 3.2 compatible**, shellcheck
  clean.
- Patches-not-fork: never mutate `upstream/` except via committed patch
  files applied by script.
- The smoke test's failure modes stay distinct. The engine's
  `Status: Proven unsolvable` at exit 0 is UNSOLVABLE — a first-class
  outcome, never success, never a generic error. This is ledger row F-8 and
  the project's reason to exist; it is the row least tolerant of softening.
- Known traps, all documented in the runbook: bliss's rm-rf/unzip Makefile
  recipe (patch after extract, build via `make CC=c++ -C third-party/bliss`);
  `cmake ../src` not `..`; grounder sources at `pandaPIgrounder/src`, a
  sibling of `cpddl/`; macOS grounder compiler is clang (`CXX=c++ CC=cc`).

## Protocol

- Work against the ledger; update Evidence (strength `attested`) as each row
  lands, not in a batch at the end.
- A wrong or impossible row is an amendment request to the operator, never
  quietly ignored.
- Iteration budget: five. If you're approaching it, stop and report rather
  than grinding.
- At close: write `closing-report.md` in this slice directory — per-row walk
  (all 11 rows, no summaries), then a **Bubble-up to the arc** section
  answering the three questions in `PROJECT-MANAGEMENT.md` Part IV
  (delivered-as-assigned? / what did the arc-plan not anticipate? /
  silent-drop diff).
- Do not write `cdc-verification.md` — that is CDC's file, produced by an
  independent context (Linux reproduction runs in the Cowork sandbox).

## Definition of done

All 11 ledger rows at final status with `attested` evidence on your
platform (macOS), ready for CDC's Linux reproduction and independent
re-walk. Not before.
