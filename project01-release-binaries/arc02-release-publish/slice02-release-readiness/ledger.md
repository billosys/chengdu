# Slice 02 (arc02): release-readiness

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-06. Closer: CC. Verifier: CDC (independent Linux
> linkage re-derivation, pinned-LICENSE checks, release download +
> install-path walk, per-row re-walk). Release/run evidence must link
> the specific artifact.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| L-1 | `docs/license-audit-v0.1.0.md` enumerates, per shipped binary per platform (6 total), every third-party component whose objects are linked in — each claim carrying a build-evidence pointer (link-command line, `nm`/`otool` output). The bliss/lpsolve not-linked suggestion from arc01 is confirmed or refuted explicitly. | CDC re-derives Linux linkage in sandbox (build log link lines + `nm` on artifacts); checks macOS claims for internal consistency vs build logs | serious | arc-plan D5; slice-doc §2 | done | attested — audit written with real evidence for all 6 binaries: macOS link lines from this session's own build (parser: own objects only; grounder: own objects + h2-fd-preprocessor's 12 `.cc.o` files + `cpddl/libpddl.a` + `boruvka/libboruvka.a`, no bliss/lpsolve/opts; engine: own modules only) cross-checked against `nm`/`otool -L` on the compiled macOS binaries (0 matches for bliss/lpsolve/cplex/cudd in any binary; h2-specific symbols like `compute_h2_mutexes`/`CausalGraph` present in the grounder). Linux confirmed independently via a **fresh** CI run triggered specifically for this audit (prior run logs had aged out of GitHub's retention): run `31072336131`, job `build (ubuntu-22.04)`, fetched via `gh api .../jobs/92522675318/logs` after `gh run view --log` returned empty in this environment — same object/archive set as macOS. bliss/lpsolve/opts confirmed **not** linked (absent from both platforms' link lines; 0 binary symbols). | one real tooling snag disclosed in the audit itself: `gh run view --log` returned empty output for reasons not diagnosed; the direct REST API call was the evidence channel actually used |
| L-2 | Every component enumerated in L-1 has its license identified from its own LICENSE file at the pinned SHA (license name + file path + SHA quoted), including the named risk item `h2-fd-preprocessor` regardless of L-1's outcome for it. | CDC opens each cited LICENSE path at the cited SHA and confirms the identification | serious | arc-plan D5; slice-doc §2 | done | attested — all 9 components identified with path+SHA: `pandaPIparser`/`pandaPIgrounder`/`pandaPIengine`/`cpddl`/`boruvka`/`opts` are BSD 3-Clause from their own `LICENSE`/`BSD-LICENSE` files at their pinned SHAs; `bliss` is LGPL-3.0 (`COPYING.LESSER` header) and `lpsolve` is LGPL (in-source header in `lp_lib.h`) — both not linked (L-1), no obligation. **`h2-fd-preprocessor` carries no LICENSE file anywhere in its tree at any commit** (exhaustive `find` + `git log --all` across all reachable history, both empty) — identified instead from its own README's explicit statement of Fast Downward provenance, cross-checked against Fast Downward's own canonical repo (`aibasel/downward/LICENSE.md`, fetched live): unambiguously GPL-3.0. Treated conservatively as GPL-3.0-covered per the audit's own reasoning (§2.2.1). | the risk item named in cc-prompt is exactly the one with no license file of its own — the audit's central finding |
| L-3 | `THIRD-PARTY-LICENSES` is generated consistently with the audit (per-binary sections, full license texts for every shipped component) and is present in the `v0.1.0` asset set. | diff its component list against L-1's tables; check the release assets | serious | arc-plan D5 | done | attested + reproduced — `licenses/` carries the 6 exact texts the audit identifies as obligated (the 3 own-component BSDs + cpddl + boruvka + h2-fd-preprocessor's GPL-3.0, sourced from `bliss/COPYING`'s own vendored canonical FSF text, disclosed in `licenses/README.md`). `package-release.sh` assembles them into `THIRD-PARTY-LICENSES` with per-component headers, failing loudly if any is missing (tamper test: removed the h2 license file locally, refused before producing output, exit 1; restored, re-verified clean). **Present in the real `v0.1.0` release**, downloaded and checksummed independently — `sha256sum -c SHA256SUMS`: `THIRD-PARTY-LICENSES: OK`. | |
| L-4 | The obligations L-2 implies are demonstrably satisfied by the released artifacts: attribution for permissive components; if any copyleft component ships, the governing license text + a Source-availability section (repos + exact SHAs + patch set) in the release notes; and the audit outcome + adopted compliance path recorded as a tracked arc-plan amendment per RQ2. | read the release notes + THIRD-PARTY-LICENSES against the audit's obligations section; check arc-plan Version History | serious | arc-plan RQ2 | done | attested + reproduced — `v0.1.0`'s release notes (read via `gh release view`) carry a "Licensing and source availability" section naming the GPL-3.0 finding and pointing at `THIRD-PARTY-LICENSES` + the audit doc, plus a "Source availability" section naming `pandaPIgrounder`'s repo (SHA in the provenance table above it) and a permalink to the audit doc **at the exact release commit** (`.../blob/427d365.../docs/license-audit-v0.1.0.md`, confirmed resolvable via `gh api .../contents/...?ref=427d365...`) for the transitive `h2-fd-preprocessor`/`cpddl`/`boruvka` SHAs — chosen deliberately over hardcoding those SHAs in the notes template, since `package-release.sh` has no access to `upstream/`'s git state at packaging time and hardcoded values would go stale. `arc-plan.md` v1.1 records RQ2's resolution (documentation-based compliance, no build change) as a dated, attributed Version History entry. | |
| L-5 | Tarballs now include `fixtures/` (the byte-verified minimal pair); README's "Install from release" section reaches a passing `pandaPIparser --verify` on those shipped fixtures in ≤ 5 commands per platform, with checksum verification inside the path. | CDC executes the Linux path verbatim from the real release; CC attests macOS; command count checked literally | serious | project ledger P4; slice-doc §3 | done | attested (macOS) — README's "Install from release" section: 4 commands per platform (curl download, checksum verify, extract, chained parse→ground→solve→convert→verify), rehearsed end-to-end locally before writing, then **walked verbatim from the real `v0.1.0` release** in a clean scratch directory (L-9's transcript is this row's macOS evidence): `shasum -a 256 -c SHA256SUMS --ignore-missing` → `OK`; final command ends `Plan verification result: true`, exit 0. Both platforms' tarballs confirmed to contain `fixtures/domain.hddl` + `fixtures/problem.hddl` (checked directly via `tar tzf` on both downloaded tarballs). Linux verbatim walk is CDC's (slice-doc §5). | this section is written as wolong's frozen fetch spec, per slice-doc §3 |
| L-6 | D4 enacted: `project-plan.md` candidates updated (`macos-x86_64` declined, `linux-arm64` deferred) with a dated v1.2 Version History entry naming this slice and the rationale. | read project-plan.md diff + Version History | correctness | arc-plan D4 | done | attested — `project-plan.md` §1's candidate list rewritten: `linux-arm64` **deferred** (GH ARM runners exist, stays a candidate), `macos-x86_64` **declined** (GH Intel runners deprecated, no consumer demand). v1.2 Version History entry dated 2026-08-06, names arc02 slice02 and D4 as source, states no scope/DoD change. | |
| L-7 | The fallback re-evaluation is dispositioned and recorded in the arc-plan change log (default proposal: code kept fetch-only, docs repointed to release binaries as the restricted-network path); README/`fetch-upstream.sh --help` consistent with whatever is decided. | read arc-plan change log + README + script help | correctness | arc01 closing-report bubble-up | done | attested — recorded as arc-plan D6 (resolved 2026-08-06) plus a v1.2 Version History entry: the default proposal adopted as-is (no operator override received at open-set review) — `fetch-upstream.sh --source pandadealer` code unchanged (already fetch-only per slice03's guard), README's fallback note and the script's own `--help` output both now lead with "prefer installing a v0.1.0+ release instead" before describing the vendored-snapshot fallback as historical/source-inspection only. Read both `README.md` and `./scripts/fetch-upstream.sh --help` directly to confirm the wording landed. | |
| L-8 | `v0.1.0` is cut: tag → green run → published release (NOT pre-release) whose assets are exactly: 2 tarballs (each 3 binaries + provenance.txt + fixtures/), `SHA256SUMS`, `release-manifest.txt`, `THIRD-PARTY-LICENSES`; notes carry compat floors + provenance table (+ Source availability if L-4 requires); `v0.0.1-rc1` remains as historical evidence. Cut only after L-1–L-7 are done — the tag is the slice's last act. | release page + run fetched independently; asset list exact; ordering checked against commit/evidence timestamps | serious | arc ledger R-3/R-5; arc-plan D1 | done | attested + reproduced — tag `v0.1.0` pushed only after L-1 through L-7 all reached `done` (checked explicitly before pushing). Run https://github.com/billosys/chengdu/actions/runs/31074130778 (commit `427d365`): green, `package + publish release` job succeeded. Release page (`gh release view v0.1.0`): `prerelease: false` (confirmed NOT pre-release), exactly 5 assets (2 tarballs + `SHA256SUMS` + `release-manifest.txt` + `THIRD-PARTY-LICENSES`), notes carry both compat floors, the 6-row provenance table, and the Source-availability section. Independently downloaded: `sha256sum -c SHA256SUMS` → all 4 checksummed files `OK`; both tarballs contain the correct 6 entries each (3 binaries + provenance.txt + fixtures/ x2). `v0.0.1-rc1` (slice01's rehearsal) remains published, unaffected. | **a real, one-way-door bug was caught and fixed before this cut**, not during it — see the "What Worked" section below |
| L-9 | Consumer rehearsal: the README install path executed verbatim from the real `v0.1.0` release on macOS (CC), ending in the passing fixture verify; transcript in evidence. (Linux verbatim walk is CDC's, at verification/arc close.) | CC transcript; CDC repeats on Linux | correctness | slice-doc §3 | done | attested — full transcript captured (121 lines, preserved in this session's scratchpad, key excerpts quoted in `closing-report.md`) from a clean `/tmp` scratch directory, executing the exact 4 macOS commands extracted programmatically from `README.md` (not retyped): real `curl` download from the public `v0.1.0` release URLs, `shasum -a 256 -c SHA256SUMS --ignore-missing` → `OK`, `tar xzf`, then the chained parse→ground→solve→convert→verify command — ending `Plan verification result: true`, `EXIT CODE: 0`. | the first real consumer of this release is this row, exactly as the ledger names it |

## What Worked

- Triggering a **fresh** CI run specifically to gather Linux link-line
  evidence, rather than either trusting stale (and, it turned out,
  inaccessible) old run logs or falling back to pure architectural
  inference, kept L-1 genuinely evidence-based on both platforms rather
  than evidence-based-on-one-platform-and-assumed-on-the-other.
- When `gh run view --log` silently returned empty output for reasons
  never diagnosed, falling back to the raw `gh api .../jobs/<id>/logs`
  REST call immediately rather than concluding "logs are unavailable"
  saved the audit's Linux evidence from being weaker than it needed to
  be.
- Reading `h2-fd-preprocessor`'s own source tree exhaustively (not just
  its top level) before concluding "no license file" — and then
  checking Fast Downward's *own current* canonical repo live rather
  than recalling its license from memory — is exactly the "verify,
  don't assume" standard arc-plan D5 set, applied to the single most
  consequential claim in this slice.
- **Rehearsing the new packaging format in real CI before the one-way
  `v0.1.0` cut caught a genuine, would-have-been-permanent bug**:
  `package-release.sh` built `THIRD-PARTY-LICENSES` and included it in
  `SHA256SUMS`, but `publish-release.sh`'s `gh release create` argument
  list was never updated to actually upload it — the entire artifact
  this slice exists to ship would have been silently absent from the
  real release. Caught by `v0.0.2-rc1`'s rehearsal, fixed, hardened with
  an explicit presence check for every non-tarball asset (so this class
  of gap can't recur silently), and re-proven via a second rehearsal
  (`v0.0.2-rc2`) before ever touching the real tag. This is precisely
  why cc-prompt named the cut "a one-way door" and put the rehearsal
  before it in the dependency order.
- Extracting the exact install-path commands from `README.md`
  programmatically for both the local pre-write rehearsal and the real
  L-9 consumer walk (rather than retyping them) guaranteed the
  documented path and the tested path were byte-identical, the same
  discipline the `readme-verbatim*` CI jobs apply mechanically.
- Templating the release notes' Source-availability section to point at
  the audit doc *at the exact release commit* (a permalink), instead of
  hardcoding transitive submodule SHAs `package-release.sh` has no way
  to regenerate from `dist/` artifacts alone, kept the notes correct by
  construction rather than correct-until-the-next-pin-bump.

## Closure

Closed at commit `427d365` on 2026-08-06 (the `v0.1.0`-cut commit).
Verified by: CC (this session; real published `v0.1.0` release, a real
red-then-fixed rehearsal cycle, and a real consumer-rehearsal
transcript). CDC independent re-walk: pending — and, per cc-prompt,
this slice's bubble-up feeds the arc close directly.
Rows: 9. Done: 9. Deferred: 0. No-op: 0.
