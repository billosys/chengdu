# Arc06 Fixture Gap Inventory

Date: 2026-08-11

## Current Fixture Inventory

This inventory reflects the current `release/0.3.x` surface after Arc05
closed. The public fixture-list entry points are Make targets:

- `make test-contract-list`
- `make test-contract-list-managed`

Current source paths are `pandaPI/parser`, `pandaPI/grounder`,
`pandaPI/engine`, and `pandaPI/runtime`.

### Baseline

Baseline fixtures remain broad smoke-style observations. They do not by
themselves prove managed-process conformance unless a component's managed
contract records also cover the same behavior.

| Component | Count | Fixture IDs |
|-----------|-------|-------------|
| parser | 4 | `parser-success-minimal`, `parser-missing-file`, `parser-broken-syntax`, `parser-broken-reference` |
| grounder | 2 | `grounder-success-minimal`, `grounder-missing-input` |
| engine | 2 | `engine-solved-minimal`, `engine-unsolvable` |
| pipeline | 1 | `pipeline-minimal` |
| total | 9 | |

### Managed Contract

Managed Contract fixtures prove the accepted Arc03/Arc05 process contract for
canonical native `pandapi-*` binaries. Arc06 Slice02 adds the first managed
pipeline fixture set and positive supervised workload.

| Component | Count | Fixture IDs |
|-----------|-------|-------------|
| parser | 12 | `parser-canonical-file-success`, `parser-canonical-stdout-success`, `parser-info-commands`, `parser-cli-usage-errors`, `parser-input-unavailable`, `parser-input-invalid`, `parser-output-unavailable`, `parser-final-status-fields`, `parser-status-stream-legality`, `parser-color-policy`, `parser-legacy-surface-fencing`, `parser-supervised-positive` |
| grounder | 14 | `grounder-canonical-file-success`, `grounder-canonical-stdout-success`, `grounder-info-commands`, `grounder-cli-usage-errors`, `grounder-input-unavailable`, `grounder-input-invalid`, `grounder-output-unavailable`, `grounder-final-status-fields`, `grounder-status-stream-legality`, `grounder-color-policy`, `grounder-h2-surface-fencing`, `grounder-cpddl-fam-surface-fencing`, `grounder-domain-no-plan`, `grounder-supervised-positive` |
| engine | 17 | `engine-canonical-file-success`, `engine-canonical-stdout-success`, `engine-info-commands`, `engine-cli-usage-errors`, `engine-input-unavailable`, `engine-input-invalid`, `engine-output-unavailable`, `engine-final-status-fields`, `engine-status-stream-legality`, `engine-color-policy`, `engine-domain-no-plan`, `engine-internal-driver-sentinel-fencing`, `engine-interactive-surface-fencing`, `engine-translation-surface-fencing`, `engine-sat-surface-fencing`, `engine-bdd-cudd-surface-fencing`, `engine-supervised-positive` |
| pipeline | 4 | `pipeline-managed-file-chain`, `pipeline-parser-stdout-to-grounder`, `pipeline-grounder-stdout-to-engine`, `pipeline-supervised-positive` |
| total | 47 | |

## Current Smoke And Pipeline Coverage

| Gate | Current coverage | Strength | Gap |
|------|------------------|----------|-----|
| `make smoke` | Builds all three binaries, parses minimal HDDL, grounds the parsed artifact, runs engine search, checks solved plan artifact and engine status. | local and CI through `make test` and CI aggregate targets | Only one positive pipeline shape; no managed pipeline fixture record with per-step Arc03 matrix mapping. |
| `make smoke-negative` | Missing input, broken syntax, broken reference, and valid unsolvable search path. | local and CI through `make test` and CI aggregate targets | Negative path is smoke-grade; it does not cover every managed pipeline failure ownership boundary. |
| `make test-corpus CORPUS_DIR=...` | Optional Transport `pfile01` parse-ground-solve loop when an external corpus checkout exists. | local optional | Not CI-owned and not a release gate. |
| `make test-contract-pipeline` | Baseline `pipeline-minimal` only. | local and CI through `make test-contract` | Baseline-only; managed pipeline contract coverage lives in `make test-contract-pipeline-managed`. |
| `make test-contract-pipeline-managed` | Managed file-backed parse-ground-solve, pairwise stdout artifact composition, and supervised parse-ground-solve. | local and CI through `make test` after Slice02 | Representative managed pipeline coverage for coverage and sanitizer follow-on slices; deeper negative pipeline ownership remains future work. |

## Arc03 Matrix Coverage

| Arc03 category | Current fixture IDs | Coverage assessment | Owner |
|----------------|---------------------|---------------------|-------|
| CLI usage | `parser-cli-usage-errors`, `grounder-cli-usage-errors`, `engine-cli-usage-errors` | Managed coverage exists for missing args, unknown option, malformed color, too many positionals, output conflict, and status/stdout conflicts. | no-op for Slice02 first batch |
| Informational commands | `parser-info-commands`, `grounder-info-commands`, `engine-info-commands` | Managed `--help`, `--version`, and `--provenance` probes exist for canonical commands. | Arc07 for public prose; Arc08 for release-asset provenance proof |
| Supported positive file-backed artifacts | `parser-canonical-file-success`, `grounder-canonical-file-success`, `engine-canonical-file-success`, `pipeline-managed-file-chain` | Managed per-component coverage exists, and Slice02 adds representative managed pipeline parse-ground-solve coverage. | covered by Slice02 |
| Supported positive stdout-backed artifacts | `parser-canonical-stdout-success`, `grounder-canonical-stdout-success`, `engine-canonical-stdout-success`, `pipeline-parser-stdout-to-grounder`, `pipeline-grounder-stdout-to-engine` | Managed per-component coverage exists for artifact ownership and status-on-stderr legality; Slice02 proves pairwise downstream acceptance after stdout capture to a file. | covered by Slice02 |
| input unavailable | `parser-input-unavailable`, `grounder-input-unavailable`, `engine-input-unavailable`; baseline `parser-missing-file`, `grounder-missing-input` | Managed missing-input coverage exists; unreadable-input variants are not broadly covered because they can be platform-sensitive. | Arc06 Slice02 |
| input invalid | `parser-input-invalid`, `grounder-input-invalid`, `engine-input-invalid`; baseline `parser-broken-syntax`, `parser-broken-reference` | Managed malformed-input coverage exists; deeper model-section/token variants remain sparse. | Arc06 Slice02 |
| output unavailable | `parser-output-unavailable`, `grounder-output-unavailable`, `engine-output-unavailable` | Managed missing-parent and output-directory coverage exists; write/flush/finalize failure coverage remains limited. | Arc06 Slice02 |
| status stream | `parser-status-stream-legality`, `grounder-status-stream-legality`, `engine-status-stream-legality`, `parser-final-status-fields`, `grounder-final-status-fields`, `engine-final-status-fields`, `pipeline-managed-file-chain`, `pipeline-supervised-positive` | Managed final status and legal stream coverage exists per component, and Slice02 proves stderr final-status composition across a managed pipeline. | covered by Slice02 |
| color | `parser-color-policy`, `grounder-color-policy`, `engine-color-policy` | Managed `--no-color`, `--no-colour`, `NO_COLOR`, and illegal `--color=always` with machine-owned streams are covered. TTY/auto/never human-presentation cases are not broad. | Arc06 Slice02 |
| surface fencing | `parser-legacy-surface-fencing`, `grounder-h2-surface-fencing`, `grounder-cpddl-fam-surface-fencing`, `engine-internal-driver-sentinel-fencing`, `engine-interactive-surface-fencing`, `engine-translation-surface-fencing`, `engine-sat-surface-fencing`, `engine-bdd-cudd-surface-fencing` | Visible non-supported surfaces are fenced as legacy, experimental, unsupported, future, or implementation-internal CLI usage failure. | no-op unless a future accepted surface changes |
| engine no-plan | `engine-domain-no-plan`, baseline `engine-unsolvable`, `make smoke-negative` unsolvable case | Managed no-plan classification exists for engine. | no-op for Slice02 first batch |
| grounder no-plan | `grounder-domain-no-plan` | Recorded as not reachable through accepted grounder-only semantics. | no-op; Re-entry: accepted grounder behavior proves no usable grounded model without engine search |
| timeout/resource/signal | engine harness timeout around search; smoke harness timeout around engine step | Harness safety exists, but there is no managed binary-owned `timeout`, `resource_limit`, `interrupted`, or signal fixture set. | Arc06 Slice02 and Arc06 Slice07 |
| dependency failure / child process failure | Optional/fenced surfaces avoid promoting dependency paths. | No supported dependency-failure fixture currently exists because no supported default path depends on optional external runtime dependencies. | no-op; Re-entry: a future supported dependency or child-process boundary is accepted |
| pipeline composition | baseline `pipeline-minimal`, `make smoke`, `make smoke-negative`, `pipeline-managed-file-chain`, `pipeline-parser-stdout-to-grounder`, `pipeline-grounder-stdout-to-engine`, `pipeline-supervised-positive` | Baseline and smoke coverage exists, and Slice02 adds representative managed pipeline, pairwise artifact, and supervised composition coverage. | covered by Slice02; deeper negative pipeline ownership remains for later if needed |
| release package executable behavior | none in current fixture set | Arc06 local source fixtures do not prove packaged release assets. | Arc08 |
| wolong fetch/install/migration | none in current fixture set | Explicitly outside Arc06 Slice01. | Arc08 |

## Missing Or Deferred Obligations

| Gap | Owner | Rationale | Re-entry |
|-----|-------|-----------|----------|
| Managed parser-grounder-engine pipeline fixture with final status on stderr at every step. | covered by Slice02 | `pipeline-managed-file-chain` proves a representative file-backed managed pipeline. | Re-enter only for additional pipeline domains or negative-path ownership boundaries. |
| Pairwise composition variants: parser stdout artifact to grounder input file, grounder stdout artifact to engine input file, and file-backed equivalents. | covered by Slice02 | `pipeline-parser-stdout-to-grounder` and `pipeline-grounder-stdout-to-engine` prove pairwise artifact acceptance after stdout capture without inventing stdin behavior. | Re-enter only for stdin streaming if a future accepted contract adds it. |
| `--supervised` positive-path fixtures for parser, grounder, and engine. | covered by Slice02 | `parser-supervised-positive`, `grounder-supervised-positive`, and `engine-supervised-positive` provide CI-safe per-component positive supervised cases; `pipeline-supervised-positive` covers the representative chain. | Re-enter for negative supervised interruption/resource cases in Slice07 if supported behavior exists. |
| TTY/color positive cases for `--color=auto`, `--color=never`, and human-stderr-only ANSI policy. | deferred after Slice07 | Current color fixtures focus on machine-stream ANSI absence and illegal color ownership; Slice02 adds supervised ANSI-free positives. Slice07 does not add a TTY simulator because no deterministic CI-safe TTY/no-TTY predicate exists today. | Re-entry: add only if the harness can create stable TTY and no-TTY observations on the supported Linux/macOS CI matrix. |
| Unreadable-input fixtures where the platform can create them safely. | deferred after Slice07 | Missing-input coverage exists; permission-based unreadability can be platform-sensitive on local/CI hosts, especially with owner/root/container differences. Slice07 does not add a platform-sensitive fixture. | Re-entry: add only with a deterministic platform predicate or skip classification that reproduces on supported CI hosts. |
| Write, flush, and finalize output-failure fixtures. | deferred after Slice02 | Missing parent and output-directory coverage exists; lower-level write, flush, and finalization failure needs a deterministic failing filesystem/device or injectable writer support that is portable across CI hosts. | Re-entry: add a portable fixture mechanism for output-finalization or a platform-gated failing-output device with documented skip semantics. |
| Binary-owned timeout and resource-limit status fixtures. | deferred after Slice07 | Arc03 names the status classes, but the current binaries do not expose accepted timeout/resource options. Existing timeouts are harness-owned safety probes around engine search and smoke execution, not binary-owned status contracts. | Re-entry: add only if a supported timeout/resource option, resource-limit option, or harness-observable binary contract is accepted. |
| Coverage reporting for chengdu-owned runtime and adoption seams. | covered by Slice03 for runtime; adoption seam coverage deferred | `make coverage` now emits Clang source-based coverage reports for `pandaPI/runtime` source and tests under `build/coverage/runtime/<platform>/report/`, with generated, inherited, third-party, `build`, and `dist` paths excluded from the owned coverage claim. Adoption seams `pandapi_parser_native.cpp`, `pandapi_grounder_native.cpp`, and `pandapi_engine_native.cpp` are deferred because reliable managed-fixture profile collection would currently mix owned shims with copied inherited build trees and generated or third-party source. | Re-entry: instrument canonical `pandapi-*` binary builds so managed fixtures collect profiles for owned native adoption files while inherited/generated and third-party paths are excluded or separately reported. |
| `clang-tidy` / Clang Static Analyzer gate for owned C++ source. | covered by Slice04 for runtime; adoption seam static analysis deferred | `make static-analysis-cpp` now configures a Debug runtime compile database under `build/static-analysis/runtime/<platform>/build/` and runs `clang-tidy` with `clang-analyzer-*` over `pandaPI/runtime` source/test translation units and reached runtime headers. Inherited planner, generated, nested third-party, `build`, and `dist` paths are excluded from the owned gate. Adoption seams remain deferred because their compile commands still come from component-specific inherited build trees and copied source directories. | Re-entry: include `pandapi_parser_native.cpp`, `pandapi_grounder_native.cpp`, and `pandapi_engine_native.cpp` after canonical `pandapi-*` builds expose reliable owned-file compile commands while inherited/generated and third-party paths remain excluded or separately classified. |
| Compiler-warning inventory and burndown for parser, grounder, engine, runtime, generated code, and nested third-party code. | covered by Slice05 | Slice05 adds `make warning-inventory`, records 56 supported-platform warning lines, fixes low-risk primary hand-written warnings, and classifies remaining generated, dependency-internal, engine payload-cast, and duplicate-link warning budgets. | Re-entry: a sanitizer gate flags a budgeted path, Arc08 promotes a warning-clean primary-source policy to release blocker, or a dependency audit takes ownership of generated/nested dependency warnings. |
| ASan/UBSan/LSan process-fixture workloads beyond runtime. | covered by Slice06 for macOS ASan/UBSan; Linux LSan re-entry remains | `make test-binary-sanitize` builds an isolated sanitizer distribution under `build/binary-sanitize/<platform>/dist` and runs managed parser, grounder, engine, and pipeline fixtures through `CHENGDU_CONTRACT_DIST_DIR`. macOS LSan is skipped with `detect_leaks=0`. Slice07 places runtime ASan/UBSan in Linux always-on CI through `make ci-linux`, keeps binary ASan/UBSan local/pre-release or scheduled because it rebuilds every binary, and routes Linux LSan to pre-release once a LeakSanitizer-capable Linux job is available. | Re-entry: run binary sanitizer on Linux or another LeakSanitizer-capable toolchain with `detect_leaks=1`; promote failures in chengdu-owned code to release blockers and triage inherited/generated/dependency findings by Slice05 ownership tier. |
| TSan workload and CI disposition. | deferred after Slice07 | Current managed fixtures and runtime tests exercise process observation, supervised mode, harness timeouts, and stream ownership, but the C++ runtime implementation observes subprocesses with a single-threaded fork/pipe/waitpid polling loop. The shell harness uses background timeout processes outside TSan's shared-memory model. The only clear in-tree thread primitives found by Slice07 are in the engine SAT path, which the managed contract fences as unsupported. | Re-entry: add a Make-backed TSan target only after an accepted supported workload includes real multithreaded chengdu-owned process observation, a child-process adapter with concurrent stream draining, or promoted supported engine concurrency. |
| Behavior-change table, tutorial examples, and public migration prose. | Arc07 | Public docs/tutorial are outside Arc06 Slice01. | Arc07 consumes this inventory and Arc06 close evidence. |
| Release package dry-run, checksums, manifests, release licenses, and wolong migration proof. | Arc08 | Release publication and consumer installation proof are outside Arc06. | Arc08 consumes final Arc06 hardening evidence. |
| Inherited `pandaPI*` command compatibility probes from the original Arc03 matrix. | no-op | Arc05 closed with canonical `pandapi-*` commands only and explicitly rejected inherited command compatibility as accepted 0.3.0 behavior. | Re-entry requires an operator-approved compatibility policy change and release notice. |
| Grounder `domain_no_plan` as an executable grounder behavior. | no-op | No accepted grounder-only normal path currently classifies no-plan without engine search. | Re-entry requires accepted grounder semantics for no usable grounded model or unreachable goal. |
| Supported dependency-failure fixture for optional planner dependencies. | no-op | No supported default 0.3.0 path depends on optional SAT, BDD/CUDD, H2, cpddl/FAM, or translation subprocess behavior. | Re-entry requires promoting a dependency-backed path to supported behavior. |

## Make And CI Gate Map

| Gate | Proof class | Current status |
|------|-------------|----------------|
| `make test-contract-list` | Baseline fixture inventory listing | local; no build required |
| `make test-contract-list-managed` | Managed Contract fixture inventory listing | local; no build required |
| `make test-contract` | Baseline parser, grounder, engine, and pipeline fixtures | local and CI through `make test` |
| `make test-contract-parser-managed` | Managed parser process contract | local and CI through `make test` |
| `make test-contract-grounder-managed` | Managed grounder process contract | local and CI through `make test` |
| `make test-contract-engine-managed` | Managed engine process contract | local and CI through `make test` |
| `make test-contract-pipeline-managed` | Managed pipeline, pairwise composition, and supervised process workload | local and CI through `make test` after Slice02 |
| `make test` | Runtime CTest, baseline contract, managed component and pipeline contracts, positive smoke, and negative smoke | local and CI through `make ci-linux` and `make ci-macos` |
| `make smoke` | Positive parser-grounder-engine pipeline | local and CI through `make test` and `make readme-verbatim` |
| `make smoke-negative` | Missing input, invalid input, broken reference, and no-plan smoke | local and CI through `make test` |
| `make provenance-check` | Dist provenance records for built binaries | local and CI through `make ci-linux` and `make ci-macos` |
| `make actionlint` | GitHub Actions syntax | local and CI lint job |
| `make format-check` | Owned C++ formatting | local and CI through `make ci-linux` and `make ci-macos` |
| `make static-analysis` | Shell syntax, shellcheck, and owned runtime C++ static analysis aggregate | local; C++ subgate uses explicit missing-tool SKIP when `clang-tidy` is unavailable |
| `make safety-checks` | Workflow Make-entrypoint check, whitespace, nested Git metadata, gitlinks | local through `make check`; CI coverage is partial |
| `make test-binary-sanitize` | ASan/UBSan over canonical `pandapi-*` managed process fixtures | local; covered by Slice06; candidate scheduled/pre-release CI gate in Slice07 |
| `make ci-linux` | Linux CI aggregate | CI on ubuntu-22.04 and ubuntu-24.04 |
| `make ci-macos` | macOS CI aggregate | CI on macos-15 |
| `make readme-verbatim` | README source-build smoke | CI on ubuntu-22.04 and macos-15 |
| release workflow package/publish targets | Release publication | Arc08; not expanded by Arc06 Slice01 |
| `make coverage` | Runtime Clang source coverage | local developer and pre-release evidence; emits reports under ignored `build/coverage/runtime/<platform>/report/`; adoption seam coverage deferred until managed-fixture profile collection is isolated |
| `make static-analysis-cpp` | Runtime clang-tidy / Clang Static Analyzer | local and candidate CI quality gate; uses `clang-analyzer-*` through `clang-tidy` over the Debug runtime compile database; adoption seam static analysis deferred until owned native compile commands are isolated |
| `make warning-inventory` | compiler-warning inventory, warning ownership, and release budget | local and pre-release evidence; covered by Slice05; emits generated reports under `build/warnings/<platform>/` and leaves long-term budgets in `slice05-compiler-warning-burndown/warning-inventory.md` |
| `make sanitize-runtime` | Runtime ASan/UBSan CTest | always-on Linux CI through `make ci-linux`; local macOS supported through `make test-runtime-sanitize` |
| `make sanitize-binaries` | ASan/UBSan binary sanitizer build and managed fixture runner | local pre-merge for sanitizer-sensitive code and scheduled/pre-release CI candidate; covered by Slice06; writes isolated dist under `build/binary-sanitize/<platform>/dist`; macOS LSan skipped pending Linux/toolchain re-entry |
| TSan target | race detection | no target added in Slice07; deferred until a representative multithreaded chengdu-owned process-observation or supported planner-concurrency workload exists |

## Final Arc06 CI Placement Synthesis

| Gate family | Always-on CI | Scheduled/pre-release | Local developer | Deferred or re-entry |
|-------------|--------------|-----------------------|-----------------|----------------------|
| Contract and smoke behavior | `make ci-linux`, `make ci-macos`, `make readme-verbatim` | release-candidate reproduction of both CI aggregates | `make test`, targeted `make test-contract-*`, `make smoke`, `make smoke-negative` | packaged-release behavior remains Arc08 |
| Workflow and repository safety | `make actionlint` through the lint job; Make-entrypoint safety through `make check` locally | release branch safety reproduction | `make safety-checks`, `make actionlint` | none |
| Coverage | not always-on today | `make coverage` before release-candidate signoff | `make coverage` after runtime/process-policy changes | adoption-seam coverage after owned native profile collection is isolated |
| Static analysis | candidate promotion after CI toolchain package is pinned | `make static-analysis` before release-candidate signoff | `make static-analysis`, targeted `make static-analysis-cpp` | adoption-seam static analysis after owned native compile commands are isolated |
| Warning inventory | not always-on because it rebuilds and records generated output | `make warning-inventory` before release-candidate signoff | `make warning-inventory` after parser, grounder, engine, or build-flag changes | remaining generated/dependency and engine payload-cast budgets use Slice05 re-entry |
| Runtime ASan/UBSan | `make sanitize-runtime` through `make ci-linux` | reproduce on macOS if runtime/process-policy changed | `make test-runtime-sanitize` | none for supported runtime ASan/UBSan |
| Binary ASan/UBSan | not always-on today because it rebuilds parser, grounder, engine, and runs all managed fixtures | `make test-binary-sanitize` before release-candidate signoff and on a scheduled sanitizer job | `make test-binary-sanitize` for adapter, inherited engine, sanitizer, or fixture-runner changes | Linux LSan with `detect_leaks=1` before publication |
| TSan | none | none until re-entry condition is met | no target added | Re-entry: supported multithreaded process observation, concurrent child-process adapter, or supported planner concurrency workload |

GitHub Actions currently calls Make targets for build/test/lint/release support:
`make install-actionlint`, `make actionlint`, `make ci-linux`,
`make ci-macos`, `make readme-verbatim`,
`make restore-release-executable-bits`, `make package-release`, and
`make publish-release`.

## Recommended First Fixture-Expansion Batch For Slice02

1. Add managed pipeline contract fixtures for a canonical file-backed
   parse-ground-solve chain with `--status=stderr` on each component.
2. Add pairwise artifact-composition cases that prove parser stdout output is
   acceptable to grounder after capture, and grounder stdout output is
   acceptable to engine after capture, without inventing stdin support.
3. Add per-component positive `--supervised` cases for parser, grounder, and
   engine, asserting no ANSI and stable stdout/stderr ownership.
4. Add deterministic output-finalization failure cases if a portable fixture
   mechanism exists; otherwise record the exact unavailable platform condition.
5. Add deterministic TTY/color positive coverage only after the harness can
   create stable TTY and no-TTY observations.

This batch should land before coverage and sanitizer slices so later gates run
against representative process workloads instead of only the already-proved
minimal component cases.
