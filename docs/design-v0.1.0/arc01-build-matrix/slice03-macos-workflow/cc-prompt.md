# CC assignment — chengdu arc01 / slice03 — macos-workflow

You are CC, the implementing context for one slice, working in the
`chengdu` repo (`~/lab/billosys/chengdu`, remote `billosys/chengdu`,
public, direct-to-main convention per `CLAUDE.md`). Read fully before
writing anything.

## Read order (before any code)

1. `/CLAUDE.md` — note the repo-home and direct-to-main entries.
2. `docs/design-v0.1.0/arc01-build-matrix/arc-plan.md` (v1.3).
3. `…/slice03-macos-workflow/slice-doc.md` — goal, scope, constraints.
4. `…/slice03-macos-workflow/ledger.md` — the specification of done: 8
   rows. `LEDGER-DISCIPLINE.md` Section A protocol applies.
5. `…/slice02-linux-workflow/cdc-verification.md` — this slice inherits
   the provenance residue (→ row M-6) and the CDC-5 deprecation note
   (arc02's unless forced); its integrity note also documents the
   checksum-before-judging protocol CDC will apply to your close.

## The work

macOS arm64 build leg on explicitly pinned runners (oldest maintained
arm64 for the build, newest for `cross-compat-macos`);
`readme-verbatim-macos`; `scripts/check-provenance.sh` wired on all
build legs both platforms; min-OS observation recorded; README CI
section update.

## Constraints that bite

- **Pin runner versions explicitly** — survey what arm64 macOS runners
  GitHub currently maintains, choose, and record the rationale in the
  ledger row. Never `macos-latest`.
- **Byte-identical README mirroring** for the macos verbatim job — CDC
  diffs it mechanically, same as slice02's W-6.
- **The provenance gate is a script, not YAML logic** — bash 3.2,
  shellcheck clean, fail-loud with named complaints, and it must be
  provably capable of failing (tamper test) — the same
  vacuous-test countermeasure every gate in this project carries.
- **No Linux regression** — M-8 requires the whole matrix green in one
  run; don't let the macOS work rot the ubuntu legs.
- macOS runners schedule slowly: batch your iteration pushes. Runner-
  environment discovery iterations are expected and cheap; a full-matrix
  failure a local check should have caught counts against the budget of
  five.
- Local macOS spot-checks (your machine is the same arch as the runner)
  are encouraged before pushing — you have the field-proven build
  already; use it to pre-validate script changes.

## Protocol

As slice02: evidence per row as it lands (`attested`, run URLs linked);
amendment requests over silent deviation; at close, `closing-report.md`
with the full 8-row walk plus the Part IV bubble-up. **Because this is
arc01's last slice, your bubble-up feeds directly into the arc close** —
be precise about anything the arc-plan didn't anticipate, and note
anything arc02 should inherit. CDC writes `cdc-verification.md` and then
assembles the arc closing-report; not yours.

## Definition of done

All 8 rows at final status with attested evidence including one linked
green run showing the complete two-platform matrix. Nothing less closes
arc01's last slice.
