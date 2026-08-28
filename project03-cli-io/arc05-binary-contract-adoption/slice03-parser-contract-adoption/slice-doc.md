# chengdu arc05 / slice03 - parser-contract-adoption - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Migrate parser normal HDDL parse behavior to the accepted managed-process
contract, and prove it through black-box contract fixtures for both the
canonical `pandapi-parser` command and inherited `pandaPIparser` compatibility
entry point.

This is the first Arc05 behavior-adoption slice. It should turn Slice02's
parser contract target into executable evidence while preserving the current
grounder and engine baseline fixtures. The slice is parser-only: grounder and
engine adoption remain later slices.

## 2. Scope

**In:**

- Make the canonical `pandapi-parser` executable available after the parser
  build, alongside inherited `pandaPIparser`.
- Preserve inherited `pandaPIparser` execution for 0.3.0 compatibility with no
  default deprecation warning.
- Migrate supported parser normal HDDL domain/problem parsing to the Arc03
  process contract for file-backed and stdout-backed output.
- Add parser handling for the accepted common options needed by this slice:
  `--output PATH|-`, `--supervised`, `--status`, `--status=stderr`,
  `--status=stdout` where legal, `--quiet`, `--verbose`, `--color=auto`,
  `--color=always`, `--color=never`, `--no-color`, `--no-colour`,
  `--help`, `--version`, and `--provenance`.
- Emit one final tagged `PANDAPI_STATUS` record when status output is enabled,
  after artifact disposition is known and before process exit.
- Map parser outcomes to the accepted status/exit vocabulary for this slice:
  `ok` / 0, `cli_usage_error` / 10, `input_unavailable` / 20,
  `output_unavailable` / 21, `input_invalid` / 22, `legacy_surface` / 31
  where visible helper paths must be fenced, and `internal_error` / 60 for
  uncaught parser defects.
- Keep stdout ownership exact: selected `.htn` artifact, informational
  help/version/provenance output, legal tagged status, or empty.
- Keep stderr ownership exact: human diagnostics and optional final tagged
  status; no human prose in stdout artifacts.
- Preserve ANSI/color policy for machine streams and no-TTY execution.
- Extend `fixtures/contract/` records and `scripts/run-contract-fixtures.sh`
  with parser contract fixtures, while retaining the existing baseline gate.
- Add local/CI parser contract proof if reliable; otherwise record the blocker
  and re-entry condition in the closing report.
- Close the slice ledger and write the CC closing report.

**Out:**

- No grounder or engine migration to the managed-process contract.
- No change to grounder or engine behavior except build/test fallout required
  to keep existing gates green.
- No Arc05-wide conformance report; Slice06 owns synthesis.
- No release-package shape change, release asset rename, checksum,
  `release-manifest.txt`, package, publish, wolong fetch/install/migration, or
  release workflow change.
- No inherited-name removal or inherited-name default deprecation warning.
- No parser verifier/output-helper product expansion. Visible helper paths are
  fenced or explicitly documented as not visible; they are not made supported.
- No H2, cpddl/FAM, engine interactive, translation, SAT, BDD, CUDD, timeout,
  resource-limit, child-process, or full-duplex stdin adoption.
- No default network fetch, package-manager bootstrap, `curl`, `git clone`,
  `FetchContent`, or `ExternalProject` path.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- whether `pandapi-parser` is produced as a wrapper, alias, copied executable,
  renamed build output, or another mechanism;
- whether parser contract behavior is implemented in a chengdu-owned adapter
  around the inherited parser, in the parser source itself, or through a
  narrow combination;
- how inherited positional output compatibility is preserved while canonical
  `--output` is preferred;
- how final `PANDAPI_STATUS` records are assembled, escaped, ordered, flushed,
  and parsed in fixtures;
- how file-backed output is finalized on success and classified on failure;
- how stdout-backed `.htn` output avoids status or human-prose contamination;
- which visible parser verifier/output-helper paths exist and how they are
  fenced without expanding product support;
- whether CI runs parser contract fixtures directly or via an existing make
  target;
- whether any dependency gate needs amendment before implementation proceeds.

If a correct implementation requires replacing the entire parser CLI, adopting
CLI11, changing release packaging, or broadening supported surfaces, stop and
ask for an amendment instead of widening the slice silently. CLI11 is allowed
only through the accepted Arc02 gate: local facade, golden/process tests, and
no third-party API leakage into planner internals.

## 4. Constraints

- Work from the accepted Arc03 `managed-process-contract.md` and
  `contract-test-matrix.md`.
- Keep parser adoption narrow: supported normal HDDL parse only.
- Keep contract fixtures black-box at the executable boundary; Catch2/runtime
  seam tests may supplement but cannot replace process fixtures.
- Preserve Slice02 baseline fixture behavior for parser, grounder, engine, and
  pipeline unless a baseline expectation is deliberately replaced by a parser
  contract fixture and documented.
- Maintain C++17 compatibility and the current Linux/macOS support matrix.
- Do not bulk-format or mechanically churn inherited `pandaPI/` source outside
  files that must change for parser adoption.
- Existing release-package and wolong-facing surfaces remain untouched until
  Arc08 unless the operator explicitly opens them earlier.
- Required assistant-authored commits must include both co-author trailers from
  `AGENTS.md`.

## 5. Verification Approach

CC verifies the slice with black-box parser contract fixtures first, then the
existing baseline, runtime, build, smoke, workflow, and whitespace gates. The
fixture runner should make parser contract checks selectable without requiring
grounder or engine contract behavior to exist.

CDC will rerun the ledger commands, inspect the diff for boundary drift,
confirm final status and stream ownership through executable fixtures, verify
that inherited parser compatibility remains, and check that grounder, engine,
release packaging, package/publish scripts, and wolong-facing release shape did
not change.

## 6. Exit Criteria

The ledger reaches final status. `pandapi-parser` and `pandaPIparser` both
execute supported normal HDDL parser paths under the accepted managed-process
contract, with positive and negative parser fixtures proving command names,
stdout/stderr ownership, artifact disposition, status/exit mapping, final
`PANDAPI_STATUS` fields, color/ANSI policy, and compatibility behavior. The
grounder and engine remain baseline-only for later adoption slices.
