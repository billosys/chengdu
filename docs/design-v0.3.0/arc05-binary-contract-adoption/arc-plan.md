# chengdu arc05 - binary-contract-adoption - arc plan

> Plan-of-record for arc05, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc begins after Arc04
> `shared-runtime-substrate` is closed and after `ci-notes.md` is accepted as
> a planning input.

## 1. Capability statement

Roadmap line: *Migrate parser, grounder, and engine onto the shared process
contract and namespaced `pandapi-*` entry points, retiring inherited
command-name compatibility as each native new-name binary lands.*

Expanded: this arc turns the accepted Arc03 managed-process contract and
Arc04 runtime substrate into executable behavior for the three primary
planner binaries. It must make the canonical `pandapi-parser`,
`pandapi-grounder`, and `pandapi-engine` entry points real. Any inherited-name
adapter shim used during migration is temporary scaffolding: once the owning
binary builds and passes its contract fixtures under the new name, tests, make
targets, tooling, and docs must move to the new name and the shim must be
deleted. The arc must prove behavior at the process boundary with black-box
fixtures rather than relying on unit or seam tests as a proxy.

Arc05 is implementation work. It may change parser, grounder, and engine
behavior only through the accepted contract. It must not expand optional
inherited surfaces just because a dependency or build path is available. Later
arcs own the downstream gates: Arc06 owns remaining CI/test hardening, Arc07
owns tutorial/docs/architecture, and Arc08 owns release packaging,
license/NOTICE proof, wolong verification, release notes, checksums,
manifests, and publication.

## 2. Design principles

1. **Quality runway before behavior changes.** Formatting, sanitizer, runtime
   build, and CI gates land before parser, grounder, or engine behavior
   changes. This keeps later diffs about contract adoption, not tool
   archaeology.
2. **Product behavior is process behavior.** Catch2 and runtime seam tests are
   useful, but executable conformance is proved by black-box process fixtures
   observing argv, environment, stdin, stdout, stderr, artifacts, exit status,
   final `PANDAPI_STATUS`, signals, timeouts, TTY/color, and normalization.
3. **One binary at a time.** Parser migrates first, then grounder, then engine.
   Each binary gets before/after evidence, canonical command proof, positive
   and negative probes, and no optional-surface expansion.
4. **No inherited-name compatibility by default.** `v0.2.0` is the transition
   release; `v0.3.0` intentionally adopts the new `pandapi-*` external
   interface and managed-process behavior. Shims may exist only long enough to
   bootstrap a native new-name build and its contract tests.
5. **Runtime boundary stays chengdu-owned.** Shared status, diagnostics,
   provenance, fixture, and policy helpers live behind `pandapi::runtime`.
   Third-party APIs must not leak through planner internals.
6. **Arc02 gates still govern dependencies.** fmt, CLI11, Catch2,
   `tl::expected`, and reproc++ enter only through their approved gates.
   Held or rejected dependencies do not re-enter by convenience.
7. **No release overclaim.** Passing Arc05 means the binaries conform locally
   and in CI to the scope this arc opens. It does not mean the expanded CI
   matrix, tutorial/docs set, release package, wolong installation path,
   license files, notices, manifests, or publication gates are complete.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `quality-tooling-runway` | Add the pre-adoption owned-source formatting, sanitizer, runtime/build, and CI runway from `ci-notes.md` without changing binary behavior. | all Arc05 source edits; CI trust before migration |
| slice02 | `contract-fixture-scaffold` | Turn the Arc03 test matrix and Arc04 fixture substrate into repo-level black-box process fixture structure and baseline probes without changing binary behavior. | parser, grounder, engine adoption proof |
| slice03 | `parser-contract-adoption` | Migrate parser normal HDDL parse behavior to the managed-process contract and add canonical command proof. | grounder pipeline input; first executable contract surface |
| slice04 | `grounder-contract-adoption` | Migrate grounder normal `.htn` grounding behavior to the managed-process contract while fencing H2 and cpddl/FAM surfaces. | parser-to-grounder composition; engine input |
| slice05 | `canonical-binary-cutover` | Closed as a deferred discovery slice after proving parser/grounder managed behavior still depended on transition shims and raw sidecars. | parser/grounder native remediation |
| slice06 | `parser-native-contract-cutover` | Replace the parser Bash adapter with a native `pandapi-parser` managed-contract artifact and remove parser old-name artifacts. | grounder native cutover; release/docs inputs |
| slice07 | `grounder-native-contract-cutover` | Replace the grounder Bash adapter with a native `pandapi-grounder` managed-contract artifact and remove grounder old-name artifacts. | engine adoption; release/docs inputs |
| slice08 | `engine-contract-adoption` | Migrate engine normal search behavior to the managed-process contract while fencing interactive, translation, SAT, BDD, and CUDD surfaces. | full primary pipeline behavior |
| slice09 | `binary-contract-synthesis` | Satisfied by this arc's closing report: compose parser, grounder, and engine evidence into the accepted Arc05 conformance report and split handoff for Arc06 CI/test hardening, Arc07 docs/tutorial inputs, and Arc08 release inputs. | project ledger P5; Arcs06-08 planning |

## 4. Dependencies

**Consumes:**

- Arc01 audit synthesis:
  [`../arc01-vendored-source-audit/audit-synthesis-pandapi.md`](../arc01-vendored-source-audit/audit-synthesis-pandapi.md)
- Arc02 combined library recommendations:
  [`../arc02-cpp-library-research/combined-library-recommendations.md`](../arc02-cpp-library-research/combined-library-recommendations.md)
- Arc03 final managed-process contract:
  [`../arc03-managed-process-contract/managed-process-contract.md`](../arc03-managed-process-contract/managed-process-contract.md)
- Arc03 contract-test matrix:
  [`../arc03-managed-process-contract/contract-test-matrix.md`](../arc03-managed-process-contract/contract-test-matrix.md)
- Arc04 closing report:
  [`../arc04-shared-runtime-substrate/closing-report.md`](../arc04-shared-runtime-substrate/closing-report.md)
- Arc04 substrate integration readiness:
  [`../arc04-shared-runtime-substrate/substrate-integration-readiness.md`](../arc04-shared-runtime-substrate/substrate-integration-readiness.md)
- CI/tooling design note:
  [`../ci-notes.md`](../ci-notes.md)
- C++ Core Guidelines substrate, especially project structure/tooling,
  headers/source hygiene, resource safety, error handling, arithmetic/UB,
  concurrency tools, and standard-library guidance.

**Leaves for later arcs:** Arc06 receives the remaining CI gate inventory,
expanded process-fixture gaps, coverage and heavier sanitizer/static-analysis
requirements, and release-readiness evidence needed by later dry-runs. Arc07
receives the final behavior-change table inputs, CLI examples,
architecture/dependency facts, and migration notes needed for tutorial and
README work. Arc08 receives dependency/license/NOTICE facts for dependencies
actually adopted, test-only dependency exclusion evidence, wolong
fetch/install/migration evidence requirements, release-asset inputs,
release-package dry-run obligations, and any remaining release-risk
amendments.

## 5. Current status

- **slice01 quality-tooling-runway - closed and CDC-verified.** Slice set:
  [`slice01-quality-tooling-runway/slice-doc.md`](slice01-quality-tooling-runway/slice-doc.md),
  [`slice01-quality-tooling-runway/ledger.md`](slice01-quality-tooling-runway/ledger.md),
  [`slice01-quality-tooling-runway/cc-prompt.md`](slice01-quality-tooling-runway/cc-prompt.md),
  [`slice01-quality-tooling-runway/closing-report.md`](slice01-quality-tooling-runway/closing-report.md),
  [`slice01-quality-tooling-runway/cdc-verification.md`](slice01-quality-tooling-runway/cdc-verification.md).
- **slice02 contract-fixture-scaffold - closed and CDC-verified.** Slice set:
  [`slice02-contract-fixture-scaffold/slice-doc.md`](slice02-contract-fixture-scaffold/slice-doc.md),
  [`slice02-contract-fixture-scaffold/ledger.md`](slice02-contract-fixture-scaffold/ledger.md),
  [`slice02-contract-fixture-scaffold/cc-prompt.md`](slice02-contract-fixture-scaffold/cc-prompt.md),
  [`slice02-contract-fixture-scaffold/closing-report.md`](slice02-contract-fixture-scaffold/closing-report.md),
  [`slice02-contract-fixture-scaffold/cdc-verification.md`](slice02-contract-fixture-scaffold/cdc-verification.md).
- **slice03 parser-contract-adoption - closed and CDC-verified.** Slice set:
  [`slice03-parser-contract-adoption/slice-doc.md`](slice03-parser-contract-adoption/slice-doc.md),
  [`slice03-parser-contract-adoption/ledger.md`](slice03-parser-contract-adoption/ledger.md),
  [`slice03-parser-contract-adoption/cc-prompt.md`](slice03-parser-contract-adoption/cc-prompt.md),
  [`slice03-parser-contract-adoption/closing-report.md`](slice03-parser-contract-adoption/closing-report.md),
  [`slice03-parser-contract-adoption/cdc-verification.md`](slice03-parser-contract-adoption/cdc-verification.md).
- **slice04 grounder-contract-adoption - closed and CDC-verified by
  supersession.** Slice set:
  [`slice04-grounder-contract-adoption/slice-doc.md`](slice04-grounder-contract-adoption/slice-doc.md),
  [`slice04-grounder-contract-adoption/ledger.md`](slice04-grounder-contract-adoption/ledger.md),
  [`slice04-grounder-contract-adoption/cc-prompt.md`](slice04-grounder-contract-adoption/cc-prompt.md),
  [`slice04-grounder-contract-adoption/closing-report.md`](slice04-grounder-contract-adoption/closing-report.md),
  [`slice04-grounder-contract-adoption/cdc-verification.md`](slice04-grounder-contract-adoption/cdc-verification.md).
- **slice05 canonical-binary-cutover - closed as deferred and
  CDC-verified.** Slice set:
  [`slice05-canonical-binary-cutover/slice-doc.md`](slice05-canonical-binary-cutover/slice-doc.md),
  [`slice05-canonical-binary-cutover/ledger.md`](slice05-canonical-binary-cutover/ledger.md),
  [`slice05-canonical-binary-cutover/cc-prompt.md`](slice05-canonical-binary-cutover/cc-prompt.md),
  [`slice05-canonical-binary-cutover/closing-report.md`](slice05-canonical-binary-cutover/closing-report.md),
  [`slice05-canonical-binary-cutover/cdc-verification.md`](slice05-canonical-binary-cutover/cdc-verification.md).
- **slice06 parser-native-contract-cutover - closed and CDC-verified.** Slice
  set:
  [`slice06-parser-native-contract-cutover/slice-doc.md`](slice06-parser-native-contract-cutover/slice-doc.md),
  [`slice06-parser-native-contract-cutover/ledger.md`](slice06-parser-native-contract-cutover/ledger.md),
  [`slice06-parser-native-contract-cutover/cc-prompt.md`](slice06-parser-native-contract-cutover/cc-prompt.md),
  [`slice06-parser-native-contract-cutover/closing-report.md`](slice06-parser-native-contract-cutover/closing-report.md),
  [`slice06-parser-native-contract-cutover/cdc-verification.md`](slice06-parser-native-contract-cutover/cdc-verification.md).
- **slice07 grounder-native-contract-cutover - closed and CDC-verified.**
  Slice set:
  [`slice07-grounder-native-contract-cutover/slice-doc.md`](slice07-grounder-native-contract-cutover/slice-doc.md),
  [`slice07-grounder-native-contract-cutover/ledger.md`](slice07-grounder-native-contract-cutover/ledger.md),
  [`slice07-grounder-native-contract-cutover/cc-prompt.md`](slice07-grounder-native-contract-cutover/cc-prompt.md),
  [`slice07-grounder-native-contract-cutover/closing-report.md`](slice07-grounder-native-contract-cutover/closing-report.md),
  [`slice07-grounder-native-contract-cutover/cdc-verification.md`](slice07-grounder-native-contract-cutover/cdc-verification.md).
- **slice08 engine-contract-adoption - closed and CDC-verified.** Slice set:
  [`slice08-engine-contract-adoption/slice-doc.md`](slice08-engine-contract-adoption/slice-doc.md),
  [`slice08-engine-contract-adoption/ledger.md`](slice08-engine-contract-adoption/ledger.md),
  [`slice08-engine-contract-adoption/cc-prompt.md`](slice08-engine-contract-adoption/cc-prompt.md),
  [`slice08-engine-contract-adoption/closing-report.md`](slice08-engine-contract-adoption/closing-report.md),
  [`slice08-engine-contract-adoption/cdc-verification.md`](slice08-engine-contract-adoption/cdc-verification.md).
- **slice09 binary-contract-synthesis - satisfied by Arc05 close.** The
  synthesis scope is represented by
  [`closing-report.md`](closing-report.md), which composes parser, grounder,
  and engine evidence and routes downstream work to Arc06, Arc07, and Arc08.

## 6. Planned implementation surface

Arc05 may touch these implementation surfaces as slices open:

| Surface | Intended ownership |
|---------|--------------------|
| `pandapi-runtime/` | Shared runtime helpers, fixtures, CMake/CTest seams, adapters needed by binary adoption. |
| `scripts/` | Temporary parser/grounder transition shims only until the owning native cutover deletes them; no new project-script entrypoints. |
| `.github/workflows/` | CI gates for build/runtime/tooling and later contract fixtures. Release workflows remain Arc08 unless explicitly opened. |
| `pandaPI/` | Parser, grounder, and engine adoption only in the per-binary slices that own those changes. |
| `mk/`, `Makefile` | Canonical local and CI entrypoints for build, test, check, actionlint, provenance, package-helper, and release-helper operations. |
| `tests/`, `fixtures/contract/`, `tools`, `README.md` | Active parser/grounder surfaces updated to canonical command names in the owning native cutover slices, with engine left inherited until Slice08. |
| `dist/`, `build/`, `release/` | Generated output only; no tracked release-shape changes until Arc08. |

## 7. Arc ledger

Composition rows verifying the capability; open here and close per-row in this
arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Every planned Arc05 slice is closed and CDC-verified, with no missing slice from the breakdown. | reproduced |
| A2 | The quality/tooling runway lands before binary behavior changes: owned-source formatting, runtime sanitizer proof, runtime/build CI coverage, and explicit deferral of coverage/TSan gates to Arc06 plus release-package gates to Arc08. | reproduced |
| A3 | The contract fixture scaffold proves product behavior through black-box process fixtures and preserves the distinction between Catch2 seam tests and executable conformance. | reproduced |
| A4 | Parser normal HDDL parse conforms to the accepted contract through canonical `pandapi-parser`, with positive and negative fixture evidence and no release requirement to preserve inherited parser-command compatibility. | reproduced |
| A5 | Grounder normal `.htn` grounding conforms to the accepted contract through canonical `pandapi-grounder`, with H2 and cpddl/FAM surfaces fenced and no release requirement to preserve inherited grounder-command compatibility. | reproduced |
| A6 | Parser active build, test, fixture, provenance, package-helper, and make surfaces use the canonical command only, with parser inherited-name shims and sidecars deleted after native parser contract tests pass. | reproduced |
| A7 | Grounder active build, test, fixture, provenance, package-helper, and make surfaces use the canonical command only, with grounder inherited-name shims and sidecars deleted after native grounder contract tests pass. | reproduced |
| A8 | Engine normal search conforms to the accepted contract through canonical `pandapi-engine`, with interactive, translation, SAT, BDD, and CUDD surfaces fenced and no release requirement to preserve inherited `pandaPIengine` compatibility. | reproduced |
| A9 | Shared runtime adoption removes or routes duplicate process-policy code without leaking third-party APIs into planner internals or expanding optional surfaces. | reproduced |
| A10 | Release-shape and wolong fetch/install changes are explicit, documented, and verified for the new `pandapi-*` interface; inherited-name shims do not silently become release surface. | reproduced |
| A11 | Arcs06-08 can be planned from Arc05 close without silent drops: CI/test hardening, tutorial/docs inputs, release assets, behavior-change table, dependency licensing/NOTICE, test-only exclusion, checksums, manifests, and wolong proof obligations are routed. | reproduced |

## 8. Open questions and risks

- **OQ1 - CLI11 import timing.** CLI11 remains gated until the binary slice
  that owns CLI migration can prove help/version/provenance compatibility and
  golden/process behavior.
- **OQ2 - fmt import timing.** fmt remains optional behind diagnostics or
  process I/O facades. If standard-library formatting remains adequate, Arc05
  may keep fmt out.
- **OQ3 - sanitizer noise.** Sanitizers are blockers for chengdu-owned runtime
  and adapter code. Inherited vendored findings must be triaged by ownership
  and may require scoped suppression, deferral, or a dedicated cleanup slice.
- **OQ4 - process fixture stdin.** Arc04 Slice05 recorded that the current
  runner is not a general full-duplex large-stdin transport. Arc05 must harden
  it before stdin-heavy fixtures rely on it.
- **OQ5 - binary rename mechanics resolved, implementation split.** Build
  targets should produce the canonical `pandapi-*` binaries directly.
  Slice05 proved parser/grounder native contract work must precede full shim
  deletion. Parser and grounder now receive separate native cutover slices;
  after each native binary passes contract tests, the owning shim and old-name
  sidecar are deleted.
- **OQ6 - generated CLI code.** Existing gengetopt surfaces are inherited.
  Replacing them is not automatically part of Arc05 unless the owning binary
  slice proves the migration is safer than preserving compatibility.

## 9. Version history

- **v1.13 - 2026-08-11.** Closed Arc05. Marked Slice04
  grounder-contract-adoption CDC-verified by supersession, marked Slice08
  engine-contract-adoption closed and CDC-verified after a CDC corrective
  sentinel-fencing patch, and satisfied Slice09 through the arc closing report.
  Surfaced by: CDC verification of current native parser/grounder/engine
  contract evidence and a fresh subagent review of the Slice08 corrective diff.
  Why: Arc05's primary-binary adoption surface is now canonical
  `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`; release,
  tutorial/docs, expanded CI, and wolong proof remain downstream.

- **v1.12 - 2026-08-10.** Marked Slice07
  grounder-native-contract-cutover closed and CDC-verified, and opened
  Slice08 engine-contract-adoption. Surfaced by: CDC reproduction of CC
  implementation commit `43151b5f`. Why: parser and grounder now both build
  and test through native canonical `pandapi-*` artifacts with old-name
  artifacts retired, leaving engine as the final inherited primary binary
  before Arc05 synthesis.

- **v1.11 - 2026-08-10.** Marked Slice06
  parser-native-contract-cutover closed and CDC-verified, and opened Slice07
  grounder-native-contract-cutover. Surfaced by: CDC reproduction of CC
  implementation commit `b70b0825`. Why: parser now has a native canonical
  `pandapi-parser` artifact with parser old-name artifacts deleted, while
  aggregate builds still show grounder transition shims and old-name artifacts
  that must be removed in their owning cutover slice.

- **v1.10 - 2026-08-10.** Closed Slice05 as a deferred discovery slice and
  split native cutover into Slice06 parser-native-contract-cutover and planned
  Slice07 grounder-native-contract-cutover; renumbered engine adoption to
  Slice08 and synthesis to Slice09. Surfaced by: Slice05 stop condition and
  CDC reproduction. Why: current managed parser/grounder behavior still lived
  in Bash adapters over raw sidecars that reject `--status=stderr`, so a
  no-shim cutover requires per-component native implementation first.

- **v1.9 - 2026-08-10.** Inserted Slice05
  canonical-binary-cutover and renumbered engine adoption to Slice06 and
  synthesis to Slice07. Surfaced by: operator scope correction after
  parser/grounder managed behavior existed under canonical names. Why:
  `v0.3.0` should delete parser/grounder transition shims and move active
  build, test, fixture, tooling, package-helper, README, and planning
  surfaces to canonical command names before engine adoption resumes.

- **v1.8 - 2026-08-10.** Recorded operator override that Arc05 should not ship
  inherited `pandaPI*` command compatibility for 0.3.0. Surfaced by: operator
  correction during Slice04 CDC review. Why: `v0.2.0` is the compatibility
  transition; `v0.3.0` intentionally moves to `pandapi-*` command names and
  managed-process behavior, so adapter shims are temporary scaffolding only.

- **v1.7 - 2026-08-10.** Opened Slice04 grounder-contract-adoption.
  Surfaced by: Slice03 CDC verification. Why: parser adoption is accepted and
  make-targeted CI now reaches the parser contract gate, so grounder can adopt
  the same managed-process contract next while preserving parser evidence and
  keeping engine behavior baseline-only.

- **v1.6 - 2026-08-10.** Marked Slice03 parser-contract-adoption closed and
  CDC-verified after independent review of CC implementation commit
  `5c807016`, make-target follow-up commit `1d91a1ae`, and CDC evidence
  repair commit `fe4efbfa`. Surfaced by: Slice03 CDC verification. Why:
  Arc05 now has accepted parser managed-process behavior through canonical
  `pandapi-parser`; the earlier inherited-command acceptance is superseded by
  v1.8 and v1.9, and the parser contract fixture gate remains routed through
  make-based CI entrypoints.

- **v1.5 - 2026-08-10.** Opened Slice03 parser-contract-adoption.
  Surfaced by: Slice02 CDC verification. Why: parser adoption can now begin
  from an accepted black-box fixture scaffold and the seeded parser contract
  target, while grounder and engine remain baseline-only until their owning
  slices.
- **v1.4 - 2026-08-10.** Marked Slice02
  contract-fixture-scaffold closed and CDC-verified after independent review
  of implementation commit `836fa434` plus F-2 repair commit `f74a7f7a`.
  Surfaced by: Slice02 CDC verification. Why: Arc05 now has an accepted
  repo-level black-box baseline fixture scaffold for parser, grounder, engine,
  and pipeline behavior before per-binary contract adoption begins.
- **v1.3 - 2026-08-10.** Opened Slice02 contract-fixture-scaffold.
  Surfaced by: Slice01 CDC verification and the accepted Arc03/Arc04
  contract-fixture substrate. Why: parser, grounder, and engine adoption need
  black-box baseline process fixtures before any binary behavior changes.
- **v1.2 - 2026-08-10.** Marked Slice01 quality-tooling-runway closed and
  CDC-verified after independent review of commit
  `a701b32e9802a171936c5d07ddc4fd75c8f9af13`. Surfaced by: Slice01 CDC
  verification. Why: the owned-source format gate, runtime sanitizer gate,
  compile-command runway, CI coverage, and documented coverage/TSan/release
  deferrals are accepted; Slice02 can open from a verified quality runway.
- **v1.1 - 2026-08-10.** Updated Arc05 handoff language after the project
  roadmap split the former Arc06 release-hardening bucket into Arc06 CI/test
  hardening, Arc07 tutorial/docs, and Arc08 release publication. Source:
  project-plan v1.33. Why: Arc05 closure must route downstream obligations to
  the right arc instead of handing one overloaded release bucket to the next
  planner.
- **v1.0 - 2026-08-10.** Opened Arc05 and slice01
  quality-tooling-runway. Source: Arc04 close, accepted `ci-notes.md`, and
  operator request to put the agreed pre-Arc06 CI/tooling work in place before
  binary adoption. Why: Arc05 needs a trustworthy owned-source quality floor
  before parser, grounder, and engine executable behavior changes begin.
