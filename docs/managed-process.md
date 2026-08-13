# Managed Process Integration for pandaPI 0.3.0

This guide is for process-manager and integration authors who run
`pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` under supervision.
It assumes you already know the basic
[`parse -> ground -> solve` tutorial](tutorial/02-first-project-workflow.md)
and now need stable process behavior instead of terminal-oriented prose.

The integration contract is simple:

- run local binaries built by `make build` from `./bin/`;
- use `--supervised` for quiet process-manager behavior;
- choose a status stream with `--status`, `--status=stderr`, or
  `--status=stdout`;
- classify outcomes from the process exit code and final `PANDAPI_STATUS`
  fields;
- treat human diagnostics as useful context for people, not as data your
  supervisor parses.

## Build Local Binaries

Run examples from the repository root:

```sh
make build
tmp="$(mktemp -d)"
echo "$tmp"
```

Keep using the same shell so `tmp` stays set.

## Supervised Mode and Status Streams

`--supervised` selects process-manager behavior: quiet human prose, no progress
spinner, no unstructured statistics, no ANSI color in machine-owned output, and
deterministic status handling. It does not enable status by itself.

Use one of these status options when your supervisor needs a final tagged
status line:

- `--status` enables one final status line on stderr.
- `--status=stderr` is the same behavior, stated explicitly.
- `--status=stdout` writes the status line to stdout only when stdout is not
  already owned by an artifact or informational stream.

For file-backed artifacts, `--status=stderr` is the usual supervised setting.
It leaves stdout empty, writes the artifact to the selected file, and puts the
final machine status on stderr.

## stdout and stderr Ownership

stdout has one owner per invocation:

- a selected artifact stream, such as `--output -`;
- an informational stream, such as help or version output;
- a tagged status stream, only when `--status=stdout` is legal;
- empty output.

stdout must not mix artifacts with diagnostics, progress, statistics, color, or
human summaries.

stderr owns human diagnostics, warnings, progress, statistics, and final status
when `--status` or `--status=stderr` is selected. Diagnostic prose can help a
person debug the command, but it is not the integration contract. Do not scrape
human prose or diagnostic prose to classify outcomes. Use the exit code and
the tagged status fields.

## Final `PANDAPI_STATUS`

When status output is enabled, the final record is a single tagged text line:

```text
PANDAPI_STATUS	status=ok	component=engine	surface=normal_search	surface_disposition=supported	exit_code=0	class=success	outcome=solved
```

Required fields are:

- `status=`
- `component=`
- `surface=`
- `surface_disposition=`
- `exit_code=`
- `class=`

Common conditional fields include `outcome=`, artifact role and path fields,
input or output path fields, `partial_output_policy=`, location or rule fields
for invalid input, timeout or resource details, dependency or child-process
details, and signal details. Unknown fields are omitted; supervisors should
accept additional fields without treating them as human prose.

The final status line is emitted after artifact disposition is known and before
the process exits. For successful file artifacts, the artifact is complete,
flushed, and closed before final status is emitted.

## Outcome Classification

Supervisors should first record the numeric exit code, then parse the final
`PANDAPI_STATUS` fields when present. The common status names and exit code
families are:

| Status | Exit code | Class | Integration meaning |
|--------|-----------|-------|---------------------|
| `ok` | `0` | success | Requested operation completed. |
| `domain_no_plan` | `2` | expected domain outcome | Valid input was processed, but no usable plan exists. |
| `cli_usage_error` | `10` | caller error | Invalid command shape, conflicting options, too many inputs, or malformed option value. |
| `input_unavailable` | `20` | caller error | Required input path or stream could not be opened or read. |
| `output_unavailable` | `21` | caller error | Output path, temp path, or artifact destination could not be created, opened, written, flushed, renamed, or cleaned up. |
| `input_invalid` | `22` | input/model error | Input was available but malformed, semantically invalid, truncated, inconsistent, or outside checked bounds. |
| `unsupported_feature` | `30` | policy failure | Requested feature is not supported by the 0.3.0 contract. |
| `legacy_surface` | `31` | policy failure | Legacy surface was reached where supervised behavior expected the supported contract. |
| `experimental_surface` | `32` | policy failure | Experimental surface was requested or failed inside fenced behavior. |
| `future_surface` | `33` | policy failure | Future surface is not available in this release. |
| `timeout` | `40` | external or policy failure | Configured time limit was exceeded. |
| `resource_limit` | `41` | external or policy failure | Configured memory, node, disk, file, recursion, or other resource policy was exceeded. |
| `interrupted` | `42` | external or policy failure | The binary handled an interrupt or termination request and exited through the status mapper. |
| `dependency_failure` | `50` | external or dependency failure | In-process dependency or adapter failed outside ordinary invalid input. |
| `child_process_failure` | `51` | external or dependency failure | Child process failed, timed out, was signaled, or returned an unmapped status. |
| `internal_error` | `60` | internal defect | Unexpected invariant, exception, or top-boundary failure. |
| `signal_terminated` | `128 + signal_number` | supervisor observation | The OS terminated the process before the binary could emit a final owned status. |

`domain_no_plan` is a valid domain result, not a generic failure. Missing
files, broken syntax, unavailable output paths, policy fences, resource limits,
and internal defects have separate status names and exit codes.

## ANSI, Color, and TTY Behavior

ANSI escape sequences are allowed only for human stderr when color is enabled
and stderr is a suitable TTY. ANSI is forbidden in stdout artifacts, tagged
status records, supervised output, machine-owned streams, files, pipes, and
no-TTY output.

Controls:

- `--color=auto` is the TTY-aware default.
- `--color=never` disables ANSI.
- `--no-color` is the canonical disable spelling.
- `--no-colour` is accepted as the alternate disable spelling.
- `NO_COLOR` disables ANSI through the environment.

Disabling controls win over enabling controls. Under `--supervised`, your
supervisor should expect no ANSI escape sequences in stdout, tagged status, or
supervised stderr.

## Successful Supervised Parser

This command writes the parser artifact to a file, keeps stdout empty, and
writes the final status to stderr:

```sh
./bin/pandapi-parser \
  --supervised \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl \
  >"$tmp/parser.stdout" \
  2>"$tmp/parser.stderr"

test -s "$tmp/minimal.htn"
test ! -s "$tmp/parser.stdout"
grep -E '^PANDAPI_STATUS	status=ok	component=parser' "$tmp/parser.stderr"
```

The supervisor can classify this as parser success from exit `0` and
`status=ok`, `component=parser`, `surface_disposition=supported`, and
`class=success`.

## Supervised Pipeline

This supervised pipeline keeps each generated artifact in the temporary
workspace and captures one status stream per process:

```sh
./bin/pandapi-parser \
  --supervised \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl \
  >"$tmp/parser.stdout" \
  2>"$tmp/parser.stderr"

./bin/pandapi-grounder \
  --supervised \
  --status=stderr \
  --output "$tmp/minimal.sas" \
  "$tmp/minimal.htn" \
  >"$tmp/grounder.stdout" \
  2>"$tmp/grounder.stderr"

./bin/pandapi-engine \
  --supervised \
  --status=stderr \
  --output "$tmp/minimal.plan" \
  "$tmp/minimal.sas" \
  >"$tmp/engine.stdout" \
  2>"$tmp/engine.stderr"

test -s "$tmp/minimal.htn"
test -s "$tmp/minimal.sas"
test -s "$tmp/minimal.plan"
grep -E '^PANDAPI_STATUS	status=ok	component=parser' "$tmp/parser.stderr"
grep -E '^PANDAPI_STATUS	status=ok	component=grounder' "$tmp/grounder.stderr"
grep -E '^PANDAPI_STATUS	status=ok	component=engine' "$tmp/engine.stderr"
```

Treat each process independently. A later step should run only after the prior
artifact is complete and the prior status is acceptable for that integration.

## Valid No-Plan Outcome

`fixtures/unsolvable` parses and grounds successfully, then the engine reports
that no usable plan exists. That is valid input with a no-plan outcome:

```sh
./bin/pandapi-parser \
  --supervised \
  --status=stderr \
  --output "$tmp/unsolvable.htn" \
  fixtures/unsolvable/domain.hddl \
  fixtures/unsolvable/problem.hddl \
  >"$tmp/parser.stdout" \
  2>"$tmp/parser.stderr"

./bin/pandapi-grounder \
  --supervised \
  --status=stderr \
  --output "$tmp/unsolvable.sas" \
  "$tmp/unsolvable.htn" \
  >"$tmp/grounder.stdout" \
  2>"$tmp/grounder.stderr"

set +e
./bin/pandapi-engine \
  --supervised \
  --status=stderr \
  --output "$tmp/unsolvable.plan" \
  "$tmp/unsolvable.sas" \
  >"$tmp/engine.stdout" \
  2>"$tmp/engine.stderr"
code="$?"
set -e

test "$code" -eq 2
grep -E "status=domain_no_plan|exit_code=2" "$tmp/engine.stderr"
```

Classify this as `domain_no_plan`, exit 2. Do not reclassify it as broken
input just because no plan artifact was produced.

## Negative Input Outcome

This example uses `fixtures/broken-syntax` to produce an invalid-input
classification. The supervisor checks status fields, not diagnostic wording:

```sh
set +e
./bin/pandapi-parser \
  --supervised \
  --status=stderr \
  --output "$tmp/broken.htn" \
  fixtures/broken-syntax/domain.hddl \
  fixtures/broken-syntax/problem.hddl \
  >"$tmp/parser.stdout" \
  2>"$tmp/parser.stderr"
code="$?"
set -e

test "$code" -eq 22
test ! -s "$tmp/parser.stdout"
grep -E "status=input_invalid|exit_code=22" "$tmp/parser.stderr"
```

Exit 22 and `status=input_invalid` mean the input was available but invalid.
For an unavailable output path, the corresponding classification is
`output_unavailable`, exit 21. Both are negative outcomes, but they are
different caller actions.

## stdout Status Conflict

`--status=stdout` is legal only when stdout is free for tagged status. It
conflicts with artifact stdout ownership. This command asks stdout to hold both
the parser artifact and the status line, so it is a usage error:

```sh
set +e
./bin/pandapi-parser \
  --supervised \
  --status=stdout \
  --output - \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl \
  >"$tmp/parser.stdout" \
  2>"$tmp/parser.stderr"
code="$?"
set -e

test "$code" -eq 10
grep -E "status=cli_usage_error|exit_code=10" "$tmp/parser.stdout"
```

Because this is a command-shape problem, the process should not enter model
processing or write an artifact. The supervisor classifies it as
`cli_usage_error`, exit 10, from the status stream.
