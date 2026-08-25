# Arc09 Blocker Ledger: wolong-supervision-proof

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Chengdu stdin contract fixture gate passes. | `make test-contract-stdio-managed` | serious | prior blocker slices | open | | |
| F-2 | The original wolong blocker report is re-read and each claim is mapped to fixed, accepted-as-rejected, or still-blocked. | `test -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md && rg -n -- "chengdu-stdin-contract-blocker|fixed|accepted-as-rejected|still-blocked" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | serious | wolong report | open | | |
| F-3 | Direct local Chengdu probes prove parser, grounder, engine, solved pipeline, and no-plan pipeline stdin behavior from `./bin/pandapi-*`. | `rg -n -- "direct Chengdu probe|pandapi-parser|pandapi-grounder|pandapi-engine|stdin|domain_no_plan|no_plan|PANDAPI_STATUS" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | correctness-grade | wolong acceptance | open | | |
| F-4 | Wolong erlexec/argv-list verification either passes or records a concrete external blocker with re-entry condition. | `rg -n -- "erlexec|argv-list|wolong-exec|resume|slice02-stdio-runner|re-entry|external blocker" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | correctness-grade | release acceptance anchor | open | | |
| F-5 | Any Wolong workspace edits, if needed, are scoped to proof fixtures/docs and are committed or explicitly left to a Wolong slice. | `rg -n -- "Wolong workspace|no Wolong edits|Wolong commit|Wolong slice|external blocker" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md` | serious | cross-repo boundary | open | | |
| F-6 | Arc09 Slice01 status is updated to resumed or explicitly still blocked. | `rg -n -- "Slice01 release-readiness-inventory.*blocked|Slice01 release-readiness-inventory.*resume|wolong supervision proof|wolong stdin" docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md docs/design-v0.3.0/project-plan.md` | serious | release planning | open | | |
| F-7 | Chengdu's existing managed contract and smoke gates still pass. | `make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed && make smoke && make smoke-negative` | serious | no regression | open | | |
| F-8 | Release-quality wayfinding still passes. | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check` | serious | release gate runway | open | | |
| F-9 | The slice keeps release publication out of scope. | `set -e; if git diff --cached --name-only -- release .github/release-notes.md.in | rg .; then exit 1; fi` | serious | boundary | open | | |
| F-10 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/cdc-verification.md` | serious | independence | open | | |
| F-11 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | open | | |

## What Worked

_(Fill at slice close.)_

## Closure

_(Fill at slice close.)_
