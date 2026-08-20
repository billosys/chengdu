# Arc09 Blocker Ledger: stdin-artifact-io

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The implementation consumes the accepted stdin contract from `blocker-stdio-contract-design`. | `rg -n -- "stdin contract|blocker-stdio-contract-design|accepted" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdin-artifact-io docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | design dependency | open | | |
| F-2 | Shared runtime/helper code owns repeated stdin materialization, path-role labeling, temporary-file cleanup, and failure status mapping. | `rg -n -- "stdin|material|temporary|cleanup|path_role|input_unavailable|input_invalid" pandaPI/runtime pandaPI/parser/src/pandapi_parser_native.cpp pandaPI/grounder/src/pandapi_grounder_native.cpp pandaPI/engine/src/pandapi_engine_native.cpp` | serious | good engineering | open | | |
| F-3 | Parser supports every accepted stdin form and rejects unsupported parser stdin forms with documented status. | `make build-parser && ./bin/pandapi-parser --supervised --status=stderr --output /tmp/chengdu-parser-stdin.htn - fixtures/minimal/problem.hddl < fixtures/minimal/domain.hddl` | correctness-grade | wolong parser need | open | | Expand with the exact accepted parser forms from the design slice. |
| F-4 | Grounder supports stdin `.htn` input while preserving stdout artifact and stderr status ownership. | `make build-grounder && ./bin/pandapi-parser --supervised --status=stderr --output - fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl | ./bin/pandapi-grounder --supervised --status=stderr --output /tmp/chengdu-grounder-stdin.sas -` | correctness-grade | wolong grounder need | open | | |
| F-5 | Engine supports stdin `.sas` input while preserving solved and no-plan status behavior. | `make build-engine && ./bin/pandapi-grounder --supervised --status=stderr --output - fixtures/grounder/minimal.htn | ./bin/pandapi-engine --supervised --status=stderr --output /tmp/chengdu-engine-stdin.plan -` | correctness-grade | wolong engine need | open | | Add no-plan probe before close. |
| F-6 | Existing managed component contracts still pass. | `make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed` | serious | no regression | open | | |
| F-7 | Existing managed pipeline, smoke, and negative smoke gates still pass. | `make test-contract-pipeline-managed && make smoke && make smoke-negative` | serious | no regression | open | | |
| F-8 | Source-quality and safety gates cover the touched code. | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check` | serious | release gate runway | open | | |
| F-9 | The slice does not edit wolong, release publication state, or public install claims. | `set -e; if git diff --cached --name-only -- release README.md docs/index.md docs/tutorial docs/reference docs/migration.md | rg .; then exit 1; fi` | serious | boundary | open | | Public managed-process/CLI docs may be updated only if required to document implemented behavior. Wolong is a separate workspace and is out of scope. |
| F-10 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdin-artifact-io/cdc-verification.md` | serious | independence | open | | |
| F-11 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | open | | |

## What Worked

_(Fill at slice close.)_

## Closure

_(Fill at slice close.)_
