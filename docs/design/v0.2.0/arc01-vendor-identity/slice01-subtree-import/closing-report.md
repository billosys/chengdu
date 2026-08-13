# Slice 01 closing report - subtree-import

Closed by CC on 2026-08-07. Verifier: pending CDC.

## Ledger walk

**F-1 - done.** Slice start was clean on `release/0.2.x` at `0474f383c61815a35a94f0413ac6b6a72ec7feaa`; `git status --short` was empty and `test ! -e pandaPI` exited 0.

**F-2 - done.** `pandaPI/pandaPIparser` was imported unsquashed from `https://github.com/panda-planner-dev/pandaPIparser.git` at `88c0995c72c5ce2685e9546ec153a3545977ed81`; import commit `ac8e385d`. Ancestor check passed and normalized tree diff was empty at `/tmp/chengdu-parser.tree.diff`.

**F-3 - done.** `pandaPI/pandaPIengine` was imported unsquashed from `https://github.com/panda-planner-dev/pandaPIengine.git` at `810f04388667db5e3e4f114e960a4efbb43b1ac0`; import commit `f734b692`. Ancestor check passed and normalized tree diff was empty at `/tmp/chengdu-engine.tree.diff`.

**F-4 - done.** `pandaPI/pandaPIgrounder` was imported unsquashed from `https://github.com/panda-planner-dev/pandaPIgrounder.git` at `b04ff7bcca710ae80a1ce73b8c027f0224565afc`; import commit `2b0a6093`. Ancestor check passed and the raw import tree matched upstream at `/tmp/chengdu-grounder-import.tree.diff`.

**F-5 - done.** At `2b0a6093:pandaPI/pandaPIgrounder`, `cpddl` was `160000 commit 8c142bab211200213746c75be71810526e834d4a` and `h2-fd-preprocessor` was `160000 commit e335433bfc53171a1c215bd49ba39cc5fb1d485b`.

**F-6 - done.** `pandaPI/pandaPIgrounder/cpddl` was imported unsquashed from `https://gitlab.com/danfis/cpddl.git` at `8c142bab211200213746c75be71810526e834d4a`; import commit `25a278a5`, after gitlink removal commit `8d24ece8`. `test -d pandaPI/pandaPIgrounder/cpddl/src` exited 0, ancestor check passed, and raw import tree diff was empty at `/tmp/chengdu-cpddl-import.tree.diff`.

**F-7 - done.** `pandaPI/pandaPIgrounder/h2-fd-preprocessor` was imported unsquashed from `https://github.com/galvusdamor/h2-fd-preprocessor.git` at `e335433bfc53171a1c215bd49ba39cc5fb1d485b`; import commit `b27e102c`, after gitlink removal commit `8d24ece8`. `test -d pandaPI/pandaPIgrounder/h2-fd-preprocessor/src` exited 0, ancestor check passed, and normalized tree diff was empty at `/tmp/chengdu-h2-fd-preprocessor.tree.diff`.

**F-8 - done.** Final `pandaPI/` has no nested `.git` directories and no gitlinks: `find pandaPI -name .git -print` and `git ls-files -s pandaPI | awk '$1 == "160000" { print }'` both returned empty at `68072cb3`. During this check, cpddl's own gitlinks were found and replaced with unsquashed subtree imports: `boruvka` from `https://gitlab.com/danfis/boruvka.git` at `e320cc7b02714e4096a686965fab343735d82bec`, import `1f0ac094`; `opts` from `https://github.com/danfis/opts.git` at `819d90e8664f9deda56f0dd8e7b62ea3ab9b2f6d`, import `68072cb3`.

**F-9 - done.** Existing 0.1.0 build/release inputs were untouched: `git diff --name-only 0474f383c61815a35a94f0413ac6b6a72ec7feaa..HEAD -- scripts .github pins.env patches fixtures README.md licenses docs/license-audit-v0.1.0.md` returned empty at `68072cb3`.

**F-10 - done.** Rows F-2 through F-7 record the five assigned component URLs, SHAs, import commits, and tree-identity checks. F-8 additionally records the two nested cpddl imports required to satisfy the no-gitlinks invariant. CDC can reproduce the checks with the commands named in the ledger.

**F-11 - done.** The committed diff from `0474f383c61815a35a94f0413ac6b6a72ec7feaa..68072cb3` was reviewed with `git status --short` for the close artifacts. The slice introduced `pandaPI/**`, updated this slice's `ledger.md`, and added this `closing-report.md`; no files outside `pandaPI/` and the slice close artifacts were introduced.

## Bubble-up to the arc

**Delivered as assigned.** Yes. Against arc-plan slice01, the parser, grounder, engine, `cpddl`, and `h2-fd-preprocessor` now live under `pandaPI/` as in-tree source with upstream history reachable and import identity checked before source edits.

**What the arc-plan did not anticipate.** `cpddl` itself contains active gitlinks at `third-party/boruvka` and `third-party/opts` at the pinned cpddl SHA. The no-gitlinks invariant required importing those as additional nested subtrees. Arc01 should carry this forward into slice04 licensing/provenance work: `boruvka` and `opts` need the same NOTICE/license treatment as the explicitly planned vendored source subtrees.

**Silent-drop diff.** Scope-as-specified was the five planned source imports, grounder gitlink verification/replacement, no source edits, untouched 0.1.0 build path, no gitlinks or nested `.git`, and close evidence. Scope-as-delivered includes all of that, plus the two necessary nested cpddl source imports. Nothing from the assigned scope is missing. No silent drops.
