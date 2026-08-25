# CC Prompt: Arc09 Slice01 release-readiness-inventory

You are implementing Arc09 Slice01 in `billosys/chengdu` on
`release/0.3.x`.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md`
4. `docs/design-v0.3.0/arc09-release-prep-publication/slice01-release-readiness-inventory/slice-doc.md`
5. `docs/design-v0.3.0/arc09-release-prep-publication/slice01-release-readiness-inventory/ledger.md`
6. Arc08 handoff:
   `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/docs-synthesis.md`
   and `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/closing-report.md`

## Goal

Create
`docs/design-v0.3.0/arc09-release-prep-publication/release-readiness-inventory.md`.

This is an inventory slice, not a fix slice. Inspect the current release
surface and classify what is ready, stale, missing, blocked, or deferred. Map
each finding to Arc09 slices and recommend the exact Slice02 scope.

## Required inventory coverage

Cover at least:

- Make release targets and CI entrypoints.
- Package archive contents, executable bits, canonical `pandapi-*` command
  names, old-name exclusion, fixtures, generated `release/` output, and
  package-local smoke/probe expectations.
- `SHA256SUMS`, `release-manifest.txt`, provenance fields, and per-platform
  evidence needs.
- License, NOTICE, source availability, `THIRD-PARTY-LICENSES`, and any stale
  license-audit or inherited-name references.
- User dependencies versus developer-only/test-only dependencies, including
  Catch2 exclusion from release artifacts.
- Release-quality gates: behavior fixtures, smoke, managed contracts,
  `make test-contract-stdio-managed`, static analysis, format, warning
  inventory, coverage baselines, sanitizer gates, provenance, README-verbatim,
  Linux LSan, and platform limitations.
- Release notes, README/public docs install wording, migration/reference links,
  and public overclaim risks.
- wolong fetch/install/migration verification needs, kept separate from
  chengdu release changes.
- Publication workflow, `make publish-release`, GitHub release behavior, tag
  and prerelease handling, idempotency, and Make-backed workflow policy.

## Scope rules

- Do not publish a release.
- Do not create a tag.
- Do not change parser, grounder, engine, or runtime product behavior.
- Do not update README or public docs from release-preparation language to
  published `v0.3.0` language in this slice.
- Do not change wolong.
- Do not commit generated `release/`, `dist/`, `build/`, or package output.
- Do not create `cdc-verification.md`.
- If an existing command exposes a blocker, record the blocker and re-entry
  condition instead of hiding it.

## Verification

Work against every row in the Slice01 ledger. Use Make entrypoints for build,
test, package, CI, and release operations. In particular, run the ledger's
non-publishing package probe if feasible:

```sh
make build
make package-release TAG=v0.3.0-inventory
```

Do not stage generated release output. If generated output appears, leave it
untracked/ignored or clean it only if that is already safe in the repository
workflow.

Before handing back, run:

```sh
make help
make safety-checks
make actionlint
git diff --check
git diff --cached --check
```

If you use narrower verification instead of `make check`, explain why. For
this inventory-only slice, it is acceptable to avoid aggregate `make check` if
you reproduce the ledger gates directly and avoid known macOS provenance
mutation/noise.

## Closing

Update the ledger row by row with evidence, write
`slice01-release-readiness-inventory/closing-report.md`, and update the Arc09
and project plans with the Slice01 bubble-up. Commit with the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
