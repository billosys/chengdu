# CDC Verification - Arc05 Slice01 - quality-tooling-runway

Verifier: CDC
Verified on: 2026-08-10
Implementation commit reviewed: `a701b32e9802a171936c5d07ddc4fd75c8f9af13`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc05 Slice01 is CDC-verified.

The slice delivered the pre-adoption quality runway without parser, grounder,
engine, release, package, publish, or wolong-facing behavior changes. CC's
closing report is structurally complete, the 21 ledger rows reproduce, and the
new gates are scoped to chengdu-owned runtime source.

The current local head during this verification was `4e2a0356` (`Updated
ignores.`), after the Slice01 implementation commit. That later ignore change
was not part of the Slice01 implementation review.

## Source and Scope Checks

- `git show --format=fuller --name-only --no-renames a701b32e...` confirmed
  the implementation touched `.clang-format`, `build-reusable.yml`,
  `pandapi-runtime/`, `scripts/build-runtime.sh`,
  `scripts/check-format-owned.sh`, `scripts/sanitize-runtime.sh`, and the
  Slice01 ledger/closing report.
- `git diff-tree --no-commit-id --name-only -r a701b32e...` confirmed no
  touched paths under `pandaPI/` and no release/package/publish paths.
- `git diff --name-only a701b32e^ a701b32e --
  docs/design-v0.3.0/ci-notes.md` returned no output.
- The runtime formatting diff is semantically inert. A whitespace-stripped
  token comparison over touched runtime `.hpp`/`.cpp` files showed one apparent
  token drift in `fixture_substrate_smoke.cpp`; inspection showed only a long
  string literal split into adjacent C++ string literals, which concatenate to
  the same value.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Slice open set contains the required runway scope, boundaries, tooling, and deferral language. |
| F-2 | reproduced | `.clang-format` exists; README and closing report document owned-source scope and no vendored bulk format. |
| F-3 | reproduced | `scripts/check-format-owned.sh` is executable, `bash -n` passes, finds `clang-format`/`xcrun`, and scans `pandapi-runtime` only. |
| F-4 | reproduced | `./scripts/check-format-owned.sh` passed: `OK: 25 files`. |
| F-5 | reproduced | `scripts/sanitize-runtime.sh` is executable, `bash -n` passes, uses strict shell hygiene, ASan/UBSan flags, and `build/runtime-sanitize/<platform>`. |
| F-6 | reproduced | `./scripts/sanitize-runtime.sh` passed on macos-arm64; CTest passed 5/5. |
| F-7 | reproduced | `./scripts/build-runtime.sh` passed; `build/runtime/macos-arm64/compile_commands.json` exists. |
| F-8 | reproduced | `clang-tidy` is not on local `PATH`; README and closing report record the installed-toolchain gate and owned-source scope. |
| F-9 | reproduced | CI runs `Build runtime` before parser, grounder, and engine build steps. |
| F-10 | reproduced | CI installs `clang-format` on Linux, uses Xcode lookup on macOS, and runs `./scripts/check-format-owned.sh`. |
| F-11 | reproduced | CI runs `./scripts/sanitize-runtime.sh` on Linux; closing report records the runner decision. |
| F-12 | reproduced | `ci-notes.md` was not edited by the implementation commit. |
| F-13 | reproduced | README and closing report defer coverage and TSan with process-fixture and representative-workload re-entry conditions. |
| F-14 | reproduced | Commit-scoped protected path check found no `pandaPI/` changes. |
| F-15 | reproduced | Commit-scoped protected path check found no release assets, manifests, pins, dist, release, package, or publish path changes. |
| F-16 | reproduced | `./scripts/build-runtime.sh`, `./scripts/build-all.sh`, `./scripts/smoke-test.sh`, and `./scripts/smoke-test.sh --negative` all passed. |
| F-17 | reproduced | `actionlint -color` passed locally. |
| F-18 | reproduced | `git diff --check` and `git diff --cached --check` passed. |
| F-19 | reproduced | Commit-scoped diff scan found no prohibited fetch/import or held/rejected dependency mechanism in the new tooling/runtime surfaces. |
| F-20 | reproduced | Closing report exists and contains capability verdict, implementation decisions, verification, ledger walk, silent-drop check, and Bubble-up to Arc05. |
| F-21 | reproduced | `cdc-verification.md` was absent from the implementation commit and absent before CDC wrote this file. |

## Commands Reproduced

```text
./scripts/check-format-owned.sh
./scripts/build-runtime.sh
find build/runtime -name compile_commands.json -print -quit
./scripts/sanitize-runtime.sh
./scripts/build-all.sh
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
actionlint -color
git diff --check
git diff --cached --check
rg -c '^\| F-' docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md
rg -n '\| open \|' docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md
```

Observed results:

- Format gate: 25 owned runtime files checked.
- Runtime build: macos-arm64 CTest passed 5/5.
- Compile database: `build/runtime/macos-arm64/compile_commands.json`.
- Sanitizer build: macos-arm64 ASan/UBSan CTest passed 5/5.
- All-build: runtime, parser, grounder, and engine builds completed.
- Positive smoke: 5 passed, 0 failed.
- Negative smoke: 4 passed, 0 failed.
- `actionlint -color`: passed.
- Whitespace checks: unstaged and staged checks passed.
- Ledger rows: 21.
- Open rows: none.

## Code Review Notes

No blocking findings.

`scripts/check-format-owned.sh` is appropriately scoped to
`pandapi-runtime/include`, `pandapi-runtime/src`, and `pandapi-runtime/tests`.
It does not scan or format vendored `pandaPI/` source.

`scripts/sanitize-runtime.sh` uses Clang ASan/UBSan flags, a dedicated
generated build tree, and CTest. It does not produce release binaries.

`.github/workflows/build-reusable.yml` adds runtime build and owned-format
gates before inherited builds, and adds Linux sanitizer coverage while leaving
macOS to the normal runtime build/format path. That matches the Slice01
requirement for at least one reliable sanitizer runner.

## Bubble-up to Arc05

Slice01 delivered the quality/tooling runway assigned by the Arc05 plan. It
does not require an Arc05 scope change.

Slice02 `contract-fixture-scaffold` is ready to open. Carry forward:

- Use the owned-source format gate for future chengdu-owned adapter/facade
  code.
- Use the runtime compile database as the concrete basis for later
  `clang-tidy`.
- Keep coverage deferred until process fixtures exercise migrated binaries and
  representative owned adapter paths.
- Keep TSan deferred until subprocess, timeout/signal, and stream-draining
  workloads are representative.
- Keep Arc06/Arc07/Arc08 downstream obligations out of Arc05 implementation
  slices unless explicitly opened.

## What Worked

The slice stayed narrow: format, sanitizer, compile commands, CI runway, and
documentation landed before binary behavior work. The separate format-only
review made it possible to distinguish clang-format churn from semantic
runtime changes.
