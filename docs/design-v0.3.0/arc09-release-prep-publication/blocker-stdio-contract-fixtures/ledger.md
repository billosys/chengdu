# Arc09 Blocker Ledger: stdio-contract-fixtures

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Contract fixtures cover accepted parser stdin input forms and rejected unsupported parser stdin forms. | `rg -n -- "parser.*stdin|domain.*stdin|problem.*stdin|both.*stdin|unsupported.*stdin" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures` | correctness-grade | wolong parser need | done | attested: `stdio-parser-domain-stdin`, `stdio-parser-problem-stdin`, and `stdio-parser-both-stdin-unsupported` added to `tests/contract/run` and `fixtures/contract/stdio-contract-records.md`; Verify passed. | |
| F-2 | Contract fixtures cover grounder stdin `.htn` input with stdout artifact and stderr status separation. | `rg -n -- "grounder.*stdin|\\.htn|--output -|--status=stderr" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures` | correctness-grade | wolong grounder need | done | attested: `stdio-grounder-stdin-stdout-status` asserts stdin `.htn`, stdout `.sas`, no stdout status, stderr status, and logical `path=-`; Verify passed. | |
| F-3 | Contract fixtures cover engine stdin `.sas` input for solved and no-plan outcomes. | `rg -n -- "engine.*stdin|\\.sas|domain_no_plan|no_plan|solved" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures` | correctness-grade | wolong engine need | done | attested: `stdio-engine-stdin-solved`, `stdio-engine-stdin-no-plan`, and both stdio pipeline records cover solved and `domain_no_plan` / `outcome=no_plan`; Verify passed. | |
| F-4 | Full supervised stdin/stdout/stderr pipeline fixtures pass for solved and no-plan chains. | `make test-contract-stdio-managed` | serious | wolong pipeline proof | done | attested: `make test-contract-stdio-managed` passed with 187 passed, 0 failed on macos-arm64. | New Make target added before public docs reference it. |
| F-5 | CI workflow changes, if any, invoke Make targets rather than scripts or harnesses directly. | `make actionlint && rg -n -- "test-contract-stdio-managed|make " .github/workflows` | serious | CI policy | done | attested: Verify passed; no workflow file was edited, and existing `make ci-linux` / `make ci-macos` inherit the new gate through `make test`. | |
| F-6 | Public managed-process and CLI docs describe supported stdin input forms and rejected forms accurately. | `rg -n -- "stdin|standard input|--output -|--status=stderr|unsupported" docs/managed-process.md docs/reference/cli.md` | correctness-grade | public contract | done | attested: `docs/managed-process.md` and `docs/reference/cli.md` now document supported stdin forms, parser `- -` rejection, stdout artifact ownership, stderr status, and logical stdin fields; Verify passed. | |
| F-7 | Existing contract and smoke gates still pass. | `make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed && make smoke && make smoke-negative` | serious | no regression | done | attested: exact Verify command completed successfully; visible summaries included parser 298/0, engine 312/0, pipeline 129/0, positive smoke 3/0, and negative smoke 4/0. | |
| F-8 | Release-quality wayfinding still passes. | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check` | serious | release gate runway | done | attested: exact Verify command passed after fixing a shellcheck SC2318 local-assignment issue in the new runner cases. | |
| F-9 | The slice does not edit wolong or publication state. | `set -e; if git diff --cached --name-only -- release | rg .; then exit 1; fi` | serious | boundary | done | attested: staged boundary check passed after staging; no `release/` or wolong workspace changes were made. | Wolong is a separate workspace and is out of scope. |
| F-10 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures/cdc-verification.md` | serious | independence | done | attested: Verify passed; no `cdc-verification.md` exists for this implementation close. | |
| F-11 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | done | attested: Verify passed after staging. | |

## What Worked

The existing contract runner was already a good home for process-boundary
checks. Adding a dedicated `stdio` component let the slice pin release-blocking
stdin behavior without weakening the older pipeline fixture meaning.

## Closure

Closed as implementation proposed done. The next Arc09 blocker is
`blocker-wolong-supervision-proof`, which should verify the same stdin/stdout/
stderr contract from wolong.
