# Slice 02 Closing Report: contract-fixture-scaffold

## Capability Verdict

Implemented. Slice02 now has a repo-level black-box contract fixture scaffold
for current inherited `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`
behavior. The runner is baseline-only: it records current process observations
and deliberately does not claim Arc03 managed-process conformance.

The baseline gate covers parser, grounder, engine, and composed pipeline
observations, including exit status, stdout, stderr, artifact state, and the
current absence of final `PANDAPI_STATUS` lines.

## Implementation Decisions

- Added `fixtures/contract/README.md` as the repo-level fixture home and policy
  document.
- Added `fixtures/contract/baseline-records.md` with YAML-like records for
  current baseline behavior and a single future contract-target parser record
  to document the intended flip point.
- Added `scripts/run-contract-fixtures.sh` as a dependency-free shell runner
  over already-built `dist/<platform>/` binaries.
- Added the baseline fixture gate to the Linux and macOS reusable build jobs
  after the inherited binaries are built.
- Kept stdin/full-duplex fixtures deferred. All current baseline fixtures use
  empty stdin.
- Did not edit `pandaPI/`, add dependencies, add package-manager fetches, add
  canonical `pandapi-*` wrappers, or touch release packaging.

## Verification

All verification below was run locally on `macos-arm64`.

- `bash -n scripts/run-contract-fixtures.sh`: passed.
- `./scripts/run-contract-fixtures.sh --list`: listed 9 baseline fixtures.
- `./scripts/run-contract-fixtures.sh --baseline --component parser`: 16
  passed, 0 failed.
- `./scripts/run-contract-fixtures.sh --baseline --component grounder`: 7
  passed, 0 failed.
- `./scripts/run-contract-fixtures.sh --baseline --component engine`: 8 passed,
  0 failed.
- `./scripts/run-contract-fixtures.sh --baseline --component pipeline`: 7
  passed, 0 failed.
- `./scripts/run-contract-fixtures.sh --baseline`: 38 passed, 0 failed.
- `./scripts/check-format-owned.sh`: OK, 25 files.
- `./scripts/build-runtime.sh`: CTest 5/5 passed.
- `./scripts/sanitize-runtime.sh`: CTest 5/5 passed.
- `./scripts/build-all.sh`: passed.
- `./scripts/smoke-test.sh`: 5 passed, 0 failed.
- `./scripts/smoke-test.sh --negative`: 4 passed, 0 failed.
- `actionlint -color`: passed.
- `git diff --check`: passed before staging.
- `git diff --cached --check`: passed after staging.

The full build emitted inherited parser, grounder, and engine warnings. No new
source changes were made in those binaries.

## Ledger Walk

- F-1 closed: the open-set scope already documents the scaffold boundaries and
  downstream handoff.
- F-2 closed: `fixtures/contract/README.md` documents fixture ownership,
  comparison policy, safety, and baseline-versus-contract semantics.
- F-3 closed: `baseline-records.md` includes the required process-observation
  fields.
- F-4 closed: baseline records and future contract-target records are labeled
  separately.
- F-5 closed: the runner is executable, shell-checked, selectable, listable,
  and emits labeled PASS/FAIL output.
- F-6 closed: the runner captures stdout, stderr, exit, and artifact state
  separately.
- F-7 closed: the runner uses isolated temp directories, cleanup traps, and
  `--preserve`.
- F-8 closed: parser baseline probes cover success, missing file, syntax
  failure, and semantic broken-reference failure.
- F-9 closed: grounder baseline probes cover generated `.htn` success and
  missing input failure.
- F-10 closed: engine baseline probes cover solved and unsolvable outcomes
  with distinct status text and exit codes.
- F-11 closed: the pipeline probe composes parser, grounder, and engine through
  temporary artifacts.
- F-12 closed: baseline records and runner output make `PANDAPI_STATUS`
  absence explicit.
- F-13 closed: fixture docs preserve the black-box process fixture versus
  Catch2/runtime seam-test distinction.
- F-14 closed: diff scans show no new dependency, network fetch, vendored
  package, or rejected dependency import.
- F-15 closed: no canonical `pandapi-*` executable, wrapper, symlink, copied
  binary, release entrypoint, or migration path was created.
- F-16 closed: no parser, grounder, or engine source changed.
- F-17 closed: existing runtime, sanitize, full build, positive smoke, and
  negative smoke gates passed.
- F-18 closed: reusable Linux and macOS CI build jobs run the baseline fixture
  gate.
- F-19 closed: local actionlint passed.
- F-20 closed: unstaged and staged whitespace checks passed.
- F-21 closed: this closing report contains the required closure sections.
- F-22 closed: no CDC verification file was created.

## Silent-Drop Check

No requested Slice02 row was dropped. The fixture runner has executable
coverage for parser, grounder, engine, and pipeline baselines. Baseline
observations remain explicitly labeled as inherited behavior, not managed
process conformance.

The only intentionally deferred fixture class is stdin/full-duplex behavior.
That remains deferred by the Arc04 caveat and is not needed for this baseline
scaffold. A manually probed malformed-engine direction was not added to the
default gate because the inherited engine can hang on malformed input; Slice02
instead records the required solved and unsolvable engine outcomes under a
bounded harness timeout.

## Bubble-up to Arc05

Slice03 can now adopt parser behavior against a concrete baseline fixture home
and runner. The first deliberate flip should be from inherited parser baseline
records toward managed-process expectations: command/argv shape, stream
ownership, artifact disposition, exit/status mapping, and final
`PANDAPI_STATUS`.

No Arc05 plan scope change is required from this slice. CDC should independently
verify the ledger and decide when to open Slice03.
