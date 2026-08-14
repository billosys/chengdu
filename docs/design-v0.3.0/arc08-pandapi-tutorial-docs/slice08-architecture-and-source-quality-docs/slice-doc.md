# Arc08 Slice08: architecture-and-source-quality-docs

Status: open
Opened: 2026-08-14

## Goal

Write the public architecture and source-quality page:

- `docs/architecture.md`

This page should explain the pandaPI 0.3.0 product shape, dependency posture,
source-class policy, generated-code handling, third-party boundaries, and
source-quality evidence in reader-facing language. It closes Arc08 A6 at
implementation-proposed-done strength and gives the README and release-prep
arcs a stable public anchor for architecture and quality posture.

The page must be honest rather than triumphalist. It should describe verified
posture, measured baselines, and known budgets without implying global
zero-warning status, global coverage floors, or release-asset proof that
belongs to Arc09.

## In Scope

- Create `docs/architecture.md`.
- Use the accepted public docs layout:
  - public project docs under `docs/`;
  - tutorial pages under `docs/tutorial/`;
  - planning/evidence artifacts under `docs/design-v0.3.0/`.
- Explain the 0.3.0 product shape:
  - the in-tree `pandaPI/` product source;
  - `pandaPI/parser`;
  - `pandaPI/grounder`;
  - `pandaPI/engine`;
  - `pandaPI/runtime`;
  - canonical `pandapi-*` binaries as the supported command surface.
- Explain the managed-process architecture at a high level and link to:
  - `docs/reference/cli.md`;
  - `docs/managed-process.md`;
  - `docs/migration.md`;
  - the tutorial sequence.
- Explain source classes in public language:
  - first-party maintained;
  - first-party generated;
  - first-party generator/template;
  - vendored third-party;
  - dependency-internal;
  - third-party generated;
  - copied build artifact;
  - generated build output.
- Explain generated-code policy:
  - do not hand-edit generated output;
  - fix owned generator/template inputs when the defect belongs there;
  - distinguish owned generator inputs from third-party skeleton noise;
  - use the parser Flex `%option noinput`/`%option nounput` closure as a short
    concrete example;
  - keep gengetopt/CLI11 re-entry separate from generated-output hand edits.
- Explain third-party/dependency boundaries:
  - `cpddl`;
  - H2;
  - nested third-party dependencies such as boruvka, opts, lpsolve, and CUDD;
  - `rss.c` as currently maintained copied-origin first-party grounder source;
  - the rule that dependency internals are not first-party cleanup obligations
    unless a later dependency audit accepts that boundary.
- Explain accepted dependency posture from Arc02 in public language:
  - standard-library baseline;
  - `fmt` through owned diagnostics/process surfaces;
  - CLI11 as the selected replacement path for command-line parsing;
  - Catch2 as a test-only seam/unit-test dependency;
  - `tl::expected`-style results behind owned status/result surfaces;
  - `reproc++` only if a supported child-process boundary is accepted later;
  - held or rejected broad dependencies only where helpful to prevent
    misunderstanding.
- Explain source-quality gates and release posture:
  - Make is the developer entrypoint;
  - managed process fixtures prove executable contracts;
  - CTest/Catch2-style seam tests prove internal seams when present;
  - formatting, static analysis, coverage, warning inventory, generated-warning
    triage, sanitizer, safety, actionlint, provenance, and contract gates are
    routed by source class;
  - measured coverage and warning counts are evidence, not public guarantees.
- Include a concise "what remains budgeted or deferred" section:
  - no global coverage floor yet;
  - no global zero-warning claim;
  - parser maintained-source burndown remains re-entry;
  - gengetopt/CLI11 migration remains re-entry;
  - dependency audits remain re-entry;
  - Linux LeakSanitizer and TSan posture remains release/pre-release re-entry
    per current source-quality synthesis.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` when the
  slice closes.

## Out Of Scope

- Do not update README, docs index, release notes, release assets, checksums,
  manifests, publish workflows, license bundles, wolong integration, or Arc09
  release-publication surfaces.
- Do not edit CI, Make targets, tests, tools, fixtures, parser/grounder/engine/
  runtime source, source-quality thresholds, package/provenance logic, or
  dependency vendoring.
- Do not add a new public source-quality threshold, coverage floor, warning
  budget, dependency obligation, or support promise that is not already
  accepted by Arc07.
- Do not re-open the old `pandaPI*` compatibility decision. Link command-name
  migration details to `docs/migration.md`; keep this page focused on
  architecture and quality posture.
- Do not expose Arc08, Slice08, CC, CDC, ledger, closing-report, project-plan,
  or arc-plan vocabulary in public docs.
- Do not present optional/fenced surfaces such as H2, SAT, translation,
  interactive mode, BDD, CUDD, or dependency internals as supported public
  behavior.
- Do not create `docs/source-quality.md` or split the public page unless the
  operator explicitly accepts a scope amendment.

## Verification Approach

This is a public-docs writing slice with evidence reconciliation:

- grep `docs/architecture.md` for expected headings, component names,
  source-class terms, dependency names, Make-backed gate names, generated-code
  policy, third-party boundaries, and release-boundary language;
- check that public links point to existing public pages;
- run `make help` and verify the page's named Make targets are actual current
  targets;
- run the source-quality evidence targets that the page treats as current
  posture anchors:
  - `make source-quality-surface`;
  - `make source-quality-profile-map`;
  - `make source-quality-naming-check`;
  - `make compile-db-first-party`;
  - `make source-quality-gate-report`;
  - `make generated-warning-triage`;
  - `make generated-warning-triage-parser`;
  - `make warning-inventory-first-party`;
- run `make safety-checks` and `make actionlint`;
- run `git diff --check` and `git diff --cached --check`;
- verify public docs avoid internal planning vocabulary;
- verify the page does not overclaim global coverage floors, global
  zero-warning status, release publication, or dependency-audit obligations;
- verify changed paths stay inside public docs and Arc08/project design
  planning surfaces unless the ledger is amended.

Aggregate `make check` is not required for this docs-writing slice unless
code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces change. If it is not run, the closing report must
state which constituent checks were run and why aggregate `make check` was
narrower than the slice risk.

## Exit Criteria

- `docs/architecture.md` exists.
- The architecture page explains the parser, grounder, engine, runtime,
  canonical command surface, and managed-process shape in public language.
- The page explains source classes and third-party/dependency boundaries
  without reviving old runtime-versus-planner ownership ambiguity.
- The page explains generated-code policy, including the parser Flex closure
  and gengetopt/CLI11 re-entry, without encouraging generated-output hand
  edits.
- The page explains accepted dependency posture, especially the CLI11
  commitment, without claiming a dependency has landed where it has not.
- The page explains current source-quality gates and measured baselines without
  claiming global coverage floors, global zero-warning status, or release proof.
- The page links to existing public tutorial, CLI, migration, and
  managed-process docs.
- The slice stays out of README, docs index, release-publication, CI, Make,
  tests/tools/fixtures, implementation source, and dependency-vendoring scope
  unless the ledger is amended.
- The closing report walks every ledger row and bubbles up whether Slice09
  README or Arc09 release-preparation handoffs need adjustment.
