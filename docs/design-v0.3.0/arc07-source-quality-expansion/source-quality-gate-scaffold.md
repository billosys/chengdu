# Arc07 Source-Quality Gate Scaffold

Status: Slice06 implementation report

## Scaffold Policy

This slice adds Make-backed first-party source-quality gate scaffold targets. It does not clear parser, grounder, or engine debt, does not create component enforcement thresholds, and sets no release floor for coverage. The scaffold reuses `SOURCE_QUALITY` selectors, `source-quality-surface`, `source-quality-profile-map`, `source-quality-naming-check`, and `compile-db-first-party` evidence.

Report-only rows are not empty green gates: each row records the current source class, target, generated evidence path, and re-entry condition needed before a later component burndown can make the gate executable.

## Source Class Routing

| Source class | Current routing |
|--------------|-----------------|
| first-party maintained | Parser, grounder, engine, and runtime product source selected for Arc07 format, static analysis, coverage, unit, warning, and sanitizer work. |
| first-party generated | Generated output from Chengdu-maintained grammars or option specs; defects are fixed at the generator/template when the finding belongs there. |
| first-party generator/template | Flex, Bison, gengetopt, CMake, or Make inputs owned by Chengdu and reviewed as source. |
| GENERATED | Generated output excluded from broad first-party maintained selectors until a later ledger accepts a strict generated-code policy. |
| THIRD_PARTY | Vendored third-party source excluded from first-party obligations unless a dependency audit accepts the boundary. |
| DEPENDENCY | Dependency-internal source reported separately from first-party maintained files. |
| COPIED_BUILD | Build-time copied source mapped back to canonical `pandaPI/<component>` roots before quality evidence is interpreted. |
| BUILD_OUTPUT | Build, dist, CMake, CTest, coverage, sanitizer, and release artifacts used as evidence, not source. |

## Gate Family Matrix

| Gate family | Component | source class | Make target | Current status | Generated evidence path | re-entry condition |
|-------------|-----------|--------------|-------------|----------------|-------------------------|--------------------|
| format | runtime | first-party maintained | `format-check-first-party` -> `format-check` | Executable through the existing runtime clang-format gate. | `build/source-quality/<platform>/format-check-first-party.md` | Runtime already executable; later slices may split `format-check-runtime` if component forms become useful. |
| format | parser | first-party maintained and first-party generator/template | Intended `format-check-parser`; aggregate `format-check-first-party` today | Report-backed re-entry; broad formatting would need component selectors and generator/template policy first. | `build/source-quality/<platform>/format-check-first-party.md` | Add parser clang-format selector that excludes generated output and third-party skeleton text. |
| format | grounder | first-party maintained and dependency exclusions | Intended `format-check-grounder`; aggregate `format-check-first-party` today | Report-backed re-entry; dependency paths remain excluded. | `build/source-quality/<platform>/format-check-first-party.md` | Add grounder clang-format selector for maintained source while excluding cpddl, H2, and nested third-party paths. |
| format | engine | first-party maintained and vendored CUDD exclusion | `format-check-engine`; aggregate `format-check-first-party` still available | Executable baseline records current drift without changing inherited formatting. | `build/source-quality/<platform>/format-check-engine.md` | Convert recorded drift to strict formatting once the engine formatting burndown is accepted. |
| static analysis | runtime | first-party maintained | `static-analysis-first-party` -> `static-analysis-cpp` | Executable clang-tidy and clang-analyzer runtime gate, including explicit missing-tool SKIP behavior. | `build/source-quality/<platform>/static-analysis-first-party.md` | Runtime already executable; component forms can be added after first-party compile-db filtering is enforcement-ready. |
| static analysis | parser | first-party maintained plus generated exclusions | Intended `static-analysis-parser`; aggregate `static-analysis-first-party` today | Compile database evidence exists; clang-tidy enforcement waits to avoid generated skeleton noise and blanket suppression. | `build/source-quality/<platform>/compile-db/parser/first-party-selected.txt` | Run clang-tidy on parser selected translation units after generated output and generator/template handling are accepted. |
| static analysis | grounder | first-party maintained plus dependency exclusions | Intended `static-analysis-grounder`; aggregate `static-analysis-first-party` today | Compile database evidence exists; clang-tidy enforcement waits to avoid dependency-internal noise. | `build/source-quality/<platform>/compile-db/grounder/first-party-selected.txt` | Run clang-tidy on grounder maintained units after cpddl/H2 exclusion policy is enforced by the target. |
| static analysis | engine | first-party maintained plus CUDD exclusion | `static-analysis-engine`; aggregate `static-analysis-first-party` still available | Executable clang-tidy and clang-analyzer baseline over `compile-db-engine` selected units; findings are reported, not blanketed. | `build/source-quality/<platform>/static-analysis-engine.md` | Decide which analyzer findings become strict after the engine baseline is accepted. |
| coverage | runtime | first-party maintained | `coverage-first-party` -> `coverage` | Executable runtime Clang source coverage baseline. | `build/coverage/runtime/<platform>/report/runtime-coverage-summary.txt` | Runtime remains the only measured baseline today; no release floor is set. |
| coverage | parser | first-party maintained and generated exclusions | Intended `coverage-parser`; aggregate `coverage-first-party` today | Profile/source map exists; managed binary profiles are not isolated enough for a component baseline yet. | `build/source-quality/<platform>/source-map.tsv` | Collect parser unit or managed-fixture profiles that map copied-build paths back to canonical parser source. |
| coverage | grounder | first-party maintained plus dependency exclusions | Intended `coverage-grounder`; aggregate `coverage-first-party` today | Profile/source map exists; dependency internals must not count as first-party baseline. | `build/source-quality/<platform>/source-map.tsv` | Collect grounder profiles with cpddl, H2, nested third-party, generated, and copied-build classes routed separately. |
| coverage | engine | first-party maintained plus CUDD exclusion | `coverage-engine`; aggregate `coverage-first-party` still available | Executable visited_list seam coverage baseline with generated cmdline, CUDD, copied-build, and BUILD_OUTPUT routing. | `build/coverage/engine/<platform>/report/engine-coverage-summary.txt` | Expand to additional engine seams or managed fixture profiles only with source-class mapping; no release floor. |
| unit | runtime | first-party maintained | `test-unit` -> `test-runtime` | Executable runtime CTest suite. | `build/source-quality/<platform>/test-unit.md` | Runtime already executable. |
| unit | parser | first-party maintained and generator/template | Intended `test-unit-parser`; aggregate `test-unit` today | Report-backed re-entry; process contract fixture evidence is black-box, not unit or seam coverage. | `build/source-quality/<platform>/test-unit.md` | Add parser CTest or Catch2 seam tests after internal seams and dependency posture are accepted. |
| unit | grounder | first-party maintained | Intended `test-unit-grounder`; aggregate `test-unit` today | Report-backed re-entry; contract fixture coverage remains process-level. | `build/source-quality/<platform>/test-unit.md` | Add grounder CTest or Catch2 seams for maintained model/output code. |
| unit | engine | first-party maintained | `test-unit-engine`; aggregate `test-unit` still available | Executable CTest seam for visited_list payload representation; Catch2 remains optional. | `build/source-quality/<platform>/test-unit-engine.md` | Add more engine CTest or Catch2 seams after risk-bearing internals are selected. |
| warning | runtime | first-party maintained | `warning-inventory-first-party` -> `warning-inventory` | Executable warning inventory with runtime zero-warning status recorded. | `build/warnings/<platform>/warning-inventory.txt` | Runtime remains zero-warning. |
| warning | parser | first-party warning and generated warning classes | `warning-inventory-first-party`; `generated-warning-triage-parser` | Executable inventory; primary parser warnings clear, and Slice06 closes the Flex `yyunput`/`yyinput` generated-warning budget through scanner input options. | `build/source-quality/<platform>/generated-warning-triage-parser.md` | Re-enter if scanner rules begin using `input()`/`unput()` or parser generated output becomes a strict generated-code gate. |
| warning | grounder | first-party warning, generated, dependency, third-party classes | `warning-inventory-first-party` | Executable inventory; primary warnings clear, dependency budgets remain separate. | `build/source-quality/<platform>/warning-inventory-first-party.md` | Dependency audit or support decision required before dependency warning budget becomes release blocker. |
| warning | engine | first-party warning plus copied-build path mapping | `warning-inventory-engine`; aggregate `warning-inventory-first-party` still available | Executable engine build inventory records visited_list payload representation status and duplicate link-library status. | `build/source-quality/<platform>/warning-inventory-engine.md` | If duplicate linker diagnostics return, classify libheuristics/libhrc ownership before strict warning policy. |
| generated-warning | parser | first-party generator/template plus third-party skeleton | `generated-warning-triage-parser`; aggregate `generated-warning-triage` still available | Parser-specific root-cause evidence records `hddl-token.cpp`, `hddl-token.l`, Flex `yyunput`/`yyinput`, `options.ggo`, `cmdline.*`, correctness, chosen remedy, and re-entry. | `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md` | Keep gengetopt disposition separate from scanner cleanup; re-enter for strict generated-code policy or option-spec replacement. |
| generated-warning | grounder | first-party generator/template, dependency-internal, third-party generated | `generated-warning-triage` | Make-backed triage records separate gengetopt and dependency classes. | `build/source-quality/<platform>/generated-warning-triage.md` | Accept component or dependency remediation slice before changing generated output. |
| generated-warning | engine | first-party generator/template and generated output | `generated-warning-triage` | Make-backed triage records route gengetopt output to option source or CLI11 migration. | `build/source-quality/<platform>/generated-warning-triage.md` | Accept CLI11 or generated-code policy slice before strict enforcement. |
| sanitizer | runtime | first-party maintained | `sanitize-first-party` -> `sanitize-runtime` | Executable ASan/UBSan runtime sanitizer gate. | `build/source-quality/<platform>/sanitize-first-party.md` | Runtime ASan/UBSan already executable. |
| sanitizer | parser | first-party maintained, generated, copied-build | `sanitize-first-party`; existing binary gate `test-binary-sanitize` | Runtime executable in scaffold; binary sanitizer remains existing local/pre-release gate with source class mapping. | `build/source-quality/<platform>/sanitize-first-party.md` | Run binary sanitizer when parser, adapter, or generated paths change; map findings back to source class. |
| sanitizer | grounder | first-party maintained, dependency, third-party, copied-build | `sanitize-first-party`; existing binary gate `test-binary-sanitize` | Runtime executable in scaffold; binary sanitizer remains existing local/pre-release gate with dependency triage. | `build/source-quality/<platform>/sanitize-first-party.md` | Re-enter on Linux LSan or dependency findings; classify cpddl/H2 separately. |
| sanitizer | engine | first-party maintained, dependency, copied-build | `sanitize-engine`; `sanitize-first-party` and `test-binary-sanitize` still available | Executable ASan/UBSan visited_list seam; binary sanitizer remains existing local/pre-release gate with optional/fenced source routing. | `build/source-quality/<platform>/sanitize-engine.md` | Re-enter when ASan, UBSan, LSan, or TSan evidence points to broader engine first-party code or supported concurrency appears. |

## Slice05 Engine Burndown Status

- Engine first-party maintained source is selected through `compile-db-engine`, `source-quality-profile-map`, and `source-quality-naming-check`.
- Generated `cmdline.c`/`cmdline.h`, vendored `CUDD`, copied build roots, `BUILD_OUTPUT`, `dist`, and `release` roots remain excluded from engine first-party gate interpretation.
- `format-check-engine`, `static-analysis-engine`, `coverage-engine`, `warning-inventory-engine`, `test-unit-engine`, and `sanitize-engine` are named Make targets with durable reports.
- `test-unit-engine` and `coverage-engine` exercise the `visited_list` payload representation seam through CTest; managed contract fixtures remain process-level contract evidence.
- `warning-inventory-engine` records the `visited_list` payload representation status and the duplicate link-library status for libheuristics/libhrc re-entry.
- `coverage-engine` records a measured baseline only; no release floor.
- `sanitize-engine` records ASan and UBSan seam evidence plus LSan, TSan, binary, source class, copied, dependency, and re-entry policy.

## Generated-Warning Triage

Each generated-warning record must include these fields before a warning is closed, suppressed, or deferred: generated file, generator/template or third-party skeleton, ownership class, root cause, correctness, chosen remedy, and re-entry.

| generated file | generator or skeleton | ownership class | root cause | correctness | chosen remedy | re-entry |
|----------------|-----------------------|-----------------|------------|-------------|---------------|----------|
| `pandaPI/parser/src/hddl-token.cpp` | `pandaPI/parser/src/hddl-token.l` through Flex skeleton | first-party generator/template plus third-party skeleton output | Flex emits `yyunput` and `yyinput` helpers not used by the current scanner rule set. | Current managed parser fixtures indicate scanner semantics are correct for accepted inputs; this is unused helper code, not known tokenization drift. | Prefer generator/template or Flex option remediation if a strict generated-warning gate is accepted; otherwise document skeleton exclusion with rationale. | Parser generated warnings become strict, or parser scanner behavior changes. |
| `pandaPI/parser/src/cmdline.c`, `cmdline.h` | `pandaPI/parser/src/options.ggo` through gengetopt skeleton | first-party generator/template plus third-party generated output | gengetopt emits helper code and human option prose outside the managed-process contract layer. | Managed native parser wrapper owns current process behavior; generated human diagnostic prose is not final contract evidence. | Fix option semantics in `options.ggo` or migrate to the accepted CLI11 path in a later component slice. | CLI11 migration or strict generated-code policy is accepted. |
| `pandaPI/grounder/src/cmdline.c`, `cmdline.h` | `pandaPI/grounder/src/options.ggo` through gengetopt skeleton | first-party generator/template plus third-party generated output | gengetopt emits legacy helper code around inherited option parsing. | Managed native grounder wrapper owns current process behavior; generated prose is not final contract evidence. | Fix option source or native wrapper, not emitted generated output by hand. | CLI11 migration or strict generated-code policy is accepted. |
| `pandaPI/engine/src/cmdline.c`, `cmdline.h` | `pandaPI/engine/src/options.ggo` through gengetopt skeleton | first-party generator/template plus third-party generated output | gengetopt emits legacy helper code around inherited option parsing. | Managed native engine wrapper owns current process behavior; generated prose is not final contract evidence. | Fix option source or native wrapper, not emitted generated output by hand. | CLI11 migration or strict generated-code policy is accepted. |
| `pandaPI/grounder/h2-fd-preprocessor/src/h2_mutexes.cc` | H2 dependency source | dependency-internal | Build warning budget records unused counters in dependency-internal H2 code. | No accepted default managed behavior depends on H2 as a source-quality surface. | Keep separate dependency budget pending H2 audit or support decision. | H2 becomes default behavior or dependency audit accepts ownership. |
| `pandaPI/grounder/cpddl/third-party/lpsolve` generated files | lp_solve Flex/Bison and third-party skeleton output | third-party generated inside vendored dependency | Build warning budget records dependency and skeleton warning classes. | Treated as dependency evidence, not first-party semantic proof. | Exclude or suppress only with dependency rationale in a later audit. | Dependency audit accepts cpddl/lpsolve warning obligations. |

## Slice06 Parser Generated-Warning Status

- `generated-warning-triage-parser` writes parser-specific evidence to `build/source-quality/<platform>/generated-warning-triage-parser.md` and `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md`.
- Flex scanner warnings for `yyunput` and `yyinput` are root-caused to default skeleton helper emission.
- `hddl-token.l` now declares `%option nounput` and `%option noinput`; accepted scanner rules do not call `unput()` or `input()`.
- `options.ggo` and generated `cmdline.c`/`cmdline.h` are dispositioned separately from the Flex fix, with managed native parser behavior preserved.

## Optional IPC Corpus Placement

`make test-corpus CORPUS_DIR=...` remains optional external corpus smoke. It is useful for manual IPC workload sampling, but it is not unit coverage, not a release-grade benchmark gate, and not a substitute for CTest/Catch2 seam tests or managed contract fixture evidence.

Re-entry for broader IPC corpus work requires a later ledger to accept corpus ownership, deterministic fixture selection, expected runtime bounds, benchmark policy, and whether the target belongs to source-quality, process fixtures, or release qualification.

## Post-Slice05 Bubble-Up

Slice05 completed the first engine component burndown foothold: named engine
targets exist, the visited_list payload representation seam is tested, the
primary payload-cast warning is closed, and duplicate link-library noise is not
observed in the focused engine warning inventory.

Recommended next slice: parser generated-warning/root-cause work. Parser primary
warnings are clear, but Flex/gengetopt generated-warning policy still needs
generator/skeleton correctness decisions before strict enforcement.

Engine remediation re-entry: expand beyond the visited_list seam only if CDC
finds residual engine warning debt, static-analysis findings selected for
strict treatment, or additional risk-bearing engine internals that need CTest
or Catch2 seams.
