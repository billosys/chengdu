# Arc05 Slice01 Closing Report - quality-tooling-runway

Closer: CC
Branch: `release/0.3.x`
Closed on: 2026-08-09

## Capability Verdict

Delivered.

Arc05 Slice01 put the pre-adoption quality runway in place before parser,
grounder, or engine behavior changes. The slice adds owned-source formatting,
runtime ASan/UBSan, runtime compile-command generation, CI coverage for the
new gates, and explicit deferrals for clang-tidy, coverage, TSan, and Arc06
release proof.

No binary adoption occurred. Parser, grounder, and engine do not link to
`pandapi::runtime`; no `pandapi-*` wrappers, symlinks, copied binaries, renamed
binaries, release entry points, package changes, or inherited-name migrations
were added.

## Implementation Decisions

The top-level `.clang-format` uses an LLVM-derived C++17 style with two-space
indentation, 88-column wrapping, left pointer alignment, stable include order,
and function braces configured to match the current runtime code shape. The
format pass was applied only to chengdu-owned runtime headers, sources, and
tests under `pandapi-runtime/include`, `pandapi-runtime/src`, and
`pandapi-runtime/tests`; vendored `pandaPI/` source was not bulk-formatted.

`scripts/check-format-owned.sh` is the owned-source format gate. It uses
strict shell hygiene, locates `clang-format` from `PATH` or macOS `xcrun`, and
checks only the runtime owned-source set. Future chengdu-owned adapter
directories can be added when they exist.

`scripts/sanitize-runtime.sh` is the ASan/UBSan gate. It uses `clang++`,
configures `pandapi-runtime/` out of source under
`build/runtime-sanitize/<platform>/`, applies `-O1 -g -fno-omit-frame-pointer
-fsanitize=address,undefined` plus sanitizer link flags, builds runtime tests,
and runs CTest. It does not produce release binaries.

`scripts/build-runtime.sh` now passes `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`, so
`build/runtime/<platform>/compile_commands.json` is available for future
clang-tidy work.

CI now runs owned-source format and runtime build before inherited parser,
grounder, and engine builds on Linux and macOS. Linux installs
`clang-format`/`clang` explicitly and runs the runtime sanitizer gate.
macOS uses the Xcode `xcrun` format lookup and runs the runtime build before
the inherited builds.

`clang-tidy` is deferred behind an installed-toolchain gate because local
`clang-tidy` is not on `PATH`. The intended first scope is owned runtime and
future adapter/facade code with the runtime compile database; Makefile-era
parser and grounder source is not forced into clang-tidy yet.

Coverage is deferred until Arc05 process fixtures exercise migrated binaries
and representative owned adapter paths. TSan is deferred until subprocess,
timeout/signal, and stream-draining workloads are representative. Arc06 still
owns release-package, license/NOTICE, test-only exclusion, checksum, manifest,
wolong, package, publish, and publication proof.

## Verification

Commands run locally:

```text
./scripts/check-format-owned.sh
./scripts/build-runtime.sh
find build/runtime -name compile_commands.json -print -quit | rg -n 'compile_commands.json'
./scripts/sanitize-runtime.sh
./scripts/build-all.sh
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
actionlint -color
git diff --check
git diff --name-only -- pandaPI
git diff --name-only -- release-manifest.txt vendor.env pins.env dist release scripts/package-release.sh scripts/publish-release.sh
rg -c '^\| F-' docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md
! test -e docs/design-v0.3.0/arc05-binary-contract-adoption/slice01-quality-tooling-runway/cdc-verification.md
```

Results:

- Format gate passed: 25 owned runtime files checked.
- Runtime build passed on macos-arm64; CTest passed 5/5.
- Compile database exists at `build/runtime/macos-arm64/compile_commands.json`.
- Sanitizer build passed on macos-arm64; CTest passed 5/5.
- `build-all` passed: runtime, parser, grounder, and engine built in order.
- Positive smoke passed: 5 passed, 0 failed.
- Negative smoke passed: 4 passed, 0 failed.
- `actionlint -color` passed.
- Protected `pandaPI/` and release/package/publish path checks returned no
  output.
- Ledger row count is 21.
- No `cdc-verification.md` exists.

`git diff --cached --check` is rerun after staging before commit.

## Ledger Walk

Rows verified: 21. Done: 21. Deferred: 0. No-op: 0.

| Row | CC disposition |
|-----|----------------|
| F-1 | Done. Slice open set documents the runway scope and boundaries. |
| F-2 | Done. `.clang-format` exists and owned/vendored scope is documented. |
| F-3 | Done. `check-format-owned.sh` is executable, parses cleanly, locates `clang-format`, and avoids vendored-source scanning. |
| F-4 | Done. Owned format check passed for 25 files. |
| F-5 | Done. `sanitize-runtime.sh` is executable, strict, sanitizer-specific, and contains ASan/UBSan compile/link flags. |
| F-6 | Done. Runtime sanitizer build and CTest passed. |
| F-7 | Done. Runtime build produces `compile_commands.json`. |
| F-8 | Done. clang-tidy decision is documented as deferred behind an installed-toolchain gate. |
| F-9 | Done. CI runs runtime build before inherited parser, grounder, and engine builds. |
| F-10 | Done. CI runs owned-source format; Linux installs `clang-format`, macOS uses `xcrun`. |
| F-11 | Done. CI includes ASan/UBSan runtime coverage on Linux. |
| F-12 | Done. `ci-notes.md` was not rewritten. |
| F-13 | Done. Coverage and TSan deferrals and re-entry conditions are documented. |
| F-14 | Done. No `pandaPI/` diff exists. |
| F-15 | Done. No release/package/publish path diff exists. |
| F-16 | Done. Runtime, all-build, positive smoke, and negative smoke gates passed. |
| F-17 | Done. Local `actionlint` passed. |
| F-18 | Done. Unstaged whitespace check passed; staged check is repeated before commit. |
| F-19 | Done. Diff-scoped prohibited import/fetch scan passed. The verify command was corrected because the open command scanned existing dependency-decision prose that must mention held libraries; the corrected command preserves the no-new-import/no-fetch gate. |
| F-20 | Done. This closing report exists with the required sections. |
| F-21 | Done. CDC verification file was not created. |

## Silent-Drop Check

Scope delivered:

- top-level `.clang-format`;
- owned-source format check script;
- runtime ASan/UBSan script;
- runtime compile-command generation;
- CI owned-format/runtime/sanitizer gates;
- runtime README documentation for tooling scope and deferrals;
- closed ledger and CC closing report.

Scope deliberately not delivered:

- no parser, grounder, or engine behavior changes;
- no `pandapi-*` entry points or inherited-name migration;
- no CLI11, fmt, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL,
  google/benchmark, SAT/BDD/CUDD, H2/cpddl, parser-generator, or package
  manager work;
- no release-package, license/NOTICE, checksum, manifest, wolong, package,
  publish, or release workflow changes;
- no coverage threshold, coverage report, TSan gate, Arc05 arc close, or CDC
  verification file.

No requested slice obligation was silently dropped. Later Arc05 slices can
start from an owned-source quality floor without mixing formatting/tooling
churn into binary contract adoption.

## Bubble-up to Arc05

Arc05 Slice02 `contract-fixture-scaffold` is ready to open after independent
CDC verification of this slice.

Carry forward:

- Use the runtime compile database and owned format gate for future owned
  adapter/facade code.
- Keep clang-tidy gated until an installed local/CI toolchain is chosen.
- Re-enter coverage only once process fixtures exercise migrated binaries and
  representative owned runtime adapter paths.
- Re-enter TSan only after subprocess, timeout/signal, and stream-draining
  workloads are representative.
- Keep Arc06 release-package, license/NOTICE, test-only exclusion, checksum,
  manifest, wolong, package, publish, and publication gates out of Arc05
  implementation slices unless explicitly opened.
