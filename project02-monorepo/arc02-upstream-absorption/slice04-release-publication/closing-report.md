# Slice 04 Closing Report - release-publication

Closer: CC
Verifier: CDC
Branch: `release/0.2.x`
Release tag: `v0.2.0`

## Result

Slice04 delivered the public `v0.2.0` release as assigned. The release tag is a
lightweight tag at commit `d38e30d396d435d5a80992106f59b403c20909e9`, the
tag-triggered workflow completed successfully, and the GitHub Release is public,
non-draft, and non-prerelease:

- Release URL: https://github.com/billosys/chengdu/releases/tag/v0.2.0
- Release commit: `d38e30d396d435d5a80992106f59b403c20909e9`
- Local tag target: `git rev-parse v0.2.0^{commit}` returned
  `d38e30d396d435d5a80992106f59b403c20909e9`
- Remote tag ref: `refs/tags/v0.2.0` object type `commit`, SHA
  `d38e30d396d435d5a80992106f59b403c20909e9`
- Pre-tag branch build: https://github.com/billosys/chengdu/actions/runs/31294772117
- Release workflow: https://github.com/billosys/chengdu/actions/runs/31294905237

The only pre-tag code/documentation change made by this slice was README
release-facing install examples, committed as `d38e30d3 docs: point install
examples at v0.2.0`. This close report and ledger update are post-publication
docs and are not part of the `v0.2.0` tag.

## Publication Evidence

Preflight was refreshed before the cut:

- `origin` was `git@github.com:billosys/chengdu.git`.
- Local `git tag --list 'v0.2.0'` returned no tag.
- GitHub release and remote tag APIs returned 404 for `v0.2.0` before tagging.
- GitHub workflows `build-reusable`, `build`, and `release` were active.

Local release gate at `d38e30d396d435d5a80992106f59b403c20909e9` passed after
removing stale generated provenance:

- `./scripts/build-parser.sh`
- `./scripts/build-grounder.sh`
- `./scripts/build-engine.sh`
- `./scripts/check-provenance.sh`
- `./scripts/smoke-test.sh`
- `./scripts/smoke-test.sh --negative`
- `./scripts/package-release.sh v0.2.0`
- `bash -n scripts/*.sh`
- `/bin/bash -n scripts/*.sh`
- `shellcheck scripts/*.sh`
- `actionlint .github/workflows/*.yml`
- `git diff --check`
- `find pandaPI -name .git -print`
- `git ls-files -s pandaPI | awk '$1 == "160000"'`

The `find` and gitlink scans produced no output.

The release asset set is exact:

- `pandapi-v0.2.0-linux-x86_64.tar.gz`
- `pandapi-v0.2.0-macos-arm64.tar.gz`
- `SHA256SUMS`
- `release-manifest.txt`
- `THIRD-PARTY-LICENSES`

Public asset verification was run in `/tmp/chengdu-v020-assets.wIuLMu`:

- `gh release download v0.2.0 --repo billosys/chengdu --dir "$tmp"` downloaded
  exactly the five expected assets.
- `shasum -a 256 -c SHA256SUMS --ignore-missing` returned OK for both tarballs,
  `release-manifest.txt`, and `THIRD-PARTY-LICENSES`.
- Both tarballs list exactly the three binaries, `provenance.txt`, and
  `fixtures/domain.hddl` / `fixtures/problem.hddl`.
- `release-manifest.txt` and both tarball `provenance.txt` files name release
  commit `d38e30d396d435d5a80992106f59b403c20909e9`, expected
  `source_prefix`, expected `upstream_sha`, expected `import_commit`, and
  `patches=none`.

Recorded upstream identities:

| Component | source_prefix | upstream_sha | import_commit |
|-----------|---------------|--------------|---------------|
| pandaPIparser | `pandaPI/pandaPIparser` | `88c0995c72c5ce2685e9546ec153a3545977ed81` | `ac8e385d0497a166f757f001d814f949c9f0504b` |
| pandaPIgrounder | `pandaPI/pandaPIgrounder` | `b04ff7bcca710ae80a1ce73b8c027f0224565afc` | `2b0a6093cd229f699865d7d2bdcdd77f5673ee67` |
| pandaPIengine | `pandaPI/pandaPIengine` | `810f04388667db5e3e4f114e960a4efbb43b1ac0` | `f734b692aaf7aeefe6805fb7983c693fbed0cf47` |

## Consumer Walk

The macOS arm64 consumer path was run from a clean scratch directory,
`/tmp/chengdu-v020-consumer.a9bs8j`, using public release downloads:

```bash
curl -fLO https://github.com/billosys/chengdu/releases/download/v0.2.0/pandapi-v0.2.0-macos-arm64.tar.gz
curl -fLO https://github.com/billosys/chengdu/releases/download/v0.2.0/SHA256SUMS
shasum -a 256 -c SHA256SUMS --ignore-missing
tar xzf pandapi-v0.2.0-macos-arm64.tar.gz
./pandaPIparser fixtures/domain.hddl fixtures/problem.hddl plan.htn
./pandaPIgrounder plan.htn plan.sas
./pandaPIengine plan.sas > plan.raw
./pandaPIparser -c plan.raw plan.txt
./pandaPIparser --verify fixtures/domain.hddl fixtures/problem.hddl plan.txt
```

The walk exited 0 and ended with `Plan verification result: true`. CDC still
owns the independent Linux consumer download walk during Slice04 verification
or Arc02 close.

## Upstream Courtesy Issues

Filed after the release URL existed:

- pandaPIengine: https://github.com/panda-planner-dev/pandaPIengine/issues/18
- pandaPIparser: https://github.com/panda-planner-dev/pandaPIparser/issues/34
- pandaPIgrounder: https://github.com/panda-planner-dev/pandaPIgrounder/issues/15

Each issue names the chengdu repo, the `v0.2.0` release URL, the relevant
upstream import SHA, the dormant-upstream plus wolong stable-binary-release
rationale, and no-pressure FYI wording. Engine names absorbed PR #14 / issue
#12 with Robert P. Goldman credited; parser names absorbed PR #21 with Robert
P. Goldman credited; grounder states no grounder upstream PR was absorbed and
Docker/workflow/status-policy work stayed deferred.

## Ledger Walk

- F-1: Done. Started from `release/0.2.x`; Slice03 closing report and CDC
  verification were present; Slice03 CDC accepted close HEAD
  `8810395c6c77e28c85895d6289a78821f7fad585`.
- F-2: Done. Preflight found target repo `billosys/chengdu`, active workflows,
  no existing local or remote `v0.2.0`, and recorded final release commit
  `d38e30d396d435d5a80992106f59b403c20909e9`.
- F-3: Done. README install examples now point at `v0.2.0` while preserving
  wolong's frozen command shape and asset names.
- F-4: Done. Pre-tag diff from slice open to release commit was `README.md`
  only; no planner source or release machinery changed.
- F-5: Done. Full local parser, grounder, engine, provenance, smoke, negative
  smoke, and package dry-run gate passed from clean generated provenance.
- F-6: Done. Shell syntax, shellcheck, actionlint, whitespace, nested `.git`,
  and gitlink scans passed.
- F-7: Done. `release/0.2.x` was pushed and branch build run `31294772117`
  passed for the exact release commit before tagging.
- F-8: Done. `v0.2.0` was created and pushed only after F-1 through F-7; local
  and remote tag checks point at the release commit.
- F-9: Done. Release workflow run `31294905237` passed and published a public
  non-draft, non-prerelease release.
- F-10: Done. The public asset list contains exactly the two tarballs,
  `SHA256SUMS`, `release-manifest.txt`, and `THIRD-PARTY-LICENSES`.
- F-11: Done. Downloaded public assets pass checksum validation, tarball content
  inspection, and manifest/provenance reconciliation.
- F-12: Done. Public macOS arm64 consumer walk passed with
  `Plan verification result: true`.
- F-13: Done. Upstream courtesy issues were filed for engine, parser, and
  grounder and their URLs are recorded above.
- F-14: Done before closeout edits. Worktree was synced with origin, no temp
  worktrees/remotes existed, scoped status was clean, and generated-file scan
  under `pandaPI/` produced no output. Recheck after this closeout commit is
  expected.
- F-15: Done. This report walks all 15 rows and includes release evidence,
  courtesy issue URLs, CDC Linux handoff, Bubble-up, and silent-drop diff.

## Bubble-up

Arc02's release-publication piece is delivered as assigned. The release exists,
the tag points at the intended commit, CI passed before and after the tag, the
public assets verify, the macOS consumer path passes, and upstream courtesy
announcements are filed.

Arc02 is ready for CDC verification and arc close, with one explicit handoff:
CDC should independently verify the Linux public consumer path and should write
`cdc-verification.md`, the Arc02 closing report, and any project close/status
synthesis. CC did not write those files.

Project/arc docs changed only at the slice level in this closeout. No Arc02
arc-plan, project-plan, source, workflow, packaging, or publish-script changes
were made after the release tag.

Silent-drop diff for the release commit: from slice-open commit `4a609c36` to
release commit `d38e30d3`, `git diff --name-only` showed only `README.md`.
For the post-publication closeout commit, the expected diff is this slice's
`ledger.md` and `closing-report.md`.
