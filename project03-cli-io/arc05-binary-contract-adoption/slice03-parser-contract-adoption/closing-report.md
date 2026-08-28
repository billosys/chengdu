# Arc05 Slice03 Closing Report: parser-contract-adoption

Date: 2026-08-10

## Capability Verdict

Closed for CC implementation.

Supported normal HDDL parser execution now conforms to the managed-process
contract through canonical `pandapi-parser` and inherited `pandaPIparser`
managed invocations. The inherited `pandaPIparser -C ...` legacy path remains
available with no default deprecation warning for 0.3.0 compatibility.

Grounder, engine, and pipeline behavior remain baseline-only.

## Implementation Decisions

- Implemented a chengdu-owned Bash adapter, `scripts/pandapi-parser-adapter.sh`,
  around the inherited parser binary instead of editing vendored parser source
  or importing a CLI dependency.
- Added `scripts/install-parser-adapter.sh` so existing build paths can wrap a
  built raw parser as `pandaPIparser.legacy`, then install both
  `pandapi-parser` and compatibility `pandaPIparser` entry points.
- Kept `scripts/build-parser.sh` as the direct parser build gate and made it
  install the adapter after compiling the inherited parser.
- Preserved inherited positional compatibility: managed `pandaPIparser` accepts
  `DOMAIN PROBLEM OUT.htn`, while raw legacy `pandaPIparser -C ...` passthrough
  remains available when no managed option is used.
- Emitted final status as tagged text, not JSON:
  `PANDAPI_STATUS	status=...	component=parser	...`.
- Classified normal parser outcomes as `ok` / 0, `cli_usage_error` / 10,
  `input_unavailable` / 20, `output_unavailable` / 21, `input_invalid` / 22,
  `legacy_surface` / 31, and `internal_error` / 60.
- Added a narrow include preflight for declared `(:include NAME)` references so
  missing HDDL includes classify as `input_unavailable` before the inherited
  parser turns them into parse errors.
- Wrote file-backed output via temporary copy plus final move, and discarded
  failed/partial outputs for managed failure classifications.
- Fenced visible parser verifier and output-helper options as
  `legacy_surface` / 31. Parser verifier and output helper behavior is not
  supported product behavior in this slice.
- Added parser contract fixture records and runner cases while keeping the
  existing baseline fixture mode intact.
- Initial CC implementation added direct workflow parser-contract coverage;
  follow-up commit `1d91a1ae` routed that coverage through `make ci-linux` /
  `make ci-macos` via `test-contract-parser-managed`, preserving the same
  parser contract gate while satisfying the make-target CI convention.

## Verification

All required local gates passed on macos-arm64:

- `./scripts/build-parser.sh`
- `./scripts/run-contract-fixtures.sh --contract --component parser`
  - final result: 300 passed, 0 failed
- `./scripts/run-contract-fixtures.sh --baseline`
  - final result: 38 passed, 0 failed
- `./scripts/check-format-owned.sh`
  - final result: OK, 25 files
- `./scripts/build-runtime.sh`
  - final result: 5/5 runtime tests passed
- `./scripts/sanitize-runtime.sh`
  - final result: 5/5 sanitizer runtime tests passed
- `./scripts/build-all.sh`
  - final result: exit 0
- `./scripts/smoke-test.sh`
  - final result: 5 passed, 0 failed
- `./scripts/smoke-test.sh --negative`
  - final result: 4 passed, 0 failed
- `actionlint -color`
  - final result: passed with no diagnostics
- `git diff --check`
  - final result: passed

Protected-boundary checks passed:

- no diff under `pandaPI/pandaPIgrounder` or `pandaPI/pandaPIengine`;
- no diff under release assets, manifests, checksums, package, or publish
  scripts;
- no Arc05-wide `closing-report.md` was created;
- no Slice03 `cdc-verification.md` was created.

Dependency gate passed with no new network fetch, vendored package,
package-manager bootstrap, `FetchContent`, `ExternalProject`, `curl`,
`git clone`, held/rejected dependency import, or third-party API leakage into
planner internals.

## Ledger Walk

- F-1 closed: slice open set documented parser-only scope and downstream
  grounder handoff.
- F-2 closed: parser build produces `dist/<platform>/pandapi-parser` and
  `dist/<platform>/pandaPIparser`.
- F-3 closed: parser contract records cover command identity, output modes,
  status fields, normalization, and safety.
- F-4 closed: fixture runner supports baseline and parser contract selection.
- F-5 closed: canonical file-backed parser success conforms.
- F-6 closed: canonical stdout-backed parser success conforms without stdout
  contamination.
- F-7 closed: inherited parser managed compatibility conforms, and legacy
  positional passthrough remains.
- F-8 closed: help/version/provenance informational commands conform.
- F-9 closed: CLI usage errors map to `cli_usage_error` / 10.
- F-10 closed: missing domain, problem, and include inputs map to
  `input_unavailable` / 20.
- F-11 closed: malformed and semantically invalid HDDL map to
  `input_invalid` / 22.
- F-12 closed: unavailable output paths map to `output_unavailable` / 21.
- F-13 closed: final `PANDAPI_STATUS` records are emitted exactly once with
  required fields.
- F-14 closed: status stream legality is enforced.
- F-15 closed: color/ANSI policy is enforced for machine-owned streams.
- F-16 closed: parser verifier and output helper surfaces are fenced as
  `legacy_surface` / 31.
- F-17 closed: parser contract adoption is represented in reusable CI.
- F-18 closed: existing baseline fixture gate still passes.
- F-19 closed: runtime, full build, and smoke gates pass.
- F-20 closed: local `actionlint` passed.
- F-21 closed: dependency gate passed with no new dependency adoption.
- F-22 closed: protected boundaries remained untouched.
- F-23 closed: whitespace checks passed.
- F-24 closed: this closing report provides the required closure sections.

## Silent-Drop Check

Delivered scope matches the slice scope:

- canonical and inherited parser commands are present;
- supported normal parse paths cover file-backed and stdout-backed artifacts;
- parser positive and negative outcomes have executable black-box fixtures;
- status/exit mapping, stream ownership, artifact disposition, and ANSI policy
  are fixture-checked;
- missing domain, problem, and include inputs are classified distinctly from
  invalid HDDL;
- parser verifier and output helper paths are fenced rather than promoted;
- inherited `pandaPIparser -C ...` remains available with no default
  deprecation warning;
- baseline grounder, engine, and pipeline behavior remains intact.

Explicitly not delivered, per scope:

- no grounder or engine contract adoption;
- no release package shape change;
- no wolong fetch/install/migration change;
- no package/publish/release asset/checksum/manifest change;
- no new third-party CLI or process dependency;
- no Arc05 close report and no CDC verification file.

## Bubble-up to Arc05

Arc05 Slice03 is ready for independent CDC verification.

Slice04 grounder-contract-adoption is ready to open after CDC accepts this
slice. The important handoff is that `scripts/run-contract-fixtures.sh` now has
a baseline/contract split and parser-only contract selection; Slice04 should
extend that same contract mode for grounder instead of changing parser
behavior or release packaging.
