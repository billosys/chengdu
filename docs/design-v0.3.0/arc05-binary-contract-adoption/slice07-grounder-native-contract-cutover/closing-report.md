# Arc05 Slice07 CC Closing Report

## Capability Verdict

Proposed done. `pandapi-grounder` is now the native canonical grounder artifact
for 0.3.0. It satisfies the accepted grounder managed-process contract without
installing `pandaPIgrounder`, `pandaPIgrounder.legacy`, or the grounder Bash
adapter scripts. Parser native behavior remains green, and engine behavior
remains inherited.

## Implementation Decisions

- Added `pandaPI/pandaPIgrounder/src/pandapi_grounder_native.cpp` as a compiled
  chengdu-owned contract entrypoint that reuses the inherited grounder algorithm
  behind a hidden in-process legacy driver.
- Built the inherited `main.cpp` as `pandaPIgrounder_legacy_main` and linked it
  with the native entrypoint plus `pandapi-runtime`.
- Routed `make build-grounder` through `build-runtime`, installed only
  `dist/<platform>/pandapi-grounder`, and removed stale old-name grounder
  artifacts from `dist/<platform>/`.
- Deleted `scripts/pandapi-grounder-adapter.sh` and
  `scripts/install-grounder-adapter.sh`; tests and tooling now call the native
  canonical command directly.
- Kept H2 as `experimental_surface` and cpddl/FAM/non-normal grounder options
  as `legacy_surface`; neither was promoted to supported normal grounding.
- Updated contract fixtures, baseline/pipeline records, smoke tests, provenance
  checks, release packaging helpers, and CI artifact smoke checks to use
  `pandapi-grounder`.

## Verification

- `make build-grounder` passed on `macos-arm64`.
- `dist/macos-arm64/pandapi-grounder` is executable and `file` reports
  `Mach-O 64-bit executable arm64`.
- `dist/macos-arm64/pandaPIgrounder` and
  `dist/macos-arm64/pandaPIgrounder.legacy` are absent.
- `make test-contract-grounder-managed` passed: `256 passed, 0 failed`.
- `make test-contract-parser-managed` passed: `285 passed, 0 failed`.
- `make smoke` passed: `3 passed, 0 failed`.
- `make smoke-negative` passed: `4 passed, 0 failed`.
- `make test` passed.

## Ledger Walk

- F-1 is satisfied by the open slice documents.
- F-2 and F-3 are satisfied by the native grounder build and artifact checks.
- F-4 and F-5 are satisfied by managed grounder contract fixtures.
- F-6 through F-9 are satisfied by removing the grounder shims/old artifacts
  and updating fixture/provenance records.
- F-10 is satisfied by the managed parser regression run.
- F-11 and F-12 are satisfied by smoke and aggregate test gates.
- F-13 and F-14 are satisfied by protected-boundary and dependency/network
  diff checks.
- F-15 is satisfied by whitespace checks.
- F-16 is satisfied by this closing report.

## Silent-Drop Check

No parser old-name artifacts were restored. No engine source or `pandapi-engine`
cutover was introduced. No Arc05 closing report, CDC verification file, release
publication output, checksum, or final manifest was created. No new dependency
fetch, package-manager bootstrap, or third-party planner API was added.

## Bubble-up to Arc05

Arc05 now has native managed-process adoption for parser and grounder. The
remaining binary adoption work is engine cutover and any later release/docs
publication acceptance owned by future slices or arcs.
