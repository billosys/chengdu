# Slice 02 closing report: in-tree-build

Closed by: CC
Implementation commit: `fde8c10650e8186e3bfca3d2194846d1e26afa5e`
Date: 2026-08-06
Ledger rows: 12. Done: 12. Deferred: 0. No-op: 0.

## Per-row walk

### F-1 - slice start state

Status: done.

Evidence: the slice started on `release/0.2.x` at
`3998315b9ea9d2345bef30ba9cfbca11de7418dd` with a clean worktree, and
`docs/design-v0.2.0/arc01-vendor-identity/slice01-subtree-import/cdc-verification.md`
present.

### F-2 - active build source

Status: done.

Evidence: `scripts/build-parser.sh`, `scripts/build-grounder.sh`, and
`scripts/build-engine.sh` now call `prepare_build_source_copy` for
`pandaPI/pandaPIparser`, `pandaPI/pandaPIgrounder`, and `pandaPI/pandaPIengine`.
Final `rg 'upstream/pandaPI|fetch-upstream\.sh' scripts/build-*.sh
scripts/lib-platform.sh` returned no active build-path references.

### F-3 - disposable ignored copies

Status: done.

Evidence: `/build/` is ignored. A clean final build recreated
`build/macos-arm64/source/pandaPIparser`,
`build/macos-arm64/source/pandaPIgrounder`, and
`build/macos-arm64/source/pandaPIengine`; after the build,
`git status --short -- pandaPI` returned empty, and the generated-output
`find` for object files and component binaries under `pandaPI/` returned
empty.

### F-4 - parser build and provenance

Status: done.

Evidence: from clean `build/ dist/`, `./scripts/build-parser.sh` passed,
`dist/macos-arm64/pandaPIparser --help` exited 0, and
`dist/macos-arm64/provenance.txt` records parser SHA
`88c0995c72c5ce2685e9546ec153a3545977ed81`.

### F-5 - engine build and provenance

Status: done.

Evidence: from clean `build/ dist/`, `./scripts/build-engine.sh` passed,
`dist/macos-arm64/pandaPIengine --help` exited 0, and
`dist/macos-arm64/provenance.txt` records engine SHA
`810f04388667db5e3e4f114e960a4efbb43b1ac0`.

### F-6 - grounder build and patch bridge

Status: done.

Evidence: from clean `build/ dist/`, `./scripts/build-grounder.sh` passed,
`dist/macos-arm64/pandaPIgrounder --help` exited 0, and
`./scripts/check-provenance.sh` passed. The grounder provenance patch list is
`0002-makefile.patch,0001-Removed-non-macos-call-in-unused-function.patch,0001-boruvka-endian.patch,bliss-0.73-cxx11-string-literal.patch`.

### F-7 - full local gate

Status: done.

Evidence: from clean `build/ dist/`, the final local source-build sequence
passed: `./scripts/build-parser.sh`, `./scripts/build-grounder.sh`,
`./scripts/build-engine.sh`, `./scripts/check-provenance.sh`,
`./scripts/smoke-test.sh`, and `./scripts/smoke-test.sh --negative`. Positive
smoke reported `5 passed, 0 failed`; negative smoke reported `4 passed, 0
failed`. No `fetch-upstream.sh` command was run.

### F-8 - workflow no-fetch state

Status: done.

Evidence: `.github/workflows/build-reusable.yml` no longer has `Fetch
upstream` steps in Linux/macOS build jobs and no longer runs
`./scripts/fetch-upstream.sh` in README-verbatim jobs. Final `rg
'fetch-upstream\.sh|git clone|git submodule' .github/workflows` returned no
matches.

### F-9 - README source-build and release install shape

Status: done.

Evidence: README source-build instructions now list four build/smoke commands:
parser, grounder, engine, smoke. README notes state that source builds use
in-tree `pandaPI/` source and do not clone planner source. The release install
commands still reference the same `v0.1.0` Linux and macOS tarballs and asset
names.

### F-10 - retained historical fetch script

Status: done.

Evidence: final `rg 'fetch-upstream\.sh' README.md .github/workflows
scripts/*.sh docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build`
showed hits only in the README historical/source-inspection note, the retained
`scripts/fetch-upstream.sh` file itself, and slice02 planning/ledger files.
There were no workflow hits and no active build-script hits.

### F-11 - local packaging dry-run

Status: done.

Evidence: after the local gate, `./scripts/package-release.sh
v0.2.0-slice02-smoke` passed for `macos-arm64`. `release/` contains
`pandapi-v0.2.0-slice02-smoke-macos-arm64.tar.gz`, `SHA256SUMS`,
`release-manifest.txt`, `THIRD-PARTY-LICENSES`, `notes.md`, and `PRERELEASE`.
Nothing was published.

### F-12 - lint/static checks

Status: done.

Evidence: `bash -n scripts/*.sh`, `shellcheck scripts/*.sh`, `actionlint
.github/workflows/*.yml`, and `git diff --check` all passed.

## What worked

- The single helper `prepare_build_source_copy` kept parser, grounder, and
  engine source handling consistent without adding per-component variants.
- Keeping the 0.1.0 provenance SHAs in `pins.env` preserved the existing
  `check-provenance.sh` contract while the active source roots changed.
- Applying the cpddl makefile bridge patch with `patch -p1` avoided relying on
  Git metadata inside the disposable copy.

## Bubble-up to the arc

1. Slice02 delivered the arc-plan assignment: local scripts and GitHub workflow
   source-build paths consume in-tree `pandaPI/` source, perform no
   planner-source network fetch at build time, and preserve the 0.1.0
   compatibility patch behavior as a bridge.
2. The implementation surfaced one operational detail: `git apply` is not the
   right primitive for bridge patches inside disposable ignored copies. The
   build now uses `patch -p1` for the cpddl makefile patch. This does not
   require an arc-plan change because slice03 is already assigned to dissolve
   build-time patches into normal in-tree commits.
3. Silent-drop diff: scope-as-specified matches scope-as-delivered. No
   deliverables were deferred or dropped. Out-of-scope items remain assigned to
   slice03, slice04, or arc02 as stated in the slice doc.

Arc-plan update decision: no update required before the next slice. The
existing slice03 plan already owns patch dissolution, and this slice did not
change arc sequencing or scope.
