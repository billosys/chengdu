# CC Prompt: Arc05 Slice02 - contract-fixture-scaffold

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`
4. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/slice-doc.md`
5. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/ledger.md`
6. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/cdc-verification.md`
7. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
8. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
9. `docs/design-v0.3.0/arc04-shared-runtime-substrate/substrate-integration-readiness.md`
10. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md`
11. `docs/design-v0.3.0/ci-notes.md`
12. `pandapi-runtime/include/pandapi/runtime/fixture.hpp`
13. `pandapi-runtime/include/pandapi/runtime/normalization.hpp`
14. `pandapi-runtime/include/pandapi/runtime/process_fixture.hpp`
15. `scripts/smoke-test.sh`
16. `.github/workflows/build-reusable.yml`

Also load the C++ guideline substrate for any touched C++ or build-facing
code:

- `cpp-guidelines` `11-anti-patterns.md`
- `12-project-structure-and-tooling.md`
- `08-concurrency.md` if you touch process observation, timeouts, signals, or
  stdin handling
- `10-expressions-and-statements.md` if you touch parsing, arithmetic, status,
  or signal/exit-code normalization
- `13-standard-library.md` if you add helper code or discuss library
  boundaries

## Mission

Implement Arc05 Slice02: contract-fixture-scaffold.

Your job is to create the black-box process fixture scaffold that future
parser, grounder, and engine adoption slices will extend. The scaffold should
make fixture structure, baseline observations, and future contract expectations
concrete, and it should provide an executable baseline gate over the current
inherited binaries.

This is not binary adoption. Current inherited behavior may be captured as a
baseline, but do not make it appear to be final managed-process conformance.

## Hard Boundaries

- No edits under `pandaPI/`.
- No adoption of `pandapi::runtime` by parser, grounder, or engine binaries.
- No canonical `pandapi-parser`, `pandapi-grounder`, or `pandapi-engine`
  wrapper, symlink, copied binary, renamed binary, release entry point, or
  package change.
- No inherited-name migration and no default inherited-name deprecation
  warning.
- No CLI11 import, CLI parser migration, or `gengetopt` replacement.
- No final `PANDAPI_STATUS` emission by inherited binaries.
- No `FetchContent`, `ExternalProject`, package-manager bootstrap, `curl`,
  `git clone`, or network download in the default build/test path.
- No release-package dry-run, checksum, manifest, license/NOTICE,
  test-only dependency exclusion, wolong fetch/install/migration, package,
  publish, or release workflow changes.
- Do not create Arc05 `closing-report.md`; arc close waits for all Arc05
  slices.
- Do not create `cdc-verification.md`; CDC owns that file.

If a useful fixture requires crossing these boundaries, stop and record an
amendment request in the closing report instead of widening the slice.

## Implementation Guidance

Recommended shape:

1. Add `fixtures/contract/README.md`.
   - Document the fixture schema and the relationship to Arc03's matrix.
   - Explain baseline observations versus future contract expectations.
   - Keep the black-box/Catch2 distinction explicit.
   - Record safety policy and the stdin/full-duplex caveat.

2. Add fixture records or fixture metadata under `fixtures/contract/`.
   - Use a representation that does not require a new default dependency.
   - Include component, surface disposition, command/argv, stdin, stdout,
     stderr, artifacts, exit/status, final status, normalization, and safety.
   - It is acceptable for some future contract expectations to be documented
     but not executable until the owning binary slice migrates behavior.

3. Add `scripts/run-contract-fixtures.sh`.
   - Use `set -euo pipefail`.
   - Detect the platform via `scripts/lib-platform.sh`.
   - Operate against `dist/<platform>/pandaPIparser`,
     `dist/<platform>/pandaPIgrounder`, and `dist/<platform>/pandaPIengine`.
   - Either require callers to run `./scripts/build-all.sh` first, or perform
     that build explicitly with labeled failure output.
   - Support at least `--baseline`, `--component parser|grounder|engine|pipeline`,
     and `--list` or an equivalent fixture-selection interface.
   - Capture stdout, stderr, and exit status separately.
   - Use isolated temp directories and clean them up by default.
   - Produce clear PASS/FAIL lines and a final summary.

4. Add executable baseline probes.
   - Parser: successful parse, missing file, broken syntax, broken reference.
   - Grounder: successful grounding from a generated `.htn`, plus at least one
     missing or malformed `.htn` case.
   - Engine: solved and unsolvable outcomes, with unsolvable kept distinct
     from malformed input and generic failure.
   - Pipeline: parser -> grounder -> engine with generated temp artifacts.

5. Consider CI integration.
   - If reliable, add the baseline fixture gate after build and before smoke.
   - If not reliable, document the blocker and re-entry condition precisely in
     the closing report.

## Required Verification

Run the ledger commands and record outputs in `ledger.md`. At minimum, run:

```bash
./scripts/run-contract-fixtures.sh --baseline
./scripts/run-contract-fixtures.sh --baseline --component parser
./scripts/run-contract-fixtures.sh --baseline --component grounder
./scripts/run-contract-fixtures.sh --baseline --component engine
./scripts/run-contract-fixtures.sh --baseline --component pipeline
./scripts/check-format-owned.sh
./scripts/build-runtime.sh
./scripts/sanitize-runtime.sh
./scripts/build-all.sh
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
git diff --name-only -- pandaPI
git diff --name-only -- release-manifest.txt vendor.env pins.env dist release scripts/package-release.sh scripts/publish-release.sh
```

Also verify:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/ledger.md
! test -e docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/cdc-verification.md
```

If local `actionlint` is available, run it. If it is unavailable, record that
the existing CI actionlint job remains the workflow syntax gate.

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc05, including whether slice03 parser-contract-adoption is
  ready to open after independent CDC verification.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
