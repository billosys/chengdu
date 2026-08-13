# chengdu arc01 / slice01 - subtree-import - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Establish the in-tree pandaPI source baseline without source-content edits.
At slice close, chengdu history contains full-history subtree imports for
pandaPIparser, pandaPIgrounder, pandaPIengine, cpddl, and h2-fd-preprocessor
under `pandaPI/`, with each imported tree identity-verified against the
0.1.0 pin or the grounder's pinned submodule SHA. The current build may still
use the 0.1.0 fetch path until slice02; this slice is only the identity
foundation.

## 2. Scope

**In:**

- Create `pandaPI/` with these prefixes:
  `pandaPI/pandaPIparser/`, `pandaPI/pandaPIgrounder/`,
  `pandaPI/pandaPIengine/`,
  `pandaPI/pandaPIgrounder/cpddl/`, and
  `pandaPI/pandaPIgrounder/h2-fd-preprocessor/`.
- Import each source with `git subtree add --prefix=<prefix> <repo> <sha>`
  and **no `--squash`**.
- Preserve the grounder's import identity before de-submoduling: verify that
  the imported grounder tree records gitlinks for `cpddl` and
  `h2-fd-preprocessor` at the expected SHAs, then replace those gitlinks with
  subtree imports at the same paths.
- Record enough import evidence in the ledger for a fresh verifier to
  reproduce identity: source URL, SHA, import commit, and normalized tree
  comparison for each prefix.
- Leave the existing 0.1.0 build scripts, workflows, `pins.env`, `patches/`,
  release machinery, fixtures, and docs behaviorally untouched except for
  any minimal documentation note required by the ledger.

**Out (disclosed, not dropped):**

- Repointing scripts/workflows to build from `pandaPI/` - slice02.
- Retiring `retired upstream-fetch helper` or changing `pins.env` semantics -
  slice02/slice04.
- Converting build-time patches into in-tree commits and retiring `patches/`
  - slice03.
- NOTICE files, `license-audit-v0.2.0.md`, release-manifest changes, and
  `THIRD-PARTY-LICENSES` regeneration - slice04.
- Absorbing upstream PRs or fixing open upstream issues - arc02.

## 3. Constraints

- No source-content edits in imported trees during this slice. If a file under
  `pandaPI/` differs from its pinned upstream tree, the difference must be the
  intentional replacement of a gitlink with the matching subtree import, not a
  source change.
- No squash imports. Full upstream history must be reachable from chengdu
  history for every imported component.
- Do not run destructive git cleanup. If a subtree import goes wrong, stop and
  report the exact state rather than rewriting history on this branch.
- Use HTTPS remotes for reproducibility unless an operator-approved local
  remote is already present.
- Keep this slice's diff limited to git history imports plus the slice's own
  ledger evidence updates. Build changes wait for slice02.

## 4. Verification approach

CC performs the subtree imports and records `attested` evidence row by row.
CDC verifies by re-reading the import commits, checking history reachability,
and reproducing normalized tree comparisons against the pinned upstream
checkouts or freshly fetched upstream trees. The important distinction is
between the upstream import point and the final de-submoduled layout: the
grounder import commit must match upstream including gitlinks, while the final
tree must contain real source directories at those former gitlink paths.

## 5. Exit criteria

The ledger's rows reach final status with evidence: clean starting state,
five full-history subtree imports, grounder gitlink identities verified,
former submodules replaced by source directories at the same paths, import
trees normalized-diff clean against pins, existing 0.1.0 build path still
untouched, and no unplanned files or source edits.

## 6. Import identity table

| Component | Prefix | Source URL | SHA |
|-----------|--------|------------|-----|
| pandaPIparser | `pandaPI/pandaPIparser` | `https://github.com/panda-planner-dev/pandaPIparser.git` | `88c0995c72c5ce2685e9546ec153a3545977ed81` |
| pandaPIgrounder | `pandaPI/pandaPIgrounder` | `https://github.com/panda-planner-dev/pandaPIgrounder.git` | `b04ff7bcca710ae80a1ce73b8c027f0224565afc` |
| pandaPIengine | `pandaPI/pandaPIengine` | `https://github.com/panda-planner-dev/pandaPIengine.git` | `810f04388667db5e3e4f114e960a4efbb43b1ac0` |
| cpddl | `pandaPI/pandaPIgrounder/cpddl` | `https://gitlab.com/danfis/cpddl.git` | `8c142bab211200213746c75be71810526e834d4a` |
| h2-fd-preprocessor | `pandaPI/pandaPIgrounder/h2-fd-preprocessor` | `https://github.com/galvusdamor/h2-fd-preprocessor.git` | `e335433bfc53171a1c215bd49ba39cc5fb1d485b` |

## 7. Suggested normalized tree check

The exact verifier script may vary, but it must compare file modes, object
IDs, and paths after normalizing away the destination prefix. One acceptable
shape:

```bash
git ls-tree -r <upstream-sha> \
  | sort > /tmp/chengdu-upstream.tree
git ls-tree -r HEAD:pandaPI/pandaPIparser \
  | sed 's#	pandaPI/pandaPIparser/#	#' \
  | sort > /tmp/chengdu-import.tree
diff -u /tmp/chengdu-upstream.tree /tmp/chengdu-import.tree
```

For the grounder, run the comparison twice: once against the grounder import
commit before replacing gitlinks, and once against the final source directory
with `cpddl` and `h2-fd-preprocessor` excluded from the parent comparison and
verified as their own subtree imports.
