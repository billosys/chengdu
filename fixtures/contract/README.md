# Contract Process Fixtures

`fixtures/contract/` is the repo-level home for Arc05 black-box process
fixtures. These fixtures observe executable behavior at the process boundary:
command name, argv, stdin mode, stdout, stderr, generated artifacts, exit
status, final status policy, normalization, and safety.

The current executable gate is baseline-only. It records inherited
`pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` behavior before the
per-binary Arc05 adoption slices migrate behavior to the managed-process
contract. A passing baseline fixture is not a managed-process conformance
claim.

## Fixture Record Shape

Fixture records use a dependency-free, YAML-like documentation format in
[`baseline-records.md`](baseline-records.md) and
[`parser-contract-records.md`](parser-contract-records.md) and
[`grounder-contract-records.md`](grounder-contract-records.md). The fields
mirror the accepted Arc04 runtime fixture vocabulary:

- `id`: stable fixture identifier used by the runner output.
- `owner`: planning owner, currently Arc05 Slice02 for baseline scaffold rows.
- `mode`: `baseline` for current inherited behavior or `contract-target` for
  future managed-process expectations that do not pass yet.
- `component`: `parser`, `grounder`, `engine`, or `pipeline`.
- `surface_disposition`: current surface state, usually `supported`.
- `command.argv`: executable and argv shape. Concrete paths are resolved by
  the runner under `dist/<platform>/` and isolated temp directories.
- `stdin.mode`: `empty` unless a later fixture explicitly requires small byte
  stdin.
- `stdout.role` and `stderr.role`: observed stream ownership role.
- `artifact`: expected generated output state such as `complete` or `absent`.
- `exit`: expected inherited numeric exit status and status label.
- `final_status`: current baseline expectation for `PANDAPI_STATUS`; inherited
  binaries currently have `observation: absent`.
- `normalization`: path, ANSI, line-ending, and predicate policy.
- `safety`: cleanup, timeout, and stdin/full-duplex policy.

## Baseline Versus Contract

Baseline records describe what the inherited binaries do today. They may
include pre-contract exit codes, mixed human output, and absent
`PANDAPI_STATUS` lines. Future parser, grounder, and engine adoption slices
must flip the relevant records or add new `contract-target` records when they
make managed-process behavior real.

Contract expectations come from the Arc03 matrix: stable command/argv shape,
stdout/stderr ownership, output artifact disposition, status/exit mapping,
final `PANDAPI_STATUS` fields, color/ANSI rules, normalization, and safety.
Those expectations are documented here so later slices can extend the runner
without treating this baseline scaffold as final conformance.

## Comparison Policy

Baseline fixture comparison is limited to stable process fields, generated
artifact state, semantic predicates, and normalized output. Paths, ANSI
sequences, line endings, timing, and other environment-sensitive details are
normalized before they can affect a fixture result.

Human diagnostic prose may be used as a baseline predicate when it is the only
current inherited signal, but it is not final managed-process contract
evidence. Later adoption slices should replace prose-sensitive checks with
stable status fields, stream ownership assertions, and final `PANDAPI_STATUS`
records where the managed-process contract requires them.

## Black-Box Versus Seam Tests

These fixtures own product behavior. They execute binaries and inspect
process observations. Catch2/runtime seam tests remain useful for helper-level
proof, but they cannot replace black-box process fixtures for parser,
grounder, engine, or pipeline behavior.

## Safety Policy

The runner uses `mktemp` work directories and removes them by default.
`--preserve` keeps the temp tree for debugging and prints its path. Engine
fixtures use a bounded harness timeout so a spinning inherited process reports
as a fixture failure instead of hanging CI.

The Arc04 stdin/full-duplex caveat remains active. Baseline fixtures here use
empty stdin only. Stdin-heavy fixtures are deferred until the process runner
has focused regression coverage or a full-duplex stdin/stdout/stderr
observation loop.

## Running

Build first, then run:

```sh
make build
./scripts/run-contract-fixtures.sh --baseline
./scripts/run-contract-fixtures.sh --baseline --component parser
./scripts/run-contract-fixtures.sh --contract --component parser
./scripts/run-contract-fixtures.sh --contract --component parser --case parser-canonical-file-success
./scripts/run-contract-fixtures.sh --contract --component grounder
./scripts/run-contract-fixtures.sh --contract --component grounder --case grounder-canonical-file-success
./scripts/run-contract-fixtures.sh --list
./scripts/run-contract-fixtures.sh --contract --component parser --list
./scripts/run-contract-fixtures.sh --contract --component grounder --list
```

The runner emits labeled PASS/FAIL lines and exits nonzero if any selected
fixture misses its baseline or managed-process contract expectation.
