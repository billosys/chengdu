# CC Prompt: Arc07 Slice03 - first-party-source-naming-normalization

You are implementing Arc07 Slice03 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/cdc-verification.md`
6. `docs/design-v0.3.0/arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/slice-doc.md`
7. `docs/design-v0.3.0/arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/ledger.md`

Also inspect the current source-quality and build surfaces before editing:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `tools/source-quality/surface`
- `pandaPI/parser/makefile`
- `pandaPI/grounder/src/Makefile`
- `pandaPI/engine/src/CMakeLists.txt`
- component-local `CMakeLists.txt` files under `pandaPI/engine/src/`

## Policy

Normalize only first-party maintained C/C++ files and directories to lower
snake case. Treat acronym clusters as words where clear, for example
`hddl_writer`, `h2_mutexes`, `int_data_structures`, or
`progression_network`.

Do not rename or promote:

- generated Flex/Bison/gengetopt outputs unless a tracked generated file must
  be updated for an include/build reference;
- `pandaPI/grounder/cpddl`;
- `pandaPI/grounder/h2-fd-preprocessor`;
- `pandaPI/engine/src/symbolic_search/cudd-3.0.0`;
- copied build artifacts under `build/`;
- public binary names or managed-process behavior.

Conventional build filenames such as `CMakeLists.txt` are exempt.

## Task

Implement the naming normalization slice:

1. Add and document Make targets:
   - `make source-quality-naming-report`
   - `make source-quality-naming-check`
2. Reuse Slice02 source-class selectors where practical, so the report/check
   covers first-party maintained source and excludes generated/dependency
   paths.
3. Write a durable report at:
   `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`.
4. In that report, include:
   - the accepted naming policy;
   - explicit exemptions;
   - source-class exclusions;
   - an initial inventory summary;
   - the final rename map.
5. Rename non-conforming first-party maintained C/C++ files and directories
   with `git mv`. Use a temporary intermediate path for case-only renames on
   macOS when necessary.
6. Update all include strings, Make references, CMake references,
   source-quality helper logic, tests, fixtures, and documentation references
   needed by the renamed paths.
7. Re-run Slice02 source-quality reports and compile database generation after
   renaming.

Prefer the smallest durable implementation. Helper logic belongs under `mk/`
or `tools/` behind Make targets. Do not add a new `scripts/` entrypoint.

## Scope Constraints

Allowed surfaces:

- first-party maintained C/C++ files and directories under:
  - `pandaPI/parser`
  - `pandaPI/grounder/src`
  - `pandaPI/engine/src`
  - `pandaPI/runtime`
- `Makefile`
- `mk/`
- `tools/source-quality/`
- component build files needed by the rename:
  - `pandaPI/parser/makefile`
  - `pandaPI/grounder/src/Makefile`
  - `pandaPI/engine/src/CMakeLists.txt`
  - component-local engine `CMakeLists.txt` files
- Arc07 Slice03 docs/ledger/closing report
- `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`

Do not change:

- parser, grounder, engine, or runtime behavior;
- public README/tutorial docs;
- CI workflows;
- release/package/publication tooling;
- Arc08 or Arc09 surfaces;
- coverage floors, warning fail budgets, static-analysis thresholds, or
  supported-surface policy;
- third-party/dependency paths listed in this prompt.

## Required Verification

Run the new and source-quality targets:

```text
make help
make source-quality-naming-report
make source-quality-naming-check
make source-quality-surface
make source-quality-profile-map
make compile-db-first-party
```

Run component checks:

```text
make build-parser
make build-grounder
make build-engine
make test-contract-parser-managed
make test-contract-grounder-managed
make test-contract-engine-managed
```

Run regression gates:

```text
make safety-checks
make actionlint
make static-analysis
make format-check
make build
make test
make provenance-check
git diff --check
git diff --cached --check
```

Run `make check` if practical. If you do not run it, say exactly why and list
the narrower Make-backed gates that substitute for this slice.

Also run the boundary checks from the ledger, especially the check that no
excluded third-party/dependency roots were renamed.

## Stop Conditions

Stop and request an amendment if:

- the rename set is too large to complete and verify in one slice;
- a generated, third-party, or dependency-internal path appears to require a
  rename for the build to work;
- the accepted lower snake case mapping is ambiguous for a significant family
  of names;
- path changes force behavior changes beyond include/build references;
- a new external dependency appears necessary;
- source-quality selectors cannot be updated without a larger build-system
  rewrite.

## Closing Requirements

Before committing:

- Close every ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/closing-report.md`.
- Include `Bubble-up to Arc07`, explicitly stating whether Slice04
  `first-party-quality-gate-scaffold` should open next, or whether naming work
  revealed a required split/remediation first.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
