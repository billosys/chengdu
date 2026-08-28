# Slice 06 Closing Report: parser-native-contract-cutover

## Capability Verdict

Proposed done. `pandapi-parser` is now the canonical parser artifact for
0.3.0: it is a compiled native executable, not a Bash adapter; no
`pandaPIparser` or `pandaPIparser.legacy` artifact is installed under
`dist/<platform>/`; and the accepted parser managed-process contract passes.

This is parser-only. Grounder remains transitional through its existing shims,
and engine remains inherited until its owning adoption slice.

## Implementation Decisions

- Added a native parser entrypoint in
  `pandaPI/pandaPIparser/src/pandapi_parser_native.cpp` for managed option
  parsing, stream/status ownership, provenance, preflight, and stable
  status/exit mapping.
- Linked the native entrypoint with the inherited parser algorithm in-process
  via a hidden legacy-driver mode, without installing a parser sidecar command
  or keeping a parser Bash adapter.
- Updated `make build-parser` to depend on `build-runtime`, build
  `pandapi-parser`, remove parser old-name artifacts from `dist/<platform>/`,
  and record parser provenance under `pandapi-parser`.
- Deleted `scripts/pandapi-parser-adapter.sh` while leaving grounder transition
  scripts intact.
- Updated parser contract records, baseline records, smoke tests, contract
  tests, provenance checks, release packaging references, and README source
  build wording to reflect the parser-native state without claiming grounder or
  engine cutover.

## Verification

- `make build-parser` passed.
- Parser artifact checks passed on `macos-arm64`: `pandapi-parser` exists,
  `pandaPIparser` is absent, and `pandaPIparser.legacy` is absent.
- Native-binary check passed: `pandapi-parser` is not shell/text and has no
  shebang.
- `make test-contract-parser-managed` passed with `285 passed, 0 failed`.
- Parser info aliases passed: help/version/provenance each reproduced
  `parser-info-commands` with `15 passed, 0 failed`.
- Parser legacy-surface alias passed: `parser-legacy-surface-fencing` reported
  `18 passed, 0 failed`.
- `make smoke` passed with `3 passed, 0 failed`.
- `make smoke-negative` passed with `4 passed, 0 failed`.
- `make test` passed and ended with `Test suite passed`.
- Protected-boundary, dependency/network-fetch, active-surface cleanup, parser
  fixture cleanup, provenance, and whitespace ledger checks passed.

## Ledger Walk

- F-1 through F-3 prove the slice remained parser-only, installs only
  `pandapi-parser`, and produces a native binary instead of an adapter.
- F-4 and F-5 prove accepted parser managed-process options and legacy helper
  fencing.
- F-6 through F-9 prove parser shim retirement across active surfaces,
  fixtures, and provenance output.
- F-10 and F-11 prove parser contract and smoke behavior through Make,
  including the aggregate release-branch test gate.
- F-12 and F-13 prove protected boundaries and dependency gates were not
  crossed.
- F-14 proves whitespace hygiene.
- F-15 is this closing report.

## Silent-Drop Check

No parser managed-process behavior was dropped silently. The parser no longer
passes inherited helper modes through; those surfaces are fenced as
`legacy_surface` with managed status. Parser invalid inputs now return stable
`input_invalid` status/exit evidence and do not retain partial artifacts.

Grounder and engine behavior was not broadened or migrated. The remaining
transitional grounder shims are intentionally still present for their owning
slice.

## Bubble-up to Arc05

Slice06 closes the parser half of the Slice05 canonical-binary blocker. The
next Arc05 adoption work can treat parser command identity as native and
canonical while continuing to track grounder and engine cutover separately.
