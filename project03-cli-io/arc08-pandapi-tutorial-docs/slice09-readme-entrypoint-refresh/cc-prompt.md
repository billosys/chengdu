# CC Prompt: Arc08 Slice09 readme-entrypoint-refresh

You are implementing Arc08 Slice09 in `chengdu` on `release/0.3.x`.

Read first:

- `AGENTS.md`
- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice09-readme-entrypoint-refresh/slice-doc.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice09-readme-entrypoint-refresh/ledger.md`
- `README.md`
- `docs/tutorial/01-htn-hddl-onboarding.md`
- `docs/tutorial/02-first-project-workflow.md`
- `docs/tutorial/03-model-a-feature-as-htn.md`
- `docs/tutorial/04-write-hddl-files.md`
- `docs/managed-process.md`
- `docs/reference/cli.md`
- `docs/migration.md`
- `docs/architecture.md`

Important policy notes:

- Public docs live under `docs/`. Planning/evidence docs live under
  `docs/design-v0.3.0/`.
- The README is public reader-facing prose. Do not mention Arc08, Slice09, CC,
  CDC, ledgers, closing reports, project plans, or arc plans in the README.
- `v0.3.0` command examples use canonical `pandapi-*` names and local
  `./bin/pandapi-*` paths for source builds.
- Do not document inherited `pandaPI*` command names as supported 0.3.0
  commands. Keep command-name history in `docs/migration.md`.
- Be precise about release status: currently published release examples may
  remain 0.2.0-specific, while 0.3.0 release assets, checksums, manifests, and
  wolong fetch/install proof remain release-preparation work until the release
  arc verifies them.

Implement the slice exactly as scoped:

- Refresh `README.md` as a concise public entry point.
- Keep the badge, logo, named-link style, and project personality where they
  still fit.
- Give a current overview of chengdu/pandaPI:
  - in-tree `pandaPI/` source;
  - parser, grounder, engine, and runtime;
  - canonical `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`;
  - managed-process behavior;
  - Make as the local and CI entrypoint.
- Keep the source-build path runnable with:
  - prerequisites;
  - `make readme-verbatim`;
  - `./bin/pandapi-parser`;
  - `./bin/pandapi-grounder`;
  - `./bin/pandapi-engine`;
  - `make smoke`;
  - `make smoke-negative`;
  - `make test`;
  - strict developer mode if still documented.
- Link the public docs suite:
  - tutorials `01` through `04`;
  - managed-process guide;
  - CLI reference;
  - migration guide;
  - architecture guide.
- Decide whether to create `docs/index.md`.
  - Create it if that keeps the README concise and improves reader
    wayfinding.
  - If you do not create it, explain why in the closing report.
  - If you create it, keep it public, reader-facing, and free of internal
    project-management vocabulary.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md`
  status/bubble-up at close.

Keep this slice out of code, Make, CI, tests, tools, fixtures,
release/package/publication, checksums, manifests, licenses, wolong
integration, parser/grounder/engine/runtime source, source-quality thresholds,
and dependency vendoring unless the operator explicitly amends scope.

Verification:

- Run every F-row command in the Slice09 ledger.
- Run `make readme-verbatim`.
- Run `make smoke && make smoke-negative`.
- Run `make help` if the README names Make targets beyond the already expected
  source-build/test path.
- Run `git diff --check` and `git diff --cached --check` after staging.
- Run the protected-boundary check from F-16.
- Aggregate `make check` is not required for this docs-entrypoint slice unless
  behavior-bearing surfaces change. If not run, explain why in the closing
  report.

Close the implementation with:

- updated `ledger.md` rows and evidence;
- `closing-report.md` walking F-1 through F-18;
- Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md`
  status/bubble-up updates.

Do not create `cdc-verification.md`.
