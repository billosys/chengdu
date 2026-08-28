# CDC verification - Slice05 canonical-binary-cutover

## Verdict

**Verified as closed with deferrals, not delivered.** CC's stop-condition
report is correct. The current canonical parser and grounder commands are
transition adapters over raw inherited sidecars, and the raw sidecars do not
understand the managed-process contract options.

## Independent Checks

Commands run from `/Users/oubiwann/lab/billosys/chengdu`:

```text
git status --short --branch
=> ## release/0.3.x

git log --oneline -8
=> 2f638d7f Open canonical binary cutover slice
=> 0e5a5da6 build: move harnesses out of scripts
=> 921f9f92 build: inline simple release support scripts

find dist -maxdepth 2 -type f -perm -111 -print | sort
=> dist/macos-arm64/pandaPIengine
=> dist/macos-arm64/pandaPIgrounder
=> dist/macos-arm64/pandaPIgrounder.legacy
=> dist/macos-arm64/pandaPIparser
=> dist/macos-arm64/pandaPIparser.legacy
=> dist/macos-arm64/pandapi-grounder
=> dist/macos-arm64/pandapi-parser

dist/macos-arm64/pandaPIparser.legacy --status=stderr
=> exit 1; unrecognized option `--status=stderr'

dist/macos-arm64/pandaPIgrounder.legacy --status=stderr
=> exit 1; unrecognized option `--status=stderr'
```

CDC also inspected `scripts/pandapi-parser-adapter.sh`,
`scripts/pandapi-grounder-adapter.sh`, `scripts/install-grounder-adapter.sh`,
`mk/build.mk`, and `mk/tests.mk`. The adapters are the current managed-process
behavior layer.

## Ledger Verification

The ledger has 15 rows and every row has a final status:

- Done: F-1, F-14, F-15.
- Deferred: F-2 through F-11.
- No-op: F-12, F-13.

The deferred rows have concrete re-entry conditions: Slice06 for parser native
cutover, a planned following slice for grounder native cutover, and later
reruns of package/check/doc cleanup once both component cutovers exist.

## Bubble-up Check

The closing report's bubble-up is accepted. Arc05 must be updated before the
next slice is planned against it, because Slice05 proved the prior cutover
slice was too large and missing native parser/grounder implementation
prerequisites.

## What Worked

The explicit prompt stop condition did its job: it caught the mismatch between
the policy target and the actual implementation substrate before the adapter
files were deleted.
