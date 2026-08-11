# CC Prompt: Arc06 Slice02 process-fixture-expansion

You are implementing Arc06 Slice02 for chengdu 0.3.0 on `release/0.3.x`.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md`
4. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
5. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/cdc-verification.md`
6. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice02-process-fixture-expansion/slice-doc.md`
7. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice02-process-fixture-expansion/ledger.md`
8. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`

## Goal

Expand Arc06 process fixtures in the first high-value batch from Slice01:
managed pipeline composition, pairwise artifact composition, positive
`--supervised` cases, and deterministic output-finalization failure coverage
where portable.

## Required Work

1. Add a Make-backed managed pipeline fixture target, expected name:
   `test-contract-pipeline-managed`.
2. Extend `tests/contract/run` only as needed to list/select/run
   `--contract --component pipeline`.
3. Add managed pipeline fixture records and implementation for a canonical
   file-backed parse-ground-solve chain using:
   - `pandapi-parser`
   - `pandapi-grounder`
   - `pandapi-engine`
   - `--status=stderr`
4. Add pairwise composition cases:
   - parser stdout captured to a file and accepted by grounder;
   - grounder stdout captured to a file and accepted by engine.
   Do not invent stdin support if the accepted CLI does not support it.
5. Add positive `--supervised` cases for parser, grounder, and engine.
6. Add deterministic output-finalization failure coverage if a portable fixture
   mechanism exists. If it does not, close that row as deferred with a concrete
   reason and re-entry condition.
7. Update `fixture-gap-inventory.md` with what Slice02 covered and what remains
   for coverage/sanitizer/TSan slices.
8. Update the Slice02 ledger as you work.
9. Write `closing-report.md` with a row-by-row ledger walk and **Bubble-up to
   the arc**.

## Constraints

- Use Make targets as the public interface. New CI or handoff behavior must
  call Make, not project scripts.
- Keep canonical `pandapi-*` commands as the accepted 0.3.0 surface.
- Do not reintroduce inherited `pandaPI*` command compatibility as accepted
  behavior.
- Do not promote optional inherited surfaces.
- Do not add coverage, `clang-tidy`, Clang Static Analyzer, sanitizer, LSan,
  or TSan gates in this slice.
- Do not edit release package/publish/checksum/manifest/wolong surfaces.
- Do not edit README, Arc07 tutorial/docs, or Arc08 release-prep docs.
- Use current source paths:
  - `pandaPI/parser`
  - `pandaPI/grounder`
  - `pandaPI/engine`
  - `pandaPI/runtime`

## Verification

Run at minimum:

```sh
make test-contract-list-managed
make test-contract-pipeline-managed
make test-contract-parser-managed
make test-contract-grounder-managed
make test-contract-engine-managed
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

If a listed fixture cannot be made portable, do not fake it. Record a deferred
ledger row with exact rationale and re-entry condition.

## Commit

Commit the slice when complete, with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
