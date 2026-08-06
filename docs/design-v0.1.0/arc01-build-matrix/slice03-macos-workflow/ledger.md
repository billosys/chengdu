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
| M-1 | The workflow carries an arm64 macOS build leg on an explicitly pinned runner version (no `macos-latest`), with a brew setup step byte-identical to the README's macOS prereq line; `actionlint` zero findings; all `uses:` still pinned. | read workflow at HEAD; `actionlint`; diff brew step vs README block | correctness | slice-doc §2 | open | | runner choice + rationale recorded in Evidence |
| M-2 | The verified run's macOS leg is green end-to-end: canonical fetch → three builds → smoke positive (5/5) → smoke negative (4/4) → artifact `dist-macos-arm64-<runner>-<sha>` uploaded containing the three binaries + `provenance.txt`. | run URL; job step list; artifact listed on run page | serious | arc ledger A1/A2 (macOS half) | open | | this row completes the arc capability's platform matrix |
| M-3 | `readme-verbatim-macos` job green on a clean pinned macOS runner, its `run:` block byte-identical to the README's macOS prereq + five-command blocks. | CDC re-derives the diff; job green in the verified run | correctness | project ledger P4 (macOS half) | open | | macOS twin of slice02 W-6 |
| M-4 | The macOS leg demonstrates patch/compiler fidelity: build log shows all four `applying patch:` lines (0002, two boruvka, bliss) and the artifact's provenance records an Apple clang compiler line for the grounder — i.e., the field-proven local path reproduced on a runner. | run log (attested) + M-6's mechanical gate (reproduced) + provenance content | serious | runbook §2-macOS; arc-plan OQ2 | open | | M-6 makes most of this row machine-checked rather than log-read |
| M-5 | `cross-compat-macos` green in the verified run: the three binaries built on the older pinned arm64 runner execute `--help` (exit 0) on the newer pinned arm64 runner. | job green on run page | serious | slice02 bubble-up 2b (leg-crossing lesson) | open | | |
| M-6 | `scripts/check-provenance.sh` exists, is shellcheck-clean and bash-3.2-safe, validates every component block (SHA = pin; per-platform patch list exact; compiler non-empty), runs as a step on ALL build legs (both platforms) in the verified run, and demonstrably fails: a tamper test (mismatched pin or patch list) produces a named complaint + nonzero exit. | run script locally against sandbox dist (clean + tampered); confirm step present in workflow + green on all legs in the verified run | correctness | slice02 W-4/W-5 residue (operator-routed) | open | | retires the attested-provenance residue class permanently |
| M-7 | README's CI section names the macOS legs (build, readme-verbatim, cross-compat) and the brew-version float caveat; badge unchanged and green. | read README at HEAD; badge on repo page | polish | slice-doc §2 | open | | |
| M-8 | No Linux regression: the same verified run has ALL slice02 legs green (both ubuntu builds, cross-compat, readme-verbatim, actionlint) alongside the new macOS legs. | run page job list — every job green in one run | correctness | slice-doc §5 | open | | the arc's full matrix, green in one run, is arc ledger A1's closing evidence |

## What Worked

_(At slice close.)_

## Closure

_Open. Rows: 8. Done: 0. Deferred: 0. No-op: 0._
