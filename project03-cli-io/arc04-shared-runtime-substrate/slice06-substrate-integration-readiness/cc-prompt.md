# CC Prompt: Arc04 Slice06 - substrate-integration-readiness

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
4. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/slice-doc.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/ledger.md`
6. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md`
7. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md`
8. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md`
9. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/cdc-verification.md`
10. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice01-runtime-build-skeleton/cdc-verification.md`
11. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
12. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
13. `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
14. `pandapi-runtime/README.md`
15. `pandapi-runtime/CMakeLists.txt`
16. `scripts/build-runtime.sh`
17. `scripts/build-parser.sh`
18. `scripts/build-grounder.sh`
19. `scripts/build-engine.sh`
20. `scripts/smoke-test.sh`

Also load the C++ guideline substrate for any touched C++ or build-facing
code:

- `cpp-guidelines` `11-anti-patterns.md`
- `02-api-design.md`
- `03-functions.md`
- `05-resource-management.md`
- `06-error-handling.md`
- `10-expressions-and-statements.md`
- `12-project-structure-and-tooling.md`
- `13-standard-library.md`

## Mission

Implement Arc04 Slice06: substrate integration readiness.

This is the final Arc04 implementation slice before arc-level close. Your job
is to turn the accepted runtime substrate into a durable Arc05/Arc06 handoff:

- write `docs/design-v0.3.0/arc04-shared-runtime-substrate/substrate-integration-readiness.md`;
- inventory what Arc04 actually delivered;
- map runtime facilities back to Arc03 and forward to Arc05;
- map Arc02 dependency decisions to the actual post-Arc04 state;
- add a build-only all-build probe to make runtime plus inherited binary
  builds a single reproducible local gate;
- update `pandapi-runtime/README.md` if useful for final Arc04 readiness;
- close the ledger with evidence and write the CC closing report.

This slice is still inert. It is not parser, grounder, or engine adoption.

## Hard Boundaries

- No edits under `pandaPI/`.
- No adoption of `pandapi::runtime` by parser, grounder, or engine binaries.
- No `pandapi-*` wrapper, symlink, copied binary, renamed binary, release
  entry point, package change, or inherited-name migration work.
- No parser, grounder, or engine managed-process conformance fixtures beyond
  existing build/smoke gates. Arc05 owns executable contract fixtures.
- No CLI11 import, CLI parser migration, or `gengetopt` replacement.
- No fmt, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL,
  google/benchmark, SAT/BDD/CUDD, H2/cpddl dependency-internal work, optional
  dependency audit, or parser-generator work.
- No `FetchContent`, `ExternalProject`, package-manager bootstrap, `curl`,
  `git clone`, or network download in the default build.
- No workflow, top-level README, release asset, manifest, `vendor.env`,
  `pins.env`, `dist`, `build`, or `release` changes.
- Do not touch `scripts/package-release.sh` or `scripts/publish-release.sh`.
- Do not create Arc04 `closing-report.md`; arc close is CDC/next-step work
  after this slice is CDC-verified.
- Do not create `cdc-verification.md`; CDC owns that file.

If you discover that Arc04 cannot hand off cleanly without widening these
boundaries, stop and record an amendment request in the closing report rather
than silently implementing Arc05 early.

## Implementation Guidance

Write the readiness report as the durable bridge between Arc04 and the next
two arcs. It should be specific enough that Arc05 can be planned without
re-reading every slice, but it must not pretend that Arc04 already proved
executable conformance.

Required report sections, names may vary slightly:

- Executive summary and verdict for Arc04 readiness.
- Evidence inputs: Arc02, Arc03, and Arc04 slices 01-05.
- Runtime surface inventory: files, headers, CMake target, tests, README, and
  build entrypoints.
- Arc03 coverage map: status/result, diagnostics/status I/O, stdout/stderr,
  `PANDAPI_STATUS`, CLI/TTY/provenance, fixtures, normalization, process
  observation.
- No-binary-adoption proof and protected-surface proof.
- Arc02 dependency gate state: adopted, optional/test-only, pilot-only, held,
  rejected.
- Arc05 adoption map for parser, grounder, and engine.
- Arc05 fixture/conformance plan, including positive, negative, status,
  stream, color, signal, timeout/resource, provenance, and non-supported
  surface probes.
- Slice05 stdin/full-duplex caveat and re-entry condition.
- Arc06 release/wolong/license/NOTICE/test-only-exclusion/CI handoff.
- Open risks and amendment triggers.
- Whether Arc04 is ready for arc-level CDC close after Slice06 CDC
  verification.

For the build probe, prefer a tiny `scripts/build-all.sh` with this shape:

```bash
#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

"$SCRIPT_DIR/build-runtime.sh"
"$SCRIPT_DIR/build-parser.sh"
"$SCRIPT_DIR/build-grounder.sh"
"$SCRIPT_DIR/build-engine.sh"
```

Do not duplicate platform detection. Let the existing build scripts own their
current platform/build behavior. Do not run packaging, publishing, smoke
tests, install steps, copies, symlinks, or renames from this helper.

## Required Verification

Run the ledger commands and record the commit SHA plus outputs in
`ledger.md`. At minimum, run:

```bash
./scripts/build-all.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
git diff --name-only -- pandaPI
git diff --name-only -- .github README.md release-manifest.txt vendor.env pins.env dist build release scripts/package-release.sh scripts/publish-release.sh
```

Also verify:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/cdc-verification.md
```

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all 24 rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc04, including whether Arc04 is ready for arc-level CDC close
  after independent Slice06 verification.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
