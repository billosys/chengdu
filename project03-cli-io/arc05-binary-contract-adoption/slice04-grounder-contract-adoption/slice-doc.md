# chengdu arc05 / slice04 - grounder-contract-adoption - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Migrate grounder normal `.htn` grounding behavior to the accepted
managed-process contract, and prove it through black-box contract fixtures for
both the canonical `pandapi-grounder` command and inherited `pandaPIgrounder`
compatibility entry point.

This is a grounder-only adoption slice. It composes with the accepted Slice03
parser contract: parser contract behavior must remain stable, grounder becomes
the second executable managed surface, and engine behavior remains
baseline-only until Slice05.

## 2. Scope

**In:**

- Make the canonical `pandapi-grounder` executable available after the grounder
  build, alongside inherited `pandaPIgrounder`.
- Preserve inherited `pandaPIgrounder` execution for 0.3.0 compatibility with
  no default deprecation warning.
- Migrate supported normal `.htn` grounding to the Arc03 process contract for
  file-backed and stdout-backed `.sas` output.
- Add grounder handling for the accepted common options needed by this slice:
  `--output PATH|-`, `--supervised`, `--status`, `--status=stderr`,
  `--status=stdout` where legal, `--quiet`, `--verbose`, `--color=auto`,
  `--color=always`, `--color=never`, `--no-color`, `--no-colour`,
  `--help`, `--version`, and `--provenance`.
- Emit one final tagged `PANDAPI_STATUS` record when status output is enabled,
  after artifact disposition is known and before process exit.
- Map grounder outcomes to the accepted status/exit vocabulary for this slice:
  `ok` / 0, `domain_no_plan` / 2 if reachable through accepted grounder
  semantics, `cli_usage_error` / 10, `input_unavailable` / 20,
  `output_unavailable` / 21, `input_invalid` / 22,
  `legacy_surface` / 31 for `cpddl`/FAM legacy surfaces where visible,
  `experimental_surface` / 32 for H2 where visible, `resource_limit` / 41 if
  an owned bounded resource policy is implemented, and `internal_error` / 60
  for uncaught grounder defects.
- Keep stdout ownership exact: selected `.sas` artifact, informational
  help/version/provenance output, legal tagged status, or empty.
- Keep stderr ownership exact: human diagnostics, progress/statistics where
  allowed, and optional final tagged status; no human prose in stdout
  artifacts.
- Fence visible H2 and `cpddl`/FAM surfaces without expanding them into
  supported 0.3.0 behavior.
- Extend `fixtures/contract/` records and `scripts/run-contract-fixtures.sh`
  with grounder contract fixtures, while retaining parser contract and
  baseline gates.
- Add make-targeted local/CI grounder contract proof. New workflow steps, if
  any, must invoke `make` targets rather than project scripts directly.
- Close the slice ledger and write the CC closing report.

**Out:**

- No parser behavior change except regression-test/fixture fallout required to
  keep accepted Slice03 parser contract evidence green.
- No engine migration to the managed-process contract.
- No change to engine behavior except build/test fallout required to keep
  existing gates green.
- No Arc05-wide conformance report; Slice06 owns synthesis.
- No release-package shape change, release asset rename, checksum,
  `release-manifest.txt`, package, publish, wolong fetch/install/migration, or
  release workflow behavior change.
- No inherited-name removal or inherited-name default deprecation warning.
- No H2 product expansion, `cpddl`/FAM product expansion, standalone dependency
  audit, optional dependency enablement, SAT/BDD/CUDD work, engine
  interactive/translation work, or full-duplex stdin adoption.
- No default network fetch, package-manager bootstrap, `curl`, `git clone`,
  `FetchContent`, or `ExternalProject` path.
- No direct project-script invocation from new CI/GitHub Actions jobs; route
  new CI behavior through make targets.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- whether `pandapi-grounder` is produced as a wrapper, alias, copied
  executable, renamed build output, source entry point, or another mechanism;
- whether grounder contract behavior is implemented in a chengdu-owned adapter
  around the inherited grounder, in grounder source, or through a narrow
  combination;
- whether a raw legacy passthrough such as `pandaPIgrounder.legacy` is needed
  to preserve inherited behavior while the inherited `pandaPIgrounder` name
  becomes managed;
- how inherited positional output compatibility is preserved while canonical
  `--output` is preferred;
- how final `PANDAPI_STATUS` records are assembled, escaped, ordered, flushed,
  and parsed in fixtures;
- how file-backed output is finalized on success and classified on failure;
- how stdout-backed `.sas` output avoids status, progress, statistics, ANSI,
  or human-prose contamination;
- which H2 and `cpddl`/FAM surfaces are visible through the new supported
  grounder CLI and how each is fenced;
- whether `domain_no_plan` is reachable through accepted grounder semantics in
  this slice, and if not, the exact re-entry condition for exercising it;
- how the make target graph is extended so reusable CI reaches the grounder
  contract gate through `make test`, `make ci-linux`, and `make ci-macos`;
- whether any dependency gate needs amendment before implementation proceeds.

If correct grounder adoption requires broad `cpddl` or H2 audits, replacing
the entire CLI with CLI11, changing release packaging, adding a network fetch,
or widening supported surfaces, stop and ask for an amendment instead of
widening the slice silently. CLI11 is allowed only through the accepted Arc02
gate: local facade, golden/process tests, and no third-party API leakage into
planner internals.

## 4. Constraints

- Work from the accepted Arc03 `managed-process-contract.md` and
  `contract-test-matrix.md`.
- Keep grounder adoption narrow: supported normal `.htn` grounding only.
- Keep contract fixtures black-box at the executable boundary; Catch2/runtime
  seam tests may supplement but cannot replace process fixtures.
- Preserve Slice03 parser contract behavior and Slice02 baseline behavior for
  engine and pipeline unless a baseline expectation is deliberately replaced
  by grounder contract evidence and documented.
- Maintain C++17 compatibility for chengdu-owned code and the current
  Linux/macOS support matrix. If inherited grounder build flags remain
  stricter or newer, document rather than silently broadening the product
  claim.
- Do not bulk-format or mechanically churn inherited `pandaPI/` source outside
  files that must change for grounder adoption.
- Existing release-package and wolong-facing surfaces remain untouched until
  Arc08 unless the operator explicitly opens them earlier.
- Required assistant-authored commits must include both co-author trailers from
  `AGENTS.md`.

## 5. Verification Approach

CC verifies the slice with black-box grounder contract fixtures first, then
the accepted parser contract, existing baseline/pipeline behavior, runtime,
build, smoke, workflow, make-target, provenance, and whitespace gates. The
fixture runner should make grounder contract checks selectable without
requiring engine contract behavior to exist.

CDC will rerun the ledger commands, inspect the diff for boundary drift,
confirm final status and stream ownership through executable fixtures, verify
that inherited grounder compatibility remains, check that H2 and `cpddl`/FAM
surfaces are fenced rather than promoted, and check that parser, engine,
release packaging, package/publish scripts, and wolong-facing release shape did
not change.

## 6. Exit Criteria

The ledger reaches final status. `pandapi-grounder` and `pandaPIgrounder` both
execute supported normal `.htn` grounding under the accepted managed-process
contract, with positive and negative grounder fixtures proving command names,
stdout/stderr ownership, artifact disposition, status/exit mapping, final
`PANDAPI_STATUS` fields, color/ANSI policy, compatibility behavior, and H2 /
`cpddl` / FAM fencing. Parser contract fixtures remain green, and engine
behavior remains baseline-only for Slice05.
