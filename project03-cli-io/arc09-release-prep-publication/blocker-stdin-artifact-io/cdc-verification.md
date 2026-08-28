# CDC Verification: Arc09 blocker-stdin-artifact-io

Date: 2026-08-24
Verified commit: `21f6881e`
Verifier: CDC

## Verdict

Accepted with CDC repair before commit.

The implementation closes the accepted stdin artifact IO blocker for Chengdu
0.3.0: parser, grounder, and engine now accept the supported `-` stdin input
forms, reject unsupported parser both-stdin input, preserve stdout artifact and
stderr status ownership, and keep engine no-plan classification stable for
stdin input.

## Review Finding Repaired Before Commit

CDC found one serious issue in the original staged implementation:
`finish()` used `std::exit`, which bypasses destructors for automatic C++
objects. That would have skipped `MaterializedStdin` cleanup on success and on
later failure paths after materialization.

The committed implementation repairs this by:

- unwinding wrapper termination through a status-emitting `ProcessExit`
  exception caught by `main`;
- keeping `MaterializedStdin` destructor cleanup as a safety net;
- explicitly cleaning materialized stdin before final status classification;
- reporting cleanup failure as `output_unavailable` with
  `operation=cleanup`;
- adding smoke coverage for destructor cleanup;
- emitting logical stdin fields (`path_role`, `path=-`, `operation=read`) on
  stdin success/no-plan status records.

## Reproduced Evidence

The following commands passed after the repair:

```bash
make build
make test-runtime
make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed
make test-contract-pipeline-managed && make smoke && make smoke-negative
make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check
make check-tools
git diff --check && git diff --cached --check
```

Observed fixture summaries:

- managed parser: `298 passed, 0 failed`
- managed grounder: `269 passed, 0 failed`
- managed engine: `312 passed, 0 failed`
- managed pipeline: `129 passed, 0 failed`
- positive smoke: `3 passed, 0 failed`
- negative smoke: `4 passed, 0 failed`
- runtime CTest: `7 tests passed, 0 failed`
- baseline contract during `make test`: `38 passed, 0 failed`

Focused stdin probes also passed:

- parser domain-from-stdin produced a non-empty `.htn` and emitted
  `status=ok component=parser path_role=domain path=- operation=read`;
- parser problem-from-stdin produced a non-empty `.htn` and emitted
  `status=ok component=parser path_role=problem path=- operation=read`;
- parser `- -` returned exit `10` and emitted `status=cli_usage_error`;
- grounder consumed parser stdout on stdin and emitted
  `status=ok component=grounder path_role=htn path=- operation=read`;
- engine consumed grounder stdout on stdin and emitted
  `status=ok component=engine outcome=solved path_role=engine_input path=- operation=read`;
- engine consumed unsolvable `.sas` stdin and preserved exit `2`,
  `status=domain_no_plan`, `outcome=no_plan`, and
  `path_role=engine_input path=- operation=read`;
- recent `/tmp/pandapi-{parser,grounder,engine}-stdin.*` materialization files
  were absent after those focused runs.

## Aggregate Check Disposition

Aggregate `make check` was not run. On macOS the aggregate includes
`record-min-os-if-macos`, the known provenance mutation/noise path. CDC
instead reproduced the aggregate's non-mutating constituents directly:
`check-tools`, `actionlint`, `static-analysis`, `format-check`,
`safety-checks`, `test`, and `provenance-check`, plus the stdin-specific
ledger probes above.

## Boundary Check

Accepted staged/committed surface:

- Arc09 blocker docs under
  `docs/design-v0.3.0/arc09-release-prep-publication/`;
- `docs/design-v0.3.0/project-plan.md`;
- parser, grounder, engine native wrappers;
- shared runtime stdin materialization source/header/test;
- runtime CMake test wiring.

No wolong workspace files, public install docs, release publication state,
package outputs, inherited-name compatibility surfaces, or JSON/packet/multipart
framing behavior were changed.

## Bubble-Up

`blocker-stdin-artifact-io` is CDC-verified closed.

Arc09 Slice01 release-readiness-inventory remains blocked until the two
follow-on blockers close:

1. `blocker-stdio-contract-fixtures`: Make-backed stdin fixtures, CI placement,
   and public managed-process/CLI documentation for the implemented stdin
   contract.
2. `blocker-wolong-supervision-proof`: wolong verification against the
   supported stdin/stdout/stderr process contract, or a concrete external
   blocker with re-entry conditions.
