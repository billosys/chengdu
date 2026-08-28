# Slice 02: linux-workflow

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-05. Closer: CC. Verifier: CDC (independent run/log
> review, artifact download + execution in the sandbox, actionlint re-run,
> per-row re-walk). Evidence citing a CI run must link the specific run.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| W-1 | `.github/workflows/build.yml` exists; triggers on `push`, `pull_request`, `workflow_dispatch`; all action versions pinned to a major (`@vN`); `actionlint` reports zero findings. | `actionlint .github/workflows/build.yml; echo $?`; `grep -E 'uses: .+@v[0-9]+' build.yml` covers every `uses:` | correctness | slice-doc §2 | done | attested (local: `actionlint` 1.7.12, exit 0; CI: `lint` job green) | all four `uses:` lines pinned to a major (`actions/checkout@v4`, `actions/download-artifact@v4`, `actions/upload-artifact@v4`); triggers block has `push`, `pull_request`, `workflow_dispatch` |
| W-2 | One linked workflow run, green on BOTH `ubuntu-22.04` and `ubuntu-24.04`: canonical fetch → build-parser → build-grounder → build-engine → smoke positive (5/5) → smoke negative (4/4), each step's log showing its PASS/OK terminal line. | run URL; CDC reads each job's step logs | serious | slice-doc §1; arc ledger A1 | done | attested — run https://github.com/oubiwann/chengdu/actions/runs/31060179607 (commit `808579a`), both `build (ubuntu-22.04)` and `build (ubuntu-24.04)` green; each job's log ends `smoke-test.sh: SUMMARY: 5 passed, 0 failed (positive...)` and `4 passed, 0 failed (negative...)` | first run (31059791259) was already green end-to-end before the `cross-compat` job existed; this run adds that job without breaking the rest |
| W-3 | The green run's fetch step log shows the cpddl submodule cloned from gitlab.com on a GH runner — arc-plan OQ1's reachability question answered empirically and recorded in the arc-plan (tracked change). | grep the fetch step log for `gitlab.com/danfis/cpddl`; arc-plan Version History entry exists | correctness | arc-plan OQ1 | done | attested — both `build` jobs' `Fetch upstream` step logs (run 31060179607) show `Submodule 'cpddl' (https://gitlab.com/danfis/cpddl.git) registered for path 'cpddl'` followed by a successful clone and `Submodule path 'cpddl': checked out '8c142ba...'`; `arc-plan.md` v1.3 records OQ1 resolved (reachability confirmed) | reachability succeeded — no re-opening needed |
| W-4 | Slice01 re-entry discharged: the same green run proves `build-grounder.sh` (canonical, Linux) and the grounder's `provenance.txt` line (SHA = pin, patches list, compiler) — slice01 F-4/F-9 Linux halves, upgraded from environment-blocked to reproduced. | CDC downloads the 22.04 artifact, checks `provenance.txt` grounder block against `pins.env`; run log shows the two `applying patch:` lines expected on Linux | serious | slice01 cdc-verification (re-entry) | done | attested — downloaded `dist-linux-x86_64-ubuntu-22.04-808579a`; `provenance.txt`'s grounder block: `sha=b04ff7bcca710ae80a1ce73b8c027f0224565afc` (matches `pins.env` `GROUNDER_SHA` exactly), `patches=0002-makefile.patch,bliss-0.73-cxx11-string-literal.patch` (2, the Linux-correct count — no boruvka macOS patches), `compiler=g++ (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0`; build log shows both `applying patch:` lines | slice01 F-4/F-9's Linux halves upgrade from environment-blocked to reproduced by this row (final upgrade is CDC's to record in its own re-walk) |
| W-5 | The 22.04 artifact contains exactly `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, `provenance.txt`; all three binaries execute `--help` with exit 0 on a fresh 22.04 container AND on 24.04 (glibc forward-compat, OQ3's baseline choice enacted); the 24.04 artifact is uploaded under a distinct name. | CDC downloads both artifacts; runs `--help` under `ubuntu:22.04` and `ubuntu:24.04` containers (sandbox) | serious | slice-doc §2; arc-plan OQ3 | done | attested — both artifacts (`dist-linux-x86_64-ubuntu-22.04-808579a`, `dist-linux-x86_64-ubuntu-24.04-808579a`) downloaded, each containing exactly the 4 expected files; 22.04-on-22.04 proven by that job's own smoke run; 22.04-on-24.04 proven by the dedicated `cross-compat` job (run 31060179607) — all three `--help` invocations print usage and the job is green | local docker verification was attempted but this sandbox can't reach the Docker Hub registry (network-blocked); the CI `cross-compat` job is the actual evidence and is strictly stronger (a real 24.04 runner, not a local emulation) |
| W-6 | The `readme-verbatim` job runs the README's prerequisite apt line + its five build/smoke commands *exactly as written* (no added flags, no reordering) on clean `ubuntu-22.04`, green in the linked run. | CDC diffs the job's `run:` block against the README code blocks — byte-identical commands; job green in run | correctness | slice01 F-11 re-entry | done | attested — `readme-verbatim (ubuntu-22.04)` job green in run 31060179607; job log's executed command lines match README's fenced blocks exactly (`sudo apt-get install -y g++ make cmake flex bison gengetopt zip git`, then the 5 build/smoke commands); confirmed both by reading the raw log and by a scripted extract-and-compare (README's `bash` code blocks vs. the workflow's `run:` block, byte-for-byte, programmatic diff: match) | |
| W-7 | CDC-3 remediated: no bare pandaPI invocation in `smoke-test.sh` can abort the script under `set -e` without a per-gate FAIL; demonstrated by a crash simulation (stub engine exiting 3) producing a graceful FAIL message + overall nonzero exit; shellcheck still clean; bash-3.2 rules still hold. | run the crash simulation (documented in the script header or a test note); `shellcheck scripts/*.sh` | correctness | slice01 cdc-verification CDC-3 | done | attested (macOS) — every bare invocation now runs inside an explicit `set +e`/`set -e` bracket (solve gate) or through the new `require_step` helper (unsolvable-fixture and corpus setup calls), which itself wraps `run_and_capture`. Crash simulation performed locally: swapped `dist/macos-arm64/pandaPIengine` for a `exit 3` stub, ran both modes — positive mode: `FAIL: solve: expected exit 0 + 'Status: Solved', got exit 3`, exit 1 (not a bash abort); negative mode: `FAIL: negative: unsolvable: expected exit 0 ... got exit 3`, exit 1; restored the real binary, re-ran clean (5/5, 4/4). `shellcheck scripts/*.sh` exit 0; `bash -n` and `/bin/bash -n` (stock 3.2) both clean | reproduction recipe documented in the script's own header comment per the ledger's Notes suggestion |
| W-8 | Fallback declared fetch-only: `fetch-upstream.sh --help` and README state it; `build-grounder.sh` on a PandaDealer layout fails fast with a message pointing at that documentation (not CDC-1's misleading patch error). | run `fetch-upstream.sh --source pandadealer && build-grounder.sh` in the sandbox → observe the new guard message + nonzero exit; grep README | correctness | arc-plan v1.2 (CDC-1 disposition a) | done | attested (macOS) — `fetch-upstream.sh --help` now documents the fallback as FETCH-ONLY with the concrete reasons (no patch files, pre-final concepts-TS syntax, missing `<cstdint>`); README's fallback note states the same and points at the guard. Ran `fetch-upstream.sh --source pandadealer && ./scripts/build-grounder.sh` locally: guard fires immediately — `build-grounder.sh: FAIL: .../upstream/pandaPIgrounder is the PandaDealer fallback (fetch-only) — it does not build. See README.md's 'Continuous integration' fallback note...`, exit 1, no attempt at the misleading patch-application error CDC-1 hit | disposition (c) — retire into releases — stays open for arc02 close, per arc-plan v1.3 |
| W-9 | README carries the CI badge (pointing at the workflow) and a short CI paragraph naming the matrix and where artifacts land; badge renders green against the linked run's branch. | view README on the repo host; badge URL matches workflow file name | polish | slice-doc §2 | done | attested — README's badge line: `https://github.com/oubiwann/chengdu/actions/workflows/build.yml/badge.svg`, linking to `.../actions/workflows/build.yml`, matching the actual workflow file name (`build.yml`, `name: build`) confirmed via `gh api repos/oubiwann/chengdu/actions/workflows`; "Continuous integration" section added naming the 22.04/24.04 matrix, the `readme-verbatim` tripwire, the `actionlint` gate, and where artifacts land | badge will render green once this branch's/main's latest run is the one GitHub shows for the default branch — cosmetic, not gated on this row |

## What Worked

- Running `actionlint` and `shellcheck` locally before ever pushing meant
  every CI iteration was testing genuinely new ground (runner reality),
  not catching mistakes a local tool would have caught for free —
  consistent with cc-prompt's framing that only runner-environment
  discoveries should count against the iteration budget.
- "No build logic in YAML" made the workflow trivial to reason about:
  every step is a one-line call to an already-locally-tested script, so
  the only genuinely new risk surface per run was the runner environment
  itself, not hidden branching in the workflow file.
- Recognizing that W-5's "22.04 artifact on 24.04" claim wasn't actually
  covered by the per-matrix-leg jobs (each only proves an artifact on the
  runner that built it) before treating the row as satisfied — and
  building a small dedicated `cross-compat` job for exactly that gap —
  produced stronger evidence (a real CI-linked run) than the ledger's own
  suggested local-container check would have, once local Docker turned
  out to be network-blocked in this sandbox.
- The whole slice went green in two pushes: the first run was already
  fully green end-to-end; the second only added the `cross-compat` job.
  No failure-driven iteration was needed — the runner had no surprises
  the local scripts hadn't already surfaced during slice01.
- Programmatically diffing the README's fenced code blocks against the
  workflow's `readme-verbatim` `run:` block (rather than eyeballing it)
  made W-6 a mechanical pass/fail instead of a judgment call.

## Closure

Closed at commit <pending — see closing-report.md> on 2026-08-05.
Verified by: CC (this session; CI evidence via GitHub-hosted runners).
CDC independent re-walk: pending.
Rows: 9. Done: 9. Deferred: 0. No-op: 0.
