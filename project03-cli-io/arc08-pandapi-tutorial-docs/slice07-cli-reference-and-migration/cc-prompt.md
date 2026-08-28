# CC Prompt: Arc08 Slice07 cli-reference-and-migration

You are implementing Arc08 Slice07 in `chengdu` on `release/0.3.x`.

Read first:

- `AGENTS.md`
- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice07-cli-reference-and-migration/slice-doc.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice07-cli-reference-and-migration/ledger.md`
- `docs/managed-process.md`
- `docs/tutorial/01-htn-hddl-onboarding.md`
- `docs/tutorial/02-first-project-workflow.md`
- `docs/tutorial/03-model-a-feature-as-htn.md`
- `docs/tutorial/04-write-hddl-files.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`

Important policy note:

- The Arc03 CLI naming document contains historical compatibility-transition
  text. The later operator decision and Arc05 cutover evidence supersede that
  public policy for 0.3.0: do not document inherited `pandaPI*` command names
  as supported 0.3.0 compatibility commands. Use them only in migration history
  and rename mapping, and state the no-compatibility guarantee plainly.

Implement the slice exactly as scoped:

- Create `docs/reference/cli.md`.
- Create `docs/migration.md`.
- Document the canonical `pandapi-parser`, `pandapi-grounder`, and
  `pandapi-engine` commands.
- Reconcile command synopsis and options with live `./bin/* --help` output.
- Cover supported normal surfaces, inputs, outputs, artifacts, common options,
  status/exit summary, and non-supported surface boundaries.
- Link to the tutorial sequence and `docs/managed-process.md` rather than
  duplicating those pages wholesale.
- Write the 0.2.0 -> 0.3.0 migration guide with command-name mapping and a
  behavior-change table.
- Use runnable repository-root examples with `./bin/pandapi-*`.

Keep public docs free of Arc08/Slice/CC/CDC/ledger/planning vocabulary. Do not
touch README, docs index, architecture/source-quality docs, release/package/
publish surfaces, `.github/`, Makefile or `mk/`, tests, tools, fixtures,
parser/grounder/engine/runtime source, licenses, or wolong integration unless
the operator explicitly amends scope.

Verification:

- Run every F-row command in the Slice07 ledger.
- Run `make build`.
- Run live help checks:
  - `./bin/pandapi-parser --help`
  - `./bin/pandapi-grounder --help`
  - `./bin/pandapi-engine --help`
- Run representative `--version` and `--provenance` commands for all three
  binaries.
- Run `make smoke`.
- Run `make smoke-negative`.
- Run `make test-contract-pipeline-managed`.
- Run `git diff --check` and `git diff --cached --check` after staging.
- Run the protected-boundary check from F-21.
- Aggregate `make check` is not required for this docs slice unless
  behavior-bearing surfaces change. If not run, explain why in the closing
  report.

Close the implementation with:

- updated `ledger.md` rows and evidence;
- `closing-report.md` walking F-1 through F-23;
- Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md`
  status/bubble-up updates.

Do not create `cdc-verification.md`.
