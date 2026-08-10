# chengdu 0.3.0 - project plan

> **chengdu 0.3.0**: the first intentional behavior/API divergence release
> for the in-tree pandaPI fork. The goal is to turn the three planner
> binaries into excellent CLI tools and excellent managed processes: clear
> command contracts, namespaced binary names, disciplined stdout/stderr,
> documented exit/status semantics, bounded failure behavior, and source
> quality high enough that future work happens in a maintainable C/C++
> monorepo rather than in three inherited islands.
>
> Plan-of-record at project scale, per `PROJECT-MANAGEMENT.md` (v2.1).
> Predecessor: `docs/design-v0.2.0/` (closed 2026-08-09, gate GO). The
> pre-planning notes in `notes.md` are absorbed here as input, not left as a
> parallel plan.

## 1. Definition of done, and boundaries

**Done means:** `v0.3.0` ships a deliberate managed-process contract for
`pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`, backed by source
audits, a design document, implementation, tests, release documentation, and
consumer-facing migration guidance. The release provides namespaced
`pandapi-*` entry points or an explicitly accepted compatibility transition,
and every intentional behavior change is documented in a behavior-change
table. The three binaries can be supervised by another process using pipes
without regex archaeology, stdout/stderr ambiguity, accidental ANSI leakage,
or exit-code guessing.

**Behavioral target:** wolong's supervised-process use case is the acceptance
anchor. A caller should be able to classify at least solved, unsolvable,
input/parse error, resource/timeout policy failure, and internal error from
documented process behavior, without scraping human diagnostic text from a
mixed stream. CLI usage remains first-class: humans still get useful help,
diagnostics, and plain command flows.

**Required first steps:** no source redesign starts until Arc01 produces one
complete audit report for each of the three pandaPI repos now vendored in this
monorepo, followed by a synthesis report, and Arc02 researches open source C++
library choices against those audits. The synthesis must address best
practices, duplicate code, shared-library/header opportunities, and
build-system shape. Arc02's combined recommendations then decide which
candidate libraries are adopted, piloted, held, or rejected before Arc03 turns
the evidence into a managed-process design.

**Explicit non-goals for 0.3.0 unless amended by the operator:**

- No solver-algorithm research or planner-search capability work for its own
  sake.
- No Windows support, package-manager distribution, or new platform matrix.
- No Erlang port protocol / length-prefixed packet mode unless the line-based
  managed-process design proves insufficient.
- No unreviewed wholesale import of large optional subsystems such as the SAT
  verifier without a dedicated plan, license review, and build/test scope.
- No silent break of wolong's frozen fetch spec. Binary renaming is in scope,
  but the release must either preserve compatibility through aliases/wrappers
  or record an explicit operator-approved migration decision.

## 2. Research basis and absorbed notes

The 0.2.0 fork decision already recorded the behavioral evidence that made
this project necessary: inconsistent exit codes, status inferred by grepping
stdout, progress/log/plan stream mixing, ANSI leakage past no-colour flags,
unbounded malformed-input behavior, and missing machine-readable status. The
0.3.0 notes added the managed-process rubric: exit-code contract, stream
separation, buffering, ANSI/TTY discipline, fail-fast malformed input,
signals, machine-readable event mode, `--version` provenance, and no cwd/temp
or interactive assumptions on non-interactive paths.

This plan adds the operator's 2026-08-09 scope expansion:

- full C/C++ source cleanup and maintainability audit before redesign;
- unified best-practice error handling across the three tools;
- unified best-practice stdio handling for CLI and supervised-process use;
- equal quality for interactive CLI use and process-manager use;
- binary namespacing as `pandapi-*` to avoid conflicts with existing binaries;
- synthesis across all three codebases, including duplicate-code removal,
  possible shared sibling libraries/headers, and candidate modern C/C++
  dependencies.

## 3. Design posture

0.3.0 should not merely patch the worst exit-code and stdout defects. The
desired endpoint is a maintained downstream product surface around the
planner engines. That implies a stronger architectural line than prior
releases:

- Define a common process contract first, then update individual tools against
  it.
- Prefer shared runtime helpers where the same concern appears in two or more
  components: status codes, diagnostics, TTY/color detection, flushing,
  provenance/version output, filesystem/path handling, resource limits, and
  machine-readable event writing.
- Keep algorithmic code and process-contract code separated so future planner
  work does not need to rediscover CLI/runtime policy.
- Treat third-party libraries as design choices, not conveniences. Candidates
  must be justified by a real defect class, maintenance benefit, performance
  gain, license compatibility, build-matrix cost, and release-packaging impact.
- Treat Arc02's accepted portfolio as a planning constraint for every later
  arc: standard-library modernization is the baseline; fmt, CLI11, and Catch2
  are adopted only through their owning design/test gates; `tl::expected` and
  reproc++ are pilots behind explicit status or child-process decisions; held
  and rejected libraries do not re-enter by drift.

## 4. Arc roadmap

| Arc | Slug | Capability (one line) | Depends on |
|-----|------|----------------------|------------|
| arc01 | `vendored-source-audit` | Complete per-repo audits for parser, grounder, and engine, then synthesize cross-codebase recommendations for cleanup, shared code, process contracts, and dependency choices. | 0.2.0 closed fork baseline |
| arc02 | `cpp-library-research` | Research open source C++ libraries against the parser, grounder, and engine audit findings; produce per-component assessments and a final combined dependency recommendation set. | arc01 per-component audit reports; closes before managed-process design |
| arc03 | `managed-process-contract` | Produce the accepted design for CLI + supervised-process behavior: command naming, exit/status taxonomy, stdout/stderr/events, buffering, ANSI/TTY, signals/resources, version/provenance, and migration policy. | arc01, arc02 |
| arc04 | `shared-runtime-substrate` | Introduce the shared C/C++ runtime/build substrate selected by the design, with tests and no behavior changes beyond wiring. | arc03 |
| arc05 | `binary-contract-adoption` | Migrate parser, grounder, and engine onto the shared process contract and namespaced entry points, preserving or explicitly migrating compatibility surfaces. | arc04 |
| arc06 | `release-hardening` | Prove the new behavior locally and in CI, update docs/release assets/migration notes, and publish `v0.3.0` only after wolong-oriented consumer verification. | arc05 |

Detailed planning is closed through Arc04. Arc05-Arc06 remain roadmap only
until the operator opens them; Arc05 must be planned from the audit synthesis,
Arc02 combined library recommendations, the accepted Arc03 managed-process
contract, and the closed Arc04 substrate, not from earlier guesses.

## 5. Current status

- **arc01 - closed.** Detailed plan:
  [`arc01-vendored-source-audit/arc-plan.md`](arc01-vendored-source-audit/arc-plan.md).
  Closing report:
  [`arc01-vendored-source-audit/closing-report.md`](arc01-vendored-source-audit/closing-report.md).
  Slice01 parser audit is closed and CDC-verified:
  [`arc01-vendored-source-audit/slice01-parser-audit/cdc-verification.md`](arc01-vendored-source-audit/slice01-parser-audit/cdc-verification.md).
  The accepted parser report has been promoted into the arc directory:
  [`arc01-vendored-source-audit/audit-results-pandapi-parser.md`](arc01-vendored-source-audit/audit-results-pandapi-parser.md).
  Slice02 grounder audit is closed and CDC-verified:
  [`arc01-vendored-source-audit/slice02-grounder-audit/cdc-verification.md`](arc01-vendored-source-audit/slice02-grounder-audit/cdc-verification.md),
  with accepted report:
  [`arc01-vendored-source-audit/audit-results-pandapi-grounder.md`](arc01-vendored-source-audit/audit-results-pandapi-grounder.md).
  Slice03 engine audit is closed and CDC-verified:
  [`arc01-vendored-source-audit/slice03-engine-audit/cdc-verification.md`](arc01-vendored-source-audit/slice03-engine-audit/cdc-verification.md),
  with accepted report:
  [`arc01-vendored-source-audit/audit-results-pandapi-engine.md`](arc01-vendored-source-audit/audit-results-pandapi-engine.md).
  Slice04 audit synthesis is closed and CDC-verified:
  [`arc01-vendored-source-audit/slice04-audit-synthesis/cdc-verification.md`](arc01-vendored-source-audit/slice04-audit-synthesis/cdc-verification.md),
  with accepted synthesis:
  [`arc01-vendored-source-audit/audit-synthesis-pandapi.md`](arc01-vendored-source-audit/audit-synthesis-pandapi.md).
- **arc02 - closed.** Detailed plan:
  [`arc02-cpp-library-research/arc-plan.md`](arc02-cpp-library-research/arc-plan.md).
  Closing report:
  [`arc02-cpp-library-research/closing-report.md`](arc02-cpp-library-research/closing-report.md).
  All four library-research slices are CDC-verified, and the accepted combined
  recommendation report is:
  [`arc02-cpp-library-research/combined-library-recommendations.md`](arc02-cpp-library-research/combined-library-recommendations.md).
- **arc03 - closed.** Detailed plan:
  [`arc03-managed-process-contract/arc-plan.md`](arc03-managed-process-contract/arc-plan.md).
  Closing report:
  [`arc03-managed-process-contract/closing-report.md`](arc03-managed-process-contract/closing-report.md).
  Accepted final contract:
  [`arc03-managed-process-contract/managed-process-contract.md`](arc03-managed-process-contract/managed-process-contract.md).
  Slice01 supported-surface classification is closed and CDC-verified:
  [`arc03-managed-process-contract/slice01-supported-surface-classification/cdc-verification.md`](arc03-managed-process-contract/slice01-supported-surface-classification/cdc-verification.md),
  with accepted report:
  [`arc03-managed-process-contract/supported-surface-classification.md`](arc03-managed-process-contract/supported-surface-classification.md).
  Slice02 status-exit-signal-taxonomy is closed and CDC-verified:
  [`arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/cdc-verification.md`](arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/cdc-verification.md),
  with accepted report:
  [`arc03-managed-process-contract/status-exit-signal-taxonomy.md`](arc03-managed-process-contract/status-exit-signal-taxonomy.md).
  Slice03 stdio-event-tty-contract is closed and CDC-verified:
  [`arc03-managed-process-contract/slice03-stdio-event-tty-contract/cdc-verification.md`](arc03-managed-process-contract/slice03-stdio-event-tty-contract/cdc-verification.md),
  with accepted report:
  [`arc03-managed-process-contract/stdio-event-tty-contract.md`](arc03-managed-process-contract/stdio-event-tty-contract.md).
  Slice04 cli-naming-version-migration is closed and CDC-verified:
  [`arc03-managed-process-contract/slice04-cli-naming-version-migration/cdc-verification.md`](arc03-managed-process-contract/slice04-cli-naming-version-migration/cdc-verification.md),
  with accepted report:
  [`arc03-managed-process-contract/cli-naming-version-migration.md`](arc03-managed-process-contract/cli-naming-version-migration.md).
  Slice05 contract-test-matrix is closed and CDC-verified:
  [`arc03-managed-process-contract/slice05-contract-test-matrix/cdc-verification.md`](arc03-managed-process-contract/slice05-contract-test-matrix/cdc-verification.md),
  with accepted report:
  [`arc03-managed-process-contract/contract-test-matrix.md`](arc03-managed-process-contract/contract-test-matrix.md).
  Slice06 managed-process-contract-synthesis is closed and CDC-verified:
  [`arc03-managed-process-contract/slice06-managed-process-contract-synthesis/cdc-verification.md`](arc03-managed-process-contract/slice06-managed-process-contract-synthesis/cdc-verification.md),
  with accepted final contract:
  [`arc03-managed-process-contract/managed-process-contract.md`](arc03-managed-process-contract/managed-process-contract.md).
- **arc04 - closed.** Detailed plan:
  [`arc04-shared-runtime-substrate/arc-plan.md`](arc04-shared-runtime-substrate/arc-plan.md).
  Closing report:
  [`arc04-shared-runtime-substrate/closing-report.md`](arc04-shared-runtime-substrate/closing-report.md).
  Slice01 runtime-build-skeleton is closed and CDC-verified:
  [`arc04-shared-runtime-substrate/slice01-runtime-build-skeleton/cdc-verification.md`](arc04-shared-runtime-substrate/slice01-runtime-build-skeleton/cdc-verification.md).
  Slice02 status-result-core is closed and CDC-verified:
  [`arc04-shared-runtime-substrate/slice02-status-result-core/cdc-verification.md`](arc04-shared-runtime-substrate/slice02-status-result-core/cdc-verification.md).
  Slice03 diagnostics-status-io is closed and CDC-verified:
  [`arc04-shared-runtime-substrate/slice03-diagnostics-status-io/slice-doc.md`](arc04-shared-runtime-substrate/slice03-diagnostics-status-io/slice-doc.md),
  [`arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md`](arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md),
  [`arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cc-prompt.md`](arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cc-prompt.md),
  [`arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md`](arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md).
  Slice04 cli-tty-provenance-core is closed and CDC-verified:
  [`arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/slice-doc.md`](arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/slice-doc.md),
  [`arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md`](arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md),
  [`arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cc-prompt.md`](arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cc-prompt.md),
  [`arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md`](arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md).
  Slice05 fixture-and-seam-test-substrate is closed and CDC-verified:
  [`arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/slice-doc.md`](arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/slice-doc.md),
  [`arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md`](arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/ledger.md),
  [`arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cc-prompt.md`](arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cc-prompt.md),
  [`arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md`](arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md).
  Slice06 substrate-integration-readiness is closed and CDC-verified:
  [`arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/slice-doc.md`](arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/slice-doc.md),
  [`arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/ledger.md`](arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/ledger.md),
  [`arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/cc-prompt.md`](arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/cc-prompt.md),
  [`arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/cdc-verification.md`](arc04-shared-runtime-substrate/slice06-substrate-integration-readiness/cdc-verification.md).
- **arc05-arc06 - roadmap only.** Arc05 is now eligible for detailed planning
  from the accepted Arc04 close when the operator opens it. Arc06 remains
  downstream of Arc05 adoption evidence.

## 5.1 Arc02 Findings Carried Forward

Arc02 was inserted after the original 0.3.0 outline and is now a load-bearing
input to all remaining work:

- **Adopted baseline:** standard-library modernization is the default for
  ownership, paths, strings, chrono/timeouts, bounds, streams, and local hash
  hygiene before adding dependencies.
- **Adopted implementation choices:** fmt, CLI11, and Catch2 are accepted for
  0.3.0 only through their owning gates: fmt under a diagnostics/process
  facade after stream policy; CLI11 after CLI/name/help/version compatibility
  and golden-output tests; Catch2 as test-only seam coverage paired with
  process fixtures.
- **Pilots:** `tl::expected` may enter only behind a local status/result facade
  after Arc03 defines the taxonomy; reproc++ may enter only behind a
  child-process adapter if a supported surface requires subprocess containment.
- **Held:** nlohmann/json remains held because slice03 selected tagged text
  status-only output instead of JSON Lines; GSL, performance containers,
  google/benchmark, parser-generator alternatives, SAT/BDD/CUDD enablement,
  H2/cpddl dependency-internal work, and optional dependency audits wait for
  explicit re-entry criteria.
- **Rejected for 0.3.0 foundations:** Abseil and Boost.Process are not
  foundation choices for this release.

Arc03 converted these findings into semantic constraints. Arc04 implemented
only the dependency gates accepted by Arc03 and kept held or rejected
dependencies out of the runtime substrate. Arc05 must adopt accepted helpers
per binary without expanding optional surfaces. Arc06 must verify license,
NOTICE, provenance, test-only dependency exclusion, and release-asset shape.

## 6. Project ledger

Composition rows verifying the definition of done; they open here and close
per-row in this project's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| P1 | Arc01 closes with complete parser, grounder, and engine audit reports plus a synthesis report; every report cites concrete file/line evidence, maps findings to C++ Core Guidelines rule IDs where applicable, and records clean checks as well as findings. | reproduced |
| P2 | Arc02 closes with parser, grounder, and engine library-research reports plus a combined recommendation report; every candidate is mapped to concrete audit defect classes, license/build/packaging consequences, maintenance evidence, and an adopt/pilot/hold/reject disposition. | reproduced |
| P3 | Arc03 closes with an accepted managed-process design covering CLI ergonomics, supervised-process behavior, binary naming, exit/status semantics, stdout/stderr/event output, buffering, ANSI/TTY, signals/resources, version/provenance, migration policy, and explicit incorporation or deferral of Arc02 dependency findings. | reproduced |
| P4 | Arc04 closes with any shared runtime/build substrate implemented, tested, and limited to the design-approved surface; duplicate process-policy code is routed through the shared substrate where adopted, with Arc02-selected dependencies entering only through approved facades/pilots. | reproduced |
| P5 | Arc05 closes with all three primary binaries conforming to the accepted process contract, including namespaced `pandapi-*` entry points or an explicit compatibility transition, without library availability expanding optional inherited surfaces. | reproduced |
| P6 | The full local and CI gate suite proves positive and negative behavior under both CLI and pipe-supervised invocation; release docs include a behavior-change table, Arc02-driven dependency behavior notes, and wolong migration evidence. | reproduced |
| P7 | `v0.3.0` is published only after release assets, checksums, manifest/provenance, dependency licensing/NOTICE obligations, test-only dependency exclusion, and the wolong fetch/install/migration path are verified on supported platforms. | reproduced |

## 7. Open questions and risks

- **OQ1 - binary rename migration resolved.** Slice04 selected canonical
  `pandapi-*` names while keeping inherited `pandaPI*` names executable in
  0.3.0 as a compatibility transition. A future breaking removal requires an
  explicit operator decision, release-note migration table, wolong
  verification, and a new design update.
- **OQ2 - report home resolved.** Arc01 audit reports are durable design
  evidence, not transient workbench output. The report home is
  `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-*.md`
  plus
  `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`,
  referenced from slice close reports. Accepted recommendations are then
  promoted into later design docs.
- **OQ3 - C/C++ knowledge substrate resolved.** Arc01 audits must use
  `ai-engineering/knowledge/cpp/SKILL.md` and its guide set as the C++ audit
  substrate. The substrate is grounded in the C++ Core Guidelines by Bjarne
  Stroustrup, Herb Sutter, and contributors; each audit starts with
  `guides/11-anti-patterns.md`, then loads topic guides relevant to the
  component under review. The chengdu managed-process rubric still governs
  stdout/stderr, exit/status, pipe, signal, provenance, and wolong-supervision
  concerns where the Core Guidelines are not specific enough.
- **OQ4 - former submodules.** `cpddl` and `h2-fd-preprocessor` are vendored
  under the grounder. Arc01 should audit grounder-owned integration and record
  dependency risks; a deep fourth/fifth audit of those upstream codebases is a
  possible recommendation, not silently part of the three primary reports.
- **OQ5 - dependency temptation routed.** Modern libraries may be the right
  answer, but each candidate must clear license, supported-platform build,
  static vs vendored packaging, maintenance, and security/update implications.
  Arc02 owns that research before Arc03 turns any dependency into design.

## 8. Version history

- **v1.31 - 2026-08-09.** Opened Arc04 slice04 cli-tty-provenance-core.
  Surfaced by: Arc04 slice03 CDC verification. Why: the accepted status/result
  and diagnostics/status I/O substrate can now support runtime common CLI
  policy, TTY/color policy, command identity, and provenance/version assembly
  while preserving Arc02 dependency gates, especially no CLI11 import before
  golden/process tests.
- **v1.30 - 2026-08-09.** Marked Arc04 slice03 diagnostics-status-io closed
  and CDC-verified after Iteration 01. Surfaced by: slice03 CDC verification
  of commit `ece82e0e311ab2e63c922a6b679bb3ae647fdbec`. Why: the accepted
  runtime status I/O substrate now covers tagged `PANDAPI_STATUS`
  writer/parser, stdout/stderr status-stream legality, final flush,
  partial-output policy, and overflow-safe numeric parsing while preserving
  Arc02 dependency gates and leaving binary adoption to Arc05.
- **v1.29 - 2026-08-09.** Opened Arc04 slice03 diagnostics-status-io. Surfaced
  by: Arc04 slice02 CDC verification. Why: the accepted status/result substrate
  can now support a tested diagnostics/status I/O facade for tagged-text
  `PANDAPI_STATUS`, stdout/stderr ownership, flushing, and partial-output
  policy while preserving Arc02 dependency gates and leaving binary adoption to
  Arc05.
- **v1.28 - 2026-08-09.** Marked Arc04 slice02 status-result-core closed and
  CDC-verified. Surfaced by: slice02 CDC verification of commit
  `e277c0a568269d18d870789f8e6b60ada479c7a9`. Why: the shared runtime now has
  tested Arc03 status/exit mapping, status-class vocabulary, dynamic signal
  status handling, and a local standard-library-only `StatusResult<T>` facade
  without binary adoption or third-party dependency leakage.
- **v1.27 - 2026-08-09.** Opened Arc04 slice02 status-result-core. Surfaced
  by: Arc04 slice01 CDC verification. Why: the accepted runtime skeleton can
  now receive the Arc03 status vocabulary, exit-code mapping, payload-class
  vocabulary, and local status/result facade as tested runtime code while
  keeping binary adoption in Arc05.
- **v1.26 - 2026-08-09.** Marked Arc04 slice01 runtime-build-skeleton closed
  and CDC-verified. Surfaced by: slice01 CDC verification of commit
  `2732865d78b5a4dc7330eac7f4ce1164627a3bc2`. Why: the chengdu-owned runtime
  root, namespace, static CMake target, CTest smoke test, and
  `scripts/build-runtime.sh` entrypoint are accepted; Arc04 can now open
  slice02 status-result-core from a stable inert runtime skeleton.
- **v1.25 - 2026-08-09.** Opened Arc04 shared-runtime-substrate and slice01
  runtime-build-skeleton. Surfaced by: operator request after Arc03 close.
  Why: the project can now establish a chengdu-owned C++ runtime/build
  substrate from the accepted Arc03 contract, while keeping per-binary behavior
  migration in Arc05 and release/wolong proof in Arc06.
- **v1.24 - 2026-08-09.** Marked Arc03 closed after arc-level composition
  review. Surfaced by: Arc03 closing report. Why: project ledger P3 is now
  delivered; Arc04 can be planned from the accepted managed-process contract,
  while Arc05 binary adoption and Arc06 release/wolong proof remain roadmap
  work.
- **v1.23 - 2026-08-09.** Marked Arc03 slice06
  managed-process-contract-synthesis closed and CDC-verified. Surfaced by:
  slice06 CDC verification. Why: Arc03 now has an accepted final normative
  managed-process contract and can move to arc-level closure; implementation,
  release packaging, and wolong runtime proof remain deliberately routed to
  Arc04-Arc06.
- **v1.22 - 2026-08-09.** Opened Arc03 slice06
  managed-process-contract-synthesis. Surfaced by: slice05 CDC verification.
  Why: the project can now synthesize the accepted Arc03 design reports into
  the final managed-process contract and route Arc04 shared runtime, Arc05
  per-binary adoption, and Arc06 release-hardening inputs without silent drops.
- **v1.21 - 2026-08-09.** Marked Arc03 slice05 contract-test-matrix closed and
  CDC-verified. Surfaced by: slice05 CDC verification. Why: Arc03 can now open
  slice06 final synthesis from accepted surface, status/exit/signal,
  stdio/event/TTY, CLI naming/version/migration, and contract-test-matrix
  evidence.
- **v1.20 - 2026-08-09.** Opened Arc03 slice05 contract-test-matrix. Surfaced
  by: slice04 CDC verification. Why: the project can now define the golden
  process fixture and acceptance-probe matrix from accepted surface,
  status/exit, stdio/event/TTY, and CLI naming/version/migration contracts
  before Arc04 builds shared test/runtime substrate or Arc05 changes binary
  behavior.
- **v1.19 - 2026-08-09.** Marked Arc03 slice04
  cli-naming-version-migration closed and CDC-verified. Surfaced by: slice04
  CDC verification. Why: Arc03 can now open slice05 from stable CLI
  naming/version/migration semantics; 0.3.0 is a compatibility transition with
  canonical `pandapi-*` commands and inherited `pandaPI*` commands retained,
  while `--supervised`, `--status`, help/version/provenance, CLI parse-error,
  and color behavior are accepted.
- **v1.18 - 2026-08-09.** Opened Arc03 slice04
  cli-naming-version-migration. Surfaced by: slice03 CDC verification. Why:
  Arc03 can now decide `pandapi-*` command names, inherited-name compatibility,
  help/version/provenance, CLI parse-error behavior, no-color/no-colour
  controls, machine-status enablement, CLI11 adoption gates, and wolong
  migration implications from accepted surface, status, and stream contracts.
- **v1.17 - 2026-08-09.** Marked Arc03 slice03
  stdio-event-tty-contract closed and CDC-verified. Surfaced by: slice03 CDC
  verification after CC's corrective close-set commit. Why: Arc03 can now
  open slice04 from an accepted stream/event/TTY contract; machine-readable
  output is tagged text status-only for 0.3.0, and nlohmann/json remains held
  unless a later accepted event-format decision reopens JSON.
- **v1.16 - 2026-08-09.** Opened Arc03 slice03 stdio-event-tty-contract.
  Surfaced by: slice02 CDC verification. Why: the project can now define
  stdout/stderr/event/TTY/color/buffering from accepted surface and status
  contracts, while making the Arc02 fmt and nlohmann/json gates explicit.
- **v1.15 - 2026-08-09.** Marked Arc03 slice02
  status-exit-signal-taxonomy closed and CDC-verified. Surfaced by: slice02
  CDC verification. Why: Arc03 can now open slice03 from an accepted status
  vocabulary and exit-code map, with Arc02 dependency gates preserved for
  later stream/event, CLI, test, and substrate decisions.
- **v1.14 - 2026-08-09.** Added explicit Arc02 findings carry-forward and
  tightened Arc03-Arc06 project ledger criteria around dependency gates,
  release packaging, and optional-surface containment. Surfaced by: operator
  concern that Arc02 was inserted after the original plan and might not be
  fully incorporated before Arc03 slice02. Why: the accepted library portfolio
  must shape status, stream, CLI, test, substrate, adoption, and release work
  without letting dependency availability redefine product semantics.
- **v1.13 - 2026-08-09.** Marked Arc03 slice01
  supported-surface-classification closed and CDC-verified. Surfaced by:
  slice01 CDC verification. Why: Arc03 can now open slice02 from the accepted
  narrow managed-process product boundary and avoid expanding 0.3.0 around
  inherited optional modes.
- **v1.12 - 2026-08-09.** Opened Arc03 managed-process-contract with slice01
  supported-surface-classification. Surfaced by: Arc01 and Arc02 close. Why:
  the contract design can now start from verified audit synthesis and
  dependency recommendations, with supported-surface classification first so
  optional inherited paths do not silently define the 0.3.0 product surface.
- **v1.11 - 2026-08-09.** Marked Arc01 closed after arc-level composition
  accepted the parser, grounder, engine, and synthesis reports. Surfaced by:
  Arc01 close. Why: Arc03 can now open from both verified prerequisites, Arc01
  audit synthesis and Arc02 combined library recommendations.
- **v1.10 - 2026-08-09.** Marked Arc02 closed after full CDC verification of
  parser, grounder, engine, and combined library-recommendation slices.
  Surfaced by: operator request to CDC-verify Arc02. Why: the dependency
  recommendation portfolio is now a verified Arc03 design input; Arc03 still
  waits for Arc01 arc-level close.
- **v1.9 - 2026-08-09.** Updated Arc02 status after slice04 combined library
  recommendations closed and CDC verification accepted the final portfolio.
  Surfaced by: Arc02 slice04 CDC verification. Why: Arc03 now has a verified
  dependency-decision input, while Arc02 arc close still needs an explicit
  policy on the attested component research slices.
- **v1.8 - 2026-08-09.** Updated Arc01 status after slice04 audit synthesis
  closed and CDC verification accepted it. Surfaced by: Arc01 slice04 CDC
  verification. Why: all Arc01 slices are verified, and the audit synthesis now
  feeds Arc02 combined recommendations plus Arc03 managed-process design.
- **v1.7 - 2026-08-09.** Opened Arc01 slice04 audit synthesis and Arc02
  slice04 combined library recommendations, and corrected Arc01 status after
  engine audit CDC verification. Surfaced by: operator architecture guidance
  after all audit and component-library inputs landed. Why: final synthesis
  must preserve big-change architecture options and library-selection impacts
  before Arc03 design begins.
- **v1.6 - 2026-08-09.** Updated Arc02 status after the engine library research
  slice landed. Surfaced by: Arc02 slice03 engine-library-research. Why: all
  three component-level library research reports now exist, so Arc02 can move
  to combined recommendations before Arc03 managed-process design.
- **v1.5 - 2026-08-09.** Updated current status after slice02 grounder audit
  closed and CDC verification accepted it. Surfaced by: slice02 CDC
  verification. Why: Arc01 has two of three per-component audits verified, and
  the grounder report now feeds Arc02 library research plus Arc03 design.
- **v1.4 - 2026-08-09.** Inserted new Arc02 `cpp-library-research` and
  renumbered the managed-process/design/implementation/release arcs to
  Arc03-Arc06. Surfaced by: operator direction after the first parser audit.
  Why: open source C++ library choices should be researched from audit evidence
  before the managed-process design locks in shared runtime and dependency
  decisions.
- **v1.3 - 2026-08-09.** Promoted the accepted parser audit report from
  workbench output into the arc directory and opened slice02 grounder-audit
  plus slice03 engine-audit. Surfaced by: operator direction after parser CDC
  verification. Why: the parser report is durable design evidence, and the
  remaining per-repo audits should use the same report home and slice shape.
- **v1.2 - 2026-08-09.** Updated current status after slice01 parser audit
  closed and CDC verification accepted it. No roadmap change. Surfaced by:
  slice01 CDC verification. Why: Arc01 remains active, and slice02
  grounder-audit is now the next unopened slice.
- **v1.1 - 2026-08-09.** Replaced the temporary "no C/C++ substrate" risk with
  the new `ai-engineering/knowledge/cpp` Core Guidelines skill as mandatory
  Arc01 audit substrate, while preserving the chengdu-specific managed-process
  rubric. Surfaced by: operator research and new C++ knowledge-pack creation.
  Why: parser/grounder/engine audits should be grounded in a durable C++
  guideline corpus before CC starts slice01.
- **v1.0 - 2026-08-09.** Initial 0.3.0 roadmap and Arc01 opening. Source:
  closed 0.2.0 project plan, `docs/design-v0.3.0/notes.md`, and operator
  expansion on 2026-08-09. Why: 0.2.0 has shipped, so the deferred
  managed-process redesign can move from placeholder notes to a ledgered
  plan-of-record.
