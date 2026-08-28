# Arc08 Slice07 CDC Verification: cli-reference-and-migration

Status: closed and CDC-verified
Date: 2026-08-14
Reviewed commit: `f456f89d docs: add Arc08 CLI reference and migration guide`

## Scope Reviewed

CDC reviewed the Slice07 implementation commit and confirmed the intended
surface:

- `docs/reference/cli.md`
- `docs/migration.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice07-cli-reference-and-migration/ledger.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice07-cli-reference-and-migration/closing-report.md`
- Arc08 and project plan status updates

The public docs use canonical `pandapi-*` commands for supported 0.3.0
behavior. Inherited `pandaPI*` names appear only in the migration guide as
historical 0.2.0-to-0.3.0 context and command-name mapping; the CLI reference
does not document them as callable 0.3.0 commands.

## Verification Evidence

CDC reproduced the Slice07 evidence independently:

- Static ledger grep checks for F-1 through F-3, F-5 through F-9, F-12
  through F-20, and F-23 passed.
- `make build` passed and produced `dist/macos-arm64` plus `bin`.
- Live `./bin/pandapi-parser --help`, `--version`, and `--provenance`
  probes passed.
- Live `./bin/pandapi-grounder --help`, `--version`, and `--provenance`
  probes passed.
- Live `./bin/pandapi-engine --help`, `--version`, and `--provenance`
  probes passed.
- `make smoke` passed with `3 passed, 0 failed`.
- `make smoke-negative` passed with `4 passed, 0 failed`.
- `make test-contract-pipeline-managed` passed with `129 passed, 0 failed`.
- `git diff --check` passed before CDC edits.
- Protected unstaged-boundary check passed before CDC edits.
- No pre-existing `cdc-verification.md` file was present for this slice.

Aggregate `make check` was not run for CDC verification because Slice07 is
docs-only and does not change code, Make, CI, tests, release packaging, or
runtime behavior. The reproduced gates cover the slice's documented contract:
public CLI prose, live command examples, managed pipeline behavior, smoke
behavior, status/exit semantics, migration boundary language, and whitespace.

## Findings

No CDC blockers were found.

The CLI reference and migration guide are aligned with the 0.3.0
no-compatibility decision: public supported behavior is documented under the
canonical `pandapi-*` names, with the inherited `pandaPI*` names treated as
historical migration context only.

The migration guide correctly avoids promising release-asset or installer
behavior before Arc09. It points local source-build usage at `make build` and
`./bin/pandapi-*`, while leaving published installation verification to the
release arc.

## Bubble-up

Arc08 A5 is closed at CDC-verified strength for CLI reference and migration
guide documentation.

Required bubble-up changes:

- Mark Slice07 closed and CDC-verified in the Arc08 plan.
- Mark Arc08 Slice07 closed and CDC-verified in the project plan.
- Preserve Slice06 as awaiting CDC verification.
- Preserve Slice08 architecture-and-source-quality-docs as the next planned
  Arc08 slice.
