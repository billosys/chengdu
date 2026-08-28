# Arc05 Slice04 Closing Report: grounder-contract-adoption

Date: 2026-08-10

## Capability Verdict

Closed for CC implementation.

Supported normal parser-generated `.htn` grounding now conforms to the
managed-process contract through canonical `pandapi-grounder` and inherited
`pandaPIgrounder` managed invocations. The inherited raw grounder remains
available as `pandaPIgrounder.legacy`, and inherited positional
`pandaPIgrounder INPUT.htn OUT.sas` compatibility remains available with no
default deprecation warning for 0.3.0 compatibility.

Parser contract behavior remains accepted. Engine and pipeline behavior remain
baseline-only until Slice05.

## Implementation Decisions

- Implemented a chengdu-owned Bash adapter,
  `scripts/pandapi-grounder-adapter.sh`, around the inherited grounder binary
  instead of editing vendored grounder source or importing a CLI dependency.
- Added `scripts/install-grounder-adapter.sh` so build paths can wrap a raw
  `pandaPIgrounder` as `pandaPIgrounder.legacy`, then install both
  `pandapi-grounder` and compatibility `pandaPIgrounder` entry points.
- Updated the make-owned `build-grounder` path in `mk/build.mk` so grounder
  builds produce `pandapi-grounder`, managed `pandaPIgrounder`, and raw
  `pandaPIgrounder.legacy`.
- Preserved inherited raw behavior for `pandaPIgrounder` invocations that use no
  managed/common/fenced options. Managed inherited invocation accepts
  `pandaPIgrounder --status=stderr INPUT.htn OUT.sas`.
- Emitted final status as tagged text, not JSON:
  `PANDAPI_STATUS	status=...	component=grounder	...`.
- Classified grounder outcomes as `ok` / 0, `domain_no_plan` / 2 where
  reachable in future accepted semantics, `cli_usage_error` / 10,
  `input_unavailable` / 20, `output_unavailable` / 21, `input_invalid` / 22,
  `legacy_surface` / 31, `experimental_surface` / 32, and `internal_error` /
  60.
- Wrote file-backed output through inherited quiet grounding to a temporary
  artifact, then copied and moved the complete artifact into place only after
  success.
- Kept stdout artifact output free of progress, statistics, ANSI, final
  `PANDAPI_STATUS`, and human diagnostic prose.
- Fenced visible H2 options as `experimental_surface` / 32.
- Fenced visible `cpddl`/FAM and non-normal inherited output/helper options as
  `legacy_surface` / 31.
- Added checked grounder fixture inputs under `fixtures/grounder/` and grounder
  fixture records under `fixtures/contract/grounder-contract-records.md`.
- Extended `scripts/run-contract-fixtures.sh` with selectable grounder contract
  cases while preserving baseline and accepted parser contract modes.
- Added `test-contract-grounder-managed` and wired it into `make test`, so
  reusable `make ci-linux` and `make ci-macos` entrypoints reach the grounder
  contract gate through make.

## Verification

All required local gates passed on macos-arm64:

- `make build-grounder`
  - final result: built `pandapi-grounder`, `pandaPIgrounder`, and
    `pandaPIgrounder.legacy`
- `./scripts/run-contract-fixtures.sh --contract --component grounder`
  - final result: 272 passed, 0 failed
- `make test-contract-parser-managed`
  - final result: parser contract 300 passed, 0 failed
- `./scripts/run-contract-fixtures.sh --baseline`
  - final result: 38 passed, 0 failed
- `make format-check`
  - final result: OK
- `make test-runtime-sanitize`
  - final result: 5/5 sanitizer runtime tests passed
- `make test`
  - final result: exit 0; includes baseline, managed parser contract, managed
    grounder contract, positive smoke, and negative smoke
- `make provenance-check`
  - final result: passed
- `make actionlint`
  - final result: passed
- `git diff --check`
  - final result: passed
- `git diff --cached --check`
  - final result: passed

Focused ledger commands also passed for slice-open documentation, grounder
fixture records, fixture runner selection, make-targeted CI, H2 fencing,
`cpddl`/FAM fencing, `domain_no_plan` assessment, dependency boundaries, and
protected file boundaries.

The build emits inherited third-party warning diagnostics from vendored
grounder dependencies on macOS. Those warnings are pre-existing inherited source
warnings and are not chengdu-owned adapter or contract failures.

## Ledger Walk

- F-1 closed: slice open set documents grounder-only scope, command identity,
  H2 and `cpddl`/FAM fencing, parser regression, engine handoff, and make CI.
- F-2 closed: `make build-grounder` produces canonical, inherited managed, and
  raw legacy grounder executables.
- F-3 closed: grounder fixture records cover command identity, output modes,
  status fields, normalization, comparison, and safety.
- F-4 closed: fixture runner supports baseline, parser contract, and grounder
  contract selection.
- F-5 closed: canonical file-backed grounder success conforms.
- F-6 closed: canonical stdout-backed grounder success conforms without stdout
  contamination.
- F-7 closed: inherited managed grounder compatibility conforms, and raw legacy
  positional passthrough remains available with no default deprecation warning.
- F-8 closed: help, version, and provenance informational commands conform.
- F-9 closed: CLI usage errors map to `cli_usage_error` / 10.
- F-10 closed: missing `.htn` input maps to `input_unavailable` / 20.
- F-11 closed: malformed `.htn` input maps to `input_invalid` / 22.
- F-12 closed: unavailable output paths map to `output_unavailable` / 21.
- F-13 closed: final `PANDAPI_STATUS` records are emitted exactly once with
  required fields.
- F-14 closed: status stream legality is enforced.
- F-15 closed: color/ANSI policy is enforced for machine-owned streams.
- F-16 closed: visible H2 options are fenced as `experimental_surface` / 32.
- F-17 closed: visible `cpddl`/FAM paths are fenced as `legacy_surface` / 31.
- F-18 closed: `domain_no_plan` was assessed and is not reachable through
  accepted grounder-only semantics in this slice. Re-entry condition: add an
  executable fixture only if a later accepted grounder behavior can prove no
  usable grounded model or reachable goal without relying on engine search.
- F-19 closed: accepted parser contract gate remains green.
- F-20 closed: existing baseline fixture gate remains green.
- F-21 closed: grounder contract adoption is represented through make-targeted
  CI; workflows continue to use make entrypoints.
- F-22 closed: format, sanitizer runtime, aggregate test, smoke, and provenance
  gates pass.
- F-23 closed: local `make actionlint` passed.
- F-24 closed: no dependency import, network fetch, package bootstrap, or
  third-party API leakage was added.
- F-25 closed: protected parser, engine, release, package, publish, checksum,
  manifest, wolong-facing, Arc05 close, and CDC boundaries remained untouched.
- F-26 closed: unstaged and staged whitespace checks passed.
- F-27 closed: this closing report provides the required closure sections.

## Silent-Drop Check

Delivered scope matches the slice scope:

- canonical and inherited grounder commands are present;
- supported normal `.htn` grounding covers file-backed and stdout-backed `.sas`
  artifacts;
- positive and negative grounder outcomes have executable black-box fixtures;
- status/exit mapping, stream ownership, artifact disposition, and ANSI policy
  are fixture-checked;
- H2 is fenced as experimental, not promoted;
- `cpddl`/FAM and non-normal inherited paths are fenced as legacy, not promoted;
- inherited raw grounder behavior remains available as `pandaPIgrounder.legacy`;
- parser contract and baseline gates remain green;
- engine contract adoption remains unopened for Slice05.

Explicitly not delivered, per scope:

- no engine contract adoption;
- no parser or engine source edits;
- no release package shape change;
- no wolong fetch/install/migration change;
- no package/publish/release asset/checksum/manifest change;
- no new third-party CLI or process dependency;
- no Arc05 close report and no CDC verification file.

## Bubble-up to Arc05

Arc05 Slice04 is ready for independent CDC verification.

Slice05 engine-contract-adoption is ready to open after CDC accepts this slice.
The important handoff is that `scripts/run-contract-fixtures.sh` now supports
baseline, parser contract, and grounder contract modes; Slice05 should extend
the same fixture runner and make-target pattern for engine without changing
accepted parser or grounder behavior or release packaging.
