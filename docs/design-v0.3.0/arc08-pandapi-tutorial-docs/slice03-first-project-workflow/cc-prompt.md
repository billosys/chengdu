# CC Prompt: Arc08 Slice03 first-project-workflow

You are implementing Arc08 Slice03 for chengdu 0.3.0 on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
4. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
5. `docs/tutorial/htn-hddl-onboarding.md`
6. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice03-first-project-workflow/slice-doc.md`
7. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice03-first-project-workflow/ledger.md`
8. `fixtures/minimal/domain.hddl`
9. `fixtures/minimal/problem.hddl`
10. `fixtures/unsolvable/domain.hddl`
11. `fixtures/unsolvable/problem.hddl`
12. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`

Treat the ledger as the specification. If a row is wrong, impossible, or too
weak for the slice goal, stop and propose an amendment rather than quietly
softening it.

## Mission

Write the first runnable workflow tutorial:

- `docs/tutorial/first-project-workflow.md`

This page should pick up after the onboarding page. The reader should parse
`fixtures/minimal`, ground the parser artifact, solve the grounded artifact,
inspect the generated files, rerun safely, and then compare with
`fixtures/unsolvable` to understand a valid no-plan outcome.

## Implementation Requirements

- Create only the public workflow page and the necessary Arc08/project
  planning status updates for Slice03.
- Use the accepted public docs layout:
  - public project docs under `docs/`;
  - tutorial pages under `docs/tutorial/`;
  - planning/evidence artifacts under `docs/design-v0.3.0/`.
- Explain and verify:
  - platform binary setup for macOS arm64 and Linux x86_64;
  - parser command producing `.htn`;
  - grounder command producing `.sas`;
  - engine command producing a plan artifact;
  - simple artifact inspection;
  - rerun hygiene in a temporary workspace;
  - no-plan comparison with `fixtures/unsolvable`.
- Decide whether one intermediate fixture beyond `fixtures/unsolvable` is
  needed. If not, say so in the closing report. If yes, add the smallest
  checked-in fixture needed and verify it.
- Keep public prose free of Arc08, Slice03, CC, CDC, ledger, and
  closing-report terminology.
- Use only canonical `pandapi-*` command names. Do not teach inherited
  `pandaPI*` command compatibility.

## Out Of Scope

- Do not create or edit:
  - `README.md`
  - release/package/publish surfaces
  - `.github/`
  - `Makefile` or `mk/`
  - parser/grounder/engine/runtime source
  - managed-process guide
  - CLI reference
  - migration page
  - architecture/source-quality page
  - docs index
- Do not create `cdc-verification.md`; CDC owns that.
- Do not turn this page into the managed-process guide. Slice04 owns
  `--supervised`, stdout/stderr ownership, final `PANDAPI_STATUS`, and status
  taxonomy details.
- Do not add speculative public links to pages that do not exist unless the
  text clearly marks them as coming later and no link-check burden is created.

## Required Verification

Run the ledger checks that apply after implementation, including:

- the F-1 through F-20 grep/boundary checks in `ledger.md`;
- the happy-path workflow verification in F-7;
- the no-plan workflow verification in F-10;
- `make test-contract-pipeline-managed`;
- `make smoke`;
- `make smoke-negative`;
- `git diff --check`;
- `git diff --cached --check`;
- staged protected-boundary check from F-18.

Aggregate `make check` is not required for this docs-writing slice unless you
touch code, Make, CI, tests, tools, release/package, or other behavior-bearing
surfaces. If you do not run it, explain why in the closing report and list the
constituent gates you did run.

## Closure

When complete:

1. Update every ledger row with final status and evidence.
2. Add
   `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice03-first-project-workflow/closing-report.md`.
3. Include a row-by-row ledger walk for F-1 through F-20.
4. Include `Bubble-up to Arc08`, explicitly stating:
   - whether Slice03 delivered A3;
   - whether an intermediate fixture beyond `fixtures/unsolvable` was needed;
   - whether Slice04, Slice05, README, or Arc09 need workflow handoff notes.
5. Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` to mark
   Slice03 CC proposed done, awaiting CDC verification.
6. Commit the completed slice with the required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```

Report the commit SHA, changed files, verification run, any deferrals or
no-ops, and final worktree status.
