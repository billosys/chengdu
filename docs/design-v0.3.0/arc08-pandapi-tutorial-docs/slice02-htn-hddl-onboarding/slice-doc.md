# Arc08 Slice02: htn-hddl-onboarding

Status: open
Opened: 2026-08-13

## Goal

Write the beginner concept chapter for the pandaPI 0.3.0 documentation suite:

- `docs/tutorial/htn-hddl-onboarding.md`

The page should help a reader who is new to HTN planning, PDDL/HDDL, or
pandaPI understand the shape of the problem before the workflow tutorial asks
them to run the full parser -> grounder -> engine chain.

This slice closes Arc08 A2 and prepares Slice03's hands-on workflow. It should
teach concepts with the checked-in minimal fixture and only enough command
surface to make the parser/artifact relationship concrete.

## In Scope

- Create `docs/tutorial/htn-hddl-onboarding.md`.
- Explain HTN planning in plain language:
  - tasks describe what needs to be achieved;
  - methods decompose compound tasks into smaller work;
  - actions are primitive executable steps;
  - ordered subtasks constrain the plan shape.
- Explain how HDDL/PDDL-style domain and problem files relate:
  - the domain describes reusable task/action/method rules;
  - the problem names the instance, objects, initial state, and top-level HTN
    task network.
- Use `fixtures/minimal/domain.hddl` and `fixtures/minimal/problem.hddl` as
  the beginner example, including a short walkthrough of the `project-work`
  domain, `ship-the-spec` problem, `produce`, `draft`, and `review` concepts.
- Decide explicitly whether the minimal fixture is enough for beginner
  onboarding. If it is enough, say so in the closing report. If it is not
  enough, add the smallest checked-in beginner fixture required and verify it.
- Explain the three-stage pandaPI pipeline at concept level:
  - `pandapi-parser` turns HDDL input into a parser artifact (`.htn`);
  - `pandapi-grounder` turns the parser artifact into a grounded planner
    artifact (`.sas`);
  - `pandapi-engine` searches the grounded artifact and writes a plan or
    reports a no-plan outcome.
- Include one runnable parser command, using the canonical `pandapi-parser`
  name, to produce a `.htn` artifact from `fixtures/minimal`. Leave the full
  parse-ground-solve workflow to Slice03.
- Explain solved versus no-plan outcomes at beginner level without teaching
  the full managed-process status taxonomy.
- Keep the public page free of internal planning vocabulary such as Arc08,
  Slice02, CC, CDC, ledgers, and closing reports.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` to mark
  Slice02 CC proposed done when the slice closes.

## Out Of Scope

- Do not write the full CLI workflow page; Slice03 owns
  `docs/tutorial/first-project-workflow.md`.
- Do not write the managed-process guide, CLI reference, migration page,
  architecture/source-quality docs, README refresh, release notes, or docs
  synthesis.
- Do not create the public docs index unless the ledger is explicitly amended.
  Slice07 or Slice08 owns the index once stable anchors exist.
- Do not edit release assets, checksums, manifests, publish workflows, license
  bundles, wolong integration, CI, Make targets, or implementation source.
- Do not teach inherited `pandaPI*` command names as a 0.3.0 compatibility
  surface.
- Do not include command blocks that are not runnable locally. If a snippet is
  conceptual rather than runnable, mark it as conceptual.
- Do not overclaim quality posture, coverage, warnings, or release readiness.

## Verification Approach

This is a public-docs writing slice with one runnable command example:

- grep the public page for HTN, HDDL/PDDL, domain/problem, fixture walkthrough,
  pipeline/artifact, solved/no-plan, and canonical command-name coverage;
- run the documented parser command or an equivalent command extracted from
  the page to prove the `.htn` artifact can be produced from
  `fixtures/minimal`;
- run `make test-contract-parser-managed` after any parser command wording is
  settled;
- verify the public page does not mention internal Arc08/CC/CDC/ledger
  mechanics;
- run `git diff --check` and `git diff --cached --check`;
- verify the changed paths stay inside the public docs page and Arc08/project
  design-planning surfaces unless the ledger is amended.

Aggregate `make check` is not required for this docs-writing slice unless code,
Make, CI, fixtures, or release/package surfaces change. If it is not run, the
closing report must state which constituent checks were run and why aggregate
`make check` was narrower than the slice risk.

## Exit Criteria

- `docs/tutorial/htn-hddl-onboarding.md` exists.
- The page explains HTN tasks, methods, actions, HDDL/PDDL domain/problem
  roles, and the pandaPI three-stage artifact pipeline.
- The page uses the checked-in `fixtures/minimal` files as the beginner
  example and records whether a new beginner fixture was needed.
- Any runnable command in the page is verified locally.
- The page uses only canonical `pandapi-*` command names and does not teach old
  `pandaPI*` command compatibility.
- The slice stays out of README, release-publication, managed-process,
  architecture/source-quality, CI, Make, and implementation scope.
- The closing report walks every ledger row and bubbles up whether Slice03
  needs a fixture, page-map, or workflow adjustment.
