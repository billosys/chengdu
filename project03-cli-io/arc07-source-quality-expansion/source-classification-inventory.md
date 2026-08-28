# Arc07 Source Classification Inventory

Date: 2026-08-12
Branch: `release/0.3.x`
Inventory scope: `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine`,
and `pandaPI/runtime`

## Summary

All four active pandaPI product components are Chengdu-owned in the fork and
release sense. Quality policy is controlled by source class:
first-party maintained code gets formatting, static analysis, coverage,
warning policy, sanitizer triage, and unit/seam-test strategy; generated and
third-party paths get routed according to the ownership of their generator,
template, or dependency boundary.

Slice02 should perform source-layout and build-surface normalization before
Arc07 expands thresholds. The current Make gates are useful, but the strongest
runtime gates still select `pandaPI/runtime` paths, while parser, grounder,
and engine builds mix first-party files with generated source, copied build
trees, and nested dependency code.

## Source-Class Taxonomy

| Class | Definition | Gate posture |
|-------|------------|--------------|
| first-party maintained | Hand-maintained product, adapter, runtime, test, build, or integration source that Chengdu ships or maintains in the fork. Includes parser, grounder, engine, runtime, and maintained integration code even when originally inherited. | In scope for format, static analysis, coverage, warning policy, sanitizer triage, and unit/seam-test strategy. |
| first-party generated | Generated output whose input grammar/spec/template is maintained by Chengdu and whose generated file is intentionally committed. | Prefer fixing the generator/template. If committed output is unavoidable, check or suppress it with explicit rationale. |
| first-party generator/template | Chengdu-maintained grammar, lexer, option spec, CMake/Make template, or other source that produces generated code. | In scope for review, formatting where applicable, tests, and generator-level fixes. |
| vendored third-party | Source imported from an external project and kept in tree as a dependency. | Excluded from first-party coverage/static-analysis/unit-test obligations unless a later dependency audit accepts the boundary. Build and sanitizer findings are triaged separately. |
| dependency-internal | Source inside a vendored dependency or copied private dependency implementation used by a first-party integration boundary. | Excluded as dependency code; first-party adapters that call into it remain in scope. |
| third-party generated | Generated output owned by a third-party project or third-party skeleton inside a vendored tree. | Excluded or suppressed with rationale; do not repair it as first-party product code unless the dependency boundary is accepted. |
| copied build artifact | Disposable source copy, object, archive, binary, or build placeholder created or retained for build mechanics. | Not a source-quality owner. If the copied file maps back to first-party source, fix the original path. |
| generated build output | Ignored/generated files created by Flex, Bison, gengetopt, CMake, CTest, coverage, warning inventory, sanitizer, packaging, or release tooling. | Not committed source-quality scope. Use the source input or report output as evidence. |
| component documentation/data | Component READMEs, NOTICE files, test data, example plans, generated docs configs, and fixture data that are not compiled product source. | Documentation/data checks only unless a later slice promotes them into examples or public docs. |

## Path Inventory

Inventory commands used:

- `find pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime -maxdepth 3 -type d | sort`
- `git ls-files pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime`
- `find build dist -maxdepth 3 -type d 2>/dev/null | sort`
- `rg -n -- "cmdline\\.c|hddl-token\\.cpp|hddl\\.cpp|hddl\\.hpp|options\\.ggo|gengetopt|flex|bison|prepare_build_source_copy" pandaPI/parser/makefile pandaPI/grounder/src/Makefile pandaPI/engine/src/CMakeLists.txt tools/shared/platform`

| Component | Path or pattern | Class | Notes |
|-----------|-----------------|-------|-------|
| parser | `pandaPI/parser/src/*.cpp`, `pandaPI/parser/src/*.hpp` excluding generated outputs | first-party maintained | Main parser implementation, writers, transformations, validation, native `pandapi-parser` entrypoint, and legacy main. Current tracked source excludes generated `hddl.cpp`, `hddl.hpp`, `hddl-token.cpp`, and `cmdline.*`. |
| parser | `pandaPI/parser/makefile` | first-party maintained | Build rules and generated-output rules for Flex, Bison, and gengetopt. |
| parser | `pandaPI/parser/src/hddl.y`, `pandaPI/parser/src/hddl-token.l`, `pandaPI/parser/src/options.ggo` | first-party generator/template | Bison grammar, Flex scanner, and gengetopt option spec. |
| parser | `pandaPI/parser/src/hddl.cpp`, `pandaPI/parser/src/hddl.hpp`, `pandaPI/parser/src/hddl-token.cpp`, `pandaPI/parser/src/hddl.output`, `pandaPI/parser/src/cmdline.c`, `pandaPI/parser/src/cmdline.h` | generated build output | Generated by `pandaPI/parser/makefile`; ignored by `pandaPI/parser/src/.gitignore` or absent from tracked source. |
| parser | `pandaPI/parser/doc/`, `pandaPI/parser/tests/`, `pandaPI/parser/README.md`, `pandaPI/parser/NOTICE`, `pandaPI/parser/LICENSE`, `pandaPI/parser/_config.yml` | component documentation/data | Inputs and component-local documentation; not compiled product source. |
| grounder | `pandaPI/grounder/src/*.cpp`, `pandaPI/grounder/src/*.h`, `pandaPI/grounder/src/*.c` excluding generated outputs | first-party maintained | Main grounder implementation, output, parser, H2/FAM integration files, `cpddl.h` integration wrapper, native `pandapi-grounder` entrypoint, and legacy main. `rss.c` is selected here as first-party maintained copied-origin RSS utility source because Chengdu ships and builds the local copy from `pandaPI/grounder/src`; warning fixes apply to this copy until a later dependency audit chooses a different ownership boundary. |
| grounder | `pandaPI/grounder/tests/*.cpp` | first-party maintained | Grounder unit/seam tests owned by Chengdu. These are source-quality evidence for maintained seams and remain separate from process-contract fixtures. |
| grounder | `pandaPI/grounder/src/Makefile`, `pandaPI/grounder/build.sh` | first-party maintained | Build entrypoints and source rules. Public repo workflow should still enter through top-level Make targets. |
| grounder | `pandaPI/grounder/src/options.ggo` | first-party generator/template | gengetopt option spec. |
| grounder | `pandaPI/grounder/src/cmdline.c`, `pandaPI/grounder/src/cmdline.h`, `pandaPI/grounder/src/*.o`, `pandaPI/grounder/src/doc/` | generated build output | Generated or build-produced in the source directory; ignored by `pandaPI/grounder/src/.gitignore` or absent from tracked source. |
| grounder | `pandaPI/grounder/cpddl/`, except `cpddl/third-party/*` | vendored third-party / dependency-internal | cpddl is in tree and linked through the grounder build. Grounder-owned integration files remain first-party maintained. |
| grounder | `pandaPI/grounder/h2-fd-preprocessor/` | vendored third-party / dependency-internal | H2 source is compiled into the grounder build but is not a supported public surface by itself. |
| grounder | `pandaPI/grounder/cpddl/third-party/bliss`, `boruvka`, `lpsolve`, `opts` | vendored third-party | Nested dependency source. Warning/static/coverage obligations require a dependency audit or accepted support change. |
| grounder | `pandaPI/grounder/cpddl/.objs/.dir`, `pandaPI/grounder/cpddl/third-party/boruvka/.objs/.dir` | copied build artifact | Tracked placeholder files for dependency build directories; not source-quality owners. |
| grounder | `pandaPI/grounder/doc/`, `pandaPI/grounder/README.md`, `pandaPI/grounder/NOTICE`, `pandaPI/grounder/LICENSE`, `pandaPI/grounder/src/Doxyfile` | component documentation/data | Component-local documentation/data/configuration; Doxygen output is not part of Arc07 source gates unless a later docs slice promotes it. |
| engine | `pandaPI/engine/src/*.cpp`, `*.h`, and first-party subdirectories `heuristics/`, `int_data_structures/`, `search/`, `sat/`, `symbolic_search/*.cpp|*.h`, `translation/` excluding CUDD | first-party maintained | Engine implementation, data structures, optional/fenced first-party subdirectories, native `pandapi-engine` entrypoint, and legacy driver. Source quality can cover these without promoting optional CLI behavior. |
| engine | `pandaPI/engine/src/CMakeLists.txt`, subdirectory `CMakeLists.txt` files | first-party maintained | CMake source/build surface, including generated-output rules. |
| engine | `pandaPI/engine/src/options.ggo` | first-party generator/template | gengetopt option spec. |
| engine | `pandaPI/engine/src/cmdline.c`, `pandaPI/engine/src/cmdline.h`, CMake-generated build trees | generated build output | Generated by the engine CMake rule from `options.ggo`; absent from tracked source until build-time generation. |
| engine | `pandaPI/engine/src/symbolic_search/cudd-3.0.0/` | vendored third-party | Bundled CUDD source. BDD/CUDD behavior is fenced as future work in current managed fixtures. |
| engine | CUDD sample `.out`, `.test.in`, `.la`, BDD examples, Autotools helper files | third-party generated / copied build artifact | Part of vendored CUDD material. Exclude from first-party gates unless CUDD is audited and accepted. |
| runtime | `pandaPI/runtime/include/`, `pandaPI/runtime/src/`, `pandaPI/runtime/tests/`, `pandaPI/runtime/CMakeLists.txt`, `pandaPI/runtime/cmake/` | first-party maintained | Current strongest quality surface: CMake/CTest, formatting, coverage, static analysis, and sanitizer gates are already runtime-centered. |
| runtime | `build/runtime/<platform>/`, `build/runtime-sanitize/<platform>/`, `build/coverage/runtime/<platform>/`, `build/static-analysis/runtime/<platform>/` | generated build output | Ignored build, coverage, sanitizer, and compile-database output. |
| all components | `build/<platform>/source/parser.*`, `grounder.*`, `engine.*`, `pandaPIgrounder.*`, `pandaPIengine.*` | copied build artifact | Created by `prepare_build_source_copy` for build-time mutation and generated files. Source-quality findings must map back to `pandaPI/<component>/...`. |
| all components | `dist/<platform>/` | generated build output | Built release/developer artifacts and provenance output; not source-quality source. |

The inventory command reported `1131` tracked paths under the four component
roots. A C/C++ path classifier over tracked files reported these source counts:

| Component | Class | Count |
|-----------|-------|-------|
| parser | first-party maintained | 38 |
| parser | first-party generator/template | 5 |
| grounder | first-party maintained | 45 |
| grounder | first-party generator/template | 1 |
| grounder | vendored third-party / dependency-internal | 108 |
| grounder | vendored third-party | 345 |
| engine | first-party maintained | 128 |
| engine | first-party generator/template | 1 |
| engine | vendored third-party | 120 |
| runtime | first-party maintained | 25 |

Counts are planning aids, not release gates. Slice02 should replace them with
canonical Make-backed selectors.

## Generated-Code Ownership

| Generated-code ownership | Generated file or output | Generator/template | Owner/class | Warning and format policy | Re-entry |
|--------------------------|--------------------------|--------------------|-------------|---------------------------|----------|
| parser grammar | `pandaPI/parser/src/hddl.cpp`, `hddl.hpp`, `hddl.output` | `pandaPI/parser/src/hddl.y` through Bison | first-party generator/template plus third-party skeleton output | Fix grammar/action defects in `hddl.y`. Skeleton-format or Bison helper warnings may be excluded or suppressed with rationale. | Generated parser warnings become release blockers or parser compile/profile isolation requires committed generated output. |
| parser scanner | `pandaPI/parser/src/hddl-token.cpp` | `pandaPI/parser/src/hddl-token.l` through Flex | first-party generator/template plus third-party skeleton output | Fix lexical rules in `hddl-token.l`; budget Flex helper warnings such as unused `yyunput` unless they point to owned rules. | Parser generated warnings become a strict warning gate. |
| parser CLI option parser | `pandaPI/parser/src/cmdline.c`, `cmdline.h` | `pandaPI/parser/src/options.ggo` through gengetopt | first-party generator/template plus third-party generated output | Fix option semantics in `options.ggo` or managed native wrapper. Do not treat generated prose as final process-contract evidence. | CLI11 replacement or strict generated-code gate is accepted. |
| grounder CLI option parser | `pandaPI/grounder/src/cmdline.c`, `cmdline.h` | `pandaPI/grounder/src/options.ggo` through gengetopt | first-party generator/template plus third-party generated output | Fix option semantics in `options.ggo` or `pandapi_grounder_native.cpp`. Budget generated warnings separately. | CLI11 replacement or strict generated-code gate is accepted. |
| engine CLI option parser | `pandaPI/engine/src/cmdline.c`, `cmdline.h` | `pandaPI/engine/src/options.ggo` through gengetopt | first-party generator/template plus third-party generated output | Fix option semantics in `options.ggo` or `pandapi_engine_native.cpp`. Budget generated warnings separately. | CLI11 replacement or strict generated-code gate is accepted. |
| CMake/CTest/runtime reports | `build/runtime*`, `build/static-analysis/runtime`, `build/coverage/runtime`, `compile_commands.json`, CTest files | runtime CMake and Make targets | generated build output | Use as evidence; do not classify as source. | Needed when expanding compile databases to parser, grounder, and engine. |
| copied component build trees | `build/<platform>/source/<component>.*` | `tools/shared/platform::prepare_build_source_copy` | copied build artifact | Findings map to the original source path or generated-output class. | Slice02 should make these copies easier to exclude from reports. |

## Third-Party And Dependency Exclusions

| Third-party/dependency path | Class | Rationale | Separately report or Re-entry |
|-----------------------------|-------|-----------|-------------------------------|
| `pandaPI/grounder/cpddl/` | vendored third-party / dependency-internal | Imported cpddl dependency linked by the grounder build. Arc01 treated direct integration as grounder-owned but did not accept a full cpddl audit. | Separately report build/sanitizer findings. Re-enter through a cpddl dependency audit or accepted FAM/support policy. |
| `pandaPI/grounder/h2-fd-preprocessor/` | vendored third-party / dependency-internal | Compiled into the grounder build for H2 integration; H2 remains fenced from supported managed behavior. | Re-enter if H2 becomes a supported default surface or a dependency audit is accepted. |
| `pandaPI/grounder/cpddl/third-party/bliss` | vendored third-party | Nested cpddl third-party source. NOTICE records materialized durable source. | Exclude from first-party gates; re-enter only through dependency audit or supported build requirement. |
| `pandaPI/grounder/cpddl/third-party/boruvka` | vendored third-party | Nested cpddl third-party source. Current warning budget includes boruvka warnings. | Exclude from first-party gates; report warnings/sanitizer findings separately. |
| `pandaPI/grounder/cpddl/third-party/lpsolve` | vendored third-party / third-party generated | Nested lp_solve source includes Flex/Bison generated files such as `lp_rlp.c` and `lp_rlp.h`. | Exclude from first-party gates; dependency audit required before cleanup obligations. |
| `pandaPI/grounder/cpddl/third-party/opts` | vendored third-party | Nested cpddl third-party source, not linked into shipped binaries per NOTICE. | Exclude from first-party gates. |
| `pandaPI/grounder/src/rss.c` | first-party maintained copied-origin RSS utility | This file carries an external author/license header, but the active build compiles the local copy from the grounder source root rather than a separate dependency tree. Slice07 keeps it in first-party maintained grounder selectors and fixes its strict-prototype warnings locally. | Re-enter if a dependency audit replaces the local copy with a vendored dependency boundary or stops maintaining this source in-tree. |
| `pandaPI/engine/src/symbolic_search/cudd-3.0.0/` | vendored third-party | Bundled CUDD source for BDD path; current engine contract fences BDD/CUDD as future work. | Exclude from first-party gates; re-enter through BDD/CUDD support and dependency audit. |
| `build/<platform>/source/*` | copied build artifact | Disposable source copies allow build-time generated files and local mutation. | Exclude from ownership counts; map diagnostics back to original source or generated output. |
| `build/*`, `dist/*`, release package work directories | generated build output | Build/test/release products. | Exclude from source-quality gates; retain as evidence where relevant. |

## Existing Gate Map

| Gate family | Current Make target(s) | Current selector | Parser | Grounder | Engine | Runtime |
|-------------|------------------------|------------------|--------|----------|--------|---------|
| format | `make format-check`, `make format` | `OWNED_CPP_DIRS := pandaPI/runtime/include pandaPI/runtime/src pandaPI/runtime/tests` | gap for first-party maintained parser and generator/template policy | gap for first-party maintained grounder and generator/template policy | gap for first-party maintained engine and generator/template policy | covered for first-party maintained runtime |
| static analysis | `make static-analysis`, `make static-analysis-cpp` | shell syntax/checks plus `clang-tidy`/`clang-analyzer-*` over runtime compile database | gap until compile database isolates parser first-party/generated paths | gap until compile database isolates grounder first-party/dependency paths | gap until compile database isolates engine first-party/CUDD paths | covered by runtime CMake compile database |
| coverage | `make coverage` | Clang source coverage for runtime CTest executables, ignoring parser/grounder/engine and build/dist paths | gap until managed-fixture profiles isolate parser first-party files | gap until managed-fixture profiles isolate grounder first-party files from dependency internals | gap until managed-fixture profiles isolate engine first-party files from CUDD/optional builds | covered baseline at runtime scope |
| warning | `make warning-inventory` | full `make build` log classified manually by warning inventory | primary warnings currently clear; generated Flex warnings budgeted | primary warnings currently clear; dependency-internal warnings budgeted | one primary `visited_list.cpp` warning budget plus linker warning budget | zero-warning current surface |
| sanitizer | `make sanitize-runtime`, `make test-runtime-sanitize`, `make sanitize-binaries`, `make test-binary-sanitize` | runtime ASan/UBSan CTest; binary ASan/UBSan managed fixtures over canonical `pandapi-*` binaries | binary sanitizer exercised through managed fixtures; source class triage still manual | binary sanitizer exercised through managed fixtures; dependency findings need separate triage | binary sanitizer exercised through managed fixtures; optional/fenced code not representative unless built | runtime sanitizer covered directly |
| unit/seam tests | runtime CTest executables, optional Catch2 smoke | runtime CMake tests only | gap for parser internal seams | gap for grounder internal seams | gap for engine internal seams, especially `visited_list` and search/data structures | covered by runtime CTest smoke/unit-style tests |
| contract tests | `make test-contract*`, `make test`, `make smoke`, `make smoke-negative` | black-box baseline and managed `pandapi-*` fixtures | covered for process contract, not internal source quality | covered for process contract, not internal source quality | covered for process contract, not internal source quality | indirectly exercised by binaries plus runtime tests |

## Gap Map

| Gap | Affected class/component | Consequence | Owning follow-up |
|-----|--------------------------|-------------|------------------|
| Canonical first-party selectors do not exist for parser, grounder, and engine. | first-party maintained and generator/template paths | Later gates would either miss product source or include generated/dependency noise. | Slice02, then Slice03. |
| Compile databases are runtime-specific. | parser, grounder, engine | `clang-tidy` cannot honestly expand to first-party component files yet. | Slice02 build-surface normalization. |
| Coverage profiles are runtime-specific and managed binary coverage would mix copied source trees with generated/dependency files. | parser, grounder, engine | No honest per-component first-party coverage baseline/floor can be set today. | Slice02 profile isolation, then Slice03 baseline target scaffold. |
| Format policy is runtime-specific. | parser, grounder, engine first-party maintained code | Arc07 cannot claim first-party formatting beyond runtime yet. | Slice03 target scaffold after Slice02 selectors. |
| Generated-output policy is descriptive, not automated. | Flex/Bison/gengetopt outputs and templates | Strict warnings/format could fail on third-party skeleton output or hide real template defects. | Slice02 classify selectors; Slice04+ generated-code burndown if selected. |
| Warning policy has budgets but no first-party zero-warning release gate across all components. | parser, grounder, engine, runtime | Current warning evidence is manual inventory, not enforced per source class. | Slice03 gate scaffold and component burndown slices. |
| Binary sanitizer source-class triage is manual. | parser, grounder, engine, dependencies | Failures may require manual mapping from copied build paths to original source. | Slice02 source-copy/report normalization. |
| Internal unit/seam tests exist for runtime only. | parser, grounder, engine | Algorithm/data-structure bugs can be hidden behind broad process fixtures. | Slice03 Catch2/CTest posture; Slice04+ component burndown. |
| Engine optional/fenced first-party subdirectories have unclear gate staging. | `sat/`, `translation/`, `interactive_planner.*`, symbolic non-CUDD wrappers | Source-quality work could accidentally promote unsupported behavior if tied only to public fixtures. | Slice02 and Slice04+ should separate source-quality gates from supported-surface acceptance. |

## Recommendations

### Slice02 Restructure

Recommendation: run Slice02 as a real source-layout and build-surface
normalization slice before gate expansion.

Minimum Slice02 outputs:

- canonical Make variables for first-party maintained paths, first-party
  generator/templates, generated outputs, vendored third-party exclusions,
  copied build artifacts, and generated build output;
- per-component compile database creation for parser, grounder, and engine,
  with first-party selectors excluding generated and dependency paths;
- profile-output isolation for managed binary coverage so copied source paths
  map back to original component paths;
- generated-output policy encoded in selectors rather than prose only;
- no public-surface expansion for H2, cpddl/FAM, SAT, translation, interactive,
  or BDD/CUDD behavior.

### Make Target Families

Proposed target families after Slice02:

- `make format-check-first-party` and component forms for parser, grounder,
  engine, runtime;
- `make static-analysis-first-party` and component forms using compile
  databases with source-class filters;
- `make coverage-first-party` as an aggregate over component baselines, with
  runtime retaining its existing target during transition;
- `make warning-policy` or `make warning-inventory-first-party` that reports
  first-party warnings separately from generated/dependency budgets;
- `make test-unit` and component forms for CTest/Catch2 seam tests;
- `make sanitize-first-party` as a triage/reporting wrapper over runtime and
  binary sanitizer outputs by source class.

### Catch2 And Test Dependency Posture

Recommendation: promote Catch2 to a pinned test-only dependency if Arc07 adds
parser, grounder, or engine internal C++ tests. Runtime already has a Catch2
smoke path, and Arc02 accepted Catch2 as test-only seam coverage paired with
black-box process fixtures. Keep process-contract conformance in the existing
fixture harness; use Catch2/CTest for internal source seams such as parser AST
helpers, grounder output/model helpers, and engine data structures.

### Warning, Static, And Format Policy

Recommendation:

- first-party maintained code should trend to zero warnings and zero required
  static-analysis findings;
- runtime keeps its current zero-warning and static-analysis posture;
- parser and grounder primary warnings remain zero for observed warning
  inventory, with parser generated Flex warnings budgeted separately;
- engine `visited_list.cpp` remains a named primary warning budget until a
  targeted seam/fixture makes the payload representation rewrite testable;
- generated output is fixed at the first-party generator/template when the
  defect belongs to the grammar/spec, and excluded or suppressed when the
  finding is third-party skeleton noise;
- vendored third-party warnings stay separately reported unless a dependency
  audit accepts that boundary.

### Coverage Baseline And Floor Strategy

Recommendation:

- do not set a global release coverage floor until Slice02 isolates
  first-party component profiles;
- keep the current runtime coverage report as the existing baseline evidence;
- add per-component measured baselines first, then decide release floors after
  at least one component burndown slice proves meaningful unit/seam signal;
- exclude generated build output, copied build trees, vendored third-party
  source, and third-party generated output from first-party coverage totals;
- report generated and dependency coverage separately only if a later audit
  accepts that boundary.

### Component Slice Order

Recommended order:

1. Slice02 source-layout/build-surface normalization.
2. Slice03 first-party gate scaffold and selector encoding.
3. Runtime follow-up only for target integration regressions, because runtime
   already has the strongest gates.
4. Parser first-party maintained source plus generator/template policy, because
   parser has the smallest component surface and clear Flex/Bison/gengetopt
   boundaries.
5. Grounder first-party maintained source and integration adapters, with
   cpddl/H2/nested dependency findings separately reported.
6. Engine first-party maintained normal-search/data-structure source,
   including the `visited_list.cpp` warning budget.
7. Engine optional/fenced first-party subdirectories only if selectors can test
   them without promoting unsupported public behavior.

## Operator Decisions And CDC Decisions Before Slice02

- Operator decision: accept source-class taxonomy and require Slice02
  restructuring before gate expansion.
- Operator decision: decide whether Slice02 may add or revise Make selectors
  and compile-database/profile build directories, without changing binary
  behavior.
- Operator decision: decide whether Catch2 becomes a pinned test dependency
  for parser, grounder, and engine seam tests, or remains optional until
  Slice03.
- Operator decision: choose release threshold posture: measured baselines
  first, release blocker coverage floor later, and zero first-party warnings
  or static-analysis findings where selectors exist.
- CDC decision: verify whether the path taxonomy is complete enough for
  Slice02, especially copied build artifacts under `build/<platform>/source/*`
  and generated outputs in source copies.
- CDC decision: confirm that excluding cpddl, H2, CUDD, and nested third-party
  generated code from first-party coverage/static/unit obligations is accepted
  unless a dependency audit re-enters.
- CDC decision: confirm that first-party source-quality gates may cover
  optional/fenced engine source without treating SAT, translation, interactive,
  or BDD/CUDD as supported public behavior.

## Arc07 Handoff

The current arc-plan already contains a conditional Slice02 for
source-layout and build-surface normalization. No amendment is required before
Slice02 if the operator accepts this report's recommendation to activate that
slice with selectors, compile databases, profile isolation, and generated-code
exclusions as the scope. Amend the arc-plan only if CDC/operator want a
different Slice02 shape or want to split selector work from compile/profile
work.
