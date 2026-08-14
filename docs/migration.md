# Migration from pandaPI 0.2.0 to 0.3.0

pandaPI 0.3.0 changes the public command surface. Version 0.2.0 was the
transition release from the old three-repository, inherited-command world into
chengdu's monorepo and prebuilt-binary distribution. Version 0.3.0 uses the
canonical `pandapi-*` commands and the managed-process behavior documented in
this docs suite.

The inherited `pandaPI*` command names are historical names for migration
mapping. They are not a 0.3.0 compatibility guarantee, and they should not be
used in new scripts, examples, or process-manager integrations.

For command details, see the [CLI command reference](reference/cli.md). For
supervised execution, see
[Managed Process Integration](managed-process.md). The tutorial sequence is:

- [HTN and HDDL onboarding](tutorial/01-htn-hddl-onboarding.md)
- [First project workflow](tutorial/02-first-project-workflow.md)
- [Model a software feature as an HTN](tutorial/03-model-a-feature-as-htn.md)
- [Write HDDL files for a small HTN project](tutorial/04-write-hddl-files.md)

## Command Rename Mapping

Rename scripts and documentation to the canonical commands:

| 0.2.0 or inherited name | 0.3.0 canonical command | Migration action |
|-------------------------|--------------------------|------------------|
| `pandaPIparser` | `pandapi-parser` | Rename parser invocations and use `--output` for the `.htn` artifact. |
| `pandaPIgrounder` | `pandapi-grounder` | Rename grounder invocations and pass the parser-generated `.htn` input. |
| `pandaPIengine` | `pandapi-engine` | Rename engine invocations and pass the grounded `.sas` input. |

Do not treat the old names as supported aliases. The safe migration is to
change callers to `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`.

## Local Source Build

From the repository root:

```sh
make build
tmp="$(mktemp -d)"
echo "$tmp"
```

Run the canonical pipeline:

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl

./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/minimal.sas" \
  "$tmp/minimal.htn"

./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/minimal.plan" \
  "$tmp/minimal.sas"
```

Release assets, published release installation, and installer-specific
commands are verified when published. Until then, local migration examples use
`make build` and `./bin/pandapi-*` from the repository root.

## Behavior Change Table

| Behavior Change | 0.2.0 / inherited behavior | 0.3.0 migration behavior |
|-----------------|----------------------------|--------------------------|
| command names | Existing scripts may call `pandaPIparser`, `pandaPIgrounder`, or `pandaPIengine`. | Rename to `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`; inherited `pandaPI*` names are not a compatibility guarantee. |
| managed status | Callers often inferred results from mixed streams or process-specific output. | Use `--status`, `--status=stderr`, or `--status=stdout` for one final `PANDAPI_STATUS` record. |
| exit codes | Some outcomes were collapsed, inherited, or ambiguous. | Classify by documented exit codes: `0` for `ok`, `2` for `domain_no_plan`, `10` for `cli_usage_error`, `20`/`21` for unavailable input/output, `22` for `input_invalid`, and separate policy, resource, dependency, and internal families. |
| stdout | stdout could contain a mixture of artifacts, progress, summaries, or diagnostic text depending on the tool and path. | stdout has one owner: artifact, informational output, legal status output, or empty. |
| stderr | Human diagnostics were not consistently separated from machine-observed state. | stderr owns human diagnostics and the default status stream; diagnostic prose is not the process contract. |
| artifact output | Output position and stream ownership differed by inherited tool behavior. | Use `--output PATH` for file artifacts, or `--output -` when stdout should carry the artifact. |
| no-plan | No-plan handling could be confused with generic failure. | Valid no-plan search returns exit `2` with `domain_no_plan`; it is a domain outcome, not invalid input. |
| invalid input | Syntax, semantic, missing-file, and internal failures could be difficult to separate. | Missing input is `input_unavailable`; malformed or inconsistent input is `input_invalid`; internal defects are `internal_error`. |
| ANSI color | Color and no-color behavior varied across inherited paths. | Use `--color=auto|always|never`, `--no-color`, or `--no-colour`; supervised and machine-owned streams must be ANSI-free. |
| unsupported surfaces | Optional or inherited surfaces could look available because code or flags existed. | Parser helpers are legacy, grounder H2 is experimental, cpddl/FAM is legacy, engine interactive is legacy, translation is experimental, SAT is unsupported, and BDD/CUDD is future work. |

## Status and Exit Migration

Update supervisors to branch on numeric exit code first, then parse final
status fields when enabled:

| Status | Exit code | Migration meaning |
|--------|-----------|-------------------|
| `ok` | `0` | Operation completed. |
| `domain_no_plan` | `2` | Valid input was processed, but no usable plan exists. |
| `cli_usage_error` | `10` | Fix the command shape or option combination. |
| `input_unavailable` | `20` | Fix a missing or unreadable input path or stream. |
| `output_unavailable` | `21` | Fix an unwritable output path or artifact destination. |
| `input_invalid` | `22` | Fix malformed, inconsistent, or semantically invalid input. |
| `unsupported_feature` | `30` | Remove the unsupported request from the 0.3.0 invocation. |
| `legacy_surface` | `31` | Do not use that inherited surface as supported 0.3.0 behavior. |
| `experimental_surface` | `32` | Treat the requested path as experimental and outside the normal surface. |
| `future_surface` | `33` | The requested path is not available in this release. |
| `timeout` | `40` | The configured time limit was exceeded. |
| `resource_limit` | `41` | A configured resource policy was exceeded. |
| `interrupted` | `42` | The binary handled termination through the status mapper. |
| `dependency_failure` | `50` | A dependency boundary failed outside ordinary invalid input. |
| `child_process_failure` | `51` | A child process failed or returned unmapped state. |
| `internal_error` | `60` | Report as a tool defect or unexpected top-boundary failure. |
| `signal_terminated` | `128 + signal_number` | The supervising process observed OS signal termination. |

Do not classify by matching human diagnostics. Human text can help a person
debug, but callers should use exit code and `PANDAPI_STATUS`.

## Migration Checklist

1. Replace inherited command names with canonical `pandapi-*` command names.
2. Add explicit `--output PATH` for each artifact your workflow needs.
3. Add `--status=stderr` when a supervisor needs stable machine state.
4. Treat stdout as either artifact output, informational output, legal status
   output, or empty; do not mix owners.
5. Update no-plan handling to expect `domain_no_plan` with exit `2`.
6. Update invalid-input handling to distinguish missing input from malformed
   input.
7. Disable color for machine-owned output with `--supervised`, `--no-color`,
   `--no-colour`, or `--color=never`.
8. Remove reliance on legacy, experimental, unsupported, or future surfaces
   unless your workflow explicitly accepts their non-supported status.
