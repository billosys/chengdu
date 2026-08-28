# Arc09 Blocker Ledger: stdin-artifact-io

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The implementation consumes the accepted stdin contract from `blocker-stdio-contract-design`. | `rg -n -- "stdin contract|blocker-stdio-contract-design|accepted" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdin-artifact-io docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | design dependency | closed | Reproduced after implementation; slice and arc docs cite the accepted contract and its follow-on implementation role. | |
| F-2 | Shared runtime/helper code owns repeated stdin materialization, path-role labeling, temporary-file cleanup, and failure status mapping. | `rg -n -- "stdin|material|temporary|cleanup|path_role|input_unavailable|input_invalid" pandaPI/runtime pandaPI/parser/src/pandapi_parser_native.cpp pandaPI/grounder/src/pandapi_grounder_native.cpp pandaPI/engine/src/pandapi_engine_native.cpp` | serious | good engineering | closed | Reproduced; `pandaPI/runtime/include/pandapi/runtime/stdin_materialization.hpp`, `pandaPI/runtime/src/stdin_materialization.cpp`, and `pandaPI/runtime/tests/stdin_materialization_smoke.cpp` own materialization, role fields, destructor cleanup, explicit cleanup status fields, and failure status helpers; wrappers call the shared helper and unwind through `ProcessExit` instead of `std::exit` so stack cleanup runs. | CDC review found and repaired the original staged `std::exit` cleanup bypass before commit. |
| F-3 | Parser supports every accepted stdin form and rejects unsupported parser stdin forms with documented status. | See `F-3 parser stdin evidence` below. | correctness-grade | wolong parser need | closed | Reproduced; domain-from-stdin and problem-from-stdin both produced `.htn` artifacts with `status=ok component=parser` and logical stdin fields; both-stdin rejected with exit `10` and `status=cli_usage_error component=parser`. | Uses `make build` so `./bin` is refreshed before the probes; recent parser stdin temp files are absent after success. |
| F-4 | Grounder supports stdin `.htn` input while preserving stdout artifact and stderr status ownership. | See `F-4 grounder stdin evidence` below. | correctness-grade | wolong grounder need | closed | Reproduced; grounder consumed parser-produced HTN from stdin, wrote the `.sas` artifact to `--output`, left stdout empty, and emitted `status=ok component=grounder path_role=htn path=- operation=read` on stderr. | Recent grounder stdin temp files are absent after success. |
| F-5 | Engine supports stdin `.sas` input while preserving solved and no-plan status behavior. | See `F-5 engine stdin evidence` below. | correctness-grade | wolong engine need | closed | Reproduced; engine consumed grounder-produced SAS from stdin and solved with `status=ok outcome=solved path_role=engine_input path=- operation=read`; direct unsolvable stdin preserved exit `2`, absent plan artifact, and `status=domain_no_plan outcome=no_plan path_role=engine_input path=- operation=read`. | Recent engine stdin temp files are absent after success and no-plan. |
| F-6 | Existing managed component contracts still pass. | `make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed` | serious | no regression | closed | Reproduced; command exited `0`; parser and engine summaries reported `0 failed`, and the combined component gate returned success. | |
| F-7 | Existing managed pipeline, smoke, and negative smoke gates still pass. | `make test-contract-pipeline-managed && make smoke && make smoke-negative` | serious | no regression | closed | Reproduced; pipeline summary reported `129 passed, 0 failed`; positive smoke reported `3 passed, 0 failed`; negative smoke reported `4 passed, 0 failed`. | |
| F-8 | Source-quality and safety gates cover the touched code. | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check` | serious | release gate runway | closed | Reproduced; safety checks, actionlint, shellcheck, clang-tidy, format-check, full test suite, and provenance-check exited `0`. | |
| F-9 | The slice does not edit wolong, release publication state, or public install claims. | See `F-9 boundary evidence` below. | serious | boundary | closed | Reproduced after staging; no protected public docs or release paths were listed. | Public managed-process/CLI docs remain for the fixture/docs follow-on slice; wolong remains untouched. |
| F-10 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdin-artifact-io/cdc-verification.md` | serious | independence | closed | Reproduced; no `cdc-verification.md` exists for this slice. | |
| F-11 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | closed | Reproduced after staging; both unstaged and staged whitespace checks exited `0`. | |

## Evidence Commands

### F-3 parser stdin evidence

```bash
set -e; make build; rm -f /tmp/chengdu-parser-stdin-domain.htn /tmp/chengdu-parser-stdin-problem.htn /tmp/chengdu-parser-both-stdin.htn; ./bin/pandapi-parser --supervised --status=stderr --output /tmp/chengdu-parser-stdin-domain.htn - fixtures/minimal/problem.hddl < fixtures/minimal/domain.hddl > /tmp/chengdu-parser-stdin-domain.stdout 2> /tmp/chengdu-parser-stdin-domain.stderr; test -s /tmp/chengdu-parser-stdin-domain.htn; test ! -s /tmp/chengdu-parser-stdin-domain.stdout; rg -n -- 'PANDAPI_STATUS.*status=ok.*component=parser.*path_role=domain.*path=-.*operation=read' /tmp/chengdu-parser-stdin-domain.stderr; ./bin/pandapi-parser --supervised --status=stderr --output /tmp/chengdu-parser-stdin-problem.htn fixtures/minimal/domain.hddl - < fixtures/minimal/problem.hddl > /tmp/chengdu-parser-stdin-problem.stdout 2> /tmp/chengdu-parser-stdin-problem.stderr; test -s /tmp/chengdu-parser-stdin-problem.htn; test ! -s /tmp/chengdu-parser-stdin-problem.stdout; rg -n -- 'PANDAPI_STATUS.*status=ok.*component=parser.*path_role=problem.*path=-.*operation=read' /tmp/chengdu-parser-stdin-problem.stderr; set +e; ./bin/pandapi-parser --supervised --status=stderr --output /tmp/chengdu-parser-both-stdin.htn - - < fixtures/minimal/domain.hddl > /tmp/chengdu-parser-both-stdin.stdout 2> /tmp/chengdu-parser-both-stdin.stderr; code=$?; set -e; test "$code" -eq 10; test ! -s /tmp/chengdu-parser-both-stdin.stdout; rg -n -- 'PANDAPI_STATUS.*status=cli_usage_error.*component=parser' /tmp/chengdu-parser-both-stdin.stderr; test -z "$(find /tmp -maxdepth 1 -mmin -2 -name 'pandapi-parser-stdin.*' -print)"
```

### F-4 grounder stdin evidence

```bash
set -e; make build; rm -f /tmp/chengdu-grounder-stdin.sas /tmp/chengdu-grounder-parser.stdout; ./bin/pandapi-parser --supervised --status=stderr --output - fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl > /tmp/chengdu-grounder-parser.stdout 2> /tmp/chengdu-grounder-parser.stderr; ./bin/pandapi-grounder --supervised --status=stderr --output /tmp/chengdu-grounder-stdin.sas - < /tmp/chengdu-grounder-parser.stdout > /tmp/chengdu-grounder-stdin.stdout 2> /tmp/chengdu-grounder-stdin.stderr; test -s /tmp/chengdu-grounder-stdin.sas; test ! -s /tmp/chengdu-grounder-stdin.stdout; rg -n -- 'PANDAPI_STATUS.*status=ok.*component=grounder.*path_role=htn.*path=-.*operation=read' /tmp/chengdu-grounder-stdin.stderr; rg -n -- 'PANDAPI_STATUS.*status=ok.*component=parser' /tmp/chengdu-grounder-parser.stderr; test -z "$(find /tmp -maxdepth 1 -mmin -2 -name 'pandapi-grounder-stdin.*' -print)"
```

### F-5 engine stdin evidence

```bash
set -e; make build; rm -f /tmp/chengdu-engine-stdin.plan /tmp/chengdu-engine-grounder.stdout /tmp/chengdu-engine-noplan-stdin.plan; ./bin/pandapi-grounder --supervised --status=stderr --output - fixtures/grounder/minimal.htn > /tmp/chengdu-engine-grounder.stdout 2> /tmp/chengdu-engine-grounder.stderr; ./bin/pandapi-engine --supervised --status=stderr --output /tmp/chengdu-engine-stdin.plan - < /tmp/chengdu-engine-grounder.stdout > /tmp/chengdu-engine-stdin.stdout 2> /tmp/chengdu-engine-stdin.stderr; test -s /tmp/chengdu-engine-stdin.plan; test ! -s /tmp/chengdu-engine-stdin.stdout; rg -n -- 'PANDAPI_STATUS.*status=ok.*component=engine.*outcome=solved.*path_role=engine_input.*path=-.*operation=read' /tmp/chengdu-engine-stdin.stderr; rg -n -- 'PANDAPI_STATUS.*status=ok.*component=grounder' /tmp/chengdu-engine-grounder.stderr; set +e; ./bin/pandapi-engine --supervised --status=stderr --output /tmp/chengdu-engine-noplan-stdin.plan - < fixtures/engine/unsolvable.sas > /tmp/chengdu-engine-noplan-stdin.stdout 2> /tmp/chengdu-engine-noplan-stdin.stderr; code=$?; set -e; test "$code" -eq 2; test ! -s /tmp/chengdu-engine-noplan-stdin.stdout; test ! -e /tmp/chengdu-engine-noplan-stdin.plan; rg -n -- 'PANDAPI_STATUS.*status=domain_no_plan.*component=engine.*outcome=no_plan.*path_role=engine_input.*path=-.*operation=read' /tmp/chengdu-engine-noplan-stdin.stderr; test -z "$(find /tmp -maxdepth 1 -mmin -2 -name 'pandapi-engine-stdin.*' -print)"
```

### F-9 boundary evidence

```bash
set -e; if git diff --cached --name-only -- release README.md docs/index.md docs/tutorial docs/reference docs/migration.md | rg .; then exit 1; fi
```

## What Worked

The shared runtime helper kept the component wrappers small: each wrapper only
identifies the accepted `-` role, delegates materialization and cleanup to the
runtime, and then reuses the existing inherited file-path flow. That preserved
file inputs, `--output -`, `--status=stderr`, stdout/status conflict handling,
and engine no-plan semantics while enabling supervised stdin pipelines.

CDC review found one cleanup correctness issue in the original staged
implementation: the wrapper-local `finish()` helpers called `std::exit`, which
bypasses destructors for automatic C++ objects. The final staged implementation
uses status-emitting `ProcessExit` exceptions caught by `main`, keeps
destructor cleanup as a safety net, and explicitly cleans materialized stdin
before reporting final outcomes so cleanup failures can be classified as
`output_unavailable` with `operation=cleanup`.

## Closure

Closed on 2026-08-20. This slice implements the accepted stdin artifact IO
contract and leaves the next release-blocking work to
`blocker-stdio-contract-fixtures`: Make-backed stdin fixture coverage, CI
placement, and current public process documentation for the implemented
contract. Slice01 remains blocked until the fixture/docs slice and wolong
supervision proof close.
