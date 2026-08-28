# CC Prompt: Arc04 Slice05 - fixture-and-seam-test-substrate

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
4. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/slice-doc.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md`
6. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md`
7. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md`
8. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/cdc-verification.md`
9. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
10. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
11. `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
12. `pandapi-runtime/README.md`
13. `pandapi-runtime/CMakeLists.txt`
14. `scripts/smoke-test.sh`

Also load the C++ guideline substrate for the touched rule families:

- `cpp-guidelines` `11-anti-patterns.md`
- `02-api-design.md`
- `03-functions.md`
- `05-resource-management.md`
- `06-error-handling.md`
- `10-expressions-and-statements.md`
- `12-project-structure-and-tooling.md`
- `13-standard-library.md`

## Mission

Implement Arc04 Slice05: a tested fixture and seam-test substrate for the
managed-process contract.

The deliverable is reusable infrastructure, not parser/grounder/engine
conformance yet. Arc05 will use this substrate to express and run executable
contract fixtures. This slice should provide:

- typed fixture records for command, argv, environment, stdin, stdout, stderr,
  artifacts, exit status, wait status, final status, normalization, and safety;
- normalization helpers for paths, LF line endings, ANSI, durations,
  timestamps, build metadata, errno, and signals;
- comparison helpers that separate golden fields from semantic predicates;
- final `PANDAPI_STATUS` matching through the accepted status parser;
- a local Linux/macOS process-observation substrate that can capture stdout,
  stderr, exit code, wait status, timeout disposition, and cleanup state;
- safe timeout/resource/signal probe vocabulary and tests;
- runtime tests proving the substrate with tiny harness-local commands or
  helper binaries;
- a test-only Catch2 integration gate that cannot fetch dependencies, cannot
  leak into public headers, and does not change the default build when Catch2
  is unavailable;
- README and CMake updates.

Keep this slice inert with respect to the inherited binaries. Do not start
Arc05 early.

## Hard Boundaries

- No edits under `pandaPI/`.
- No adoption of this substrate by parser, grounder, or engine binaries.
- No `pandapi-*` wrapper, symlink, copied binary, release packaging, or
  inherited-name migration work.
- No parser, grounder, or engine conformance fixtures in this slice. Tiny
  harness self-tests are allowed; executable planner contract tests are Arc05.
- No replacement of `gengetopt`, no CLI11 import, and no CLI parser migration.
- No fmt adoption unless it remains behind the existing diagnostics/process
  facade and no direct formatting dependency appears in public headers.
- No `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL,
  google/benchmark, SAT/BDD/CUDD, H2/cpddl dependency-internal work, or
  optional dependency audits.
- No `FetchContent`, `ExternalProject`, package-manager bootstrap, `curl`,
  `git clone`, or network download in the default build.
- Catch2 is test-only. Do not expose Catch2 types from public headers. Do not
  make the default runtime build fail when Catch2 is not locally available.
- Do not touch workflow, top-level README, release asset, manifest,
  `vendor.env`, `pins.env`, `dist`, `build`, or `release` paths.
- Do not create `cdc-verification.md`; CDC owns that file.

If you discover that the fixture substrate truly needs reproc++, Boost.Process,
JSON, or a non-test dependency in this slice, stop and record an amendment
request or bubble-up rather than importing it silently.

## Implementation Guidance

Prefer small, strongly typed C++17 APIs. Suggested shapes, names optional:

- `FixtureSpec` for one process fixture;
- `CommandSpec` for executable path, argv, environment, working directory, and
  stdin policy;
- `StreamExpectation` for stdout/stderr roles and comparison policy;
- `OutputArtifactExpectation` for file artifact path and expected state;
- `ExpectedExit` and `WaitStatusExpectation` for exited versus signaled
  outcomes;
- `FinalStatusExpectation` for required/forbidden/absent tagged status;
- `NormalizationRules` for path tokens, line endings, ANSI, durations,
  timestamps, build metadata, errno, and signals;
- `ComparisonResult` for structured mismatch reporting;
- `ProcessObservation` for captured stdout, stderr, exit code, wait status,
  timeout, and cleanup outcome.

For process execution, a small local Linux/macOS implementation is acceptable.
Use RAII-style wrappers for file descriptors and child-process handles. Keep
shell use out of production runtime APIs. If a shell is used in tests, make it
explicit, fixture-local, and deterministic.

For timeout and signal tests:

- default tests must be CI-safe and bounded;
- harness-timeout tests may use a tiny helper command that sleeps past a short
  timeout and is killed by the harness;
- signal normalization can be tested through local wait-status/status-mapping
  helpers without destructive `SIGKILL` default tests;
- resource-limit probes should be represented as safe/quarantine/manual until
  a deterministic binary-owned limit exists.

For Catch2:

- add only a test-only gate or adapter;
- avoid network fetching and default-build dependency;
- document the local availability expectation;
- keep existing assert/CTest smoke tests working;
- make release exclusion obvious to Arc06.

## Required Verification

Run the ledger commands and record the commit SHA plus outputs in
`ledger.md`. At minimum, run:

```bash
./scripts/build-runtime.sh
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
```

Also verify:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md
```

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all 24 rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc04, including whether slice06 planning should change.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
