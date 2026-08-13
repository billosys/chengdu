# Arc08 Slice01: docs-information-architecture

Status: open
Opened: 2026-08-13

## Goal

Create the documentation blueprint for the pandaPI 0.3.0 tutorial/docs suite.
This slice should make the reader journey, page map, example strategy,
evidence sources, stale-compatibility correction, and voice/style rules clear
before later slices write the individual tutorial chapters.

The output is a durable design report:

- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`

The report should be practical enough that Slice02 can start writing beginner
docs without rediscovering scope.

## In Scope

- Define target audiences and reader journeys:
  - first-time HTN/PDDL/HDDL learner;
  - CLI user running the three-stage pipeline;
  - process/supervisor user integrating `pandapi-*`;
  - maintainer or release reader who needs architecture, dependency, and
    source-quality posture.
- Propose the public docs file map, including the preferred public docs home.
- Inventory source evidence from Arc03, Arc05, Arc06, Arc07, README, fixtures,
  and Make targets.
- Select existing examples/fixtures for beginner and intermediate docs, and
  identify any example gaps as later-slice work.
- Define the documentation voice and style rules for clear, warm, precise
  technical prose.
- Record the command-name policy: canonical `pandapi-*` names only for 0.3.0
  public examples; inherited `pandaPI*` names may appear only as historical
  migration context.
- Define which future slices own README, behavior-change table, architecture,
  dependency/source-quality prose, and wolong migration guidance.
- Update Arc08 `arc-plan.md` if the blueprint changes the slice breakdown.
- Update `docs/design-v0.3.0/project-plan.md` to mark Arc08 active if not
  already updated.

## Out Of Scope

- Do not write the full beginner tutorial, CLI reference, managed-process
  guide, README refresh, release notes, or release publication docs.
- Do not edit release assets, checksums, manifests, publish workflows, or
  license bundles.
- Do not change parser, grounder, engine, runtime, fixture-runner, Make, or CI
  behavior except for documentation-only evidence commands if explicitly
  required by the ledger.
- Do not claim inherited `pandaPI*` command-name compatibility for 0.3.0.
- Do not convert Arc07 coverage baselines, warning counts, or analyzer
  baselines into public guarantees or release floors.

## Verification Approach

This is a docs-planning slice. Verification is document-structure and boundary
driven:

- grep the blueprint for required audiences, page map, evidence sources,
  examples, style rules, command-name policy, and future-slice ownership;
- check that the Arc08 plan and project plan point to the open slice;
- run `git diff --check` and `git diff --cached --check`;
- verify no public README, release, CI, implementation, fixture, or Make
  behavior surfaces are changed by this slice unless the ledger is amended.

## Exit Criteria

- `documentation-blueprint.md` exists and is detailed enough for Slice02 to
  write the beginner concept chapter.
- The blueprint names public docs home, audience journeys, page map, example
  strategy, source/evidence inputs, voice/style rules, and stale
  compatibility correction.
- The blueprint routes each major docs responsibility to a later Arc08 slice.
- Arc08 plan and project plan are updated consistently.
- The slice stays within documentation planning scope.
- The closing report walks every ledger row and bubbles up whether Arc08's
  slice breakdown needs adjustment.
