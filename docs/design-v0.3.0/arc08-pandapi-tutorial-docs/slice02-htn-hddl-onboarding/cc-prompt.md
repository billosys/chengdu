# CC Prompt: Arc08 Slice02 htn-hddl-onboarding

You are implementing Arc08 Slice02 for chengdu 0.3.0 on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
4. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
5. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice02-htn-hddl-onboarding/slice-doc.md`
6. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice02-htn-hddl-onboarding/ledger.md`
7. `fixtures/minimal/domain.hddl`
8. `fixtures/minimal/problem.hddl`
9. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`

Treat the ledger as the specification. If a row is wrong, impossible, or too
weak for the slice goal, stop and propose an amendment rather than quietly
softening it.

## Mission

Write the beginner concept chapter:

- `docs/tutorial/htn-hddl-onboarding.md`

This page is for a reader who may not know HTN planning, PDDL/HDDL, or
pandaPI. Teach the concepts before the full command workflow. Use
`fixtures/minimal` as the concrete example and include one verified
`pandapi-parser` command that produces a `.htn` artifact. Leave the complete
parse -> ground -> solve workflow to Slice03.

## Implementation Requirements

- Create only the public onboarding page and the necessary Arc08/project
  planning status updates for Slice02.
- Explain:
  - HTN tasks, methods, primitive actions, decomposition, and ordered
    subtasks;
  - domain versus problem files;
  - how `fixtures/minimal/domain.hddl` and
    `fixtures/minimal/problem.hddl` map to those concepts;
  - the conceptual pipeline:
    `pandapi-parser` -> `.htn`,
    `pandapi-grounder` -> `.sas`,
    `pandapi-engine` -> plan/no-plan outcome;
  - solved versus no-plan outcomes in beginner terms.
- Include one runnable parser command. Make it portable across the supported
  local platforms by selecting `dist/macos-arm64` or `dist/linux-x86_64` from
  `uname`, or use an equally runnable local pattern.
- Decide whether the minimal fixture is enough for beginner onboarding. If it
  is enough, state that decision in the closing report. If not, add the
  smallest checked-in fixture needed and verify it.
- Keep public prose free of Arc08, Slice02, CC, CDC, ledger, and closing-report
  terminology.
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
- Do not add speculative public links to pages that do not exist unless the
  text clearly marks them as coming later and no link-check burden is created.

## Required Verification

Run the ledger checks that apply after implementation, including:

- the F-1 through F-16 grep/boundary checks in `ledger.md`;
- the parser command verification in F-8;
- `make test-contract-parser-managed`;
- `git diff --check`;
- `git diff --cached --check`;
- staged protected-boundary check from F-14.

Aggregate `make check` is not required for this docs-writing slice unless you
touch code, Make, CI, tests, tools, release/package, or other behavior-bearing
surfaces. If you do not run it, explain why in the closing report and list the
constituent gates you did run.

## Closure

When complete:

1. Update every ledger row with final status and evidence.
2. Add
   `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice02-htn-hddl-onboarding/closing-report.md`.
3. Include a row-by-row ledger walk for F-1 through F-16.
4. Include `Bubble-up to Arc08`, explicitly stating:
   - whether Slice02 delivered A2;
   - whether the minimal fixture was enough;
   - whether Slice03 needs any fixture or workflow adjustment.
5. Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` to mark
   Slice02 CC proposed done, awaiting CDC verification.
6. Commit the completed slice with the required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```

Report the commit SHA, changed files, verification run, any deferrals or
no-ops, and final worktree status.
