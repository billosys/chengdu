# CDC Verification: Arc07 Slice05 engine-first-party-quality-burndown

Date: 2026-08-12

Verified commit:
`97e5c1829b04c7f27993f57a7267134a9f8f9bc6`

CDC verdict: verified closed.

Slice05 successfully converts the Slice04 engine report-backed scaffold into
Make-backed engine component gates. The engine targets are executable, source
class scoped, and documented. `static-analysis-engine` and
`format-check-engine` are accepted as reported baselines for this slice, not
as strict zero-finding or zero-drift release gates.

## Scope Review

The reviewed commit changes only Arc07 documentation, Make/source-quality
tooling, and engine source/test/build files:

- `docs/design-v0.3.0/arc07-source-quality-expansion/...`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `tools/source-quality/surface`
- `pandaPI/engine/src/CMakeLists.txt`
- `pandaPI/engine/src/tests/visited_list_payload_test.cpp`
- `pandaPI/engine/src/visited_list.cpp`
- `pandaPI/engine/src/visited_list.h`

CDC confirmed no `.github/`, README, release tooling, Arc08, Arc09, parser,
or grounder surfaces changed. CDC also found no commit diff that introduced a
global warning policy, coverage floor, public behavior expansion, dependency
expansion, wolong migration change, or optional/fenced engine support.

## Reproduced Evidence

CDC reproduced the engine target family:

- `make compile-db-engine`
- `make format-check-engine`
- `make static-analysis-engine`
- `make test-unit-engine`
- `make coverage-engine`
- `make warning-inventory-engine`
- `make sanitize-engine`

Observed engine evidence:

- `format-check-engine` recorded 126 baseline drift entries in
  `build/source-quality/macos-arm64/format-check-engine-drift.txt`.
- `static-analysis-engine` recorded clang-tidy exit status 1 and 98 reported
  finding lines in `build/static-analysis/engine/macos-arm64/clang-tidy.txt`.
- `test-unit-engine` ran the `pandapi_engine_visited_list_payload` CTest seam
  and passed 1/1.
- `coverage-engine` recorded a measured baseline with `visited_list.h` at
  100.00% line coverage and total line coverage at 66.67%; no release floor
  was set.
- `warning-inventory-engine` reported `visited_list payload representation
  status: closed` and `duplicate link-library status: not observed`.
- CDC's negative greps found no residual old `visited_list` integer-to-pointer
  payload warning and no duplicate link-library diagnostic in the engine
  warning logs.
- `sanitize-engine` ran the visited-list seam under ASan/UBSan. LSan remains a
  macOS skip with Linux/toolchain re-entry, and TSan remains re-entry-only
  because this slice has no accepted engine concurrency workload.

CDC reproduced the public behavior and source-quality runway gates:

- `make build-engine`
- `make test-contract-engine-managed`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-first-party`
- `make warning-inventory-first-party`

Observed broader evidence:

- Managed engine fixtures reported 312 passed, 0 failed.
- First-party warning inventory reported 54 warning lines.
- The source-quality surface, profile map, naming check, and first-party
  compile database gates passed.

CDC reproduced the regression chain:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Observed regression evidence:

- Runtime tests passed: 5/0.
- Baseline fixtures passed: 38/0.
- Managed parser fixtures passed: 298/0.
- Managed grounder fixtures passed: 269/0.
- Managed engine fixtures passed: 312/0.
- Managed pipeline fixtures passed: 129/0.
- Positive smoke passed: 3/0.
- Negative smoke passed: 4/0.
- Provenance check passed for all three components on `macos-arm64`.
- Whitespace checks passed.

`make check` was not run as the aggregate because the project has a known
macOS aggregate provenance mutation/noise path. CDC reproduced the exact
constituent gates required by F-12 instead.

## Ledger Verification

| Row | CDC status | Evidence |
|-----|------------|----------|
| F-1 | verified | `make help` lists all six engine targets; Make/tool wiring grep passed. |
| F-2 | verified | `make compile-db-engine` passed; selected first-party engine source and source-class documentation were present. |
| F-3 | verified | `make format-check-engine` passed and recorded 126 baseline drift entries without rewriting source. |
| F-4 | verified | `make static-analysis-engine` completed as a reported clang-tidy baseline with status 1 and 98 finding lines; no blanket suppression was added. |
| F-5 | verified | `make test-unit-engine` passed the visited-list payload representation CTest seam. |
| F-6 | verified | `make warning-inventory-engine` passed; the old visited-list payload-cast warning was absent. |
| F-7 | verified | `make warning-inventory-engine` passed; duplicate link-library noise was not observed. |
| F-8 | verified | `make coverage-engine` passed and recorded a measured baseline with no release floor. |
| F-9 | verified | `make sanitize-engine` passed under ASan/UBSan and preserved LSan/TSan re-entry language. |
| F-10 | verified | `make build-engine` and `make test-contract-engine-managed` passed with 312/0 managed engine fixtures. |
| F-11 | verified | Source-quality runway passed; first-party warning inventory reported 54 warning lines. |
| F-12 | verified | Safety, actionlint, static-analysis, format, test, provenance, and whitespace gates passed as constituent commands. |
| F-13 | verified | Boundary checks found no protected-scope changes or global policy expansion. |
| F-14 | verified | Closing report walks F-1 through F-14 and includes Arc07 bubble-up. |

## C++ Review Notes

CDC inspected the `visited_list` change. The legacy trie API still stores an
opaque `void*` payload, so this slice narrows the pointer/integer
representation behind `visited_list_payload::encode_cost`,
`decode_cost`, and `known_marker` helpers and covers the representation with a
CTest seam. This is acceptable for the slice boundary because it removes the
direct warning-bearing casts from call sites without expanding the inherited
data-structure rewrite.

The CMake link cleanup is also acceptable: redundant direct links to
`heuristics` and `hrc` were removed, while transitive linkage through `hlm`
preserves the build and the duplicate link warning is no longer observed.

## Bubble-up

Arc07 may mark Slice05 closed and CDC-verified.

Recommended next work remains parser generated-warning/root-cause analysis.
That slice should preserve the Arc07 generated-warning policy: identify why
each warning is emitted, whether the generated code is semantically correct,
which generator/template/skeleton owns the emitted code, and whether the fix
belongs in a Chengdu-owned generator/template, input/options, a compiler-flag
boundary, or a documented third-party exclusion.

Engine re-entry is not blocked, but should be explicit: stricter
`static-analysis-engine` selection, additional engine internals under CTest or
Catch2, or any newly observed engine warning should be opened under a later
ledger.
