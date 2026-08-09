# pandaPI Audit Synthesis

## 1. Executive Summary

Date: 2026-08-09

This synthesis consumes the three accepted Arc01 audit reports and their CDC
verification reports:

- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
  with `slice01-parser-audit/cdc-verification.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
  with `slice02-grounder-audit/cdc-verification.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
  with `slice03-engine-audit/cdc-verification.md`

The audits agree on the main diagnosis: the three inherited PANDA binaries are
not ready to be treated as excellent managed processes. Parser, grounder, and
engine all have process-contract defects in status propagation, stdout/stderr
discipline, CLI identity, test oracles, and source-level ownership/error
handling. The safest 0.3.0 path is not a thin shell wrapper alone. The
recommended architecture is the moderate path: define the managed-process
contract in Arc03, implement a small shared runtime/process substrate in Arc04,
then adopt it per binary in Arc05 with compatibility decisions made explicitly.

The big-change architecture is not rejected; it is deferred behind re-entry
conditions. It becomes warranted if Arc03 classifies optional paths such as
engine translation or grounder H2 containment as supported 0.3.0 surfaces, or
if Arc02's combined library recommendations show that dependency choices force
a build-system/runtime substrate larger than a small process facade.

## 2. Evidence Base

Accepted component audits:

| Component | Report sections used | CDC status |
|-----------|----------------------|------------|
| Parser | Parser report sections 3-5; findings `P-001` through `P-015`; synthesis inputs in section 4. | `slice01-parser-audit/cdc-verification.md` verdict accepted; no ledger rows dropped; protected-path diff produced no output. |
| Grounder | Grounder report sections 3-6; findings `G-001` through `G-019`; dependency boundary notes in section 4; synthesis inputs in section 5. | `slice02-grounder-audit/cdc-verification.md` verdict accepted; no ledger rows dropped; protected-path diff produced no output. |
| Engine | Engine report sections 3-5; findings `E-001` through `E-016`; synthesis inputs in section 4. | `slice03-engine-audit/cdc-verification.md` verdict accepted; no ledger rows dropped; protected-path diff produced no output. |

Arc02 context, not final dependency decisions:

- `docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/closing-report.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/closing-report.md`

The Arc02 component research reports consistently point at a standard-library
baseline plus possible shared candidates such as CLI11, fmt, `tl::expected`,
Catch2, and conditional process-containment tooling. Those are inputs to
Arc02 slice04 combined library recommendations. This synthesis does not adopt
or reject final libraries owned by Arc02, and it does not write the final
managed-process contract owned by Arc03.

## 3. Shared Defect Classes

### 3.1 Error and Status Propagation

Shared defect: all three binaries need a single status taxonomy and a rule that
ordinary failures return through `main()` rather than deep process exits.

| Component | Evidence |
|-----------|----------|
| Parser | Parser report section 3, `P-004`, finds deep parser/verifier helpers terminating the process directly. `P-005` finds argument validation that reports an error but continues. |
| Grounder | Grounder report section 3, `G-004`, finds deep helper `exit(-1)` and `_exit(0)` paths. `G-005` finds missing arguments treated as stdin/stdout execution. `G-006` finds output streams opened after parse failure. |
| Engine | Engine report section 3, `E-002`, finds input/model errors terminating from deep helpers. `E-003` finds incomplete exit taxonomy even after the unsolvable-status fix. `E-008` and `E-009` add timeout/signal/SAT path status gaps. |

Architecture input: Arc03 should define a chengdu status taxonomy before
source repair. Minimum categories should include success, CLI usage error,
input/open error, parse/model validation error, unsupported feature, proven
unsolvable/unreachable, timeout/resource policy failure, interrupted/signal
termination, dependency/child-process failure, and internal defect. Arc04
should then provide a shared process facade that maps typed component outcomes
to exit codes and diagnostics.

### 3.2 stdout/stderr, TTY, ANSI, and Buffering

Shared defect: stdout is not reliably reserved for machine data or a structured
event stream, and color/TTY behavior is inconsistent.

| Component | Evidence |
|-----------|----------|
| Parser | Parser report section 3, `P-006`, finds diagnostics and progress contaminating stdout. `P-007` finds `--no-colour` does not suppress parser syntax-error ANSI escapes. |
| Grounder | Grounder report section 3, `G-007`, finds default stdout mixing configuration, progress, and machine output. `G-008` finds quiet mode suppressing H2 dependency output by poisoning `std::cout`. `G-009` finds debug/output-domain ANSI color without TTY/no-color control. |
| Engine | Engine report section 3, `E-004`, finds stdout mixing diagnostics, progress, statistics, status, and plan data. `E-005` finds ANSI color with no disable flag or TTY policy. |

Architecture input: Arc03 should make stream ownership explicit. For each
command mode, stdout is either the selected data artifact or a documented
machine-event stream, never both. Human diagnostics and progress go to stderr
or a structured event sink. Color defaults to auto-on-TTY only and must be
disabled for machine mode. Arc04 should provide a shared diagnostics/stream
router, including flush policy under pipes.

### 3.3 CLI Identity, Help, Version, and `pandapi-*` Naming

Shared defect: every component still exposes inherited upstream identity and
needs coordinated namespacing.

| Component | Evidence |
|-----------|----------|
| Parser | Parser report section 3, `P-008`, identifies upstream `pandaPIparser` version `0.1`; `P-014` names shared diagnostic/color/output-file/status helper candidates. |
| Grounder | Grounder report section 3, `G-010`, identifies inherited `pandaPIgrounder 0.1`; `G-011` identifies generated help formatting and policy gaps. |
| Engine | Engine report section 3, `E-006`, identifies upstream `pandaPIengine` version `0.1`; `E-007` finds interactive mode reachable from the public CLI. |

Architecture input: Arc03 owns the compatibility decision for `pandapi-parser`,
`pandapi-grounder`, and `pandapi-engine`. The recommended design should keep
wolong's frozen fetch/install shape unless the operator explicitly accepts a
breaking transition. If names change in 0.3.0, ship aliases/wrappers or a
documented migration table with release-asset implications.

### 3.4 Resource, Timeout, Signal, and Child-Process Behavior

Shared defect: resource behavior is component-specific today and not yet a
managed-process contract.

| Component | Evidence |
|-----------|----------|
| Parser | Parser report section 3, `P-002`, finds raw `FILE*` include handling and cwd-relative include behavior; `P-015` finds C library APIs on hot input paths. |
| Grounder | Grounder report section 3, `G-008`, finds global stream-state mutation around H2; `G-013` finds H2/cpddl dependency internals compiled/copied into the grounder; `G-019` finds documented RAM/runtime limits without tool-enforced policy. |
| Engine | Engine report section 3, `E-008`, finds timeout/signal behavior split between internal polling and an unsafe wrapper. `E-009` finds optional SAT signal/thread/direct-exit behavior. `E-015` finds translation mode shelling out with fixed filenames and weak cleanup. |

Architecture input: Arc03 must classify timeout and signal semantics for all
supported paths. Arc04 should provide RAII filesystem/temp/output helpers and
a common timeout/resource-status vocabulary. Child-process containment is not
a default dependency decision; it is only a supported-surface decision if H2 or
engine translation remains in the 0.3.0 contract.

### 3.5 Ownership, Lifetime, Globals, Generated Code, and Header Hygiene

Shared defect: all three codebases retain C/C++ source-quality debt that blocks
reuse, repeatable tests, and in-process worker aspirations.

| Component | Evidence |
|-----------|----------|
| Parser | Parser report section 3, `P-001`, finds release-disabled validation asserts. `P-009` finds no reusable parser API boundary. `P-012` finds raw owning pointers and explicit `new`. `P-013` finds headers exporting `using namespace std`. |
| Grounder | Grounder report section 3, `G-001`, finds an H2 string-literal pointer arithmetic bug. `G-002` finds trusted counts/indexes from `.htn` input. `G-003` finds release-disabled asserts. `G-016` finds raw stream ownership. `G-017` finds `std::hash` specializations in namespace `std`. `G-018` finds const input mutation through `const_cast`. |
| Engine | Engine report section 3, `E-001`, finds malformed input and release-disabled assert risks. `E-012` finds raw-pointer-heavy model/search/heuristic ownership. `E-013` finds namespace policy and specialization leakage. |

Architecture input: The shared substrate should not try to modernize planner
algorithms wholesale in 0.3.0. It should first harden process boundaries,
file/path ownership, status/result propagation, and test seams. Algorithmic
ownership cleanup belongs behind those seams, per component.

### 3.6 Build, Tests, Fixtures, and Warning Policy

Shared defect: build wrappers succeed, but the verification surface is not yet
strong enough for contract migration.

| Component | Evidence |
|-----------|----------|
| Parser | Parser report section 3, `P-010`, finds fixtures without a make test target or expected-output oracle. `P-011` finds warnings non-fatal. |
| Grounder | Grounder report section 3, `G-014`, finds no primary grounder test/fixture posture. `G-015` finds primary warnings allowed through. |
| Engine | Engine report section 3, `E-010`, finds no local test target or fixture oracle. `E-011` finds visible but non-fatal warnings. |

Architecture input: Test first, then contract migration. Arc03 should specify
golden process fixtures for help, version, no-argument invocation, missing
input, malformed input, valid pipe/file output, unsolvable/unreachable,
timeout/resource behavior, color modes, and optional-path containment. Arc04
should add reusable fixture harness support before Arc05 adopts the contract
binary by binary.

## 4. Supported Surface and Optional Paths

The 0.3.0 supported surface must be smaller than the inherited executable
surface until optional paths are explicitly classified.

| Surface | Classification recommendation | Evidence and rationale |
|---------|-------------------------------|------------------------|
| Parser normal HDDL parse path | Supported 0.3.0 path. | Parser findings `P-001` through `P-008` affect the primary path and must feed Arc03/Arc05. |
| Parser verifier/output helper paths | Optional or supported only after Arc03 classification. | Parser `P-004` includes verifier/helper exits; `P-009` says no reusable API boundary yet. |
| Grounder normal parser-output `.htn` to planner output path | Supported 0.3.0 path. | Grounder `G-002`, `G-004`, `G-005`, `G-007`, `G-014` are primary-path defects. |
| Grounder H2 path | Optional/experimental until classified. | Grounder `G-001`, `G-008`, and `G-013` show H2-specific integration risks. Supporting H2 may require an adapter or child-process containment decision. |
| Grounder `cpddl`/FAM integration | Supported only to the extent current default behavior requires it; dependency-internal audit remains separate. | Grounder section 4 distinguishes grounder-owned integration from dependency-internal findings; `G-013` is the integration boundary risk. |
| Engine normal search path | Supported 0.3.0 path. | Engine `E-003`, `E-004`, `E-006`, `E-008`, `E-010`, and `E-012` affect primary command behavior. |
| Engine interactive mode | Legacy or hidden unless Arc03 accepts it. | Engine `E-007` finds interactive mode reachable from the public CLI; managed-process mode should not inherit interactive assumptions. |
| Engine translation mode | Optional/legacy unless explicitly accepted. | Engine `E-015` shells out with fixed filenames and weak cleanup; process-library adoption is a non-input until the surface is supported. |
| Engine SAT path | Experimental unless separately planned. | Engine `E-009` finds process-level signals, threads, and direct exits in optional SAT behavior. |
| Engine BDD/CUDD path | Experimental/optional dependency, out of default 0.3.0 contract. | Engine `E-016` says bundled CUDD/BDD needs a separate dependency decision; default build does not ship it. |

Non-input for Arc03: library availability must not decide whether translation,
interactive, SAT, BDD, CUDD, H2, or deep `cpddl` internals are supported.
Arc03 classifies the surface first; Arc02 and Arc04 choose dependencies and
implementation details second.

## 5. Shared Runtime/Header/Library Candidates

These candidates recur in two or more components and should be considered for a
shared runtime or shared header surface.

| Candidate | Defect class retired | Evidence | Owner |
|-----------|----------------------|----------|-------|
| Process facade and status enum | Deep exits, collapsed statuses, usage/input/unsupported/internal taxonomy. | Parser `P-004`, `P-005`; Grounder `G-004`, `G-005`, `G-006`; Engine `E-002`, `E-003`, `E-008`, `E-009`. | Arc03 defines; Arc04 implements; Arc05 adopts. |
| Diagnostics and stream router | stdout/stderr separation, quiet/progress modes, buffering/flush, machine events. | Parser `P-006`; Grounder `G-007`, `G-008`; Engine `E-004`. | Arc03 defines; Arc04 implements. |
| Color/TTY policy helper | ANSI/no-color/auto TTY consistency. | Parser `P-007`; Grounder `G-009`; Engine `E-005`. | Arc03/Arc04. |
| CLI facade or parser wrapper | Usage errors, help/version consistency, `pandapi-*` naming. | Parser `P-008`; Grounder `G-010`, `G-011`; Engine `E-006`, `E-007`. | Arc03 contract; Arc05 adoption. |
| Version/provenance helper | Chengdu version, upstream import point, process-contract version. | All three reports identify inherited `0.1` upstream identity: `P-008`, `G-010`, `E-006`. | Arc03/Arc04. |
| RAII filesystem/output/temp helpers | File handles, output-open-after-error, cwd/temp assumptions, cleanup. | Parser `P-002`, `P-012`, `P-015`; Grounder `G-006`, `G-016`; Engine `E-015`. | Arc04, with per-component adoption. |
| Checked parsing/model validation helpers | Ill-formed input, release-disabled asserts, bounds. | Parser `P-001`, `P-003`; Grounder `G-002`, `G-003`; Engine `E-001`. | Arc04 supplies patterns; Arc05 applies per binary. |
| Test harness for process contracts | Fixture oracles for status/stdout/stderr/files/timeouts/help/version. | Parser `P-010`; Grounder `G-014`; Engine `E-010`; all CDC reports reproduced process probes or smoke behavior. | Arc04 test substrate; Arc05 contract gates. |
| Warning policy tiers | Primary-source warnings versus dependency-internal warnings. | Parser `P-011`; Grounder `G-015`; Engine `E-011`; grounder CDC notes dependency-internal warning separation. | Arc04 build/test substrate. |

Arc02 library inputs, not decisions:

- CLI11 is a strong candidate for future CLI redesign, but only after golden
  help/status/stdout/stderr tests exist.
- fmt is a strong formatting primitive under a chengdu diagnostics facade; it
  must not spread direct stdout writes through planner internals.
- `tl::expected` or a local equivalent is a strong pilot for typed results
  after Arc03 defines the taxonomy.
- Catch2 is a strong C++ unit-test candidate, paired with a process fixture
  runner because unit tests alone do not prove managed-process behavior.
- nlohmann/json is a hold/pilot only if Arc03 chooses JSON Lines or another
  JSON event/status mode.
- reproc++ or a local child-process wrapper is conditional on supported H2 or
  translation containment. Boost.Process is a fallback only if broader Boost
  adoption becomes justified.
- CUDD/BDD/SAT dependencies remain out of default 0.3.0 adoption until
  optional supported surfaces are separately accepted.

## 6. Architecture Options

Architecture decision matrix:

| Path | Benefits | Costs | Risks | Recommendation |
|------|----------|-------|-------|----------------|
| Conservative wrapper/contract repair | Fastest visible `pandapi-*` surface; lowest algorithm churn. | Duplicates policy and leaves many deep defects in place. | Wrappers cannot reliably classify deep exits or mixed stdout. | Fallback only. |
| Moderate shared runtime substrate plus per-binary adoption | Retires shared process defects once, keeps algorithmic code mostly intact, gives Arc04/Arc05 checkable work. | Requires a small substrate, golden tests, and per-component adapters. | Substrate can sprawl if optional surfaces are admitted too early. | Recommended. |
| Big-change architecture | Best long-term boundaries for optional dependencies and build/runtime cohesion. | Highest blast radius and CDC burden. | Can smuggle optional SAT/BDD/CUDD/H2/translation scope into 0.3.0. | Hold behind re-entry conditions. |

### Option A: Conservative wrapper/contract repair

Summary: keep inherited binaries mostly intact, add outer wrappers/aliases,
normalize release names, and patch only the most visible stdout/status defects
inside each component.

Benefits:

- Lowest immediate source churn.
- Fastest path to visible `pandapi-*` commands.
- Least risk of destabilizing planner algorithms.

Costs:

- Duplicates process policy across three inherited islands.
- Does not retire raw ownership, release-disabled validation, or deep exit
  patterns broadly.
- Makes optional surfaces harder to classify because wrappers see only process
  symptoms, not typed component outcomes.

Risks:

- Parser `P-004`, grounder `G-004`, and engine `E-002` show deep exits that
  wrappers cannot reliably classify.
- Parser `P-006`, grounder `G-007`, and engine `E-004` show stdout data/diagnostic
  mixing that must be fixed inside binaries, not only outside them.
- Engine timeout/signal paths in `E-008` and optional SAT path in `E-009` can
  still bypass wrapper policy.

Prerequisites:

- Arc03 defines a minimal exit/status and stream contract.
- Golden process tests exist before wrapper behavior is declared supported.

Implementation sequence:

1. Arc03 writes minimal managed-process contract.
2. Arc04 adds process fixture tests and wrapper/alias machinery.
3. Arc05 patches only required per-binary stdout/status defects.
4. Release hardening verifies wolong install shape unchanged.

Re-entry conditions:

- Choose this path only if 0.3.0 must minimize source churn and Arc03 explicitly
  defers shared runtime work.
- Re-enter the moderate path when a second component needs the same internal
  helper or when wrappers cannot classify a deep failure.

Verdict: not recommended as the primary 0.3.0 architecture. It is viable as a
fallback if schedule pressure dominates, but the audits show too many repeated
internal defects for wrapper-only work to be the clean design.

### Option B: Moderate shared runtime substrate plus per-binary adoption

Summary: define one managed-process contract, implement a small shared C/C++
runtime/process substrate, then adopt it in parser, grounder, and engine with
component-local algorithmic code left mostly intact.

Benefits:

- Directly addresses shared defect classes without rewriting planner
  algorithms.
- Gives Arc03 one contract and Arc04 one substrate for status, streams, color,
  provenance, RAII file/path helpers, and tests.
- Lets Arc05 migrate one binary at a time with contract tests.
- Keeps Arc02 dependency choices scoped: libraries sit under a chengdu facade,
  not directly in planner internals.

Costs:

- Requires build/layout work before behavior adoption.
- Requires a careful compatibility bridge for generated `options.ggo` surfaces
  or a coordinated CLI parser migration.
- Requires per-component adapters to translate legacy errors/exits into typed
  outcomes.

Risks:

- A too-large substrate could become a premature framework.
- If optional paths are admitted too early, child-process/dependency concerns
  can bloat Arc04.
- Library choices from Arc02 could drift into design before Arc03 fixes the
  contract vocabulary.

Prerequisites:

- Arc03 classifies supported versus optional surfaces.
- Arc03 defines the status taxonomy, stream contract, color/TTY policy,
  help/version/provenance policy, and binary-name migration.
- Arc02 slice04 recommends a dependency portfolio, including no-new-dependency
  baselines and adoption/pilot/hold/reject dispositions.

Implementation sequence:

1. Arc03: contract first. Define process statuses, stdout/stderr/event rules,
   color policy, version/provenance, signal/resource semantics, and supported
   surfaces.
2. Arc04: test first. Add a shared process fixture harness and golden tests for
   current and target behavior.
3. Arc04: shared substrate. Implement status enum/mapper, diagnostics stream
   router, color/TTY helper, provenance helper, RAII path/output helpers, and
   warning-policy tiers.
4. Arc05: binary adoption. Migrate parser first for fixture maturity, grounder
   second for parser-generated `.htn` fixture reuse, engine third for timeout
   and optional-surface complexity.
5. Arc06: release hardening. Prove CLI and pipe-supervised behavior, update
   behavior-change tables, and preserve or explicitly migrate wolong's frozen
   fetch/install shape.

Re-entry conditions:

- Escalate to big-change architecture if a supported path requires robust
  child-process containment, if CLI replacement forces build-system migration,
  or if per-binary adoption reveals planner internals cannot return typed
  outcomes without larger seams.
- Fall back to conservative repair only if Arc04 cannot land the substrate
  without expanding beyond process/runtime concerns.

Verdict: recommended. It is the boringly checkable path: contract first, tests
first, small substrate, then per-binary adoption.

### Option C: Big-change architecture

Summary: reorganize the fork around a larger shared runtime/build substrate,
possibly with a unified CMake layout, shared component libraries, stronger
dependency portfolio, child-process containment, and explicit optional-feature
modules.

Benefits:

- Best long-term maintainability if Chengdu becomes a sustained C++ monorepo.
- Gives clean boundaries for optional H2, cpddl, translation, SAT, BDD, and
  CUDD surfaces.
- Can replace generated/ad hoc CLI/status/diagnostic infrastructure across all
  components in one coherent design.
- Can put dependency notices, build integration, and warning tiers on a single
  foundation.

Costs:

- Highest migration blast radius.
- Highest risk to inherited planner behavior.
- Likely requires build-system decisions before user-facing behavior is stable.
- More CDC burden: every supported path needs stronger integration proof.

Risks:

- Parser-generator or algorithmic library rewrites could distract from the
  0.3.0 managed-process goal.
- Enabling optional CUDD/BDD/SAT/H2/translation surfaces could expand release
  scope without operator acceptance.
- A broad Boost/Abseil/process-library move could create packaging and NOTICE
  work out of proportion to the near-term defect classes.

Prerequisites:

- Arc02 combined recommendations must justify a broader dependency/build
  posture with current metadata and release-packaging consequences.
- Arc03 must explicitly accept which optional surfaces are supported.
- The operator must accept any wolong fetch/install shape change.

Implementation sequence:

1. Arc03 defines supported surfaces and rejects or hides experimental ones.
2. Arc02 slice04 finalizes the dependency portfolio with license/build/NOTICE
   evidence.
3. A new planning update splits Arc04 into substrate/build-system slices and
   optional-feature boundary slices.
4. Arc05 adopts binaries only after the shared runtime/build substrate is
   proven by contract tests.
5. Arc06 adds release packaging, NOTICE, checksums, and wolong migration proof.

Re-entry conditions:

- Choose this path if Arc03 keeps translation/H2/SAT/BDD/CUDD in the supported
  0.3.0 surface, or if Arc02 proves the small-substrate path would immediately
  be replaced.
- Choose this path later if moderate adoption creates repeated component-local
  adapters that should clearly be libraries.

Verdict: hold as an explicit future route. It is warranted only if supported
surface or dependency decisions force it. Do not smuggle it into 0.3.0 through
library enthusiasm.

## 7. Recommended Sequencing for Later Arcs

Implementation sequencing candidates:

| Step | Owning arc | Candidate work | Inputs |
|------|------------|----------------|--------|
| 1 | Arc02 | Combined library recommendations. | This synthesis's shared candidates and optional-surface cautions. |
| 2 | Arc03 | Managed-process contract. | Audit findings `P-004`/`G-004`/`E-003`, stream findings `P-006`/`G-007`/`E-004`, naming findings `P-008`/`G-010`/`E-006`, and optional-surface table above. |
| 3 | Arc03 | Supported-surface classification. | H2/cpddl, translation, interactive, SAT, BDD, CUDD classifications from section 4. |
| 4 | Arc04 | Test first: process fixture harness. | Build/test findings `P-010`, `G-014`, `E-010`; CDC probe patterns. |
| 5 | Arc04 | Shared substrate. | Status enum, diagnostics router, color/TTY helper, version/provenance helper, RAII filesystem/temp/output helpers, warning tiers. |
| 6 | Arc05 | Binary adoption: parser. | Parser fixtures exist; retire `P-004`, `P-006`, `P-007`, `P-008` first. |
| 7 | Arc05 | Binary adoption: grounder. | Reuse parser-generated `.htn`; retire `G-004`, `G-005`, `G-007`, `G-014`; classify H2/cpddl before default support. |
| 8 | Arc05 | Binary adoption: engine. | Retire `E-003`, `E-004`, `E-006`, `E-008`; keep interactive/translation/SAT/BDD/CUDD outside unless accepted. |
| 9 | Arc06 | Release hardening and migration. | Behavior-change table, aliases/wrappers or explicit rename decision, wolong fetch/install proof. |

Contract first means Arc03 writes the behavior target before Arc04 builds the
substrate. Test first means Arc04 creates executable proof before Arc05 changes
component behavior. Dependency choices should follow Arc02, and each dependency
must remain behind a chengdu-owned facade unless Arc03 deliberately exposes it.

## 8. Inputs and Non-inputs for Arc02 and Arc03

Inputs to Arc02 combined library recommendations:

- Treat the standard library baseline as the default: RAII streams,
  `std::filesystem`, `std::chrono`, scoped ownership, local hashers, checked
  parsing helpers, and ordinary containers before third-party choices.
- Evaluate CLI11, fmt, `tl::expected`, Catch2, nlohmann/json, reproc++,
  Boost.Process, GSL, Abseil, performance containers, benchmarking tools,
  parser generators, and CUDD only against the specific audit defect classes
  in this synthesis.
- Separate process-contract libraries from algorithmic/dependency-internal
  libraries.
- Include license, NOTICE, static/dynamic packaging, supported-platform build,
  and wolong release-shape consequences for every adopt/pilot candidate.

Non-inputs to Arc02:

- Do not adopt a process library just because engine translation mode or
  grounder H2 could use it; first Arc03 must classify those paths as supported.
- Do not enable CUDD/BDD/SAT in the default build as a library decision.
- Do not recommend parser-generator replacement for 0.3.0 unless Arc02 proves
  it retires more than the managed-process defects at acceptable cost.

Inputs to Arc03 managed-process contract:

- Define statuses and exit codes around all primary failure classes, not
  component-specific messages.
- Define stdout/stderr/event ownership for humans and supervisors.
- Define color/TTY/no-color behavior.
- Define binary names, compatibility aliases, and version/provenance fields.
- Define timeout, resource, signal, and interrupted-process behavior.
- Define supported, legacy, optional, and experimental surfaces.
- Define contract tests as part of the design, not as an implementation detail.

Non-inputs to Arc03:

- Do not let CLI11, fmt, JSON, reproc++, Boost.Process, or CUDD availability
  decide product semantics.
- Do not silently break wolong's frozen fetch/install shape.
- Do not promise in-process reusable libraries for parser/grounder/engine until
  the process boundary is stable.

## 9. Final Recommendation

Recommend Option B: moderate shared runtime substrate plus per-binary adoption.

The reason is evidence density. Parser, grounder, and engine independently
show the same managed-process defect classes, and CDC reproduced the audits as
diagnosis-only inputs. A wrapper-only fix leaves repeated internal failures in
place. A big architecture move is credible but premature before Arc02 finalizes
library recommendations and Arc03 classifies supported surfaces. The moderate
path preserves boring verification: contract first, test first, shared
substrate second, binary adoption third, release hardening last.

The first non-negotiable design decision for Arc03 is the supported surface.
Once that surface is explicit, dependency choices and implementation slices can
be made without silent scope expansion.
