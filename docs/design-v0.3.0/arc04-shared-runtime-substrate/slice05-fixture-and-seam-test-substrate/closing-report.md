# Slice 05: Fixture and Seam-Test Substrate Closing Report

## Capability Verdict

Delivered. The runtime now has a reusable fixture and seam-test substrate for
typed fixture records, golden-output normalization, structured comparison,
final-status matching through the accepted tagged status parser, and local
Linux/macOS process observation.

This slice did not adopt the substrate into parser, grounder, or engine. It did
not add `pandapi-*` executable entry points, release assets, packaging changes,
or CDC verification.

## Implementation Decisions

- Split the substrate into `fixture.hpp`, `normalization.hpp`, and
  `process_fixture.hpp` so fixture vocabulary, text/status normalization, and
  local process observation remain independently reviewable.
- Implemented direct POSIX process execution with local RAII file-descriptor
  ownership in the `.cpp` file. The public API exposes observations, not raw
  process handles or ownership-transfer pointers.
- Kept wait status distinct from process exit code. Signal helpers cover
  exited/signaled state and supervisor-style `128 + signal_number` mapping.
- Represented harness timeout, binary timeout, resource-limit, quarantine,
  manual, and CI-safe/default probe policy as typed fixture data.
- Matched final status through `parse_status_record` and `StatusRecord`, not
  through human diagnostic regex matching.
- Added structured comparison results for byte-stable golden fields and
  semantic predicates.
- Added an assert-based CTest smoke test as the default proof and an optional
  Catch2 smoke test that is built only when a local Catch2 package is already
  available.
- Corrected the F-16 and F-18 ledger Verify commands to test for empty
  protected-path diff output. The original `! git diff --name-only` form failed
  on correct no-diff output because `git diff --name-only` exits zero.

## Verification

- `./scripts/build-runtime.sh` passed on macos-arm64. The default run reported
  Catch2 absent and skipped the optional test-only gate; all five registered
  CTests passed.
- `./scripts/build-parser.sh` passed on macos-arm64 with inherited warnings.
- `./scripts/build-grounder.sh` passed on macos-arm64 with inherited warnings.
- `./scripts/build-engine.sh` passed on macos-arm64 with inherited warnings.
- `./scripts/smoke-test.sh --negative` passed.
- `git diff --check` passed.
- `git diff --cached --check` passed.
- `git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release` returned no output.
- `rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md` returned `24`.
- `! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md` passed.
- `! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md` passed.

## Ledger Walk

- F-1: Open-set documentation exists and covers the required fixture, matrix,
  boundary, Catch2, and C++ pressure terms.
- F-2: `pandapi-runtime/README.md` documents the implemented substrate and no
  binary adoption.
- F-3: Public runtime headers exist for fixture records, normalization, and
  process observation.
- F-4: Fixture types cover command identity, argv, environment, stdin,
  streams, artifacts, exits, wait status, final status, normalization, and
  safety policy.
- F-5: Process observation captures stdout, stderr, exit code, wait status,
  timeout disposition, and cleanup state without reproc++ or Boost.Process.
- F-6: Probe policy distinguishes harness timeout, binary timeout, resource
  limit, quarantine, manual, and CI-safe probes.
- F-7: Wait-status and signal helpers cover exited/signaled state, SIGINT,
  SIGTERM, SIGKILL, and supervisor exit-code mapping.
- F-8: Final status matching uses the accepted tagged status parser.
- F-9: Normalization covers path tokens, LF line endings, ANSI handling,
  durations, timestamps, build metadata, errno, and signals.
- F-10: Comparison helpers distinguish byte-stable golden fields from semantic
  predicates and return structured failures.
- F-11: Stream and artifact expectation helpers encode output roles, artifact
  states, and ANSI-forbidden machine streams.
- F-12: Runtime tests exercise fixture records, normalization, status matching,
  comparisons, process observation, failures, and timeout behavior.
- F-13: CMake builds the new sources and registers the fixture substrate tests.
- F-14: Catch2 integration is optional, local, test-only, and not fetched.
- F-15: No prohibited dependency family was vendored, fetched, included, or
  exposed.
- F-16: Test-only material is not routed into release packaging, manifests, or
  public installation surfaces.
- F-17: Public headers avoid the listed C++ Core Guidelines hazards.
- F-18: The fixture substrate was not adopted into parser, grounder, or engine
  and no `pandapi-*` entry points were added.
- F-19: Runtime build and registered runtime tests pass.
- F-20: Existing parser, grounder, and engine builds pass.
- F-21: Existing negative smoke behavior passes.
- F-22: Protected planner, workflow, README, release, manifest, pins, `dist`,
  and `build` paths have no tracked diff.
- F-23: This closing report is present and covers the required sections.
- F-24: No CDC verification file was created.

## Silent-Drop Check

No requested slice behavior was dropped. The implemented substrate provides the
fixture, normalization, status, comparison, process-observation, safety-policy,
and optional Catch2 seams requested by the open set.

Non-goals stayed out of scope: no parser/grounder/engine adoption, no
black-box conformance suite migration, no release packaging change, no new
runtime dependency, and no CDC-owned verification file.

## Bubble-up to Arc04

Arc04 now has a concrete fixture and seam-test substrate that later slices can
use when they are ready to classify integration readiness and adopt shared
runtime behavior. Slice06 can proceed against the new typed APIs and the
existing no-binary-adoption boundary without requiring an arc-plan change.
