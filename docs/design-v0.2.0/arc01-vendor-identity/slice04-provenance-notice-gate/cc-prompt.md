# CC assignment - chengdu arc01 / slice04 - provenance-notice-gate

You are CC, the implementing context for one slice, working in the
`chengdu` repo (`~/lab/billosys/chengdu`) on `release/0.2.x`. This file is
your assignment; read it fully before writing anything.

## Read order (before any code)

1. `/CLAUDE.md` - standing project contracts.
2. `docs/design-v0.2.0/project-plan.md` - project DoD, fork policy, and
   provenance/licensing requirements.
3. `docs/design-v0.2.0/arc01-vendor-identity/arc-plan.md` - arc capability,
   sequencing rule, OQ4's license-audit boundary, and arc ledger A2-A6.
4. `docs/design-v0.2.0/arc01-vendor-identity/slice03-patch-dissolution/cdc-verification.md`
   - the closed patch-dissolution baseline you are building from.
5. `docs/design-v0.2.0/arc01-vendor-identity/slice04-provenance-notice-gate/slice-doc.md`
   - goal, scope, constraints, exit criteria.
6. `docs/design-v0.2.0/arc01-vendor-identity/slice04-provenance-notice-gate/ledger.md`
   - the specification of done. Fourteen rows; every one reaches a final
   status before this slice advances. Follow the CC protocol in the
   collaboration framework's `LEDGER-DISCIPLINE.md` Section A.

## The work

Close arc01's public identity surface:

- Add `vendor.env` with the seven history-imported source identities:
  prefix, upstream URL, upstream SHA, and subtree import commit. Keep
  `pins.env` as historical import-point evidence, not the active provenance
  source.
- Rework provenance generation/checking so binaries report current chengdu
  source identity plus import identities, not "built from upstream SHA plus
  patches." `patches=none` remains true.
- Retire `scripts/fetch-upstream.sh` from active tooling/documentation.
- Add NOTICE files for the three pandaPI roots, `cpddl`,
  `h2-fd-preprocessor`, `boruvka`, `opts`, durable `bliss`, and `lpsolve`.
- Write `docs/license-audit-v0.2.0.md` for the in-tree fork baseline,
  distinguishing identity imports, slice03 patch-dissolution changes, and
  future arc02 changes.
- Update `licenses/`, package generation, README, and release notes so the
  current release authority is v0.2.0 and source availability points at
  chengdu's public repo at the release commit.
- Prove local behavior and package shape still pass, and gather GitHub Actions
  evidence for the two-platform no-fetch gate.

## Constraints

- Do not absorb upstream PRs or fix upstream issues in this slice. Arc02 owns
  content changes after this baseline.
- Do not publish the real `v0.2.0` release. Dry-runs and CI evidence only.
- Preserve the frozen release asset shape. No new public assets unless you ask
  for an amendment.
- No active planner-source fetch in build, release, README, or workflow paths.
- Keep bash 3.2 compatibility and the disposable build-copy invariant.
- An audit claim needs evidence. If evidence is unavailable, disclose the
  limitation and do not mark the row done.
- If a ledger row is wrong or impossible, request an amendment rather than
  quietly weakening it.

## Protocol

- Work against the ledger; update Evidence with strength `attested` as each
  row lands.
- At close: write `closing-report.md` in this slice directory - per-row walk
  for all 14 rows, then a **Bubble-up to the arc** section answering:
  delivered-as-assigned, what the arc-plan did not anticipate, and the
  silent-drop diff.
- Because this is arc01's final slice, also hand CDC the evidence needed for
  arc ledger closure: slice close pointers, import identity/provenance checks,
  no-fetch CI run URL(s), behavior/package gate results, and NOTICE/license
  audit evidence.
- Do not write `cdc-verification.md`; that is CDC's independent file.

## Definition of done

All 14 ledger rows at final status with `attested` evidence, active
provenance re-anchored on chengdu source plus import identities, NOTICE and
license-audit v0.2.0 complete, release docs/manifests updated, full local gate
passing, full two-platform no-fetch CI evidence recorded, and arc01 ready for
CDC verification plus arc close.
