# Arc09 Plan: release-prep-publication

Status: active; Slice01 release-readiness-inventory open
Opened: 2026-08-14

## 1. Capability

Arc09 verifies and publishes `v0.3.0`. It takes the managed-process binaries,
source-quality gates, and public documentation from Arcs05-08 and turns them
into release evidence: package contents, checksums, manifest/provenance,
license and NOTICE obligations, test-only dependency exclusion, source-quality
release gates, release notes, wolong fetch/install/migration proof, publication
behavior, and post-publication re-run checks.

Arc09 is the first arc allowed to make public `v0.3.0` release claims. Earlier
arcs may provide evidence, docs, and release-preparation wording, but only
Arc09 can convert that into published-installation instructions or a release
announcement after the assets and re-run behavior are verified.

## 2. Release Principles

1. **Package what we actually support.** Release archives ship canonical
   `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` binaries and
   must not include inherited `pandaPI*` command names.
2. **Make release evidence machine-checkable.** Checksums, manifest,
   provenance, package contents, executable bits, and version/provenance
   outputs need named Make-backed verification.
3. **Keep source availability and licensing explicit.** `THIRD-PARTY-LICENSES`,
   NOTICE/source-availability wording, and release notes must match the
   shipped binary contents and the in-tree dependency boundaries.
4. **Separate user dependencies from developer-only dependencies.** Release
   artifacts must not require or ship test-only dependencies such as Catch2.
   Developer dependency requirements remain documented for source builders and
   maintainers.
5. **Treat quality gates as release gates, not decoration.** Behavior,
   contract, smoke, sanitizer, static-analysis, warning, coverage, provenance,
   README-verbatim, and source-quality evidence must be run in the release
   context, with any platform-specific limitation documented as a release
   decision rather than hidden.
6. **Verify wolong against release shape.** wolong is the acceptance anchor
   for supervised process use. Its migration/fetch/install path must be
   verified against packaged or published assets, not merely local `bin/`
   builds.
7. **Publish last.** GitHub publication happens only after dry-run/package
   proof, release notes, assets, checksums, manifest, license evidence, and
   wolong behavior have closed.

## 3. Slice Breakdown

| Slice | Slug | Scope | Load-bearing for |
|-------|------|-------|------------------|
| slice01 | `release-readiness-inventory` | Inventory the current release/package/publish/CI/docs/licensing/wolong surfaces, classify gaps against P9 and the Arc08 handoff, and recommend final slice ordering. No release behavior changes yet. | all Arc09 slices |
| slice02 | `package-shape-and-manifest` | Update and verify package contents, archive names, canonical executable names, executable bits, checksums, release manifest, provenance, and package-local smoke probes. | license, docs, wolong, publication |
| slice03 | `license-notice-source-availability` | Audit shipped binary contents against license texts, NOTICE/source-availability obligations, release notes language, and package assets; fix stale names or paths. | release notes, publication |
| slice04 | `release-quality-gates` | Establish the pre-release gate bundle over behavior, managed contracts, source quality, sanitizer, static analysis, warning inventory, coverage baselines, provenance, and supported-platform evidence. | release candidate signoff |
| slice05 | `release-notes-and-install-docs` | Update release notes, README/public docs install examples, migration pointers, and release wording from preparation language to verified v0.3.0 asset language. | wolong proof, publication |
| slice06 | `wolong-fetch-install-migration-proof` | Verify wolong can fetch/install/migrate to the packaged or published `pandapi-*` assets and classify supervised outcomes from the managed-process contract. | publication signoff |
| slice07 | `publication-workflow-proof` | Prove release workflow/publish behavior, dry-run or sandbox publication checks, tag/pre-release/final-release semantics, idempotency, and CI entrypoints through Make targets. | final publication |
| slice08 | `release-candidate-synthesis` | Reproduce the full release-candidate gate set, publish `v0.3.0` when authorized, verify downloaded assets, and close Arc09/project P9. | project close |

Detailed slice plans after Slice01 may adjust this breakdown if the inventory
finds stale release tooling, licensing gaps, missing Make targets, or wolong
integration requirements. Any change must update this plan and its Version
History before the next slice is opened.

## 4. Dependencies And Inputs

Consumes:

- Arc05 canonical native `pandapi-*` binaries and no inherited-name shim
  policy.
- Arc06 CI/test hardening, managed fixtures, sanitizer/static-analysis
  placement, warning inventory, coverage evidence, and Linux LSan re-entry.
- Arc07 first-party source-quality classification, gate scaffold, warning
  burndown, generated-code policy, and source-quality synthesis.
- Arc08 README, docs index, tutorial, managed-process, CLI reference,
  migration, architecture, docs synthesis, and release-preparation handoff.
- Existing Make release targets: `make package-release TAG=...`,
  `make publish-release TAG=...`, `make provenance-check`,
  `make readme-verbatim`, `make ci-linux`, and `make ci-macos`.
- Existing GitHub Actions release support and release notes template.
- wolong as the supervised-process acceptance consumer.

Leaves:

- Published `v0.3.0` assets, checksums, manifest/provenance, and release notes.
- Verified public install and migration documentation for `v0.3.0`.
- Verified wolong fetch/install/migration behavior.
- Project-level P9 evidence suitable for the 0.3.0 project closing report.

## 5. Current Status

- **slice01 release-readiness-inventory - open.** Slice set:
  [`slice01-release-readiness-inventory/slice-doc.md`](slice01-release-readiness-inventory/slice-doc.md),
  [`slice01-release-readiness-inventory/ledger.md`](slice01-release-readiness-inventory/ledger.md),
  [`slice01-release-readiness-inventory/cc-prompt.md`](slice01-release-readiness-inventory/cc-prompt.md).
  This slice inventories release tooling, package contents, docs, license
  assets, CI, source-quality gates, and wolong proof gaps before any release
  behavior or publication work changes.

## 6. Allowed Surfaces

Arc09 may create or edit:

| Surface | Intended ownership |
|---------|--------------------|
| `docs/design-v0.3.0/arc09-release-prep-publication/` | Arc09 plans, ledgers, reports, inventory, and close evidence. |
| `Makefile`, `mk/` | Release verification, package, CI, and publication targets when needed. |
| `tools/release/`, `tools/provenance/`, `tools/shared/` | Release/package/provenance helpers behind Make targets. |
| `.github/workflows/`, `.github/release-notes.md.in` | Release CI, publication workflow, and notes template; all jobs must invoke Make targets. |
| `release/` | Generated release output and package proof; generated artifacts should not be committed unless explicitly intended. |
| `licenses/`, `pandaPI/**/NOTICE`, license docs | License/NOTICE/source-availability evidence and corrections. |
| `README.md`, `docs/` | Public install, migration, release, and documentation updates once assets are verified. |
| `/Users/oubiwann/lab/billosys/wolong` | wolong verification only when a slice explicitly includes that workspace. |

Arc09 should not change parser, grounder, engine, or runtime product behavior
unless a release-blocking defect is found and the operator accepts a
remediation slice or scope amendment.

## 7. Arc Ledger

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Slice01 closes with a release-readiness inventory mapping package, publish, CI, docs, license/NOTICE, dependency, source-quality, and wolong gaps to planned slices. | reproduced |
| A2 | Release package archives contain only supported canonical `pandapi-*` binaries plus intended fixtures/provenance/license assets, with executable bits and no inherited `pandaPI*` commands. | reproduced |
| A3 | Checksums, release manifest, and per-component provenance are generated and verified for every supported platform artifact. | reproduced |
| A4 | License, NOTICE, source-availability, and third-party-license assets match shipped binary contents and public release notes. | reproduced |
| A5 | Release artifacts exclude test-only/developer-only dependencies and document any required user/source-build dependencies accurately. | reproduced |
| A6 | Release-quality gates run in the release context, including behavior fixtures, smoke, static analysis, formatting, warning inventory, coverage baselines, sanitizer gates, provenance, README-verbatim, and platform-specific re-entry dispositions. | reproduced |
| A7 | Release notes and public install/migration docs describe verified `v0.3.0` assets, canonical commands, checksums, provenance, licensing, and behavior without overclaiming. | reproduced |
| A8 | wolong fetch/install/migration behavior is verified against packaged or published `pandapi-*` assets and the managed-process status/exit contract. | reproduced |
| A9 | Publication workflow and `make publish-release` behavior are proven, idempotency/rerun behavior is documented, and CI entrypoints use Make targets. | reproduced |
| A10 | Final release synthesis proves Arc09 composes into project ledger P9 and supports closing the 0.3.0 project release gate. | reproduced |

## 8. Open Questions And Risks

- **OQ1 - supported platform evidence.** Existing local CDC evidence is macOS
  arm64. Arc09 must decide which rows require CI or Linux reproduction before
  publication, especially Linux package contents and LSan behavior.
- **OQ2 - release notes staleness.** The current release notes template still
  contains inherited component names and older documentation paths. Slice01
  should inventory all stale public-release language before Slice05 fixes it.
- **OQ3 - licensing depth.** The release package currently assembles license
  text from `licenses/` and references a v0.2.0 audit. Arc09 must decide what
  proof is sufficient for the changed 0.3.0 source and binary shape.
- **OQ4 - generated release artifacts.** `release/` is generated output. Slices
  must be explicit about whether evidence is committed as docs/reports or kept
  as regenerated local/CI output.
- **OQ5 - wolong boundary.** wolong verification may require coordinated edits
  in the wolong workspace. Slice06 should keep chengdu release behavior and
  wolong consumer changes clearly separated.
- **OQ6 - publication authorization.** Actual GitHub publication may require
  operator approval, credentials, and network access. Arc09 should prove as
  much as possible before that gate and record any blocked external action with
  a concrete re-entry condition.

## 9. Version History

- **v1.0 - 2026-08-14.** Opened Arc09 release-prep-publication with Slice01
  release-readiness-inventory. Surfaced by: Arc08 closed and CDC-verified,
  project ledger P9, and the Arc08 release-preparation handoff. Why: the
  public docs suite is ready, but `v0.3.0` still needs release assets,
  checksums, manifest/provenance, license/NOTICE evidence, source-quality
  release gates, wolong fetch/install/migration proof, publication behavior,
  and final release synthesis before any published-release claim is valid.
