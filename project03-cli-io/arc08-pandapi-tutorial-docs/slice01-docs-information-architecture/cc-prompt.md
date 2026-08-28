# CC Prompt: Arc08 Slice01 docs-information-architecture

You are implementing Arc08 Slice01 for chengdu 0.3.0.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md`
4. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice01-docs-information-architecture/slice-doc.md`
5. `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice01-docs-information-architecture/ledger.md`

Then inspect the source evidence needed for the blueprint:

- `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
- `docs/design-v0.3.0/arc05-binary-contract-adoption/closing-report.md`
- `docs/design-v0.3.0/arc06-ci-and-test-hardening/closing-report.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-synthesis.md`
- `README.md`
- `fixtures/`
- `Makefile` and `mk/help.mk`

## Task

Create:

- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/documentation-blueprint.md`

The blueprint should be the plan for the public docs suite, not the full docs
suite itself. Make it concrete, reader-centered, and useful for the writing
slices that follow.

It must cover:

- audience journeys for beginner concepts, CLI workflow, managed-process
  integration, and maintainer/release architecture posture;
- proposed public docs home and page map;
- evidence/source inputs from Arc03, Arc05, Arc06, Arc07, README, fixtures,
  and Make targets;
- selected existing examples/fixtures and any example gaps for later slices;
- docs voice/style rules;
- 0.3.0 command-name policy:
  - examples use `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`;
  - inherited `pandaPI*` names appear only as historical migration context;
  - do not claim old-name compatibility for 0.3.0;
- routing for README, behavior-change table, managed-process docs,
  architecture/dependency/source-quality docs, and wolong migration guidance;
- any recommended adjustment to the Arc08 slice breakdown.

## Important Policy

Older Arc03 docs contain pre-override compatibility language. Treat that as
historical design input only. The current project plan and operator decision
govern Arc08 public docs: `v0.3.0` is the canonical `pandapi-*` interface and
does not preserve inherited `pandaPI*` command-name compatibility.

Do not edit:

- `README.md`
- `.github/`
- `release/`
- `Makefile` or `mk/`
- `fixtures/`
- `tests/`
- `tools/`
- `pandaPI/`
- `licenses/`

unless you stop and request a ledger amendment first.

## Verification

Run the exact ledger commands for F-1 through F-12, except F-13 which is for
your closing report after you write it.

Also run:

```bash
git diff --check
git diff --cached --check
```

Do not run aggregate `make check` for this docs-planning slice unless you
intentionally decide it is needed. If you skip it, say why.

## Closing Report

Write:

- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice01-docs-information-architecture/closing-report.md`

The closing report must:

- walk F-1 through F-13 row by row;
- include the verification commands you ran;
- include Bubble-up to Arc08:
  - did Slice01 deliver the documentation blueprint?
  - did it change the Arc08 slice breakdown, public docs home, or example
    strategy?
  - what should CDC consider before opening Slice02?

Do not create `cdc-verification.md`; CDC writes that after independent review.
