# CC assignment - chengdu arc03 / slice04 - cli-naming-version-migration

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc03-managed-process-contract/arc-plan.md`
4. `docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/slice-doc.md`
5. `docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/ledger.md`
6. Accepted Arc03 slice01 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/cdc-verification.md`
7. Accepted Arc03 slice02 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/cdc-verification.md`
8. Accepted Arc03 slice03 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/cdc-verification.md`
9. Arc01 close and synthesis:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/closing-report.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
10. Arc02 close and combined recommendations:
   - `docs/design-v0.3.0/arc02-cpp-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
11. Current release/packaging docs as needed for asset-shape implications:
   - `README.md`
   - `release-manifest.txt`
   - scripts under `scripts/`, read-only

## The work

Produce:

`docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`

This is the fourth Arc03 design artifact. It must define how the accepted
process contract appears to users and supervisors at the command line.

The contract must cover:

- canonical `pandapi-*` names for parser, grounder, and engine;
- inherited `pandaPI*` compatibility policy;
- release-asset and install-shape implications, especially wolong compatibility;
- command synopsis and argument semantics for the three supported normal paths;
- option semantics for human CLI, supervised/process-manager, and tagged
  machine-status behavior;
- help, usage, examples, and parse-error behavior;
- version and provenance output;
- color/no-color/no-colour/NO_COLOR behavior;
- treatment of legacy, experimental, unsupported, and future surfaces in help,
  diagnostics, hidden options, and status mapping;
- CLI11's Arc02 disposition after the CLI semantics are defined.

## Decisions you must make

Decide the canonical 0.3.0 command names. The operator wants namespaced
`pandapi-*` names to avoid conflicts. Decide whether inherited names remain as
aliases, wrappers, symlinks, warning compatibility commands, hard errors, or
are removed from release assets.

Decide whether the 0.3.0 release is a compatibility transition or a breaking
rename. If a breaking change is recommended, state the operator decision
required before Arc05/Arc06 can implement it. Preserve wolong's frozen
fetch/install shape unless the report explicitly records an accepted breaking
migration requirement.

Decide final policy-level option spellings for the modes accepted by slice03:
human CLI, supervised/process-manager behavior, and tagged machine-status
output. Include the final spelling for enabling `PANDAPI_STATUS` records.

Decide help/version behavior:

- whether help and version are successful informational commands;
- what stdout/stderr they use;
- what fields version/provenance must include;
- how unsupported or legacy surfaces are described without making them
  supported.

Decide parse-error behavior. Malformed CLI shape must classify as
`cli_usage_error` and exit `10`, and must not continue into input/model
processing.

Decide whether CLI11 should be adopted in Arc04/Arc05, held, or piloted. The
answer must be based on preserving this CLI contract and future golden-output
tests, not on library preference alone.

## Arc02 and prior-slice constraints you must preserve

Arc02 and slice01-03 are load-bearing:

- Supported product surfaces remain parser normal HDDL parse, grounder normal
  `.htn` grounding, and engine normal search.
- Status names and numeric exit codes come from slice02 and must not change.
- Stdout/stderr, tagged status, color, TTY, quiet/progress, and partial-output
  behavior come from slice03 and must not change.
- Machine-readable status output is tagged text status-only output for 0.3.0.
  `nlohmann/json` remains held unless the report explicitly reopens and
  accepts a JSON event-format decision.
- CLI11 is slice04-owned semantically, but implementation adoption belongs to
  Arc04/Arc05 after golden-output constraints are defined.
- fmt remains behind a future diagnostics/process I/O facade.
- Catch2 and process fixtures remain slice05/Arc04-owned.
- reproc++ remains conditional and cannot be justified by CLI naming alone.
- Abseil and Boost.Process remain rejected as 0.3.0 foundation choices.

## Constraints

- Do not edit planner source, dependency source, scripts, workflows, release
  assets, build outputs, `vendor.env`, or `pins.env`.
- Do not adopt dependencies or change build policy.
- Do not change status names, numeric exit codes, tagged status field schema,
  stdout/stderr ownership, buffering, TTY/color rules, or supported surfaces.
- Do not write the final contract test matrix; route test inputs to slice05.
- Do not implement aliases/wrappers or binary renaming; route implementation to
  Arc05 and release documentation to Arc06.
- Preserve wolong's frozen fetch/install shape unless the operator explicitly
  accepts a breaking migration.

## Suggested report shape

1. Executive summary.
2. Evidence base: Arc01, Arc02, slice01, slice02, and slice03.
3. Naming decision and compatibility posture.
4. Release asset and wolong migration implications.
5. Supported command synopsis table.
6. Mode and option semantics.
7. Help, usage, and examples policy.
8. Version/provenance policy.
9. CLI parse-error and status mapping policy.
10. Color/no-color/no-colour and TTY policy.
11. Legacy, experimental, unsupported, and future surface presentation.
12. CLI11 and Arc02 dependency-gating implications.
13. Later slice and arc inputs.
14. Final recommendation.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc03, and leave `cdc-verification.md` for CDC.
