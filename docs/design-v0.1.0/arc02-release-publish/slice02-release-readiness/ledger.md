# Slice 02 (arc02): release-readiness

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-06. Closer: CC. Verifier: CDC (independent Linux
> linkage re-derivation, pinned-LICENSE checks, release download +
> install-path walk, per-row re-walk). Release/run evidence must link
> the specific artifact.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| L-1 | `docs/license-audit-v0.1.0.md` enumerates, per shipped binary per platform (6 total), every third-party component whose objects are linked in — each claim carrying a build-evidence pointer (link-command line, `nm`/`otool` output). The bliss/lpsolve not-linked suggestion from arc01 is confirmed or refuted explicitly. | CDC re-derives Linux linkage in sandbox (build log link lines + `nm` on artifacts); checks macOS claims for internal consistency vs build logs | serious | arc-plan D5; slice-doc §2 | open | | assertion without evidence pointer = row not done |
| L-2 | Every component enumerated in L-1 has its license identified from its own LICENSE file at the pinned SHA (license name + file path + SHA quoted), including the named risk item `h2-fd-preprocessor` regardless of L-1's outcome for it. | CDC opens each cited LICENSE path at the cited SHA and confirms the identification | serious | arc-plan D5; slice-doc §2 | open | | the grounder's link line already includes h2 objects (arc01 evidence) — this row decides the distribution terms |
| L-3 | `THIRD-PARTY-LICENSES` is generated consistently with the audit (per-binary sections, full license texts for every shipped component) and is present in the `v0.1.0` asset set. | diff its component list against L-1's tables; check the release assets | serious | arc-plan D5 | open | | |
| L-4 | The obligations L-2 implies are demonstrably satisfied by the released artifacts: attribution for permissive components; if any copyleft component ships, the governing license text + a Source-availability section (repos + exact SHAs + patch set) in the release notes; and the audit outcome + adopted compliance path recorded as a tracked arc-plan amendment per RQ2. | read the release notes + THIRD-PARTY-LICENSES against the audit's obligations section; check arc-plan Version History | serious | arc-plan RQ2 | open | | outcome-neutral: the audit decides what "satisfied" requires |
| L-5 | Tarballs now include `fixtures/` (the byte-verified minimal pair); README's "Install from release" section reaches a passing `pandaPIparser --verify` on those shipped fixtures in ≤ 5 commands per platform, with checksum verification inside the path. | CDC executes the Linux path verbatim from the real release; CC attests macOS; command count checked literally | serious | project ledger P4; slice-doc §3 | open | | this section is wolong arc03's fetch spec — write it as such |
| L-6 | D4 enacted: `project-plan.md` candidates updated (`macos-x86_64` declined, `linux-arm64` deferred) with a dated v1.2 Version History entry naming this slice and the rationale. | read project-plan.md diff + Version History | correctness | arc-plan D4 | open | | plan-change discipline: body change distinguishable, entry dated + attributed |
| L-7 | The fallback re-evaluation is dispositioned and recorded in the arc-plan change log (default proposal: code kept fetch-only, docs repointed to release binaries as the restricted-network path); README/`fetch-upstream.sh --help` consistent with whatever is decided. | read arc-plan change log + README + script help | correctness | arc01 closing-report bubble-up | open | | operator may override the default at open-set review |
| L-8 | `v0.1.0` is cut: tag → green run → published release (NOT pre-release) whose assets are exactly: 2 tarballs (each 3 binaries + provenance.txt + fixtures/), `SHA256SUMS`, `release-manifest.txt`, `THIRD-PARTY-LICENSES`; notes carry compat floors + provenance table (+ Source availability if L-4 requires); `v0.0.1-rc1` remains as historical evidence. Cut only after L-1–L-7 are done — the tag is the slice's last act. | release page + run fetched independently; asset list exact; ordering checked against commit/evidence timestamps | serious | arc ledger R-3/R-5; arc-plan D1 | open | | one-way door: botched cut = delete release+tag, retag (release.yml header) |
| L-9 | Consumer rehearsal: the README install path executed verbatim from the real `v0.1.0` release on macOS (CC), ending in the passing fixture verify; transcript in evidence. (Linux verbatim walk is CDC's, at verification/arc close.) | CC transcript; CDC repeats on Linux | correctness | slice-doc §3 | open | | the first real consumer is this row |

## What Worked

_(At slice close.)_

## Closure

_Open. Rows: 9. Done: 0. Deferred: 0. No-op: 0._
