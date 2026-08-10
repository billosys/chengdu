# Parser Contract Fixture Records

These records describe executable Arc05 parser-contract checks. They cover the
canonical native `pandapi-parser` command, while keeping grounder and engine
behavior on their owning fixture paths.

The runner implements these records with:

```sh
./scripts/run-contract-fixtures.sh --contract --component parser
```

## Comparison and Safety Policy

Parser contract comparison uses stable fields, semantic predicates, and
normalization. Fixture checks compare exit code, generated artifact state,
stdout/stderr ownership, final `PANDAPI_STATUS` fields, ANSI absence where
machine output is owned, and selected stable informational fields. Paths,
temporary names, line endings, and inherited diagnostic prose are normalized or
ignored unless a row explicitly asserts their absence from machine-owned
streams.

Human diagnostic prose is not final managed-process contract evidence. It may
help diagnose failures, but parser contract classification comes from exit
code and `PANDAPI_STATUS` fields.

Safety remains black-box and bounded: fixtures use repository-owned minimal
inputs, isolated `mktemp` outputs, empty stdin, no network, and deterministic
CI-safe unavailable-output cases.

## Records

### parser-canonical-file-success

- `mode`: `parser-contract`
- `component`: `parser`
- `command.argv`: `pandapi-parser --status=stderr --output OUT.htn DOMAIN.hddl PROBLEM.hddl`
- `artifact`: `file-backed`, complete `.htn`
- `stdout.role`: empty
- `stderr.role`: final status plus optional human diagnostics
- `exit`: `ok` / 0
- `final_status`: `PANDAPI_STATUS status=ok component=parser surface=normal_parse surface_disposition=supported exit_code=0 class=success`

### parser-canonical-stdout-success

- `mode`: `parser-contract`
- `component`: `parser`
- `command.argv`: `pandapi-parser --status=stderr --output - DOMAIN.hddl PROBLEM.hddl`
- `artifact`: `stdout-backed`, complete `.htn`
- `stdout.role`: artifact only
- `stderr.role`: final status
- `exit`: `ok` / 0
- `normalization`: stdout must not contain human prose or `PANDAPI_STATUS`

### parser-info-commands

- `mode`: `parser-contract`
- `component`: `parser`
- `command.argv`: `pandapi-parser --help`, `pandapi-parser --version`, `pandapi-parser --provenance`
- `stdout.role`: informational fields
- `stderr.role`: empty
- `exit`: `ok` / 0
- `identity`: output includes `canonical_command=pandapi-parser` and does not advertise a parser legacy sidecar

### parser-cli-usage-errors

- `mode`: `parser-contract`
- `component`: `parser`
- `status`: `cli_usage_error`
- `exit`: 10
- `cases`: missing required args, unknown option, malformed color value, too many positionals, `--output` conflict, `--status=stdout` with stdout artifact, and `--status=stdout` with informational stdout
- `artifact`: absent
- `final_status`: `PANDAPI_STATUS` emitted on the requested legal stream

### parser-input-unavailable

- `mode`: `parser-contract`
- `component`: `parser`
- `status`: `input_unavailable`
- `exit`: 20
- `cases`: missing domain input, missing problem input, and missing HDDL include input
- `artifact`: absent
- `stdout.role`: empty
- `final_status`: includes path role and open operation where feasible

### parser-input-invalid

- `mode`: `parser-contract`
- `component`: `parser`
- `status`: `input_invalid`
- `exit`: 22
- `cases`: malformed HDDL and semantically invalid HDDL reference
- `artifact`: absent
- `classification`: distinct from `input_unavailable`, `output_unavailable`, timeout, dependency failure, and internal error

### parser-output-unavailable

- `mode`: `parser-contract`
- `component`: `parser`
- `status`: `output_unavailable`
- `exit`: 21
- `cases`: missing output parent and output path that is a directory
- `artifact`: absent
- `final_status`: includes `partial_output_policy=absent`

### parser-final-status-fields

- `mode`: `parser-contract`
- `component`: `parser`
- `final_status`: exactly one `PANDAPI_STATUS` record when enabled
- `required_fields`: `status`, `component`, `surface`, `surface_disposition`, `exit_code`, `class`
- `additional_fields`: `partial_output_policy=complete`, `artifact=file`

### parser-status-stream-legality

- `mode`: `parser-contract`
- `component`: `parser`
- `policy`: `--status=stderr` works with file-backed and stdout-backed artifacts; `--status=stdout` works only when stdout is otherwise empty
- `illegal`: `--status=stdout --output -` maps to `cli_usage_error` / 10

### parser-color-policy

- `mode`: `parser-contract`
- `component`: `parser`
- `policy`: `--no-color`, `--no-colour`, and `NO_COLOR` prevent ANSI in machine-owned stderr/status output
- `illegal`: `--color=always` with status, supervised output, or stdout artifact ownership maps to `cli_usage_error` / 10

### parser-legacy-surface-fencing

- `mode`: `parser-contract`
- `component`: `parser`
- `surfaces`: parser verifier and output helper options remain visible only as legacy helper paths
- `status`: `legacy_surface`
- `exit`: 31
- `surface_disposition`: `legacy`
- `safety`: these helper paths are not expanded into supported product behavior
