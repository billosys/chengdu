# CDC verification - chengdu arc01 / slice01 - subtree-import

Verified by CDC on 2026-08-07, against `cec75597` on `release/0.2.x`.
Role: independent ledger reproduction and bubble-up check, per
`LEDGER-DISCIPLINE.md` Section A and `PROJECT-MANAGEMENT.md` Part IV.

## Verdict

**Accepted.** Rows F-1 through F-11 reproduce or check cleanly. The slice
delivered its assigned piece of arc01 and surfaced one real arc-plan update:
cpddl itself carried two nested gitlinks (`boruvka`, `opts`) that needed
subtree imports and must be carried into slice04 NOTICE/licensing work.

## Row Reproduction

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Current branch is `release/0.2.x`; worktree clean; `git ls-tree -d --name-only 0474f383c61815a35a94f0413ac6b6a72ec7feaa pandaPI` returned empty. |
| F-2 | reproduced | `88c0995c72c5ce2685e9546ec153a3545977ed81` is an ancestor of `HEAD`; `diff -u <(git ls-tree -r 88c0995... \| sort) <(git ls-tree -r HEAD:pandaPI/pandaPIparser \| sort)` returned empty. |
| F-3 | reproduced | `810f04388667db5e3e4f114e960a4efbb43b1ac0` is an ancestor of `HEAD`; normalized tree diff against `HEAD:pandaPI/pandaPIengine` returned empty. |
| F-4 | reproduced | `b04ff7bcca710ae80a1ce73b8c027f0224565afc` is an ancestor of `HEAD`; normalized tree diff against `2b0a6093:pandaPI/pandaPIgrounder` returned empty. |
| F-5 | reproduced | `git ls-tree 2b0a6093:pandaPI/pandaPIgrounder cpddl h2-fd-preprocessor` returned the expected `160000` entries for `8c142bab211200213746c75be71810526e834d4a` and `e335433bfc53171a1c215bd49ba39cc5fb1d485b`. |
| F-6 | reproduced | `8c142bab211200213746c75be71810526e834d4a` is an ancestor of `HEAD`; normalized tree diff against `25a278a5:pandaPI/pandaPIgrounder/cpddl` returned empty. |
| F-7 | reproduced | `e335433bfc53171a1c215bd49ba39cc5fb1d485b` is an ancestor of `HEAD`; normalized tree diff against `HEAD:pandaPI/pandaPIgrounder/h2-fd-preprocessor` returned empty. |
| F-8 | reproduced | `find pandaPI -name .git -print` returned empty; `git ls-files -s pandaPI \| awk '$1 == "160000" { print }'` returned empty. Additional reproduction: `e320cc7b02714e4096a686965fab343735d82bec` and `819d90e8664f9deda56f0dd8e7b62ea3ab9b2f6d` are ancestors of `HEAD`, and normalized tree diffs for `boruvka` and `opts` returned empty. |
| F-9 | reproduced | `git diff --name-only 0474f383c61815a35a94f0413ac6b6a72ec7feaa..HEAD -- scripts .github pins.env patches fixtures README.md licenses docs/license-audit-v0.1.0.md` returned empty. |
| F-10 | reproduced | Ledger rows F-2 through F-8 and the closing report record URL/SHA/import identity for the five planned imports and the two discovered nested imports. |
| F-11 | reproduced | `git diff --name-only 0474f383c61815a35a94f0413ac6b6a72ec7feaa..HEAD` introduced only `pandaPI/**`, this slice's `ledger.md`, and this slice's `closing-report.md`. |

## Additional Checks

- `git status --short` after CDC reproduction: empty.
- `git diff --check 0474f383c61815a35a94f0413ac6b6a72ec7feaa..HEAD` was
  intentionally not used as a pass/fail gate: it reports upstream whitespace
  in the identity-imported source. Fixing those lines in slice01 would violate
  the no-source-edits scope.

## Bubble-up Check

**Delivered as assigned:** yes. Slice01 imported the planned pandaPI source
baseline with full upstream history and reproduced tree identity before source
edits.

**Arc-plan change required:** yes. The slice revealed two additional vendored
source subtrees (`boruvka`, `opts`) below cpddl. I updated
[`../arc-plan.md`](../arc-plan.md) to include them in the vendoring layout and
identity table, marked OQ1 resolved, amended OQ4, and added version-history
entry v1.1.

**Silent-drop diff:** complete. The delivered scope covers the five planned
imports, grounder gitlink replacement, no nested gitlinks or `.git`
directories, untouched 0.1.0 build/release inputs, and close evidence. The two
extra imports are additive work required by the no-gitlinks invariant, not a
substitution for planned scope.
