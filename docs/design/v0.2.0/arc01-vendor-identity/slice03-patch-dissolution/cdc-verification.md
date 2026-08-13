# CDC verification - chengdu arc01 / slice03 - patch-dissolution

Verified by CDC on 2026-08-07, against `a312969f` on `release/0.2.x`.
Implementation commit: `0958e760f4b63f06dd6ff424ff7c472340d1e27b`.
Role: independent ledger reproduction and bubble-up check, per
`LEDGER-DISCIPLINE.md` Section A and `PROJECT-MANAGEMENT.md` Part IV.

## Verdict

**Accepted.** Rows F-1 through F-12 reproduce or check cleanly on
`macos-arm64`. The slice delivered its assigned piece of arc01: the active
0.1.0 compatibility patch bridge is dissolved, grounder builds consume ordinary
in-tree source history, durable bliss source is tracked under cpddl, provenance
reports `patches=none`, and the local behavior/package gate still passes.

No arc-plan update is required before slice04. The only boundary discovery,
retiring cpddl's bundled `bliss-0.73-memleak.patch` after folding its contents
into durable bliss source, is compatible with the existing slice04 assignment
for provenance re-anchoring, NOTICE/license audit work, and release-manifest
composition.

## Row Reproduction

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Current branch is `release/0.2.x`; the pre-verification `git status --short` returned empty; `test -f docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build/cdc-verification.md` exited 0. Commit history shows slice03 opened at `db2f8a45`, implemented by `0958e760`, and closed by `a312969f`. |
| F-2 | reproduced | `test -f pandaPI/pandaPIgrounder/cpddl/third-party/bliss/bliss.cc` exited 0. `rg 'unzip|bliss-0.73-memleak.patch|bliss-0.73-cxx11-string-literal.patch' pandaPI/pandaPIgrounder/cpddl/Makefile scripts/build-grounder.sh` returned no matches. |
| F-3 | reproduced | Inspection of cpddl build metadata matched the ledger claim: former submodule/update behavior is replaced by local-file checks and Darwin-hostile `-lrt` links are absent. `rg 'git submodule|git apply|patch -p| -lrt' pandaPI/pandaPIgrounder/cpddl/Makefile pandaPI/pandaPIgrounder/cpddl/Makefile.include pandaPI/pandaPIgrounder/cpddl/bin/Makefile` returned no matches. |
| F-4 | reproduced | `rg '#include <endian.h>|sem_timedwait| -lrt' pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/Makefile pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/test/Makefile pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/src/msg-schema.c pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/src/fifo-sem.c pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/src/ring_queue.c` returned no matches. |
| F-5 | reproduced | `scripts/build-grounder.sh` now prepares the disposable `pandaPIgrounder` source copy, builds cpddl dependencies/library and grounder source, copies the binary, and appends grounder provenance with `patches=none`. `rg 'patch -p|git apply|PATCHES_DIR|APPLIED_PATCHES|\.patch' scripts/build-grounder.sh` returned no matches. |
| F-6 | reproduced | `git ls-files patches 'pandaPI/pandaPIgrounder/*.patch'` returned empty. `rg 'patches/' scripts README.md .github/workflows` returned no matches. The retired tracked inputs include the former top-level bridge patches and cpddl's bundled `bliss-0.73-memleak.patch`. |
| F-7 | reproduced | From a cleared `build/ dist/ release/` state, the three builds passed and `./scripts/check-provenance.sh` reported all 3 components verified against `pins.env`. `dist/macos-arm64/provenance.txt` records parser, grounder, and engine with their historical SHAs and `patches=none`; the grounder block records SHA `b04ff7bcca710ae80a1ce73b8c027f0224565afc`. |
| F-8 | reproduced | From clean generated output, `./scripts/build-parser.sh`, `./scripts/build-grounder.sh`, `./scripts/build-engine.sh`, `./scripts/check-provenance.sh`, `./scripts/smoke-test.sh`, and `./scripts/smoke-test.sh --negative` all passed. Positive smoke reported `5 passed, 0 failed`; negative smoke reported `4 passed, 0 failed`. |
| F-9 | reproduced | After the full gate, `git status --short -- pandaPI` returned empty, and `find pandaPI -type f \( -name '*.o' -o -name '*.a' -o -name pandaPIparser -o -name pandaPIgrounder -o -name pandaPIengine \) -print` returned empty. Generated outputs stayed under ignored `build/`, `dist/`, and `release/` paths. |
| F-10 | reproduced | README source-build notes now describe former build-time compatibility deltas as normal in-tree source history. `.github/workflows/build-reusable.yml` remains on the slice02 no-fetch source-build path. `rg 'compatibility patch|patch bridge|applied by script|patches remain' README.md .github/workflows` returned no matches. |
| F-11 | reproduced | The verification loop over `0002-makefile.patch`, `0001-boruvka-endian.patch`, `0001-Removed-non-macos-call-in-unused-function.patch`, and `bliss-0.73-cxx11-string-literal.patch` passed. Commit `0958e760` carries `Dissolves-Patch` trailers for all four bridge patch names. |
| F-12 | reproduced | `bash -n scripts/*.sh`, `shellcheck scripts/*.sh`, `actionlint .github/workflows/*.yml`, `git diff --check`, and `./scripts/package-release.sh v0.2.0-slice03-smoke` all passed. `release/` contains `pandapi-v0.2.0-slice03-smoke-macos-arm64.tar.gz`, `SHA256SUMS`, `release-manifest.txt`, `THIRD-PARTY-LICENSES`, `notes.md`, and `PRERELEASE`; nothing was published. |

## Additional Checks

- `dist/macos-arm64/provenance.txt` contains exactly the three expected
  component blocks and records `patches=none` for parser, grounder, and engine.
- `scripts/check-provenance.sh` now expects `none` for every component patch
  field while preserving the historical component SHA checks until slice04.
- `git status --short` returned empty after the reproduced build, smoke,
  package, lint, and static checks; generated outputs remain ignored.
- Upstream compiler warnings were observed during local builds, but all build
  commands exited 0 and the maintained script/workflow gates passed.

## Bubble-up Check

**Delivered as assigned:** yes. Slice03 dissolved the active grounder patch
bridge into in-tree source history, replaced dynamic bliss extraction with
tracked durable source, retired active bridge patch files, and made
`patches=none` true in build provenance.

**Arc-plan change required:** no. The implementation detail around retiring
cpddl's bundled `bliss-0.73-memleak.patch` does not change arc scope or
sequencing. Slice04 already owns provenance re-anchoring, NOTICE/license audit
work, release-manifest composition, and two-platform CI evidence; it can recover
the necessary history from the vendored archive, the `0958e760` diff, and the
slice close reports.

**Silent-drop diff:** complete. Scope-as-specified matches scope-as-delivered:
no ledger row is deferred or no-op, no active build path applies source patches
or fetches planner source, README/workflow language no longer describes an
active compatibility patch bridge, local behavior/package gates pass, and
vendored `pandaPI/` remains clean after builds. The remaining arc01 work is
still correctly assigned to slice04.
