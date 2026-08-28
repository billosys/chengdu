# Arc08 Slice09: readme-entrypoint-refresh

Status: open
Opened: 2026-08-14

## Goal

Refresh the top-level `README.md` as the public entry point for the pandaPI
0.3.0 documentation suite now that the tutorial, managed-process, CLI,
migration, and architecture anchors exist.

The README should help a new reader quickly answer:

- What is chengdu/pandaPI?
- Which release/install path is currently published versus still awaiting
  0.3.0 release proof?
- How do I build from source and get local `./bin/pandapi-*` commands?
- Where do I start learning HTN/HDDL modelling and the full
  parse -> ground -> solve workflow?
- Where are CLI, managed-process, migration, and architecture details?
- What should maintainers run locally?

This slice closes Arc08 A7 at implementation-proposed-done strength and gives
Slice10 a stable README/docs composition surface to verify.

## In Scope

- Update `README.md` as a concise public entry point.
- Keep the existing badge, logo, named-link style, and project personality
  where possible.
- Correct the overview for the current 0.3.0 repository shape:
  - in-tree `pandaPI/` product source;
  - canonical `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`
    command names;
  - managed-process behavior;
  - Make-backed build/test/release entrypoints.
- Keep release-install wording honest:
  - the currently published release examples may remain 0.2.0-specific if
    that is what exists today;
  - 0.3.0 release assets, checksums, manifests, and wolong fetch/install proof
    must be described as release-preparation work until Arc09 verifies them;
  - do not imply unpublished 0.3.0 tarballs already exist.
- Keep source-build instructions centered on:
  - prerequisites;
  - `make readme-verbatim`;
  - `./bin/pandapi-parser`;
  - `./bin/pandapi-grounder`;
  - `./bin/pandapi-engine`;
  - `make smoke`;
  - `make smoke-negative`;
  - `make test`;
  - strict developer mode when applicable.
- Link to the current public docs:
  - `docs/tutorial/01-htn-hddl-onboarding.md`;
  - `docs/tutorial/02-first-project-workflow.md`;
  - `docs/tutorial/03-model-a-feature-as-htn.md`;
  - `docs/tutorial/04-write-hddl-files.md`;
  - `docs/managed-process.md`;
  - `docs/reference/cli.md`;
  - `docs/migration.md`;
  - `docs/architecture.md`.
- Decide whether to create `docs/index.md` as a compact public docs landing
  page:
  - create it if that keeps the README concise and improves reader wayfinding;
  - otherwise record in the closing report why the README can link the suite
    directly and leave `docs/index.md` for Slice10 or later.
- If `docs/index.md` is created, keep it public and reader-facing, with links
  to the same public docs suite.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` when the
  slice closes.

## Out Of Scope

- Do not edit code, Make targets, CI workflows, tests, tools, fixtures,
  package/release assets, checksums, manifests, license bundles, release notes,
  wolong integration, parser/grounder/engine/runtime implementation source, or
  source-quality thresholds.
- Do not publish or simulate a 0.3.0 release.
- Do not update release workflow behavior or package contents.
- Do not create a migration guarantee for inherited `pandaPI*` command names.
  The product name `pandaPI` is fine; inherited command names should appear
  only as historical migration context in `docs/migration.md`, not as README
  supported commands.
- Do not expose Arc08, Slice09, CC, CDC, ledger, closing-report,
  project-plan, or arc-plan vocabulary in public README/docs prose.
- Do not turn measured source-quality baselines into public guarantees or
  release floors.
- Do not broaden the README into a full tutorial, CLI reference, architecture
  page, or release note. Link to those pages instead.

## Verification Approach

This is a public-docs entrypoint slice:

- grep `README.md` for project overview, source-build, docs wayfinding,
  release-install honesty, Make entrypoints, and `./bin/pandapi-*` commands;
- if `docs/index.md` is created, verify it links the public docs suite and
  avoids project-management vocabulary;
- verify every README public-docs link points to an existing file;
- run `make readme-verbatim`;
- run `make smoke` and `make smoke-negative`;
- run `make help` if the README names Make targets beyond the existing
  `readme-verbatim`, smoke, test, and strict-mode path;
- run `git diff --check` and `git diff --cached --check`;
- verify public README/docs prose does not teach inherited command names as a
  supported 0.3.0 surface;
- verify public README/docs prose does not claim unpublished 0.3.0 assets,
  checksums, manifests, or wolong fetch/install proof;
- verify changed paths stay inside `README.md`, optional `docs/index.md`, and
  Arc08/project design planning surfaces unless the ledger is amended.

Aggregate `make check` is not required for this docs-entrypoint slice unless
code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces change. If it is not run, the closing report must
state which constituent checks were run and why aggregate `make check` was
narrower than the slice risk.

## Exit Criteria

- `README.md` is a short, accurate public entry point into the 0.3.0 docs
  suite.
- The README clearly distinguishes currently published release examples from
  0.3.0 release-preparation proof still owned by Arc09.
- The README source-build path uses `make readme-verbatim` and local
  `./bin/pandapi-*` commands.
- The README links readers to the tutorial sequence, managed-process guide,
  CLI reference, migration guide, and architecture page.
- Optional `docs/index.md` is either created and verified, or explicitly
  deferred with a reason in the closing report.
- The slice preserves `make readme-verbatim` and smoke behavior.
- Public README/docs prose avoids internal project-management vocabulary.
- Public README/docs prose does not revive inherited command-name
  compatibility.
- The slice stays out of release publication, CI, Make, tests/tools/fixtures,
  implementation source, and dependency-vendoring scope unless the ledger is
  amended.
- The closing report walks every ledger row and bubbles up what Slice10 must
  verify for final Arc08 composition.
