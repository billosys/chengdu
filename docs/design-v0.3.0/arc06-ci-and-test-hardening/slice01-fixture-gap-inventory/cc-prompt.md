# CC Prompt: Arc06 Slice01 fixture-gap-inventory

You are implementing Arc06 Slice01 for chengdu 0.3.0 on `release/0.3.x`.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md`
4. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/slice-doc.md`
5. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/ledger.md`
6. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
7. `docs/design-v0.3.0/arc05-binary-contract-adoption/closing-report.md`
8. `docs/design-v0.3.0/ci-notes.md`

## Goal

Produce the Arc06 fixture and gate inventory that lets later slices harden the
right things in the right order. This slice is an inventory and Make-target
surface repair, not a fixture-expansion or behavior-change slice.

## Required Work

1. Inspect current fixture and gate surfaces:
   - `fixtures/contract/`
   - `tests/contract/run`
   - `tests/smoke/run`
   - `mk/*.mk`
   - `.github/workflows/*.yml`
   - `pandaPI/runtime/`
2. If Make cannot list managed contract fixtures without direct harness calls,
   add a narrow Make target such as `test-contract-list-managed`, and list it
   in `make help`.
3. Create:
   - `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
4. The report must include:
   - current baseline fixture counts by component;
   - current managed contract fixture counts by component;
   - current smoke and pipeline coverage;
   - mapping from current fixture IDs to Arc03 matrix categories;
   - missing/deferred/superseded obligations;
   - owner routing for each gap: Arc06 Slice02-Slice06, Arc07, Arc08, or no-op;
   - Make and GitHub Actions gate map;
   - recommended first fixture-expansion batch for Slice02.
5. Update the slice ledger with attested evidence.
6. Write `closing-report.md` for this slice, including:
   - row-by-row ledger walk;
   - verification commands and outcomes;
   - **Bubble-up to the arc** with the recommended Slice02 batch and any arc
     plan changes needed before Slice02 opens.

## Constraints

- Use Make targets as the public interface. Do not add new CI or handoff text
  that teaches direct `tests/contract/run` or `tests/smoke/run` invocation when
  a Make target exists.
- Do not change parser, grounder, or engine behavior.
- Do not add new fixtures in this slice beyond list/inventory support.
- Do not add coverage, sanitizer, TSan, `clang-tidy`, or workflow hardening
  gates yet.
- Do not edit release package/publish/checksum/manifest/wolong surfaces.
- Do not edit public tutorial/README/migration prose except through narrow
  internal Arc06 handoff notes.
- Use current source paths:
  - `pandaPI/parser`
  - `pandaPI/grounder`
  - `pandaPI/engine`
  - `pandaPI/runtime`

## Verification

Run at minimum:

```sh
make test-contract-list
make test-contract-list-managed
make actionlint
make static-analysis
make format-check
make safety-checks
make test
make provenance-check
git diff --check
git diff --cached --check
```

If you intentionally run narrower substitutes, explain exactly why in the
closing report.

## Commit

Commit the slice when complete, with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
