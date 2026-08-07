# Slice 03: patch-dissolution

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-07. Closer: CC. Verifier: CDC. This slice retires the
> 0.1.0 build-time patch bridge by turning those deltas into ordinary in-tree
> source history.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Slice starts from `release/0.2.x` with slice02 CDC-verified and worktree clean. | `git branch --show-current`; `git status --short`; `test -f docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build/cdc-verification.md` | serious | arc-plan section 2 | open | | Slice start commit is expected to be `dd1fb5d6`. |
| F-2 | OQ3 is implemented as durable extracted bliss source under `pandaPI/pandaPIgrounder/cpddl/third-party/bliss/`, and builds no longer unzip bliss during normal source builds. | `test -f pandaPI/pandaPIgrounder/cpddl/third-party/bliss/bliss.cc`; `rg 'unzip|bliss-0.73-memleak.patch|bliss-0.73-cxx11-string-literal.patch' pandaPI/pandaPIgrounder/cpddl/Makefile scripts/build-grounder.sh` has no active build recipe hits | serious | arc-plan OQ3 v1.3 | open | | If historical mentions remain, they must be comments/docs, not executable recipe lines. |
| F-3 | The cpddl makefile bridge delta from `0002-makefile.patch` is represented in in-tree source/build metadata, without active submodule fetch or patch commands. | inspect `pandaPI/pandaPIgrounder/cpddl/Makefile`, `Makefile.include`, and `bin/Makefile`; `rg 'git submodule|git apply|patch -p| -lrt' pandaPI/pandaPIgrounder/cpddl/Makefile pandaPI/pandaPIgrounder/cpddl/Makefile.include pandaPI/pandaPIgrounder/cpddl/bin/Makefile` returns no active bridge residue | serious | slice-doc section 2 | open | | Preserve Linux behavior while removing Darwin-hostile `-lrt` paths. |
| F-4 | The two macOS boruvka bridge deltas are represented in `third-party/boruvka/` source and build metadata. | `rg '#include <endian.h>|sem_timedwait| -lrt' pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/Makefile pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/test/Makefile pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/src/msg-schema.c pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/src/fifo-sem.c pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/src/ring_queue.c` returns no active bridge residue | serious | slice-doc section 2 | open | | The row verifies the existing bridge behavior is source, not a build-time patch. |
| F-5 | `scripts/build-grounder.sh` no longer applies patches, tracks an applied patch list, or consumes `patches/`; it builds only copied in-tree source. | `rg 'patch -p|git apply|PATCHES_DIR|APPLIED_PATCHES|\\.patch' scripts/build-grounder.sh` returns no active patch-bridge hits; inspect grounder build flow | serious | arc-plan A5 precursor | open | | The script may still use `prepare_build_source_copy`. |
| F-6 | Bridge patch files are retired as active inputs after their deltas become source history. | `test -z "$(git ls-files patches 'pandaPI/pandaPIgrounder/*.patch')"`; `rg 'patches/' scripts README.md .github/workflows` has no active build-path hits | serious | project-plan section 3; arc-plan A5 | open | | cpddl's bundled `third-party/bliss-0.73-memleak.patch` may remain only as historical/source evidence, not an active build input. |
| F-7 | Grounder provenance reports no build-time patches on supported platforms while component SHAs still match `pins.env` until slice04 re-anchors provenance. | clean build; `./scripts/check-provenance.sh`; inspect `dist/*/provenance.txt` grounder block has `patches=none`; inspect `scripts/check-provenance.sh` expected grounder patch list | serious | slice-doc section 2; arc-plan A5 | open | | Slice04 owns replacing the historical SHA scheme. |
| F-8 | Full local source-build gate passes without planner-source network fetch or build-time patch application. | from clean `build/ dist/ release/`: `./scripts/build-parser.sh`; `./scripts/build-grounder.sh`; `./scripts/build-engine.sh`; `./scripts/check-provenance.sh`; `./scripts/smoke-test.sh`; `./scripts/smoke-test.sh --negative` | serious | arc-plan A4/A5 precursor | open | | No `fetch-upstream.sh`, `patch`, or `git apply` command should be part of the build scripts. |
| F-9 | Builds still leave the vendored source tree clean; generated bliss, boruvka, cpddl, parser, grounder, and engine artifacts stay in ignored build/dist/release paths. | after full gate, `git status --short -- pandaPI` empty; `find pandaPI -type f \\( -name '*.o' -o -name '*.a' -o -name pandaPIparser -o -name pandaPIgrounder -o -name pandaPIengine \\) -print` shows no generated build outputs | serious | slice02 invariant | open | | Durable bliss source is allowed; generated objects/libs are not. |
| F-10 | README and workflow/source-build documentation no longer describe active compatibility patches applied by scripts. | inspect README source-build notes and `.github/workflows/build-reusable.yml`; `rg 'compatibility patch|patch bridge|applied by script|patches remain' README.md .github/workflows` returns no stale active-build wording | correctness | slice-doc section 2 | open | | Planning docs may still discuss historical slice scope. |
| F-11 | Patch-dissolution provenance is present in git history for each bridge delta. | `for p in 0002-makefile.patch 0001-boruvka-endian.patch 0001-Removed-non-macos-call-in-unused-function.patch bliss-0.73-cxx11-string-literal.patch; do git log dd1fb5d6..HEAD --format='%H%n%s%n%B' -- pandaPI scripts README.md | rg "Dissolves-Patch: $p" >/dev/null || exit 1; done` | correctness | slice-doc section 3 | open | | One commit per logical delta is preferred; if combined, the message must name every dissolved patch. |
| F-12 | Shell scripts and workflow syntax remain lintable, and local packaging still preserves asset shape. | `bash -n scripts/*.sh`; `shellcheck scripts/*.sh`; `actionlint .github/workflows/*.yml`; `./scripts/package-release.sh v0.2.0-slice03-smoke`; `git diff --check` | polish | local quality gate | open | | Do not publish. `release/` remains ignored. |

## What Worked

_(At slice close. Patterns that made the slice close cleanly.)_

## Closure

Closed at commit <pending> on <date>. Verified by: <pending>.
Rows: 12. Done: <n>. Deferred: <n>. No-op: <n>.
