# CDC Verification: Arc09 blocker-stdio-contract-fixtures

Date: 2026-08-25
Verified commit: `5f237c46`
Verifier: CDC

## Verdict

Accepted.

The implementation closes the fixture and documentation blocker for Chengdu's
accepted stdin artifact contract. The new Make-backed stdio component proves
parser, grounder, engine, and supervised pipeline stdin/stdout/stderr behavior
for both solved and no-plan flows, and the public managed-process and CLI docs
now describe the supported and rejected stdin forms.

## Reproduced Evidence

The following ledger commands were reproduced successfully:

```bash
rg -n -- "parser.*stdin|domain.*stdin|problem.*stdin|both.*stdin|unsupported.*stdin" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures
rg -n -- "grounder.*stdin|\\.htn|--output -|--status=stderr" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures
rg -n -- "engine.*stdin|\\.sas|domain_no_plan|no_plan|solved" tests fixtures docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures
make test-contract-stdio-managed
make actionlint && rg -n -- "test-contract-stdio-managed|make " .github/workflows
rg -n -- "stdin|standard input|--output -|--status=stderr|unsupported" docs/managed-process.md docs/reference/cli.md
make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed && make smoke && make smoke-negative
make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check
test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-fixtures/cdc-verification.md
git diff --check && git diff --cached --check
```

Observed summaries:

- managed stdio: `187 passed, 0 failed`
- managed parser: `298 passed, 0 failed`
- managed grounder: `269 passed, 0 failed`
- managed engine: `312 passed, 0 failed`
- managed pipeline: `129 passed, 0 failed`
- runtime CTest during `make test`: `7 tests passed, 0 failed`
- baseline contract during `make test`: `38 passed, 0 failed`
- positive smoke: `3 passed, 0 failed`
- negative smoke: `4 passed, 0 failed`
- provenance: `all 3 components verified against vendor.env and Git state`

The operator also reported an aggregate `make check` pass before this CDC
verification pass. CDC did not rely on that as the sole evidence; the exact
non-publication ledger gates above were reproduced locally.

## Focused Fixture Findings

The stdio fixture component verifies the following release-blocking contract
points:

- parser accepts exactly one stdin input role, with domain-from-stdin and
  problem-from-stdin covered separately;
- parser rejects `- -` with `cli_usage_error` because 0.3.0 has no framing
  protocol for two logical parser inputs on one stdin stream;
- grounder accepts one complete parser-generated `.htn` artifact on stdin and
  can write the `.sas` artifact to stdout while keeping final status on stderr;
- engine accepts one complete grounder-generated `.sas` artifact on stdin and
  can write solved output to stdout while keeping final status on stderr;
- engine no-plan stdin preserves exit `2`, `status=domain_no_plan`,
  `class=expected_domain_outcome`, `partial_output_policy=absent`, and
  `outcome=no_plan`;
- solved and no-plan supervised chains pass parser stdout bytes to grounder
  stdin and grounder stdout bytes to engine stdin without leaking
  `PANDAPI_STATUS` into artifact stdout.

## Boundary Check

Accepted implementation surface:

- `tests/contract/run`;
- `mk/help.mk` and `mk/tests.mk`;
- `fixtures/contract/README.md`;
- `fixtures/contract/stdio-contract-records.md`;
- public docs under `docs/managed-process.md` and `docs/reference/cli.md`;
- Arc09 planning and close artifacts under
  `docs/design-v0.3.0/arc09-release-prep-publication/`;
- `docs/design-v0.3.0/project-plan.md`.

No wolong workspace files, release publication state, package artifacts, tags,
or generated release output were changed. The staged boundary check for
`release/` paths passed.

## Independence Note

F-10's literal command was run before creating this CDC verification document
and passed. CDC also checked commit `5f237c46` directly; it did not contain a
`cdc-verification.md` path for this blocker. This file is the independent
verification artifact added after the implementation close.

## Bubble-Up

`blocker-stdio-contract-fixtures` is CDC-verified closed.

Arc09 Slice01 release-readiness-inventory remains blocked by exactly one
known unnumbered blocker: `blocker-wolong-supervision-proof`. That next
blocker should verify wolong against the supported stdin/stdout/stderr
contract or record a concrete external blocker with re-entry conditions.
