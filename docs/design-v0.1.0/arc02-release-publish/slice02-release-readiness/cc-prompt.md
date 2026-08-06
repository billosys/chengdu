# CC assignment — chengdu arc02 / slice02 — release-readiness

You are CC, the implementing context for one slice, in `chengdu`
(`~/lab/billosys/chengdu`, remote `billosys/chengdu`, public,
direct-to-main). This slice closes arc02. Read fully before writing
anything.

## Read order (before any code)

1. `/CLAUDE.md`.
2. `docs/design-v0.1.0/arc02-release-publish/arc-plan.md` (v1.0) — D4,
   D5, RQ2, and §4's format-freeze language.
3. `…/slice02-release-readiness/slice-doc.md` — especially §2 (the
   audit method) and §4 (evidence-pointer constraint).
4. `…/slice02-release-readiness/ledger.md` — 9 rows; Section A protocol.
5. `…/slice01-release-workflow/closing-report.md` bubble-up — the
   asset-set growth warning, format freeze, and idempotency contract
   you inherit.

## The work, in dependency order

Audit (L-1, L-2) → `THIRD-PARTY-LICENSES` + obligations (L-3, L-4) →
fixtures-in-tarball + install docs (L-5) → plan dispositions (L-6,
L-7) → **only then** the `v0.1.0` cut (L-8) → consumer rehearsal (L-9).
The tag is the slice's last act; nothing about it is exploratory.

## Constraints that bite

- **Every audit claim carries an evidence pointer.** Link-command
  lines from real build logs, `nm`/`otool` on real artifacts, LICENSE
  paths at exact SHAs. "It's BSD, everyone knows" closes nothing.
- **`h2-fd-preprocessor` is the named risk item.** Its objects are on
  the grounder link line (arc01, verified); its ancestry suggests
  GPL-family. If the audit confirms copyleft: documentation-based
  compliance is the default (license text + Source-availability section
  naming repos/SHAs/patches), recorded as a tracked arc-plan amendment
  per RQ2. A build change to unlink anything is NOT yours to decide —
  that's an operator conversation via amendment request.
- **Format changes are batched here, then frozen.** fixtures/ in
  tarballs + THIRD-PARTY-LICENSES in assets is the final v0.1.0 shape;
  wolong consumes it frozen. Update `package-release.sh` (and its
  tamper-resistance) accordingly; shellcheck/bash-3.2 as always.
- **The cut is a one-way door** — L-8 lists its preconditions; verify
  all eight other rows are at final status before pushing the tag.
  Botched cut recovery is delete-release-and-tag-then-retag, per the
  release.yml header you wrote last slice.
- The audit doc lives at `docs/license-audit-v0.1.0.md` (operator
  approved the location with this open set — do not relocate).

## Protocol

Evidence per row as it lands; amendment requests over silent deviation;
five-iteration budget (audit-evidence gathering iterations are
expected; a wrong cut is not an iteration, it's an incident). At close:
`closing-report.md` with the 9-row walk plus the Part IV bubble-up —
**this bubble-up feeds the arc close directly**: state plainly whether
the arc's capability (tag → verified, checksummed, provenance-carrying,
license-clean release) is now real, and hand CDC the evidence set for
arc ledger R-1 through R-6. CDC writes `cdc-verification.md`, then
assembles the arc closing-report; the operator gates the arc.

## Definition of done

All 9 rows at final status: an audit that would survive hostile review,
a published `v0.1.0` carrying every obligation the audit found, install
docs a stranger could follow, both plan dispositions recorded, and a
consumer rehearsal transcript. This is the release wolong builds on —
close it like that's true, because it is.
