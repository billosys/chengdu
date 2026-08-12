# Arc06 Slice06: binary-sanitizer-gates

## Goal

Extend sanitizer evidence from the owned runtime CTest gate to representative
canonical `pandapi-*` process workloads. Build and run ASan/UBSan, and LSan
where supported, through Make-backed entry points so sanitizer failures can be
triaged against the warning ownership budget closed in Slice05.

This slice follows Slice05 because compiler-warning ownership is now explicit:
runtime is the zero-warning owned surface, low-risk primary parser and grounder
warnings are burned down, and remaining generated/dependency/engine warning
budgets have re-entry criteria. That classification is the floor for honest
sanitizer triage.

## In Scope

- Preserve or reuse the existing runtime sanitizer gate:
  `make test-runtime-sanitize` / `make sanitize-runtime`.
- Add Make-backed binary sanitizer target(s) for canonical
  `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` where the local
  toolchain supports sanitizer builds.
- Run representative managed contract fixtures under sanitizer-built binaries
  or an explicitly documented equivalent isolated sanitizer distribution.
- Include ASan and UBSan where supported by the platform/toolchain.
- Include LSan where supported; if macOS or the local toolchain cannot provide
  reliable leak detection, document the exact skip, re-entry condition, and
  Linux/CI expectation.
- Reuse Slice02's managed parser, grounder, engine, and pipeline workload
  shape so the sanitizer gate exercises more than minimal runtime CTest paths.
- Triage any sanitizer finding by ownership tier: owned runtime/adoption code,
  primary parser/grounder/engine code, generated code, nested third-party code,
  or platform/toolchain behavior.
- Update GitHub Actions only through Make targets if a CI sanitizer gate is
  added or changed.

## Out of Scope

- No TSan. ThreadSanitizer and race-detection workload disposition belong to
  Slice07.
- No coverage, static-analysis, compiler-warning, release-package, Arc07
  tutorial, or Arc08 publication work.
- No algorithm redesign or broad inherited-source cleanup unless required to
  fix a reproduced sanitizer failure and covered by targeted evidence.
- No optional-surface expansion. Sanitizer coverage must not promote inherited
  H2, cpddl/FAM, SAT, BDD/CUDD, translation, or interactive surfaces to
  supported 0.3.0 behavior.
- No blanket sanitizer suppressions over generated or nested third-party code.
  Any suppression must be narrow, documented, and grep-verifiable.

## Verification Approach

- `make help` must list any new binary sanitizer target.
- `make test-runtime-sanitize` or `make sanitize-runtime` must continue to
  pass.
- Binary sanitizer target(s) must build sanitizer-instrumented canonical
  `pandapi-*` binaries or document a precise toolchain limitation with
  re-entry condition.
- Representative managed fixture workload must run against the sanitizer
  binary surface where supported.
- ASan/UBSan/LSan disposition must be explicit by platform and component.
- Findings, skips, suppressions, and remaining risks must reference ownership
  tiers from Slice05 rather than using generic inherited-noise wording.
- Workflow changes, if any, must call Make targets and pass `make safety-checks`
  plus `make actionlint`.

## Exit Criteria

- Runtime sanitizer baseline still passes.
- Canonical binary sanitizer workload exists through Make, or each unavailable
  component/platform path has a concrete skip and re-entry condition.
- Representative parser, grounder, engine, and pipeline process fixtures run
  under sanitizer coverage where supported.
- ASan, UBSan, and LSan support state is documented honestly for the local
  platform and any CI platform touched by the slice.
- No sanitizer finding in owned runtime/adoption or primary source remains
  untriaged.
- Slice ledger rows close with evidence, and CDC can reproduce the sanitizer
  target behavior, skips, fixture workload, CI entrypoint policy, and
  regression gates.

## Bubble-up Expectation

The closing report must say whether ASan/UBSan/LSan evidence is strong enough
for Arc08 release preparation, which sanitizer jobs should be local-only,
always-on CI, scheduled CI, or pre-release-only, and whether any Slice05 warning
budget item was promoted by sanitizer evidence.
