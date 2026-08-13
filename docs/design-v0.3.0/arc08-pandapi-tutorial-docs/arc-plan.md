# Arc08 Plan: pandapi-tutorial-docs

Status: active; Slice02 htn-hddl-onboarding closed and CDC-verified; Slice03 next
Opened: 2026-08-13

## 1. Capability

Arc08 creates the pandaPI 0.3.0 user-learning and documentation suite. The
docs must bring a reader from first contact with HTN/PDDL/HDDL concepts to
running the canonical `pandapi-*` tools, understanding the new managed-process
contract, and knowing where source-quality, dependency, architecture, and
migration boundaries sit.

This is not release publication. Arc09 owns package assets, checksums,
publication, final release notes, license/NOTICE release proof, and wolong
fetch/install verification. Arc08 owns the human-facing explanation that makes
those release assets understandable once Arc09 proves and publishes them.

## 2. Documentation Principles

1. **Teach the supported path first.** The first reader journey is
   `pandapi-parser` -> `pandapi-grounder` -> `pandapi-engine`, using canonical
   command names and the supported normal surfaces.
2. **Explain the concepts before the flags.** HTN, PDDL, HDDL, parse artifact,
   grounded artifact, plan artifact, status, and no-plan outcomes need plain
   language before command-reference density appears.
3. **Show real command flows.** Tutorial commands must run against checked-in
   fixtures or against examples added by an Arc08 slice. Examples that cannot
   be executed locally are not tutorial proof.
4. **Separate human CLI and supervised use.** CLI docs should be comfortable
   for a terminal user; managed-process docs should teach stdout, stderr,
   exit/status, `--supervised`, `--status`, and `PANDAPI_STATUS` without asking
   a supervisor to scrape human prose.
5. **Reflect the final 0.3.0 break.** Public docs must not teach inherited
   `pandaPI*` command-name compatibility as a 0.3.0 guarantee. Migration prose
   should explain that 0.2.0 was the transition release and 0.3.0 uses the new
   `pandapi-*` interface.
6. **Tell the truth about quality posture.** Arc07 source-quality baselines,
   source classes, generated-code policy, and dependency boundaries should be
   explained as engineering posture, not exaggerated into public claims of
   global zero warnings or coverage floors.
7. **Make the docs beautiful by being kind to attention.** Pages should use
   crisp introductions, small working examples, precise headings, command
   blocks a reader can run, and links that let advanced readers go deeper
   without interrupting the beginner path.

## 3. Slice Breakdown

| Slice | Slug | Scope | Load-bearing for |
|-------|------|-------|------------------|
| slice01 | `docs-information-architecture` | Create the documentation blueprint: audience journeys, public docs map, source/evidence inputs, example strategy, voice/style rules, status of stale compatibility wording, and acceptance criteria for later writing slices. No README or release docs edits yet. | all Arc08 writing |
| slice02 | `htn-hddl-onboarding` | Write the beginner concept chapter covering HTN planning, PDDL/HDDL roles, pandaPI's three-stage pipeline, artifacts, and the minimal checked-in example. | workflow tutorial |
| slice03 | `first-project-workflow` | Write the simple hands-on workflow: parse, ground, solve, inspect artifacts, rerun, and understand solved versus no-plan outcomes using checked-in fixtures. | CLI reference, README |
| slice04 | `managed-process-workflow` | Write the supervised/process-manager workflow: `--supervised`, `--status`, stdout/stderr ownership, final `PANDAPI_STATUS`, exit/status taxonomy, pipeline composition, and negative outcomes. | migration and wolong docs |
| slice05 | `cli-reference-and-migration` | Write the command reference and behavior-change/migration table for canonical `pandapi-*` names, common options, statuses, unsupported/legacy/future surfaces, and 0.2.0 -> 0.3.0 migration. | README and release prep |
| slice06 | `architecture-and-source-quality-docs` | Write architecture, dependency, source-quality, generated-code, and third-party-boundary docs from Arc02 and Arc07 without turning measured baselines into public guarantees. | README and release prep |
| slice07 | `readme-entrypoint-refresh` | Update top-level README as the public entry point into the tutorial/docs suite, keeping release-install wording honest about Arc09-owned publication proof and preserving `make readme-verbatim`. | arc synthesis |
| slice08 | `docs-synthesis` | Verify the docs suite composes, links are valid, examples run, README points to the right material, Arc09 handoffs are explicit, and Arc08 can close. | Arc09 release prep |

Detailed slice plans after Slice01 may change this breakdown if the
documentation blueprint surfaces a better order, missing page, or example
gap. Any change must update this plan and its Version History before the next
slice is opened.

## 4. Dependencies And Inputs

Consumes:

- Arc03 managed-process contract, status/exit taxonomy, stdio/event/TTY
  contract, supported-surface classification, CLI naming/version/migration
  evidence, and contract-test matrix.
- Arc05 canonical native `pandapi-*` binary adoption and no-shim policy.
- Arc06 fixture, smoke, coverage, sanitizer, static-analysis, warning, and
  CI/local gate evidence.
- Arc07 source-quality synthesis, source-class policy, generated-code policy,
  dependency boundaries, and Arc08 handoff.
- Current checked-in fixtures under `fixtures/`, especially `minimal`,
  `unsolvable`, `broken-syntax`, `broken-reference`, and component artifacts.
- Current Make-backed build/test entrypoints.

Leaves for Arc09:

- Public docs paths that Arc09 release assets and release notes can link to.
- A behavior-change and migration table that Arc09 can verify against package
  contents and wolong fetch/install/migration proof.
- Explicit source-quality release-gate wording that does not invent thresholds
  Arc07 did not land.

## 5. Current Status

- **slice01 docs-information-architecture - closed and CDC-verified.** Slice
  set:
  [`slice01-docs-information-architecture/slice-doc.md`](slice01-docs-information-architecture/slice-doc.md),
  [`slice01-docs-information-architecture/ledger.md`](slice01-docs-information-architecture/ledger.md),
  [`slice01-docs-information-architecture/cc-prompt.md`](slice01-docs-information-architecture/cc-prompt.md),
  [`documentation-blueprint.md`](documentation-blueprint.md),
  [`slice01-docs-information-architecture/closing-report.md`](slice01-docs-information-architecture/closing-report.md),
  [`slice01-docs-information-architecture/cdc-verification.md`](slice01-docs-information-architecture/cdc-verification.md).
  The blueprint now records the operator correction: public project docs live
  under `docs/`, tutorial pages live under `docs/tutorial/`, and design
  evidence remains under `docs/design-v0.3.0/`.
- **slice02 htn-hddl-onboarding - closed and CDC-verified.** Slice set:
  [`slice02-htn-hddl-onboarding/slice-doc.md`](slice02-htn-hddl-onboarding/slice-doc.md),
  [`slice02-htn-hddl-onboarding/ledger.md`](slice02-htn-hddl-onboarding/ledger.md),
  [`slice02-htn-hddl-onboarding/cc-prompt.md`](slice02-htn-hddl-onboarding/cc-prompt.md),
  [`../../tutorial/htn-hddl-onboarding.md`](../../tutorial/htn-hddl-onboarding.md),
  [`slice02-htn-hddl-onboarding/closing-report.md`](slice02-htn-hddl-onboarding/closing-report.md),
  [`slice02-htn-hddl-onboarding/cdc-verification.md`](slice02-htn-hddl-onboarding/cdc-verification.md).
  This slice writes
  `docs/tutorial/htn-hddl-onboarding.md`, teaches beginner
  HTN/PDDL/HDDL concepts through `fixtures/minimal`, verifies the first parser
  command, and resolves that the minimal fixture is enough before Slice03
  writes the full workflow.

## 6. Planned Documentation Surface

Arc08 may create or edit:

| Surface | Intended ownership |
|---------|--------------------|
| `docs/` | User-facing tutorial, concept, workflow, CLI, architecture, migration, and source-quality docs. |
| `README.md` | Public entry point, but only in the README slice after the docs suite has stable anchors. |
| `fixtures/` | Example fixtures only when a docs slice needs a checked-in runnable example not already present. |
| `Makefile`, `mk/` | Documentation verification targets only if a slice needs a reusable Make-backed docs check. |
| `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/` | Arc08 plans, docs blueprint, ledgers, reports, and close evidence. |

Arc08 should not edit release package assets, publish workflows, checksums,
release manifests, license bundles, wolong integration code, parser/grounder/
engine/runtime implementation source, or source-quality thresholds unless a
slice ledger is explicitly amended and the operator accepts the scope change.

## 7. Arc Ledger

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Arc08 defines and lands a coherent public documentation architecture, including audience journeys, page map, example policy, style/voice rules, and source/evidence inputs. | reproduced |
| A2 | Beginner concept docs explain HTN/PDDL/HDDL, the pandaPI three-stage pipeline, artifacts, solved/no-plan outcomes, and where the checked-in examples fit. | reproduced |
| A3 | Hands-on CLI docs teach a runnable parse-ground-solve workflow using canonical `pandapi-*` commands and checked-in or slice-added fixtures. | reproduced |
| A4 | Managed-process docs teach `--supervised`, `--status`, stdout/stderr ownership, `PANDAPI_STATUS`, exit/status taxonomy, ANSI/color behavior, and negative outcomes without prose scraping. | reproduced |
| A5 | CLI reference and migration docs reflect the final 0.3.0 no-compatibility policy for inherited `pandaPI*` command names while preserving historical 0.2.0 transition context accurately. | reproduced |
| A6 | Architecture/dependency/source-quality docs explain source classes, generated-code policy, third-party boundaries, accepted dependencies, and remaining budgets without overclaiming coverage floors or zero-warning status. | reproduced |
| A7 | Top-level README becomes a clear entry point into the 0.3.0 docs suite and continues to pass `make readme-verbatim`. | reproduced |
| A8 | Every tutorial command intended to be runnable is verified through Make-backed or direct local commands, and non-runnable conceptual examples are clearly marked as conceptual. | reproduced |
| A9 | Arc08 stays out of Arc09 release-publication scope while handing Arc09 concrete docs, migration, release-note, and wolong verification inputs. | reproduced |
| A10 | The docs suite composes without silent drops: all planned pages exist, internal links resolve where locally checkable, examples are consistent, and the arc closing report walks the delivered suite against A1-A10. | reproduced |

## 8. Open Questions And Risks

- **OQ1 - docs home resolved.** Slice01 accepted `docs/`
  as the public docs home. Public docs stay outside the design-planning tree,
  while design evidence remains under this arc.
- **OQ2 - example depth routed.** Existing fixtures are minimal and
  contract-focused. Slice02 and Slice03 must explicitly decide whether those
  fixtures are enough before adding new checked-in examples.
- **OQ3 - stale compatibility wording.** Earlier Arc03 documents still contain
  pre-override compatibility language. Arc08 public docs must follow the later
  project-plan/operator decision: 0.3.0 is a breaking interface release with
  canonical `pandapi-*` names.
- **OQ4 - README timing.** README should not be refreshed until stable docs
  anchors exist, otherwise `make readme-verbatim` can lock in unfinished
  links.
- **OQ5 - wolong boundary.** Arc08 can write migration guidance, but Arc09
  verifies wolong fetch/install/migration against release assets.

## 9. Version History

- **v1.5 - 2026-08-13.** Corrected the public docs home and marked Slice02
  htn-hddl-onboarding closed and CDC-verified. Surfaced by: operator
  correction plus Slice02 CDC verification. Why: public project docs belong
  under `docs/`, tutorial pages belong under `docs/tutorial/`, and
  `docs/design-v0.3.0/` is reserved for planning, design, implementation
  tracking, ledgers, and verification artifacts.
- **v1.4 - 2026-08-13.** Marked Slice02 htn-hddl-onboarding CC proposed
  done. Surfaced by: public onboarding page, Slice02 ledger, and Slice02
  closing report. Why: the beginner concept page now teaches HTN/PDDL/HDDL,
  maps `fixtures/minimal` to domain/problem concepts, verifies one parser
  command, and records that no new beginner fixture is needed before Slice03.
- **v1.3 - 2026-08-13.** Opened Slice02 htn-hddl-onboarding. Surfaced by:
  Slice01 CDC verification and the accepted documentation blueprint. Why:
  Arc08 can now begin public docs writing with the beginner concept page,
  using `fixtures/minimal`, canonical `pandapi-*` names, and an explicit
  fixture-adequacy decision before Slice03's workflow tutorial.
- **v1.2 - 2026-08-13.** Marked Slice01 docs-information-architecture closed
  and CDC-verified. Surfaced by: Slice01 CDC verification. Why: CDC reproduced
  the F-1 through F-13 ledger evidence, accepted `docs/` as the
  public docs home, accepted the no-breakdown-change recommendation, and routed
  fixture adequacy plus migration-table details to later slices.
- **v1.1 - 2026-08-13.** Marked Slice01 docs-information-architecture CC
  proposed done. Surfaced by: `documentation-blueprint.md` and Slice01 closing
  report. Why: the public docs home, audience journeys, page map, example
  strategy, evidence inputs, command-name policy, voice/style rules, and later
  slice routing are now recorded; no slice breakdown change is recommended.
- **v1.0 - 2026-08-13.** Opened Arc08 with Slice01
  docs-information-architecture. Surfaced by: Arc07 CDC-verified close and
  project-plan roadmap. Why: source-quality and managed-process evidence now
  compose, so user-facing tutorial/docs work can begin from verified inputs.
