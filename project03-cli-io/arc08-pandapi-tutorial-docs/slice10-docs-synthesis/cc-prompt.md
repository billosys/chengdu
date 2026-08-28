# CC Prompt: Arc08 Slice10 docs-synthesis

You are implementing Arc08 Slice10 in `chengdu` on `release/0.3.x`.

Read first:

- `AGENTS.md`
- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice10-docs-synthesis/slice-doc.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice10-docs-synthesis/ledger.md`
- `README.md`
- all public docs under `docs/` except `docs/design*`
- Slice01 through Slice09 closing reports and CDC verification files

Important policy notes:

- Public docs live under `docs/`. Planning/evidence docs live under
  `docs/design-v0.3.0/`.
- Public prose must not mention Arc08, Slice10, CC, CDC, ledgers,
  closing reports, project plans, or arc plans.
- `v0.3.0` public command examples use canonical `pandapi-*` names and local
  `./bin/pandapi-*` paths for source builds.
- Inherited `pandaPI*` command names are not supported 0.3.0 commands. They
  may appear only as historical migration context in `docs/migration.md`.
- 0.3.0 release assets are not published yet. Keep checksums, manifests,
  release notes, license/NOTICE release proof, wolong fetch/install proof, and
  publication in release-preparation language for Arc09.

Implement the slice exactly as scoped:

- Create `docs/index.md` as the public docs landing page.
- Update `README.md` only as needed to point to `docs/index.md` and preserve
  the most useful direct links.
- Make small consistency fixes across public docs only when the composition
  review finds a concrete mismatch.
- Create
  `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/docs-synthesis.md`.
- Create `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/closing-report.md`
  as the Arc08 implementation-proposed-done closing report.
- Close the Slice10 ledger and write
  `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice10-docs-synthesis/closing-report.md`.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` to mark
  Slice10 implementation proposed done and route Arc09
  `release-prep-publication` as next.

Keep this slice out of release assets, release workflows, checksums, manifests,
license bundles, wolong integration, parser/grounder/engine/runtime source,
tests, fixtures, Makefile, `mk/`, tools, source-quality thresholds, and
dependency vendoring unless the operator explicitly amends scope.

Verification:

- Run every F-row command in the Slice10 ledger.
- Run `make readme-verbatim`.
- Run `make smoke && make smoke-negative`.
- Run `make test-contract-pipeline-managed`.
- Run live `--help`, `--version`, and `--provenance` probes for
  `./bin/pandapi-parser`, `./bin/pandapi-grounder`, and
  `./bin/pandapi-engine`.
- Run `make safety-checks && make actionlint`.
- Run `git diff --check` and `git diff --cached --check` after staging.
- Run the protected-boundary check from F-20.
- Aggregate `make check` is not required for this docs-composition slice
  unless behavior-bearing surfaces change. If not run, explain why in the
  closing report.

Close with:

- updated `ledger.md` rows and evidence;
- Slice10 `closing-report.md` walking F-1 through F-21;
- Arc08 `closing-report.md` walking slices 01-10 and arc ledger A1-A12;
- Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md`
  status/bubble-up updates.

Do not create `cdc-verification.md`.
