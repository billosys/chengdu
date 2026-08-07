# chengdu arc01 - vendor-identity - arc plan

> Plan-of-record for arc01, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md).

## 1. Capability statement

Roadmap line: *pandaPI (and former submodules) live in-tree with full
history, identity-verified against upstream at the pins, built and gated by
CI from in-tree sources only - behaviorally 0.1.0-equivalent by the same
gates.*

Expanded: this arc changes chengdu from a build harness that fetches planner
source into a classic in-tree source distribution. At arc close, the three
pandaPI components and the grounder's former submodules live under
`pandaPI/`, imported with full upstream history and verified against the
same pins that 0.1.0 built from. The build and release workflows consume the
in-tree source only; no workflow step clones `panda-planner-dev/*`,
`gitlab.com/danfis/cpddl`, or the h2 preprocessor source at build time. The
0.1.0 behavioral gate remains the oracle: parser, grounder, engine,
conversion, verifier, negative fixtures, provenance checks, package shape,
and wolong's frozen fetch spec all still pass before any upstream PR or issue
fix is absorbed in arc02.

The sequencing rule for the arc is strict: **identity import, build repoint,
compatibility patch dissolution, provenance/licensing closure**. "Move the
code" and "change the code" do not share a commit.

## 2. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `subtree-import` | Import pandaPIparser, pandaPIgrounder, pandaPIengine, and the grounder's former submodules (`cpddl`, `h2-fd-preprocessor`) under `pandaPI/` with `git subtree add` and no squash; verify every imported tree against the 0.1.0 pins/submodule SHAs before any source-content edits. | slice02, slice03, project ledger P1 |
| slice02 | `in-tree-build` | Repoint local scripts and GitHub workflows so builds use `pandaPI/` as their source root and perform no network source fetch at build time, while preserving the 0.1.0 patch-application behavior as a compatibility bridge. | slice03, slice04, arc02 |
| slice03 | `patch-dissolution` | Convert the 0.1.0 compatibility patches from build-time patch files into ordinary in-tree commits with provenance in commit messages; retire `patches/` as an active build input and simplify scripts accordingly. | slice04, arc02 |
| slice04 | `provenance-notice-gate` | Re-anchor provenance and release manifests on the chengdu commit plus component import commits, add NOTICE/license delineation for every vendored source subtree, update docs, and demonstrate the full two-platform no-fetch CI/release gate. | arc close, arc02 |

Sizing judgment: slice01 is a history/import slice, intentionally narrow
despite touching a large tree; the work is mostly ordered git operations plus
identity checks. slice02 is script/workflow repointing without source edits.
slice03 is the first source-content change slice, and it is separate so every
patch-derived commit can be reviewed as a normal fork delta. slice04 is the
composition and public-contract slice: provenance, NOTICEs, license audit
revision, release-manifest shape, README, and CI evidence all meet there.

## 3. Vendoring layout decision

The arc adopts this in-tree layout:

```text
pandaPI/
  pandaPIparser/
  pandaPIgrounder/
    cpddl/
      third-party/
        boruvka/
        opts/
    h2-fd-preprocessor/
  pandaPIengine/
```

The former submodules stay at the same paths the grounder already expects
inside `pandaPIgrounder/`. That preserves upstream build assumptions and
keeps the final monorepo legible: the grounder's source and its build-time
source dependencies live in the same local shape as the upstream checkout,
but without gitlinks or build-time submodule fetches.

Expected import identities at slice01 open:

| Component | Import prefix | Source URL | SHA |
|-----------|---------------|------------|-----|
| pandaPIparser | `pandaPI/pandaPIparser` | `https://github.com/panda-planner-dev/pandaPIparser.git` | `88c0995c72c5ce2685e9546ec153a3545977ed81` |
| pandaPIgrounder | `pandaPI/pandaPIgrounder` | `https://github.com/panda-planner-dev/pandaPIgrounder.git` | `b04ff7bcca710ae80a1ce73b8c027f0224565afc` |
| pandaPIengine | `pandaPI/pandaPIengine` | `https://github.com/panda-planner-dev/pandaPIengine.git` | `810f04388667db5e3e4f114e960a4efbb43b1ac0` |
| cpddl | `pandaPI/pandaPIgrounder/cpddl` | `https://gitlab.com/danfis/cpddl.git` | `8c142bab211200213746c75be71810526e834d4a` |
| boruvka | `pandaPI/pandaPIgrounder/cpddl/third-party/boruvka` | `https://gitlab.com/danfis/boruvka.git` | `e320cc7b02714e4096a686965fab343735d82bec` |
| opts | `pandaPI/pandaPIgrounder/cpddl/third-party/opts` | `https://github.com/danfis/opts.git` | `819d90e8664f9deda56f0dd8e7b62ea3ab9b2f6d` |
| h2-fd-preprocessor | `pandaPI/pandaPIgrounder/h2-fd-preprocessor` | `https://github.com/galvusdamor/h2-fd-preprocessor.git` | `e335433bfc53171a1c215bd49ba39cc5fb1d485b` |

Note on h2 provenance: the 0.2.0 project plan describes the former
submodules collectively as danfis-origin dependencies. The grounder's pinned
`.gitmodules` records h2-fd-preprocessor at
`github.com/galvusdamor/h2-fd-preprocessor.git`; slice01 treats the pinned
`.gitmodules` as the identity source and records the exact URL above.

## 4. Dependencies

**Consumes:** `docs/design-v0.2.0/project-plan.md`; `CLAUDE.md` fork policy;
`pins.env` as the historical import-point record; the existing 0.1.0 scripts,
workflows, fixtures, release packaging, and license audit as the behavioral
oracle.

**Leaves for arc02:** a CI-gated in-tree fork baseline. Arc02 starts from
vendored source whose import identity is verified, whose compatibility
patches are ordinary fork commits, and whose release machinery still produces
wolong-compatible assets.

## 5. Open questions and risks

- **OQ1 - RESOLVED 2026-08-07 (v1.1): import mechanics for nested former
  submodules.** The grounder import initially contained gitlinks at `cpddl`
  and `h2-fd-preprocessor`; slice01 verified those gitlinks and replaced them
  with subtree imports at the same paths. Slice01 also found cpddl's own
  gitlinks at `third-party/boruvka` and `third-party/opts`; those are now
  imported as nested subtrees and explicitly tracked for slice04
  provenance/licensing work.
- **OQ2 - RESOLVED 2026-08-07 (v1.2): scratch build versus direct in-tree
  mutation during the compatibility bridge.** Slice02 builds from disposable
  copies under an ignored `build/` tree. The active source of truth is
  `pandaPI/`, but patch application and object files happen only in the build
  copy until slice03 dissolves the patches into ordinary in-tree commits.
- **OQ3 (slice03): bliss patch representation.** The 0.1.0 bliss fix applies
  to a zip-extracted third-party source during build. Slice03 must decide the
  durable fork representation before retiring `patches/`: extracted vendored
  source, an in-tree replacement archive, or another explicitly documented
  build-normalization commit. Re-entry: slice03 plan.
- **OQ4 (slice04): license-audit revision boundary.** `license-audit-v0.2.0.md`
  must distinguish identity-import licensing from later arc02 source changes.
  Include the slice01-surfaced `boruvka` and `opts` nested subtrees in NOTICE
  and license-delineation planning. Re-entry: slice04 plan.

## 6. Arc ledger

Composition rows verifying the capability; open here, close (per-row walk)
in this arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | All four slices close with no silent drops, and their close reports point to CDC verification. | attested |
| A2 | Every vendored source subtree has full upstream history reachable from chengdu history, and the imported tree at its import point is identity-equivalent to the pinned upstream tree. | reproduced |
| A3 | A full GitHub Actions build/release gate consumes only in-tree planner source: workflow logs show no clone/fetch of panda-planner-dev, gitlab.com/danfis/cpddl, or h2-fd-preprocessor source during build. | reproduced |
| A4 | The in-tree baseline is behaviorally equivalent to 0.1.0 by the standing gate sequence: positive smoke, negative smoke, provenance check, per-platform artifact `--help`, release packaging, and wolong's frozen asset shape. | reproduced |
| A5 | `patches/` no longer participates in the active build; each 0.1.0 compatibility delta is represented as a normal in-tree commit with provenance. | reproduced |
| A6 | Vendoring provenance and licensing are explicit: per-subtree NOTICE files exist, `license-audit-v0.2.0.md` is landed, `THIRD-PARTY-LICENSES` is regenerated or confirmed current, and release manifests name the chengdu commit plus component import identities. | reproduced |

## 7. Version history

- **v1.2 - 2026-08-07.** Resolved OQ2 for slice02: builds will consume
  in-tree `pandaPI/` by copying sources into an ignored disposable `build/`
  tree, keeping patch-bridge mutations and object files out of the vendored
  source tree. Surfaced by: slice02 planning. Why: direct patch application in
  `pandaPI/` would dirty imported source before slice03's patch-dissolution
  commits.
- **v1.1 - 2026-08-07.** Updated the vendoring layout and import identity
  table to include cpddl's slice01-surfaced nested gitlinks:
  `third-party/boruvka` and `third-party/opts`. Marked OQ1 resolved and
  amended OQ4 so slice04 explicitly carries the two nested subtrees into
  NOTICE/license planning. Surfaced by: slice01 subtree-import close. Why:
  CDC verification confirmed the no-gitlinks invariant required these extra
  imports; leaving them implicit would risk a slice04 licensing/provenance
  silent drop.
- **v1.0 - 2026-08-07.** Initial arc breakdown. Source: 0.2.0 project plan
  v1.0 and the operator's instruction to open arc01. No slice bubble-ups yet.
