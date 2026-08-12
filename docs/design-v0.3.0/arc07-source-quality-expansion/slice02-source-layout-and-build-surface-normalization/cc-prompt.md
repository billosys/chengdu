# CC Prompt: Arc07 Slice02 - source-layout-and-build-surface-normalization

You are implementing Arc07 Slice02 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/slice01-source-classification-inventory/cdc-verification.md`
6. `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/slice-doc.md`
7. `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/ledger.md`

Also inspect the current Make/build surfaces before editing:

- `Makefile`
- `mk/config.mk`
- `mk/build.mk`
- `mk/format.mk`
- `mk/checks.mk`
- `mk/coverage.mk`
- `mk/help.mk`
- `tools/shared/platform`
- `pandaPI/parser/makefile`
- `pandaPI/grounder/src/Makefile`
- `pandaPI/engine/src/CMakeLists.txt`

## Policy

Use the Slice01 source-class taxonomy as accepted policy:

- parser, grounder, engine, and runtime are all Chengdu-owned product
  components;
- first-party maintained code is the future target for formatting, static
  analysis, coverage, warnings, sanitizers, and unit/seam tests;
- generated output is routed through its generator/template owner when it is
  Chengdu-maintained, and otherwise reported or excluded with rationale;
- vendored third-party and dependency-internal source is excluded from
  first-party obligations unless a later dependency audit accepts that
  boundary.

This slice normalizes build and selector surfaces. It does not enforce the
broader gates yet.

## Task

Implement the Slice02 normalization surface:

1. Add canonical Make variables or Make-invoked helper logic for source-class
   selectors and exclusions.
2. Add and document these Make targets:
   - `make source-quality-surface`
   - `make source-quality-profile-map`
   - `make compile-db-parser`
   - `make compile-db-grounder`
   - `make compile-db-engine`
   - `make compile-db-first-party`
3. Write generated reports under `build/source-quality/<platform>/`.
4. Generate parser, grounder, and engine compile databases under
   `build/source-quality/<platform>/compile-db/<component>/`.
5. Put first-party selected translation-unit lists or equivalent filter
   evidence beside the raw compile databases.
6. Encode generated-output, copied-build, vendored third-party, and
   dependency-internal exclusions in Make or Make-invoked helper logic.
7. Add source/profile mapping evidence so future coverage work can map copied
   build roots back to canonical `pandaPI/<component>` roots.

Implementation shape is yours. Prefer the smallest durable Make structure.
Helper scripts are acceptable under `tools/` only if they are implementation
details behind Make targets. Do not add new `scripts/` entrypoints.

## Scope Constraints

Allowed surfaces:

- `Makefile`
- `mk/`
- `tools/`
- component build files only if needed for compile database generation:
  `pandaPI/parser/makefile`, `pandaPI/grounder/src/Makefile`,
  `pandaPI/engine/src/CMakeLists.txt`
- Arc07 Slice02 docs/ledger/closing report

Do not change:

- parser, grounder, engine, or runtime behavior;
- algorithm/data-structure source cleanup;
- generated grammar/lexer/option output;
- public README/tutorial docs;
- CI workflows;
- release/package/publication tooling;
- Arc08 or Arc09 surfaces;
- coverage floors, warning fail budgets, or expanded static-analysis gates;
- Catch2/test dependency posture unless you stop for an amendment.

Do not promote H2, cpddl/FAM, SAT, translation, interactive, or BDD/CUDD
behavior into supported public surface.

## Required Verification

Run the new targets:

```text
make help
make source-quality-surface
make source-quality-profile-map
make compile-db-parser
make compile-db-grounder
make compile-db-engine
make compile-db-first-party
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

## Stop Conditions

Stop and request an amendment if:

- parser or grounder compile database generation requires a larger build-system
  rewrite than this slice can honestly hold;
- a new external dependency appears necessary;
- implementing selectors would require changing public binary behavior;
- the source-class taxonomy is insufficient for a real path you encounter;
- the work naturally splits into separate selector and compile/profile slices.

## Closing Requirements

Before committing:

- Close every ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/closing-report.md`.
- Include `Bubble-up to Arc07`, explicitly stating whether Slice03
  first-party-quality-gate-scaffold should open next, or whether Slice02 found
  a required split/remediation first.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
