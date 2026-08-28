# chengdu arc02 / slice02 — release-readiness — closing report

> Written by CC (this session). Per-row walk of `ledger.md`'s 9 rows,
> then the bubble-up per `PROJECT-MANAGEMENT.md` Part IV. **This is
> arc02's final slice** — this bubble-up feeds the arc close directly.
> CDC's independent review (`cdc-verification.md`), then CDC assembling
> the arc closing-report, is the next step — not performed here.

## Per-row walk

Rows: 9. Done: 9. Deferred: 0. No-op: 0. Primary evidence: the real,
published, non-prerelease `v0.1.0` release
(https://github.com/billosys/chengdu/releases/tag/v0.1.0, run
https://github.com/billosys/chengdu/actions/runs/31074130778, commit
`427d365`), backed by `docs/license-audit-v0.1.0.md`, and a real
consumer-rehearsal transcript.

**L-1** — linkage audit, evidence-based, all 6 binaries. **done.**
`docs/license-audit-v0.1.0.md` traces every binary's actual link
command from a real build log (macOS: this session's own build;
Linux: a **fresh** CI run triggered specifically for this audit, since
prior run logs had aged out of GitHub's retention — worked around a
silent `gh run view --log` failure by calling the REST API directly)
and cross-checks against `nm`/`otool -L` on the compiled macOS
binaries. Confirms: `pandaPIparser` and `pandaPIengine` link no
third-party code on either platform; `pandaPIgrounder` links
`h2-fd-preprocessor` (12 object files), `cpddl`, and `boruvka`; `bliss`,
`lpsolve`, and `opts` are built but **not** linked (0 matching symbols
in the compiled binary) — arc01's suggestion, now verified rather than
assumed.

**L-2** — license identification, including the named risk item.
**done.** All 9 investigated components identified with a LICENSE
path and pinned SHA, except `h2-fd-preprocessor`, whose vendored copy
carries **no license file anywhere in its tree at any commit**
(exhaustive `find` + full `git log` history, both confirmed empty) —
identified instead from its own README's explicit Fast Downward
provenance claim, cross-checked live against Fast Downward's current
canonical repository: unambiguously GPL-3.0.

**L-3** — `THIRD-PARTY-LICENSES` generated and shipped. **done.**
`licenses/` carries the 6 obligated texts; `package-release.sh`
assembles them with per-component headers and refuses to package if
any is missing (tamper-tested locally). Present and checksum-verified
in the real `v0.1.0` release.

**L-4** — obligations satisfied, RQ2 recorded. **done.** `v0.1.0`'s
release notes carry a Licensing section and a Source-availability
section pointing at `pandaPIgrounder`'s SHA (in the provenance table)
and a permalink to the audit doc at the exact release commit — chosen
over hardcoding transitive SHAs the packaging script has no way to
regenerate. `arc-plan.md` v1.1 records RQ2's resolution
(documentation-based compliance, no build change) as a dated,
attributed Version History entry.

**L-5** — fixtures in tarballs, install docs real. **done.** README's
"Install from release" section: 4 commands per platform, rehearsed
locally before writing, then walked verbatim from the real release
(L-9). Both platforms' tarballs confirmed to contain `fixtures/`.

**L-6** — D4 enacted. **done.** `project-plan.md` §1: `macos-x86_64`
declined, `linux-arm64` deferred, v1.2 Version History entry dated and
attributed.

**L-7** — fallback disposition recorded. **done.** arc-plan D6
(resolved): the PandaDealer fallback stays fetch-only; README and
`fetch-upstream.sh --help` both now lead restricted-network users to
release binaries first. No operator override received at open-set
review; the slice-doc's default proposal was adopted as-is.

**L-8** — `v0.1.0` cut. **done.** Pushed only after L-1 through L-7 all
reached `done`. Run 31074130778 green; release `prerelease: false`,
exactly 5 assets, notes correct; independently downloaded and
checksum-verified. **A real bug was caught before this row closed, not
during it** — see the bubble-up below.

**L-9** — consumer rehearsal. **done.** A 121-line transcript, captured
from a clean scratch directory, running the exact 4 macOS commands
extracted programmatically from `README.md`: real download from the
public `v0.1.0` URLs, checksum `OK`, extraction, and the full gate
chain ending `Plan verification result: true`, exit 0.

## Bubble-up — feeding the arc close directly

**Is arc02's capability now real?** Yes, stated plainly: *"A git tag
turns verified artifacts into a GitHub Release with checksums and a
provenance manifest"* — now demonstrated end-to-end with a real,
non-prerelease tag (`v0.1.0`), a real download-and-verify by an actual
consumer walk (L-9), and license obligations discharged with
evidence, not assumption. The arc's roadmap line is delivered.

**Evidence set for CDC, arc ledger R-1 through R-6:**

- **R-1/R-2** (slices closed cleanly): this closing-report + `ledger.md`
  (slice02); slice01's `closing-report.md` + `cdc-verification.md`
  (already accepted).
- **R-3** (a real tag produces a release whose assets download-verify
  and gate-pass on clean machines): `v0.1.0`, run `31074130778`. macOS
  half **reproduced by this session's own consumer rehearsal** (L-9,
  transcript in evidence, ending in a passing `--verify`). Linux half
  is explicitly CDC's per slice-doc §5 — the same install commands,
  same release, same expected outcome.
- **R-4** (refuse-to-publish, reproduced): inherited unchanged from
  slice01 (`v0.0.0-rc-refusal-test`, run `31069859859`) — this slice's
  own rehearsal cycle (`v0.0.2-rc1` → bug found → fixed → `v0.0.2-rc2`)
  is *additional*, not substitute, evidence that the gate structure
  holds under real mistakes, not just deliberate ones.
- **R-5** (manifest reconciles): `v0.1.0`'s `release-manifest.txt`
  matches `pins.env` and each tarball's embedded `provenance.txt`,
  independently checksum-verified (L-3, L-8).
- **R-6** (bubble-up findings dispositioned): this section, plus the
  finding named below.

**What this slice revealed that the arc-plan did not anticipate:** one
item, already resolved, not a gap for CDC or the operator to carry
forward — recorded here because Part IV asks the question explicitly,
not because it's still open. `publish-release.sh`'s asset-upload list
was not automatically kept in sync with `package-release.sh`'s output:
when this slice added `THIRD-PARTY-LICENSES` to the packaging step, the
publish step's `gh release create` argument list was not updated to
match, and the first rehearsal (`v0.0.2-rc1`) published successfully
*without* the one asset this entire slice exists to ship. This is
exactly why the rehearsal-before-the-real-cut step existed in the
dependency order cc-prompt specified — caught before `v0.1.0`, not
during it. Fixed (the upload list corrected) and hardened (an explicit
presence check for every non-tarball asset now runs before any `gh`
call, so a future asset added to packaging but not to publishing fails
loudly instead of shipping silently short). No arc-plan change is
needed; this is a slice-internal process finding, disclosed for the
record and because it's a real lesson for whoever next touches
`package-release.sh`/`publish-release.sh` together: **the two scripts'
asset lists must be changed as a pair, and that pairing should be
tested in a real rehearsal, not just read-reviewed.**

**The silent-drop diff at slice scale.** Scope-as-delivered matches
scope-as-specified (slice-doc §3) exactly. All "In" items delivered
(L-1 through L-9). "Out" items are all explicitly named and none were
silently started or dropped: signing/attestation (future-work note
only), no build change in response to the audit (documentation-based
compliance was sufficient — confirmed, not assumed, by the audit
itself), wolong-side fetch code (arc03's, against now-frozen formats),
and upstream contribution tickets (still open, parallel work,
unaffected by this slice).
