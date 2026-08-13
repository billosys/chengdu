# Slice 03: macos-workflow

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-06. Closer: CC. Verifier: CDC (public run pages,
> authenticated-HEAD file review, local provenance-gate reproduction,
> per-row re-walk). Evidence citing a CI run must link the specific run;
> "the verified run" below means one green run cited consistently across
> rows, per "one run, one story."

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| M-1 | The workflow carries an arm64 macOS build leg on an explicitly pinned runner version (no `macos-latest`), with a brew setup step byte-identical to the README's macOS prereq line; `actionlint` zero findings; all `uses:` still pinned. | read workflow at HEAD; `actionlint`; diff brew step vs README block | correctness | slice-doc §2 | done | attested — `build-macos` job runs on `macos-15`. Runner choice sourced from `actions/runner-images`' README (fetched 2026-08-06): 2 GA arm64 macOS images exist, `macos-15` and `macos-26` (`macos-latest`); `macos-14`/`macos-14-xlarge` (arm64) is marked deprecated; `xcode-27` is Preview, not GA. `macos-15` chosen as the oldest maintained GA arm64 image (the build/compat floor, mirroring how `ubuntu-22.04` was chosen for Linux in slice02); `macos-26` reserved for `cross-compat-macos`. `actionlint` 1.7.12: exit 0, zero findings, locally and in the `lint` CI job. `Install prerequisites` step: `brew install gcc make cmake flex bison gengetopt zip` — byte-identical to README's macOS prereq block (confirmed by the same programmatic extract-and-compare used for M-3). All `uses:` lines remain pinned to a major. | |
| M-2 | The verified run's macOS leg is green end-to-end: canonical fetch → three builds → smoke positive (5/5) → smoke negative (4/4) → artifact `dist-macos-arm64-<runner>-<sha>` uploaded containing the three binaries + `provenance.txt`. | run URL; job step list; artifact listed on run page | serious | arc ledger A1/A2 (macOS half) | done | attested — run https://github.com/billosys/chengdu/actions/runs/31065314655 (commit `fd4ae6a`), `build (macos-15)` job green, all 13 steps green; log ends `smoke-test.sh: SUMMARY: 5 passed, 0 failed (positive...)` then `4 passed, 0 failed (negative...)`; `dist-macos-arm64-macos-15-fd4ae6a` artifact downloaded, contains exactly `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, `provenance.txt`. | first push, no iteration needed — full 8-job matrix green on the first try |
| M-3 | `readme-verbatim-macos` job green on a clean pinned macOS runner, its `run:` block byte-identical to the README's macOS prereq + five-command blocks. | CDC re-derives the diff; job green in the verified run | correctness | project ledger P4 (macOS half) | done | attested — `readme-verbatim-macos (macos-15)` job green in run 31065314655. Confirmed by a scripted extraction-and-compare (line-based, not a naive DOTALL regex — see Notes) of the job's `run:` block against README's macOS-prereq + five-command fenced blocks: exact match, 6 lines, zero differences. | macOS twin of slice02 W-6; the first extraction attempt used a regex with `re.DOTALL` that over-matched into the following job's YAML — caught immediately by an implausible diff result, rewritten as an explicit line-indentation walk before trusting the "match" |
| M-4 | The macOS leg demonstrates patch/compiler fidelity: build log shows all four `applying patch:` lines (0002, two boruvka, bliss) and the artifact's provenance records an Apple clang compiler line for the grounder — i.e., the field-proven local path reproduced on a runner. | run log (attested) + M-6's mechanical gate (reproduced) + provenance content | serious | runbook §2-macOS; arc-plan OQ2 | done | attested — build log shows all four lines in order: `applying patch: 0002-makefile.patch`, `applying patch: 0001-Removed-non-macos-call-in-unused-function.patch`, `applying patch: 0001-boruvka-endian.patch`, `applying patch: bliss-0.73-cxx11-string-literal.patch`. Downloaded artifact's `provenance.txt`: grounder block's `patches=` lists all four (order-independent match confirmed by M-6's own gate, which passed); `compiler=Apple clang version 17.0.0 (clang-1700.0.13.5)` for all three components — same compiler identity as this session's local field-proven build. | M-6's gate machine-checked the SHA/patch-list/compiler-non-empty claims; this row's log-read confirms the specific four-patch identity M-6 doesn't name individually |
| M-5 | `cross-compat-macos` green in the verified run: the three binaries built on the older pinned arm64 runner execute `--help` (exit 0) on the newer pinned arm64 runner. | job green on run page | serious | slice02 bubble-up 2b (leg-crossing lesson) | done | attested — `cross-compat-macos (macos-15 artifact on macos-26)` job green (12s), run 31065314655: downloads `dist-macos-arm64-macos-15-fd4ae6a`, all three binaries print `Usage: ...` under `macos-26`, job exits 0. Authored into the workflow from the start this slice (per slice-doc §2), rather than discovered mid-slice as in slice02. | |
| M-6 | `scripts/check-provenance.sh` exists, is shellcheck-clean and bash-3.2-safe, validates every component block (SHA = pin; per-platform patch list exact; compiler non-empty), runs as a step on ALL build legs (both platforms) in the verified run, and demonstrably fails: a tamper test (mismatched pin or patch list) produces a named complaint + nonzero exit. | run script locally against sandbox dist (clean + tampered); confirm step present in workflow + green on all legs in the verified run | correctness | slice02 W-4/W-5 residue (operator-routed) | done | attested (macOS local) + attested (CI, all 3 legs) — `shellcheck scripts/*.sh` exit 0; `bash -n` / stock `/bin/bash -n` (3.2) both clean. Local tamper tests (macOS): SHA mismatch → `FAIL: pandaPIgrounder: sha mismatch — ...`, exit 1; patch-list mismatch (macOS 4-set truncated to Linux's 2-set) → `FAIL: pandaPIgrounder: patches mismatch on macos-arm64 — ...`, exit 1; emptied compiler field → `FAIL: pandaPIparser: compiler field is empty`, exit 1 — three distinct, named, correctly-attributed failures, all nonzero. Clean-file positive control also re-run and passed after each tamper test. In CI (run 31065314655): `Check provenance` step present and green on all three build legs — `build (macos-15)`: `check-provenance.sh: OK: ... (macos-arm64)`; `build (ubuntu-22.04)` and `build (ubuntu-24.04)`: `check-provenance.sh: OK: ... (linux-x86_64)` each. | retires slice02's W-4/W-5 residue class permanently: provenance is now CI-enforced on every green run, not attested after the fact |
| M-7 | README's CI section names the macOS legs (build, readme-verbatim, cross-compat) and the brew-version float caveat; badge unchanged and green. | read README at HEAD; badge on repo page | polish | slice-doc §2 | done | attested — README's "Continuous integration" section names `build (macos-15)`, `cross-compat-macos`, and `readme-verbatim-macos` explicitly alongside the Linux legs, and states the brew-version-float accepted-risk note. Badge line unchanged: `https://github.com/billosys/chengdu/actions/workflows/build.yml/badge.svg` linking to the workflow page; confirmed the workflow is still named `build` via `gh api repos/billosys/chengdu/actions/workflows`. | |
| M-8 | No Linux regression: the same verified run has ALL slice02 legs green (both ubuntu builds, cross-compat, readme-verbatim, actionlint) alongside the new macOS legs. | run page job list — every job green in one run | correctness | slice-doc §5 | done | attested — run 31065314655, all 8 jobs green in one run: `actionlint`, `build (ubuntu-22.04)`, `build (ubuntu-24.04)`, `cross-compat`, `readme-verbatim`, `build (macos-15)`, `cross-compat-macos`, `readme-verbatim-macos`. `check-provenance.sh`'s addition to the two ubuntu legs (new this slice) did not regress either — both `Check provenance` steps green, `OK: ... (linux-x86_64)`. | this is arc ledger A1's closing evidence: the full two-platform matrix, green, in one run |

## What Worked

- Sourcing the runner-pin decision from `actions/runner-images`' own
  README (fetched live, not recalled) rather than guessing avoided
  pinning to something already deprecated (`macos-14`) or a floating
  alias (`macos-latest`) — the kind of mistake that would only surface
  as a confusing scheduling failure weeks later when GitHub actually
  removed the image.
- Authoring `cross-compat-macos` into the workflow from the start,
  instead of discovering the leg-crossing gap mid-slice the way slice02
  did (bubble-up 2b) — applying a lesson at authoring time is cheaper
  than applying it as a fix.
- A full local dry run of the exact CI sequence (fetch → three builds →
  `record-min-os.sh` → `check-provenance.sh` → smoke positive → smoke
  negative) on this machine, matching the macOS runner's own
  architecture, meant the pushed workflow had zero genuinely new
  variables to fail on except the runner environment itself — and none
  did; the whole 8-job matrix went green on the first push.
- The M-3 byte-diff script's first version silently over-matched past
  its intended job boundary (a `re.DOTALL` regex swallowing the next
  job's YAML) and produced a `False` that would have read as "drift
  found" — a false negative catch, not a false positive, but still worth
  naming: even a "just diff two strings" check benefits from a sanity
  read of its own output before trusting it.
- `check-provenance.sh`'s three independent tamper tests (SHA, patch
  list, compiler) each producing a distinctly-worded, correctly
  component-attributed failure — rather than one generic "provenance
  invalid" message — made this row's evidence unambiguous and gave CDC
  a clean re-run target.

## Closure

Closed at commit `fd4ae6a` on 2026-08-06.
Verified by: CC (this session; CI evidence via GitHub-hosted runners,
both platforms). CDC independent re-walk: pending — and, per cc-prompt,
CDC's verification here feeds directly into the arc01 close.
Rows: 8. Done: 8. Deferred: 0. No-op: 0.
