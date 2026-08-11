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
canonical native `pandapi-*` binaries. There is not yet a managed pipeline
fixture set.

| Component | Count | Fixture IDs |
|-----------|-------|-------------|
| parser | 11 | `parser-canonical-file-success`, `parser-canonical-stdout-success`, `parser-info-commands`, `parser-cli-usage-errors`, `parser-input-unavailable`, `parser-input-invalid`, `parser-output-unavailable`, `parser-final-status-fields`, `parser-status-stream-legality`, `parser-color-policy`, `parser-legacy-surface-fencing` |
| grounder | 13 | `grounder-canonical-file-success`, `grounder-canonical-stdout-success`, `grounder-info-commands`, `grounder-cli-usage-errors`, `grounder-input-unavailable`, `grounder-input-invalid`, `grounder-output-unavailable`, `grounder-final-status-fields`, `grounder-status-stream-legality`, `grounder-color-policy`, `grounder-h2-surface-fencing`, `grounder-cpddl-fam-surface-fencing`, `grounder-domain-no-plan` |
| engine | 16 | `engine-canonical-file-success`, `engine-canonical-stdout-success`, `engine-info-commands`, `engine-cli-usage-errors`, `engine-input-unavailable`, `engine-input-invalid`, `engine-output-unavailable`, `engine-final-status-fields`, `engine-status-stream-legality`, `engine-color-policy`, `engine-domain-no-plan`, `engine-internal-driver-sentinel-fencing`, `engine-interactive-surface-fencing`, `engine-translation-surface-fencing`, `engine-sat-surface-fencing`, `engine-bdd-cudd-surface-fencing` |
| pipeline | 0 | none |
| total | 40 | |

## Current Smoke And Pipeline Coverage

| Gate | Current coverage | Strength | Gap |
|------|------------------|----------|-----|
| `make smoke` | Builds all three binaries, parses minimal HDDL, grounds the parsed artifact, runs engine search, checks solved plan artifact and engine status. | local and CI through `make test` and CI aggregate targets | Only one positive pipeline shape; no managed pipeline fixture record with per-step Arc03 matrix mapping. |
| `make smoke-negative` | Missing input, broken syntax, broken reference, and valid unsolvable search path. | local and CI through `make test` and CI aggregate targets | Negative path is smoke-grade; it does not cover every managed pipeline failure ownership boundary. |
| `make test-corpus CORPUS_DIR=...` | Optional Transport `pfile01` parse-ground-solve loop when an external corpus checkout exists. | local optional | Not CI-owned and not a release gate. |
| `make test-contract-pipeline` | Baseline `pipeline-minimal` only. | local and CI through `make test-contract` | No managed pipeline contract fixtures yet. |

## Arc03 Matrix Coverage

| Arc03 category | Current fixture IDs | Coverage assessment | Owner |
|----------------|---------------------|---------------------|-------|
| CLI usage | `parser-cli-usage-errors`, `grounder-cli-usage-errors`, `engine-cli-usage-errors` | Managed coverage exists for missing args, unknown option, malformed color, too many positionals, output conflict, and status/stdout conflicts. | no-op for Slice02 first batch |
| Informational commands | `parser-info-commands`, `grounder-info-commands`, `engine-info-commands` | Managed `--help`, `--version`, and `--provenance` probes exist for canonical commands. | Arc07 for public prose; Arc08 for release-asset provenance proof |
| Supported positive file-backed artifacts | `parser-canonical-file-success`, `grounder-canonical-file-success`, `engine-canonical-file-success` | Managed per-component coverage exists. | no-op for Slice02 first batch |
| Supported positive stdout-backed artifacts | `parser-canonical-stdout-success`, `grounder-canonical-stdout-success`, `engine-canonical-stdout-success` | Managed per-component coverage exists for artifact ownership and status-on-stderr legality. | no-op for Slice02 first batch |
| input unavailable | `parser-input-unavailable`, `grounder-input-unavailable`, `engine-input-unavailable`; baseline `parser-missing-file`, `grounder-missing-input` | Managed missing-input coverage exists; unreadable-input variants are not broadly covered because they can be platform-sensitive. | Arc06 Slice02 |
| input invalid | `parser-input-invalid`, `grounder-input-invalid`, `engine-input-invalid`; baseline `parser-broken-syntax`, `parser-broken-reference` | Managed malformed-input coverage exists; deeper model-section/token variants remain sparse. | Arc06 Slice02 |
| output unavailable | `parser-output-unavailable`, `grounder-output-unavailable`, `engine-output-unavailable` | Managed missing-parent and output-directory coverage exists; write/flush/finalize failure coverage remains limited. | Arc06 Slice02 |
| status stream | `parser-status-stream-legality`, `grounder-status-stream-legality`, `engine-status-stream-legality`, `parser-final-status-fields`, `grounder-final-status-fields`, `engine-final-status-fields` | Managed final status and legal stream coverage exists per component. | Arc06 Slice02 for managed pipeline status composition |
| color | `parser-color-policy`, `grounder-color-policy`, `engine-color-policy` | Managed `--no-color`, `--no-colour`, `NO_COLOR`, and illegal `--color=always` with machine-owned streams are covered. TTY/auto/never human-presentation cases are not broad. | Arc06 Slice02 |
| surface fencing | `parser-legacy-surface-fencing`, `grounder-h2-surface-fencing`, `grounder-cpddl-fam-surface-fencing`, `engine-internal-driver-sentinel-fencing`, `engine-interactive-surface-fencing`, `engine-translation-surface-fencing`, `engine-sat-surface-fencing`, `engine-bdd-cudd-surface-fencing` | Visible non-supported surfaces are fenced as legacy, experimental, unsupported, future, or implementation-internal CLI usage failure. | no-op unless a future accepted surface changes |
| engine no-plan | `engine-domain-no-plan`, baseline `engine-unsolvable`, `make smoke-negative` unsolvable case | Managed no-plan classification exists for engine. | no-op for Slice02 first batch |
| grounder no-plan | `grounder-domain-no-plan` | Recorded as not reachable through accepted grounder-only semantics. | no-op; Re-entry: accepted grounder behavior proves no usable grounded model without engine search |
| timeout/resource/signal | engine harness timeout around search; smoke harness timeout around engine step | Harness safety exists, but there is no managed binary-owned `timeout`, `resource_limit`, `interrupted`, or signal fixture set. | Arc06 Slice02 and Arc06 Slice06 |
| dependency failure / child process failure | Optional/fenced surfaces avoid promoting dependency paths. | No supported dependency-failure fixture currently exists because no supported default path depends on optional external runtime dependencies. | no-op; Re-entry: a future supported dependency or child-process boundary is accepted |
| pipeline composition | baseline `pipeline-minimal`, `make smoke`, `make smoke-negative` | Baseline and smoke coverage exists; managed pipeline contract fixtures do not. | Arc06 Slice02 |
| release package executable behavior | none in current fixture set | Arc06 local source fixtures do not prove packaged release assets. | Arc08 |
| wolong fetch/install/migration | none in current fixture set | Explicitly outside Arc06 Slice01. | Arc08 |

## Missing Or Deferred Obligations

| Gap | Owner | Rationale | Re-entry |
|-----|-------|-----------|----------|
| Managed parser-grounder-engine pipeline fixture with final status on stderr at every step. | Arc06 Slice02 | The baseline pipeline and smoke gate prove composition, but not a managed contract pipeline record. | Slice02 opens with this as the first fixture batch. |
| Pairwise composition variants: parser stdout artifact to grounder input file, grounder stdout artifact to engine input file, and file-backed equivalents. | Arc06 Slice02 | Current per-component stdout fixtures do not prove downstream acceptance of those artifacts in one black-box chain. | Add if the target CLI supports the composition without inventing stdin behavior. |
| `--supervised` positive-path fixtures for parser, grounder, and engine. | Arc06 Slice02 | Current contract fixtures cover supervised mostly through conflict/color policy, not a positive supervised invocation. | Add CI-safe per-component positive supervised cases. |
| TTY/color positive cases for `--color=auto`, `--color=never`, and human-stderr-only ANSI policy. | Arc06 Slice02 | Current color fixtures focus on machine-stream ANSI absence and illegal color ownership. | Add only if the harness can make TTY/no-TTY behavior deterministic. |
| Unreadable-input fixtures where the platform can create them safely. | Arc06 Slice02 | Missing-input coverage exists; permission-based unreadability can be platform-sensitive on local/CI hosts. | Add only with a deterministic platform predicate or skip classification. |
| Write, flush, and finalize output-failure fixtures. | Arc06 Slice02 | Missing parent and output-directory coverage exists; lower-level write/finalize failure needs deterministic devices or helper support. | Add deterministic cases first; defer host-specific cases. |
| Binary-owned timeout and resource-limit status fixtures. | Arc06 Slice02 | Arc03 names the status classes, but the current binaries do not expose accepted timeout/resource options. | Re-enter only if a supported timeout/resource option or harness-observable contract is accepted. |
| Coverage reporting for chengdu-owned runtime and adoption seams. | Arc06 Slice03 | Current gates build and test; they do not emit scoped coverage reports. | Slice03 defines instrumentation, exclusions, and report format. |
| `clang-tidy` / Clang Static Analyzer gate for owned C++ source. | Arc06 Slice04 | Current `make static-analysis` is shell syntax plus shellcheck. | Slice04 adds or explicitly defers owned C++ static analysis with suppression policy. |
| ASan/UBSan/LSan process-fixture workloads beyond runtime. | Arc06 Slice05 | `sanitize-runtime` exists, but binary sanitizer gates are not yet representative. | Slice05 runs representative fixtures under supported sanitizer builds or records toolchain limits. |
| TSan workload and CI disposition. | Arc06 Slice06 | TSan needs meaningful concurrency/process-observation workload. | Slice06 adds a meaningful gate or defers with a concrete workload condition. |
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
| `make test` | Runtime CTest, baseline contract, managed component contracts, positive smoke, and negative smoke | local and CI through `make ci-linux` and `make ci-macos` |
| `make smoke` | Positive parser-grounder-engine pipeline | local and CI through `make test` and `make readme-verbatim` |
| `make smoke-negative` | Missing input, invalid input, broken reference, and no-plan smoke | local and CI through `make test` |
| `make provenance-check` | Dist provenance records for built binaries | local and CI through `make ci-linux` and `make ci-macos` |
| `make actionlint` | GitHub Actions syntax | local and CI lint job |
| `make format-check` | Owned C++ formatting | local and CI through `make ci-linux` and `make ci-macos` |
| `make static-analysis` | Shell syntax plus shellcheck | local and CI through Linux aggregate; C++ static analysis deferred |
| `make safety-checks` | Workflow Make-entrypoint check, whitespace, nested Git metadata, gitlinks | local through `make check`; CI coverage is partial |
| `make sanitize-runtime` | Runtime ASan/UBSan CTest | Linux CI through `make ci-linux`; binary sanitizer fixtures deferred |
| `make ci-linux` | Linux CI aggregate | CI on ubuntu-22.04 and ubuntu-24.04 |
| `make ci-macos` | macOS CI aggregate | CI on macos-15 |
| `make readme-verbatim` | README source-build smoke | CI on ubuntu-22.04 and macos-15 |
| release workflow package/publish targets | Release publication | Arc08; not expanded by Arc06 Slice01 |
| coverage report target | Coverage | deferred to Arc06 Slice03 |
| C++ static-analysis target | clang-tidy / analyzer | deferred to Arc06 Slice04 |
| binary sanitizer fixture target | ASan/UBSan/LSan over process fixtures | deferred to Arc06 Slice05 |
| TSan target | race detection | deferred to Arc06 Slice06 |

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
