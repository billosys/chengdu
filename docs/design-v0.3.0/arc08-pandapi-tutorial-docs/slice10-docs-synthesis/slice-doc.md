# Arc08 Slice10: docs-synthesis

Status: open
Opened: 2026-08-14

## Goal

Perform the final Arc08 documentation composition pass and prepare Arc08 for
closure.

This slice verifies that the public pandaPI 0.3.0 documentation suite composes
as one coherent reader experience:

- README entry point;
- public docs landing page;
- tutorial sequence;
- managed-process guide;
- CLI reference;
- migration guide;
- architecture/source-quality guide;
- release-preparation handoff to Arc09.

The slice should fix small public-doc consistency issues found by the
composition pass, create the final public docs landing page, write durable
Arc08 synthesis evidence, and write the Arc08 closing report at
implementation-proposed-done strength.

## In Scope

- Create `docs/index.md` as the public docs landing page.
- Update `README.md` only as needed to point to `docs/index.md` and preserve
  the direct high-value links already introduced by Slice09.
- Make small consistency corrections across the public docs suite:
  - `docs/tutorial/01-htn-hddl-onboarding.md`;
  - `docs/tutorial/02-first-project-workflow.md`;
  - `docs/tutorial/03-model-a-feature-as-htn.md`;
  - `docs/tutorial/04-write-hddl-files.md`;
  - `docs/managed-process.md`;
  - `docs/reference/cli.md`;
  - `docs/migration.md`;
  - `docs/architecture.md`.
- Create
  `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/docs-synthesis.md`
  summarizing:
  - delivered public pages;
  - reader journeys;
  - runnable examples and verification gates;
  - public command-name policy;
  - managed-process integration posture;
  - source-quality/release-preparation posture;
  - Arc09 handoff inputs.
- Create the Arc08 arc-level `closing-report.md` with:
  - slice walk for Slice01 through Slice10;
  - arc ledger A1 through A12 walk;
  - composition verdict;
  - silent-drop diff;
  - bubble-up to the project;
  - explicit Arc09 release-preparation handoff.
- Update this slice's `ledger.md` and add a slice-level `closing-report.md`
  when implementation finishes.
- Update Arc08 `arc-plan.md` and `docs/design-v0.3.0/project-plan.md` when the
  slice closes.

## Out Of Scope

- Do not publish a release or simulate release publication.
- Do not edit release package assets, checksums, manifests, release workflows,
  license bundles, wolong integration code, parser/grounder/engine/runtime
  implementation source, source-quality thresholds, tests, fixtures, Make
  targets, `mk/`, or tooling unless a composition bug proves the slice cannot
  close without an explicit scope amendment.
- Do not create or alter 0.3.0 release notes beyond naming what Arc09 must
  produce.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Do not expose Arc08, Slice10, CC, CDC, ledger, closing-report, project-plan,
  or arc-plan vocabulary in public docs. Those terms may appear in design
  evidence and closing reports.
- Do not document inherited `pandaPI*` command names as supported 0.3.0
  commands. Historical migration context remains confined to
  `docs/migration.md`.
- Do not overclaim global zero-warning status, global coverage floors,
  dependency-audit completion, package publication, or wolong proof.

## Verification Approach

This is a docs-composition and arc-close slice:

- verify every planned public page exists;
- verify `docs/index.md` links the public docs suite;
- verify README links `docs/index.md` and the most important docs anchors;
- verify local links across README and public docs resolve;
- verify public docs avoid internal planning vocabulary;
- verify inherited command names appear only in `docs/migration.md` historical
  context;
- verify source-build and tutorial examples consistently use
  `./bin/pandapi-*` for runnable local commands;
- verify release-publication claims remain framed as release-preparation
  handoffs, not published evidence;
- run `make readme-verbatim`;
- run `make smoke && make smoke-negative`;
- run `make test-contract-pipeline-managed`;
- run live `--help`, `--version`, and `--provenance` probes for the three
  `./bin/pandapi-*` binaries;
- run `make safety-checks && make actionlint`;
- run `git diff --check` and `git diff --cached --check`;
- verify protected paths stay untouched unless the ledger is amended.

Aggregate `make check` is not required for this docs-composition slice unless
code, Make, CI, tests, tools, fixtures, release/package, or other
behavior-bearing surfaces change. If it is not run, the closing report must
state which constituent gates were run and why aggregate `make check` was
narrower than the slice risk.

## Exit Criteria

- `docs/index.md` exists and provides clean public wayfinding.
- `docs-synthesis.md` exists and records how the delivered docs suite composes.
- Arc08 `closing-report.md` exists and walks Slice01 through Slice10 plus arc
  ledger rows A1 through A12.
- README, docs index, tutorials, managed-process guide, CLI reference,
  migration guide, and architecture guide link coherently.
- Runnable docs examples and command references remain verified by Make-backed
  and direct binary probes.
- Public docs avoid internal planning vocabulary.
- Public docs preserve the 0.3.0 canonical `pandapi-*` command policy.
- Public docs avoid false 0.3.0 release-publication claims.
- Arc09 receives concrete handoff inputs for release notes, packages,
  checksums, manifest/provenance, license/NOTICE proof, test-only dependency
  exclusion, wolong fetch/install/migration verification, and publication.
- Slice10 closes without touching release, CI, Make, tests/tools/fixtures,
  implementation source, or dependency-vendoring scope unless the ledger is
  explicitly amended.
