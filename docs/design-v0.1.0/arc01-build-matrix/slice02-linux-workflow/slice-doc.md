# chengdu arc01 / slice02 — linux-workflow — slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md) (v1.2). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Put slice01's substrate under GitHub Actions on Linux. At slice close, every
push and PR to chengdu runs the full canonical pipeline — fetch → three
builds → smoke (positive + negative) — on ubuntu runners, uploads the
verified binaries + provenance as workflow artifacts, and proves the three
things slice01 could not: canonical grounder builds on Linux via the scripts
(slice01 F-4 re-entry), the README's five commands work verbatim on a
genuinely clean machine (F-11 re-entry), and GH runners reach gitlab.com for
the cpddl submodule (arc-plan OQ1's empirical half). CI is a *caller* of the
slice01 scripts — no build logic lives in YAML.

## 2. Scope

**In:**

- `.github/workflows/build.yml` — triggers: `push`, `pull_request`,
  `workflow_dispatch`. Linux jobs only (macOS is slice03; the workflow's
  job structure should make adding the macOS job additive, not surgical).
- **Runner matrix: `ubuntu-22.04` and `ubuntu-24.04`.** This settles
  arc-plan OQ3: release binaries will come from the *oldest* supported LTS
  (glibc 2.35 baseline — parser and grounder link `-static`, but the engine
  is dynamic, so the build floor defines the compat floor); the newer
  runner is the forward-compat check. Artifacts are uploaded from the
  22.04 job (canonical) and the 24.04 job (labeled distinctly, for
  comparison only — arc02 packages only the 22.04 set).
- Job steps: apt deps (mirroring README) → `fetch-upstream.sh` (canonical)
  → the three build scripts → `smoke-test.sh` → `smoke-test.sh --negative`
  → upload `dist/linux-x86_64/` (3 binaries + `provenance.txt`) via
  `actions/upload-artifact`, artifact name carrying platform + runner +
  short commit SHA.
- A separate **`readme-verbatim` job**: executes the README's prerequisite
  line and its five build commands *exactly as written* on a clean
  `ubuntu-22.04` runner — the mechanical discharge of slice01 F-11's
  Linux half. If the README and the main job's steps drift, this job is
  the tripwire.
- **`actionlint` gate** — the workflow file itself passes actionlint (the
  shellcheck of GH Actions), locally and as a CI step.
- **CDC-3 remediation** (routed from slice01's verification): fix
  `smoke-test.sh`'s bare engine/parser/grounder invocations under
  `set -e` (positive solve gate; negative gate d) so an unexpected crash
  produces a per-gate FAIL message and nonzero exit, not a silent bash
  abort. Verify includes a simulated-crash test (stub binary that exits 3).
- **OQ1 disposition, implemented** (proposed at planning, operator-reviewed
  with this doc): the PandaDealer fallback is declared **fetch-only** —
  `fetch-upstream.sh --help` and the README say so plainly, and
  `build-grounder.sh` detects the PandaDealer layout and fails with a
  pointer to that documentation instead of the misleading patch error
  CDC-1 observed. Compat patches for the 2023 snapshot are explicitly NOT
  built (low value: canonical works everywhere a runner runs); revisit at
  arc02 close, when chengdu's own releases become the natural fallback.
- README: CI badge; a "Continuous integration" paragraph (what the matrix
  covers, where artifacts land); the fallback fetch-only note.

**Out (disclosed, not dropped):**

- macOS job — slice03 (the workflow skeleton this slice leaves behind is
  its on-ramp).
- Releases, tags, checksums, manifest packaging — arc02 (consumes the
  22.04 artifacts and the provenance byproduct as-is).
- `linux-arm64` — project-plan candidate, not committed; do not add
  speculatively.
- Build caching (upstream clones, object files) — deliberate non-goal this
  slice; determinism first, optimization when run-time hurts. Recorded so
  its later addition is a tracked change, not drift.
- Corpus smoke (`--corpus`) in CI — optional flag stays local-only for
  now; pulling `ipc2023-domains` into CI is an arc02-or-later decision.

## 3. Constraints

- **No build logic in YAML.** Workflow steps call `scripts/*.sh`; any
  logic worth branching on belongs in the scripts, where it is
  shellcheck-gated and locally runnable. The one exception: runner
  bookkeeping (checkout, artifact upload, badge plumbing).
- Any edit to `scripts/*.sh` preserves slice01's constraints: bash 3.2
  compatible, shellcheck clean, fail-loud, distinct failure messages.
- Workflow pins action versions (e.g. `actions/checkout@v4`,
  `actions/upload-artifact@v4`) — the pins-not-floats discipline applied
  to CI dependencies.

## 4. Verification approach

The CI runs themselves are the primary evidence — linked run URLs, green
across the matrix, artifact downloads. CC implements, pushes, and iterates
against real runs (the runner is the only honest test bed for a workflow).
CDC verification: independent review of the linked runs and logs (gitlab
reachability observed in the fetch step's output), download of the
artifacts and execution of the binaries' `--help`/smoke locally in the
sandbox, `actionlint` re-run, and the per-row re-walk. Rows whose Verify is
"a green run at CI" carry evidence strength `reproduced` when CDC has
independently confirmed the linked run and artifact rather than trusting
the summary.

## 5. Exit criteria

The ledger's 9 rows at final status — see [`./ledger.md`](./ledger.md).
Shape: workflow exists and lints (W-1); the matrix runs green end-to-end
(W-2); gitlab reachability recorded (W-3); slice01's grounder/provenance
Linux re-entry discharged (W-4); artifacts complete and runnable (W-5);
README verbatim job green (W-6); CDC-3 fixed with crash-path proof (W-7);
fallback declared fetch-only in code and docs (W-8); badge + CI docs in
README (W-9).
