# Slice 03 closing report: patch-dissolution

Closed by: CC
Implementation commit: `0958e760f4b63f06dd6ff424ff7c472340d1e27b`
Date: 2026-08-07
Ledger rows: 12. Done: 12. Deferred: 0. No-op: 0.

## Per-row walk

### F-1 - slice start state

Status: done.

Evidence: the implementation started on `release/0.2.x` from clean worktree
state with
`docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build/cdc-verification.md`
present. The actual slice03 start commit was
`db2f8a45e1f4269d5f434aca00ecf1f79f35d86f`; `dd1fb5d6` is the slice02 CDC
verification commit immediately before the slice03 open commit.

### F-2 - durable bliss source

Status: done.

Evidence: `pandaPI/pandaPIgrounder/cpddl/third-party/bliss/bliss.cc` is tracked
source derived from the vendored `bliss-0.73.zip`. The cpddl make recipe builds
that source directly, and final `rg
'unzip|bliss-0.73-memleak.patch|bliss-0.73-cxx11-string-literal.patch'
pandaPI/pandaPIgrounder/cpddl/Makefile scripts/build-grounder.sh` returned no
matches.

### F-3 - cpddl makefile bridge

Status: done.

Evidence: `0958e760` folds the `0002-makefile.patch` behavior into cpddl
source/build metadata: Darwin-hostile `-lrt` links are gone, and former
submodule targets no longer fetch source. Final `rg 'git submodule|git
apply|patch -p| -lrt' pandaPI/pandaPIgrounder/cpddl/Makefile
pandaPI/pandaPIgrounder/cpddl/Makefile.include
pandaPI/pandaPIgrounder/cpddl/bin/Makefile` returned no matches.

### F-4 - boruvka macOS bridge deltas

Status: done.

Evidence: `0958e760` folds both boruvka macOS bridge deltas into
`third-party/boruvka/`: no `-lrt`, no `<endian.h>` include in the checked file,
and no `sem_timedwait` use in the checked timeout helpers. Final `rg
'#include <endian.h>|sem_timedwait| -lrt'` across the ledger-listed boruvka
files returned no matches.

### F-5 - grounder script no longer patches

Status: done.

Evidence: `scripts/build-grounder.sh` now only prepares the disposable source
copy, builds cpddl deps/lib and grounder source, copies the binary, and appends
grounder provenance with `patches=none`. Final `rg 'patch -p|git
apply|PATCHES_DIR|APPLIED_PATCHES|\.patch' scripts/build-grounder.sh` returned
no matches.

### F-6 - bridge patch files retired

Status: done.

Evidence: after `0958e760`, `git ls-files patches 'pandaPI/pandaPIgrounder/*.patch'`
returned empty, and `rg 'patches/' scripts README.md .github/workflows`
returned no matches. The cpddl bundled `bliss-0.73-memleak.patch` file was also
retired because its delta is now represented in durable bliss source and the
ledger's tracked-input check includes it.

### F-7 - provenance says no build-time patches

Status: done.

Evidence: `./scripts/check-provenance.sh` passed. The local
`dist/macos-arm64/provenance.txt` and post-commit `release/release-manifest.txt`
record all three components with their historical `pins.env` SHAs and
`patches=none`; the grounder SHA remains
`b04ff7bcca710ae80a1ce73b8c027f0224565afc` until slice04 re-anchors
provenance.

### F-8 - full local gate

Status: done.

Evidence: from clean generated output, `./scripts/build-parser.sh`,
`./scripts/build-grounder.sh`, `./scripts/build-engine.sh`,
`./scripts/check-provenance.sh`, `./scripts/smoke-test.sh`, and
`./scripts/smoke-test.sh --negative` all passed. Positive smoke reported `5
passed, 0 failed`; negative smoke reported `4 passed, 0 failed`. Active build
scripts contain no `patch`, `git apply`, or planner-source fetch path.

### F-9 - vendored source remains clean after builds

Status: done.

Evidence: after the full gate and implementation commit, `git status --short
-- pandaPI` returned empty. The generated-output `find` for `*.o`, `*.a`,
`pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` under `pandaPI/`
returned empty.

### F-10 - README/workflow wording

Status: done.

Evidence: README source-build notes now describe in-tree source with former
build-time compatibility deltas as normal source history. The workflow remains
on the slice02 no-fetch build path. Final `rg 'compatibility patch|patch
bridge|applied by script|patches remain' README.md .github/workflows` returned
no matches.

### F-11 - git-history provenance

Status: done.

Evidence: the required verification loop over `0002-makefile.patch`,
`0001-boruvka-endian.patch`,
`0001-Removed-non-macos-call-in-unused-function.patch`, and
`bliss-0.73-cxx11-string-literal.patch` passed. Commit `0958e760` carries a
`Dissolves-Patch` trailer and reason for each bridge patch.

### F-12 - lint/static/package gate

Status: done.

Evidence: `bash -n scripts/*.sh`, `shellcheck scripts/*.sh`, `actionlint
.github/workflows/*.yml`, `git diff --check`, and post-commit
`./scripts/package-release.sh v0.2.0-slice03-smoke` all passed. `release/`
contains `pandapi-v0.2.0-slice03-smoke-macos-arm64.tar.gz`, `SHA256SUMS`,
`release-manifest.txt`, `THIRD-PARTY-LICENSES`, `notes.md`, and `PRERELEASE`.
Nothing was published.

## What worked

- The slice02 disposable build-copy invariant gave this slice a clean way to
  test source-history deltas without dirtying vendored source during builds.
- Replacing dynamic bliss extraction with tracked source removed the most
  fragile part of the bridge and made `patches=none` a real build claim.
- A single implementation commit with four explicit `Dissolves-Patch` trailers
  keeps the audit trail compact while still naming every retired bridge delta.

## Bubble-up to the arc

1. Slice03 delivered the arc-plan assignment: the 0.1.0 compatibility patch
   bridge is no longer an active build input; the deltas are represented as
   ordinary in-tree source history, durable bliss source exists under cpddl,
   and the standing local behavior/package gate still passes.
2. The implementation revealed one boundary detail: the ledger's tracked-input
   check includes cpddl's bundled `bliss-0.73-memleak.patch`, so keeping it as
   historical source evidence would have left F-6 open. The file was retired
   after its contents were folded into durable bliss source. Slice04 can
   recover provenance from the vendored archive, the `0958e760` commit diff,
   and this close report when writing NOTICE/license material.
3. Silent-drop diff: scope-as-specified matches scope-as-delivered. No ledger
   row is deferred or no-op. Slice04 remains responsible for provenance
   re-anchoring, NOTICE/license audit work, release-manifest composition, and
   full two-platform CI evidence; none of that is claimed closed here.

Arc-plan update decision: no update required before slice04. The arc plan
already assigns provenance/licensing/release composition to slice04, and the
memleak-patch-file retirement does not change arc scope or sequencing.
