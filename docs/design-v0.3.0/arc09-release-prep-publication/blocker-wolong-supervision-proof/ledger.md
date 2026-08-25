# Arc09 Blocker Ledger: wolong-supervision-proof

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Chengdu stdin contract fixture gate passes. | `make test-contract-stdio-managed` | serious | prior blocker slices | closed | `SUMMARY: 187 passed, 0 failed (contract, stdio, macos-arm64)`. | |
| F-2 | The original wolong blocker report is re-read and each claim is mapped to fixed, accepted-as-rejected, or still-blocked. | `test -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md && grep -nE "chengdu-stdin-contract-blocker|fixed|accepted-as-rejected|still-blocked" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | serious | wolong report | closed | `wolong-supervision-proof.md` maps every original claim to `fixed`, `accepted-as-rejected`, or `still-blocked`. | |
| F-3 | Direct local Chengdu probes prove parser, grounder, engine, solved pipeline, and no-plan pipeline stdin behavior from `./bin/pandapi-*`. | `grep -nE "direct Chengdu probe|pandapi-parser|pandapi-grounder|pandapi-engine|stdin|domain_no_plan|no_plan|PANDAPI_STATUS" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | correctness-grade | wolong acceptance | closed | Direct probes passed for parser domain stdin, parser problem stdin, parser both-stdin rejection, grounder stdin, engine solved/no-plan stdin, and solved/no-plan chains. | |
| F-4 | Wolong erlexec/argv-list verification either passes or records a concrete external blocker with re-entry condition. | `grep -nE "erlexec|argv-list|wolong-exec|resume|slice02-stdio-runner|re-entry|external blocker" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | correctness-grade | release acceptance anchor | closed | Raw erlexec argv-list stdin proof passed; `wolong-exec:run/3` still lacks stdin bytes/EOF, so the external blocker is Wolong Arc03 `slice02-stdio-runner`. | |
| F-5 | Any Wolong workspace edits, if needed, are scoped to proof fixtures/docs and are committed or explicitly left to a Wolong slice. | `grep -nE "Wolong workspace|no Wolong edits|Wolong commit|Wolong slice|external blocker" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | serious | cross-repo boundary | closed | Wolong workspace remained clean; no Wolong edits or commit were made; implementation is left to a Wolong slice. | |
| F-6 | Arc09 Slice01 status is updated to resumed or explicitly still blocked. | `grep -nE "Slice01 release-readiness-inventory.*blocked|Slice01 release-readiness-inventory.*resume|wolong supervision proof|wolong stdin|slice02-stdio-runner" docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md docs/design-v0.3.0/project-plan.md` | serious | release planning | closed | Arc09 and project plans keep Slice01 blocked by the Wolong Arc03 `slice02-stdio-runner` external blocker. | |
| F-7 | Chengdu's existing managed contract and smoke gates still pass. | `make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed && make smoke && make smoke-negative` | serious | no regression | closed | Parser `298 passed`, grounder `269 passed`, engine `312 passed`, pipeline `129 passed`, positive smoke `3 passed`, negative smoke `4 passed`. | |
| F-8 | Release-quality wayfinding still passes. | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check` | serious | release gate runway | closed | Safety checks, actionlint, static analysis, format check, full `make test`, and provenance check passed. | |
| F-9 | The slice keeps release publication out of scope. | `set -e; names=$(git diff --cached --name-only -- release .github/release-notes.md.in); test -z "$names"` | serious | boundary | closed | No release output or release notes files are staged by this slice. | |
| F-10 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/cdc-verification.md` | serious | independence | closed | No `cdc-verification.md` exists for this slice. | |
| F-11 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | closed | Passed after staging. | |

## What Worked

Chengdu stdin behavior is fixed and fixture-proven. Direct shell probes match
the accepted contract, and raw Wolong erlexec argv-list stdin supervision
passes against a real Chengdu binary. The remaining release blocker is not a
Chengdu stdin defect; it is the lack of stdin bytes/EOF support in Wolong's
current `wolong-exec:run/3` public runner/API.

## Closure

Closed as a proof and routing slice. Arc09 Slice01
`release-readiness-inventory` remains blocked until Wolong Arc03
`slice02-stdio-runner`, or an equivalent Wolong-owned implementation slice,
adds and verifies stdin artifact delivery through the Wolong runner/API.
