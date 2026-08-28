# Arc07 Slice03 Closing Report: first-party-source-naming-normalization

Status: CC proposed done
Implementation commit: this commit

## Capability Verdict

Proposed done. Slice03 normalizes first-party maintained C/C++ source file and
source directory basenames to lower snake case, adds Make-backed naming
report/check targets, and preserves generated/build/dependency exclusions.

New Make targets:

- `make source-quality-naming-report`
- `make source-quality-naming-check`

Durable report:

- `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`

## Implementation Notes

- Added deterministic naming report/check subcommands to
  `tools/source-quality/surface` and exposed them through `mk/source-quality.mk`.
- Documented target discovery in `mk/help.mk`.
- Renamed non-conforming first-party maintained parser, grounder, and engine
  files/directories with `git mv`.
- Updated parser makefile entries, CMake path/build inputs, and include
  references to the renamed paths. CMake target names remain inherited metadata
  outside the filesystem naming policy.
- Kept generated output, dependency-internal source, vendored third-party
  source, copied build artifacts, and build output out of naming scope.
- Updated Arc07 source-classification path references that named renamed files.

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | `make help \| rg "source-quality-naming-(report\|check)"` passes; `rg -n -- "source-quality-naming-(report\|check)" Makefile mk tools` finds Make/help/helper wiring. |
| F-2 | done | `make source-quality-naming-report` writes the durable report; the required term check finds `lower snake case`, `first-party maintained`, `rename map`, `excluded`, `cpddl`, `h2-fd-preprocessor`, and `cudd-3.0.0`. |
| F-3 | done | Renames were performed with `git mv`; staged summary evidence contains parser, grounder, and engine renames and no intentional first-party create/delete pairs. |
| F-4 | done | `make source-quality-naming-check` passes with zero current naming violations over the first-party maintained C/C++ surface. |
| F-5 | done | `make build-parser` passes; `make test-contract-parser-managed` passes with `SUMMARY: 298 passed, 0 failed`. |
| F-6 | done | `make build-grounder` passes; `make test-contract-grounder-managed` passes with `SUMMARY: 269 passed, 0 failed`. |
| F-7 | done | `make build-engine` passes; `make test-contract-engine-managed` passes with `SUMMARY: 312 passed, 0 failed`. |
| F-8 | done | Excluded roots `pandaPI/grounder/cpddl`, `pandaPI/grounder/h2-fd-preprocessor`, and `pandaPI/engine/src/symbolic_search/cudd-3.0.0` have no staged changes; `make source-quality-surface` still reports `GENERATED`, `THIRD_PARTY`, `DEPENDENCY`, `COPIED_BUILD`, and `BUILD_OUTPUT`. |
| F-9 | done | `make source-quality-surface`, `make source-quality-profile-map`, and `make compile-db-first-party` pass and regenerate evidence under `build/source-quality/macos-arm64/`. |
| F-10 | done | Required regression gates pass; see Verification. `make check` was not run separately because the slice reproduced the requested constituent gates directly and avoids the known aggregate provenance-race ambiguity. |
| F-11 | done | Staged boundary checks show no `.github`, README, release, `tools/release`, Arc08, or Arc09 changes and no Make/tools quality-threshold or public-behavior expansion terms. |
| F-12 | done | This closing report walks F-1 through F-12 and includes Bubble-up to Arc07 with the Slice04 recommendation and split/remediation statement. |

## Verification

Naming and source-quality verification:

- `bash -n tools/source-quality/surface`
- `make help | rg "source-quality-naming-(report|check)"`
- `make source-quality-naming-report`
- `make source-quality-naming-check`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-first-party`

Component verification:

- `make build-parser`
- `make test-contract-parser-managed`
- `make build-grounder`
- `make test-contract-grounder-managed`
- `make build-engine`
- `make test-contract-engine-managed`

Regression verification:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make build`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Expected inherited warnings remain visible during component builds, including
generated/dependency warnings and the existing engine `visited_list.cpp` warning
budget. They are not new slice failures.

## Boundary Check

Changed surfaces are limited to first-party path renames, build/include
references required by those renames, Make-backed source-quality naming
plumbing, and Arc07 Slice03 evidence/docs.

No excluded dependency roots were edited:

- `pandaPI/grounder/cpddl`
- `pandaPI/grounder/h2-fd-preprocessor`
- `pandaPI/engine/src/symbolic_search/cudd-3.0.0`

No README/tutorial, CI workflow, release/package/publication, Arc08, Arc09,
coverage floor, warning fail budget, or public supported-surface policy was
added.

## Bubble-up to Arc07

Slice04 `first-party-quality-gate-scaffold` should open next. Slice03 did not
reveal a required split or remediation before Slice04: the naming check is
Make-backed, the first-party maintained naming surface is clean, source-class
exclusions remain intact, and parser/grounder/engine managed contract fixtures
still pass after the rename set.

Slice04 should build on this naming gate and the Slice02 source-quality
selectors, while still avoiding coverage floors, warning fail budgets,
README/tutorial expansion, and public behavior changes unless its own ledger
accepts those policies.
