# chengdu — standing session instructions

**chengdu**: CI build matrix + binary releases for the PANDA (pandaPI) HTN
planning toolchain (HDDL). Linux + macOS. Primary consumer: `wolong`.
As of 0.2.0, chengdu is a **deliberate public fork**: pandaPI sources
live in-tree under `pandaPI/` with full upstream history.

- Planning artifacts live under `docs/design-vX.Y.Z/…`, per the
  collaboration framework's `PROJECT-MANAGEMENT.md` (canonical layout;
  confirmed by operator 2026-08-05). Do not invent parallel structures.
  **Active design version: `docs/design-v0.3.0/`.** v0.1.0 is closed
  (2026-08-06, gate GO — see its `closing-report.md`); v0.2.0 is closed
  (2026-08-09, gate GO — see its `closing-report.md`).
- Load the `collaboration-framework` skill at session start. Read
  `docs/design-v0.3.0/project-plan.md` before doing anything else; read
  `docs/design-v0.2.0/project-plan.md` and
  `docs/design-v0.2.0/closing-report.md` when the closed fork/release baseline
  matters.
- Design substrate (treat as the build spec): **"PANDA Runbook — Setup,
  Validation, and Execution"** in the operator's Dropbox
  (`Duncan's Research/LLMs/`). Linux path evidence grade: reproduced
  (2026-08-05 sandbox). macOS path: field-tested same day.
- **Fork policy (supersedes 0.1.0's patches-not-fork, operator decision
  2026-08-06; evidence basis in the 0.2.0 plan §2):** pandaPI is
  vendored via `git subtree add` (no squash) at the 0.1.0 pins (verified
  ≡ upstream HEADs, 2026-08-06); upstream PRs are absorbed via
  `cherry-pick -Xsubtree` with native author credit + PR URL trailers;
  issue fixes reference upstream issue URLs. `pins.env` is historical
  once import lands. Upstream contribution tickets are courtesy, no
  longer obligation — but NOTICE files and fork-announcement issues are
  required (see 0.2.0 plan §3). Arc01 landed in 0.2.0; `pins.env` is now
  historical import-point evidence, and active builds use the in-tree
  `pandaPI/` source.
- **Repo home: `billosys/chengdu`, public** (operator decision 2026-08-06;
  an early slice02 session created `oubiwann/chengdu` private by mistake —
  corrected by transfer). Badge/release URLs must use the billosys path.
- **Workflow convention (operator override, 2026-08-06): push directly to
  `main` — no PR ceremony while the project has no external consumers.**
  The quality gate is unchanged: a slice is not closed until its ledger
  closes and CDC's independent verification (`cdc-verification.md`) lands.
  Merge timing and verification are separate concerns; only the second is
  load-bearing.
- **CI entrypoint convention (operator override, 2026-08-10):** all new
  GitHub Actions / CI jobs must invoke `make` targets, not call project
  scripts directly. Scripts remain implementation details behind make targets
  and local developer helpers. If a new job needs behavior without a target,
  add the target in the same change before wiring the workflow.
- **Commit footer convention (operator override, 2026-08-07):** every future
  assistant-authored commit message includes these trailers:
  `Co-authored-by: Codex <noreply@openai.com>` and
  `Co-authored-by: Billo AI <ai-engineering@billo.systems>`.
- **Standing constraint for every release:** wolong's fetch spec is
  frozen (asset naming, `SHA256SUMS`, `release-manifest.txt` shape,
  4-command install path). Releases that change the shape are breaking
  changes and need an explicit operator decision.
