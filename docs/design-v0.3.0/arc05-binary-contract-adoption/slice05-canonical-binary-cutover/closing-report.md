# Slice05 closing report - canonical-binary-cutover

## Capability Verdict

**Not delivered; validly deferred.** Slice05 opened the correct policy target:
parser and grounder should not ship 0.3.0 as inherited-name compatibility
commands. CC then hit the slice prompt's explicit stop condition before making
implementation edits: current managed parser and grounder behavior is provided
by transition Bash adapters plus `.legacy` sidecars, not by native canonical
artifacts.

This close records the discovery and defers the cutover work into smaller
native-component slices. It must not be read as evidence that parser or
grounder shims were removed.

## Implementation Decisions

- No implementation changes were made after the Slice05 open-set commit
  `2f638d7f`.
- The raw inherited parser and grounder binaries do not accept managed-process
  contract options such as `--status=stderr`.
- Removing the adapters now would regress the accepted parser and grounder
  managed contract fixtures.
- Arc05 is amended to split the remediation by component:
  - Slice06 implements parser native managed-contract cutover.
  - A planned following slice implements grounder native managed-contract
    cutover.
  - Engine adoption moves after those remediation slices.

## Verification

CDC reproduced the blocker with the current distribution artifacts:

```text
dist/macos-arm64/pandaPIparser.legacy --status=stderr
=> dist/macos-arm64/pandaPIparser.legacy: unrecognized option `--status=stderr'

dist/macos-arm64/pandaPIgrounder.legacy --status=stderr
=> dist/macos-arm64/pandaPIgrounder.legacy: unrecognized option `--status=stderr'
```

CDC also confirmed `dist/macos-arm64/` currently contains canonical adapter
artifacts and inherited sidecars:

```text
pandapi-parser
pandapi-grounder
pandaPIparser.legacy
pandaPIgrounder.legacy
```

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | Open set committed at `2f638d7f`; CDC reran the open-set grep. |
| F-2 | deferred | Parser still installs old-name and `.legacy` artifacts; raw sidecar rejects `--status=stderr`. Re-entry: Slice06. |
| F-3 | deferred | Grounder still installs old-name and `.legacy` artifacts; raw sidecar rejects `--status=stderr`. Re-entry: planned grounder native cutover slice. |
| F-4 | deferred | Parser and grounder shim scripts remain necessary for current managed behavior. Re-entry: parser in Slice06, grounder in the planned grounder slice. |
| F-5 | deferred | Runners still reflect the transition state backed by adapters and sidecars. |
| F-6 | deferred | Fixture records still document transition behavior. |
| F-7 | deferred | Make targets still install/invoke transition parser and grounder shims. |
| F-8 | deferred | README still teaches inherited parser/grounder command examples. |
| F-9 | deferred | Contract fixtures can pass today only through transition adapters; not valid cutover evidence. |
| F-10 | deferred | `make check` was not rerun as cutover proof because cutover did not occur. |
| F-11 | deferred | Package dry-run would still see old parser/grounder artifacts; rerun after native cutovers. |
| F-12 | no-op | CC made no implementation edits; CDC creation of `cdc-verification.md` is outside the CC boundary this row protects. |
| F-13 | no-op | No implementation diff landed for this slice after the open-set commit. |
| F-14 | done | `git diff --check` and `git diff --cached --check` passed during close. |
| F-15 | done | This administrative closing report records the capability verdict, decisions, verification, ledger walk, silent-drop check, and bubble-up. |

Rows: 15. Done: 3. Deferred: 10. No-op: 2.

## Silent-Drop Check

Scope-as-specified was a full parser/grounder no-shim cutover. Scope delivered
was discovery only: the slice proved that the cutover cannot honestly be done
by deleting adapters because native parser and grounder contract behavior does
not yet exist.

The missing implementation work is not silently dropped. It is routed into
Slice06 for parser and a planned following slice for grounder.

## Bubble-up to Arc05

Slice05 did not deliver the assigned Arc05 cutover piece. It revealed a
missing prerequisite in the arc plan: accepted parser/grounder managed behavior
was still implemented in Bash adapters. Arc05 must split the work before
engine adoption:

- first remove the parser transition adapter by making `pandapi-parser` a
  native managed-contract artifact;
- then remove the grounder transition adapter by making `pandapi-grounder` a
  native managed-contract artifact;
- only after both component cutovers should Arc05 continue to engine adoption
  and final binary-contract synthesis.

The arc plan and project plan receive version-history updates for this
resequencing before Slice06 opens.
