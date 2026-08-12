# CC Prompt - Arc06 Slice05 - compiler-warning-burndown

You are implementing Arc06 Slice05 in `billosys/chengdu` on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md`
4. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
5. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice04-static-analysis-gate/cdc-verification.md`
6. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/slice-doc.md`
7. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/ledger.md`
8. `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
9. `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
10. `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
11. `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`

Use the top-level Makefile as the entrypoint. Run `make help` first for
wayfinding. New CI behavior must call Make targets only.

## Assignment

Inventory and burn down compiler warnings before sanitizer/release evidence
starts depending on a noisy build.

Required outcome:

- Capture the current warning stream reproducibly.
- Classify warning classes by ownership tier:
  primary hand-written parser, grounder, engine, runtime, generated source,
  nested third-party/dependency-internal source, copied build-tree artifacts,
  and platform/toolchain-only noise.
- Disposition Arc01 warning findings `P-011`, `G-015`, and `E-011`.
- Fix low-risk primary hand-written warnings where behavior-preserving and
  covered by existing fixtures.
- Document remaining warning budgets, suppressions, and re-entry conditions.
- Keep any new warning inventory/check workflow Make-backed.

## Boundaries

- Do not redesign planner algorithms or change accepted process behavior.
- Do not blanket-promote generated or third-party warnings to `-Werror`.
- Do not hide warning classes with broad suppressions.
- Do not regenerate generated source unless the generator path is documented
  and the warning fix requires it.
- Do not add sanitizer, LSan, TSan, coverage, release-package, Arc07 tutorial,
  or Arc08 publication work.
- Do not expand unsupported optional surfaces.

## Verification

Close every ledger row with exact evidence. At minimum, run:

```sh
make help
<warning inventory command or Make target>
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

If a warning remains, the ledger/closing report must identify its ownership
tier, why it remains, and the release re-entry condition. "Inherited warning"
without a concrete tier is not enough.

## Close

Write `closing-report.md` with a per-row walk and a Bubble-up to Arc06.

The bubble-up must answer:

- whether the warning state is clean enough for Slice06 binary sanitizer gates
  to produce meaningful release-readiness signal;
- which warning classes, if any, Arc08 release publication must mention or gate;
- whether any suppression or warning budget should become a long-term Make/CI
  policy.

Commit the completed slice with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
