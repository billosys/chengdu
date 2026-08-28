# chengdu arc03 / slice04 - cli-naming-version-migration - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Produce the Arc03 CLI naming, version/provenance, help, compatibility, and
migration contract report:

`docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`

This report decides the user-facing command contract that maps the accepted
slice01 supported surface, slice02 status/exit taxonomy, and slice03
stream/status/TTY contract onto concrete CLI behavior. It must decide the
`pandapi-*` naming strategy, compatibility treatment for inherited binary
names, help/version/provenance behavior, option grammar semantics, parse-error
behavior, no-color/no-colour policy, machine-status enablement spelling, and
wolong release-shape implications.

## 2. Scope

**In:**

- Accepted slice01 surface classifications from
  `../supported-surface-classification.md`.
- Accepted slice02 status vocabulary, numeric exit-code table, and payload
  fields from `../status-exit-signal-taxonomy.md`.
- Accepted slice03 stream contract from `../stdio-event-tty-contract.md`,
  including:
  - stdout is one selected artifact sink, tagged status sink, or empty;
  - stderr is human diagnostics/progress/statistics/summary;
  - machine-readable output is tagged text status-only output for 0.3.0;
  - `nlohmann/json` remains held;
  - `fmt` remains behind a future diagnostics/process I/O facade.
- Arc02 dependency gates:
  - CLI11 is accepted only after this slice defines the CLI semantics and
    golden-output compatibility obligations;
  - Catch2/process fixtures remain slice05/Arc04-owned;
  - `tl::expected` remains behind status/result substrate work;
  - reproc++ remains conditional and not justified by CLI naming;
  - Abseil and Boost.Process remain rejected as 0.3.0 foundations.
- Binary naming and migration policy for:
  - `pandapi-parser`;
  - `pandapi-grounder`;
  - `pandapi-engine`;
  - inherited names `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`.
- Compatibility strategy: dual names, wrappers, aliases, symlinks, explicit
  breaking rename, deprecation messaging, and release asset shape.
- Human and supervised CLI option semantics for selected modes from slice03:
  human CLI, supervised/process-manager, and machine-status/tagged-status
  behavior.
- Help, usage, examples, error messages, parse-error classification, and
  version/provenance output, at design level.
- Color controls: `NO_COLOR`, no-color/no-colour compatibility, default auto
  color only for suitable human TTY stderr, and no ANSI in machine/supervised
  output.
- Wolong frozen fetch/install implications, including tarball contents,
  manifest/checksum expectations, install paths, and migration guidance.
- Later-slice inputs for slice05 golden process fixtures and Arc04/Arc05/Arc06
  implementation, adoption, and release documentation.

**Out:**

- No planner source changes, dependency vendoring, build-system edits, scripts,
  workflows, release assets, generated artifacts, `vendor.env`, or `pins.env`.
- No adoption or integration of CLI11; this slice may recommend it behind
  conditions but does not change dependencies.
- No changes to status names, numeric exit codes, tagged status field schema,
  stdout/stderr ownership, buffering, TTY/color rules, or supported surfaces.
- No final contract-test matrix or fixture implementation; slice05 owns tests.
- No implementation order for per-binary migrations beyond routing inputs to
  Arc05.
- No change to wolong's frozen fetch/install shape unless the operator
  explicitly accepts a breaking migration in this design report.

## 3. Required Decisions

The report must decide, at design level:

- canonical binary names for 0.3.0 and the mapping from inherited names to
  `pandapi-*` names;
- whether inherited names remain as aliases/wrappers/symlinks, warn and
  continue, become hard errors, or are removed from release assets;
- whether 0.3.0 is a compatibility transition or a breaking rename for tarball
  contents and installed command paths;
- command synopsis shape for parser, grounder, and engine supported normal
  paths without expanding optional surfaces;
- policy-level option names for human/supervised/machine-status behavior,
  including the final spelling that enables tagged `PANDAPI_STATUS` output;
- final policy for `--color`, `--no-color`, `--no-colour`, and `NO_COLOR`;
- help output requirements and whether help is `ok`/exit `0`;
- version/provenance output requirements and whether version is `ok`/exit `0`;
- CLI parse-error behavior and mapping to `cli_usage_error`/exit `10`;
- how unsupported, legacy, experimental, and future surfaces appear in help,
  error text, hidden options, or compatibility notes;
- whether CLI11 should be adopted in Arc04/Arc05, held, or piloted, and which
  compatibility/golden-output conditions must be met first;
- wolong migration requirements, including old/new command invocation, release
  artifact shape, behavior-change table entries, and consumer verification
  evidence;
- later-slice inputs for slice05 tests and Arc04/Arc05/Arc06 implementation.

## 4. Constraints

- Names and flags must serve the product contract, not the other way around.
  CLI11 is an implementation candidate after behavior is defined.
- Preserve the accepted slice03 machine-status format: tagged text status-only
  output. Do not re-open JSON or `nlohmann/json` unless the report explicitly
  records an accepted event-format change and rationale.
- Preserve stdout ownership. Help/version and human diagnostics must not create
  new machine-data ambiguity.
- Preserve slice01's supported-surface boundary. Do not make legacy,
  experimental, unsupported, or future paths look supported through help text or
  naming convenience.
- Preserve slice02's status/exit semantics. CLI parse errors map to
  `cli_usage_error`/exit `10`.
- Preserve wolong's frozen fetch/install shape unless the report explicitly
  asks for and records an operator-accepted breaking migration.
- Keep the design usable for humans and supervisors: human help must be clear,
  and supervised mode must be deterministic and quiet.

## 5. Verification Approach

CC verifies by producing the CLI naming/version/migration contract report,
updating the ledger, and writing a closing report with a row-by-row walk and
Bubble-up to Arc03. CDC verifies row count, command-name policy, compatibility
strategy, help/version/provenance rules, CLI parse-error mapping, color/status
option semantics, CLI11 dependency gating, wolong migration handling,
later-slice routing, and protected-path cleanliness.

## 6. Exit Criteria

The ledger reaches final status. The report exists at the planned path, decides
the `pandapi-*` naming and inherited-name migration policy, defines help,
version/provenance, CLI parse-error, color, supervised/machine-status, and
compatibility behavior, preserves slice01-03 contracts, gates CLI11 through
golden-output compatibility work, routes test/substrate/adoption/release inputs
to later slices/arcs, and leaves source/build/release paths untouched.
