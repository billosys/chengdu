# Arc09 Blocker Ledger: stdio-contract-fixtures

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Contract fixtures cover accepted parser stdin input forms and rejected unsupported parser stdin forms. | `rg -n -- "parser.*stdin|domain.*stdin|problem.*stdin|both.*stdin|unsupported.*stdin" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures` | correctness-grade | wolong parser need | open | | |
| F-2 | Contract fixtures cover grounder stdin `.htn` input with stdout artifact and stderr status separation. | `rg -n -- "grounder.*stdin|\\.htn|--output -|--status=stderr" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures` | correctness-grade | wolong grounder need | open | | |
| F-3 | Contract fixtures cover engine stdin `.sas` input for solved and no-plan outcomes. | `rg -n -- "engine.*stdin|\\.sas|domain_no_plan|no_plan|solved" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures` | correctness-grade | wolong engine need | open | | |
| F-4 | Full supervised stdin/stdout/stderr pipeline fixtures pass for solved and no-plan chains. | `make test-contract-stdio-managed` | serious | wolong pipeline proof | open | | Add this Make target or equivalent named Make target before CI/docs reference it. |
| F-5 | CI workflow changes, if any, invoke Make targets rather than scripts or harnesses directly. | `make actionlint && rg -n -- "test-contract-stdio-managed|make " .github/workflows` | serious | CI policy | open | | |
| F-6 | Public managed-process and CLI docs describe supported stdin input forms and rejected forms accurately. | `rg -n -- "stdin|standard input|--output -|--status=stderr|unsupported" docs/managed-process.md docs/reference/cli.md` | correctness-grade | public contract | open | | |
| F-7 | Existing contract and smoke gates still pass. | `make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed && make smoke && make smoke-negative` | serious | no regression | open | | |
| F-8 | Release-quality wayfinding still passes. | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check` | serious | release gate runway | open | | |
| F-9 | The slice does not edit wolong or publication state. | `set -e; if git diff --cached --name-only -- release | rg .; then exit 1; fi` | serious | boundary | open | | Wolong is a separate workspace and is out of scope. |
| F-10 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures/cdc-verification.md` | serious | independence | open | | |
| F-11 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | open | | |

## What Worked

_(Fill at slice close.)_

## Closure

_(Fill at slice close.)_
