# Arc08 Docs Synthesis

Status: implementation proposed done
Date: 2026-08-14

## Composition Verdict

The pandaPI 0.3.0 documentation suite now composes as one public reader
surface:

- `README.md` introduces chengdu, the in-tree pandaPI product shape, source
  build path, published v0.2.0 install example, and public docs wayfinding.
- `docs/index.md` is the public docs landing page.
- `docs/tutorial/01-htn-hddl-onboarding.md` introduces HTN, PDDL/HDDL,
  domain/problem roles, pipeline artifacts, solved outcomes, and no-plan
  language.
- `docs/tutorial/02-first-project-workflow.md` teaches a runnable
  parse -> ground -> solve workflow with checked-in fixtures.
- `docs/tutorial/03-model-a-feature-as-htn.md` teaches the pre-HDDL modelling
  bridge from a software feature request to an HTN model.
- `docs/tutorial/04-write-hddl-files.md` turns the feature model into
  temporary `domain.hddl` and `problem.hddl` files and verifies parse,
  ground, solve, no-plan, and invalid-input examples.
- `docs/managed-process.md` teaches supervised process integration,
  stdout/stderr ownership, exit/status classification, final
  `PANDAPI_STATUS`, and negative outcomes.
- `docs/reference/cli.md` records the supported CLI surface and common
  options for `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`.
- `docs/migration.md` records the 0.2.0 -> 0.3.0 behavior changes and
  command-name migration.
- `docs/architecture.md` explains the product shape, source-quality posture,
  generated-code policy, dependency boundaries, Make-backed evidence anchors,
  measured baselines, and release-preparation boundary.

## Reader Journeys

| Journey | Entry point | Outcome |
|---------|-------------|---------|
| Beginner concept learner | Tutorial 01 | Understand HTN/PDDL/HDDL concepts, domain/problem input, pipeline artifacts, solved, and no-plan outcomes. |
| CLI workflow user | Tutorial 02 | Build local binaries, run `./bin/pandapi-parser`, `./bin/pandapi-grounder`, and `./bin/pandapi-engine` against checked-in fixtures, inspect artifacts, and compare happy path with no-plan. |
| HTN modelling learner | Tutorial 03 | Turn a prose feature request into objects, facts, compound tasks, primitive actions, methods, ordering constraints, and solved/no-plan boundaries. |
| HDDL author | Tutorial 04 | Write runnable HDDL files in a temporary workspace and validate them through the local pipeline. |
| Managed-process integrator | Managed Process Integration | Classify supervised executions from exit code and final tagged status fields, not human diagnostic prose. |
| CLI/reference reader | CLI Reference and Migration | Use canonical commands, common options, supported surfaces, status/exit summaries, and migration rules. |
| Maintainer/release reader | Architecture and Source Quality | Understand product structure, source classes, dependency posture, quality gates, measured baselines, and release-preparation handoff. |

## Runnable Examples and Verification Gates

Runnable public examples use repository-root commands and local source-build
binaries under `./bin/`. The suite relies on these example classes:

- `fixtures/minimal` for beginner parsing and happy-path parse -> ground ->
  solve.
- `fixtures/unsolvable` for valid no-plan behavior and exit `2` /
  `domain_no_plan`.
- `fixtures/broken-syntax` and `fixtures/broken-reference` through smoke and
  managed fixtures for invalid-input outcomes.
- Temporary HDDL files in Tutorial 04 for authoring practice without adding a
  new checked-in fixture.

Slice10 verification reruns the composition gates:

- public page existence and link resolution;
- public no-internal-vocabulary checks;
- inherited-name fences;
- `make readme-verbatim`;
- `make smoke && make smoke-negative`;
- `make test-contract-pipeline-managed`;
- live `--help`, `--version`, and `--provenance` probes for all three
  `./bin/pandapi-*` commands;
- `make safety-checks && make actionlint`;
- protected-boundary and whitespace checks.

## Public Command Policy

Public 0.3.0 runnable examples use canonical command names:

- `pandapi-parser`
- `pandapi-grounder`
- `pandapi-engine`

Repository-root runnable examples use:

- `./bin/pandapi-parser`
- `./bin/pandapi-grounder`
- `./bin/pandapi-engine`

Inherited `pandaPI*` names appear only in `docs/migration.md` as historical
0.2.0/inherited context and command-rename mapping. They are not a 0.3.0
compatibility guarantee.

## Managed Process

The managed-process docs establish a supervisor-facing contract:

- stdout has exactly one owner: artifact output, informational output, legal
  status output, or empty output;
- stderr owns human diagnostics and default status output;
- final `PANDAPI_STATUS` records carry stable fields;
- supervisors classify from numeric exit code plus status fields;
- diagnostic prose is for humans and is not final contract evidence;
- supervised output keeps machine-owned streams ANSI-free.

This is consistent across the README, docs index, tutorial workflow,
managed-process guide, CLI reference, migration guide, and architecture page.

## Quality and Release-Preparation Posture

The architecture page intentionally describes measured source-quality evidence
as posture and baseline evidence, not as universal quality guarantees. It does
not claim all code is covered, all dependencies are audited, global
zero-warning status, or published 0.3.0 assets.

Release-publication proof remains outside Arc08. The public docs use
release-preparation language for package assets, checksums, manifest and
provenance, license/NOTICE bundles, release notes, test-only dependency
exclusion, wolong fetch/install/migration proof, and publication.

## Arc09 Handoff

Arc09 `release-prep-publication` should consume these public anchors:

- `README.md` for release install/source-build positioning and top-level
  project introduction.
- `docs/index.md` for public docs wayfinding.
- `docs/migration.md` for behavior-change and command-name migration guidance.
- `docs/reference/cli.md` for supported command and option surfaces.
- `docs/managed-process.md` for wolong/supervisor classification behavior.
- `docs/architecture.md` for release-preparation posture, dependency
  boundaries, source-quality baselines, and quality-gate context.

Arc09 must still verify release assets, checksums, manifest/provenance,
license/NOTICE release proof, test-only dependency exclusion, release notes,
wolong fetch/install/migration behavior, source-quality release gates, and
publication idempotency before any v0.3.0 release claim is made.
