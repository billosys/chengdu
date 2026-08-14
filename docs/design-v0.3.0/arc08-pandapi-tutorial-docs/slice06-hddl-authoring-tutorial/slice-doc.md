# Arc08 Slice06: hddl-authoring-tutorial

Status: open
Opened: 2026-08-14

## Goal

Write the public tutorial chapter:

- `docs/tutorial/04-write-hddl-files.md`

This page should turn the Slice05 `status-summary` HTN model into concrete
`domain.hddl` and `problem.hddl` files, verify them through the canonical
`pandapi-*` workflow, and teach common authoring mistakes. It closes Arc08
A12 at implementation-proposed-done strength.

## In Scope

- Create `docs/tutorial/04-write-hddl-files.md`.
- Use the accepted public docs layout:
  - public project docs under `docs/`;
  - tutorial pages under `docs/tutorial/`;
  - planning/evidence artifacts under `docs/design-v0.3.0/`.
- Reuse the Slice05 `status-summary` feature-shipping model.
- Teach concrete HDDL domain sections:
  - `:types`;
  - `:predicates`;
  - `:task`;
  - `:method`;
  - `:action`;
  - preconditions and effects.
- Teach concrete problem sections:
  - `:domain`;
  - `:objects`;
  - `:htn`;
  - `:init`.
- Include runnable commands that create the HDDL files in a temporary
  workspace from the repository root.
- Verify parse -> ground -> solve with `./bin/pandapi-parser`,
  `./bin/pandapi-grounder`, and `./bin/pandapi-engine`.
- Include verified authoring mistakes:
  - a valid no-plan case caused by a missing initial fact;
  - an invalid-input case caused by broken HDDL syntax.
- Decide whether a new checked-in fixture is needed.
- Update Arc08 `arc-plan.md`, `documentation-blueprint.md`, and
  `docs/design-v0.3.0/project-plan.md` when the slice closes.

## Out Of Scope

- Do not write the CLI reference, behavior-change table, migration page,
  architecture/source-quality docs, docs index, README refresh, release notes,
  or docs synthesis.
- Do not edit release assets, checksums, manifests, publish workflows, license
  bundles, wolong integration, CI, Make targets, tests, tools, parser/
  grounder/engine/runtime source, or source-quality thresholds.
- Do not add a checked-in fixture unless the tutorial truly needs one and the
  ledger is amended.
- Do not teach inherited `pandaPI*` command names as a 0.3.0 compatibility
  surface.
- Do not expose Arc08, Slice06, CC, CDC, ledger, closing-report, or
  project-plan vocabulary in the public page.

## Verification Approach

This is a public-docs writing slice with runnable tutorial commands:

- grep the public page for domain/problem authoring sections and Slice05 model
  continuity;
- run the documented happy-path authoring workflow, or an equivalent extracted
  sequence, and verify non-empty `.htn`, `.sas`, and plan artifacts;
- verify the generated plan contains the expected primitive feature-shipping
  actions;
- run the documented missing-initial-fact no-plan example and verify exit `2`
  plus `domain_no_plan`;
- run the documented broken-syntax example and verify exit `22` plus
  `input_invalid`;
- verify the page records the fixture decision;
- verify the public page uses canonical `pandapi-*` command names and avoids
  inherited command-name compatibility;
- verify the public page does not mention internal planning machinery;
- verify Arc08/project planning and blueprint updates;
- run `make test-contract-pipeline-managed` as the Make-backed tutorial
  workflow regression gate;
- run `git diff --check` and `git diff --cached --check`;
- verify changed paths stay inside the public docs page and Arc08/project
  design-planning surfaces unless the ledger is amended.

Aggregate `make check` is not required for this docs-writing slice unless
code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces change. If it is not run, the closing report must
state which constituent checks were run and why aggregate `make check` was
narrower than the slice risk.

## Exit Criteria

- `docs/tutorial/04-write-hddl-files.md` exists.
- The page writes runnable `domain.hddl` and `problem.hddl` files from the
  Slice05 model.
- The page explains domain sections, problem sections, methods, actions,
  predicates, preconditions, effects, and task networks.
- The documented happy path parses, grounds, solves, and produces the
  expected primitive action sequence.
- The page verifies a valid no-plan authoring mistake.
- The page verifies an invalid-input syntax mistake.
- The page decides whether a new checked-in fixture is needed.
- The page uses only canonical `pandapi-*` command names and does not teach
  old `pandaPI*` command compatibility.
- The slice stays out of README, release-publication, CLI-reference,
  migration, architecture/source-quality, CI, Make, tests/tools/fixtures, and
  implementation scope unless the ledger is amended.
- The closing report walks every ledger row and bubbles up whether Slice07,
  README, or Arc09 need any handoff adjustment.
