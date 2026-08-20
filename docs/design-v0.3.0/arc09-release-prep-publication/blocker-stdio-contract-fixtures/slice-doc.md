# Arc09 Blocker Slice: stdio-contract-fixtures

Status: open; blocks Slice01 release-readiness-inventory
Opened: 2026-08-20

## Goal

Turn the stdin contract into durable Make-backed contract fixtures, CI gates,
and public managed-process documentation so future changes cannot regress the
wolong pipeline.

## Scope

In scope:

- Add contract fixtures for accepted parser stdin forms.
- Add contract fixtures for grounder stdin artifact input.
- Add contract fixtures for engine stdin artifact input, including solved and
  no-plan behavior.
- Add full supervised stdin/stdout/stderr pipeline fixtures for solved and
  no-plan chains.
- Add or update Make targets for the fixture set before any CI workflow calls
  them.
- Update current public managed-process and CLI reference docs to describe
  the supported stdin forms and rejected forms.
- Update Arc09 release-readiness inventory expectations to require this gate.

Out of scope:

- No product behavior changes beyond fixture/doc fixes needed to reflect the
  implementation slice.
- No release publication.
- No wolong code changes.
- No `cdc-verification.md`.

## Exit Criteria

- Stdin fixtures prove direct component input and end-to-end supervised
  pipeline behavior.
- CI uses Make targets only.
- Public docs match the implemented contract without overclaiming.
- Previous managed-process, smoke, negative smoke, and release-quality gates
  still pass.
