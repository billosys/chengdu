# CDC Verification - Arc05 Slice02 - contract-fixture-scaffold

Verifier: CDC
Verified on: 2026-08-10
Implementation commit reviewed: `836fa434`
Repair commit reviewed: `f74a7f7a`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc05 Slice02 is CDC-verified after the narrow F-2 documentation repair.

The slice delivered a repo-level black-box contract fixture scaffold for the
current inherited parser, grounder, engine, and composed pipeline behavior. It
records baseline observations without claiming Arc03 managed-process
conformance, keeps the runner dependency-free, and routes parser adoption to
Slice03.

CDC initially found one blocker: ledger row F-2 claimed the fixture README
documented a comparison policy, but the README did not contain an explicit
comparison-policy section or literal `comparison` evidence. CC corrected that
with the smallest document-only repair in `f74a7f7a`: a `## Comparison Policy`
section in `fixtures/contract/README.md` and a matching F-2 evidence note.

## Source and Scope Checks

- `git show --format=fuller --name-only --no-renames 836fa434` confirmed the
  implementation touched the fixture runner, fixture docs, reusable CI, and
  Slice02 design artifacts.
- `git diff-tree --no-commit-id --name-only -r 836fa434` confirmed no touched
  paths under `pandaPI/` and no release, package, publish, or wolong-facing
  release-shape paths.
- Commit-range scans over `452a6e07..836fa434` found no prohibited dependency
  import, network fetch, vendored package, canonical `pandapi-*` entrypoint,
  or release packaging change.
- `f74a7f7a` was limited to `fixtures/contract/README.md` and the Slice02
  ledger F-2 evidence text.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Slice open set contains scaffold scope, baseline boundary, no-binary-adoption boundary, fixture runner, CI, full-duplex caveat, and downstream handoff. |
| F-2 | reproduced after repair | Exact F-2 command passed after `fixtures/contract/README.md` added `## Comparison Policy`. |
| F-3 | reproduced | Fixture records contain component, surface disposition, argv/stdin/stdout/stderr/artifact/exit/final-status/normalization/safety fields. |
| F-4 | reproduced | Baseline records and future contract-target records are labeled separately. |
| F-5 | reproduced | Runner is executable, `bash -n` clean, listable/selectable, and emits labeled PASS/FAIL output. |
| F-6 | reproduced | Runner captures stdout, stderr, exit status, and artifact state separately. |
| F-7 | reproduced | Runner uses isolated temp directories, cleanup traps, and explicit preserve-for-debug mode. |
| F-8 | reproduced | Parser baseline passed: 16 checks, 0 failed. |
| F-9 | reproduced | Grounder baseline passed: 7 checks, 0 failed. |
| F-10 | reproduced | Engine baseline passed: 8 checks, 0 failed. |
| F-11 | reproduced | Pipeline baseline passed: 7 checks, 0 failed. |
| F-12 | reproduced | Baseline output makes current final `PANDAPI_STATUS` absence explicit. |
| F-13 | reproduced | README and closing report preserve the black-box fixture versus Catch2/runtime seam-test distinction. |
| F-14 | reproduced | Diff scans found no dependency, network fetch, vendored package, or held/rejected dependency import. |
| F-15 | reproduced | Diff scans found no canonical `pandapi-*` executable, wrapper, symlink, copied binary, release entrypoint, or migration path. |
| F-16 | reproduced | Commit-scoped checks found no `pandaPI/` source edits. |
| F-17 | reproduced | Format, runtime, sanitizer, full build, positive smoke, and negative smoke gates passed. |
| F-18 | reproduced | Reusable Linux and macOS build jobs run `./scripts/run-contract-fixtures.sh --baseline`. |
| F-19 | reproduced | `actionlint -color` passed locally. |
| F-20 | reproduced | Unstaged and staged whitespace checks passed. |
| F-21 | reproduced | Closing report contains the required closeout sections. |
| F-22 | reproduced | CC did not create CDC's verification file. |

## Commands Reproduced

```text
./scripts/check-format-owned.sh
./scripts/build-runtime.sh
./scripts/sanitize-runtime.sh
./scripts/build-all.sh
./scripts/run-contract-fixtures.sh --baseline --component parser
./scripts/run-contract-fixtures.sh --baseline --component grounder
./scripts/run-contract-fixtures.sh --baseline --component engine
./scripts/run-contract-fixtures.sh --baseline --component pipeline
./scripts/run-contract-fixtures.sh --baseline
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
actionlint -color
git diff --check
git diff --cached --check
```

Observed results:

- Format gate: 25 owned runtime files checked.
- Runtime build: macos-arm64 CTest passed 5/5.
- Sanitizer build: macos-arm64 ASan/UBSan CTest passed 5/5.
- Contract fixtures: parser 16/0, grounder 7/0, engine 8/0, pipeline 7/0;
  full baseline 38 passed, 0 failed.
- Positive smoke: 5 passed, 0 failed.
- Negative smoke: 4 passed, 0 failed.
- `actionlint -color`: passed.
- Whitespace checks: unstaged and staged checks passed.
- Exact repaired F-2 command: passed.

## Code Review Notes

No blocking findings remain.

The fixture runner is appropriately black-box: it executes built binaries and
observes process outputs, exit status, and generated artifacts. The current
fixture records are explicit that inherited baseline behavior does not equal
final managed-process contract conformance.

The runner deliberately avoids dependencies and network fetches. It does not
touch parser, grounder, or engine source and does not introduce canonical
`pandapi-*` entry points. Those behavior changes stay with the per-binary
adoption slices.

The reusable workflow now gates inherited builds with the baseline fixture
runner on Linux and macOS, which is the right runway for later parser,
grounder, and engine contract flips.

## Bubble-up to Arc05

Slice03 `parser-contract-adoption` is ready to open from a concrete baseline
fixture home. Carry forward:

- Flip parser expectations deliberately from inherited baseline observations
  toward managed-process contract records.
- Preserve both canonical and inherited command proof when parser adoption
  creates the first `pandapi-*` surface.
- Replace prose-sensitive checks with stable status fields, stream ownership
  assertions, and final `PANDAPI_STATUS` records where the parser contract
  requires them.
- Keep stdin/full-duplex hardening visible as a later fixture-runner concern;
  Slice02 did not need it for empty-stdin baseline probes.
