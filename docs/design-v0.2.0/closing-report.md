# chengdu 0.2.0 Closing Report

Closer: CDC
Branch: `release/0.2.x`
Closed on: 2026-08-09
Project plan: [`project-plan.md`](project-plan.md)
Gate: GO

## Project Verdict

Delivered. The `chengdu 0.2.0` project definition of done is met:

- pandaPI sources live in-tree under `pandaPI/` with full upstream history
  preserved and import identity verified.
- Build and release machinery consume in-tree sources, not upstream planner
  source fetches.
- Vetted upstream PRs/issues were absorbed with explicit credit and the
  remaining upstream work was dispositioned as defer/no-op with rationale.
- Licensing, NOTICE, release manifest, and provenance are re-anchored on the
  public chengdu fork identity.
- `v0.2.0` is public in wolong's frozen asset shape and the supported-platform
  consumer paths have been reproduced from public release tarballs.

Operator gate input: after pushing Slice04 CDC verification and Arc02 close,
the operator reported CI/CD green and requested final `0.2.0` closeout.

## Arc Walk

The project roadmap contains two arcs. Both are closed:

| Arc | Outcome | Evidence |
|-----|---------|----------|
| arc01 `vendor-identity` | delivered | [`arc01-vendor-identity/closing-report.md`](arc01-vendor-identity/closing-report.md) closes 4 slices and 6 arc-ledger rows. It establishes the in-tree fork baseline, no-fetch build path, patch dissolution, provenance, NOTICE, and licensing surface. |
| arc02 `upstream-absorption` | delivered | [`arc02-upstream-absorption/closing-report.md`](arc02-upstream-absorption/closing-report.md) closes 4 slices and 7 arc-ledger rows. It absorbs engine PR #14 / issue #12 and parser PR #21 with credit, publishes `v0.2.0`, verifies public assets/consumer paths, and files upstream courtesy issues. |

Arc count matches the project roadmap: 2 planned, 2 closed, 0 missing.

## Project-Level Evidence

The final pushed pre-project-close state was refreshed before this report:

| Claim | Evidence |
|-------|----------|
| Branch sync | `git status --short --branch` reported `release/0.2.x...origin/release/0.2.x` clean after `git fetch origin release/0.2.x --tags`. |
| Latest branch CI | GitHub Actions run `31296322736` completed `success` for `da968387480f7b9166aea26704740c33c12c9c49`, the Arc02 close commit. |
| CI job matrix | Run `31296322736` has 8 successful jobs: actionlint, README-verbatim on Ubuntu 22.04, builds on macOS 15 / Ubuntu 22.04 / Ubuntu 24.04, README-verbatim on macOS 15, and both cross-compat jobs. |
| Release tag | GitHub tag ref `refs/tags/v0.2.0` is object type `commit`, SHA `d38e30d396d435d5a80992106f59b403c20909e9`. |
| Public release | GitHub release `v0.2.0` is public, non-draft, non-prerelease, published `2026-08-09T04:39:12Z`: https://github.com/billosys/chengdu/releases/tag/v0.2.0 |
| Public assets | Release assets are exactly `pandapi-v0.2.0-linux-x86_64.tar.gz`, `pandapi-v0.2.0-macos-arm64.tar.gz`, `release-manifest.txt`, `SHA256SUMS`, and `THIRD-PARTY-LICENSES`. |

The current `gh run view --log` and per-job log endpoints returned empty bodies
for run `31296322736`, so this report does not claim a fresh final-head log
grep from those endpoints. The no-fetch claim rests on:

- Arc01's reproduced workflow log scan at run `31239214748`, which found no
  active clone/fetch of `panda-planner-dev`, `gitlab.com/danfis`,
  `galvusdamor/h2-fd-preprocessor`, or `fetch-upstream`.
- Current active path inspection: `scripts/` and `.github/` contain no
  `fetch-upstream`, planner-source `git clone`, `panda-planner-dev`,
  `gitlab.com/danfis`, or `galvusdamor/h2-fd-preprocessor` references. The
  only active network hit found is the actionlint installer in
  `.github/workflows/build-reusable.yml`.
- Latest branch run `31296322736` is green across the full matrix after the
  Arc02 close docs.

## Project Ledger Walk

Rows verified: 5. Done: 5. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| P1 | Done, reproduced. CDC reran import identity checks for parser, grounder, engine, cpddl, h2-fd-preprocessor, boruvka, and opts. For all seven, the upstream SHA is an ancestor of `HEAD`, the import commit's second parent is the upstream SHA, and normalized `git ls-tree` comparison between upstream root and the import prefix is empty. `vendor.env` records all prefixes, upstream URLs, upstream SHAs, and import commits. |
| P2 | Done, reconciled. Arc01 established and log-verified the no-fetch build baseline; active scripts/workflows still contain no planner-source fetch path; latest pushed branch CI run `31296322736` is green across build, README-verbatim, actionlint, and cross-compat jobs. |
| P3 | Done, reproduced. Arc02 absorbed exactly two upstream changes. Parser PR #21 is native author-preserving at `49c02f7f` with `Upstream-PR`. Engine PR #14 / issue #12 is an accepted credited manual-port exception at `4da38819` with `Upstream-author`, `Upstream-PR`, and `Upstream-Issue`. Both implementation commits carry the required Codex and Billo AI co-author trailers. |
| P4 | Done, reproduced. `docs/license-audit-v0.2.0.md` is present; nine `NOTICE` files exist under `pandaPI/`; release packaging generated `THIRD-PARTY-LICENSES` and Slice04 CDC verified it as a public release asset. The audit lists linked license obligations and states source availability through the public chengdu release commit. |
| P5 | Done, reproduced. `v0.2.0` is live and public, published after a green pre-tag branch run and green tag release workflow. Slice04 CDC verified public assets by checksum, tarball contents, manifest/provenance reconciliation, and public macOS arm64 plus Linux x86_64 consumer walks ending `Plan verification result: true`. |

## Composition Check

The arcs compose into the project DoD:

1. Arc01 changed the source ownership model first, importing the planner
   sources and former submodules into chengdu history without source edits,
   dissolving the build-time patch bridge, and proving no-fetch builds.
2. Arc02 then changed only the selected source behavior/build-system surfaces:
   engine exit status, parser makefile install dependency, README release URLs,
   and release/courtesy documentation. That preserves the project invariant
   that "move the code" and "change the code" never share a commit.
3. Release publication ties the resulting fork to public artifacts whose
   provenance names chengdu commit, source prefixes, upstream import SHAs,
   import commits, and `patches=none`.

No project-level silent drop remains. Deferred work is explicitly out of the
0.2.0 boundary: systematic CLI/API/stdio redesign, parser normalization and
resource control, SAT verifier absorption, grounder status policy, Docker,
Windows/new-platform support, package-manager distribution, and `linux-arm64`.

## Bubble-up / Next Project

No remediation arc is required for `0.2.0`. The next planning surface is
`docs/design-v0.3.0/notes.md` and any future 0.3.0 project plan that chooses
which deferred items to promote.

The main carry-forward facts:

- chengdu is now a deliberate public in-tree fork, not a fetch-and-patch
  harness.
- Native author preservation is preferred for clean upstream PRs, but dirty PRs
  may close as credited manual ports with explicit upstream author/PR/issue
  trailers.
- Public release verification should continue to execute actual downloaded
  tarballs on both supported platforms, because that is the wolong contract.

## What Worked

- The two-arc split kept identity movement separate from source behavior
  changes.
- The ledgered slice rhythm made deferred upstream work visible instead of
  silently dropping it.
- Public release checks caught the real consumer surface: asset names,
  checksums, tarball contents, provenance, and the parse-ground-solve-verify
  chain.
