# Arc09 Blocker Ledger: wolong-supervision-proof

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Chengdu stdin contract fixture gate passes. | `make test-contract-stdio-managed` | serious | prior blocker slices | open | | |
| F-2 | The original wolong blocker report is re-read and each claim is mapped to fixed, accepted, or still-blocked. | `rg -n -- "fixed|accepted|still-blocked|chengdu-stdin-contract-blocker" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof` | serious | wolong report | open | | |
| F-3 | Live probes prove parser, grounder, engine, solved pipeline, and no-plan pipeline stdin behavior from argv-list equivalent commands. | `rg -n -- "pandapi-parser|pandapi-grounder|pandapi-engine|stdin|domain_no_plan|argv-list|erlexec" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof` | correctness-grade | wolong acceptance | open | | |
| F-4 | Wolong verification either passes or records a concrete external blocker with re-entry condition. | `rg -n -- "wolong|resume|slice02-stdio-runner|re-entry|blocked" docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof` | serious | release acceptance anchor | open | | |
| F-5 | Arc09 Slice01 status is updated to resumed or explicitly still blocked. | `rg -n -- "Slice01 release-readiness-inventory.*blocked|Slice01 release-readiness-inventory.*resume|wolong stdin" docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md docs/design-v0.3.0/project-plan.md` | serious | release planning | open | | |
| F-6 | The slice keeps release publication out of scope. | `set -e; if git diff --cached --name-only -- release .github/release-notes.md.in | rg .; then exit 1; fi` | serious | boundary | open | | |
| F-7 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/cdc-verification.md` | serious | independence | open | | |
| F-8 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | open | | |

## What Worked

_(Fill at slice close.)_

## Closure

_(Fill at slice close.)_
