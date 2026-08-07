# Slice 01: subtree-import

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-07. Closer: CC. Verifier: CDC. This slice is an
> identity/history import slice only; build repointing starts in slice02.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The slice starts from a clean worktree on `release/0.2.x`, and no `pandaPI/` tree already exists. | `git branch --show-current`; `git status --short`; `test ! -e pandaPI` | serious | slice-doc section 3 | open | | |
| F-2 | `pandaPI/pandaPIparser` is imported with full history, no squash, at `88c0995c72c5ce2685e9546ec153a3545977ed81`. | `git merge-base --is-ancestor 88c0995c72c5ce2685e9546ec153a3545977ed81 HEAD`; normalized `git ls-tree` diff against parser SHA is empty | serious | project-plan section 3; arc-plan section 3 | open | | Record the subtree import commit. |
| F-3 | `pandaPI/pandaPIengine` is imported with full history, no squash, at `810f04388667db5e3e4f114e960a4efbb43b1ac0`. | `git merge-base --is-ancestor 810f04388667db5e3e4f114e960a4efbb43b1ac0 HEAD`; normalized `git ls-tree` diff against engine SHA is empty | serious | project-plan section 3; arc-plan section 3 | open | | Record the subtree import commit. |
| F-4 | `pandaPI/pandaPIgrounder` is imported with full history, no squash, at `b04ff7bcca710ae80a1ce73b8c027f0224565afc`, and its import commit matches upstream before de-submoduling. | `git merge-base --is-ancestor b04ff7bcca710ae80a1ce73b8c027f0224565afc HEAD`; normalized tree diff against grounder SHA is empty at the grounder import commit | serious | project-plan section 3; arc-plan section 3; arc-plan OQ1 | open | | The comparison at this stage should still see gitlinks for the former submodules. |
| F-5 | The imported grounder tree records gitlinks for `cpddl` and `h2-fd-preprocessor` at the expected SHAs before those paths are replaced. | `git ls-tree <grounder-import-commit>:pandaPI/pandaPIgrounder cpddl h2-fd-preprocessor` shows `160000 commit 8c142bab211200213746c75be71810526e834d4a` and `160000 commit e335433bfc53171a1c215bd49ba39cc5fb1d485b` | correctness | arc-plan OQ1 | open | | This is the bridge between upstream grounder identity and final in-tree source layout. |
| F-6 | `pandaPI/pandaPIgrounder/cpddl` is imported with full history, no squash, at `8c142bab211200213746c75be71810526e834d4a`, replacing the grounder gitlink at the same path. | `test -d pandaPI/pandaPIgrounder/cpddl/src`; `git merge-base --is-ancestor 8c142bab211200213746c75be71810526e834d4a HEAD`; normalized tree diff against cpddl SHA is empty | serious | project-plan section 3; arc-plan section 3 | open | | Use the grounder's pinned submodule SHA as the identity source. |
| F-7 | `pandaPI/pandaPIgrounder/h2-fd-preprocessor` is imported with full history, no squash, at `e335433bfc53171a1c215bd49ba39cc5fb1d485b`, replacing the grounder gitlink at the same path. | `test -d pandaPI/pandaPIgrounder/h2-fd-preprocessor/src`; `git merge-base --is-ancestor e335433bfc53171a1c215bd49ba39cc5fb1d485b HEAD`; normalized tree diff against h2 SHA is empty | serious | project-plan section 3; arc-plan section 3 | open | | Use the grounder's pinned submodule SHA and `.gitmodules` URL as the identity source. |
| F-8 | The final `pandaPI/` layout has real source directories, not gitlinks or nested `.git` repositories, at every planned prefix. | `find pandaPI -name .git -print` returns nothing; `git ls-files -s pandaPI | awk '$1 == "160000" { print }'` returns nothing | correctness | slice-doc section 2 | open | | Full history is in chengdu history, not nested repos. |
| F-9 | Existing 0.1.0 build/release inputs are not behaviorally changed in this slice: scripts, workflows, `pins.env`, `patches/`, fixtures, and release packaging remain untouched except for unavoidable git metadata/history. | `git diff --name-only <slice-start>..HEAD -- scripts .github pins.env patches fixtures README.md licenses docs/license-audit-v0.1.0.md` returns nothing | correctness | slice-doc section 2 out-list | open | | If a doc note is required, disclose it in the closing report; do not slip build changes into this slice. |
| F-10 | Import evidence is complete enough for CDC reproduction: source URL, SHA, import commit, and tree-identity command/output are recorded for all five imported components. | Inspect this ledger evidence and closing report; count 5 component evidence blocks | serious | LEDGER-DISCIPLINE.md Section A | open | | Evidence can be attached in ledger rows or the closing report, but every row must be reproducible. |
| F-11 | No unplanned files are introduced outside `pandaPI/` and this slice's own open/close artifacts. | `git diff --name-only <slice-start>..HEAD` reviewed against slice-doc section 2 | polish | slice-doc section 3 | open | | The close-set files are expected only at slice close. |

## What Worked

_(At slice close. Patterns that made the slice close cleanly.)_

## Closure

Closed at commit <pending> on <date>. Verified by: <pending>.
Rows: 11. Done: <n>. Deferred: <n>. No-op: <n>.
