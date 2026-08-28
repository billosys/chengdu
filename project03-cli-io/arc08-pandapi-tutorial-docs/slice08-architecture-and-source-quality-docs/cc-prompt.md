# CC Prompt: Arc08 Slice08 architecture-and-source-quality-docs

You are implementing Arc08 Slice08 in `chengdu` on `release/0.3.x`.

Read first:

- `AGENTS.md`
- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice08-architecture-and-source-quality-docs/slice-doc.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice08-architecture-and-source-quality-docs/ledger.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-synthesis.md`
- `docs/reference/cli.md`
- `docs/managed-process.md`
- `docs/migration.md`
- `docs/tutorial/01-htn-hddl-onboarding.md`
- `docs/tutorial/02-first-project-workflow.md`
- `docs/tutorial/03-model-a-feature-as-htn.md`
- `docs/tutorial/04-write-hddl-files.md`

Important policy notes:

- Public docs live under `docs/`. Planning/evidence docs live under
  `docs/design-v0.3.0/`.
- Public prose should not require readers to know Arc08, Slice08, CC, CDC,
  ledger, or project-plan mechanics.
- `v0.3.0` public command examples use canonical `pandapi-*` names. Do not
  document inherited `pandaPI*` command names as supported commands.
- Source-quality language must be honest. Do not claim global zero warnings,
  global coverage floors, complete dependency audits, or release-publication
  proof that current evidence does not support.
- CLI11 is the selected command-line parsing replacement path, not a vague
  possibility. Be precise: it is selected policy/re-entry work, but it has not
  replaced current parser/grounder/engine option parsing yet.

Implement the slice exactly as scoped:

- Create `docs/architecture.md`.
- Explain the in-tree product shape: `pandaPI/parser`, `pandaPI/grounder`,
  `pandaPI/engine`, `pandaPI/runtime`, and canonical `pandapi-*` binaries.
- Explain the managed-process architecture at a high level and link to
  `docs/reference/cli.md`, `docs/managed-process.md`, `docs/migration.md`, and
  the tutorial sequence.
- Explain source classes in reader-facing language:
  first-party maintained, first-party generated, first-party generator/
  template, vendored third-party, dependency-internal, third-party generated,
  copied build artifact, and generated build output.
- Explain generated-code policy, including the parser Flex
  `%option noinput`/`%option nounput` closure, and keep gengetopt/CLI11
  re-entry separate from generated-output hand edits.
- Explain third-party and dependency boundaries for `cpddl`, H2, boruvka,
  opts, lpsolve, CUDD, and `rss.c`.
- Explain accepted dependency posture from Arc02 and current source-quality
  posture from Arc07.
- Explain Make-backed source-quality gates and measured baselines without
  turning them into public guarantees.
- Explain what remains deferred or budgeted for release/pre-release follow-up.

Keep this slice out of README, docs index, release/package/publication,
`.github/`, Makefile or `mk/`, tests, tools, fixtures, parser/grounder/engine/
runtime source, licenses, dependency vendoring, and wolong integration unless
the operator explicitly amends scope.

Verification:

- Run every F-row command in the Slice08 ledger.
- Run `make help`.
- Run source-quality evidence anchors:
  - `make source-quality-surface`
  - `make source-quality-profile-map`
  - `make source-quality-naming-check`
  - `make compile-db-first-party`
  - `make source-quality-gate-report`
  - `make generated-warning-triage`
  - `make generated-warning-triage-parser`
  - `make warning-inventory-first-party`
- Run `make safety-checks`.
- Run `make actionlint`.
- Run `git diff --check` and `git diff --cached --check` after staging.
- Run the protected-boundary check from F-18.
- Aggregate `make check` is not required for this docs slice unless
  behavior-bearing surfaces change. If not run, explain why in the closing
  report.

Close the implementation with:

- updated `ledger.md` rows and evidence;
- `closing-report.md` walking F-1 through F-21;
- Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md`
  status/bubble-up updates.

Do not create `cdc-verification.md`.
