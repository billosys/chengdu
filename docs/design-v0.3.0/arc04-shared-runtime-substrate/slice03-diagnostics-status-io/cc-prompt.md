# CC Prompt: Arc04 Slice03 - diagnostics-status-io

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
4. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/slice-doc.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md`
6. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice02-status-result-core/cdc-verification.md`
7. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
8. `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
9. `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
10. `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

Also load the C++ guideline substrate for the touched rule families:

- `cpp-guidelines` `11-anti-patterns.md`
- `02-api-design.md`
- `06-error-handling.md`
- `09-classes-and-resource-management.md`
- `12-project-structure-and-tooling.md` if CMake/header layout questions arise
- `13-standard-library.md`

## Mission

Implement Arc04 Slice03: a tested `pandapi::runtime` diagnostics/status I/O
facade.

The deliverable is a C++17, standard-library-only runtime facility that turns
Arc03's accepted stdout/stderr/status contract into code:

- status record value type built from Slice02 `ProcessStatus`;
- status stream selection helpers for stdout/stderr ownership;
- tagged-text `PANDAPI_STATUS` writer;
- tagged-text parser returning the local `StatusResult` facade or equivalent;
- single-line value handling that prevents newline, tab, ANSI, and placeholder
  prose leaks;
- final-status flushing;
- partial-output policy vocabulary;
- seam tests proving write, parse, stream legality, invalid input, flushing,
  and partial-output behavior;
- README/CMake updates.

Keep this slice inert with respect to the inherited binaries. Do not adopt the
helpers inside parser, grounder, or engine yet.

## Hard Boundaries

- No edits under `pandaPI/`.
- No `pandapi-*` wrapper or inherited-name migration work.
- No CLI11 wrapper, TTY/color policy, provenance assembler, Catch2 import,
  reproc++ adapter, process fixture harness, JSON Lines event stream, or Arc05
  executable contract migration.
- No external dependency import, vendoring, fetching, `find_package`, or direct
  includes for fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json,
  Boost, Abseil, or GSL.
- Do not use fmt directly in this slice. Shape the facade so fmt can later sit
  behind it if a dedicated dependency-import gate opens.
- Do not expose a third-party type from a public runtime header.
- Do not touch workflow, top-level README, release asset, manifest,
  `vendor.env`, `pins.env`, `dist`, `build`, or `release` paths.
- Do not create `cdc-verification.md`; CDC owns that file.

If you discover fmt should enter in this slice, stop and record that as an
amendment request or bubble-up rather than importing it silently.

## Implementation Guidance

Prefer small, boring C++17 types:

- `enum class StatusStream { Stderr, Stdout }` or equivalent;
- `enum class PartialOutputPolicy` with absent, retained, discarded, complete,
  and unknown outcomes;
- a status record type that stores `ProcessStatus`, surface name, and optional
  fields without placeholder prose;
- `std::ostream&` for writer injection and testability;
- `std::string_view` for stable names where lifetime is static;
- `std::string` for serialized records;
- local escaping or validation helpers that keep records single-line;
- `noexcept` on pure mapping and legality checks;
- no global mutable stream state.

The implementation should make later Arc05 adoption easier, but it should not
try to solve future CLI, TTY/color, provenance, or process-fixture slices.

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
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md
```

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all 20 rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc04, including whether slice04 planning should change.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
