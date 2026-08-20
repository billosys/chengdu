# Arc09 Blocker Ledger: stdio-contract-design

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The blocker slice is explicitly marked as blocking Slice01 release-readiness-inventory. | `rg -n -- "blocks Slice01 release-readiness-inventory" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design/slice-doc.md docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | wolong blocker | open | | |
| F-2 | The design records parser stdin semantics for domain-from-stdin, problem-from-stdin, and both-input forms. | `rg -n -- "domain from stdin|problem from stdin|both.*stdin|framing|rejected" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | correctness-grade | wolong parser need | open | | |
| F-3 | The design records grounder stdin semantics for parser-generated `.htn` artifacts. | `rg -n -- "grounder.*stdin|\\.htn|parser-generated" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | correctness-grade | wolong grounder need | open | | |
| F-4 | The design records engine stdin semantics for grounder-generated `.sas` artifacts and no-plan classification preservation. | `rg -n -- "engine.*stdin|\\.sas|domain_no_plan|no_plan" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | correctness-grade | wolong engine need | open | | |
| F-5 | The design preserves stdout and stderr ownership rules for `--output -` plus `--status=stderr`. | `rg -n -- "--output -|--status=stderr|stdout ownership|stderr|PANDAPI_STATUS" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | managed-process contract | open | | |
| F-6 | The design chooses shared runtime/helper ownership for repeated stdin materialization or gives a concrete no-share rationale. | `rg -n -- "shared runtime|stdin materialization|helper|reuse|no-share rationale" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | good engineering | open | | |
| F-7 | The slice stays out of product code, public docs, packaging, publication, and wolong workspace changes. | `set -e; if git diff --cached --name-only -- pandaPI README.md docs/index.md docs/tutorial docs/managed-process.md docs/reference docs/migration.md docs/architecture.md Makefile mk tools .github/workflows release | rg .; then exit 1; fi` | serious | boundary | open | | Wolong is a separate workspace; this chengdu slice must not edit it. |
| F-8 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design/cdc-verification.md` | serious | independence | open | | |
| F-9 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | open | | |

## What Worked

_(Fill at slice close.)_

## Closure

_(Fill at slice close.)_
