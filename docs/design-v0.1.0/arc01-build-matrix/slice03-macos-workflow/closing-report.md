# chengdu arc01 / slice03 — macos-workflow — closing report

> Written by CC (this session). Per-row walk of `ledger.md`'s 8 rows,
> then the bubble-up to `arc-plan.md`, per `PROJECT-MANAGEMENT.md` Part IV
> and `LEDGER-DISCIPLINE.md` Section A. **This is arc01's final slice** —
> this bubble-up feeds directly into the arc close. CDC's independent
> review (`cdc-verification.md`), followed by CDC assembling the arc
> `closing-report.md`, is the next step — not performed here.

## Per-row walk

Rows: 8. Done: 8. Deferred: 0. No-op: 0. Primary evidence: one push to
`main` (commit `fd4ae6a`), the whole 8-job matrix green on the first
try — https://github.com/billosys/chengdu/actions/runs/31065314655. Every
row below cites this run, per "one run, one story."

**M-1** — macOS build leg, pinned runner, lints clean. **done.**
`build-macos` job runs on `macos-15`, chosen from a live survey of
`actions/runner-images`' README (fetched 2026-08-06): only two GA arm64
macOS images currently exist, `macos-15` and `macos-26`
(`macos-latest`); `macos-14`/`macos-14-xlarge` (arm64) is marked
deprecated; `xcode-27` is Preview, not GA. `macos-15` — the oldest
maintained GA arm64 image — was chosen as the build/compat floor,
mirroring how `ubuntu-22.04` was chosen for Linux in slice02; `macos-26`
is reserved for the forward-compat check. `actionlint` 1.7.12: exit 0,
zero findings, both locally and in the `lint` CI job. The brew
prerequisites step (`brew install gcc make cmake flex bison gengetopt
zip`) is byte-identical to README's macOS prereq block.

**M-2** — macOS leg green end-to-end. **done.** `build (macos-15)` job
green, all 13 steps, in run 31065314655; log ends
`smoke-test.sh: SUMMARY: 5 passed, 0 failed (positive...)` then
`4 passed, 0 failed (negative...)`. Artifact
`dist-macos-arm64-macos-15-fd4ae6a` downloaded and confirmed to contain
exactly the three binaries plus `provenance.txt`.

**M-3** — `readme-verbatim-macos` byte-identical. **done.** Job green in
the verified run. Confirmed via a scripted, line-indentation-based
extraction of the job's `run:` block against README's macOS-prereq and
five-command fenced blocks: exact match. Worth naming: the first
comparison script used a `re.DOTALL` regex that silently over-matched
past the intended job boundary into the next job's YAML, producing an
implausible `False`; caught by reading the diff output before trusting
it, rewritten as an explicit indentation walk, then re-verified `True`.

**M-4** — patch/compiler fidelity on the runner. **done.** Build log
shows all four `applying patch:` lines in order (`0002-makefile.patch`,
`0001-Removed-non-macos-call-in-unused-function.patch`,
`0001-boruvka-endian.patch`, `bliss-0.73-cxx11-string-literal.patch`).
The artifact's `provenance.txt`: grounder's `patches=` field lists all
four; `compiler=Apple clang version 17.0.0 (clang-1700.0.13.5)` for all
three components — the same compiler identity as this session's local
field-proven build, now reproduced on an actual GitHub-hosted runner
rather than only the operator's machine.

**M-5** — `cross-compat-macos` green. **done.** Job green (12s) in the
verified run: downloads the `macos-15` artifact, all three binaries
print usage under `macos-26`, exit 0. Unlike slice02 (where the
equivalent gap was discovered mid-slice and patched in — bubble-up 2b),
this job was authored into the workflow from the start per slice-doc §2,
applying that lesson at design time rather than as a fix.

**M-6** — `check-provenance.sh`: exists, gates all build legs, provably
fails. **done.** `shellcheck scripts/*.sh` exit 0; `bash -n` and stock
`/bin/bash -n` (3.2) both clean. Three independent local tamper tests on
macOS, each a distinct, correctly-attributed, nonzero-exit failure: SHA
mismatch (`FAIL: pandaPIgrounder: sha mismatch — ...`), patch-list
mismatch (macOS's 4-patch set truncated to Linux's 2-patch set —
`FAIL: pandaPIgrounder: patches mismatch on macos-arm64 — ...`), and an
emptied compiler field (`FAIL: pandaPIparser: compiler field is
empty`); the clean file re-verified passing after each. In CI (run
31065314655), the `Check provenance` step is present and green on all
three build legs: `macos-15` (`... macos-arm64`), `ubuntu-22.04` and
`ubuntu-24.04` (`... linux-x86_64` each). This retires slice02's
W-4/W-5 residue class permanently — provenance is now enforced by every
green run, not attested after the fact.

**M-7** — README CI section + badge. **done.** README's "Continuous
integration" section names `build (macos-15)`, `cross-compat-macos`, and
`readme-verbatim-macos` alongside the Linux legs, plus the brew-version
float accepted-risk note. Badge line unchanged
(`.../workflows/build.yml/badge.svg`); workflow confirmed still named
`build` via the GitHub API.

**M-8** — no Linux regression. **done.** All 8 jobs green in run
31065314655: `actionlint`, `build (ubuntu-22.04)`, `build
(ubuntu-24.04)`, `cross-compat`, `readme-verbatim`, `build (macos-15)`,
`cross-compat-macos`, `readme-verbatim-macos`. `check-provenance.sh`'s
addition to both ubuntu legs (new this slice) did not regress either —
both `Check provenance` steps green.

## Bubble-up to the arc

**1. Did this slice deliver the piece of the arc's capability the
arc-plan assigned it?** Yes, against arc-plan §2's slice03 line ("macOS
(arm64) job in the same workflow: brew deps, patch application, grounder
compiler override, smoke gate, artifacts") — delivered, plus the
explicitly-assigned extras (`readme-verbatim-macos`,
`cross-compat-macos`, `check-provenance.sh` on all legs, the min-OS
observation). The arc's capability statement — "any push to chengdu
builds all three pandaPI components from pinned upstream SHAs, applies
the patch set, runs the full verified gate sequence... and uploads the
binaries as workflow artifacts — on both platforms, every time" — is now
demonstrated end-to-end in one run (M-2, M-8). Arc ledger rows A1–A4 now
have direct evidence available (run 31065314655, both artifacts, the
patches/ directory unchanged since slice01) for CDC's composition check
at arc close; closing those rows is explicitly CDC's step, not this
report's.

**2. What did implementing this slice reveal that the arc-plan did not
anticipate?** Nothing that changes the arc-plan's capability statement,
slice breakdown, or open questions — OQ1/OQ2/OQ3 were already resolved
by v1.3, and this slice's work confirms OQ2 (macOS grounder compiler is
clang) on a real runner rather than only the operator's local machine,
which is expected slice work, not a surprise. One concrete new data
point for **arc02** specifically, not an arc-plan change: the min-OS
observation (`scripts/record-min-os.sh`) recorded `min_os=15.0` for all
three macOS binaries built on the `macos-15` runner — meaning, as built
today, these binaries will not run on macOS versions older than 15.
Arc02's release-packaging work needs this number as an input to its
macOS compat-floor decision (the slice-doc named this row "observed and
recorded, not yet a policy" — the observation is now on record; the
policy call is arc02's).

**3. The silent-drop diff at slice scale.** Scope-as-delivered matches
scope-as-specified (slice-doc §2) exactly. In-scope items are all
present and verified (M-1 through M-8). Out-of-scope items are all
explicitly named in slice-doc §2's "Out" list and none were silently
started or dropped: releases/tags/checksums/manifest (arc02, now
consuming both platforms' artifacts plus CI-validated provenance),
`macos-x86_64` (not added — this slice's runner survey found no GA
arm64 concerns needing it, and the decision stays arc02's), universal
(fat) binaries via `lipo` (not investigated or implemented — the
slice-doc's own instruction was "do not implement speculatively," and
no runner-survey finding made it look cheap enough to reconsider),
brew-version pinning for deps (not pinned, disclosed as accepted risk in
README's CI section, matching slice-doc exactly), and the Node20
deprecation action-version bumps (CDC-5; still present as non-blocking
annotations in run 31065314655 — not forced to act mid-slice, correctly
left for arc02's workflow touch). Nothing is missing from either list.
