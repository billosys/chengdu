# CC assignment - chengdu arc02 / slice04 - release-publication

You are CC, the implementing context for the final planned Arc02 slice,
working in the `chengdu` repo (`~/lab/billosys/chengdu`) on
`release/0.2.x`. This file is your assignment; read it fully before writing or
publishing anything.

## Read order (before any work)

1. `/AGENTS.md` - standing project contracts, release policy, commit footer
   convention, direct branch workflow, and wolong fetch-spec constraint.
2. `docs/design-v0.2.0/project-plan.md` - 0.2.0 definition of done, fork
   policy, release asset boundary, and project ledger P2/P3/P5.
3. `docs/design-v0.2.0/arc02-upstream-absorption/arc-plan.md` - Arc02
   capability, final absorption queue, release-publication scope, and arc
   ledger A4-A7.
4. `docs/design-v0.2.0/arc02-upstream-absorption/slice03-parser-makefile-pr/closing-report.md`
   and `docs/design-v0.2.0/arc02-upstream-absorption/slice03-parser-makefile-pr/cdc-verification.md`
   - the verified prior source absorption and provenance caveat.
5. `docs/design-v0.2.0/arc02-upstream-absorption/slice04-release-publication/slice-doc.md`
   - goal, scope, constraints, and release preflight snapshot.
6. `docs/design-v0.2.0/arc02-upstream-absorption/slice04-release-publication/ledger.md`
   - the specification of done. Fifteen rows; every row reaches final status
   before this slice advances.
7. `README.md`, `.github/workflows/build.yml`,
   `.github/workflows/release.yml`, `.github/workflows/build-reusable.yml`,
   `scripts/package-release.sh`, `scripts/publish-release.sh`, and
   `.github/release-notes.md.in`.
8. For public upstream announcements:
   `/Users/oubiwann/.agents/skills/collaboration-framework/docs/CONTRIBUTION-STYLE.md`
   and
   `/Users/oubiwann/.agents/skills/collaboration-framework/templates/CONTRIBUTION-TICKET.md`.

## The work, in dependency order

### 1. Refresh preflight and prepare the final release commit

- Confirm branch, remote, worktree state, and current commit.
- Confirm there is no existing local or remote `v0.2.0` tag and no existing
  GitHub Release for `v0.2.0`. If either exists, stop before changing it and
  ask the operator.
- Confirm GitHub workflows `build-reusable`, `build`, and `release` are active.
- Update release-facing docs that should now point at `v0.2.0`, especially the
  README install examples. Preserve the exact frozen command shape and asset
  names.
- Keep source and release machinery untouched unless a real gate failure proves
  a release-blocking defect. If that happens, stop and report the defect before
  widening scope.
- Commit any prep/docs changes with the required trailers.

### 2. Run the local release gate

Use clean ignored generated output where practical. The Slice03 CDC check
proved that stale `dist/<platform>/provenance.txt` can make
`check-provenance.sh` read the wrong first block, so do not reuse stale
provenance for the final gate.

Run and record:

```bash
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/check-provenance.sh
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
./scripts/package-release.sh v0.2.0
bash -n scripts/*.sh
/bin/bash -n scripts/*.sh
shellcheck scripts/*.sh
actionlint .github/workflows/*.yml
git diff --check
find pandaPI -name .git -print
git ls-files -s pandaPI | awk '$1 == "160000"'
```

Expected `find`/gitlink scan output is empty.

### 3. Push branch, wait for the pre-tag matrix, then tag

The local branch is ahead of origin at slice open. Push the final release
commit before creating the tag:

```bash
git push origin release/0.2.x
```

Find and watch the `build.yml` run for the exact final commit SHA. Do not tag
until that run is green across the full matrix.

Only after ledger rows F-1 through F-7 are done:

```bash
git tag v0.2.0 <final-release-sha>
git push origin v0.2.0
```

If you use an annotated tag instead, record that explicitly and still verify
`v0.2.0^{commit}` equals the final release commit.

### 4. Wait for release workflow and verify the public release

Find and watch the `release.yml` run for the tag. It must complete green and
publish a non-draft, non-prerelease GitHub Release.

Then verify from public release assets in a clean temp directory:

```bash
tmp="$(mktemp -d)"
gh release download v0.2.0 --repo billosys/chengdu --dir "$tmp"
(
  cd "$tmp"
  sha256sum -c SHA256SUMS --ignore-missing || shasum -a 256 -c SHA256SUMS --ignore-missing
  tar tzf pandapi-v0.2.0-linux-x86_64.tar.gz
  tar tzf pandapi-v0.2.0-macos-arm64.tar.gz
)
```

Inspect the asset list exactly:

- `pandapi-v0.2.0-linux-x86_64.tar.gz`
- `pandapi-v0.2.0-macos-arm64.tar.gz`
- `SHA256SUMS`
- `release-manifest.txt`
- `THIRD-PARTY-LICENSES`

Inspect `release-manifest.txt` and each tarball's `provenance.txt` for the
final release commit, expected `source_prefix`/`upstream_sha`/`import_commit`
values from `vendor.env`, and `patches=none`.

### 5. Walk the consumer path

From a clean macOS scratch directory, execute the README install path with
`v0.2.0` and record the transcript. It must reach:

```text
Plan verification result: true
```

and exit 0. CDC will reproduce the Linux path independently during
`cdc-verification.md` or Arc02 close unless you also have a clean Linux
environment and can record it now.

### 6. File upstream courtesy announcements

After the release URL exists, file one friendly, specific, low-pressure
announcement issue per upstream repo:

- `panda-planner-dev/pandaPIengine`: announce the chengdu `v0.2.0` fork/release
  and that it absorbed PR #14 / issue #12 with Robert P. Goldman credited.
- `panda-planner-dev/pandaPIparser`: announce the chengdu `v0.2.0` fork/release
  and that it absorbed PR #21 with Robert P. Goldman credited.
- `panda-planner-dev/pandaPIgrounder`: announce the chengdu `v0.2.0`
  fork/release, in-tree vendoring at the recorded import SHA, and that no
  grounder upstream PR was absorbed for 0.2.0; Docker/workflow and status-policy
  work stayed out of scope/deferred.

Each issue should name:

- chengdu repo URL and `v0.2.0` release URL;
- the upstream import SHA from `vendor.env`;
- the absorbed PRs/issues relevant to that repo, if any;
- the reason for the fork: dormant upstream plus wolong's need for stable
  binary releases from in-tree source;
- no-pressure wording: this is FYI/courtesy and not a request for maintainer
  labor.

Record the three issue URLs in the ledger and closing report. If GitHub denies
issue creation or issues are disabled, stop and ask the operator; do not
silently downgrade this requirement to a local note.

## Constraints that bite

- The tag is the one-way door. Do not create or push `v0.2.0` until local gates
  and the pre-tag full matrix pass.
- Do not delete or replace a tag/release without explicit operator approval.
- Do not change wolong's frozen asset shape.
- Do not introduce 0.3.0 behavior work.
- Do not leave temp branches, temp remotes, dirty tracked files, nested `.git`
  directories, mode `160000` gitlinks, or generated files under `pandaPI/`.
- Do not write `cdc-verification.md`, Arc02 `closing-report.md`, or project
  close artifacts.

## Closing report requirements

Write
`docs/design-v0.2.0/arc02-upstream-absorption/slice04-release-publication/closing-report.md`
after publication and verification. It must include:

- release commit SHA, tag SHA/commit target, release URL, branch build run URL,
  and release workflow run URL;
- exact asset list and checksum/tarball/manifest verification evidence;
- macOS consumer-walk transcript summary and CDC Linux handoff;
- upstream courtesy issue URLs;
- a row-by-row walk of all 15 ledger rows;
- Bubble-up to the arc/project: whether Slice04 delivered the release piece of
  Arc02, whether Arc02 is ready for CDC verification and arc close, whether any
  arc/project docs changed, and the silent-drop diff.

## Definition of done

All 15 ledger rows at final status; `v0.2.0` is public, non-prerelease, and
download-verifiable; wolong's fetch shape is intact; courtesy announcements are
filed; worktree is clean; closing report exists; no CDC/arc/project close files
are written by CC.
