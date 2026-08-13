# Arc08 Slice03: first-project-workflow

Status: open
Opened: 2026-08-13

## Goal

Write the first hands-on pandaPI workflow tutorial:

- `docs/tutorial/first-project-workflow.md`

This page should take the reader from the Slice02 concepts into a runnable
parse -> ground -> solve workflow using checked-in fixtures and canonical
`pandapi-*` commands. It closes Arc08 A3 and gives later CLI reference,
README, and release-prep docs a verified happy-path workflow to point at.

## In Scope

- Create `docs/tutorial/first-project-workflow.md`.
- Use `fixtures/minimal/domain.hddl` and `fixtures/minimal/problem.hddl` for
  the happy path.
- Build or locate the local platform binaries through documented commands that
  work for macOS arm64 and Linux x86_64.
- Teach the workflow in runnable steps:
  - create a temporary workspace;
  - run `pandapi-parser` to produce a `.htn` artifact;
  - inspect or sanity-check the `.htn` artifact;
  - run `pandapi-grounder` to produce a `.sas` artifact;
  - inspect or sanity-check the `.sas` artifact;
  - run `pandapi-engine` to produce a plan artifact;
  - inspect or sanity-check the plan artifact.
- Show how to rerun the workflow by deleting or replacing generated artifacts
  in the temporary workspace rather than editing checked-in fixtures.
- Use `fixtures/unsolvable` after the happy path to explain a valid no-plan
  outcome at workflow level.
- Decide explicitly whether an intermediate example beyond
  `fixtures/unsolvable` is needed before later docs. If not needed, say so in
  the closing report. If needed, add the smallest checked-in fixture and
  verify it.
- Keep public prose free of Arc08, Slice03, CC, CDC, ledger, and
  closing-report vocabulary.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` to mark
  Slice03 CC proposed done when the slice closes.

## Out Of Scope

- Do not write the beginner HTN/HDDL concept page; Slice02 owns it.
- Do not write the managed-process guide, CLI reference, migration page,
  architecture/source-quality docs, README refresh, release notes, or docs
  synthesis.
- Do not create the public docs index unless the ledger is explicitly amended.
- Do not edit release assets, checksums, manifests, publish workflows, license
  bundles, wolong integration, CI, Make targets, or implementation source.
- Do not teach inherited `pandaPI*` command names as a 0.3.0 compatibility
  surface.
- Do not turn this page into the managed-process contract. It may use stable
  command examples, but Slice04 owns `--supervised`, `PANDAPI_STATUS`,
  stdout/stderr ownership, and status taxonomy details.
- Do not include command blocks that are not runnable locally. If a snippet is
  conceptual rather than runnable, mark it as conceptual.

## Verification Approach

This is a public-docs writing slice with runnable workflow commands:

- grep the public page for the target workflow, fixtures, canonical commands,
  artifact names, rerun guidance, and no-plan comparison;
- run the documented happy-path parse -> ground -> solve command sequence, or
  an equivalent extracted sequence, and verify non-empty generated artifacts;
- run the documented no-plan comparison, or an equivalent extracted sequence,
  and verify the expected no-plan exit/status behavior;
- run `make test-contract-pipeline-managed`;
- run `make smoke` and `make smoke-negative`;
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

- `docs/tutorial/first-project-workflow.md` exists.
- The page teaches a runnable parse -> ground -> solve workflow using
  `fixtures/minimal`.
- The page verifies generated `.htn`, `.sas`, and plan artifacts in a temporary
  workspace.
- The page explains rerun hygiene and a valid no-plan comparison using
  checked-in fixtures.
- Any runnable command in the page is verified locally.
- The page uses only canonical `pandapi-*` command names and does not teach old
  `pandaPI*` command compatibility.
- The slice stays out of README, release-publication, managed-process,
  architecture/source-quality, CI, Make, and implementation scope.
- The closing report walks every ledger row and bubbles up whether Slice04,
  Slice05, README, or Arc09 need any workflow handoff adjustment.
