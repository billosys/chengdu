# CC Prompt: Arc07 Slice04 - first-party-quality-gate-scaffold

You are implementing Arc07 Slice04 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`
6. `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/cdc-verification.md`
7. `docs/design-v0.3.0/arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/cdc-verification.md`
8. `docs/design-v0.3.0/arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/slice-doc.md`
9. `docs/design-v0.3.0/arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/ledger.md`
10. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/warning-inventory.md`
11. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`

Also inspect current Make/tool surfaces before editing:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `mk/checks.mk`
- `mk/format.mk`
- `mk/coverage.mk`
- `mk/tests.mk`
- `mk/build.mk`
- `tools/source-quality/surface`

## Policy

This is a scaffold slice, not a cleanup slice.

Add target families and reports that make first-party source-quality work
explicit across parser, grounder, engine, runtime, and Chengdu-owned
generators/templates. Do not clear warnings, retrofit broad unit tests, set
coverage floors, or silently promote optional/fenced public behavior.

Every new target must be honest. It should either:

- run a real existing gate;
- generate a meaningful evidence/report file from current selectors, compile
  databases, build logs, profile maps, sanitizer output, or fixture placement;
- fail or skip with a clear missing-tool/precondition message plus documented
  re-entry condition.

Do not add empty green targets.

Generated-warning work requires root-cause and correctness analysis. For each
generated-warning class the scaffold records, capture:

- generated file;
- generator/template or third-party skeleton;
- ownership class;
- why the warning is emitted;
- whether the generated code is semantically correct;
- chosen remedy;
- re-entry condition.

If the issue belongs to a Chengdu-owned generator/template, the later fix
belongs at the generator/template, not as a warning-hiding patch to generated
output. Third-party skeleton output may be excluded or suppressed only with
rationale.

## Task

Implement the Slice04 scaffold:

1. Add and document Make targets:
   - `make source-quality-gate-report`
   - `make format-check-first-party`
   - `make static-analysis-first-party`
   - `make coverage-first-party`
   - `make warning-inventory-first-party`
   - `make generated-warning-triage`
   - `make test-unit`
   - `make sanitize-first-party`
2. Reuse Slice02/03 source-quality selectors, compile database outputs,
   profile/source maps, naming checks, and generated/dependency exclusions.
3. Write a durable report at:
   `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`.
4. In that report, map each gate family by:
   - component;
   - source class;
   - Make target;
   - current execution status;
   - generated evidence path;
   - re-entry condition.
5. Add component target forms when they can be honest in this slice. If a
   component form is not yet executable, list its intended target name in the
   durable report with the reason and re-entry condition.
6. Include generated-warning triage records or schema with all required
   root-cause/correctness fields.
7. Include optional IPC corpus placement. Existing `make test-corpus
   CORPUS_DIR=...` should remain optional external-corpus smoke unless this
   slice explicitly stops for an amendment.
8. Preserve existing behavior and current CI/release/public-doc boundaries.

Prefer the smallest durable implementation. Helper logic belongs under `mk/`
or `tools/` behind Make targets. Do not add a new `scripts/` entrypoint.

## Scope Constraints

Allowed surfaces:

- `Makefile`
- `mk/`
- `tools/source-quality/`
- tests/tooling files only if needed to make an existing target callable
  through Make without changing behavior
- `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`
- Arc07 Slice04 docs/ledger/closing report

Be cautious with production C/C++ source. This slice should not need parser,
grounder, engine, runtime, or generator behavior changes. If it appears to
need one, stop and request an amendment unless the change is a tiny include or
build-reference correction required by the scaffold.

Do not change:

- public `pandapi-*` CLI behavior;
- parser, grounder, engine, or runtime semantics;
- public README/tutorial docs;
- CI workflows;
- release/package/publication tooling;
- Arc08 or Arc09 surfaces;
- coverage floors;
- warning fail budgets;
- global `-Werror`;
- static-analysis thresholds;
- broad IPC benchmark release policy;
- Catch2 or other dependency pinning;
- third-party/dependency-internal source obligations.

## Required Verification

Run the new scaffold targets:

```text
make help
make source-quality-gate-report
make format-check-first-party
make static-analysis-first-party
make coverage-first-party
make warning-inventory-first-party
make generated-warning-triage
make test-unit
make sanitize-first-party
```

Run the existing source-quality runway:

```text
make source-quality-surface
make source-quality-profile-map
make source-quality-naming-check
make compile-db-first-party
```

Run regression gates:

```text
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

Run `make check` if practical. If you do not run it, say exactly why and list
the narrower Make-backed gates that substitute for this slice.

Also run the boundary checks from the ledger, especially:

- no `.github`, README, release, `tools/release`, Arc08, or Arc09 changes;
- no quality-threshold or public-supported-surface expansion;
- no dependency pinning or new external dependency.

## Stop Conditions

Stop and request an amendment if:

- a target cannot be implemented without becoming an empty green target;
- component source-quality gates need production C/C++ rewrites before target
  scaffolding can land;
- generated-warning triage cannot identify generator/skeleton ownership well
  enough to produce honest records;
- a new external dependency appears necessary;
- CI workflow changes appear necessary;
- a coverage floor, warning failure budget, static-analysis threshold, or
  release-grade IPC corpus policy appears necessary;
- source-quality selectors cannot route first-party, generated, dependency,
  and third-party paths without a larger build-system rewrite.

## Closing Requirements

Before committing:

- Close every ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/closing-report.md`.
- Include `Bubble-up to Arc07`, explicitly stating the recommended Slice05
  component burndown order or whether scaffold work revealed a required
  split/remediation first.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
