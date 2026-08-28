# chengdu — standing session instructions

**chengdu**: CI build matrix + binary releases for the PANDA (pandaPI) HTN
planning toolchain (HDDL). Linux + macOS. Primary consumer: `wolong`.
As of 0.2.0, chengdu is a **deliberate public fork**: pandaPI product source
lives in-tree under `pandaPI/`. The `parser/`, `grounder/`, and `engine/`
source roots preserve upstream history; `runtime/` is chengdu-owned shared
runtime substrate for the 0.3.0 managed-process rewrite.

- Planning artifacts live on the dedicated `planning` branch/worktree, not on
  release/source branches. Use `git worktree list` to locate it; the current
  local convention is `.worktrees/planning`. Do not recreate project, arc,
  slice, ledger, closing-report, or CDC verification artifacts under `docs/` on
  `release/*` or source branches.
- Use the current `collaboration-framework` skill at session start. For
  planning work, read `PROJECT-MANAGEMENT.md` as the wayfinder and then load
  the relevant `docs/pm/` files. The current canonical layout is
  `projectNN-<slug>/project-plan.md`, project `ledger.md`,
  `arcNN-<slug>/arc-plan.md`, arc `ledger.md`, and per-slice
  `slice-plan.md`, `ledger.md`, `cc-prompt.md`, `closing-report.md`, and
  `cdc-verification.md`. Older `docs/design-vX.Y.Z/.../slice-doc.md` paths are
  historical/provenance context unless the active planning branch explicitly
  says otherwise.
- Before implementation or slice/arc/project closure, read the active
  `project-plan.md` from the `planning` worktree first, then the relevant
  `arc-plan.md`, slice `slice-plan.md`, and ledger files. In the current
  planning tree, the active 0.3.0 work is under `project03-cli-io/`; the
  earlier 0.1.0 and 0.2.0 planning baselines are under
  `project01-release-binaries/` and `project02-monorepo/`.
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
- **Makefile entrypoint convention (operator override, 2026-08-10):** use the
  top-level `Makefile` as the entry point for build, test, check, CI, and
  release operations. Run `make help` for wayfinding. Before committing code
  changes, run `make check`; if a narrower target is intentionally used
  instead, report exactly what ran and why `make check` was not used. New CI
  behavior must reuse an existing `make` target from `Makefile` or `mk/*`, or
  add one before the workflow calls it. Do not add new documentation or
  automation that teaches users to call `./scripts/*` directly when a Make
  target exists.
- **Commit footer convention (operator override, 2026-08-07):** every future
  assistant-authored commit message includes these trailers:
  `Co-authored-by: Codex <noreply@openai.com>` and
  `Co-authored-by: Billo AI <ai-engineering@billo.systems>`.
- **0.3.0 binary-name/interface decision (operator override, 2026-08-10):**
  `v0.3.0` is not a backwards-compatibility release for inherited
  `pandaPI*` command names or old process behavior. `v0.2.0` is the
  transition release from the old three-repo pandaPI world to chengdu's
  monorepo/prebuilt-binary distribution. `v0.3.0` may intentionally break the
  old external interface and should ship the new `pandapi-*` command names and
  managed-process contract. Any parser/grounder adapter shims kept during
  Arc05 are temporary implementation scaffolding only: once a binary builds
  and passes its contract tests under the new name, update tests, make
  targets, docs, release tooling, and README to the new name, then delete the
  shim. Arc08 must document and verify the new wolong fetch/install/migration
  path rather than preserve the old one by default.
