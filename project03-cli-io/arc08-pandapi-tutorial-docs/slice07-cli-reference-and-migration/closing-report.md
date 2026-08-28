# Arc08 Slice07 Closing Report: cli-reference-and-migration

Status: implementation proposed done, awaiting CDC verification.

## Scope

Slice07 created the public CLI reference at `docs/reference/cli.md`, created
the public migration guide at `docs/migration.md`, and updated Arc08/project
planning status. It did not touch README, release/package/publish surfaces,
`.github/`, Makefile or `mk/`, tests, tools, fixtures, parser/grounder/engine
runtime source, licenses, or wolong integration.

## Ledger Walk

| Row | Result | Evidence |
|-----|--------|----------|
| F-1 | done | `docs/reference/cli.md` exists and identifies the canonical command reference surface. |
| F-2 | done | `docs/migration.md` exists and covers the 0.2.0 to 0.3.0 transition. |
| F-3 | done | The CLI reference documents parser, grounder, and engine synopsis lines with input and output artifacts. |
| F-4 | done | Live `./bin/pandapi-parser --help`, `./bin/pandapi-grounder --help`, and `./bin/pandapi-engine --help` passed after `make build`; usage lines, common options, and supported-surface wording were checked against the page. |
| F-5 | done | The common-options section covers output, status, supervised mode, quiet/verbose, color, help, version, and provenance. |
| F-6 | done | The reference explains stdout/stderr/artifact/status ownership and links to `docs/managed-process.md`. |
| F-7 | done | The reference summarizes status and exit-code classes and says diagnostic prose is not final classification evidence. |
| F-8 | done | The reference separates supported, legacy, experimental, unsupported, and future surfaces without treating non-supported surfaces as supported contracts. |
| F-9 | done | Repository-root examples use `./bin/pandapi-*`, checked-in fixtures, and temporary output directories. |
| F-10 | done | Representative example gates passed locally: `make build`, `make smoke` with `3 passed, 0 failed`, and `make smoke-negative` with `4 passed, 0 failed`. |
| F-11 | done | `make test-contract-pipeline-managed` passed with `129 passed, 0 failed`. |
| F-12 | done | The migration guide names the 0.2.0 to 0.3.0 command-name change and does not promise inherited command compatibility. |
| F-13 | done | The migration table maps `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` to the canonical `pandapi-*` names. |
| F-14 | done | The behavior-change table covers command names, managed status, exit codes, stdout/stderr ownership, artifact output, no-plan, invalid input, ANSI/color, and unsupported surfaces. |
| F-15 | done | The migration guide gives source-build/local-run commands and limits installation wording to release assets verified when published. |
| F-16 | done | Public pages link to the tutorial sequence and managed-process guide. |
| F-17 | done | `docs/reference/cli.md` has no inherited command-name matches. |
| F-18 | done | In `docs/migration.md`, inherited command names appear only in historical transition, mapping, and no-compatibility context. |
| F-19 | done | Public pages contain no internal planning vocabulary from the forbidden list. |
| F-20 | done | Arc08 `arc-plan.md` and the project plan mark Slice07 proposed done and keep Slice08 as next. |
| F-21 | done | Staged protected-boundary check passed; changed paths stayed in public docs and Arc08/project design-planning surfaces. |
| F-22 | done | `git diff --check` and `git diff --cached --check` passed after staging. |
| F-23 | done | This report walks F-1 through F-23 and includes Bubble-up to Arc08 with Slice08, README, and release handoff notes. |

## Verification

The static ledger greps passed for F-1 through F-3, F-5 through F-9, F-12
through F-20, and F-23.

Live binary help was checked with:

```sh
make build
./bin/pandapi-parser --help
./bin/pandapi-grounder --help
./bin/pandapi-engine --help
```

The help output provided the canonical command usage, common options, and
surface notes used in the CLI reference. Version and provenance output was also
checked for all three binaries while reconciling the common options.

Representative CLI examples and managed contracts passed:

```sh
make build
make smoke
make smoke-negative
make test-contract-pipeline-managed
```

Results:

- `make smoke`: `3 passed, 0 failed`
- `make smoke-negative`: `4 passed, 0 failed`
- `make test-contract-pipeline-managed`: `129 passed, 0 failed`

Aggregate `make check` was intentionally not run. This was a docs-writing
slice that changed no code, Make, CI, tests, tools, release/package, fixtures,
or other behavior-bearing surfaces. The narrower gates were the F-1 through
F-23 ledger checks, live binary help/version/provenance checks, smoke gates,
managed pipeline contract gate, staged protected-boundary check, and whitespace
checks.

## Bubble-up to Arc08

Slice07 delivers the public command reference and migration path at
implementation-proposed-done strength. The reference now documents the
canonical parser, grounder, and engine commands; separates artifact streams
from status evidence; and points readers to managed-process details when they
need supervisor-facing behavior.

Slice08 remains next for architecture and source-quality documentation. README
should wait until the complete tutorial/reference/architecture set is stable,
then link to the finished public docs sequence. Release work has no asset or
installer change from this slice; the handoff is limited to public migration
wording that says installation assets are verified when published.
