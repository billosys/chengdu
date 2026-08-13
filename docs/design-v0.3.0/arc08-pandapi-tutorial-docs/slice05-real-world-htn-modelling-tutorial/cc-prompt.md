# CC Prompt: Arc08 Slice05 real-world-htn-modelling-tutorial

You are implementing Arc08 Slice05 in `chengdu` on `release/0.3.x`.

Read first:

- `AGENTS.md`
- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice05-real-world-htn-modelling-tutorial/slice-doc.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice05-real-world-htn-modelling-tutorial/ledger.md`
- `docs/tutorial/01-htn-hddl-onboarding.md`
- `docs/tutorial/02-first-project-workflow.md`

Implement the slice exactly as scoped:

- Create `docs/tutorial/03-model-a-feature-as-htn.md`.
- Teach how to turn a prose software feature request into an HTN model before
  writing HDDL syntax.
- Cover planning question, scope, domain/problem split, objects, facts,
  compound tasks, primitive actions, methods, ordering constraints,
  solved/no-plan boundaries, and fixture decision.
- Clearly mark conceptual examples as conceptual and avoid runnable shell
  command blocks in this page.
- Keep public docs free of Arc08/Slice/CC/CDC/ledger/planning vocabulary.
- Do not add a checked-in fixture unless the ledger is amended first.
- Do not write the HDDL authoring tutorial; Slice06 owns
  `docs/tutorial/04-write-hddl-files.md`.

Update only the public tutorial page and Arc08/project planning surfaces
needed by the ledger. Do not touch README, release/package/publish surfaces,
`.github/`, Makefile or `mk/`, tests, tools, fixtures, parser/grounder/engine/
runtime source, CLI reference, migration page, architecture page, docs index,
licenses, or wolong integration unless the operator explicitly amends scope.

Verification:

- Run every F-row command in the Slice05 ledger.
- Run `git diff --check` and `git diff --cached --check` after staging.
- Run the protected-boundary check from F-18.
- Aggregate `make check` is not required for this conceptual docs slice unless
  behavior-bearing surfaces change. If not run, explain why in the closing
  report.

Close the implementation with:

- updated `ledger.md` rows and evidence;
- `closing-report.md` walking F-1 through F-20;
- Arc08 `arc-plan.md`, `documentation-blueprint.md`, and
  `docs/design-v0.3.0/project-plan.md` status/bubble-up updates.

Do not create `cdc-verification.md`.
