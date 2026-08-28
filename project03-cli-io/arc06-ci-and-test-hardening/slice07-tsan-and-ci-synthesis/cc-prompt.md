# CC Prompt - Arc06 Slice07 - tsan-and-ci-synthesis

You are implementing Arc06 Slice07 in `billosys/chengdu` on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md`
4. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
5. `docs/design-v0.3.0/ci-notes.md`, especially section 8.3 TSan
6. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice02-process-fixture-expansion/cdc-verification.md`
7. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate/cdc-verification.md`
8. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice04-static-analysis-gate/cdc-verification.md`
9. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/cdc-verification.md`
10. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/cdc-verification.md`
11. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/slice-doc.md`
12. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/ledger.md`
13. `mk/ci.mk`, `mk/build.mk`, `mk/tests.mk`, `mk/checks.mk`, and `mk/help.mk`

Use the top-level Makefile as the entrypoint. Run `make help` first for
wayfinding. New CI behavior must call Make targets only.

## Assignment

Complete the final Arc06 hardening slice:

- Decide whether TSan can provide meaningful evidence with the current
  runtime, supervised fixture, shell harness, parser, grounder, engine, and
  nested dependency workload.
- If yes, add a Make-backed TSan target and run it against the meaningful
  workload.
- If no, explicitly defer TSan with a concrete reason, owner, and re-entry
  condition. A precise deferral is a valid outcome.
- Synthesize CI placement for Arc06 gates: coverage, static analysis, warning
  inventory, runtime sanitizer, binary sanitizer, LSan re-entry, and TSan.
- Update the fixture/gate inventory and Arc06 docs so Arc07 and Arc08 can
  start without rediscovering the evidence.
- If all Arc06 rows are satisfied, write the Arc06 closing report. If not,
  identify the exact missing row and next iteration.

## Boundaries

- Do not write Arc07 tutorial, README, behavior-change table, or public docs.
  Only prepare handoff inputs.
- Do not write Arc08 release packages, checksums, license/NOTICE proof, wolong
  migration proof, or publication work. Only prepare release-prep gate inputs.
- Do not expand inherited optional surfaces.
- Do not combine TSan with ASan/UBSan.
- Do not add broad planner redesign, dependency audits, or warning burndown
  unless a TSan finding requires a narrow, evidenced fix.
- Do not call project scripts directly from CI/GitHub Actions; add or reuse
  Make targets first.

## Verification

Close every ledger row with exact evidence. At minimum, run:

```sh
make help
<TSan Make target, or documented no-target/deferred probes>
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

Run `make test-runtime-sanitize` and `make test-binary-sanitize` if you change
sanitizer placement, sanitizer Make plumbing, or sanitizer documentation
semantics. If you do not run them, say why and reference Slice06 CDC evidence.

If `make check` is not run, say why and list the narrower reproduced gates.

## Close

Write `closing-report.md` for Slice07 with a per-row walk and Bubble-up to
Arc06.

If Arc06 can close, also write
`docs/design-v0.3.0/arc06-ci-and-test-hardening/closing-report.md` with:

- Arc06 GO/NO-GO verdict;
- per-arc-row evidence for A1-A12;
- final CI/local/pre-release gate map;
- Arc07 documentation/tutorial handoff inputs;
- Arc08 release-prep handoff inputs and remaining release gates;
- explicit deferrals and re-entry conditions.

Commit the completed slice with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
