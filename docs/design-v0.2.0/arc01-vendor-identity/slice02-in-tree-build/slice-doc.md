# chengdu arc01 / slice02 - in-tree-build - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Make the vendored `pandaPI/` tree the active build source without changing
planner behavior. At slice close, local build scripts and GitHub Actions build
from in-tree source only, with no planner-source network fetch. The 0.1.0
compatibility patch behavior remains as a temporary bridge: patches still
apply during the build, but only inside disposable build copies so the
vendored source tree stays clean until slice03 turns those patches into normal
fork commits.

## 2. Scope

**In:**

- Repoint `scripts/build-parser.sh`, `scripts/build-grounder.sh`, and
  `scripts/build-engine.sh` from `upstream/pandaPI*` to `pandaPI/pandaPI*`.
- Add a disposable build-copy mechanism under an ignored `build/` tree, used
  by all three build scripts. The copy must be fresh enough that re-running a
  build does not inherit old patch or object state.
- Preserve current dist shape: binaries and `provenance.txt` still land in
  `dist/<platform>/`.
- Preserve current provenance values for this bridge slice: component SHAs
  still match `pins.env`, and grounder patch lists still match the 0.1.0
  expectations checked by `scripts/check-provenance.sh`.
- Update `.github/workflows/build-reusable.yml` so build and README-verbatim
  jobs no longer run `scripts/fetch-upstream.sh` and do not clone planner
  source.
- Update README source-build docs to the new in-tree build path. The release
  install/fetch spec is not changed.
- Keep `scripts/fetch-upstream.sh` only as historical/source-inspection
  tooling if retained; it must not be part of source build, CI build, release
  build, or README verbatim paths.
- Extend `.gitignore` for the disposable build tree.
- Run the standing local gate after repointing: three build scripts,
  `check-provenance.sh`, positive smoke, negative smoke, and local packaging
  dry-run.

**Out (disclosed, not dropped):**

- Converting `patches/*.patch` and upstream grounder patch files into in-tree
  source commits - slice03.
- Retiring `patches/` as an active build input - slice03.
- Re-anchoring release manifests on chengdu import commits instead of
  `pins.env` component SHAs - slice04.
- NOTICE files, `license-audit-v0.2.0.md`, `THIRD-PARTY-LICENSES`
  regeneration, and explicit provenance for `boruvka`/`opts` - slice04.
- Absorbing upstream PRs or issue fixes - arc02.

## 3. Constraints

- No planner-source network fetch in active build paths. GitHub Actions
  `actions/checkout`, artifact download/upload, and actionlint installation
  are not planner-source fetches; cloning `panda-planner-dev/*`,
  `gitlab.com/danfis/*`, or h2-fd-preprocessor source during build is banned.
- Do not patch or compile directly inside `pandaPI/`; builds must leave
  `git status --short -- pandaPI` empty.
- Bash remains stock macOS bash 3.2 compatible: no associative arrays,
  `mapfile`, or Bash 4+ features.
- Keep the behavior oracle unchanged: the same positive smoke, negative smoke,
  provenance check, artifact names, and package shape that 0.1.0 proved.
- Preserve the release install commands and wolong fetch spec. README source
  build commands may change; release consumption commands may not.

## 4. Verification approach

CC verifies on the local platform by running the full source-build chain from
a clean `build/` + `dist/` state, then running `check-provenance.sh`,
`smoke-test.sh`, `smoke-test.sh --negative`, and a packaging dry-run such as
`scripts/package-release.sh v0.2.0-slice02-smoke`. CDC reproduces the same
commands where possible and independently checks the static no-fetch
properties in scripts/workflows/README. A later slice04 arc-scale gate will
provide the full two-platform no-fetch CI evidence; this slice must make that
gate possible and locally checkable.

## 5. Exit criteria

The ledger's 12 rows at final status. Summary shape: build scripts consume
`pandaPI/` through disposable copies, `pandaPI/` stays clean after builds,
workflows and README remove active source fetch, provenance/smoke/package
gates still pass, shellcheck is clean, and `fetch-upstream.sh` is no longer an
active build dependency.

## 6. Implementation notes

The preferred build-copy shape is:

```text
build/
  <platform>/
    source/
      pandaPIparser/
      pandaPIgrounder/
      pandaPIengine/
```

Scripts may choose a slightly different ignored path, but the invariant is
load-bearing: copy from `pandaPI/`, mutate/build only in the copy, emit
artifacts to `dist/<platform>/`, and leave `pandaPI/` clean. If copying large
trees proves slow, optimize after preserving the invariant; do not trade away
source cleanliness to save seconds in this bridge slice.
