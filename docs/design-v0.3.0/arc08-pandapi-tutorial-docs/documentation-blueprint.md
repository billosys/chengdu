# pandaPI 0.3.0 Documentation Blueprint

Arc08 owns the pandaPI 0.3.0 tutorial and documentation suite for readers who
need to learn the product, run the canonical commands, integrate the managed
process contract, or understand the architecture and release posture. The
suite should turn the verified Arc03 through Arc07 evidence into public
documentation without copying planning-document noise into user-facing prose.

The public docs home is `docs/`. Tutorial pages live under `docs/tutorial/`.
Design, planning, implementation tracking, ledgers, and CDC/CC evidence remain
under `docs/design-v0.3.0/`.

## Reader Journeys

| Reader | Journey | First useful page | Success condition |
|--------|---------|-------------------|-------------------|
| Beginner concept learner | Learn what HTN planning is, how PDDL/HDDL input relates to pandaPI, and why the pipeline has parser, grounder, and engine stages. | `docs/tutorial/01-htn-hddl-onboarding.md` | The reader can explain domain/problem input, `.htn`, `.sas`, plan output, solved, and no-plan outcomes before seeing dense flags. |
| CLI workflow user | Run a simple parse -> ground -> solve workflow with `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`. | `docs/tutorial/02-first-project-workflow.md` | The reader can build, run checked-in examples, inspect artifacts, and repeat the workflow locally. |
| HTN modelling learner | Turn a real-world planning issue into an HTN model before writing syntax. | `docs/tutorial/03-model-a-feature-as-htn.md` | The reader can identify objects, facts, compound tasks, primitive actions, methods, ordering constraints, and solved/no-plan boundaries from prose. |
| HDDL author | Write runnable `domain.hddl` and `problem.hddl` files from the HTN model. | `docs/tutorial/04-write-hddl-files.md` | The reader can author, validate, parse, ground, and solve a small HDDL project without starting from a prepared fixture. |
| Managed-process integrator | Integrate supervised process execution using `--supervised`, `--status`, stdout/stderr ownership, exit codes, and final `PANDAPI_STATUS`. | `docs/managed-process.md` | The reader can classify outcomes from exit code and tagged status fields, not human diagnostic prose. |
| Maintainer/release architecture reader | Understand the 0.3.0 fork shape, source-quality posture, dependency boundaries, Make gates, and Arc09 release handoff. | `docs/architecture.md` | The reader knows what is supported, what remains deferred, and which release claims still require Arc09 proof. |

## Public Docs Home And Page Map

`docs/` is the public docs home because it keeps reader-facing project
documentation outside the design/planning tree while giving README and release
notes stable link targets. `docs/design-*` paths are for planning, design, and
implementation tracking artifacts only.

Recommended file map and page map:

| Path | Owner slice | Purpose |
|------|-------------|---------|
| `docs/index.md` | Slice09 README entrypoint refresh or Slice10 synthesis if anchors settle later | Public docs landing page with links to tutorial, reference, migration, managed-process, and architecture pages. |
| `docs/tutorial/01-htn-hddl-onboarding.md` | Slice02 | Beginner tutorial for HTN, PDDL, HDDL, pipeline concepts, artifacts, solved/no-plan language, and `fixtures/minimal`. |
| `docs/tutorial/02-first-project-workflow.md` | Slice03 | Runnable CLI tutorial using checked-in fixtures and canonical `pandapi-*` commands. |
| `docs/managed-process.md` | Slice04 | Supervised integration guide for stdout, stderr, `--supervised`, `--status`, `PANDAPI_STATUS`, exit/status taxonomy, color/TTY, and negative outcomes. |
| `docs/tutorial/03-model-a-feature-as-htn.md` | Slice05 | Real-world modelling tutorial that turns a software feature request into an HTN model before writing HDDL syntax. |
| `docs/tutorial/04-write-hddl-files.md` | Slice06 | HDDL authoring tutorial that turns the Slice05 model into `domain.hddl` and `problem.hddl`, then validates it through the pandaPI workflow. |
| `docs/reference/cli.md` | Slice07 | Command reference for `pandapi-parser`, `pandapi-grounder`, `pandapi-engine`, common options, statuses, and supported surfaces. |
| `docs/migration.md` | Slice07 | 0.2.0 -> 0.3.0 behavior-change table and migration guidance. |
| `docs/architecture.md` | Slice08 | Architecture, dependency, source-quality, source-class, generated-code, and third-party boundary documentation. |
| `README.md` | Slice09 | Short public entry point into the docs suite after stable anchors exist. |

## Evidence Inputs

The documentation suite should cite or summarize these evidence sources:

- Arc03 managed-process contract: supported surfaces, status vocabulary, exit
  codes, stdout/stderr ownership, tagged `PANDAPI_STATUS`, color/TTY rules,
  and the contract-test matrix.
- Arc05 binary contract adoption: canonical native `pandapi-*` binaries are
  the accepted 0.3.0 surface. Inherited `pandaPI*` names are not supported
  compatibility guarantees for public 0.3.0 examples.
- Arc06 CI and test hardening: managed parser, grounder, engine, and pipeline
  fixtures; `make test`; `make smoke`; negative smoke outcomes; sanitizer,
  coverage, static-analysis, warning, and CI/local gate routing.
- Arc07 source-quality synthesis: source-quality source classes, generated-code
  policy, dependency boundaries, accepted measured baselines, and explicit
  caution against public coverage-floor or zero-warning claims.
- `README.md`: current build-from-source shape, release-install wording,
  top-level Make entrypoint posture, canonical command names, CI notes, and
  release boundaries.
- `fixtures/`: `fixtures/minimal`, `fixtures/unsolvable`,
  `fixtures/broken-syntax`, `fixtures/broken-reference`,
  `fixtures/grounder/minimal.htn`, `fixtures/grounder/malformed.htn`,
  `fixtures/engine/minimal.sas`, `fixtures/engine/unsolvable.sas`, and
  `fixtures/contract/*-records.md`.
- Make targets from `make help` and `mk/help.mk`: build, component builds,
  `make test`, `make smoke`, `make smoke-negative`, contract fixture targets,
  source-quality targets, coverage targets, sanitizer targets, and release
  utility targets where later docs need command evidence.

## Command-Name Policy

Public 0.3.0 examples use only:

- `pandapi-parser`
- `pandapi-grounder`
- `pandapi-engine`

The inherited `pandaPI*` names may appear only as historical migration context:
0.2.0 was the transition release, and 0.3.0 documentation teaches the canonical
`pandapi-*` interface. Older Arc03 text that planned inherited-name executable
compatibility is historical design input, not a compatibility guarantee for
Arc08 public docs. Migration prose should say what changed and what users must
rename, without implying old names remain a supported managed-process surface.

## Example Strategy

Beginner example:

- Use `fixtures/minimal/domain.hddl` and `fixtures/minimal/problem.hddl` for
  the first HTN/PDDL/HDDL explanation and the first parser command.
- Show the parser output as a generated `.htn` artifact, then feed that into
  the grounder and engine in the Slice03 workflow.

Intermediate example:

- Use the checked-in pipeline path from `fixtures/minimal` through generated
  `.htn` and `.sas` artifacts, then compare it with `fixtures/unsolvable` to
  teach solved versus `domain_no_plan`.
- Use `broken-syntax` and `broken-reference` only after the happy path, as
  negative examples for syntax and semantic invalid-input outcomes.

Real-world modelling example:

- Use a software engineering feature request as the modelling bridge from
  prose to HTN. The tutorial should identify the planning question, actors or
  artifacts as objects, predicates/facts, compound tasks, primitive actions,
  methods, ordering constraints, and solved/no-plan boundaries before showing
  HDDL syntax.
- Slice05 resolved that the modelling page does not need a checked-in fixture:
  its examples are conceptual and intentionally pre-HDDL. Slice06 should reuse
  the same `status-summary` feature model and decide whether runnable HDDL
  proof belongs in a new fixture or in temporary files created by the
  tutorial commands.

HDDL authoring example:

- Turn the modelling example into `domain.hddl` and `problem.hddl` with a
  line-by-line explanation of domain sections, problem sections, task network,
  methods, actions, predicates, and common syntax/modeling mistakes.
- Verify the files with the canonical local pipeline. If the tutorial
  generates files in a temporary workspace rather than committing a fixture,
  the commands must still be runnable from the repository root.

Managed-process example:

- Use the managed fixture records and direct commands with `--supervised` and
  `--status=stderr` to teach stable fields, stream ownership, and status
  classification.
- Baseline fixture comparison should use stable fields, semantic predicates,
  generated artifact state, normalized stdout/stderr, and final status fields.
  Human diagnostic prose can illustrate what a person may see, but it is not
  final managed-process contract evidence.

Example gap and later slice work:

- Slice02 should decide whether the minimal domain needs a small annotated
  companion diagram or plain-English walkthrough. If a new beginner example is
  needed, it should be checked in with verification in that slice.
- Slice03 should decide whether one intermediate example beyond
  `fixtures/unsolvable` is needed to avoid teaching only a toy happy path. Any
  added example must be runnable locally.
- Slice04 should add no speculative supervisor code unless it is verified; it
  can use command blocks and expected status predicates first.

## Voice And Style Rules

- Write clear, precise, reader-centered prose. Explain one concept before
  introducing the next command block.
- Prefer runnable command blocks over abstract descriptions. Mark conceptual
  examples as conceptual when they are not intended to execute.
- Keep pages kind to attention: short introductions, stable headings, small
  examples, and links to deeper reference material after the beginner path.
- Do not require readers to understand Arc numbers, CDC closure, or planning
  ledgers before they can use the tools.
- Do not ask supervisors or scripts to scrape diagnostic prose. Human messages
  can be useful, but public managed-process docs must teach exit codes,
  stdout/stderr roles, and tagged status fields.
- Do not overclaim quality posture. Coverage baselines, warning inventories,
  static-analysis results, and sanitizer runs are evidence and release-prep
  inputs, not universal quality guarantees.

## Future Slice Routing

| Responsibility | Route |
|----------------|-------|
| Beginner HTN/PDDL/HDDL concept docs | Slice02 |
| First runnable CLI workflow | Slice03 |
| Managed-process and supervised integration docs | Slice04 |
| Real-world issue to HTN model tutorial | Slice05 |
| HDDL domain/problem authoring tutorial | Slice06 |
| Behavior-change table, CLI reference, command-name migration context | Slice07 |
| Architecture, dependency, source-quality, source-class, generated-code, and third-party boundary docs | Slice08 |
| README entry point and `make readme-verbatim` preservation | Slice09 |
| Docs composition, link/example verification, and final Arc08 close | Slice10 |
| Release notes, package assets, release publication, and wolong fetch/install/migration verification | Arc09 |

## Slice Breakdown Recommendation

Slice breakdown change accepted after Slice03/Slice04 reader-journey review:
insert a modelling tutorial and an HDDL authoring tutorial before
reference/migration, architecture, README, and synthesis work.

The implementation sequence preserves the already-completed managed-process
Slice04, then inserts real-world modelling and HDDL authoring before
reference/migration, architecture, README, and synthesis work.

The reader-facing tutorial sequence is: concepts first, prepared-fixture CLI
workflow second, real-world modelling third, and HDDL authoring fourth.
`docs/managed-process.md` remains a public integration guide outside the
numbered tutorial path.

The only refinement is tactical: Slice02 and Slice03 should explicitly record
whether the existing fixture set is enough before adding examples; Slice05 and
Slice06 should explicitly decide whether their modelling/authoring examples
need a new checked-in fixture; and Slice07 should place the 0.2.0 -> 0.3.0
behavior-change table in `migration.md` so
Arc09 can link release notes and wolong migration proof to one stable page.
