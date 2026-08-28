# CDC Verification: Arc09 blocker-stdio-contract-design

Date: 2026-08-20

Verdict: CDC verified; design blocker closed.

Reviewed commits:

- `130100ee` - `Close Arc09 stdio contract design blocker`
- `e262b455` - `Add Arc09 stdio design closing report`

## Summary

CDC independently reviewed the accepted stdin contract design, the closed
ledger, the Arc09/project status updates, and the corrective close-set
`closing-report.md`. The blocker delivers its assigned design-only piece:
Chengdu now has an accepted contract for supported and rejected stdin input
forms before implementation begins.

The accepted design is:

- `pandapi-parser` supports exactly one stdin input role at a time: domain
  from stdin or problem from stdin.
- `pandapi-parser - -` is rejected for 0.3.0 because no accepted framing
  exists for two HDDL documents on one stdin stream.
- `pandapi-grounder` supports one complete parser-generated `.htn` artifact
  on stdin.
- `pandapi-engine` supports one complete grounder-generated `.sas` artifact
  on stdin.
- Engine stdin behavior must preserve valid no-plan classification:
  `domain_no_plan`, exit `2`, `outcome=no_plan`.
- `--output -` plus `--status=stderr` remains the supervised
  stdout/stderr/PANDAPI_STATUS shape.
- Shared runtime/helper code must own repeated stdin materialization,
  path-role behavior, cleanup, and common status mapping.

No product implementation was expected from this slice, and none was found in
the reviewed commits.

## Reproduced Ledger Rows

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | `rg -n -- "blocks Slice01 release-readiness-inventory" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design/slice-doc.md docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` found the blocker marker in both files. |
| F-2 | reproduced | `rg -n -- "domain from stdin|problem from stdin|both.*stdin|framing|rejected" ...` found parser domain-stdin, problem-stdin, rejected both-stdin, and framing rationale in `stdin-contract-design.md`. |
| F-3 | reproduced | `rg -n -- "grounder.*stdin|\\.htn|parser-generated" ...` found grounder stdin semantics for a parser-generated `.htn` artifact. |
| F-4 | reproduced | `rg -n -- "engine.*stdin|\\.sas|domain_no_plan|no_plan" ...` found engine stdin semantics, `.sas` input, and no-plan preservation. |
| F-5 | reproduced | `rg -n -- "--output -|--status=stderr|stdout ownership|stderr|PANDAPI_STATUS" ...` found stdout artifact ownership, stderr status ownership, final `PANDAPI_STATUS`, and the stdout status conflict. |
| F-6 | reproduced | `rg -n -- "shared runtime|stdin materialization|helper|reuse|no-share rationale" ...` found the shared runtime/helper ownership decision and handoff. |
| F-7 | reproduced | Commit-scope boundary check over `e262b455` found no product code, public docs, Make, tooling, CI, release, package, or wolong changes. Commit `130100ee` changed only Arc09/project planning and evidence files. |
| F-8 | reproduced | Before this CDC file was created, `test ! -f .../cdc-verification.md` passed. |
| F-9 | reproduced | `git diff --check && git diff --cached --check` passed before CDC verification was added. |

## Additional Verification

Ran:

```bash
make safety-checks
make actionlint
git diff --check
git diff --cached --check
```

Results:

- repository safety checks passed;
- workflow entrypoints still use Make;
- actionlint passed;
- whitespace checks passed.

The reviewed commits include the required co-author trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```

## Bubble-Up Check

The slice delivered the piece assigned by Arc09: it made the stdin contract
decision without product implementation. The Arc09 plan was updated to mark
`blocker-stdio-contract-design` closed while keeping
`slice01-release-readiness-inventory` blocked.

The remaining blocker chain is correctly preserved:

1. `blocker-stdin-artifact-io` implements the accepted stdin forms through
   shared runtime/helper code.
2. `blocker-stdio-contract-fixtures` proves the behavior with Make-backed
   component and supervised pipeline fixtures and current public contract docs.
3. `blocker-wolong-supervision-proof` verifies wolong can consume the
   supported contract before Slice01 resumes.

No Arc09 release-readiness or publication work should resume from this
verification alone.

## Silent-Drop Check

CDC found no silent drop in this design slice. The slice intentionally did not
deliver implementation, fixtures, public docs, package changes, publication
behavior, or wolong changes; those remain assigned to the follow-on blocker
slices. The parser `- -` form is explicitly rejected rather than silently
left ambiguous.

## Final Decision

CDC verifies `blocker-stdio-contract-design` as closed. Arc09 may proceed to
`blocker-stdin-artifact-io`, and Slice01 release-readiness-inventory remains
blocked until the remaining wolong stdin blockers close.
