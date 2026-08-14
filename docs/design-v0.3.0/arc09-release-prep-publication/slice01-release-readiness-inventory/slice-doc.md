# Arc09 Slice01: release-readiness-inventory

Status: open
Opened: 2026-08-14

## Goal

Create the release-readiness inventory for `v0.3.0`: a durable report that
maps the current release tooling, package outputs, CI workflows, public docs,
license/NOTICE/source-availability assets, dependency boundaries,
source-quality gates, and wolong verification needs against project ledger
P9 and Arc09 A1-A10.

This slice does not fix release behavior. It makes the release surface
inspectable, names stale or missing pieces, and recommends the exact next
slice order before package, docs, CI, wolong, or publication changes begin.

## Scope

In scope:

- Inspect current Make release targets, package helper behavior, provenance
  helper behavior, release notes template, GitHub Actions release workflow, and
  public docs release wording.
- Generate a durable inventory at
  `docs/design-v0.3.0/arc09-release-prep-publication/release-readiness-inventory.md`.
- Classify each release-readiness item as ready, stale, missing, blocked, or
  deferred with a re-entry condition.
- Map every gap to an Arc09 slice or an explicit no-op/defer rationale.
- Recommend whether the Arc09 slice breakdown needs amendment before Slice02.

Out of scope:

- No release publication.
- No tag creation.
- No changes to parser, grounder, engine, or runtime product behavior.
- No release package behavior changes unless the operator explicitly amends
  the slice.
- No README/public-doc install update from preparation language to published
  `v0.3.0` language.
- No wolong code changes.
- No committed generated `release/`, `dist/`, `build/`, or package outputs.

## Verification Approach

This slice is documentation and inventory work. Verification is primarily
grep/read evidence plus non-publishing Make-backed probes where useful. The
inventory should be specific enough that Slice02 can start from named gaps
instead of rediscovering the release surface.

Use Make entrypoints for build/test/release operations. Do not teach or add
new CI jobs that call scripts directly.

## Exit Criteria

- Arc09 `arc-plan.md` remains current after inventory findings.
- `release-readiness-inventory.md` exists and covers package assets,
  checksums, manifest/provenance, release notes/docs, license/NOTICE,
  dependency/test-only boundaries, source-quality release gates, CI/workflow
  entrypoints, wolong proof, publication/idempotency, and generated-output
  handling.
- The inventory contains a recommendation for Slice02 scope and any needed
  Arc09 slice-order amendment.
- Existing release/package targets are not changed by this slice unless the
  operator explicitly amends scope.
- No close-set `cdc-verification.md` is created by CC.
