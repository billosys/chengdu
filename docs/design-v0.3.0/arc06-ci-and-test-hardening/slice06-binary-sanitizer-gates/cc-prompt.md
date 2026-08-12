# CC Prompt - Arc06 Slice06 - binary-sanitizer-gates

You are implementing Arc06 Slice06 in `billosys/chengdu` on `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md`
4. `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
5. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice02-process-fixture-expansion/cdc-verification.md`
6. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate/cdc-verification.md`
7. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/cdc-verification.md`
8. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/warning-inventory.md`
9. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/slice-doc.md`
10. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/ledger.md`
11. `pandaPI/runtime/README.md`
12. `mk/build.mk`, `mk/tests.mk`, `mk/checks.mk`, and `mk/help.mk`

Use the top-level Makefile as the entrypoint. Run `make help` first for
wayfinding. New CI behavior must call Make targets only.

## Assignment

Extend sanitizer coverage from the existing runtime ASan/UBSan CTest gate to
representative canonical `pandapi-*` binary process workloads.

Required outcome:

- Keep `make test-runtime-sanitize` / `make sanitize-runtime` passing.
- Add Make-backed binary sanitizer target(s), listed in `make help`, for
  `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` where the local
  toolchain supports sanitizer builds.
- Run representative managed parser, grounder, engine, and pipeline fixtures
  against sanitizer-built binaries or an explicitly documented equivalent
  isolated sanitizer distribution.
- Enable ASan and UBSan where supported.
- Enable LSan where supported; if macOS or the active toolchain cannot provide
  reliable leak detection, record an explicit SKIP and re-entry condition.
- Triage any finding using the ownership tiers from Slice05:
  owned runtime/adoption, primary parser/grounder/engine, generated source,
  nested third-party/dependency-internal source, or platform/toolchain behavior.

## Boundaries

- Do not add TSan; that belongs to Slice07.
- Do not add coverage, static-analysis, compiler-warning, release package,
  README/tutorial, Arc07, or Arc08 work.
- Do not redesign planner algorithms unless a sanitizer finding requires a
  narrow fix and existing or added targeted evidence covers it.
- Do not promote inherited optional H2, cpddl/FAM, SAT, BDD/CUDD, translation,
  or interactive surfaces to supported 0.3.0 behavior.
- Do not add blanket sanitizer suppressions. Any suppression must be narrow,
  documented, and grep-verifiable.
- Do not call project scripts directly from CI/GitHub Actions; add or reuse
  Make targets first.

## Verification

Close every ledger row with exact evidence. At minimum, run:

```sh
make help
make test-runtime-sanitize
<new binary sanitizer Make target(s), or documented SKIP probes>
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

If `make check` is not run, say why and list the narrower reproduced gates.

If a sanitizer path is skipped, the closing report must name the component,
sanitizer family, platform/toolchain reason, and re-entry condition. "Not
supported" is not enough.

## Close

Write `closing-report.md` with a per-row walk and a Bubble-up to Arc06.

The bubble-up must answer:

- whether ASan/UBSan/LSan evidence is strong enough for Arc08 release
  preparation;
- which sanitizer jobs should be always-on CI, scheduled CI, local-only, or
  pre-release-only;
- whether any Slice05 warning budget item was promoted by sanitizer evidence;
- what remains for Slice07 TSan and CI synthesis.

Commit the completed slice with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
