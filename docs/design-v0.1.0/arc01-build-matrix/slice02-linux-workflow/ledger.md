# Slice 02: linux-workflow

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-05. Closer: CC. Verifier: CDC (independent run/log
> review, artifact download + execution in the sandbox, actionlint re-run,
> per-row re-walk). Evidence citing a CI run must link the specific run.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| W-1 | `.github/workflows/build.yml` exists; triggers on `push`, `pull_request`, `workflow_dispatch`; all action versions pinned to a major (`@vN`); `actionlint` reports zero findings. | `actionlint .github/workflows/build.yml; echo $?`; `grep -E 'uses: .+@v[0-9]+' build.yml` covers every `uses:` | correctness | slice-doc §2 | open | | |
| W-2 | One linked workflow run, green on BOTH `ubuntu-22.04` and `ubuntu-24.04`: canonical fetch → build-parser → build-grounder → build-engine → smoke positive (5/5) → smoke negative (4/4), each step's log showing its PASS/OK terminal line. | run URL; CDC reads each job's step logs | serious | slice-doc §1; arc ledger A1 | open | | this row is the arc capability's Linux half |
| W-3 | The green run's fetch step log shows the cpddl submodule cloned from gitlab.com on a GH runner — arc-plan OQ1's reachability question answered empirically and recorded in the arc-plan (tracked change). | grep the fetch step log for `gitlab.com/danfis/cpddl`; arc-plan Version History entry exists | correctness | arc-plan OQ1 | open | | if reachability FAILS, this row becomes the evidence for re-opening OQ1's disposition — either way it closes with a recorded answer |
| W-4 | Slice01 re-entry discharged: the same green run proves `build-grounder.sh` (canonical, Linux) and the grounder's `provenance.txt` line (SHA = pin, patches list, compiler) — slice01 F-4/F-9 Linux halves, upgraded from environment-blocked to reproduced. | CDC downloads the 22.04 artifact, checks `provenance.txt` grounder block against `pins.env`; run log shows the two `applying patch:` lines expected on Linux | serious | slice01 cdc-verification (re-entry) | open | | close slice01's loop explicitly — cite this row from the arc-plan if OQ1 entry touches it |
| W-5 | The 22.04 artifact contains exactly `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, `provenance.txt`; all three binaries execute `--help` with exit 0 on a fresh 22.04 container AND on 24.04 (glibc forward-compat, OQ3's baseline choice enacted); the 24.04 artifact is uploaded under a distinct name. | CDC downloads both artifacts; runs `--help` under `ubuntu:22.04` and `ubuntu:24.04` containers (sandbox) | serious | slice-doc §2; arc-plan OQ3 | open | | 22.04 set is what arc02 will package |
| W-6 | The `readme-verbatim` job runs the README's prerequisite apt line + its five build/smoke commands *exactly as written* (no added flags, no reordering) on clean `ubuntu-22.04`, green in the linked run. | CDC diffs the job's `run:` block against the README code blocks — byte-identical commands; job green in run | correctness | slice01 F-11 re-entry | open | | drift between README and workflow = this job red |
| W-7 | CDC-3 remediated: no bare pandaPI invocation in `smoke-test.sh` can abort the script under `set -e` without a per-gate FAIL; demonstrated by a crash simulation (stub engine exiting 3) producing a graceful FAIL message + overall nonzero exit; shellcheck still clean; bash-3.2 rules still hold. | run the crash simulation (documented in the script header or a test note); `shellcheck scripts/*.sh` | correctness | slice01 cdc-verification CDC-3 | open | | fix pattern: route through `run_and_capture` or `\|\| RC=$?` |
| W-8 | Fallback declared fetch-only: `fetch-upstream.sh --help` and README state it; `build-grounder.sh` on a PandaDealer layout fails fast with a message pointing at that documentation (not CDC-1's misleading patch error). | run `fetch-upstream.sh --source pandadealer && build-grounder.sh` in the sandbox → observe the new guard message + nonzero exit; grep README | correctness | arc-plan v1.2 (CDC-1 disposition a) | open | | disposition (c) — retire into releases — re-evaluated at arc02 close, per arc-plan |
| W-9 | README carries the CI badge (pointing at the workflow) and a short CI paragraph naming the matrix and where artifacts land; badge renders green against the linked run's branch. | view README on the repo host; badge URL matches workflow file name | polish | slice-doc §2 | open | | |

## What Worked

_(At slice close.)_

## Closure

_Open. Rows: 9. Done: 0. Deferred: 0. No-op: 0._
