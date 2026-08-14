# Arc08 Slice07: cli-reference-and-migration

Status: open
Opened: 2026-08-14

## Goal

Write the public reference and migration pages:

- `docs/reference/cli.md`
- `docs/migration.md`

These pages should turn the tutorial path into a concise command reference and
a clear 0.2.0 -> 0.3.0 migration guide. They close Arc08 A5 at
implementation-proposed-done strength and give later README/release work stable
public anchors for command behavior and migration wording.

## In Scope

- Create `docs/reference/cli.md`, creating `docs/reference/` if needed.
- Create `docs/migration.md`.
- Use the accepted public docs layout:
  - public project docs under `docs/`;
  - tutorial pages under `docs/tutorial/`;
  - planning/evidence artifacts under `docs/design-v0.3.0/`.
- Document canonical commands:
  - `pandapi-parser`;
  - `pandapi-grounder`;
  - `pandapi-engine`.
- Document supported normal surfaces:
  - parser HDDL domain/problem parse to `.htn`;
  - grounder `.htn` grounding to `.sas`;
  - engine `.sas` search to plan output.
- Document common options from the current binary help output:
  - `--output PATH|-`;
  - `--status`, `--status=stderr`, `--status=stdout`;
  - `--supervised`;
  - `--quiet`;
  - `--verbose`;
  - `--color=auto|always|never`;
  - `--no-color`;
  - `--no-colour`;
  - `--help`;
  - `--version`;
  - `--provenance`.
- Summarize stdout/stderr/artifact/status ownership and link to the managed
  process guide for process-supervisor detail.
- Summarize status and exit-code classes from the managed-process contract,
  including `ok`, `domain_no_plan`, `cli_usage_error`, `input_unavailable`,
  `output_unavailable`, `input_invalid`, `unsupported_feature`,
  `legacy_surface`, `experimental_surface`, `future_surface`, `timeout`,
  `resource_limit`, `interrupted`, `dependency_failure`,
  `child_process_failure`, `internal_error`, and `signal_terminated`.
- Explain unsupported, legacy, experimental, and future surfaces without
  advertising them as supported 0.3.0 behavior.
- Write a behavior-change and migration table for 0.2.0 -> 0.3.0.
- Make the final 0.3.0 command-name policy explicit:
  - `v0.2.0` is the transition release from the old three-repo/inherited-name
    world into chengdu's monorepo/prebuilt-binary distribution;
  - `v0.3.0` uses the canonical `pandapi-*` command names and managed-process
    behavior;
  - inherited `pandaPI*` command names are not a 0.3.0 compatibility
    guarantee.
- Include runnable examples from the repository root using `./bin/pandapi-*`.
- Link to the existing tutorial sequence and managed-process guide where
  helpful.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` when the
  slice closes.

## Out Of Scope

- Do not update README, docs index, architecture/source-quality docs, release
  notes, release assets, checksums, manifests, publish workflows, license
  bundles, or wolong integration.
- Do not edit CI, Make targets, tests, tools, fixtures, parser/grounder/engine/
  runtime source, source-quality thresholds, or package/provenance logic.
- Do not create new fixtures unless the operator explicitly amends scope.
- Do not re-open the Arc03 compatibility-transition text as public 0.3.0
  policy. It is historical design input superseded by the operator's later
  no-compatibility decision and Arc05 native binary cutover evidence.
- Do not expose Arc08, Slice07, CC, CDC, ledger, closing-report, project-plan,
  or arc-plan vocabulary in public docs.
- Do not make release-install or wolong verification claims. Release-asset
  proof belongs to the release-preparation arc.
- Do not write architecture, dependency, generated-code, source-quality, or
  third-party boundary prose; Slice08 owns that material.

## Verification Approach

This is a public-docs writing slice with live command-reference evidence:

- grep the public pages for the expected headings, command names, options,
  supported surfaces, status/exit entries, and migration table;
- run `make build` before live binary checks unless the binaries are already
  demonstrably current;
- compare the documented command synopsis and common option list against
  `./bin/pandapi-parser --help`, `./bin/pandapi-grounder --help`, and
  `./bin/pandapi-engine --help`;
- run representative help/version/provenance commands for all three binaries;
- run representative tutorial-compatible examples with `./bin/pandapi-*` where
  the docs present command blocks as runnable;
- run `make smoke` and `make smoke-negative`;
- run `make test-contract-pipeline-managed` to preserve parser -> grounder ->
  engine process-contract evidence;
- verify `docs/reference/cli.md` does not document inherited command names as
  supported commands;
- verify `docs/migration.md` mentions inherited command names only in
  historical/mapping/no-compatibility context;
- verify public pages avoid internal planning vocabulary;
- verify Arc08/project planning updates at close;
- run `git diff --check` and `git diff --cached --check`;
- verify changed paths stay inside public docs and Arc08/project design
  planning surfaces unless the ledger is amended.

Aggregate `make check` is not required for this docs-writing slice unless
code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces change. If it is not run, the closing report must
state which constituent checks were run and why aggregate `make check` was
narrower than the slice risk.

## Exit Criteria

- `docs/reference/cli.md` exists.
- `docs/migration.md` exists.
- The CLI reference documents the three canonical commands, supported normal
  surfaces, synopsis, inputs, outputs, artifact behavior, common options,
  status/exit summary, and non-supported surface boundaries.
- The migration guide clearly explains the 0.2.0 -> 0.3.0 command and behavior
  changes, including the no inherited-name compatibility guarantee.
- Public examples use `./bin/pandapi-*` for repository-root commands.
- The public pages link to the tutorial sequence and managed-process guide
  without duplicating those pages wholesale.
- Live `--help` output for parser, grounder, and engine has been checked
  against the reference.
- Smoke, negative smoke, and managed pipeline contract gates pass.
- The slice stays out of README, docs index, architecture/source-quality,
  release-publication, CI, Make, tests/tools/fixtures, and implementation scope
  unless the ledger is amended.
- The closing report walks every ledger row and bubbles up whether Slice08,
  README, or release-preparation handoffs need adjustment.
