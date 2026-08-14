# Command Reference for pandaPI 0.3.0

This reference summarizes the supported `pandapi-*` command-line surface.
Start with the tutorial sequence if you are learning the workflow:

- [HTN and HDDL onboarding](../tutorial/01-htn-hddl-onboarding.md)
- [First project workflow](../tutorial/02-first-project-workflow.md)
- [Model a software feature as an HTN](../tutorial/03-model-a-feature-as-htn.md)
- [Write HDDL files for a small HTN project](../tutorial/04-write-hddl-files.md)

For supervised process integration, see
[Managed Process Integration](../managed-process.md). That guide gives the
full stdout, stderr, `PANDAPI_STATUS`, and process-supervisor treatment.

Run repository-root examples after building local binaries:

```sh
make build
tmp="$(mktemp -d)"
echo "$tmp"
```

Keep using the same shell so `$tmp` stays set.

## Supported Surfaces

pandaPI 0.3.0 supports one normal surface per binary:

| Command | Supported surface | Input | Output artifact |
|---------|-------------------|-------|-----------------|
| `pandapi-parser` | Normal HDDL domain/problem parsing to parser output. | `DOMAIN.hddl` and `PROBLEM.hddl` | `OUT.htn` |
| `pandapi-grounder` | Normal parser-generated `.htn` grounding to planner input. | `INPUT.htn` | `OUT.sas` |
| `pandapi-engine` | Normal search over parser/grounder-produced planner input. | `INPUT.sas` | `PLAN` plan artifact |

Other inherited or dormant surfaces are fenced. Parser helper modes are
legacy behavior. Grounder H2 is experimental; cpddl and FAM inference are
legacy advanced behavior. Engine interactive mode is legacy, translation is
experimental, SAT is unsupported, and BDD/CUDD is future work. These surfaces
are not supported normal behavior for the 0.3.0 command reference.

## Command Synopsis

The live help output uses these supported forms:

```text
pandapi-parser [COMMON] [--output OUT.htn|-] DOMAIN.hddl PROBLEM.hddl
pandapi-grounder [COMMON] [--output OUT.sas|-] INPUT.htn
pandapi-engine [COMMON] [--output PLAN|-] INPUT.sas
```

`--output PATH` writes the selected artifact to a file. `--output -` writes the
artifact to stdout, so stdout is no longer available for a status stream.

## Common Options

These options are common to `pandapi-parser`, `pandapi-grounder`, and
`pandapi-engine`:

| Option | Meaning |
|--------|---------|
| `--output PATH|-` | Write the parser, grounder, or plan artifact to `PATH`, or to stdout with `-`. |
| `--status` | Emit one final `PANDAPI_STATUS` record on stderr. |
| `--status=stderr` | Emit the final status record on stderr. |
| `--status=stdout` | Emit the final status record on stdout only when stdout is otherwise empty. |
| `--supervised` | Suppress inherited human prose, progress/statistics where applicable, and ANSI behavior for process-manager use. |
| `--quiet` | Suppress non-fatal human diagnostics. |
| `--verbose` | Permit additional human diagnostics on stderr. |
| `--color=auto|always|never` | Select human color behavior. `auto` is the default; `never` disables ANSI. |
| `--no-color` | Disable ANSI output. |
| `--no-colour` | Alternate spelling for `--no-color`. |
| `--help` | Show command help on stdout and exit successfully. |
| `--version` | Show concise version fields on stdout and exit successfully. |
| `--provenance` | Show provenance fields on stdout and exit successfully. |

Status output does not change the exit code. It adds a final machine-readable
line for callers that need stable classification.

## Output Ownership

Each invocation gives stdout exactly one owner:

- artifact output when `--output -` is selected;
- informational output for `--help`, `--version`, or `--provenance`;
- the tagged status stream when `--status=stdout` is legal;
- otherwise empty output.

stderr owns human diagnostics, warnings, progress, statistics, and final status
when `--status` or `--status=stderr` is selected. Diagnostic prose is for
people. Scripts and supervisors should classify from the exit code and
`PANDAPI_STATUS` fields, not from diagnostic prose.

For artifact files, the usual process-manager pattern is:

```sh
./bin/pandapi-parser \
  --supervised \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl \
  >"$tmp/parser.stdout" \
  2>"$tmp/parser.stderr"
```

The artifact is written to `$tmp/minimal.htn`, stdout stays empty, and stderr
contains the final status record.

## Status and Exit Summary

Use exit codes and status names for programmatic classification. Do not scrape
diagnostic prose.

| Status | Exit code | Meaning |
|--------|-----------|---------|
| `ok` | `0` | The requested operation completed. |
| `domain_no_plan` | `2` | Valid input was processed, but no usable plan exists. |
| `cli_usage_error` | `10` | Invalid command shape, conflicting options, too many inputs, or malformed option value. |
| `input_unavailable` | `20` | Required input path or stream could not be opened or read. |
| `output_unavailable` | `21` | Output path or artifact destination could not be created or written. |
| `input_invalid` | `22` | Input was available but malformed, semantically invalid, truncated, inconsistent, or outside checked bounds. |
| `unsupported_feature` | `30` | Requested feature is not supported by the 0.3.0 contract. |
| `legacy_surface` | `31` | Legacy surface was reached where supported managed behavior was expected. |
| `experimental_surface` | `32` | Experimental surface was requested or failed inside fenced behavior. |
| `future_surface` | `33` | Future surface is not available in this release. |
| `timeout` | `40` | Configured time limit was exceeded. |
| `resource_limit` | `41` | Configured resource policy was exceeded. |
| `interrupted` | `42` | The binary handled an interrupt or termination request through the status mapper. |
| `dependency_failure` | `50` | In-process dependency or adapter failed outside ordinary invalid input. |
| `child_process_failure` | `51` | Child process failed, timed out, was signaled, or returned an unmapped status. |
| `internal_error` | `60` | Unexpected invariant, exception, or top-boundary failure. |
| `signal_terminated` | `128 + signal_number` | The OS terminated the process before the binary could emit a final owned status. |

`domain_no_plan` is a valid domain result, not malformed input and not an
internal failure.

## Parser Reference

Usage:

```text
pandapi-parser [COMMON] [--output OUT.htn|-] DOMAIN.hddl PROBLEM.hddl
```

`pandapi-parser` reads one HDDL domain and one HDDL problem, then writes a
parser artifact.

Runnable example:

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl
test -s "$tmp/minimal.htn"
```

To stream the parser artifact to stdout, use `--output -` and do not also ask
for `--status=stdout`:

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output - \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl \
  >"$tmp/minimal.htn"
test -s "$tmp/minimal.htn"
```

## Grounder Reference

Usage:

```text
pandapi-grounder [COMMON] [--output OUT.sas|-] INPUT.htn
```

`pandapi-grounder` reads a parser-generated `.htn` artifact and writes a
grounded planner artifact.

Runnable example:

```sh
./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/minimal.sas" \
  "$tmp/minimal.htn"
test -s "$tmp/minimal.sas"
```

## Engine Reference

Usage:

```text
pandapi-engine [COMMON] [--output PLAN|-] INPUT.sas
```

`pandapi-engine` reads a grounded planner artifact and writes a plan artifact
when search finds a plan.

Runnable example:

```sh
./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/minimal.plan" \
  "$tmp/minimal.sas"
test -s "$tmp/minimal.plan"
cat "$tmp/minimal.plan"
```

For a valid no-plan case:

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/unsolvable.htn" \
  fixtures/unsolvable/domain.hddl \
  fixtures/unsolvable/problem.hddl

./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/unsolvable.sas" \
  "$tmp/unsolvable.htn"

set +e
./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/unsolvable.plan" \
  "$tmp/unsolvable.sas" \
  >"$tmp/unsolvable.stdout" \
  2>"$tmp/unsolvable.stderr"
code="$?"
set -e

test "$code" -eq 2
grep -E "status=domain_no_plan|exit_code=2" "$tmp/unsolvable.stderr"
```

That outcome means valid input reached search, and search found no usable
plan.
