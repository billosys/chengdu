# CC assignment - chengdu arc01 / slice03 - patch-dissolution

You are CC, the implementing context for one slice, working in the
`chengdu` repo (`~/lab/billosys/chengdu`) on `release/0.2.x`. This file is
your assignment; read it fully before writing anything.

## Read order (before any code)

1. `/CLAUDE.md` - standing project contracts.
2. `docs/design-v0.2.0/project-plan.md` - project DoD and fork policy.
3. `docs/design-v0.2.0/arc01-vendor-identity/arc-plan.md` - arc capability,
   sequencing rule, and OQ3's durable bliss-source decision.
4. `docs/design-v0.2.0/arc01-vendor-identity/slice02-in-tree-build/cdc-verification.md`
   - the closed in-tree build baseline you are building from.
5. `docs/design-v0.2.0/arc01-vendor-identity/slice03-patch-dissolution/slice-doc.md`
   - goal, scope, constraints, exit criteria.
6. `docs/design-v0.2.0/arc01-vendor-identity/slice03-patch-dissolution/ledger.md`
   - the specification of done. Twelve rows; every one reaches a final
   status before this slice advances. Follow the CC protocol in the
   collaboration framework's `LEDGER-DISCIPLINE.md` Section A.

## The work

Retire the build-time patch bridge by converting it into source history:

- Apply the cpddl makefile compatibility delta in
  `pandaPI/pandaPIgrounder/cpddl/` as normal source/build metadata.
- Apply the two macOS boruvka compatibility deltas directly in
  `pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/`.
- Extract durable bliss source under
  `pandaPI/pandaPIgrounder/cpddl/third-party/bliss/`, using the already
  vendored `bliss-0.73.zip` as the source, then make cpddl's memleak fix and
  chengdu's C++11 string-literal fix normal source changes there.
- Simplify `scripts/build-grounder.sh` so it builds copied in-tree source but
  does not apply patches, track an applied-patch CSV, unzip bliss, or read
  from `patches/`.
- Adjust `scripts/check-provenance.sh` and any packaging expectations so the
  grounder provenance block says `patches=none` while the component SHA still
  matches `GROUNDER_SHA` until slice04 reworks provenance.
- Retire bridge patch files as active inputs. The `patches/` directory should
  no longer be needed; grounder-level `000*.patch` files should not remain as
  build inputs after their deltas are source.
- Update README/source-build wording so it describes the post-dissolution
  state.

## Commit/provenance discipline

Prefer one source commit per logical dissolved patch. If you combine commits,
the combined commit message must still name every dissolved patch. Use clear
trailers, for example:

```text
Dissolves-Patch: 0002-makefile.patch
Reason: remove build-time patching now that pandaPI source is in-tree
```

Expected bridge patch names:

- `0002-makefile.patch`
- `0001-boruvka-endian.patch`
- `0001-Removed-non-macos-call-in-unused-function.patch`
- `bliss-0.73-cxx11-string-literal.patch`

`bliss-0.73-memleak.patch` is cpddl's own bundled third-party patch. Its
contents must be reflected in the durable bliss source if the build no longer
applies it dynamically, but it does not need a `Dissolves-Patch` trailer as a
chengdu bridge patch.

## Constraints

- No planner-source network fetch in active build paths.
- No build-time patch application in active source-build paths.
- Keep disposable build copies; do not compile directly inside `pandaPI/`.
- Keep bash 3.2 compatibility.
- Preserve the 0.1.0 behavior oracle: provenance, positive smoke, negative
  smoke, and package shape.
- Do not do slice04's provenance/licensing/NOTICE rewrite here.
- If a ledger row is wrong or impossible, request an amendment rather than
  quietly weakening the row.

## Protocol

- Work against the ledger; update Evidence with strength `attested` as each
  row lands.
- At close: write `closing-report.md` in this slice directory - per-row walk
  for all 12 rows, then a **Bubble-up to the arc** section answering:
  delivered-as-assigned, what the arc-plan did not anticipate, and the
  silent-drop diff.
- Do not write `cdc-verification.md`; that is CDC's independent file.

## Definition of done

All 12 ledger rows at final status with `attested` evidence, no active
build-time compatibility patches, durable bliss source in-tree, grounder
provenance reporting `patches=none`, standing local gate passing, and slice04
left with provenance/licensing/release composition rather than patch cleanup.
