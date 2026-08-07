# Slice 02: in-tree-build

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-07. Closer: CC. Verifier: CDC. This slice repoints
> builds to in-tree source while preserving the 0.1.0 compatibility patch
> bridge.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Slice starts from `release/0.2.x` with slice01 closed and worktree clean. | `git branch --show-current`; `git status --short`; `test -f docs/design-v0.2.0/arc01-vendor-identity/slice01-subtree-import/cdc-verification.md` | serious | arc-plan section 2 | open | | |
| F-2 | Build scripts consume in-tree source under `pandaPI/`, not `upstream/`, and no active build script tells users to run `fetch-upstream.sh`. | `rg 'upstream/pandaPI|fetch-upstream\\.sh' scripts/build-*.sh scripts/lib-platform.sh` returns no active build-path references | serious | slice-doc section 2 | open | | Historical comments are allowed only if they do not describe the active build path. |
| F-3 | Builds use disposable ignored copies, and a build leaves `pandaPI/` clean: no patch residue, generated objects, binaries, or build dirs under vendored source. | clean `build/ dist/`; run build scripts; `git status --short -- pandaPI` empty; `find pandaPI -type f \\( -name '*.o' -o -name pandaPIparser -o -name pandaPIgrounder -o -name pandaPIengine \\) -print` shows no generated build outputs | serious | arc-plan OQ2 v1.2 | open | | Add `/build/` to `.gitignore` or equivalent ignored disposable path. |
| F-4 | Parser builds from in-tree source and emits `dist/<platform>/pandaPIparser` whose provenance SHA matches `PARSER_SHA`. | clean run `./scripts/build-parser.sh`; `dist/*/pandaPIparser --help`; inspect `dist/*/provenance.txt` parser block | serious | slice-doc section 2 | open | | |
| F-5 | Engine builds from in-tree source and emits `dist/<platform>/pandaPIengine` whose provenance SHA matches `ENGINE_SHA`. | clean run `./scripts/build-engine.sh`; `dist/*/pandaPIengine --help`; inspect `dist/*/provenance.txt` engine block | serious | slice-doc section 2 | open | | |
| F-6 | Grounder builds from in-tree source using the compatibility patch bridge in the disposable copy, emits `dist/<platform>/pandaPIgrounder`, and records the same platform patch list expected by `check-provenance.sh`. | clean run `./scripts/build-grounder.sh`; `dist/*/pandaPIgrounder --help`; `./scripts/check-provenance.sh`; inspect grounder patch list | serious | slice-doc section 2; arc-plan OQ2 v1.2 | open | | Patches still apply during build in slice02; retiring them is slice03. |
| F-7 | Full local source-build gate passes without running `fetch-upstream.sh`: parser build, grounder build, engine build, provenance check, positive smoke, negative smoke. | from clean `build/ dist/`: `./scripts/build-parser.sh`; `./scripts/build-grounder.sh`; `./scripts/build-engine.sh`; `./scripts/check-provenance.sh`; `./scripts/smoke-test.sh`; `./scripts/smoke-test.sh --negative` | serious | arc-plan A4 precursor | open | | |
| F-8 | GitHub Actions build and README-verbatim jobs no longer fetch planner source before building. | `rg 'fetch-upstream\\.sh|git clone|git submodule' .github/workflows` has no planner-source fetch in build/readme jobs; inspect `build-reusable.yml` steps | correctness | arc-plan A3 precursor | open | | `actions/checkout`, artifact actions, and actionlint installer are allowed. |
| F-9 | README source-build instructions describe the in-tree build path and mirror workflow commands; release install/fetch spec remains unchanged. | inspect README source-build command block; compare to readme-verbatim jobs; `git diff` confirms release install URLs/asset shape were not changed | correctness | project-plan P5; slice-doc section 3 | open | | Source-build command count may drop from 5 to 4. |
| F-10 | `scripts/fetch-upstream.sh` is no longer an active build dependency; if retained, docs mark it historical/source-inspection only. | `rg 'fetch-upstream\\.sh' README.md .github/workflows scripts/*.sh docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build` and inspect every hit | correctness | slice-doc section 2 | open | | Do not delete it unless the implementation can do so without widening the slice. |
| F-11 | Local release packaging dry-run still succeeds from in-tree-built `dist/` and preserves existing asset shape for present platforms. | after local gate, `./scripts/package-release.sh v0.2.0-slice02-smoke`; inspect `release/` for `pandapi-v0.2.0-slice02-smoke-<platform>.tar.gz`, `SHA256SUMS`, `release-manifest.txt`, `THIRD-PARTY-LICENSES` | correctness | project-plan P5 precursor | open | | Do not publish. `release/` remains ignored. |
| F-12 | Shell scripts and workflow syntax remain lintable. | `shellcheck scripts/*.sh`; actionlint on `.github/workflows/*.yml` if available, otherwise record unavailable tool and run YAML/static inspection | polish | local quality gate | open | | A missing optional linter is a disclosed verification limit, not a silent pass. |

## What Worked

_(At slice close. Patterns that made the slice close cleanly.)_

## Closure

Closed at commit <pending> on <date>. Verified by: <pending>.
Rows: 12. Done: <n>. Deferred: <n>. No-op: <n>.
