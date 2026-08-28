# CC Prompt: Arc07 Slice08 - source-quality-synthesis

You are implementing Arc07 Slice08 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`
6. `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`
7. `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md`
8. Every Slice01 through Slice07 `closing-report.md` and
   `cdc-verification.md`
9. `docs/design-v0.3.0/arc06-ci-and-test-hardening/closing-report.md`
10. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
11. `docs/design-v0.3.0/arc07-source-quality-expansion/slice08-source-quality-synthesis/slice-doc.md`
12. `docs/design-v0.3.0/arc07-source-quality-expansion/slice08-source-quality-synthesis/ledger.md`

Also inspect current Make/tool surfaces before writing synthesis claims:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `mk/checks.mk`
- `mk/coverage.mk`
- `mk/tests.mk`
- `tools/source-quality/surface`
- current `build/source-quality/<platform>/` evidence if generated during
  verification
- current `build/warnings/<platform>/` evidence if generated during
  verification

## Policy

This is a synthesis and arc-composition slice. Do not use it to sneak in
implementation work.

The job is to answer, with evidence:

- What source is first-party maintained and what source is generated,
  dependency-internal, vendored third-party, third-party generated, copied
  build artifact, or build output?
- Which gates are strict release gates today?
- Which gates are measured baselines?
- Which items are accepted post-0.3.0 budgets?
- Which items still block release?
- Can Arc07 honestly close after this slice?

If the answer to the last question is "no", do not write an Arc07 arc-level
closing report. Instead, update the plans with the next required slice and
make the reason impossible to miss.

## Task

Implement Slice08:

1. Create
   `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-synthesis.md`.
2. Reconcile Arc07 Slices01-07 and the Slice08 evidence against Arc07 ledger
   rows A1-A9.
3. Include a source-class policy summary with the final Arc07 disposition for
   `rss.c`, cpddl, H2, boruvka, opts, lpsolve, CUDD, generated `cmdline.*`,
   Flex/Bison/gengetopt outputs, copied build roots, and build output.
4. Include a component/gate matrix covering parser, grounder, engine, and
   runtime across:
   - format;
   - static analysis;
   - coverage;
   - unit/seam tests;
   - warning policy;
   - sanitizer/TSan;
   - generated-warning triage;
   - managed process-contract fixtures.
5. For every gate or source class, classify the release posture as one of:
   - strict release blocker;
   - accepted measured baseline;
   - accepted deferred/post-0.3.0 budget with reason and re-entry;
   - out of first-party scope pending dependency audit.
6. Name all remaining work, including parser maintained-source burndown,
   gengetopt/CLI11 replacement, stricter generated-code policy, dependency
   audits, coverage floors, warning/static-analysis thresholds, and any
   Arc09 release-prep gates.
7. Decide whether Arc07 can be marked CC proposed done.
   - If yes, write
     `docs/design-v0.3.0/arc07-source-quality-expansion/closing-report.md`
     with a slice walk, A1-A9 arc-ledger walk, composition check, and
     Bubble-up to the project.
   - If no, do not write the arc closing report; update the plans with the
     next required slice.
8. Update this slice's `ledger.md`, write this slice's `closing-report.md`,
   and update `arc-plan.md` plus `project-plan.md` with the outcome.

## Scope Constraints

Allowed surfaces:

- Arc07 synthesis/report docs.
- Arc07 Slice08 docs.
- Arc07 `arc-plan.md`.
- `docs/design-v0.3.0/project-plan.md`.

Do not change:

- parser, grounder, engine, or runtime source/build/test implementation;
- Make targets or source-quality tooling;
- CI workflows;
- public README/tutorial docs;
- Arc08 or Arc09 docs;
- release/package/publication tooling;
- wolong migration guidance;
- dependency pins/fetch behavior;
- CLI11/gengetopt implementation;
- global coverage floors, `-Werror`, warning fail policy, or static-analysis
  thresholds.

Do not add a new `scripts/` entrypoint. All verification must use Make-backed
targets.

## Required Verification

Evidence-composition and source-quality gates:

```text
make source-quality-surface
make source-quality-profile-map
make source-quality-naming-check
make compile-db-first-party
make warning-inventory-first-party
make generated-warning-triage-parser
make generated-warning-triage
```

Component evidence:

```text
make coverage
make coverage-engine
make coverage-grounder
make test-unit
make test-unit-engine
make test-unit-grounder
make test-runtime-sanitize
make test-binary-sanitize
make sanitize-engine
make sanitize-grounder
```

Regression evidence:

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

Also run scope and report checks:

```text
rg -n -- "release blocker|measured baseline|deferred|post-0\\.3\\.0|out of first-party scope|re-entry" docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-synthesis.md
rg -n -- "A1|A2|A3|A4|A5|A6|A7|A8|A9" docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-synthesis.md
git diff --cached --name-only
```

Before committing, confirm the staged path set stays inside the allowed docs
surfaces. This slice should not touch `pandaPI/`, `mk/`, `tools/`, `.github/`,
README, release, Arc08, or Arc09 paths unless the ledger is amended first.

## Stop Conditions

Stop and request an amendment if:

- Arc07 cannot close without implementing new parser, grounder, engine, or
  runtime source-quality work.
- You find an Arc07 ledger row A1-A9 that completed slices do not satisfy.
- Any remaining item cannot be classified as a release blocker, measured
  baseline, deferred/post-0.3.0 budget, or out-of-first-party-scope boundary.
- A coverage floor, warning failure policy, static-analysis threshold, CLI11
  migration, dependency audit, or public docs/release change appears necessary
  to make the synthesis honest.
- Verification requires changing Make targets, CI workflows, or implementation
  source.
- You cannot reproduce the source-quality or regression evidence with
  Make-backed targets.

## Closing Requirements

Before committing:

- Close every Slice08 ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice08-source-quality-synthesis/closing-report.md`.
- If and only if Arc07 composes, write
  `docs/design-v0.3.0/arc07-source-quality-expansion/closing-report.md` and
  mark Arc07 as CC proposed done in the plans.
- If Arc07 does not compose, do not create the arc closing report; open or
  identify the next required slice in the plans.
- Run the required verification above.
- Run `git diff --check` and `git diff --cached --check`.
- Commit the result with both required trailers:
  - `Co-authored-by: Codex <noreply@openai.com>`
  - `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Leave the worktree clean. Do not create `cdc-verification.md`; CDC writes that
after independent review.
