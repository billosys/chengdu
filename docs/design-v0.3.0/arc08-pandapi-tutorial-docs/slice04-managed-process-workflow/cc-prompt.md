# CC Prompt: Arc08 Slice04 managed-process-workflow

You are implementing Arc08 Slice04 for chengdu 0.3.0 on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
4. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
5. `docs/tutorial/01-htn-hddl-onboarding.md`
6. `docs/tutorial/02-first-project-workflow.md`
7. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice04-managed-process-workflow/slice-doc.md`
8. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice04-managed-process-workflow/ledger.md`
9. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
10. `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
11. `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
12. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
13. `tests/contract/run`
14. `fixtures/minimal/domain.hddl`
15. `fixtures/minimal/problem.hddl`
16. `fixtures/unsolvable/domain.hddl`
17. `fixtures/unsolvable/problem.hddl`
18. `fixtures/broken-syntax/domain.hddl`
19. `fixtures/broken-syntax/problem.hddl`

Treat the ledger as the specification. If a row is wrong, impossible, or too
weak for the slice goal, stop and propose an amendment rather than quietly
softening it.

## Mission

Write the public managed-process guide:

- `docs/managed-process.md`

The reader is a process-manager or integration author. They need to run
`pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` under supervision,
capture stdout/stderr safely, classify outcomes from exit codes and final
`PANDAPI_STATUS` fields, and avoid scraping human diagnostic prose.

## Implementation Requirements

- Create only the public managed-process page and the necessary Arc08/project
  planning status updates for Slice04.
- Use the accepted public docs layout:
  - public project docs under `docs/`;
  - tutorial pages under `docs/tutorial/`;
  - planning/evidence artifacts under `docs/design-v0.3.0/`.
- Use local build examples with `make build` and `./bin/pandapi-*`.
- Explain:
  - `--supervised`;
  - `--status`, `--status=stderr`, and `--status=stdout`;
  - stdout and stderr ownership;
  - final `PANDAPI_STATUS` tagged records;
  - required status fields;
  - common status names and exit codes;
  - ANSI/color behavior under supervised output;
  - stdout conflict behavior when stdout is already the artifact or
    informational stream.
- Include runnable examples for:
  - successful supervised parser invocation;
  - supervised parse -> ground -> solve pipeline;
  - valid no-plan outcome with `fixtures/unsolvable`;
  - invalid input or output-unavailable negative outcome;
  - stdout ownership conflict or status/artifact stream conflict.
- Classify outcomes from exit code and status fields. Human diagnostics may be
  described as useful for people, but they are not the integration contract.
- Use only canonical `pandapi-*` command names. Do not teach inherited
  `pandaPI*` command compatibility.
- Keep public prose free of Arc08, Slice04, CC, CDC, ledger, and
  closing-report terminology.

## Out Of Scope

- Do not create or edit:
  - `README.md`
  - release/package/publish surfaces
  - `.github/`
  - `Makefile` or `mk/`
  - `tests/`, `tools/`, or `fixtures/`
  - parser/grounder/engine/runtime source
  - CLI reference
  - migration page
  - architecture/source-quality page
  - docs index
- Do not create `cdc-verification.md`; CDC owns that.
- Do not include speculative supervisor library code. Shell commands and
  expected status predicates are enough for this slice unless the operator
  explicitly accepts a scope amendment.
- Do not revive inherited-name compatibility language from older Arc03 docs.
  Public 0.3.0 docs follow the later no-compatibility decision.

## Required Verification

Run the ledger checks that apply after implementation, including:

- the F-1 through F-25 grep/boundary checks in `ledger.md`;
- the supervised parser verification in F-9;
- the supervised pipeline verification in F-11;
- the no-plan verification in F-13;
- the negative outcome verification in F-15;
- the stdout conflict verification in F-17;
- `make test-contract-parser-managed`;
- `make test-contract-grounder-managed`;
- `make test-contract-engine-managed`;
- `make test-contract-pipeline-managed`;
- `git diff --check`;
- `git diff --cached --check`;
- staged protected-boundary check from F-23.

Aggregate `make check` is not required for this docs-writing slice unless you
touch code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces. If you do not run it, explain why in the closing
report and list the constituent gates you did run.

## Closure

When complete:

1. Update every ledger row with final status and evidence.
2. Add
   `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice04-managed-process-workflow/closing-report.md`.
3. Include a row-by-row ledger walk for F-1 through F-25.
4. Include `Bubble-up to Arc08`, explicitly stating:
   - whether Slice04 delivered A4;
   - whether any fixture or verification gap needs a later docs/testing slice;
   - whether Slice05, README, or Arc09 need managed-process handoff notes.
5. Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` to mark
   Slice04 CC proposed done, awaiting CDC verification.
6. Commit the completed slice with the required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```

Report the commit SHA, changed files, verification run, any deferrals or
no-ops, and final worktree status.
