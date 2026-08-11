# chengdu arc05 / slice08 - engine-contract-adoption - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Migrate engine normal search behavior to the accepted managed-process contract
through canonical `pandapi-engine`, retire active inherited `pandaPIengine`
artifacts for 0.3.0, and preserve the already-native parser and grounder
surfaces.

Slice06 made `pandapi-parser` native. Slice07 made `pandapi-grounder` native.
This slice completes the primary binary adoption sequence by making the engine
contract-native under the new command name.

## 2. Scope

**In:**

- Build/install `dist/<platform>/pandapi-engine` as the engine artifact used
  by tests, smoke, tooling, provenance, and package helpers.
- Stop installing `dist/<platform>/pandaPIengine` as an active 0.3.0 engine
  artifact.
- Migrate supported normal engine search to the Arc03 process contract for
  solved and no-plan outcomes.
- Add engine contract fixture records and runner support for canonical
  `pandapi-engine`.
- Support accepted common options needed by this slice, including
  `--output PATH|-`, `--supervised`, `--status`, `--status=stderr`,
  `--status=stdout` where legal, `--quiet`, `--verbose`, `--color=auto`,
  `--color=always`, `--color=never`, `--no-color`, `--no-colour`, `--help`,
  `--version`, and `--provenance`.
- Emit exactly one final tagged `PANDAPI_STATUS` record when status output is
  enabled, after artifact disposition is known and before process exit.
- Map engine outcomes to the accepted status/exit vocabulary for this slice:
  `ok` / 0, `domain_no_plan` / 2, `cli_usage_error` / 10,
  `input_unavailable` / 20, `output_unavailable` / 21,
  `input_invalid` / 22, `legacy_surface` / 31 for visible interactive legacy
  paths, `experimental_surface` / 32 for visible translation paths,
  `unsupported_feature` / 30 for visible SAT paths, `future_surface` / 33 for
  visible BDD/CUDD paths, `timeout` / 40 if an owned timeout policy is
  implemented, `resource_limit` / 41 where applicable, and `internal_error` /
  60 for uncaught engine defects.
- Fence interactive, translation, SAT, BDD, and CUDD surfaces without
  promoting them into supported 0.3.0 normal search behavior.
- Update baseline/pipeline records, smoke tests, make targets, provenance
  checks, package helpers, and active docs that still refer to inherited
  `pandaPIengine` as an accepted active command.
- Preserve Slice06/Slice07 parser and grounder native behavior.
- Keep build/test/check/CI entrypoints routed through make targets.
- Close this slice ledger and write the CC closing report.

**Out:**

- No parser or grounder behavior change except regression-test/fixture fallout
  required to keep accepted native parser/grounder evidence green.
- No release publication, final release manifest, checksums, or GitHub release
  publishing behavior.
- No SAT support, BDD/CUDD support, interactive TTY redesign, or translation
  child-process productization.
- No optional dependency enablement, standalone optional-dependency audit, or
  broad engine algorithm modernization beyond what is required for the managed
  normal-search contract.
- No new dependency import, network fetch, `FetchContent`, `ExternalProject`,
  `git clone`, or `curl` in default build/test paths.
- No direct project-script invocation from new CI/GitHub Actions jobs.
- No Arc05 close report.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- whether the native engine contract is implemented by refactoring the
  inherited engine entrypoint, adding a chengdu-owned compiled entrypoint, or
  another non-shell mechanism;
- how engine managed-process option parsing maps to existing engine behavior
  without making interactive, translation, SAT, BDD, or CUDD supported 0.3.0
  normal surfaces;
- how engine solved/no-plan status, diagnostics, plan artifacts, stdout/stderr
  ownership, and output-file errors are produced through `pandapi::runtime` or
  another accepted chengdu-owned runtime boundary;
- whether timeout/resource-limit behavior can be implemented safely in this
  slice or must remain a documented deferral with re-entry conditions;
- how engine provenance avoids claiming an inherited-name compatibility
  command while still preserving source/upstream credit and optional-surface
  disposition;
- what engine references can be safely updated now, and what remains deferred
  until Arc05 synthesis or Arc07/Arc08 documentation and release work.

If engine managed behavior still requires active `pandaPIengine`
compatibility, a shell adapter, or broad optional-surface work, stop and ask
for an amendment. Do not mark engine canonical/no-compat rows done.

## 4. Constraints

- Treat the operator's no-compatibility decision as current policy:
  `v0.3.0` should not ship inherited engine command compatibility.
- Keep this slice engine-only except for necessary parser/grounder regression
  proof and active pipeline command updates.
- Do not bulk-format inherited `pandaPI/` source.
- Use make targets for build, test, check, CI, and release-helper operations.
- New CI behavior must use make targets.
- Preserve Arc02 dependency gates and Arc03 managed-process semantics.
- Preserve Slice06 parser-native and Slice07 grounder-native behavior.
- Required assistant-authored commits must include both co-author trailers from
  `AGENTS.md`.

## 5. Verification Approach

CC verifies by rebuilding the engine, proving
`dist/<platform>/pandapi-engine` is executable and not a shell adapter, proving
`dist/<platform>/pandaPIengine` is absent, rerunning engine managed contract
fixtures through make, and grep-checking active engine surfaces for stale
inherited command references.

CDC will rerun the ledger commands, inspect the diff for accidental parser,
grounder, release-publication, or dependency expansion, and confirm that the
full parser-to-grounder-to-engine pipeline uses canonical `pandapi-*`
commands.

## 6. Exit Criteria

`make build-engine` leaves `dist/<platform>/pandapi-engine` and no active
`dist/<platform>/pandaPIengine`. The artifact is native/non-shell and passes
engine managed contract fixtures through `make test-contract-engine-managed`
or the accepted make target name. Active build/test/tooling/package/provenance
surfaces use `pandapi-engine` for the engine. Parser and grounder managed
contracts remain green, smoke and aggregate tests pass, and optional engine
surfaces are fenced rather than promoted.
