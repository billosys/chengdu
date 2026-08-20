# Wolong Stdin Contract Audit

Date: 2026-08-20

Source: Wolong blocker report:
`/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/chengdu-stdin-contract-blocker.md`

Status: release-blocking; routes unnumbered Arc09 blocker slices before
Slice01 release-readiness-inventory can honestly close.

## Assessment

Wolong's blocker is verified. Chengdu 0.3.0 currently supports stdout artifact
ownership through `--output -`, final machine status on stderr through
`--status=stderr`, and the expected no-plan engine classification, but it does
not implement a supported stdin input contract for `pandapi-parser`,
`pandapi-grounder`, or `pandapi-engine`.

This is not merely a missing test. The native command wrappers parse a leading
`-` operand as an unknown option and later validate accepted operands as
readable file paths. The contract fixture runner also captures stdout to files
for pairwise composition rather than driving downstream components from stdin.
That means current evidence proves file-backed managed-process behavior, not
the direct stdin/stdout/stderr pipeline needed by wolong's erlexec supervision
work.

## Verified Claims

### Direct stdin input is rejected

Local probes against `./bin` reproduced wolong's result:

```text
./bin/pandapi-parser --supervised --status=stderr --output /tmp/chengdu-stdin-parser-domain.htn - fixtures/minimal/problem.hddl
=> exit 10; status=cli_usage_error

./bin/pandapi-parser --supervised --status=stderr --output /tmp/chengdu-stdin-parser-problem.htn fixtures/minimal/domain.hddl -
=> exit 10; status=cli_usage_error

./bin/pandapi-grounder --supervised --status=stderr --output /tmp/chengdu-stdin-grounder.sas -
=> exit 10; status=cli_usage_error

./bin/pandapi-engine --supervised --status=stderr --output /tmp/chengdu-stdin-engine.plan -
=> exit 10; status=cli_usage_error
```

The diagnostics identify `-` as an unknown option for all three components.
The final status lines remain machine-readable and classify the result as
caller error, but the desired input form is unavailable.

### Downstream pipe probes fail at stdin input

Pairwise shell pipeline probes also reproduce the downstream failure:

```text
./bin/pandapi-parser --supervised --status=stderr --output - fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl |
  ./bin/pandapi-grounder --supervised --status=stderr --output /tmp/chengdu-stdin-pipeline.sas -
=> exit 10 from grounder; status=cli_usage_error

./bin/pandapi-grounder --supervised --status=stderr --output - fixtures/grounder/minimal.htn |
  ./bin/pandapi-engine --supervised --status=stderr --output /tmp/chengdu-stdin-pipeline.plan -
=> exit 10 from engine; status=cli_usage_error
```

This confirms the missing capability is downstream stdin consumption, not
artifact content production.

### Native wrapper audit

- `pandaPI/parser/src/pandapi_parser_native.cpp:331` treats an operand that
  starts with `-` as an unknown option unless it follows `--`.
- `pandaPI/parser/src/pandapi_parser_native.cpp:579` and
  `pandaPI/parser/src/pandapi_parser_native.cpp:581` require the domain
  operand to be a readable file path.
- `pandaPI/parser/src/pandapi_parser_native.cpp:589` requires the problem
  operand to be a readable file path.
- `pandaPI/grounder/src/pandapi_grounder_native.cpp:332` treats leading `-`
  as an unknown option.
- `pandaPI/grounder/src/pandapi_grounder_native.cpp:566` and
  `pandaPI/grounder/src/pandapi_grounder_native.cpp:567` require the input
  operand to be a readable file path.
- `pandaPI/engine/src/pandapi_engine_native.cpp:367` treats leading `-` as an
  unknown option.
- `pandaPI/engine/src/pandapi_engine_native.cpp:607` and
  `pandaPI/engine/src/pandapi_engine_native.cpp:608` require the input
  operand to be a readable file path.

The parser also has a two-input contract, so stdin design must explicitly
define domain/problem roles. A single `-` can be unambiguous when the other
role is a path, but a `- -` form needs either documented framing or explicit
rejection with a stable status.

### Fixture audit

- `tests/contract/run:288` captures stdout/stderr without feeding stdin.
- `tests/contract/run:1434` proves parser artifact stdout only by capturing it
  to a file before invoking grounder.
- `tests/contract/run:1456` proves grounder artifact stdout only by capturing
  it to a file before invoking engine.
- `fixtures/contract/parser-contract-records.md`,
  `fixtures/contract/grounder-contract-records.md`, and
  `fixtures/contract/engine-contract-records.md` record empty stdin safety.
- `fixtures/contract/pipeline-contract-records.md` explicitly states that no
  fixture claims stdin streaming support.
- `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
  records pairwise artifact composition "after capture" and re-entry for
  stdin streaming only if a future accepted contract adds it.

## Release Impact

The project plan names wolong's supervised-process use case as the acceptance
anchor for 0.3.0. Arc09 cannot close Slice01 release-readiness-inventory, and
cannot later verify wolong fetch/install/migration behavior, while the required
stdin contract is absent or unspecified.

Arc09 therefore inserts the following unnumbered blockers before Slice01
release-readiness work resumes:

1. `blocker-stdio-contract-design`
2. `blocker-stdin-artifact-io`
3. `blocker-stdio-contract-fixtures`
4. `blocker-wolong-supervision-proof`

These blockers intentionally require design-first remediation and shared
runtime reuse before implementation. The goal is a durable managed-process
contract, not three local patches that happen to pass the first wolong probe.
