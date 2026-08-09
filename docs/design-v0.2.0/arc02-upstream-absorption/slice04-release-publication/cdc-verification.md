# CDC verification - Slice 04: release-publication

Verified by CDC on 2026-08-09 for `release/0.2.x`.

## Verdict

Accepted. CC's Slice04 closure is reproduced at close HEAD
`d0d8ef63`, with public release tag `v0.2.0` pointing at
`d38e30d396d435d5a80992106f59b403c20909e9`.

Scope checked: slice-open commit `4a609c36`, release commit `d38e30d3`,
and docs-close commit `d0d8ef63`. The release commit changed only
`README.md` and preserved the required assistant co-author trailers. The
post-publication docs close changed only this slice's ledger and closing
report before CDC added this file.

## Live GitHub Evidence

CDC refreshed the public GitHub state rather than relying on CC's transcript:

| Claim | CDC result |
|-------|------------|
| Release | `v0.2.0`, public, non-draft, non-prerelease, published `2026-08-09T04:39:12Z` |
| Release URL | https://github.com/billosys/chengdu/releases/tag/v0.2.0 |
| Remote tag ref | `refs/tags/v0.2.0`, object type `commit`, SHA `d38e30d396d435d5a80992106f59b403c20909e9` |
| Pre-tag branch run | `31294772117`, workflow `build`, event `push`, head SHA `d38e30d396d435d5a80992106f59b403c20909e9`, conclusion `success` |
| Tag release run | `31294905237`, workflow `release`, event `push`, head SHA `d38e30d396d435d5a80992106f59b403c20909e9`, conclusion `success` |
| Assets | exactly `pandapi-v0.2.0-linux-x86_64.tar.gz`, `pandapi-v0.2.0-macos-arm64.tar.gz`, `release-manifest.txt`, `SHA256SUMS`, `THIRD-PARTY-LICENSES` |

GitHub's release API reports `target_commitish=main`, but the release
authority for this slice is the pushed tag ref, and that ref points directly
at the intended release commit.

## Public Asset Verification

CDC downloaded the public assets into
`/private/tmp/chengdu-v020-assets-cdc.gFHYid` with `gh release download
v0.2.0 --repo billosys/chengdu --dir "$asset_tmp"`.

| Check | CDC result |
|-------|------------|
| Asset set | reproduced exactly; no missing or extra assets |
| `shasum -a 256 -c SHA256SUMS --ignore-missing` | pass for both tarballs, `release-manifest.txt`, and `THIRD-PARTY-LICENSES` |
| Linux tarball contents | exactly `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, `provenance.txt`, `fixtures/domain.hddl`, `fixtures/problem.hddl` |
| macOS tarball contents | exactly `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, `provenance.txt`, `fixtures/domain.hddl`, `fixtures/problem.hddl` |
| File identities | Linux assets are x86-64 ELF binaries; macOS assets are arm64 Mach-O binaries |
| Manifest/provenance anchors | both platforms name release commit `d38e30d396d435d5a80992106f59b403c20909e9`, expected `source_prefix`, expected `upstream_sha`, expected `import_commit`, and `patches=none` |

Recorded upstream identities match the release manifest and per-tarball
provenance:

| Component | source_prefix | upstream_sha | import_commit |
|-----------|---------------|--------------|---------------|
| pandaPIparser | `pandaPI/pandaPIparser` | `88c0995c72c5ce2685e9546ec153a3545977ed81` | `ac8e385d0497a166f757f001d814f949c9f0504b` |
| pandaPIgrounder | `pandaPI/pandaPIgrounder` | `b04ff7bcca710ae80a1ce73b8c027f0224565afc` | `2b0a6093cd229f699865d7d2bdcdd77f5673ee67` |
| pandaPIengine | `pandaPI/pandaPIengine` | `810f04388667db5e3e4f114e960a4efbb43b1ac0` | `f734b692aaf7aeefe6805fb7983c693fbed0cf47` |

## Consumer Walks

CDC reproduced the public macOS arm64 consumer path in
`/private/tmp/chengdu-v020-consumer-cdc.6iJB1c` using direct release downloads
with `curl -fLO`:

| Step | Result |
|------|--------|
| Download macOS tarball and `SHA256SUMS` | pass |
| `shasum -a 256 -c SHA256SUMS --ignore-missing` | pass for `pandapi-v0.2.0-macos-arm64.tar.gz` |
| Parse, ground, solve, convert, verify | pass; final command exited 0 and printed `Plan verification result: true` |

CDC also discharged the Linux handoff in
`/private/tmp/chengdu-v020-linux-consumer-cdc.4NUMvj` by running the public
`linux-x86_64` tarball inside Docker with `--platform linux/amd64` on
`ubuntu:22.04`:

| Step | Result |
|------|--------|
| Filtered Linux checksum with `sha256sum -c` | pass for `pandapi-v0.2.0-linux-x86_64.tar.gz` |
| Parse, ground, solve, convert, verify | pass; final command exited 0 and printed `Plan verification result: true` |

Two rejected Linux host images were useful negative environment evidence: a
BusyBox/musl image lacked the expected glibc loader for `pandaPIengine`, and an
older Debian-based image had glibc/libstdc++ too old for the released engine.
The Ubuntu 22.04 run is therefore the accepted Linux consumer execution
evidence.

## Reproduced Release-Commit Gate

CDC created a detached temp worktree at the exact release commit and removed it
after verification:

```text
git worktree add --detach /private/tmp/chengdu-v020-release-wt.KKHxCw d38e30d396d435d5a80992106f59b403c20909e9
git worktree remove --force /private/tmp/chengdu-v020-release-wt.KKHxCw
```

The release-commit gate passed:

| Gate | CDC result |
|------|------------|
| `./scripts/build-parser.sh` | pass; produced `dist/macos-arm64/pandaPIparser` |
| `./scripts/build-grounder.sh` | pass; produced `dist/macos-arm64/pandaPIgrounder` |
| `./scripts/build-engine.sh` | pass; produced `dist/macos-arm64/pandaPIengine` |
| `./scripts/check-provenance.sh` | pass; all 3 components verified against `vendor.env` and Git state |
| `./scripts/smoke-test.sh` | pass; 5 passed, 0 failed |
| `./scripts/smoke-test.sh --negative` | pass; 4 passed, 0 failed, including unsolvable engine exit 2 |
| `./scripts/package-release.sh v0.2.0` | pass; macOS release dry-run built tarball, licenses, manifest, checksums, and notes |
| `bash -n scripts/*.sh` | pass |
| `/bin/bash -n scripts/*.sh` | pass |
| `shellcheck scripts/*.sh` | pass |
| `actionlint .github/workflows/*.yml` | pass |
| `git diff --check` | pass |
| `find pandaPI -name .git -print` | pass; no output |
| `git ls-files -s pandaPI \| awk '$1 == "160000"'` | pass; no output |

Existing compiler warnings remain warning-only and are not new Slice04 signal.

## Upstream Courtesy Issues

CDC verified the three upstream courtesy issues through the GitHub API:

| Repo | Issue | CDC result |
|------|-------|------------|
| `pandaPIengine` | https://github.com/panda-planner-dev/pandaPIengine/issues/18 | open; names chengdu repo/release, engine import SHA, PR #14 / issue #12 absorption, and Robert P. Goldman credit |
| `pandaPIparser` | https://github.com/panda-planner-dev/pandaPIparser/issues/34 | open; names chengdu repo/release, parser import SHA, PR #21 absorption, and Robert P. Goldman credit |
| `pandaPIgrounder` | https://github.com/panda-planner-dev/pandaPIgrounder/issues/15 | open; names chengdu repo/release, grounder import SHA, and states no grounder PR was absorbed for v0.2.0 |

## Ledger Verification

Rows verified: 15. Done: 15. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| F-1 | Reproduced. Branch is `release/0.2.x`; Slice03 close and CDC verification exist; worktree was clean before CDC wrote this file. |
| F-2 | Reproduced. Release/tag/workflow state was live-checked; tag did not preexist at slice open per slice docs, and the final release commit is recorded as `d38e30d396d435d5a80992106f59b403c20909e9`. |
| F-3 | Reproduced. `README.md` install examples point at `v0.2.0`; the frozen download/checksum/extract/export shape is preserved. |
| F-4 | Reproduced. The release commit diff from `4a609c36` to `d38e30d3` is `README.md` only; no planner source, workflow, package, or publish script changes landed in the release cut. |
| F-5 | Reproduced. Detached release-commit local gate passed parser, grounder, engine, provenance, positive smoke, negative smoke, and `v0.2.0` package dry-run. |
| F-6 | Reproduced. Bash syntax, shellcheck, actionlint, whitespace, nested `.git`, and gitlink scans passed. |
| F-7 | Reproduced. Pre-tag branch run `31294772117` passed for release commit `d38e30d396d435d5a80992106f59b403c20909e9`. |
| F-8 | Reproduced. Remote tag `v0.2.0` is a commit ref pointing at the intended release commit. |
| F-9 | Reproduced. Tag-triggered release run `31294905237` passed and published a public, non-draft, non-prerelease release. |
| F-10 | Reproduced. Public asset list is exactly the two tarballs plus `SHA256SUMS`, `release-manifest.txt`, and `THIRD-PARTY-LICENSES`. |
| F-11 | Reproduced. Downloaded assets pass checksum validation, tarball content inspection, file identity checks, and manifest/provenance reconciliation. |
| F-12 | Reproduced beyond CC's macOS evidence. CDC reran the public macOS arm64 consumer path and independently ran the public Linux x86_64 consumer path under Ubuntu 22.04; both ended with `Plan verification result: true`. |
| F-13 | Reproduced. Courtesy issues exist for engine, parser, and grounder with release URL, import identity, fork rationale, and absorbed-change credit where applicable. |
| F-14 | Reproduced. Current remote is `git@github.com:billosys/chengdu.git`; only the main worktree remains; no nested `.git` or gitlinks exist under `pandaPI/`; worktree was clean before CDC wrote this file. |
| F-15 | Reproduced. Closing report exists, walks all 15 rows, includes Bubble-up, and explicitly leaves CDC verification, arc close, and project close to CDC. |

## Bubble-up Check

Delivered-as-assigned: yes. Slice04 publishes `v0.2.0` in wolong's frozen
asset shape, tied to a green pre-tag branch build and a green tag release
workflow, with public assets that checksum, unpack, reconcile to provenance,
and execute on both supported platforms.

Silent-drop diff: complete. The release cut changed only the README examples;
post-publication closeout changed only slice close docs. No planner source,
workflow, release-shape, package-script, or out-of-scope 0.3.0 work was
smuggled into this slice.

Arc/project plan impact: Arc02 is ready for arc close. The Linux consumer
handoff named by CC is now discharged by CDC. Project close/status synthesis is
still separate CDC-owned work after Arc02's composition ledger is walked.

## What Worked

- The one-way-door sequencing held: preflight, local gates, branch CI, tag, tag
  workflow, public release, public asset inspection, then courtesy issues.
- Public download checks caught the actual consumer contract on both supported
  platforms instead of trusting local build outputs.
- The release/docs split is clean: the public tag points at the release commit,
  while slice close artifacts remain post-publication documentation.
