# CC Prompt: Arc08 Slice06 hddl-authoring-tutorial

You are implementing Arc08 Slice06 in `chengdu` on `release/0.3.x`.

Read first:

- `AGENTS.md`
- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice06-hddl-authoring-tutorial/slice-doc.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice06-hddl-authoring-tutorial/ledger.md`
- `docs/tutorial/03-model-a-feature-as-htn.md`

Implement the slice exactly as scoped:

- Create `docs/tutorial/04-write-hddl-files.md`.
- Reuse the Slice05 `status-summary` model.
- Write concrete `domain.hddl` and `problem.hddl` examples in temporary-file
  command blocks.
- Explain domain sections, problem sections, task network, methods, actions,
  predicates, preconditions, effects, and common syntax/modeling mistakes.
- Verify the authored files through `./bin/pandapi-parser`,
  `./bin/pandapi-grounder`, and `./bin/pandapi-engine`.
- Include verified no-plan and invalid-input mistake examples.
- Decide whether a checked-in fixture is needed.

Keep public docs free of Arc08/Slice/CC/CDC/ledger/planning vocabulary. Do not
touch README, release/package/publish surfaces, `.github/`, Makefile or `mk/`,
tests, tools, fixtures, parser/grounder/engine/runtime source, CLI reference,
migration page, architecture page, docs index, licenses, or wolong integration
unless the operator explicitly amends scope.

Verification:

- Run every F-row command in the Slice06 ledger.
- Run the happy-path, no-plan, and broken-syntax tutorial commands.
- Run `make test-contract-pipeline-managed`.
- Run `git diff --check` and `git diff --cached --check` after staging.
- Run the protected-boundary check from F-20.
- Aggregate `make check` is not required for this docs slice unless
  behavior-bearing surfaces change. If not run, explain why in the closing
  report.

Close the implementation with:

- updated `ledger.md` rows and evidence;
- `closing-report.md` walking F-1 through F-22;
- Arc08 `arc-plan.md`, `documentation-blueprint.md`, and
  `docs/design-v0.3.0/project-plan.md` status/bubble-up updates.

Do not create `cdc-verification.md`.
