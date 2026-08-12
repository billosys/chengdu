# Arc07 Slice02: source-layout-and-build-surface-normalization

Status: open
Opened: 2026-08-12

## Goal

Normalize the Make-backed source-quality surface so later Arc07 slices can
expand formatting, static analysis, coverage, warning policy, sanitizer triage,
and unit/seam tests across first-party parser, grounder, engine, and runtime
code without accidentally measuring generated output, copied build trees, or
vendored/dependency-internal source.

This slice activates the recommendation from Slice01: do source-layout and
build-surface normalization before adding broader quality thresholds.

## Background

Slice01 classified the current tree and identified the current blocker:
runtime gates already have good signal, but parser, grounder, and engine build
paths mix first-party maintained source with generated files, copied build
source trees, nested dependencies, and optional/fenced code. Arc07 cannot set
honest source-quality gates until the Make layer has canonical selectors and
per-component build evidence surfaces.

Accepted decisions for this slice:

- the Slice01 source-class taxonomy is accepted as the policy basis;
- Slice02 may add or revise Make selectors, generated reports, compile
  database build directories, and profile/source-map build directories;
- Slice02 must not change public binary behavior or supported-surface policy;
- Catch2/test dependency pinning remains a Slice03 decision unless CC finds a
  concrete blocker and requests an amendment;
- release coverage floors and zero-warning/static-analysis enforcement remain
  downstream of measured baselines.

## In Scope

- Add canonical Make variables or helper logic for source classes:
  first-party maintained files, first-party generator/templates, generated
  outputs, vendored third-party exclusions, dependency-internal exclusions,
  copied build artifacts, and generated build output.
- Add a Make-backed source-quality surface report target:
  `make source-quality-surface`.
- Add per-component compile database targets:
  `make compile-db-parser`, `make compile-db-grounder`,
  `make compile-db-engine`, and aggregate `make compile-db-first-party`.
- Store generated source-quality reports under
  `build/source-quality/<platform>/`.
- Store compile database output under
  `build/source-quality/<platform>/compile-db/<component>/`.
- Encode generated-output and third-party/dependency exclusions in Make or
  Make-invoked helper logic, not only in prose.
- Add a Make-backed profile/source-map normalization report target:
  `make source-quality-profile-map`, so future coverage slices can map copied
  build paths back to canonical `pandaPI/<component>` source roots.
- Update `make help` for every new public Make target.
- Keep all new tooling behind Make targets. Helper code may live under `mk/`
  or `tools/`, but users and CI should be taught the Make targets.

## Out Of Scope

- No parser, grounder, engine, or runtime behavior changes.
- No algorithm/data-structure cleanup, warning fixes, formatting sweeps, or
  generated-code rewrites.
- No new coverage floors, warning fail budgets, static-analysis enforcement
  across parser/grounder/engine, or unit-test framework adoption.
- No public README/tutorial, wolong migration, release packaging, publication,
  or CI workflow changes.
- No promotion of H2, cpddl/FAM, SAT, translation, interactive, or BDD/CUDD
  behavior into supported public surface.
- No new non-test runtime dependency.

## Required Target Shape

Slice02 should leave these Make targets callable and documented by
`make help`:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-parser`
- `make compile-db-grounder`
- `make compile-db-engine`
- `make compile-db-first-party`

The exact implementation is CC's choice, but the targets must be
Make-backed, deterministic, and local. If a component compile database cannot
be generated honestly without a larger build-system rewrite, stop and request
an amendment rather than closing the row as deferred.

## Verification Approach

Primary verification is Make-backed:

- `make help`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-parser`
- `make compile-db-grounder`
- `make compile-db-engine`
- `make compile-db-first-party`

Regression checks should include:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make build`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

If `make check` is practical and does not introduce the known macOS provenance
mutation/race, run it. If a narrower set is used, the closing report must say
exactly what ran and why.

## Exit Criteria

- New Make targets produce source-quality surface, compile database, and
  profile/source-map evidence under ignored `build/source-quality/<platform>/`
  paths.
- Parser, grounder, engine, and runtime first-party selectors are visible from
  Make or Make-invoked helper reports.
- Generated output, copied build artifacts, vendored third-party source, and
  dependency-internal source are excluded or separately reported by the
  selector surface.
- Parser, grounder, and engine compile databases exist and are accompanied by
  first-party selected translation-unit lists or equivalent filter evidence.
- Future coverage/profile output can be routed through component-specific
  directories and mapped back from copied build roots to canonical source
  roots.
- Existing build/test/provenance gates still pass.
- Ledger rows close with evidence, and CDC can reproduce every target.
