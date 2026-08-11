# Pipeline Contract Fixture Records

These records describe Arc06 managed pipeline fixtures for canonical
`pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` composition. They
exercise a representative parse-ground-solve workload without adding stdin
support or promoting optional surfaces.

Run the pipeline records through the Make public interface:

```sh
make test-contract-pipeline-managed
```

## Comparison and Safety Policy

Pipeline comparison uses stable process fields, semantic predicates, generated
artifact state, normalized stdout/stderr observations, and final
`PANDAPI_STATUS` records. Fixture checks require successful exit codes,
complete intermediate artifacts, stream ownership, ANSI-free machine output,
and one final status record on stderr for each managed step.

Human diagnostic prose can help explain a failure, but it is not final
managed-process contract evidence for the pipeline. Contract evidence comes
from command argv, exit/status classification, stdout/stderr ownership,
artifact disposition, and normalized semantic predicates.

Pipeline fixtures use repository-owned minimal inputs, isolated temporary
artifacts, empty stdin, and the existing engine harness timeout. Pairwise
composition captures stdout artifacts into regular files before invoking the
next component; no fixture claims stdin streaming support.

## Records

### pipeline-managed-file-chain

- `mode`: `pipeline-contract`
- `component`: `pipeline`
- `command.argv`: `pandapi-parser --status=stderr --output OUT.htn DOMAIN.hddl PROBLEM.hddl`; `pandapi-grounder --status=stderr --output OUT.sas OUT.htn`; `pandapi-engine --status=stderr --output OUT.plan OUT.sas`
- `composition`: file-backed parse-ground-solve
- `artifact`: complete `.htn`, `.sas`, and `.plan` files
- `stdout.role`: empty for every step
- `stderr.role`: final status plus optional human diagnostics
- `exit`: `ok` / 0 for parser, grounder, and engine
- `final_status`: one `PANDAPI_STATUS` record on stderr for every step

### pipeline-parser-stdout-to-grounder

- `mode`: `pipeline-contract`
- `component`: `pipeline`
- `command.argv`: `pandapi-parser --status=stderr --output - DOMAIN.hddl PROBLEM.hddl`; captured parser stdout to `OUT.htn`; `pandapi-grounder --status=stderr --output OUT.sas OUT.htn`
- `composition`: pairwise parser stdout artifact to grounder input file
- `artifact`: parser stdout is a complete `.htn` artifact; grounder writes a complete `.sas` artifact
- `stdout.role`: parser stdout is artifact only; grounder stdout is empty
- `stderr.role`: final status for each step
- `final_status`: parser and grounder each emit one stderr `PANDAPI_STATUS`

### pipeline-grounder-stdout-to-engine

- `mode`: `pipeline-contract`
- `component`: `pipeline`
- `command.argv`: `pandapi-parser --status=stderr --output OUT.htn DOMAIN.hddl PROBLEM.hddl`; `pandapi-grounder --status=stderr --output - OUT.htn`; captured grounder stdout to `OUT.sas`; `pandapi-engine --status=stderr --output OUT.plan OUT.sas`
- `composition`: pairwise grounder stdout artifact to engine input file
- `artifact`: complete `.htn`, stdout-backed `.sas`, and `.plan` artifacts
- `stdout.role`: grounder stdout is artifact only; parser and engine stdout are empty
- `stderr.role`: final status for each step
- `final_status`: parser, grounder, and engine each emit one stderr `PANDAPI_STATUS`

### pipeline-supervised-positive

- `mode`: `pipeline-contract`
- `component`: `pipeline`
- `command.argv`: parser, grounder, and engine each run with `--supervised --status=stderr --output PATH`
- `composition`: supervised parse-ground-solve
- `artifact`: complete `.htn`, `.sas`, and `.plan` files
- `stdout.role`: empty for every step
- `stderr.role`: final status, ANSI-free
- `final_status`: one stderr `PANDAPI_STATUS` record for every step
