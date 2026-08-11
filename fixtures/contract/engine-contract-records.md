# Engine Contract Fixture Records

These records describe executable Arc05 Slice08 engine-contract checks. They
cover the canonical native `pandapi-engine` command. Parser and grounder
contract behavior remains accepted separately.

The runner implements these records with:

```sh
./tests/contract/run --contract --component engine
```

## Comparison and Safety Policy

Engine contract comparison uses stable fields, semantic predicates, and
normalization. Fixture checks compare exit code, generated plan artifact state,
stdout/stderr ownership, status stream legality, final `PANDAPI_STATUS`
fields, ANSI absence where machine output is owned, and selected stable
informational fields. Paths, temporary names, line endings, durations, and
inherited search prose are normalized or ignored unless a row explicitly
asserts their absence from machine-owned streams.

Human diagnostic prose is not final managed-process contract evidence. It may
help diagnose failures, but engine contract classification comes from exit
code and `PANDAPI_STATUS` fields.

Safety remains black-box and bounded: fixtures use repository-owned `.sas`
inputs, isolated `mktemp` outputs, empty stdin, no network, deterministic
CI-safe unavailable-output cases, and a harness timeout around search.

## Records

### engine-canonical-file-success

- `mode`: `engine-contract`
- `component`: `engine`
- `surface`: `normal_search`
- `command.argv`: `pandapi-engine --status=stderr --output OUT.plan INPUT.sas`
- `artifact`: `file-backed`, complete plan/search artifact
- `stdout.role`: empty
- `stderr.role`: final status plus optional human diagnostics
- `exit`: `ok` / 0
- `final_status`: `PANDAPI_STATUS status=ok component=engine surface=normal_search surface_disposition=supported exit_code=0 class=success`

### engine-canonical-stdout-success

- `mode`: `engine-contract`
- `component`: `engine`
- `command.argv`: `pandapi-engine --status=stderr --output - INPUT.sas`
- `artifact`: `stdout-backed`, complete plan/search artifact
- `stdout.role`: artifact only
- `stderr.role`: final status
- `exit`: `ok` / 0
- `normalization`: stdout must not contain ANSI or `PANDAPI_STATUS`

### engine-info-commands

- `mode`: `engine-contract`
- `component`: `engine`
- `command.argv`: `pandapi-engine --help`, `pandapi-engine --version`, `pandapi-engine --provenance`
- `stdout.role`: informational fields
- `stderr.role`: empty
- `exit`: `ok` / 0
- `identity`: output includes `canonical_command=pandapi-engine` and does not advertise an inherited engine command
- `boundary`: default help states interactive is legacy, translation is experimental, SAT is unsupported, and BDD/CUDD is future work

### engine-cli-usage-errors

- `mode`: `engine-contract`
- `component`: `engine`
- `status`: `cli_usage_error`
- `exit`: 10
- `cases`: missing required args, unknown option, malformed color value, too many positionals, positional output conflict, `--status=stdout` with stdout artifact, and `--status=stdout` with informational stdout
- `artifact`: absent
- `final_status`: `PANDAPI_STATUS` emitted on the requested legal stream

### engine-input-unavailable

- `mode`: `engine-contract`
- `component`: `engine`
- `status`: `input_unavailable`
- `exit`: 20
- `cases`: missing `.sas` input
- `artifact`: absent
- `stdout.role`: empty
- `final_status`: includes path role and open operation where feasible

### engine-input-invalid

- `mode`: `engine-contract`
- `component`: `engine`
- `status`: `input_invalid`
- `exit`: 22
- `cases`: malformed `.sas` input
- `artifact`: absent
- `classification`: distinct from `input_unavailable`, `output_unavailable`, timeout, resource limit, dependency failure, and internal error

### engine-output-unavailable

- `mode`: `engine-contract`
- `component`: `engine`
- `status`: `output_unavailable`
- `exit`: 21
- `cases`: missing output parent and output path that is a directory
- `artifact`: absent
- `final_status`: includes `partial_output_policy=absent`

### engine-final-status-fields

- `mode`: `engine-contract`
- `component`: `engine`
- `final_status`: exactly one `PANDAPI_STATUS` record when enabled
- `required_fields`: `status`, `component`, `surface`, `surface_disposition`, `exit_code`, `class`
- `additional_fields`: `partial_output_policy=complete`, `artifact=file`, `outcome=solved`

### engine-status-stream-legality

- `mode`: `engine-contract`
- `component`: `engine`
- `policy`: `--status=stderr` works with file-backed and stdout-backed artifacts; `--status=stdout` works only when stdout is otherwise empty
- `illegal`: `--status=stdout --output -` maps to `cli_usage_error` / 10

### engine-color-policy

- `mode`: `engine-contract`
- `component`: `engine`
- `policy`: `--no-color`, `--no-colour`, and `NO_COLOR` prevent ANSI in machine-owned stderr/status output
- `illegal`: `--color=always` with status, supervised output, or stdout artifact ownership maps to `cli_usage_error` / 10

### engine-domain-no-plan

- `mode`: `engine-contract`
- `component`: `engine`
- `surface`: `normal_search`
- `status`: `domain_no_plan`
- `exit`: 2
- `artifact`: absent
- `final_status`: includes `partial_output_policy=absent` and `outcome=no_plan`
- `classification`: no-plan is a completed valid search result, not malformed input, timeout, resource exhaustion, dependency failure, child-process failure, or internal error

### engine-internal-driver-sentinel-fencing

- `mode`: `engine-contract`
- `component`: `engine`
- `command.argv`: `pandapi-engine --status=stderr --pandapi-engine-legacy-driver INPUT.sas`
- `status`: `cli_usage_error`
- `exit`: 10
- `stdout.role`: empty
- `final_status`: exactly one managed `PANDAPI_STATUS` record on stderr
- `safety`: implementation-internal child-driver mechanics must not become a public unmanaged CLI escape hatch

### engine-interactive-surface-fencing

- `mode`: `engine-contract`
- `component`: `engine`
- `surface`: interactive
- `status`: `legacy_surface`
- `exit`: 31
- `surface_disposition`: `legacy`
- `safety`: interactive behavior is not expanded into supported normal search

### engine-translation-surface-fencing

- `mode`: `engine-contract`
- `component`: `engine`
- `surface`: translation
- `status`: `experimental_surface`
- `exit`: 32
- `surface_disposition`: `experimental`
- `safety`: translation and child-process behavior remain outside supported normal search

### engine-sat-surface-fencing

- `mode`: `engine-contract`
- `component`: `engine`
- `surface`: SAT
- `status`: `unsupported_feature`
- `exit`: 30
- `surface_disposition`: `unsupported`
- `safety`: SAT is not supported in this managed engine CLI

### engine-bdd-cudd-surface-fencing

- `mode`: `engine-contract`
- `component`: `engine`
- `surface`: BDD/CUDD
- `status`: `future_surface`
- `exit`: 33
- `surface_disposition`: `future`
- `safety`: BDD/CUDD remains future work and is not built into the supported normal-search surface
