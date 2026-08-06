# chengdu arc01 / slice02 — linux-workflow — closing report

> Written by CC (this session). Per-row walk of `ledger.md`'s 9 rows, then
> the bubble-up to `arc-plan.md`, per `PROJECT-MANAGEMENT.md` Part IV and
> `LEDGER-DISCIPLINE.md` Section A. CDC's independent review (run/log
> re-read, artifact download + execution, `actionlint` re-run, per-row
> re-walk) is the next step, not yet performed.

## Per-row walk

Rows: 9. Done: 9. Deferred: 0. No-op: 0. Primary evidence: two pushes to
branch `slice02-linux-workflow`, the second (commit `808579a`) fully
green across all five jobs —
https://github.com/oubiwann/chengdu/actions/runs/31060179607. W-2 through
W-6 all cite this one run, per cc-prompt's "one run, one story."

**W-1** — workflow exists, lints clean. **done.**
`.github/workflows/build.yml` triggers on `push`, `pull_request`,
`workflow_dispatch`; all four `uses:` lines pinned to a major
(`actions/checkout@v4`, `actions/download-artifact@v4`,
`actions/upload-artifact@v4`). `actionlint` (1.7.12, matching the CI
job's pinned version): exit 0, zero findings, both locally and as the
`lint` CI job.

**W-2** — one linked run, green on both runners. **done.** Run
31060179607: `build (ubuntu-22.04)` and `build (ubuntu-24.04)` both
green, canonical fetch → three builds → smoke positive (5/5) → smoke
negative (4/4) on each, each job's log ending in the smoke script's
`SUMMARY: N passed, 0 failed` terminal lines.

**W-3** — gitlab.com reachability, recorded. **done.** Both build jobs'
`Fetch upstream` step logs show `Submodule 'cpddl'
(https://gitlab.com/danfis/cpddl.git) registered for path 'cpddl'`
followed by a successful clone and checkout at the pinned SHA.
`arc-plan.md` v1.3 records OQ1's reachability half as resolved, citing
this run.

**W-4** — slice01 F-4/F-9 Linux re-entry. **done.** The 22.04 artifact's
`provenance.txt` grounder block: SHA matches `pins.env` exactly, two
patches applied (`0002-makefile.patch`, `bliss-0.73-cxx11-string-literal.patch`
— the Linux-correct count, no macOS-only boruvka patches), compiler
`g++ (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0`. Build log shows both
`applying patch:` lines. This is the mechanical discharge slice01's
`cdc-verification.md` named as the re-entry condition for its
environment-blocked Linux half.

**W-5** — artifact contents + cross-runner `--help`. **done.** Both
artifacts (`dist-linux-x86_64-ubuntu-22.04-808579a`,
`dist-linux-x86_64-ubuntu-24.04-808579a`) downloaded, each containing
exactly the 4 expected files. 22.04-on-22.04 proven by that job's own
smoke run; 22.04-on-24.04 proven by a dedicated `cross-compat` job added
mid-slice (see bubble-up below) — all three binaries print usage under
an actual `ubuntu-24.04` runner, job green. **Deviation from the ledger's
suggested Verify, disclosed:** local Docker could not reach the Docker
Hub registry from this sandbox (network-blocked), so the
container-based check the ledger names wasn't performed locally. The
`cross-compat` CI job substitutes stronger evidence (a real GitHub-hosted
`ubuntu-24.04` runner, not a local emulation of one) for the same claim.

**W-6** — `readme-verbatim` byte-identical to README. **done.** Job
green in run 31060179607. Confirmed two ways: reading the raw job log
(the six executed command lines match README's fenced blocks verbatim,
in order) and a scripted extraction-and-compare (parsed both README's
```bash``` blocks and the workflow's `run:` block, joined, diffed —
exact match, zero differences).

**W-7** — CDC-3 remediated. **done.** Every previously-bare pandaPI
invocation now runs inside an explicit `set +e`/`set -e` bracket (the
positive-mode solve gate) or through a new `require_step` helper (the
unsolvable-fixture and `--corpus` setup calls), which itself wraps the
existing `run_and_capture`. Crash simulation performed locally on macOS:
swapped `dist/macos-arm64/pandaPIengine` for a two-line `exit 3` stub,
ran both smoke modes. Positive mode: `FAIL: solve: expected exit 0 +
'Status: Solved', got exit 3`, overall exit 1 — a labeled failure, not a
bash abort. Negative mode: same pattern on the unsolvable fixture's
engine call, exit 1. Restored the real binary, re-ran clean (5/5, 4/4).
`shellcheck scripts/*.sh` exit 0; `bash -n` and stock `/bin/bash -n`
(3.2) both clean throughout.

**W-8** — fetch-only fallback guard. **done.** `fetch-upstream.sh --help`
documents the fallback as fetch-only with CDC-1's three concrete reasons
(no patch files; pre-final concepts-TS syntax; missing `<cstdint>`).
README's fallback note states the same. `build-grounder.sh` now checks
`[ -L "$SRC_ROOT" ]` (true only when `fetch-upstream.sh --source
pandadealer` symlinked the tree) and fails fast with a message pointing
back to the README note. Verified locally: `fetch-upstream.sh --source
pandadealer && ./scripts/build-grounder.sh` — guard fires immediately,
exit 1, no attempt at CDC-1's confusing "patch does not apply" path.
Canonical mode re-fetched and re-verified afterward to restore working
state.

**W-9** — README badge + CI section. **done.** Badge line:
`https://github.com/oubiwann/chengdu/actions/workflows/build.yml/badge.svg`,
linking to the workflow's actual page; confirmed the workflow file is
named `build.yml` with `name: build` via `gh api
repos/oubiwann/chengdu/actions/workflows`. A "Continuous integration"
section names the 22.04/24.04 matrix, the `readme-verbatim` tripwire, and
the `actionlint` gate, and states where artifacts land.

## Bubble-up to the arc

**1. Did this slice deliver the piece of the arc's capability the
arc-plan assigned it?** Yes, against arc-plan §2's slice02 line ("Linux
job: deps via apt, calls the slice01 scripts, uploads artifacts, smoke
gate green required") — delivered and exceeded: also a `readme-verbatim`
tripwire job, an `actionlint` gate, a `cross-compat` job, the CDC-3 fix,
and the OQ1 fallback disposition, all of which slice-doc §2 assigned to
this slice specifically (not scope creep — cc-prompt named all of them
explicitly). The arc capability's Linux half (arc-plan A1/A2: one green
run, both platforms, artifacts complete and `--help`-able) is now
demonstrated end-to-end; the macOS half remains slice03's.

**2. What did implementing this slice reveal that the arc-plan did not
anticipate?** Two things, disposed differently: **(a)** OQ1 and OQ3 were
open questions this slice was explicitly assigned to close — closing
them is expected slice work, not a surprise, but per the slice-close
arc-plan-update step this session updated `arc-plan.md` to v1.3 recording
both resolutions with linked evidence, rather than leaving the answers
implicit in this ledger alone. **(b)** A narrower, genuine gap: slice-doc
§2's job enumeration (apt deps → fetch → three builds → smoke → smoke
negative → upload) does not by itself satisfy its own ledger's W-5
criterion — proving the 22.04-built artifact also runs under 24.04 needs
a job that crosses matrix legs, which two independently-run matrix jobs
structurally cannot provide (each only proves an artifact on the runner
that built it). This slice added a `cross-compat` job to close that gap;
future slice-docs that specify a build matrix plus a cross-runner
compatibility claim should name the cross-checking job explicitly rather
than leaving it to be inferred from the ledger. No arc-plan change is
needed for this — it's a slice-doc-authoring lesson, not a capability or
sequencing change.

**3. The silent-drop diff at slice scale.** Scope-as-delivered matches
scope-as-specified (slice-doc §2) exactly, plus the disclosed addition
in (2b) above. In-scope items are all present and verified (W-1 through
W-9). Out-of-scope items are all explicitly named in slice-doc §2's "Out"
list and none were silently started or dropped: the macOS job (slice03),
release/checksum/manifest machinery (arc02), `linux-arm64` (not added,
correctly), build caching (not added, correctly — recorded as a
deliberate non-goal, not drift), and corpus smoke in CI (the `--corpus`
flag stays local-only, correctly not wired into the workflow). Nothing
is missing from either list.
