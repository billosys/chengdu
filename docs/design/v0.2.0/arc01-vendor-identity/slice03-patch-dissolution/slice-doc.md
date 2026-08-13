# chengdu arc01 / slice03 - patch-dissolution - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Turn the 0.1.0 compatibility patch bridge into normal in-tree source history.
At slice close, source builds still use disposable `build/` copies, but no
active build path applies patch files. The four bridge deltas are represented
as ordinary source changes under `pandaPI/`, with provenance in commit
messages, and the build/provenance scripts report that no build-time patches
remain.

## 2. Scope

**In:**

- Dissolve the cpddl makefile bridge from `0002-makefile.patch` into
  `pandaPI/pandaPIgrounder/cpddl/` source/build metadata, including the
  portable link/compiler pieces still needed after slice02.
- Dissolve the two macOS boruvka bridge patches into
  `pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/` source.
- Resolve the bliss representation by extracting durable source to
  `pandaPI/pandaPIgrounder/cpddl/third-party/bliss/`, derived from the
  vendored `bliss-0.73.zip`, then applying cpddl's existing memleak fix and
  chengdu's `bliss-0.73-cxx11-string-literal.patch` as source changes.
- Remove build-time patch application from `scripts/build-grounder.sh` and
  adjust cpddl/bliss build recipes so they compile in-tree source instead of
  unzipping and patching during the build.
- Retire bridge patch files as active inputs: `patches/` should no longer be
  needed, and the grounder-level `000*.patch` files should not remain as
  build inputs after their deltas are source commits.
- Keep the slice02 disposable build-copy invariant: builds may mutate and
  compile inside `build/<platform>/source/`, not inside `pandaPI/`.
- Update `scripts/check-provenance.sh` and packaging behavior so the grounder
  provenance patch field is `none` on supported platforms while component
  SHAs still match the historical import pins until slice04 re-anchors
  provenance.
- Update README/source-build wording so it no longer describes active
  build-time compatibility patches.
- Run the standing local gate: parser, grounder, engine, provenance check,
  positive smoke, negative smoke, packaging dry-run, shell/script/workflow
  static checks.

**Out (disclosed, not dropped):**

- Re-anchoring provenance and release manifests on chengdu commits instead of
  `pins.env` component SHAs - slice04.
- NOTICE files, `license-audit-v0.2.0.md`, `THIRD-PARTY-LICENSES`
  regeneration, and explicit license/provenance text for extracted bliss,
  boruvka, opts, and h2 - slice04.
- Absorbing upstream PRs or issue fixes - arc02.
- Behavioral redesigns beyond preserving the 0.1.0 gate behavior - 0.3.0.
- Publishing a release.

## 3. Constraints

- No active build path may run `patch`, `git apply`, or consume files under
  `patches/` to make planner source buildable.
- Preserve bash 3.2 compatibility in maintained scripts.
- Preserve the 0.1.0 behavior oracle: positive smoke, negative smoke,
  provenance check, artifact names, and package shape.
- Do not compile directly under `pandaPI/`; generated objects, libraries, and
  binaries belong under ignored `build/`, `dist/`, or `release/`.
- Commit provenance matters. Each dissolved bridge delta needs a commit
  message or trailer naming the original patch file and why the change is
  now source history.
- Do not widen into slice04's public provenance/licensing rewrite. It is OK
  to make build recipes stop consuming patch files; it is not OK to claim the
  full license-audit/NOTICE surface is closed here.

## 4. Verification approach

CC verifies locally by starting from clean `build/ dist/ release/`, running
the three build scripts, `check-provenance.sh`, both smokes, and packaging.
Then CC checks static invariants: no active patch application in maintained
build scripts or cpddl/bliss recipes, no bridge patch files in active input
paths, `pandaPI/` clean after builds, and provenance patch fields set to
`none`. CDC reproduces the same commands where possible and independently
checks the patch-dissolution commit provenance.

## 5. Exit criteria

The ledger's 12 rows reach final status. Summary shape: every bridge patch is
ordinary source history with provenance, build scripts no longer patch
planner source, durable bliss source builds in-tree, provenance reports no
build-time patches, the standing behavior/package gate still passes, and
slice04 is left with provenance/licensing/release composition rather than
hidden patch-bridge cleanup.

## 6. Implementation notes

The preferred bliss representation is:

```text
pandaPI/pandaPIgrounder/cpddl/third-party/
  bliss-0.73.zip
  bliss-0.73-memleak.patch        # may remain only as historical source data
  bliss/                          # durable source used by builds
```

If CC removes the archive or memleak patch, the closing report must explain
why that is still within slice03 and how slice04 will recover license and
provenance evidence. The safer default is to keep archive/provenance inputs
until slice04, but make sure no active build recipe consumes them.

Expected dissolved bridge patch names for commit provenance:

- `0002-makefile.patch`
- `0001-boruvka-endian.patch`
- `0001-Removed-non-macos-call-in-unused-function.patch`
- `bliss-0.73-cxx11-string-literal.patch`

`bliss-0.73-memleak.patch` is cpddl's own bundled third-party patch, not a
chengdu bridge patch, but its contents must also be reflected in the durable
bliss source if the build no longer applies it dynamically.
