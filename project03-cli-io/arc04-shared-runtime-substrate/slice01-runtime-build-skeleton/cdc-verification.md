# CDC Verification: runtime-build-skeleton

Verifier: CDC
Branch: `release/0.3.x`
Verified on: 2026-08-09
Verified commit: `2732865d78b5a4dc7330eac7f4ce1164627a3bc2`
CC closing report:
[`closing-report.md`](closing-report.md)

## Verdict

Accepted. Slice01 is CDC-verified.

The committed slice creates the planned chengdu-owned runtime skeleton at
`pandapi-runtime/`, outside inherited planner source, with a C++17 static
library target, alias target `pandapi::runtime`, self-contained public header,
implementation source, plain CTest smoke test, and executable
`scripts/build-runtime.sh` entrypoint.

The slice stays inside its intended boundary: no parser, grounder, engine,
workflow, release asset, manifest, or historical pin files changed; no
third-party dependency is vendored, fetched, discovered, or included by the new
runtime source/build surfaces; and existing parser/grounder/engine build plus
negative-smoke behavior still pass.

## Commit and Scope Check

- `git status --short --branch`: clean on `release/0.3.x` before CDC docs.
- `git show --name-status --format=full 2732865d78b5a4dc7330eac7f4ce1164627a3bc2`:
  commit includes the required `Codex` and `Billo AI` co-author trailers.
- Commit files are limited to the slice close set, `pandapi-runtime/`, and
  `scripts/build-runtime.sh`.
- `git diff --check 2732865d78b5a4dc7330eac7f4ce1164627a3bc2^..2732865d78b5a4dc7330eac7f4ce1164627a3bc2`:
  passed with no output.
- `git diff --name-only 2732865d78b5a4dc7330eac7f4ce1164627a3bc2^..2732865d78b5a4dc7330eac7f4ce1164627a3bc2 -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release`:
  produced no output.

## Independent Verification

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | accepted | `pandapi-runtime/README.md` exists and names scope, non-goals, Arc03 inputs, Arc02 dependency gates, source layout, no behavior change, `pandapi::runtime`, future status/result, diagnostics/process, CLI parser, `PANDAPI_STATUS`, TTY/color, finalization, provenance, fixture harness, and dependency gates. |
| F-2 | accepted | `pandapi-runtime/include/pandapi/runtime`, `src`, `tests`, and `cmake` all exist outside `pandaPI/`. |
| F-3 | accepted | `pandapi-runtime/CMakeLists.txt` defines `project(pandapi_runtime ...)`, static `pandapi_runtime`, alias `pandapi::runtime`, public include directories, C++17 properties, `pandapi_runtime_smoke`, `enable_testing`, and `add_test`. |
| F-4 | accepted | Header/source/test exist; the public header is self-contained with `#pragma once`, `<string>`, and declarations under `namespace pandapi::runtime`. |
| F-5 | accepted | `scripts/build-runtime.sh` is executable and configures/builds `pandapi-runtime` under `build/runtime/$PLATFORM` with CMake Release, then runs CTest. |
| F-6 | accepted | `./scripts/build-runtime.sh` passed on `macos-arm64`; CTest reported 1/1 `pandapi_runtime_smoke` passed. |
| F-7 | accepted | The corrected import-surface check found no `FetchContent`, `ExternalProject`, approved future dependency `find_package`, direct fmt/CLI11/Catch2 includes, `expected.hpp`, or `reproc` usage in runtime source/build surfaces or `scripts/build-runtime.sh`. |
| F-8 | accepted | Public headers contain no global `using namespace`, mutable namespace-scope object definition, raw pointer ownership declaration, `new`, `delete`, `malloc`, or `free`. |
| F-9 | accepted | Generated runtime build outputs remain outside tracked source status; no runtime build artifacts are staged under `pandapi-runtime/`. |
| F-10 | accepted | `./scripts/build-parser.sh && ./scripts/build-grounder.sh && ./scripts/build-engine.sh` passed on `macos-arm64`; inherited planner warnings only. |
| F-11 | accepted | `./scripts/smoke-test.sh --negative` passed on `macos-arm64`: 4 passed, 0 failed. |
| F-12 | accepted | `git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release` produced no output after verification. |

Additional close-set checks:

- Ledger row count: `12`.
- Open ledger row check: no `| open |` rows remain.
- CC `closing-report.md` exists and includes capability verdict,
  implementation decisions, verification, ledger walk, silent-drop check,
  and Arc04 bubble-up.

## F-7 Ledger Correction Review

CDC accepts CC's F-7 verify-command correction. The open ledger required README
documentation to name the Arc02 dependency gates, while the original dependency
ban also scanned the whole runtime tree. That made README compliance conflict
with the no-import criterion. The closed check now scans the actual
source/build/import surfaces plus `scripts/build-runtime.sh`, preserving the
substantive gate: dependency names may appear in planning prose, but no
third-party code, package discovery, or direct include enters slice01.

## Bubble-up

Arc04 can proceed to slice02 from this accepted foundation. The runtime root,
namespace, build target, CTest hook, and build script are now stable enough for
the status/result substrate to use.

Carry these constraints forward:

- Keep all third-party APIs behind local `pandapi::runtime` facades or
  test-only targets.
- Do not wire parser, grounder, or engine behavior to runtime helpers until a
  slice explicitly owns and tests that boundary.
- Preserve release and wolong fetch/install path shape until Arc06 proves any
  release packaging changes.
