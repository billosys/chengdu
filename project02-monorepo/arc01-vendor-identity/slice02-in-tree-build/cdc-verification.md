# CDC verification - chengdu arc01 / slice02 - in-tree-build

Verified by CDC on 2026-08-07, against `7c326e1e` on `release/0.2.x`.
Implementation commit: `fde8c10650e8186e3bfca3d2194846d1e26afa5e`.
Role: independent ledger reproduction and bubble-up check, per
`LEDGER-DISCIPLINE.md` Section A and `PROJECT-MANAGEMENT.md` Part IV.

## Verdict

**Accepted.** Rows F-1 through F-12 reproduce or check cleanly on
`macos-arm64`. The slice delivered its assigned piece of arc01: active local
and workflow build paths now consume the in-tree `pandaPI/` source, do not run
planner-source fetches, preserve the 0.1.0 provenance/patch bridge, and leave
the vendored source tree clean by building in ignored disposable copies.

No arc-plan update is required before slice03. The only implementation
discovery, using `patch -p1` rather than `git apply` inside disposable source
copies, is already contained by slice03's planned patch-dissolution scope.

## Row Reproduction

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Current branch is `release/0.2.x`; `git status --short` returned empty; `test -f docs/design-v0.2.0/arc01-vendor-identity/slice01-subtree-import/cdc-verification.md` exited 0. Commit history shows slice02 opened at `3998315b` and closed by `fde8c106` + `7c326e1e`. |
| F-2 | reproduced | `rg 'upstream/pandaPI|fetch-upstream\.sh' scripts/build-*.sh scripts/lib-platform.sh` returned no matches. `scripts/build-parser.sh`, `scripts/build-grounder.sh`, and `scripts/build-engine.sh` call `prepare_build_source_copy` for `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`. |
| F-3 | reproduced | From a cleared `build/ dist/ release/` state, the three build scripts recreated `build/macos-arm64/source/pandaPIparser`, `build/macos-arm64/source/pandaPIgrounder`, and `build/macos-arm64/source/pandaPIengine`. After the builds, `git status --short -- pandaPI` returned empty and `find pandaPI -type f \( -name '*.o' -o -name pandaPIparser -o -name pandaPIgrounder -o -name pandaPIengine \) -print` returned empty. |
| F-4 | reproduced | `./scripts/build-parser.sh` passed; `dist/macos-arm64/pandaPIparser --help` exited 0; `dist/macos-arm64/provenance.txt` records parser SHA `88c0995c72c5ce2685e9546ec153a3545977ed81`, matching `PARSER_SHA`. |
| F-5 | reproduced | `./scripts/build-engine.sh` passed; `dist/macos-arm64/pandaPIengine --help` exited 0; `dist/macos-arm64/provenance.txt` records engine SHA `810f04388667db5e3e4f114e960a4efbb43b1ac0`, matching `ENGINE_SHA`. |
| F-6 | reproduced | `./scripts/build-grounder.sh` passed; `dist/macos-arm64/pandaPIgrounder --help` exited 0; `./scripts/check-provenance.sh` passed; provenance records `0002-makefile.patch,0001-Removed-non-macos-call-in-unused-function.patch,0001-boruvka-endian.patch,bliss-0.73-cxx11-string-literal.patch`. |
| F-7 | reproduced | From a cleared `build/ dist/ release/` state: parser build, grounder build, engine build, `./scripts/check-provenance.sh`, `./scripts/smoke-test.sh`, and `./scripts/smoke-test.sh --negative` all passed. Positive smoke reported `5 passed, 0 failed`; negative smoke reported `4 passed, 0 failed`. No fetch command was run. |
| F-8 | reproduced | `rg 'fetch-upstream\.sh|git clone|git submodule' .github/workflows` returned no matches. Inspection of `.github/workflows/build-reusable.yml` confirms build and README-verbatim jobs run checkout, prerequisites, three builds, provenance/smoke gates, and artifact steps without planner-source clone/fetch steps. |
| F-9 | reproduced | README source-build section is now "Build + smoke test (4 commands)" with parser, grounder, engine, smoke. README notes say source builds use in-tree `pandaPI/` and do not clone planner source. Release install URLs and asset names remain the `v0.1.0` Linux/macOS tarballs and `SHA256SUMS` shape. |
| F-10 | reproduced | `rg -n 'fetch-upstream\.sh' README.md .github/workflows scripts/*.sh docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build` showed hits only in README's historical/source-inspection note, the retained `retired upstream-fetch helper`, and slice02 planning/close docs. No workflow or active build-script hits remain. |
| F-11 | reproduced | `./scripts/package-release.sh v0.2.0-slice02-smoke` passed for `macos-arm64`; `release/` contains `pandapi-v0.2.0-slice02-smoke-macos-arm64.tar.gz`, `SHA256SUMS`, `release-manifest.txt`, `THIRD-PARTY-LICENSES`, `notes.md`, and `PRERELEASE`. Nothing was published; `release/` remains ignored. |
| F-12 | reproduced | `bash -n scripts/*.sh`, `shellcheck scripts/*.sh`, `actionlint .github/workflows/*.yml`, and `git diff --check` all passed. |

## Additional Checks

- `dist/macos-arm64/provenance.txt` contains exactly the three expected
  component blocks and `check-provenance.sh` verifies all three against
  `pins.env`.
- `.gitignore` now ignores `/build/`; generated `build/`, `dist/`, and
  `release/` outputs do not appear in `git status --short`.
- Upstream compiler warnings were observed during local builds, but all build
  commands exited 0 and the lint/static gates for maintained scripts and
  workflows passed.

## Bubble-up Check

**Delivered as assigned:** yes. Slice02 repointed scripts and workflow source
builds from fetched `upstream/` checkouts to the vendored `pandaPI/` tree,
while preserving the 0.1.0 bridge behavior through patches applied inside
ignored disposable source copies.

**Arc-plan change required:** no. The implementation detail about using
`patch -p1` instead of `git apply` in disposable copies does not change arc
scope or sequencing. Slice03 already owns dissolving the active patch bridge
into normal in-tree commits.

**Silent-drop diff:** complete. Scope-as-specified matches
scope-as-delivered: active build scripts use `pandaPI/`, no active build path
runs `retired upstream-fetch helper`, README/workflow source-build paths are updated,
release install shape is unchanged, provenance/smoke/package gates pass, and
vendored source remains clean after builds. The out-of-scope items remain
assigned to slice03, slice04, or arc02 as documented in the slice plan.
