# chengdu — standing session instructions

**chengdu**: CI build matrix + binary releases for the PANDA (pandaPI) HTN
planning toolchain (HDDL). Linux + macOS. Primary consumer: `wolong`.

- Planning artifacts live under `docs/design-v0.1.0/…`, per the
  collaboration framework's `PROJECT-MANAGEMENT.md` (canonical layout;
  confirmed by operator 2026-08-05). Do not invent parallel structures.
- Load the `collaboration-framework` skill at session start. Read
  `docs/design-v0.1.0/project-plan.md` before doing anything else.
- Design substrate (treat as the build spec): **"PANDA Runbook — Setup,
  Validation, and Execution"** in the operator's Dropbox
  (`Duncan's Research/LLMs/`). Linux path evidence grade: reproduced
  (2026-08-05 sandbox). macOS path: field-tested same day.
- Upstream pandaPI is consumed at pinned SHAs, never forked silently; all
  deltas are named patch files in `patches/`. Upstreamable fixes get
  contribution tickets (`CONTRIBUTION-STYLE.md` + template).
- **Repo home: `billosys/chengdu`, public** (operator decision 2026-08-06;
  an early slice02 session created `oubiwann/chengdu` private by mistake —
  corrected by transfer). Badge/release URLs must use the billosys path.
- **Workflow convention (operator override, 2026-08-06): push directly to
  `main` — no PR ceremony while the project has no external consumers.**
  The quality gate is unchanged: a slice is not closed until its ledger
  closes and CDC's independent verification (`cdc-verification.md`) lands.
  Merge timing and verification are separate concerns; only the second is
  load-bearing.
