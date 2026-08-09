# CC Prompt: Arc04 Slice02 - status-result-core

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
4. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/slice-doc.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/ledger.md`
6. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice01-runtime-build-skeleton/cdc-verification.md`
7. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
8. `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
9. `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

Also load the C++ guideline substrate for the touched rule families:

- `cpp-guidelines` `11-anti-patterns.md`
- `02-api-design.md`
- `06-error-handling.md`
- `12-project-structure-and-tooling.md` if CMake/header layout questions arise

## Mission

Implement Arc04 Slice02: a tested `pandapi::runtime` status/result core.

The deliverable is a C++17, standard-library-only runtime facility that turns
Arc03's accepted process taxonomy into code:

- `ProcessStatus` or equivalent strongly typed status object;
- `StatusCode` or equivalent status enum containing every Arc03 status;
- typed status class, component, and surface-disposition vocabulary;
- stable status-name and numeric exit-code mapping;
- supervisor-observed `signal_terminated` support as `128 + signal_number`;
- local `StatusResult` facade for value-or-status returns;
- seam tests proving all accepted names, classes, and exit codes;
- README/CMake updates.

Keep this slice inert with respect to the inherited binaries. Do not adopt the
helpers inside parser, grounder, or engine yet.

## Hard Boundaries

- No edits under `pandaPI/`.
- No `pandapi-*` wrapper or inherited-name migration work.
- No stdout/stderr/status-line writer, diagnostics facade, TTY/color policy,
  CLI11 wrapper, fmt integration, Catch2 import, reproc++ adapter, provenance
  assembler, or process fixture harness.
- No external dependency import, vendoring, fetching, `find_package`, or direct
  includes for fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json,
  Boost, Abseil, or GSL.
- Do not expose a third-party type from a public runtime header.
- Do not touch workflow, top-level README, release asset, manifest,
  `vendor.env`, `pins.env`, `dist`, `build`, or `release` paths.
- Do not create `cdc-verification.md`; CDC owns that file.

If you discover that `tl::expected` should enter in this slice, stop and record
that as an amendment request/bubble-up rather than importing it silently.

## Implementation Guidance

Prefer small, boring C++17 types:

- `enum class` for status/status-class/component/surface disposition;
- `std::string_view` for stable names where lifetime is static;
- `int` for process exit codes at the process boundary;
- a small local `StatusResult<T>` facade rather than spreading optional/status
  pairs through call sites;
- explicit construction helpers for success/failure cases;
- `noexcept` on pure mapping functions.

The implementation should make later Arc05 adoption easier, but it should not
try to solve future stream, CLI, or process-fixture slices.

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
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/cdc-verification.md
```

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all 18 rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc04, including whether slice03 planning should change.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
