# CDC Verification: Arc09 blocker-wolong-supervision-proof

Date: 2026-08-25

Status: verified. The Chengdu proof/routing slice is closed and CDC-verified;
Arc09 Slice01 remains blocked by the Wolong-owned stdin runner/API work.

Reviewed commit:

- `41692d66` - `Close Arc09 Wolong supervision proof`

## Scope Reviewed

I reviewed the committed proof packet:

- `slice-doc.md`
- `ledger.md`
- `wolong-supervision-proof.md`
- `closing-report.md`
- Arc09 `arc-plan.md`
- project `project-plan.md`

The committed docs correctly distinguish three evidence layers:

- Chengdu's supported stdin/stdout/stderr contract is now fixture-proven.
- Raw Wolong erlexec argv-list supervision can pass stdin bytes, send EOF,
  collect stdout/stderr, and observe process exit.
- Wolong's public `wolong-exec:run/3` runner/API still lacks stdin bytes and
  EOF support, so release readiness remains externally blocked.

## Independent Audit

I inspected the current Wolong runner surface:

- `/Users/oubiwann/lab/billosys/wolong/src/wolong-exec.lfe`
- `/Users/oubiwann/lab/billosys/wolong/test/`
- `/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/`

Audit result:

- `wolong-exec.lfe` exports only `(run 3)`.
- `validate-opts` accepts timeout, kill-timeout, and output-limit settings.
- `run-valid` constructs argv-list execution and enables `monitor`, `stdout`,
  `stderr`, `kill_group`, `group`, and `kill_timeout`.
- The public runner does not enable erlexec `stdin`.
- The public runner has no `exec:send/2` path for stdin bytes.
- The public runner has no EOF send path for child stdin.
- Existing Wolong tests cover stdout/stderr capture, limits, process status,
  timeout handling, gate classification, and pipeline behavior, but not public
  runner stdin artifact delivery.

This independently confirms the proof report's blocker routing.

## Reproduced Evidence

Chengdu stdio fixture gate:

```text
make test-contract-stdio-managed
SUMMARY: 187 passed, 0 failed (contract, stdio, macos-arm64)
```

Existing managed contract and smoke chain:

```text
make test-contract-parser-managed
SUMMARY: 298 passed, 0 failed (contract, parser, macos-arm64)

make test-contract-grounder-managed
SUMMARY: 269 passed, 0 failed (contract, grounder, macos-arm64)

make test-contract-engine-managed
SUMMARY: 312 passed, 0 failed (contract, engine, macos-arm64)

make test-contract-pipeline-managed
SUMMARY: 129 passed, 0 failed (contract, pipeline, macos-arm64)

make smoke
SUMMARY: 3 passed, 0 failed (positive mode, macos-arm64)

make smoke-negative
SUMMARY: 4 passed, 0 failed (negative mode, macos-arm64)
```

Release-quality chain:

```text
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
```

Result: all passed. `make test` also re-ran baseline, managed, stdio, smoke,
and runtime coverage relevant to this proof.

Raw Wolong erlexec argv-list stdin proof:

```text
erlexec argv-list stdin proof: reason=normal stdout_bytes=446 stderr=PANDAPI_STATUS	status=ok	component=grounder	surface=normal_grounding	surface_disposition=supported	exit_code=0	class=success	partial_output_policy=complete	artifact=stdout	path_role=htn	path=-	operation=read
```

Wolong gates:

```text
rebar3 compile
rebar3 as test eunit
rebar3 as test ct
rebar3 xref
rebar3 dialyzer
rebar3 lfe format --check
```

Result: compile passed; EUnit passed `9 tests, 0 failures`; Common Test passed
`All 62 tests passed`; xref, dialyzer, and format passed.

Ledger/boundary checks:

```text
F-2 through F-6 grep checks passed.
No release output or release notes paths were staged by the CC close commit.
No CDC verification file existed before this CDC step.
git diff --check passed.
git diff --cached --check passed.
```

Workspace checks:

```text
chengdu: ## release/0.3.x
wolong:  ## main...origin/main
```

Both worktrees were clean before the CDC documentation update.

## CDC Finding

No correction is required for the committed Chengdu proof/routing slice.

The remaining blocker is correctly assigned outside Chengdu: Wolong Arc03
`slice02-stdio-runner`, or an equivalent Wolong-owned implementation slice,
must expose and test stdin artifact bytes plus EOF through the public
`wolong-exec` runner/API. Raw erlexec proof is necessary evidence, but it is
not sufficient release proof because it bypasses Wolong's public runner.

Arc09 Slice01 `release-readiness-inventory` must remain blocked until that
Wolong runner/API slice closes and Chengdu reruns the supervision proof against
the public Wolong surface.

## Notes

I did not run aggregate `make check` in this CDC step. This was a documentation
proof/routing close, and the ledger's load-bearing constituent gates were
reproduced directly: stdio contract fixtures, managed contracts, smoke,
release-quality wayfinding, raw Wolong erlexec proof, Wolong gates, boundary,
and whitespace checks.
