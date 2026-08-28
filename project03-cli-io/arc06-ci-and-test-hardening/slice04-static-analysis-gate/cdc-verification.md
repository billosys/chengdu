# CDC Verification - Arc06 Slice04 - static-analysis-gate

Verifier: CDC
Verified on: 2026-08-11
Implementation commit reviewed: `04491682`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc06 Slice04 is CDC-verified.

The slice delivered a Make-backed C++ static-analysis gate for owned runtime
code. `make static-analysis-cpp` configures a Debug
`pandaPI/runtime` compile database under
`build/static-analysis/runtime/<platform>/build/` and runs `clang-tidy` with
`clang-analyzer-*` over the runtime source and test translation units present
in that database.

The adoption-seam deferral is valid. Static analysis for
`pandapi_parser_native.cpp`, `pandapi_grounder_native.cpp`, and
`pandapi_engine_native.cpp` remains deferred until canonical `pandapi-*` builds
expose reliable compile commands for those owned native files without mixing
inherited/generated and third-party findings into the owned signal.

## Source and Scope Checks

- `git show --stat --oneline --name-status 04491682` confirmed the commit is
  limited to Arc06 docs, Make static-analysis wiring, and
  `pandaPI/runtime/README.md`.
- `git show --format=fuller --no-patch 04491682` confirmed both required
  co-author trailers are present.
- `mk/checks.mk` adds `static-analysis-cpp` and wires it into the existing
  `static-analysis` aggregate after shell syntax and shellcheck.
- The target resolves `clang-tidy` from `CLANG_TIDY`, `PATH`, `xcrun`, or
  common Homebrew LLVM paths; missing tool behavior is an explicit SKIP with a
  re-entry condition, not a silent no-op.
- An adapted committed-path boundary check confirmed no touched parser,
  grounder, engine product behavior, top-level README, release packaging,
  release workflow, Arc07, or Arc08 surfaces.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | `make help | rg -n "static.*C\\+\\+|clang-tidy|analyzer|cpp-static|static-analysis-cpp"` lists both `make static-analysis` and `make static-analysis-cpp`. |
| F-2 | reproduced | Runtime compile databases exist at `build/runtime/macos-arm64/compile_commands.json` and `build/static-analysis/runtime/macos-arm64/build/compile_commands.json`; `make static-analysis-cpp` uses the dedicated Debug database. |
| F-3 | reproduced | `make static-analysis-cpp` ran `/opt/homebrew/opt/llvm/bin/clang-tidy` 20.1.6 and passed over 14 runtime translation units. |
| F-4 | reproduced with deferral | `clang-analyzer-*` checks are active through `clang-tidy`; standalone `scan-build` is deferred with reason and re-entry condition in Slice04 docs/runtime README. |
| F-5 | reproduced | Slice04 docs and runtime README document included runtime source/tests/reached headers, excluded inherited/generated/third-party/build/dist paths, and deferred adoption seams. |
| F-6 | reproduced | No `NOLINT` suppressions were introduced; the no-suppression state and future narrow-suppression policy are documented. |
| F-7 | reproduced as deferred | Adoption-seam deferral names the three native files, reason, and re-entry condition. |
| F-8 | reproduced | `make safety-checks && make actionlint` passed; no workflow change was made. |
| F-9 | reproduced | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` passed. |
| F-10 | reproduced | Adapted post-commit boundary check found no touched parser, grounder, engine product, README, release, Arc07, or Arc08 surfaces. |

## Commands Reproduced

```text
make static-analysis-cpp
make help | rg -n "static.*C\\+\\+|clang-tidy|analyzer|cpp-static|static-analysis-cpp"
runtime compile-database presence checks
ledger greps for clang-tidy, analyzer disposition, path policy, suppressions, and adoption-seam re-entry
make safety-checks && make actionlint
post-commit boundary check for 04491682
make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check
```

Observed results:

- `make static-analysis-cpp`: passed with Homebrew LLVM `clang-tidy` 20.1.6.
- Runtime static-analysis scope: 14 translation units under
  `pandaPI/runtime/src` and `pandaPI/runtime/tests`.
- Static-analysis aggregate: passed.
- Format check: passed for 25 owned C++ files.
- Managed fixtures during `make test`: parser 298/0, grounder 269/0, engine
  312/0, pipeline 129/0.
- Positive smoke: 3/0. Negative smoke: 4/0.
- Provenance check passed for all three components on `macos-arm64`.

Inherited compiler warnings appeared during parser, grounder, and engine
builds. They did not block Slice04 because compiler-warning inventory and
burndown is explicitly assigned to Slice05.

## Review Notes

No blocking findings remain.

CDC found one stale documentation row in the fixture inventory: the
`make static-analysis` gate map row still described shell syntax/shellcheck
only and said C++ static analysis was deferred. That was corrected in the CDC
close/open packet so the aggregate map now matches `mk/checks.mk`.

The `clang-tidy` scope is intentionally narrow and useful: owned runtime
source/tests first, with inherited planner, generated, third-party, `build`,
and `dist` paths excluded from the owned signal. This is strong enough to
inform warning ownership, but it is not a replacement for the Slice05 warning
inventory across parser, grounder, engine, runtime, generated code, and nested
third-party code.

## Bubble-up to Arc06

Slice04 delivered Arc06 row A6 for the owned-runtime surface: C++ static
analysis now runs through Make, uses a reliable compile database, documents
toolchain behavior, and records the adoption-seam deferral honestly.

The next slice should open as Slice05 `compiler-warning-burndown`. Its first
job is to turn the inherited warning stream observed during `make test` into a
current inventory by ownership tier before deciding which warnings to fix,
budget, suppress, or defer.
