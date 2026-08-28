# Arc08 Slice05: real-world-htn-modelling-tutorial

Status: open
Opened: 2026-08-13

## Goal

Write the public tutorial chapter:

- `docs/tutorial/03-model-a-feature-as-htn.md`

This page should bridge the reader from prepared fixtures to their own
planning work. It teaches how to turn a prose software feature request into
an HTN model before writing HDDL syntax, and it closes Arc08 A11 at
implementation-proposed-done strength.

## In Scope

- Create `docs/tutorial/03-model-a-feature-as-htn.md`.
- Use the accepted public docs layout:
  - public project docs under `docs/`;
  - tutorial pages under `docs/tutorial/`;
  - planning/evidence artifacts under `docs/design-v0.3.0/`.
- Use a software engineering feature request as the real-world modelling
  example.
- Teach the modelling bridge:
  - planning question and scope;
  - domain versus problem split;
  - objects;
  - predicates/facts;
  - compound tasks;
  - primitive actions;
  - methods;
  - ordering constraints;
  - solved and no-plan boundaries.
- Clearly mark conceptual examples as conceptual and not terminal-runnable.
- Decide whether this modelling chapter needs a new checked-in fixture.
- Update Arc08 `arc-plan.md`, `documentation-blueprint.md`, and
  `docs/design-v0.3.0/project-plan.md` when the slice closes.

## Out Of Scope

- Do not write concrete `domain.hddl` or `problem.hddl` files; Slice06 owns
  HDDL authoring.
- Do not add a fixture unless the modelling page truly needs one and the
  ledger is amended.
- Do not rewrite the onboarding, first workflow, or managed-process pages.
- Do not write the CLI reference, behavior-change table, migration page,
  architecture/source-quality docs, docs index, README refresh, release notes,
  or docs synthesis.
- Do not edit release assets, checksums, manifests, publish workflows, license
  bundles, wolong integration, CI, Make targets, tests, tools, fixtures, or
  implementation source unless a ledger row is amended and the operator
  accepts the scope change.
- Do not teach inherited `pandaPI*` command names as a 0.3.0 compatibility
  surface.
- Do not expose Arc08, Slice05, CC, CDC, ledger, closing-report, or
  project-plan vocabulary in the public page.

## Verification Approach

This is a conceptual public-docs writing slice:

- grep the public page for the real-world software feature example and the
  modelling bridge terms;
- verify the page teaches domain/problem, objects, facts, compound tasks,
  primitive actions, methods, ordering, solved, no-plan, and fixture decision;
- verify the page marks conceptual examples as conceptual;
- verify the page has no runnable shell command blocks;
- verify the public page does not mention inherited command names as
  compatibility surfaces;
- verify the public page does not mention internal planning mechanics;
- verify Arc08/project planning and blueprint updates;
- run `git diff --check` and `git diff --cached --check`;
- verify the changed paths stay inside the public docs page and Arc08/project
  design-planning surfaces unless the ledger is amended.

Aggregate `make check` is not required for this docs-writing slice unless
code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces change. If it is not run, the closing report must
state which constituent checks were run and why aggregate `make check` was
narrower than the slice risk.

## Exit Criteria

- `docs/tutorial/03-model-a-feature-as-htn.md` exists.
- The page starts from a concrete software feature request.
- The page teaches the planning question and explicitly narrows the model's
  scope.
- The page explains the domain/problem split for the example.
- The page identifies objects and facts/predicates.
- The page identifies compound tasks and primitive actions.
- The page explains methods and ordering constraints.
- The page defines solved and no-plan boundaries.
- The page includes a compact pre-HDDL checklist.
- The page clearly states that examples are conceptual and that Slice06 writes
  runnable HDDL files.
- The page decides whether a new fixture is needed for this slice.
- The slice stays out of README, release-publication, CLI-reference,
  migration, architecture/source-quality, CI, Make, tests/tools/fixtures, and
  implementation scope unless the ledger is amended.
- The closing report walks every ledger row and bubbles up whether Slice06,
  README, or Arc09 need any handoff adjustment.
