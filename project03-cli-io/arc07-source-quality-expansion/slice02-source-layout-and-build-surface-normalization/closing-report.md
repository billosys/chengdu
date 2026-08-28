# Arc07 Slice02 Closing Report: source-layout-and-build-surface-normalization

Status: CC proposed done
Implementation commit: this commit

## Capability Verdict

Proposed done. Slice02 adds a Make-backed source-quality normalization surface
without changing parser, grounder, engine, runtime, public docs, CI, release
tooling, or quality thresholds.

New public Make targets:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-parser`
- `make compile-db-grounder`
- `make compile-db-engine`
- `make compile-db-first-party`

Generated evidence is written under `build/source-quality/<platform>/`.

## Implementation Notes

- Added `mk/source-quality.mk` and included it from the top-level `Makefile`.
- Added source-quality Make variables in `mk/config.mk` for first-party roots,
  generator/templates, generated outputs, third-party/dependency exclusions,
  copied build roots, and generated build output.
- Added `tools/source-quality/surface`, a Make-invoked helper that writes the
  surface report, source/profile map, and parser/grounder/engine compile
  database artifacts with first-party filter evidence.
- Parser and grounder compile databases are Make-derived because their local
  build systems are Make-based and do not emit `compile_commands.json`.
  Engine compile database generation uses CMake's
  `CMAKE_EXPORT_COMPILE_COMMANDS=ON`.
- First-party selected translation-unit counts from generated evidence:
  parser 19 of 22 raw entries, grounder 22 of 36 raw entries, engine 52 of 53
  raw entries, aggregate 93 selected entries.

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | `rg -n -- "source-quality\|SOURCE_QUALITY\|FIRST_PARTY\|GENERATOR\|GENERATED\|THIRD_PARTY\|DEPENDENCY\|COPIED_BUILD\|BUILD_OUTPUT" Makefile mk tools` passes. |
| F-2 | done | `make help` documents `source-quality-surface`; `make source-quality-surface` passes and writes `build/source-quality/macos-arm64/source-quality-surface.md` and `.tsv`. |
| F-3 | done | The surface report names `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine`, `pandaPI/runtime`, and `first-party maintained`. |
| F-4 | done | Make/helper/report evidence names `hddl.y`, `hddl-token.l`, `options.ggo`, `cmdline.(c\|h)`, `hddl.(cpp\|hpp)`, `hddl-token.cpp`, Flex, Bison, and gengetopt policy. |
| F-5 | done | Make/helper/report evidence names `cpddl`, `h2-fd-preprocessor`, `third-party`, and `cudd-3.0.0` exclusions. |
| F-6 | done | Make/helper/report evidence names copied build roots, `build/$(PLATFORM)/source`, `dist`, generated build output, copied build, and `source-map` evidence. |
| F-7 | done | `make help` documents `compile-db-parser`; `make compile-db-parser` writes non-empty `compile_commands.json`, `first-party-selected.txt`, and `filter-report.md`. |
| F-8 | done | `make help` documents `compile-db-grounder`; `make compile-db-grounder` writes non-empty `compile_commands.json`, `first-party-selected.txt`, and `filter-report.md`. |
| F-9 | done | `make help` documents `compile-db-engine`; `make compile-db-engine` writes non-empty `compile_commands.json`, `first-party-selected.txt`, and `filter-report.md`. |
| F-10 | done | `make compile-db-first-party` runs parser, grounder, and engine compile DB targets and writes aggregate `compile-db/first-party-selected.txt`. |
| F-11 | done | `make help` documents `source-quality-profile-map`; `make source-quality-profile-map` writes `source-quality-profile-map.md` and `source-map.tsv` with parser, grounder, engine, runtime, `LLVM_PROFILE_FILE`, and `pandaPI/` mappings. |
| F-12 | done | Staged boundary checks pass: no `.github`, README, release, `tools/release`, Arc08, or Arc09 changes, and no Make/tools threshold or public-surface expansion terms. |
| F-13 | done | Required regression gates passed; see Verification. `make check` was not run separately because this slice ran the prompt's narrower superset/constituent gates, including `make build`, `make test`, `make provenance-check`, static analysis, format, safety, actionlint, and whitespace checks. |
| F-14 | done | This closing report walks F-1 through F-14 and includes Bubble-up to Arc07. |

## Verification

New target verification:

- `make help`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-parser`
- `make compile-db-grounder`
- `make compile-db-engine`
- `make compile-db-first-party`

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

## Boundary Check

Changed source surfaces are limited to Make source-quality plumbing and Arc07
slice evidence:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `tools/source-quality/surface`
- `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/ledger.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/closing-report.md`

No parser, grounder, engine, or runtime behavior source changed. No generated
grammar/lexer/option output changed. No README/tutorial, CI workflow,
release/package/publication, Arc08, or Arc09 surface changed. No coverage
floor, warning fail budget, expanded static-analysis enforcement, or public
supported-surface policy was added.

## Bubble-up to Arc07

Slice03 `first-party-quality-gate-scaffold` should open next. Slice02 did not
find a required split or remediation first: parser and grounder compile
database evidence can be generated through Make-derived entries without a
larger build-system rewrite, engine compile database generation works through
CMake, and source/profile mapping evidence is now available under
`build/source-quality/<platform>/`.

Slice03 should build on these selectors and reports, but should still avoid
coverage floors, warning fail budgets, Catch2 dependency pinning, or
parser/grounder/engine static-analysis enforcement until its own ledger
accepts those policies.
