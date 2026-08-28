# CC Prompt - Arc06 Slice04 - static-analysis-gate

You are implementing Arc06 Slice04 in `billosys/chengdu` on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md`
4. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
5. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate/cdc-verification.md`
6. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice04-static-analysis-gate/slice-doc.md`
7. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice04-static-analysis-gate/ledger.md`
8. `pandaPI/runtime/README.md`
9. `docs/design-v0.3.0/ci-notes.md`

Use the top-level Makefile as the entrypoint. Run `make help` first for
wayfinding. New CI behavior must call Make targets only.

## Assignment

Add or graduate C++ static-analysis gates for owned runtime/process-policy code.

Required outcome:

- Add Make-backed C++ static-analysis target(s), listed in `make help`.
- Use `pandaPI/runtime` and its compile database as the required first scope.
- Prefer `clang-tidy` as the first concrete gate if the toolchain is available.
- Add Clang Static Analyzer only if it can run reproducibly in this slice; if
  not, defer it explicitly with a re-entry condition.
- Document included, excluded, and deferred paths.
- Document missing-tool behavior and suppression policy.
- Include adoption seams only if their compile commands are reliable; otherwise
  defer them explicitly.

## Boundaries

- Do not perform the compiler-warning burndown; that is Slice05.
- Do not add sanitizer, LSan, TSan, release package, publication, Arc07
  tutorial, or Arc08 release-prep work.
- Do not change parser, grounder, or engine product behavior.
- Do not bulk-format or broadly suppress inherited/generated/third-party code.
- Do not teach users or CI to call static-analysis tools directly when a Make
  target exists.

## Verification

Close every ledger row with exact evidence. At minimum, run:

```sh
make help
<new Make static-analysis target>
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

If the local environment lacks `clang-tidy`, `scan-build`, or equivalent
analyzer tools, the target must fail or skip explicitly as designed, and the
ledger must record the deferral and re-entry condition. Silent no-op gates are
not acceptable.

## Close

Write `closing-report.md` with a per-row walk and a Bubble-up to Arc06.

The bubble-up must answer:

- whether the static-analysis signal is strong enough to inform Slice05
  warning inventory/burndown;
- whether any toolchain, suppression, or compile-database gaps remain;
- whether adoption seams are included or deferred.

Commit the completed slice with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
