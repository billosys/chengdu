# Architecture and Source Quality for pandaPI 0.3.0

This page explains the 0.3.0 product shape and the engineering quality
posture behind it. It is for readers who want to understand where the parser,
grounder, engine, runtime, dependencies, generated code, and release
preparation boundaries sit before relying on the public commands.

For hands-on use, start with the tutorial sequence:

- [HTN and HDDL onboarding](tutorial/01-htn-hddl-onboarding.md)
- [First project workflow](tutorial/02-first-project-workflow.md)
- [Model a software feature as an HTN](tutorial/03-model-a-feature-as-htn.md)
- [Write HDDL files for a small HTN project](tutorial/04-write-hddl-files.md)

For command and process details, see `docs/reference/cli.md`
([CLI command reference](reference/cli.md)), `docs/managed-process.md`
([Managed Process Integration](managed-process.md)), and `docs/migration.md`
([Migration from 0.2.0 to 0.3.0](migration.md)).

## Product Shape

pandaPI 0.3.0 is an in-tree product fork. The product source lives under
`pandaPI/`, with four active component roots:

| Path | Role |
|------|------|
| `pandaPI/parser` | Reads HDDL domain/problem input and produces the parser artifact consumed by the grounder. |
| `pandaPI/grounder` | Reads parser output and produces the grounded planner artifact consumed by the engine. |
| `pandaPI/engine` | Searches grounded planner input and writes a plan artifact when a plan is found. |
| `pandaPI/runtime` | Provides shared runtime support for status, diagnostics, TTY/color, provenance, and testable process-policy behavior. |

The supported public command surface is the canonical `pandapi-*` binaries:
`pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`. The normal
pipeline is parser -> grounder -> engine. Optional or inherited internal
surfaces do not become supported public behavior just because source code or
flags exist for them; the command reference records the current surface
classification.

## Managed Process Architecture

The managed process architecture separates planner work from process policy.
Each binary owns one normal operation, while shared runtime helpers make the
process boundary predictable for both terminal users and supervisors.

At the process boundary:

- stdout has one owner: artifact output, informational output, a legal status
  stream, or empty output;
- stderr owns human diagnostics and the default status stream;
- a final `PANDAPI_STATUS` record gives machine-readable fields when status
  output is enabled;
- supervisors classify from exit code and status fields, not from diagnostic
  prose;
- supervised operation keeps machine-owned streams ANSI-free.

The detailed contract is in `docs/managed-process.md`. The command syntax,
common options, and status/exit summary are in `docs/reference/cli.md`. The
0.2.0 to 0.3.0 behavior-change summary is in `docs/migration.md`.

## Source Quality

All in-tree product components are maintained by this fork in the product
sense. Quality gates are routed by source class rather than by the older idea
that only runtime code was local and planner code was inherited.

| Source class | Reader-facing meaning | Quality posture |
|--------------|-----------------------|-----------------|
| first-party maintained | Hand-maintained product, adapter, runtime, test, build, or integration source shipped or maintained by this fork. This includes parser, grounder, engine, runtime, and maintained copied-origin code such as `rss.c`. | In scope for formatting, static analysis, coverage evidence, warning policy, sanitizer triage, and unit or seam-test strategy. |
| first-party generated | Committed generated output whose grammar, spec, or template is maintained by this fork. | Prefer fixing the generator or template. Do not hand-edit generated output as the normal remedy. |
| first-party generator/template | Maintained inputs such as Flex, Bison, gengetopt, Make, and CMake files that generate other code or build surfaces. | Review and fix these as source when the root cause belongs there. |
| vendored third-party | External dependency source carried in tree. | Not first-party cleanup scope unless a later dependency audit accepts that boundary. |
| dependency-internal | Source inside a dependency or private dependency implementation reached through a first-party integration boundary. | Report separately from first-party source. First-party adapters remain in scope. |
| third-party generated | Generated output owned by a third-party project or skeleton inside a dependency. | Exclude or suppress only with dependency rationale; cleanup requires dependency-audit re-entry. |
| copied build artifact | Disposable source copies, placeholders, objects, archives, or binaries retained for build mechanics. | Not source-quality ownership. Map diagnostics back to canonical `pandaPI/<component>` source. |
| generated build output | Build, dist, CMake, CTest, coverage, warning inventory, sanitizer, packaging, and release output. | Evidence, not source. |

This source class policy lets the project improve first-party code without
silently taking ownership of every dependency-internal warning or generated
skeleton issue.

## Generated Code

Generated-code work follows root cause, ownership, correctness, remedy, and
re-entry. The first question is not "which generated file has a warning?" but
"which maintained input, generated output, or third-party skeleton owns this
behavior?"

The policy is:

- do not hand-edit generated output as the default fix;
- fix owned generator/template inputs when the defect belongs there;
- distinguish owned generator inputs from third-party skeleton noise;
- record correctness impact before suppressing, excluding, or closing a
  generated warning;
- keep gengetopt option-parser output separate from the selected CLI11
  replacement path.

The parser scanner is the concrete closure example. Flex generated helper
warnings in `hddl-token.cpp` were traced back to `hddl-token.l`: the current
scanner rules do not call `input()` or `unput()`, so the maintained scanner
input now declares `%option noinput` and `%option nounput`. That remedy changes
the generator/template input and lets Flex omit unused helpers. It does not
hand-edit `hddl-token.cpp`.

By contrast, gengetopt output from `options.ggo` and generated `cmdline.*`
files remains separate. The current managed native wrappers own public process
behavior; generated human diagnostic prose is not final managed-process
contract evidence. Option-source cleanup and the selected CLI11 migration are
re-entry work, not a reason to patch generated files by hand.

## Third-Party Boundaries

The grounder and engine carry third-party and dependency-internal source that
must be kept separate from first-party obligations:

| Boundary | Current posture |
|----------|-----------------|
| `cpddl` | Vendored third-party and dependency-internal grounder dependency. First-party integration code remains in scope, but cpddl internals require dependency audit before becoming cleanup obligations. |
| H2 | Vendored dependency-internal grounder code. H2 remains fenced from the supported normal command surface. |
| boruvka | Nested third-party source under cpddl. Warning or sanitizer findings are separately reported unless a dependency audit accepts the boundary. |
| opts | Nested third-party source under cpddl. It is not first-party source-quality scope by default. |
| lpsolve | Nested third-party and third-party generated source under cpddl. Cleanup requires dependency-audit re-entry. |
| CUDD | Vendored third-party engine dependency for BDD/CUDD future work. It is excluded from first-party gates unless BDD/CUDD support and dependency audit are accepted later. |
| `rss.c` | Maintained copied-origin RSS utility source compiled from `pandaPI/grounder/src`. It is currently first-party maintained for grounder gates until a dependency audit changes that boundary. |

Dependency internals are not first-party cleanup obligations unless a later
dependency audit accepts that boundary. That rule avoids promoting optional or
fenced implementation details into public support promises.

## Dependencies

The dependency posture is intentionally small and owned through local
interfaces.

| Dependency posture | Current decision |
|--------------------|------------------|
| standard library | Baseline modernization path for ownership, paths, strings, streams, checked access, local hashers, and time/resource accounting. |
| fmt | Accepted formatting primitive under owned diagnostics/process surfaces. Direct planner-internal stdout/stderr formatting remains the wrong boundary. |
| CLI11 | Selected replacement path for command-line parsing. It has not replaced current parser, grounder, or engine option parsing yet; migration requires golden help, usage, stream, and status tests. |
| Catch2 | Accepted as a test-only seam/unit-test dependency when present. It proves internal seams, while managed process fixtures prove executable contracts. |
| expected-style results | Accepted only behind owned status/result surfaces, such as local status/result facades. Do not leak a dependency vocabulary through every planner header. |
| reproc++ | Conditional only if a supported child-process boundary is accepted later and a local wrapper is insufficient. |
| held or rejected broad dependencies | JSON, broad GSL use, performance containers, benchmarking, parser-generator alternatives, optional SAT/BDD/CUDD/H2/cpddl work, Abseil, and Boost.Process stay out unless a later decision re-enters them with evidence. |

CLI11 is selected policy and re-entry work, not a vague possibility. The
important distinction is timing: the replacement path is chosen, but the
current binaries still use existing option-parsing surfaces until the migration
lands with tests.

## Make-Backed Quality Gates

Make is the developer entrypoint. Run `make help` for the current target list.
The source-quality page treats named targets as evidence anchors only when
they exist as Make targets and are routed through source-class policy.

Current source-quality and release-preparation anchors include:

| Gate family | Make target examples | What they prove today |
|-------------|----------------------|-----------------------|
| source surface | `make source-quality-surface`, `make source-quality-profile-map`, `make source-quality-naming-check`, `make compile-db-first-party` | Source classes, naming, compile-database selection, and profile/source mapping are current and reproducible. |
| reports | `make source-quality-gate-report`, `make warning-inventory-first-party`, `make generated-warning-triage`, `make generated-warning-triage-parser` | Current source-quality evidence, warning classes, and generated-warning triage are documented by source class. |
| formatting and static analysis | `make format-check`, `make static-analysis` | Runtime strict gates and broader baselines are routed through current selectors. Component baselines are evidence until stricter gates are accepted. |
| coverage | `make coverage` and component coverage targets where present | Measured baseline coverage evidence, not a public release floor. |
| seam and contract tests | CTest/Catch2-style seam tests when present, `make test`, and managed process fixtures | Internal seams and executable contracts are different kinds of evidence. |
| sanitizers | `make test-binary-sanitize` and component sanitizer targets where present | ASan/UBSan evidence for binaries and selected seams, with source-class triage. |
| safety and workflow | `make safety-checks`, `make actionlint`, `make provenance-check` | Repository safety checks, workflow linting, and provenance evidence for release preparation. |

Managed process fixtures prove executable contracts: exit codes, stdout,
stderr, artifacts, and final status behavior. CTest/Catch2-style seam tests
prove internal seams when those seams exist. Neither replaces the other.

## Measured Baselines and Budgets

The current posture includes measured baseline evidence, not universal quality
guarantees.

Accepted measured baseline examples:

- runtime coverage: 74.09% line coverage, no release floor;
- engine visited-list seam coverage: 66.67% line coverage, no release floor;
- grounder topsort seam coverage: 63.79% line and 81.13% region coverage, no
  release floor;
- warning-inventory output reports warning lines by class rather than using a
  global fail threshold;
- grounder maintained-source warning count: 0 in the current first-party
  inventory;
- engine and grounder format/static-analysis evidence is baseline evidence
  until stricter component gates are accepted.

No project-wide coverage threshold is set. The project also makes no global
zero-warning claim. Those are budget and re-entry decisions, not facts created
by the current evidence.

Current deferred or budgeted work:

- parser maintained-source burndown remains re-entry for strict parser
  formatting, parser clang-tidy, parser coverage baseline, parser unit/seam
  tests, and parser internal sanitizer seams;
- gengetopt cleanup and CLI11 migration remain re-entry for parser, grounder,
  and engine;
- stricter generated-code policy remains re-entry after root-cause and
  correctness triage;
- dependency audits remain re-entry for cpddl, H2, boruvka, opts, lpsolve,
  CUDD, and third-party generated output;
- Linux LeakSanitizer remains release-preparation re-entry on a capable Linux
  toolchain;
- TSan remains deferred until supported concurrent process observation,
  child-process stream draining, or planner concurrency gives it a meaningful
  target.

## Release Preparation

This page is not publication proof. Release assets, checksums, manifest
contents, license and NOTICE bundles, test-only dependency exclusion, wolong
fetch/install/migration behavior, and publication idempotency still need
release preparation verification.

Until those release checks run, treat this page as architecture and
source-quality context. Release assets are not published by this page, and
published-installation claims should be verified when published.
