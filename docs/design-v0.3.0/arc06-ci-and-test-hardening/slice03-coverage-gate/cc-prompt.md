# CC Prompt - Arc06 Slice03 - coverage-gate

You are implementing Arc06 Slice03 in `billosys/chengdu` on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md`
4. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
5. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice02-process-fixture-expansion/cdc-verification.md`
6. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate/slice-doc.md`
7. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate/ledger.md`
8. `pandaPI/runtime/README.md`

Use the top-level Makefile as the entrypoint. Run `make help` first for
wayfinding. New CI behavior must call Make targets only.

## Assignment

Implement the Slice03 coverage gate with the smallest honest scope that
delivers useful coverage evidence.

Required outcome:

- Add a Make-backed coverage entrypoint, expected as `make coverage`, listed in
  `make help`.
- Use Clang source-based coverage (`-fprofile-instr-generate`,
  `-fcoverage-mapping`, `llvm-profdata`, `llvm-cov`) when available.
- Build and test `pandaPI/runtime` under coverage instrumentation and emit a
  deterministic local report.
- Document the generated report path and coverage exclusions.
- Include chengdu-owned parser/grounder/engine adoption seams only if coverage
  instrumentation and profile collection are reliable in this slice.
- If adoption-seam coverage is not reliable, explicitly defer it with the
  technical reason and re-entry condition.
- Update Arc06 inventory/planning docs and `pandaPI/runtime/README.md` only as
  needed for active coverage behavior.

## Boundaries

- Do not change parser, grounder, or engine product behavior.
- Do not add warning burndown, `clang-tidy`, Clang Static Analyzer, ASan,
  UBSan, LSan, TSan, release package, publication, Arc07 tutorial, or Arc08
  release-prep work.
- Do not teach users or CI to call `./scripts/*` directly when a Make target
  exists.
- Do not present inherited planner, generated parser/lexer, nested
  third-party, build, or `dist/` coverage as the owned-code coverage signal.
- Do not commit generated coverage reports.

## Verification

Close every ledger row with exact evidence. At minimum, run:

```sh
make coverage
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

If you choose `make check` instead of the narrower quality sequence, report
that exactly. If a tool is unavailable and the coverage target skips, the skip
must be explicit, documented, and reflected in the ledger.

## Close

Write `closing-report.md` with a per-row walk and a Bubble-up to Arc06.

The bubble-up must answer:

- whether runtime coverage is now useful enough for Slice04 static-analysis
  planning and Slice06 sanitizer workload selection;
- whether adoption-seam coverage landed or was deferred;
- which remaining coverage gaps need routing before Arc06 close.

Commit the completed slice with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
