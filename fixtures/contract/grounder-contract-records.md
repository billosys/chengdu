# Grounder Contract Fixture Records

These records describe executable Arc05 Slice04 grounder-contract checks. They
cover the canonical `pandapi-grounder` command. Parser contract behavior
remains accepted separately, and engine behavior remains on the baseline fixture
path.

The runner implements these records with:

```sh
make test-contract-grounder-managed
```

## Comparison and Safety Policy

Grounder contract comparison uses stable fields, semantic predicates, and
normalization. Fixture checks compare exit code, generated `.sas` artifact
state, stdout/stderr ownership, status stream legality, final `PANDAPI_STATUS`
fields, ANSI absence where machine output is owned, and selected stable
informational fields. Paths, temporary names, line endings, and inherited
diagnostic prose are normalized or ignored unless a row explicitly asserts their
absence from machine-owned streams.

Human diagnostic prose is not final managed-process contract evidence. It may
help diagnose failures, but grounder contract classification comes from exit
code and `PANDAPI_STATUS` fields.

Safety remains black-box and bounded: fixtures use repository-owned `.htn`
inputs, isolated `mktemp` outputs, empty stdin, no network, and deterministic
CI-safe unavailable-output cases.

## Records

### grounder-canonical-file-success

- `mode`: `grounder-contract`
- `component`: `grounder`
- `command.argv`: `pandapi-grounder --status=stderr --output OUT.sas INPUT.htn`
- `artifact`: `file-backed`, complete `.sas`
- `stdout.role`: empty
- `stderr.role`: final status plus optional human diagnostics
- `exit`: `ok` / 0
- `final_status`: `PANDAPI_STATUS status=ok component=grounder surface=normal_grounding surface_disposition=supported exit_code=0 class=success`

### grounder-canonical-stdout-success

- `mode`: `grounder-contract`
- `component`: `grounder`
- `command.argv`: `pandapi-grounder --status=stderr --output - INPUT.htn`
- `artifact`: `stdout-backed`, complete `.sas`
- `stdout.role`: artifact only
- `stderr.role`: final status
- `exit`: `ok` / 0
- `normalization`: stdout must not contain progress, statistics, ANSI, human prose, or `PANDAPI_STATUS`

### grounder-info-commands

- `mode`: `grounder-contract`
- `component`: `grounder`
- `command.argv`: `pandapi-grounder --help`, `pandapi-grounder --version`, `pandapi-grounder --provenance`
- `stdout.role`: informational fields
- `stderr.role`: empty
- `exit`: `ok` / 0
- `identity`: output includes `canonical_command=pandapi-grounder` and does not advertise an old-name grounder sidecar
- `boundary`: default help states H2 is experimental and cpddl/FAM is legacy, not supported normal grounding

### grounder-cli-usage-errors

- `mode`: `grounder-contract`
- `component`: `grounder`
- `status`: `cli_usage_error`
- `exit`: 10
- `cases`: missing required args, unknown option, malformed color value, too many positionals, `--output` conflict, `--status=stdout` with stdout artifact, and `--status=stdout` with informational stdout
- `artifact`: absent
- `final_status`: `PANDAPI_STATUS` emitted on the requested legal stream

### grounder-input-unavailable

- `mode`: `grounder-contract`
- `component`: `grounder`
- `status`: `input_unavailable`
- `exit`: 20
- `cases`: missing `.htn` input
- `artifact`: absent
- `stdout.role`: empty
- `final_status`: includes path role and open operation where feasible

### grounder-input-invalid

- `mode`: `grounder-contract`
- `component`: `grounder`
- `status`: `input_invalid`
- `exit`: 22
- `cases`: malformed `.htn` input
- `artifact`: absent
- `classification`: distinct from `input_unavailable`, `output_unavailable`, timeout, resource limit, dependency failure, and internal error

### grounder-output-unavailable

- `mode`: `grounder-contract`
- `component`: `grounder`
- `status`: `output_unavailable`
- `exit`: 21
- `cases`: missing output parent and output path that is a directory
- `artifact`: absent
- `final_status`: includes `partial_output_policy=absent`

### grounder-final-status-fields

- `mode`: `grounder-contract`
- `component`: `grounder`
- `final_status`: exactly one `PANDAPI_STATUS` record when enabled
- `required_fields`: `status`, `component`, `surface`, `surface_disposition`, `exit_code`, `class`
- `additional_fields`: `partial_output_policy=complete`, `artifact=file`

### grounder-status-stream-legality

- `mode`: `grounder-contract`
- `component`: `grounder`
- `policy`: `--status=stderr` works with file-backed and stdout-backed artifacts; `--status=stdout` works only when stdout is otherwise empty
- `illegal`: `--status=stdout --output -` maps to `cli_usage_error` / 10

### grounder-color-policy

- `mode`: `grounder-contract`
- `component`: `grounder`
- `policy`: `--no-color`, `--no-colour`, and `NO_COLOR` prevent ANSI in machine-owned stderr/status output
- `illegal`: `--color=always` with status, supervised output, or stdout artifact ownership maps to `cli_usage_error` / 10

### grounder-h2-surface-fencing

- `mode`: `grounder-contract`
- `component`: `grounder`
- `surface`: H2
- `status`: `experimental_surface`
- `exit`: 32
- `surface_disposition`: `experimental`
- `safety`: H2 is not expanded into supported normal grounding behavior

### grounder-cpddl-fam-surface-fencing

- `mode`: `grounder-contract`
- `component`: `grounder`
- `surfaces`: cpddl and FAM lifted invariant inference
- `status`: `legacy_surface`
- `exit`: 31
- `surface_disposition`: `legacy`
- `safety`: cpddl/FAM remains an implementation dependency and inherited advanced path, not supported normal grounding behavior

### grounder-domain-no-plan

- `mode`: `grounder-contract`
- `component`: `grounder`
- `status`: `domain_no_plan`
- `assessment`: not reachable through accepted grounder-only semantics in this slice
- `re-entry`: add this fixture only if a later accepted grounder behavior can prove no usable grounded model or reachable goal without relying on engine search

### grounder-supervised-positive

- `mode`: `grounder-contract`
- `component`: `grounder`
- `command.argv`: `pandapi-grounder --supervised --status=stderr --output OUT.sas INPUT.htn`
- `artifact`: file-backed, complete `.sas`
- `stdout.role`: empty
- `stderr.role`: ANSI-free final status plus optional human diagnostics
- `exit`: `ok` / 0
- `final_status`: one stderr `PANDAPI_STATUS` record with `status=ok`
