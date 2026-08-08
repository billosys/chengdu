# Slice 04 closing report: provenance-notice-gate

> CC close for arc01 slice04, 2026-08-07. Ledger:
> [`ledger.md`](ledger.md). Implementation commits: `acaf183f`
> (`build: reanchor vendored provenance`) and `258311a1`
> (`ci: fetch chengdu history for provenance checks`).

## Ledger walk

### F-1 - start state

Status: done.

Evidence: the slice started on `release/0.2.x` at `38fca56d`, with a clean
worktree and `docs/design-v0.2.0/arc01-vendor-identity/slice03-patch-dissolution/cdc-verification.md`
present.

### F-2 - vendor.env

Status: done.

Evidence: `vendor.env` landed with all seven history-imported identities:
parser, grounder, engine, cpddl, h2-fd-preprocessor, boruvka, and opts. Each
entry records prefix, URL, upstream SHA, and import commit. `pins.env` is now
explicitly historical, and active build/check scripts no longer source it.

### F-3 - provenance schema

Status: done.

Evidence: local `dist/macos-arm64/provenance.txt` records `chengdu_commit`,
`source_prefix`, `upstream_url`, `upstream_sha`, `import_commit`,
`patches=none`, and `compiler` for all three shipped components. `rg '^sha='`
returns no matches.

### F-4 - provenance checker

Status: done.

Evidence: `./scripts/check-provenance.sh` passed locally and in CI. A tampered
copy with a zeroed parser `import_commit` failed with a named
`pandaPIparser: import_commit mismatch` error and nonzero exit.

### F-5 - build scripts

Status: done.

Evidence: `scripts/build-parser.sh`, `scripts/build-grounder.sh`, and
`scripts/build-engine.sh` source `vendor.env` and call `append_provenance`
with component metadata keys. Final `rg 'pins\.env|PARSER_SHA|GROUNDER_SHA|ENGINE_SHA'`
over active build/check scripts returned no matches.

### F-6 - retired fetch helper

Status: done.

Evidence: `scripts/fetch-upstream.sh` was deleted. The final no-fetch grep over
README, workflows, scripts, v0.2 design docs, and release notes returned no
matches for the retired helper or planner-source clone patterns.

### F-7 - NOTICE files

Status: done.

Evidence: all nine expected NOTICE files exist under `pandaPI/`: parser,
grounder, engine, cpddl, h2-fd-preprocessor, boruvka, opts, bliss, and
lpsolve. Field grep confirms Origin, License, Import, Derived, Divergence, and
Arc01 coverage.

### F-8 - license audit v0.2.0

Status: done.

Evidence: `docs/license-audit-v0.2.0.md` landed and distinguishes identity
imports, slice03 patch-dissolution, and arc02 deferrals. It explicitly covers
bliss, boruvka, lpsolve, opts, h2-fd-preprocessor, GPL-3.0, BSD 3-Clause, and
LGPL terms.

### F-9 - license bundle

Status: done.

Evidence: `licenses/README.md` and `scripts/package-release.sh` now reference
`docs/license-audit-v0.2.0.md`. The package dry-run generated
`release/THIRD-PARTY-LICENSES` with v0.2.0 audit authority and the required
texts for shipped binary components.

### F-10 - README and release notes

Status: done.

Evidence: README and `.github/release-notes.md.in` describe source
availability through chengdu at the release commit. The rendered provenance
table names `chengdu commit`, `source_prefix`, `upstream_sha`, and
`import_commit`; stale v0.1.0/upstream-patch wording is absent from active
README/release-note paths.

### F-11 - local behavioral gate

Status: done.

Evidence: after clearing generated `build/` and `dist/`, the local macOS gate
passed: parser build, grounder build, engine build, `check-provenance.sh`,
positive smoke (`5 passed, 0 failed`), negative smoke (`4 passed, 0 failed`),
and `./scripts/package-release.sh v0.2.0-slice04-smoke`.

### F-12 - GitHub Actions no-fetch gate

Status: done.

Evidence: run https://github.com/billosys/chengdu/actions/runs/31238964833
completed successfully for head
`258311a16e40529d9411fd2fc39f95eccc3cd090`, event `push`. The ubuntu-22.04
and macos-15 build jobs passed build, provenance check, positive/negative
smoke, and artifact upload; ubuntu-24.04 passed the forward build check;
README-verbatim jobs and cross-compat jobs passed. Log grep over build and
README jobs for `panda-planner-dev`, `gitlab.com/danfis`,
`galvusdamor/h2-fd-preprocessor`, and the retired fetch helper returned no
matches.

Note: run 31238874950 failed first because Actions checkout was shallow and
the new Git ancestry check could not resolve historical import commits.
Commit `258311a1` fixed this by setting `fetch-depth: 0` for chengdu checkout
jobs. This fetches chengdu history only, not planner source.

### F-13 - release artifact shape

Status: done.

Evidence: local package dry-run generated `release/release-manifest.txt`,
`release/notes.md`, `release/SHA256SUMS`, `release/THIRD-PARTY-LICENSES`, and
`pandapi-v0.2.0-slice04-smoke-macos-arm64.tar.gz`. The tarball contains
`fixtures/`, `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, and
`provenance.txt`; checksum verification passed. CI artifact upload proves the
two-platform equivalent gate.

### F-14 - lint and source cleanliness

Status: done.

Evidence: `bash -n scripts/*.sh`, `shellcheck scripts/*.sh`, `actionlint
.github/workflows/*.yml`, and `git diff --check` passed. After the build and
commits, `git status --short -- pandaPI` and generated-output `find` both
returned empty.

## What Worked

- Re-anchoring provenance on the chengdu commit made the post-slice public
  contract match reality: binaries are built from chengdu source, while
  upstream SHAs are import identities.
- The shallow-checkout CI failure was useful: it proved the import-ancestry
  gate was real, not just string comparison. Fetching full chengdu history is
  now part of the workflow contract.
- Keeping packaging asset shape unchanged limited wolong-facing risk while
  letting the manifest and release notes carry the v0.2.0 source story.

## Bubble-up to the arc

1. Slice04 delivered the piece assigned by the arc plan: provenance and
   release manifests are re-anchored on chengdu commit plus import identities,
   NOTICE and license-audit coverage is explicit, release docs are updated,
   and the two-platform no-fetch CI gate is green.

2. The only implementation finding that affected arc mechanics was the need
   for full chengdu history in CI checkout. This does not change arc scope or
   sequencing; it is the mechanism required to verify A2/A6 import ancestry in
   a clean runner.

3. Silent-drop diff: no requested slice04 scope was dropped. Real v0.2.0
   publication and upstream PR/issue absorption remain out of scope as planned
   for arc02. No arc-plan body change is required before arc01 close; CDC can
   use this close plus its independent verification to close arc01's ledger.
