# Slice 01 (arc02): release-workflow

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-06. Closer: CC. Verifier: CDC (public run/release
> pages, authenticated clone, local packaging reproduction, per-row
> re-walk). Run/release evidence must link the specific run/release.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| G-1 | Build+gate logic is refactored into one reusable workflow consumed by both the push/PR workflow and `release.yml`; no logic duplicated between them; `actionlint` zero findings on all workflow files. | read workflows at HEAD; grep for duplicated step blocks; actionlint | correctness | slice-doc §2 | open | | the no-second-implementation rule, applied to YAML |
| G-2 | `scripts/package-release.sh` run locally against both platforms' dist trees produces: two correctly-named tarballs containing exactly 3 binaries + `provenance.txt` each; a `SHA256SUMS` that verifies (`sha256sum -c`) against every asset; a `release-manifest.txt` whose per-platform blocks reconcile with `pins.env` and each tarball's embedded provenance. Tamper test: a corrupted tarball fails checksum verification; a manifest/pins mismatch is refused (via `check-provenance.sh` reuse). shellcheck clean, bash 3.2. | CDC reproduces in sandbox: run, unpack, `sha256sum -c`, reconcile, tamper | serious | slice-doc §2; arc ledger R-5 | open | | |
| G-3 | A pre-release test tag runs the full path: reusable build green on all legs → package → publish; the resulting GitHub Release (marked pre-release) carries exactly the asset set (2 tarballs + SHA256SUMS + manifest + notes). | release page fetched independently; asset list checked; run linked | serious | arc ledger R-3 (rehearsal) | open | | real `v0.1.0` is slice02's; this proves the machinery |
| G-4 | Refuse-to-publish demonstrated: a deliberately-broken build under a test tag → linked red run AND no corresponding release in the releases list; restoration commit linked; test tag cleaned up. | run page (red) + releases page (absence) fetched independently | serious | project ledger P2 (building block); arc ledger R-4 | open | | the gate must be shown failing, per house rule |
| G-5 | The published test release's notes render from the template with: tag, per-platform provenance summary (SHAs, patch counts), and both compat floors (macOS 15+ arm64; glibc 2.35+ x86_64) exactly as arc-plan D2/D3 state. | read the release body on the release page | correctness | arc-plan D2/D3 | open | | |
| G-6 | CDC-5 retired: all `actions/*` uses at current majors; the verified run's annotations contain no Node20 deprecation warnings. | grep workflow `uses:`; run page annotations | polish | slice02 (arc01) cdc-verification CDC-5 | open | | |
| G-7 | No CI behavior change from the refactor: a post-refactor push run is green with the same job set and gates as arc01's close run (8 jobs incl. both verbatim + both cross-compat + provenance steps). | compare job lists: post-refactor run vs run 31065314655 | correctness | slice-doc §2 | open | | |
| G-8 | Workflow/README docs current: CI section notes the release workflow (tag-triggered, refuses on red, draft toggle documented); release idempotency behavior documented in `release.yml` header. | read README + workflow header | polish | slice-doc §3 | open | | consumer install docs remain slice02's |

## What Worked

_(At slice close.)_

## Closure

_Open. Rows: 8. Done: 0. Deferred: 0. No-op: 0._
