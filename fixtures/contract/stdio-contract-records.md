# stdio Contract Fixture Records

These records describe the Arc09 managed stdin/stdout/stderr contract fixtures
for the canonical `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`
binaries. They prove the release-blocking wolong process shape: supervised
stdin input, stdout artifacts where selected, stderr `PANDAPI_STATUS`, and
no-plan classification without scraping human diagnostics.

Run the stdio records through the Make public interface:

```sh
make test-contract-stdio-managed
```

## Comparison and Safety Policy

stdio comparison uses stable process fields, semantic artifact predicates,
stdout/stderr ownership, final `PANDAPI_STATUS` records, and ANSI absence in
machine-owned streams. Temporary materialization paths are implementation
details; status assertions use the caller-visible logical input path `path=-`
and the component path role.

Human diagnostic prose is not managed-process contract evidence. Fixture
classification comes from argv shape, exit code, stdout/stderr ownership,
artifact disposition, and final status fields.

The fixtures use repository-owned minimal and unsolvable inputs, isolated
`mktemp` outputs, stdin byte streams sourced from known fixture artifacts, and
the existing engine harness timeout.

## Records

### stdio-parser-domain-stdin

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: `pandapi-parser --supervised --status=stderr --output OUT.htn - PROBLEM.hddl`
- `stdin.mode`: domain `.hddl` bytes
- `artifact`: file-backed, complete `.htn`
- `stdout.role`: empty
- `stderr.role`: ANSI-free final status
- `exit`: `ok` / 0
- `final_status`: includes `path_role=domain`, `path=-`, and `operation=read`

### stdio-parser-problem-stdin

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: `pandapi-parser --supervised --status=stderr --output OUT.htn DOMAIN.hddl -`
- `stdin.mode`: problem `.hddl` bytes
- `artifact`: file-backed, complete `.htn`
- `stdout.role`: empty
- `stderr.role`: ANSI-free final status
- `exit`: `ok` / 0
- `final_status`: includes `path_role=problem`, `path=-`, and `operation=read`

### stdio-parser-both-stdin-unsupported

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: `pandapi-parser --supervised --status=stderr --output OUT.htn - -`
- `stdin.mode`: domain `.hddl` bytes
- `artifact`: absent
- `stdout.role`: empty
- `stderr.role`: ANSI-free final status
- `exit`: `cli_usage_error` / 10
- `policy`: both parser inputs from stdin are unsupported because no 0.3.0
  framing exists for two HDDL documents on one stdin stream

### stdio-grounder-stdin-stdout-status

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: `pandapi-grounder --supervised --status=stderr --output - -`
- `stdin.mode`: parser-generated `.htn` bytes
- `artifact`: stdout-backed, complete `.sas`
- `stdout.role`: artifact only
- `stderr.role`: ANSI-free final status
- `exit`: `ok` / 0
- `final_status`: includes `path_role=htn`, `path=-`, and `operation=read`

### stdio-engine-stdin-solved

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: `pandapi-engine --supervised --status=stderr --output - -`
- `stdin.mode`: grounder-generated `.sas` bytes
- `artifact`: stdout-backed, complete solved plan
- `stdout.role`: artifact only
- `stderr.role`: ANSI-free final status
- `exit`: `ok` / 0
- `final_status`: includes `outcome=solved`, `path_role=engine_input`,
  `path=-`, and `operation=read`

### stdio-engine-stdin-no-plan

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: `pandapi-engine --supervised --status=stderr --output - -`
- `stdin.mode`: unsolvable grounder-generated `.sas` bytes
- `artifact`: absent
- `stdout.role`: empty
- `stderr.role`: ANSI-free final status plus optional human diagnostics
- `exit`: `domain_no_plan` / 2
- `final_status`: includes `partial_output_policy=absent`,
  `outcome=no_plan`, `path_role=engine_input`, `path=-`, and `operation=read`

### stdio-pipeline-supervised-solved

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: parser emits `.htn` with `--output -`; grounder consumes that
  `.htn` from stdin and emits `.sas` with `--output -`; engine consumes that
  `.sas` from stdin and emits the solved plan with `--output -`
- `stdin.mode`: empty for parser, parser stdout bytes for grounder, grounder
  stdout bytes for engine
- `stdout.role`: artifact only for each step
- `stderr.role`: ANSI-free final status for each step
- `exit`: `ok` / 0 for parser, grounder, and engine
- `final_status`: grounder and engine stdin records include logical `path=-`

### stdio-pipeline-supervised-no-plan

- `mode`: `stdio-contract`
- `component`: `stdio`
- `command.argv`: parser emits unsolvable `.htn` with `--output -`; grounder
  consumes stdin and emits unsolvable `.sas` with `--output -`; engine consumes
  stdin with `--output -`
- `stdin.mode`: empty for parser, parser stdout bytes for grounder, grounder
  stdout bytes for engine
- `stdout.role`: parser and grounder stdout are artifacts; engine stdout is
  empty because no plan artifact exists
- `stderr.role`: ANSI-free final status for each step
- `exit`: parser `ok` / 0, grounder `ok` / 0, engine `domain_no_plan` / 2
- `final_status`: engine no-plan status includes `outcome=no_plan` and
  logical stdin fields
