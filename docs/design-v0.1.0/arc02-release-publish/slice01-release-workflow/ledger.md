# Slice 01 (arc02): release-workflow

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-06. Closer: CC. Verifier: CDC (public run/release
> pages, authenticated clone, local packaging reproduction, per-row
> re-walk). Run/release evidence must link the specific run/release.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| G-1 | Build+gate logic is refactored into one reusable workflow consumed by both the push/PR workflow and `release.yml`; no logic duplicated between them; `actionlint` zero findings on all workflow files. | read workflows at HEAD; grep for duplicated step blocks; actionlint | correctness | slice-doc §2 | done | attested — `.github/workflows/build-reusable.yml` (workflow_call target) carries all 7 jobs (lint, build×2, cross-compat, build-macos, cross-compat-macos, readme-verbatim×2). `build.yml` and `release.yml` are both thin callers (`jobs: call-build: uses: ./.github/workflows/build-reusable.yml`) — zero step blocks duplicated between them (grep confirms no `run:`/`steps:` content outside build-reusable.yml except release.yml's own `publish` job, which is new logic, not a duplicate of anything). `actionlint .github/workflows/*.yml`: exit 0, zero findings, locally and as the `lint` CI job (all four files) | |
| G-2 | `scripts/package-release.sh` run locally against both platforms' dist trees produces: two correctly-named tarballs containing exactly 3 binaries + `provenance.txt` each; a `SHA256SUMS` that verifies (`sha256sum -c`) against every asset; a `release-manifest.txt` whose per-platform blocks reconcile with `pins.env` and each tarball's embedded provenance. Tamper test: a corrupted tarball fails checksum verification; a manifest/pins mismatch is refused (via `check-provenance.sh` reuse). shellcheck clean, bash 3.2. | CDC reproduces in sandbox: run, unpack, `sha256sum -c`, reconcile, tamper | serious | slice-doc §2; arc ledger R-5 | done | attested (macOS local) + reproduced (real release, see G-3) — local run against a real macOS dist tree: correct tarball name (`pandapi-<tag>-macos-arm64.tar.gz`), exactly 4 files inside (3 binaries + provenance.txt), `SHA256SUMS` verifies clean. Corrupted-tarball tamper test: flipped one byte, `sha256sum -c` reports `FAILED`, exit 1; restored, re-verified clean. Manifest/pins tamper test: tampered `dist/macos-arm64/provenance.txt`'s parser SHA, `package-release.sh` refuses before producing any output (`check-provenance.sh: FAIL: pandaPIparser: sha mismatch`, exit 1, `release/` left empty); restored, re-verified clean. `shellcheck scripts/*.sh` exit 0; `bash -n` / stock `/bin/bash -n` (3.2) both clean. Real-release reconciliation (G-3's release, both platforms): downloaded assets' `sha256sum -c` verifies against the shipped `SHA256SUMS`; `release-manifest.txt`'s per-platform blocks match `pins.env` and each tarball's own embedded `provenance.txt` byte-for-byte | one bug found and fixed during this row's own testing: the notes-template patch-count used `wc -l` on comma-turned-newlines, undercounting by one (4 patches read as 3) — fixed to `awk -F',' '{print NF}'`, re-verified correct in the real release's notes (G-5) |
| G-3 | A pre-release test tag runs the full path: reusable build green on all legs → package → publish; the resulting GitHub Release (marked pre-release) carries exactly the asset set (2 tarballs + SHA256SUMS + manifest + notes). | release page fetched independently; asset list checked; run linked | serious | arc ledger R-3 (rehearsal) | done | attested — tag `v0.0.1-rc1`, run https://github.com/billosys/chengdu/actions/runs/31069094843 (commit `d9b21f8`), `package + publish release` job green. Release page (https://github.com/billosys/chengdu/releases/tag/v0.0.1-rc1): `prerelease: true`, exactly 4 assets (`pandapi-v0.0.1-rc1-linux-x86_64.tar.gz`, `pandapi-v0.0.1-rc1-macos-arm64.tar.gz`, `SHA256SUMS`, `release-manifest.txt`) plus the rendered notes as the release body. Independently downloaded via `gh release download`: `sha256sum -c SHA256SUMS` verifies all three checksummed files clean; both tarballs contain exactly the 4 expected files; extracted macOS binaries run `--help` (exit 0, all three) on this machine, matching `provenance.txt`'s recorded SHAs/patches/compiler/min-OS block exactly | real `v0.1.0` is slice02's, after the license audit; this tag proves the machinery only, kept published (pre-release-marked, not deleted) as durable evidence per slice-doc §2's "cleaned up or clearly marked pre-release — CC's call, disclosed" |
| G-4 | Refuse-to-publish demonstrated: a deliberately-broken build under a test tag → linked red run AND no corresponding release in the releases list; restoration commit linked; test tag cleaned up. | run page (red) + releases page (absence) fetched independently | serious | project ledger P2 (building block); arc ledger R-4 | done | attested — branch `refusal-test-rehearsal` (deleted after gathering evidence; commits remain reachable by SHA), tampered `pins.env`'s `PARSER_SHA` to an all-zero SHA (commit `5fea2d7`), tagged `v0.0.0-rc-refusal-test`, pushed. Run https://github.com/billosys/chengdu/actions/runs/31069859859: **failure** — every `build`/`readme-verbatim*` job red at the `Fetch upstream`/`Run README verbatim` step (`git: fatal: reference is not a tree`, exit 128), every downstream step (including both `cross-compat*` jobs) skipped, `package + publish release` job never started (`needs: call-build` correctly gated on the whole matrix). Absence confirmed independently: `gh release view v0.0.0-rc-refusal-test` → `release not found` (exit 1); `gh release list` shows only the legitimate `v0.0.1-rc1`. Restoration commit: `b4394f9` ("Restore pins.env after refuse-to-publish rehearsal"), `pins.env` diffed back to byte-identical with pre-rehearsal state and re-verified (`fetch-upstream.sh` succeeds again). Test tag deleted (local + remote) after evidence gathering; the branch itself was deleted too — the one commit worth keeping (the `run_engine` timeout fix, see Notes) was fast-forwarded onto `main` (`233e0a5`) *before* the deliberate breakage, so nothing of value was lost | this row's rehearsal surfaced a real, unrelated bug — see the "What Worked" section below |
| G-5 | The published test release's notes render from the template with: tag, per-platform provenance summary (SHAs, patch counts), and both compat floors (macOS 15+ arm64; glibc 2.35+ x86_64) exactly as arc-plan D2/D3 state. | read the release body on the release page | correctness | arc-plan D2/D3 | done | attested — `v0.0.1-rc1`'s release body (read via `gh release view`): tag `v0.0.1-rc1` throughout; "Compatibility" section states `Linux: glibc 2.35+ (x86_64) — built and verified on ubuntu-22.04` and `macOS: macOS 15+ (arm64, Apple Silicon)`, matching D2/D3 verbatim; "Provenance" table has one row per platform×component (6 rows), each with the correct upstream SHA and the correct patch count (Linux grounder: 2; macOS grounder: 4 — the post-fix correct counts, see G-2) | |
| G-6 | CDC-5 retired: all `actions/*` uses at current majors; the verified run's annotations contain no Node20 deprecation warnings. | grep workflow `uses:`; run page annotations | polish | slice02 (arc01) cdc-verification CDC-5 | done | attested — majors resolved from each action's actual `/releases/latest` (not recalled): `actions/checkout@v7`, `actions/upload-artifact@v7`, `actions/download-artifact@v8`; each confirmed `using: node24` in its `action.yml` before pinning. `grep -rn uses: .github/workflows/*.yml` shows every action reference at these majors, consistently, across all four workflow files. Run https://github.com/billosys/chengdu/actions/runs/31068904658: zero "Node.js 20 is deprecated" annotations (grep-confirmed absent from the run's full annotation text) — the two annotations present are unrelated Homebrew tap-trust notices | prior runs (arc01 close, slice02) carried this warning on `checkout@v4`/`upload-artifact@v4`/`download-artifact@v4`; this run is the first clean one |
| G-7 | No CI behavior change from the refactor: a post-refactor push run is green with the same job set and gates as arc01's close run (8 jobs incl. both verbatim + both cross-compat + provenance steps). | compare job lists: post-refactor run vs run 31065314655 | correctness | slice-doc §2 | done | attested — run https://github.com/billosys/chengdu/actions/runs/31068904658 (commit `d9b21f8`, pre-dating the branches-only trigger fix but otherwise the reusable-workflow refactor in full): 8 jobs, all green — `actionlint`, `build (ubuntu-22.04)`, `build (ubuntu-24.04)`, `cross-compat`, `build (macos-15)`, `cross-compat-macos`, `readme-verbatim (ubuntu-22.04)`, `readme-verbatim-macos (macos-15)` — the exact same 8 names as arc01's close run 31065314655, now nested under the `call-build` reusable-workflow-call job instead of listed flat, with identical per-job step lists (fetch → three builds → `Check provenance` → smoke ×2 → upload, or the two-job verbatim/cross-compat shapes) | job *nesting* changed (flat → `call-build / <job>`), job *identity, count, and step content* did not — this is what "the refactor moves callers, not logic" predicted |
| G-8 | Workflow/README docs current: CI section notes the release workflow (tag-triggered, refuses on red, draft toggle documented); release idempotency behavior documented in `release.yml` header. | read README + workflow header | polish | slice-doc §3 | done | attested — README's new "Releases" section (`## Releases`, after "Continuous integration") states: tag-triggered (`v*`), runs the identical build+gate matrix, publish only reachable if the whole matrix is green, direct-publish default with the one-line draft-mode pointer, and the idempotency-fails-loudly behavior with a pointer to the workflow header. `release.yml`'s top comment block documents the same three points (trigger/gating, idempotency contract, draft-mode toggle) as the authoritative version; README summarizes it, doesn't duplicate it verbatim | consumer "Install from release" docs remain slice02's, per slice-doc §2's Out list |

## What Worked

- Resolving CDC-5's action majors from each action's real
  `/releases/latest` and confirming `using: node24` in its `action.yml`
  — rather than guessing at current version numbers — meant the bump
  was verifiably correct before ever pushing, not just plausible.
- The `git: fatal: reference is not a tree` failure mode (from slice01,
  reused deliberately for G-4) is exactly the kind of fail-loud,
  unambiguous error that makes a refusal demonstration legible at a
  glance on the run page — no digging through logs to confirm what
  broke.
- Rehearsing the refusal locally first (confirming `fetch-upstream.sh`
  fails in ~2 seconds against the tampered pin) before ever pushing
  meant the CI run's outcome was never in doubt — the push was
  confirmation, not discovery.
- The rehearsal surfaced a real, unrelated bug: a deliberately-broken
  *fixture* (the first break attempted, before switching to the pins.env
  approach for the actual CI demo) fed the engine a malformed `.sas`
  file, which spun indefinitely — 12+ GB RSS and climbing when killed —
  because no gate in `smoke-test.sh` bounded the engine call with a
  timeout. Every other failure mode in this project fails loud and fast;
  this one would have hung a CI job for hours with no FAIL message at
  all. Fixed (`run_engine`, a portable background-watcher timeout, no
  dependency on GNU coreutils' `timeout`/`gtimeout`) and landed on `main`
  independently of the rehearsal itself, since it's real, general
  robustness, not rehearsal-specific.
- The fix for that bug contained the *same* CDC-3-shaped mistake it was
  supposed to prevent elsewhere: `wait "$watcher"` after killing the
  watcher returns nonzero, and under `set -e` that silently aborted the
  calling function — swallowing the FAIL message and the summary line
  entirely, discovered only because the summary line's absence looked
  wrong on inspection, not because anything crashed loudly. Fixed by
  moving `set -e`'s re-enable to after both cleanup commands. Worth
  naming: a countermeasure for a known failure class isn't automatically
  immune to instances of that same class in its own implementation.
- Choosing the pins.env-tamper mechanism (not the fixture-break
  mechanism) for the actual CI refusal demonstration — after the
  fixture-break rehearsal surfaced the engine-hang bug locally — kept
  the real CI evidence-gathering run fast (~15-25s per leg to a clean
  fail) rather than waiting out a 60-second timeout on every leg.
- Fast-forwarding `main` to include the `run_engine` fix *before*
  committing the deliberate breakage on the rehearsal branch meant the
  valuable fix was safe on `main` regardless of what happened next on
  the rehearsal branch, and the branch could be deleted afterward
  without losing anything.

## Closure

Closed at commit `233e0a5` on 2026-08-06.
Verified by: CC (this session; CI evidence via GitHub-hosted runners,
a real published pre-release, and a real red run with confirmed release
absence). CDC independent re-walk: pending.
Rows: 8. Done: 8. Deferred: 0. No-op: 0.
