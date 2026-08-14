# pandaPI 0.3.0 Documentation

This is the public documentation home for chengdu's pandaPI 0.3.0 source
tree. Start with the tutorial sequence if you are new to HTN planning or HDDL,
then use the integration and reference pages when you are ready to automate or
migrate real workflows.

## Tutorial Path

1. [HTN and HDDL onboarding](tutorial/01-htn-hddl-onboarding.md)
2. [First project workflow](tutorial/02-first-project-workflow.md)
3. [Model a software feature as an HTN](tutorial/03-model-a-feature-as-htn.md)
4. [Write HDDL files for a small HTN project](tutorial/04-write-hddl-files.md)

The tutorial path starts with concepts, runs the checked-in minimal fixture,
turns a software feature request into an HTN model, and then writes temporary
HDDL files that can be parsed, grounded, and solved locally.

## Managed Process

[Managed Process Integration](managed-process.md) is for supervisors and
process managers. It covers `--supervised`, `--status`, stdout/stderr
ownership, final `PANDAPI_STATUS` records, exit/status classification, color
policy, and negative outcomes.

Classify automated runs from exit codes and tagged status fields. Human
diagnostic prose is context for people, not a stable integration contract.

## CLI

[CLI Reference](reference/cli.md) documents the supported `pandapi-parser`,
`pandapi-grounder`, and `pandapi-engine` surfaces, common options, artifact
ownership, status output, and live command shapes.

Repository-root examples use local source-build binaries under `./bin/`:

```sh
make build
tmp="$(mktemp -d)"
./bin/pandapi-parser --help >/dev/null
./bin/pandapi-grounder --help >/dev/null
./bin/pandapi-engine --help >/dev/null
```

## Migration

[Migration from 0.2.0 to 0.3.0](migration.md) explains the command-name and
managed-process behavior changes. New scripts and examples should use the
canonical `pandapi-*` commands.

0.3.0 release assets are not published yet. Until the release is published
and verified, local migration examples use `make build` and `./bin/pandapi-*`
from the repository root.

## Architecture

[Architecture and Source Quality](architecture.md) explains the in-tree
`pandaPI/` product shape, parser/grounder/engine/runtime roles, source-class
policy, generated-code posture, dependency boundaries, Make-backed quality
gates, measured baselines, and release-preparation boundaries.

Release assets, checksums, manifest/provenance, license and NOTICE bundles,
release notes, wolong fetch/install proof, and publication remain release
preparation work until they are verified for the release.
