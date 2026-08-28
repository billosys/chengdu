# Arc07 Slice05 Closing Report: engine-first-party-quality-burndown

Status: CC proposed done
Date: 2026-08-12
Branch: `release/0.3.x`

## Scope Delivered

Slice05 converts the engine rows from the Slice04 source-quality scaffold into
Make-backed component targets without changing parser, grounder, public CLI
behavior, CI workflows, release tooling, README/tutorial docs, Arc08, Arc09,
dependencies, global warning policy, global coverage policy, or optional/fenced
engine support.

Delivered targets:

- `format-check-engine`
- `static-analysis-engine`
- `coverage-engine`
- `warning-inventory-engine`
- `test-unit-engine`
- `sanitize-engine`

Code changes:

- `visited_list` payload cost encoding is isolated behind helper functions and
  covered by a CTest seam.
- Redundant direct `pandapi-engine` links to `heuristics` and `hrc` were removed;
  `hlm` continues to provide the required transitive libraries.

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | `make help` lists all six engine targets, and `rg` found Make/tool wiring. |
| F-2 | done | `make compile-db-engine` passed; `first-party-selected.txt` exists; scaffold/tooling grep confirmed engine, first-party maintained, cmdline, CUDD, COPIED_BUILD, BUILD_OUTPUT, and generated routing. |
| F-3 | done | `make format-check-engine` passed and recorded 126 inherited engine format drift entries without rewriting source. |
| F-4 | done | `make static-analysis-engine` completed with clang-tidy status 1 and 98 reported finding lines in `build/static-analysis/engine/macos-arm64/clang-tidy.txt`; this is a reported baseline, not a blanket suppression. |
| F-5 | done | `make test-unit-engine` passed; CTest ran `pandapi_engine_visited_list_payload` for the `visited_list` payload representation seam. |
| F-6 | done | `make warning-inventory-engine` passed; the exact negative grep for the old `visited_list` payload-cast warning passed. |
| F-7 | done | `make warning-inventory-engine` passed; duplicate `libheuristics`/`libhrc` linker noise was not observed after removing redundant direct links. |
| F-8 | done | `make coverage-engine` passed and printed `build/coverage/engine/macos-arm64/report/engine-coverage-summary.txt`; the report states no release floor. |
| F-9 | done | `make sanitize-engine` passed with the engine seam under ASan/UBSan and recorded LSan/TSan/binary source-class re-entry. |
| F-10 | done | `make build-engine && make test-contract-engine-managed` passed; managed engine fixtures reported 312 passed, 0 failed. |
| F-11 | done | Existing source-quality runway passed, including `warning-inventory-first-party`; aggregate warning inventory captured 54 warning lines. |
| F-12 | done | `make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` passed. |
| F-13 | done | Cached boundary check passed after staging. |
| F-14 | done | This closing report walks F-1 through F-14 and includes Bubble-up to Arc07. |

## Verification

Engine component targets:

- `make help` passed.
- `make compile-db-engine` passed.
- `make format-check-engine` passed.
- `make static-analysis-engine` completed and wrote the analyzer baseline.
- `make coverage-engine` passed.
- `make warning-inventory-engine` passed.
- `make test-unit-engine` passed.
- `make sanitize-engine` passed.
- `make build-engine` passed.
- `make test-contract-engine-managed` passed.

Cross-surface and regression gates:

- `make source-quality-surface` passed.
- `make source-quality-profile-map` passed.
- `make source-quality-naming-check` passed.
- `make compile-db-first-party` passed.
- `make warning-inventory-first-party` passed.
- `make safety-checks` passed.
- `make actionlint` passed.
- `make static-analysis` passed.
- `make format-check` passed.
- `make test` passed.
- `make provenance-check` passed.
- `git diff --check` passed.
- `git diff --cached --check` passed.

`make check` was not run as the aggregate. The exact F-12 constituent chain
was run instead because it reproduces the required behavior, safety, static,
format, provenance, and whitespace gates directly while avoiding the known
macOS aggregate provenance mutation/noise path.

## Bubble-up to Arc07

Recommended next slice: parser generated-warning/root-cause work.

Rationale: engine now has its first component target foothold, the
`visited_list` payload representation warning is closed with seam evidence, and
duplicate engine link-library noise is not observed. Parser primary warnings
are clear, but Flex/gengetopt generated-warning policy still needs
generator/skeleton correctness decisions before strict enforcement.

Engine remediation re-entry: return to engine if CDC finds residual warning
debt, selects specific static-analysis findings for strict treatment, or wants
additional risk-bearing engine internals covered by CTest or Catch2 seams.
