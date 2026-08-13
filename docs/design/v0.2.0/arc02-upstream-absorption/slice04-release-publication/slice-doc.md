# chengdu arc02 / slice04 - release-publication - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).
>
> This is Arc02's final planned slice. Its close should hand CDC enough
> evidence to verify the real `v0.2.0` release and assemble the arc close.

## 1. Goal

Publish `v0.2.0` from the in-tree pandaPI fork, in wolong's frozen release
asset shape, after the selected Arc02 upstream absorptions have landed and
been CDC-verified.

At slice close, `v0.2.0` exists as a public GitHub Release in
`billosys/chengdu`; the tag points at the intended final release commit; the
full release workflow is green; the asset set is exact; downloaded assets
checksum and unpack correctly; a real consumer walk reaches
`Plan verification result: true`; and the upstream courtesy announcement
issues are filed and recorded.

## 2. Release preflight snapshot at slice open

CDC refreshed local and GitHub state on 2026-08-08 while opening this slice:

| Item | State |
|------|-------|
| Branch | `release/0.2.x`, clean, ahead of `origin/release/0.2.x` by 4 commits |
| Local tags | `v0.0.1-rc1`, `v0.0.2-rc2`, `v0.1.0`; no local `v0.2.0` |
| GitHub release `v0.2.0` | not found (`gh api repos/billosys/chengdu/releases/tags/v0.2.0` returned 404) |
| GitHub tag ref `v0.2.0` | not found (`gh api repos/billosys/chengdu/git/ref/tags/v0.2.0` returned 404) |
| GitHub workflows | `build-reusable`, `build`, and `release` are active |
| Remote | `origin git@github.com:billosys/chengdu.git` |

Implication: CC must push `release/0.2.x` before tagging. The release tag must
be created only after the final release commit is on the public branch and the
pre-tag full matrix is green.

## 3. Scope

**In:**

- Refresh release/tag/workflow state at implementation start. If a `v0.2.0`
  tag or release already exists, stop before modifying it and ask the operator.
- Update release-facing docs that still point at `v0.1.0` when they should use
  the new stable release, especially README install URLs and tarball names.
  Preserve the four-command shape exactly.
- Rebuild from clean ignored generated output where practical, so provenance
  records the final release commit and stale first blocks cannot fool or break
  `check-provenance.sh`.
- Run the full local no-fetch gate and a local `v0.2.0` package dry-run before
  publication.
- Push the final release commit to `origin/release/0.2.x`, wait for the full
  branch build matrix to pass, then create and push tag `v0.2.0`.
- Wait for the tag-triggered release workflow to complete successfully and
  publish the GitHub Release.
- Verify the real release assets by downloading them, checking `SHA256SUMS`,
  checking tarball contents, checking `release-manifest.txt`, and confirming
  release notes/provenance point at the final commit.
- Walk the public macOS arm64 consumer install path from a clean scratch
  directory. Linux consumer verification is CDC's independent reproduction at
  slice verification/arc close, unless CC has a clean Linux environment and can
  record it too.
- File one upstream courtesy announcement issue each on:
  `panda-planner-dev/pandaPIengine`, `panda-planner-dev/pandaPIparser`, and
  `panda-planner-dev/pandaPIgrounder`. Each issue should name the chengdu
  release URL, fork rationale, import SHA(s), and any absorbed upstream PR/issue
  credit relevant to that repo.
- Close this slice with a per-row closing report and a Bubble-up that states
  whether Arc02 is ready for CDC verification and arc close.

**Out:**

- No planner source changes unless a release gate exposes a correctness issue.
  If that happens, stop and treat it as a release-blocking defect, not as
  opportunistic scope growth.
- No release asset-shape changes: names, tarball contents, `SHA256SUMS`,
  `release-manifest.txt`, `THIRD-PARTY-LICENSES`, and the README command shape
  stay compatible with wolong's frozen fetch spec.
- No 0.3.0 CLI/API redesign, parser resource/normalization fixes, SAT verifier
  absorption, Docker work, new platform support, signing/attestation, package
  manager distribution, or wolong-side code.
- No draft-release model change unless the operator explicitly decides to
  change `scripts/publish-release.sh`.
- No `cdc-verification.md`, arc closing report, or project closing report; CDC
  writes those after independent verification.

## 4. Constraints

- The final tag is a one-way door. `release.yml` intentionally fails if a
  release already exists for a tag. A botched cut requires deleting the release
  and tag before retagging; do not do that without explicit operator approval.
- The tag must point at the final release commit, including any README/release
  doc updates and this slice's implementation-side close docs if CC chooses to
  include them before tagging.
- The branch must be pushed before the tag, and the tag must be pushed only
  after the pre-tag branch matrix is green.
- Provenance is commit-sensitive and append-based. Start the final local gate
  from clean ignored generated output, or at minimum remove stale
  `dist/<platform>/provenance.txt` before rebuilding all three components.
- Public upstream announcements follow the contribution style: friendly,
  specific, calibrated, low-pressure, and respectful of upstream ownership.
- Assistant-authored commits use:
  `Co-authored-by: Codex <noreply@openai.com>` and
  `Co-authored-by: Billo AI <ai-engineering@billo.systems>`.

## 5. Verification approach

CC verifies the release by chaining local gates, public GitHub Actions evidence,
public release asset inspection, and a real consumer download walk. CDC verifies
by re-running practical local checks at the released commit, independently
inspecting the GitHub release/run/tag/assets, and performing the Linux consumer
download path during Slice04 verification or the Arc02 close.

This slice intentionally uses both pre-tag and tag-triggered CI:

- Pre-tag branch CI catches release-candidate breakage before the one-way tag.
- The release workflow is still the publishing gate, because it rebuilds the
  same matrix from the tag and only publishes after all legs pass.

## 6. Exit criteria

All ledger rows reach final status. `v0.2.0` is published, not a prerelease,
from the intended final commit. The release workflow is green. The exact frozen
asset set is downloadable, checksummed, and internally coherent. The macOS
consumer path passes from the public release. Upstream courtesy announcements
are filed and recorded. The closing report walks every row and bubbles up that
Arc02 is ready for CDC verification and arc close.
