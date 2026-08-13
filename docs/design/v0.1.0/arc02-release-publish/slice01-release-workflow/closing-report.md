# chengdu arc02 / slice01 — release-workflow — closing report

> Written by CC (this session). Per-row walk of `ledger.md`'s 8 rows,
> then the bubble-up to the arc, per `PROJECT-MANAGEMENT.md` Part IV and
> `LEDGER-DISCIPLINE.md` Section A. This arc has two slices — this
> bubble-up shapes slice02's open set directly. CDC's independent review
> (`cdc-verification.md`) is the next step, not performed here.

## Per-row walk

Rows: 8. Done: 8. Deferred: 0. No-op: 0. Primary evidence: three linked
artifacts — the post-refactor push run
(https://github.com/billosys/chengdu/actions/runs/31068904658), the
published pre-release test tag `v0.0.1-rc1`
(https://github.com/billosys/chengdu/releases/tag/v0.0.1-rc1, run
https://github.com/billosys/chengdu/actions/runs/31069094843), and the
refusal-demonstration red run
(https://github.com/billosys/chengdu/actions/runs/31069859859).

**G-1** — reusable workflow, no duplication. **done.**
`build-reusable.yml` (`workflow_call`) carries all 7 build/gate jobs;
`build.yml` and `release.yml` are both thin callers with zero step
content of their own beyond `release.yml`'s new `publish` job.
`actionlint`: exit 0 across all four workflow files, locally and in CI.

**G-2** — `package-release.sh` correct and tamper-resistant. **done.**
Local run against a real macOS `dist/` tree produces the correctly-named
tarball (4 files inside), a verifying `SHA256SUMS`, and a reconciling
manifest. Corrupted-tarball tamper test: `sha256sum -c` reports
`FAILED`. Manifest/pins-mismatch tamper test: `package-release.sh`
refuses before writing any output, via `check-provenance.sh --platform`
reuse. `shellcheck`/`bash -n` (including stock 3.2) clean throughout.
Reconciled a second time against the real `v0.0.1-rc1` release's
downloaded assets. **One bug found and fixed during this row's own
testing:** the notes-template's patch-count arithmetic used `wc -l` on
comma-turned-newlines, which undercounts by one when the last field has
no trailing newline — 4 macOS patches were reported as 3. Fixed to
`awk -F',' '{print NF}'`; verified correct in the real release's
rendered notes (G-5).

**G-3** — happy-path test tag, full asset set. **done.** Tag
`v0.0.1-rc1` ran fetch → build → check-provenance → smoke (both legs) →
package → publish end-to-end, green. The release page carries exactly 4
assets (2 tarballs, `SHA256SUMS`, `release-manifest.txt`) plus rendered
notes as the body, `prerelease: true`. Independently downloaded,
checksummed, unpacked, and executed (`--help`, exit 0, all three macOS
binaries) on this machine. Kept published, pre-release-marked, as
durable evidence rather than deleted — the slice-doc's own disclosed
option.

**G-4** — refuse-to-publish, demonstrated. **done.** A deliberately
tampered `PARSER_SHA` (all-zero SHA), tagged `v0.0.0-rc-refusal-test`,
produced a run that failed at `Fetch upstream` on every leg (`git:
fatal: reference is not a tree`), skipped every downstream step
including both `cross-compat*` jobs, and never started the `publish`
job at all — `needs: call-build` correctly gated the entire matrix.
Absence independently confirmed: `gh release view` returns "release not
found"; `gh release list` shows only the legitimate `v0.0.1-rc1`.
Restoration commit `b4394f9` reverts `pins.env` to byte-identical with
its pre-rehearsal state, re-verified working. Test tag and the working
branch both deleted after evidence-gathering (the run itself remains
permanently viewable by URL). See the bubble-up below for what this
rehearsal surfaced beyond G-4 itself.

**G-5** — release notes render correctly. **done.** `v0.0.1-rc1`'s
release body: tag substituted throughout; both compat floors
(`glibc 2.35+ (x86_64)` / `macOS 15+ (arm64, Apple Silicon)`) exactly
as arc-plan D2/D3 state; a 6-row provenance table (2 platforms × 3
components) with correct SHAs and, post-G-2's-fix, correct patch
counts (Linux grounder: 2; macOS grounder: 4).

**G-6** — CDC-5 retired. **done.** `actions/checkout@v7`,
`actions/upload-artifact@v7`, `actions/download-artifact@v8` — each
version resolved from the action's real `/releases/latest` and each
confirmed `using: node24` in its `action.yml` before pinning, not
assumed. The post-refactor run (31068904658) carries zero "Node.js 20
is deprecated" annotations; the two present are unrelated Homebrew
tap-trust notices.

**G-7** — no CI behavior change from the refactor. **done.** Run
31068904658: the same 8 job names as arc01's close run (31065314655),
same per-job step content, now nested under `call-build /` instead of
listed flat. Job nesting changed; job identity, count, and step content
did not.

**G-8** — workflow/README docs current. **done.** README's new
"Releases" section states the trigger, the whole-matrix gating, the
direct-publish default with a pointer to the one-line draft toggle, and
the idempotency contract with a pointer to the authoritative
`release.yml` header, which documents the same three points in full.

## Bubble-up to the arc

**1. Did this slice deliver the piece of the arc's capability the
arc-plan assigned it?** Yes, against arc-plan §3's slice01 line ("The
machinery: build logic refactored for reuse, tag-triggered
`release.yml`, `package-release.sh`..., release-notes template..., the
refuse-to-publish demonstration..., CDC-5 action bumps") — every named
component delivered, plus the "happy-path test tag" cc-prompt assigned
explicitly. Arc ledger R-3's rehearsal half and R-5 both have direct
evidence available from this slice (a real published release, checksum-
and-manifest-reconciled); R-3's *real* half (a real `v0.1.0` tag) and
R-4's formal closure remain arc-scale composition checks for the arc
closing-report, not something this slice closes on its own.

**2. What did implementing this slice reveal that the arc-plan did not
anticipate — and what does slice02 inherit?**

- **A packaging-format finding, frozen for wolong at v0.1.0:** the
  asset set proven here — two `pandapi-<tag>-<platform>.tar.gz`
  tarballs, `SHA256SUMS`, `release-manifest.txt` — is real and correct,
  but it is **not yet the final v0.1.0 asset set**. Arc-plan D5 assigns
  slice02 a `THIRD-PARTY-LICENSES` file as a first-class release asset;
  `package-release.sh` does not produce one today. Slice02 either
  extends `package-release.sh` to emit it or adds a packaging step
  alongside it — either way, the asset-set shape wolong will consume
  frozen grows by one file between this slice's rehearsal and the real
  cut. This is disclosed here precisely so slice02 plans it rather than
  discovering it mid-cut.
- **The release-manifest.txt and notes.md formats are now real, not
  theoretical**, and should be treated as frozen inputs to slice02's
  README "Install from release" work — do not redesign them there
  without a tracked plan-change, per the "formats are frozen at v0.1.0"
  language in arc-plan §4.
- **The idempotency contract (fail loudly on re-run, never silently
  overwrite) is load-bearing for how slice02 should recover from a
  botched real cut:** if v0.1.0's tag push fails partway, the documented
  recovery is delete-the-release-and-tag-then-retag, not re-push the
  same tag expecting an update. Slice02 should plan its real cut with
  this in mind.
- **A real, unrelated robustness bug, found and fixed, not left for
  slice02:** `smoke-test.sh`'s engine invocation had no timeout, and a
  malformed `.sas` file (from any upstream cascade — not
  release-specific) could hang it indefinitely. Fixed with a portable
  background-watcher timeout (`run_engine`, 60s) and landed on `main`
  independently of this slice's release work, since the bug predates
  and is orthogonal to arc02. No slice02 action needed; recorded here
  as the disclosure trail.
- **No arc-plan change is needed.** None of the above alters the arc's
  capability statement, slice breakdown, or open questions (RQ1/RQ2
  remain as planned — RQ1's direct-publish default is now implemented
  and proven, not just decided).

**3. The silent-drop diff at slice scale.** Scope-as-delivered matches
scope-as-specified (slice-doc §2) exactly. In-scope items are all
present and verified (G-1 through G-8). Out-of-scope items are all
explicitly named in slice-doc §2's "Out" list and none were silently
started or dropped: the license/linkage audit and
`THIRD-PARTY-LICENSES` (slice02), README "Install from release" docs
(slice02), D4's project-plan candidate changes (slice02), the fallback
retire-into-releases re-evaluation (slice02), and signing/attestation
(recorded as a future-work idea only, not implemented). Nothing is
missing from either list.
