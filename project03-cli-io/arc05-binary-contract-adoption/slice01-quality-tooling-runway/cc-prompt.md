# CC Prompt: Arc05 Slice01 - quality-tooling-runway

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`
4. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/slice-doc.md`
5. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md`
6. `docs/design-v0.3.0/ci-notes.md`
7. `docs/design-v0.3.0/arc04-shared-runtime-substrate/closing-report.md`
8. `docs/design-v0.3.0/arc04-shared-runtime-substrate/substrate-integration-readiness.md`
9. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
10. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
11. `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
12. `.github/workflows/build-reusable.yml`
13. `.github/workflows/build.yml`
14. `pandapi-runtime/CMakeLists.txt`
15. `pandapi-runtime/README.md`
16. `scripts/build-runtime.sh`
17. `scripts/build-all.sh`
18. `scripts/smoke-test.sh`

Also load the C++ guideline substrate for any touched C++ or build-facing
code:

- `cpp-guidelines` `11-anti-patterns.md`
- `12-project-structure-and-tooling.md`
- `08-concurrency.md` if sanitizer or TSan rationale touches concurrency
- `10-expressions-and-statements.md` if sanitizer notes touch arithmetic/UB
- `13-standard-library.md` if scripts/docs discuss library boundaries

## Mission

Implement Arc05 Slice01: quality-tooling-runway.

Your job is to put the agreed pre-adoption quality floor in place before any
parser, grounder, or engine behavior changes begin:

- add a top-level `.clang-format`;
- add an owned-source formatting check script scoped to chengdu-owned source;
- add a runtime ASan/UBSan sanitizer script;
- ensure runtime CMake builds produce or preserve `compile_commands.json`;
- update CI to run the runtime/build and quality gates that are reliable now;
- document the `clang-tidy`, coverage, TSan, and Arc06 release-proof
  deferrals;
- close the ledger and write the CC closing report.

This slice is a runway. It is not binary adoption.

## Hard Boundaries

- No edits under `pandaPI/`.
- No adoption of `pandapi::runtime` by parser, grounder, or engine binaries.
- No `pandapi-*` wrapper, symlink, copied binary, renamed binary, release
  entry point, package change, or inherited-name migration.
- No CLI11 import, CLI parser migration, or `gengetopt` replacement.
- No fmt, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL,
  google/benchmark, SAT/BDD/CUDD, H2/cpddl dependency-internal work, optional
  dependency audit, or parser-generator work.
- No `FetchContent`, `ExternalProject`, package-manager bootstrap, `curl`,
  `git clone`, or network download in the default build.
- No release-package dry-run, checksum, manifest, license/NOTICE,
  test-only dependency exclusion, wolong fetch/install/migration, package,
  publish, or release workflow changes.
- Do not create Arc05 `closing-report.md`; arc close is CDC/next-step work
  after all Arc05 slices are CDC-verified.
- Do not create `cdc-verification.md`; CDC owns that file.

If you discover that a tooling gate requires crossing these boundaries, stop
and record an amendment request in the closing report instead of widening the
slice.

## Implementation Guidance

Recommended shape:

1. Add `.clang-format`.
   - Base it on a conservative LLVM-style C++17-friendly layout unless the
     existing runtime code strongly points elsewhere.
   - Do not bulk-format `pandaPI/`.

2. Add `scripts/check-format-owned.sh`.
   - Use `set -euo pipefail`.
   - Locate `clang-format` either from `PATH` or from `xcrun --find
     clang-format`.
   - Check owned files under `pandapi-runtime/include`, `pandapi-runtime/src`,
     and `pandapi-runtime/tests`.
   - Future adapter directories can be added when they exist.

3. Add `scripts/sanitize-runtime.sh`.
   - Use Clang if available.
   - Configure out of source under a sanitizer-specific `build/` path.
   - Use ASan/UBSan flags like `-O1 -g -fno-omit-frame-pointer
     -fsanitize=address,undefined`.
   - Build and run CTest with `--output-on-failure`.
   - Do not produce release binaries.

4. Ensure compile commands are available for the runtime CMake build.
   - Prefer `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` in runtime/tooling builds.
   - Do not force Makefile-era parser/grounder sources into clang-tidy yet.

5. Update CI carefully.
   - Existing CI has an actionlint job and Linux/macOS build jobs.
   - Add runtime build coverage before inherited builds.
   - Add owned-source format and sanitizer gates only where tool availability
     is reliable or installed explicitly.
   - Keep release workflows out of scope.

6. Document deferrals.
   - `clang-tidy`: implement only if the tool is reliable in the chosen
     local/CI environment; otherwise document the install/toolchain gate.
   - coverage: planned for Arc05 after fixtures drive migrated binaries.
   - TSan: later/heavier once subprocess, timeout/signal, and stream draining
     workloads are representative.
   - Arc06: release packaging, license/NOTICE, test-only exclusion, wolong,
     checksums, manifests, and publication gates.

## Required Verification

Run the ledger commands and record outputs in `ledger.md`. At minimum, run:

```bash
./scripts/check-format-owned.sh
./scripts/sanitize-runtime.sh
./scripts/build-runtime.sh
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
rg -c '^\| F-' docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md
! test -e docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/cdc-verification.md
```

If local `actionlint` is available, run it. If it is unavailable, record that
the existing CI actionlint job remains the workflow syntax gate.

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all 21 rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc05, including whether slice02 contract-fixture-scaffold is
  ready to open after independent CDC verification.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
