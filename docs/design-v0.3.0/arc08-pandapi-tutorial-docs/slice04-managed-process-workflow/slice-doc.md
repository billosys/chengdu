# Arc08 Slice04: managed-process-workflow

Status: open
Opened: 2026-08-13

## Goal

Write the public supervised/process-manager integration guide:

- `docs/managed-process.md`

This page should teach how another process can run the canonical
`pandapi-*` commands, classify outcomes from exit codes and final
`PANDAPI_STATUS` records, and preserve stdout/stderr ownership without
scraping human diagnostic prose. It closes Arc08 A4 and gives Slice05,
README, and Arc09/wolong migration work a verified managed-process guide to
point at.

## In Scope

- Create `docs/managed-process.md`.
- Use the accepted public docs layout:
  - public project docs under `docs/`;
  - tutorial pages under `docs/tutorial/`;
  - planning/evidence artifacts under `docs/design-v0.3.0/`.
- Use canonical local binaries through `make build` and `./bin/pandapi-*`
  examples.
- Teach the managed-process mode and options:
  - `--supervised`;
  - `--status`;
  - `--status=stderr`;
  - `--status=stdout`;
  - stdout-conflict behavior when stdout is already the artifact or
    informational stream.
- Teach stream ownership:
  - stdout is the selected artifact/status/info stream or empty;
  - stderr is human diagnostics, progress, warnings, statistics, and optional
    final status;
  - supervisors classify from exit code and tagged status fields, not human
    prose.
- Teach final `PANDAPI_STATUS` records, required fields, and stable parsing
  expectations.
- Teach the common status/exit classes needed by integrators:
  `ok`, `domain_no_plan`, `cli_usage_error`, `input_unavailable`,
  `output_unavailable`, `input_invalid`, surface-policy statuses,
  `unsupported_feature`, timeout/resource/interrupted, dependency/child
  failures, `internal_error`, and `signal_terminated`.
- Teach ANSI/color behavior for supervised output, including `--no-color`,
  `--no-colour`, `NO_COLOR`, TTY-aware defaults, and no ANSI in machine-owned
  streams.
- Include runnable examples for:
  - one successful supervised parser command;
  - one supervised parse -> ground -> solve pipeline;
  - one valid no-plan outcome;
  - one invalid-input or output-unavailable negative outcome;
  - one stdout/status conflict or stdout artifact/status ownership case.
- Use checked-in fixtures only unless the ledger is explicitly amended.
- Keep public prose free of Arc08, Slice04, CC, CDC, ledger, and
  closing-report vocabulary.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` to mark
  Slice04 CC proposed done when the slice closes.

## Out Of Scope

- Do not rewrite the beginner onboarding or first workflow tutorials.
- Do not write the full CLI reference, behavior-change table, migration page,
  architecture/source-quality docs, README refresh, release notes, or docs
  synthesis.
- Do not create the public docs index unless the ledger is explicitly amended.
- Do not edit release assets, checksums, manifests, publish workflows, license
  bundles, wolong integration, CI, Make targets, tests, tools, fixtures, or
  implementation source unless a ledger row is amended and the operator accepts
  the scope change.
- Do not teach inherited `pandaPI*` command names as a 0.3.0 compatibility
  surface. Older Arc03 compatibility wording is historical input superseded by
  the later 0.3.0 no-compatibility decision.
- Do not include speculative supervisor library code. Shell command blocks and
  expected status predicates are enough unless a verified example program is
  explicitly accepted into scope.
- Do not require readers to parse or regex-match human diagnostic prose.

## Verification Approach

This is a public-docs writing slice with runnable managed-process commands:

- grep the public page for supervised mode, status options, stream ownership,
  `PANDAPI_STATUS`, required fields, status/exit taxonomy, ANSI/color policy,
  and negative outcomes;
- run the documented successful supervised parser command, or an equivalent
  extracted sequence, and verify the artifact, empty stdout, and final status;
- run the documented supervised pipeline command, or an equivalent extracted
  sequence, and verify parser, grounder, and engine artifacts/statuses;
- run the documented no-plan command, or an equivalent extracted sequence, and
  verify exit `2` and `domain_no_plan`;
- run one documented negative command, or an equivalent extracted sequence,
  and verify the status class without relying on diagnostic prose;
- run the managed contract fixture gates:
  - `make test-contract-parser-managed`;
  - `make test-contract-grounder-managed`;
  - `make test-contract-engine-managed`;
  - `make test-contract-pipeline-managed`;
- verify the public page does not mention internal Arc08/CC/CDC/ledger
  mechanics;
- run `git diff --check` and `git diff --cached --check`;
- verify the changed paths stay inside the public docs page and Arc08/project
  design-planning surfaces unless the ledger is amended.

Aggregate `make check` is not required for this docs-writing slice unless
code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces change. If it is not run, the closing report must
state which constituent checks were run and why aggregate `make check` was
narrower than the slice risk.

## Exit Criteria

- `docs/managed-process.md` exists.
- The page teaches supervised/process-manager usage with `--supervised`,
  `--status`, `--status=stderr`, and `--status=stdout`.
- The page documents stdout/stderr ownership and tells integrators not to
  classify outcomes from human diagnostic prose.
- The page documents final `PANDAPI_STATUS` records and the required fields a
  supervisor can parse.
- The page documents common status/exit classes, including solved,
  no-plan, usage error, unavailable input/output, invalid input, surface
  policy, unsupported feature, timeout/resource/interruption, dependency/child
  failure, internal error, and signal termination.
- The page documents ANSI/color behavior for supervised/machine-owned output.
- The page includes and verifies runnable positive, pipeline, no-plan,
  negative, and stream-conflict examples using `./bin/pandapi-*`.
- The page uses only canonical `pandapi-*` command names and does not teach
  old `pandaPI*` command compatibility.
- The slice stays out of README, release-publication, CLI-reference,
  migration, architecture/source-quality, CI, Make, tests/tools/fixtures, and
  implementation scope unless the ledger is amended.
- The closing report walks every ledger row and bubbles up whether Slice05,
  README, or Arc09 need any managed-process handoff adjustment.
