# Arc05 Slice08 CDC Verification

Date: 2026-08-11

## Verdict

Accepted after CDC corrective patch.

CC's implementation commit `a676b7f3` delivered native `pandapi-engine`
adoption, but CDC review found one blocker: the internal
`--pandapi-engine-legacy-driver` sentinel was externally reachable and could
run outside the managed CLI contract. CDC corrected that in the narrowest
implementation seam and added executable sentinel-fencing coverage.

## Corrective Finding

- **F-19 blocker, fixed:** public invocation of
  `pandapi-engine --pandapi-engine-legacy-driver ...` bypassed normal option
  parsing and emitted inherited human output as a successful run.
- **Fix:** remove the public sentinel branch and invoke
  `pandaPIengine_legacy_main` directly inside the forked child used by the
  managed wrapper.
- **Regression fixture:** `engine-internal-driver-sentinel-fencing` proves the
  sentinel is now a normal `cli_usage_error` / 10 with empty stdout and exactly
  one final status record on stderr.

## Reproduction

CDC reproduced the corrected slice on `release/0.3.x`, macOS arm64:

- `make build-engine` passed.
- `./tests/contract/run --contract --component engine --case engine-internal-driver-sentinel-fencing` passed: 10 passed, 0 failed.
- Direct sentinel probe returned exit 10, empty stdout, and one
  `PANDAPI_STATUS status=cli_usage_error ... exit_code=10 class=caller_error`
  record.
- `make test-contract-engine-managed` passed: 299 passed, 0 failed.
- `make test-contract-parser-managed` passed: 285 passed, 0 failed.
- `make test-contract-grounder-managed` passed: 256 passed, 0 failed.
- `make smoke` passed: 3 passed, 0 failed.
- `make smoke-negative` passed: 4 passed, 0 failed.
- `make provenance-check` passed.
- `make actionlint` passed.
- `make test` passed, including runtime, parser, grounder, engine, and smoke
  gates.
- `git diff --check` passed.

Inherited compiler warnings appeared during C/C++ builds; no new warning class
was treated as Slice08 evidence.

## Independent Review

Because CDC touched code during verification, a fresh subagent review examined
the corrective diff. It reported no findings and independently reproduced:

- `make build-engine`
- focused sentinel fixture: 10 passed, 0 failed
- `make test-contract-engine-managed`: 299 passed, 0 failed
- direct sentinel probe: exit 10, empty stdout, one managed status
- direct normal engine probe: exit 0, managed status, solved outcome
- `git diff --check`

Residual risk is bounded to the current native engine bridge: the forked child
now calls inherited engine code in-process instead of self-execing through a
public sentinel. That is acceptable for Arc05 contract adoption and should be
revisited only if Arc06 expands process-isolation or timeout supervision.

## Ledger Close

- F-1 through F-18 are accepted by CC evidence plus CDC reproduction.
- F-19 is accepted by the corrective fixture and direct probe.
- Parser and grounder native regressions remain accepted.
- No release publication workflow, checksum, final manifest, or wolong-facing
  release shape was accepted in this slice.

## Bubble-up

Slice08 closes the final primary binary adoption surface for Arc05. Arc05 can
now close by synthesizing parser, grounder, and engine evidence, with release
publication, docs/tutorial, expanded CI, and wolong migration proof routed to
their downstream arcs.
