# chengdu 0.3.0 - project plan

> **chengdu 0.3.0**: the first intentional behavior/API divergence release
> for the in-tree pandaPI fork. The goal is to turn the three planner
> binaries into excellent CLI tools and excellent managed processes: clear
> command contracts, namespaced binary names, disciplined stdout/stderr,
> documented exit/status semantics, bounded failure behavior, and source
> quality high enough that future work happens in a maintainable C/C++
> monorepo rather than in three inherited islands. The release also establishes
> a new user-learning surface for the fork: a highly detailed pandaPI 0.3.0
> tutorial, project workflow guidance, README updates, and architecture
> documentation that explain the new runtime/CLI shape rather than assuming
> prior planner or pandaPI experience.
>
> Plan-of-record at project scale, per `PROJECT-MANAGEMENT.md` (v2.1).
> Predecessor: `docs/design-v0.2.0/` (closed 2026-08-09, gate GO). The
> pre-planning notes in `notes.md` are absorbed here as input, not left as a
> parallel plan.

## 1. Definition of done, and boundaries

**Done means:** `v0.3.0` ships a deliberate managed-process contract for
`pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`, backed by source
audits, a design document, implementation, tests, release documentation, and
consumer-facing migration guidance. The release does not require inherited
`pandaPI*` command-name compatibility; any parser or grounder shims used while
Arc05 migrates binaries are temporary scaffolding and must be deleted once the
owning binary builds and passes contract tests under its new name. Every
intentional behavior change is documented in a behavior-change table. The
three binaries can be supervised by another process using pipes without regex
archaeology, stdout/stderr ambiguity, accidental ANSI leakage, or exit-code
guessing. The release also includes a public tutorial and
documentation set that can bring a newcomer from first HTN/PDDL/HDDL concepts
through simple and intermediate pandaPI project workflows, then into the new
0.3.0 `pandapi-*` CLI surface and architecture.

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
- No silent break of wolong's fetch/install expectations. Binary renaming and
  interface changes are in scope for 0.3.0 by explicit operator decision on
  2026-08-10; Arc08 must document the new wolong migration path and Arc09
  must verify it rather than preserve inherited command compatibility by
  default.

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

The CI/tooling research note
[`ci-notes.md`](ci-notes.md) is also an accepted planning input. It does not
replace Arc05 or Arc06, but it changes the order of work: formatting,
sanitizer, owned-source build, and runtime CI gates should be put in place at
the start of Arc05 before the binaries begin changing behavior. Coverage,
ThreadSanitizer, expanded process fixtures, and the remaining test/CI matrix
belong in later Arc05 or Arc06 gates. Release-package proof, license/NOTICE
proof, and wolong migration evidence now move to Arc09 publication gates after
the 2026-08-12 source-quality insertion.

This plan added the operator's 2026-08-10 roadmap expansion, later renumbered
by the 2026-08-12 source-quality correction below:

- split the former all-in-one Arc06 release-hardening bucket into three arcs;
- keep the remaining testing, CI, coverage, sanitizer, and conformance
  hardening in Arc06;
- add Arc07 for a pandaPI 0.3.0 tutorial and documentation suite, including
  HTN/PDDL/HDDL onboarding, project workflow examples, new `pandapi-*` CLI
  guidance, README updates, and architecture/dependency documentation;
- reserve Arc08 for release preparation, release-asset verification, wolong
  migration proof, and publication.

This plan adds the operator's 2026-08-12 source-quality correction:

- all in-tree pandaPI product code is Chengdu-owned in the fork/product sense,
  including parser, grounder, engine, and runtime;
- first-party maintained source requires formatting, static analysis,
  coverage, unit-test strategy, warning policy, and sanitizer triage before
  release;
- generated code from Chengdu-owned generators/templates should be fixed at
  the generator/template, while generated code from third-party skeletons may
  be excluded or suppressed with rationale;
- generated-warning cleanup must include root-cause and correctness analysis
  before closure: each warning should be traced to the generated file,
  generator/template or skeleton, ownership class, reason emitted, semantic
  correctness impact, chosen remedy, and re-entry condition;
- vendored third-party/dependency-internal code is excluded from Chengdu
  coverage/static-analysis/unit-test obligations unless a later dependency
  audit explicitly takes ownership of that boundary;
- insert a new Arc07 `source-quality-expansion` before public tutorial/docs
  and release publication work. The former Arc07 tutorial/docs work becomes
  Arc08, and the former Arc08 release-prep work becomes Arc09.

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
| arc05 | `binary-contract-adoption` | Migrate parser, grounder, and engine onto the shared process contract and namespaced entry points, retiring transition shims after canonical builds pass. | arc04 |
| arc06 | `ci-and-test-hardening` | Complete the remaining local and CI proof for the new behavior: expanded process fixtures, coverage, compiler-warning cleanup, heavier sanitizer/static-analysis gates, and pre-release CI evidence. | arc05 |
| arc07 | `source-quality-expansion` | Expand first-party source-quality policy and gates across parser, grounder, engine, runtime, and Chengdu-owned generators/templates, while excluding third-party source from first-party obligations. | arc06 |
| arc08 | `pandapi-tutorial-docs` | Create the pandaPI 0.3.0 tutorial and documentation suite: HTN/PDDL/HDDL onboarding, project workflow examples, `pandapi-*` CLI guidance, README updates, and architecture/dependency docs. | arc05, arc06, arc07 |
| arc09 | `release-prep-publication` | Verify release assets, checksums, manifest/provenance, dependency licensing/NOTICE, test-only dependency exclusion, wolong fetch/install/migration, source-quality release gates, and publish `v0.3.0`. | arc06, arc07, arc08 |

Detailed planning is open through Arc07. Arcs08-09 remain roadmap only until
Arc07 produces source-quality classification, gates, and a split handoff for
documentation/tutorial and release-publication work.

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
- **arc05 - closed.** Detailed plan:
  [`arc05-binary-contract-adoption/arc-plan.md`](arc05-binary-contract-adoption/arc-plan.md).
  Slice01 quality-tooling-runway is closed and CDC-verified:
  [`arc05-binary-contract-adoption/slice01-quality-tooling-runway/slice-doc.md`](arc05-binary-contract-adoption/slice01-quality-tooling-runway/slice-doc.md),
  [`arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md`](arc05-binary-contract-adoption/slice01-quality-tooling-runway/ledger.md),
  [`arc05-binary-contract-adoption/slice01-quality-tooling-runway/cc-prompt.md`](arc05-binary-contract-adoption/slice01-quality-tooling-runway/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice01-quality-tooling-runway/closing-report.md`](arc05-binary-contract-adoption/slice01-quality-tooling-runway/closing-report.md),
  [`arc05-binary-contract-adoption/slice01-quality-tooling-runway/cdc-verification.md`](arc05-binary-contract-adoption/slice01-quality-tooling-runway/cdc-verification.md).
  Slice02 contract-fixture-scaffold is closed and CDC-verified:
  [`arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/slice-doc.md`](arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/slice-doc.md),
  [`arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/ledger.md`](arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/ledger.md),
  [`arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/cc-prompt.md`](arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/closing-report.md`](arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/closing-report.md),
  [`arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/cdc-verification.md`](arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/cdc-verification.md).
  Slice03 parser-contract-adoption is closed and CDC-verified:
  [`arc05-binary-contract-adoption/slice03-parser-contract-adoption/slice-doc.md`](arc05-binary-contract-adoption/slice03-parser-contract-adoption/slice-doc.md),
  [`arc05-binary-contract-adoption/slice03-parser-contract-adoption/ledger.md`](arc05-binary-contract-adoption/slice03-parser-contract-adoption/ledger.md),
  [`arc05-binary-contract-adoption/slice03-parser-contract-adoption/cc-prompt.md`](arc05-binary-contract-adoption/slice03-parser-contract-adoption/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice03-parser-contract-adoption/closing-report.md`](arc05-binary-contract-adoption/slice03-parser-contract-adoption/closing-report.md),
  [`arc05-binary-contract-adoption/slice03-parser-contract-adoption/cdc-verification.md`](arc05-binary-contract-adoption/slice03-parser-contract-adoption/cdc-verification.md).
  Slice04 grounder-contract-adoption is closed and CDC-verified by
  supersession:
  [`arc05-binary-contract-adoption/slice04-grounder-contract-adoption/slice-doc.md`](arc05-binary-contract-adoption/slice04-grounder-contract-adoption/slice-doc.md),
  [`arc05-binary-contract-adoption/slice04-grounder-contract-adoption/ledger.md`](arc05-binary-contract-adoption/slice04-grounder-contract-adoption/ledger.md),
  [`arc05-binary-contract-adoption/slice04-grounder-contract-adoption/cc-prompt.md`](arc05-binary-contract-adoption/slice04-grounder-contract-adoption/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice04-grounder-contract-adoption/closing-report.md`](arc05-binary-contract-adoption/slice04-grounder-contract-adoption/closing-report.md),
  [`arc05-binary-contract-adoption/slice04-grounder-contract-adoption/cdc-verification.md`](arc05-binary-contract-adoption/slice04-grounder-contract-adoption/cdc-verification.md).
  Slice05 canonical-binary-cutover is closed as deferred and CDC-verified:
  [`arc05-binary-contract-adoption/slice05-canonical-binary-cutover/slice-doc.md`](arc05-binary-contract-adoption/slice05-canonical-binary-cutover/slice-doc.md),
  [`arc05-binary-contract-adoption/slice05-canonical-binary-cutover/ledger.md`](arc05-binary-contract-adoption/slice05-canonical-binary-cutover/ledger.md),
  [`arc05-binary-contract-adoption/slice05-canonical-binary-cutover/cc-prompt.md`](arc05-binary-contract-adoption/slice05-canonical-binary-cutover/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice05-canonical-binary-cutover/closing-report.md`](arc05-binary-contract-adoption/slice05-canonical-binary-cutover/closing-report.md),
  [`arc05-binary-contract-adoption/slice05-canonical-binary-cutover/cdc-verification.md`](arc05-binary-contract-adoption/slice05-canonical-binary-cutover/cdc-verification.md).
  Slice06 parser-native-contract-cutover is closed and CDC-verified:
  [`arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/slice-doc.md`](arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/slice-doc.md),
  [`arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/ledger.md`](arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/ledger.md),
  [`arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/cc-prompt.md`](arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/closing-report.md`](arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/closing-report.md),
  [`arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/cdc-verification.md`](arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/cdc-verification.md).
  Slice07 grounder-native-contract-cutover is closed and CDC-verified:
  [`arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/slice-doc.md`](arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/slice-doc.md),
  [`arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/ledger.md`](arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/ledger.md),
  [`arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/cc-prompt.md`](arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/closing-report.md`](arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/closing-report.md),
  [`arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/cdc-verification.md`](arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/cdc-verification.md).
  Slice08 engine-contract-adoption is closed and CDC-verified:
  [`arc05-binary-contract-adoption/slice08-engine-contract-adoption/slice-doc.md`](arc05-binary-contract-adoption/slice08-engine-contract-adoption/slice-doc.md),
  [`arc05-binary-contract-adoption/slice08-engine-contract-adoption/ledger.md`](arc05-binary-contract-adoption/slice08-engine-contract-adoption/ledger.md),
  [`arc05-binary-contract-adoption/slice08-engine-contract-adoption/cc-prompt.md`](arc05-binary-contract-adoption/slice08-engine-contract-adoption/cc-prompt.md),
  [`arc05-binary-contract-adoption/slice08-engine-contract-adoption/closing-report.md`](arc05-binary-contract-adoption/slice08-engine-contract-adoption/closing-report.md),
  [`arc05-binary-contract-adoption/slice08-engine-contract-adoption/cdc-verification.md`](arc05-binary-contract-adoption/slice08-engine-contract-adoption/cdc-verification.md).
  Arc05 is closed:
  [`arc05-binary-contract-adoption/closing-report.md`](arc05-binary-contract-adoption/closing-report.md).
- **arc06 - closed and CDC-verified.** Detailed plan:
  [`arc06-ci-and-test-hardening/arc-plan.md`](arc06-ci-and-test-hardening/arc-plan.md).
  Slice01 fixture-gap-inventory is closed and CDC-verified:
  [`arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/slice-doc.md`](arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/slice-doc.md),
  [`arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/ledger.md`](arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/ledger.md),
  [`arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/cc-prompt.md`](arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/cc-prompt.md),
  [`arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/closing-report.md`](arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/closing-report.md),
  [`arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/cdc-verification.md`](arc06-ci-and-test-hardening/slice01-fixture-gap-inventory/cdc-verification.md).
  Slice02 process-fixture-expansion is closed and CDC-verified:
  [`arc06-ci-and-test-hardening/slice02-process-fixture-expansion/slice-doc.md`](arc06-ci-and-test-hardening/slice02-process-fixture-expansion/slice-doc.md),
  [`arc06-ci-and-test-hardening/slice02-process-fixture-expansion/ledger.md`](arc06-ci-and-test-hardening/slice02-process-fixture-expansion/ledger.md),
  [`arc06-ci-and-test-hardening/slice02-process-fixture-expansion/cc-prompt.md`](arc06-ci-and-test-hardening/slice02-process-fixture-expansion/cc-prompt.md),
  [`arc06-ci-and-test-hardening/slice02-process-fixture-expansion/closing-report.md`](arc06-ci-and-test-hardening/slice02-process-fixture-expansion/closing-report.md),
  [`arc06-ci-and-test-hardening/slice02-process-fixture-expansion/cdc-verification.md`](arc06-ci-and-test-hardening/slice02-process-fixture-expansion/cdc-verification.md).
  Slice03 coverage-gate is closed and CDC-verified:
  [`arc06-ci-and-test-hardening/slice03-coverage-gate/slice-doc.md`](arc06-ci-and-test-hardening/slice03-coverage-gate/slice-doc.md),
  [`arc06-ci-and-test-hardening/slice03-coverage-gate/ledger.md`](arc06-ci-and-test-hardening/slice03-coverage-gate/ledger.md),
  [`arc06-ci-and-test-hardening/slice03-coverage-gate/cc-prompt.md`](arc06-ci-and-test-hardening/slice03-coverage-gate/cc-prompt.md),
  [`arc06-ci-and-test-hardening/slice03-coverage-gate/closing-report.md`](arc06-ci-and-test-hardening/slice03-coverage-gate/closing-report.md),
  [`arc06-ci-and-test-hardening/slice03-coverage-gate/cdc-verification.md`](arc06-ci-and-test-hardening/slice03-coverage-gate/cdc-verification.md).
  Slice04 static-analysis-gate is closed and CDC-verified:
  [`arc06-ci-and-test-hardening/slice04-static-analysis-gate/slice-doc.md`](arc06-ci-and-test-hardening/slice04-static-analysis-gate/slice-doc.md),
  [`arc06-ci-and-test-hardening/slice04-static-analysis-gate/ledger.md`](arc06-ci-and-test-hardening/slice04-static-analysis-gate/ledger.md),
  [`arc06-ci-and-test-hardening/slice04-static-analysis-gate/cc-prompt.md`](arc06-ci-and-test-hardening/slice04-static-analysis-gate/cc-prompt.md),
  [`arc06-ci-and-test-hardening/slice04-static-analysis-gate/closing-report.md`](arc06-ci-and-test-hardening/slice04-static-analysis-gate/closing-report.md),
  [`arc06-ci-and-test-hardening/slice04-static-analysis-gate/cdc-verification.md`](arc06-ci-and-test-hardening/slice04-static-analysis-gate/cdc-verification.md).
  Slice05 compiler-warning-burndown is closed and CDC-verified:
  [`arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/slice-doc.md`](arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/slice-doc.md),
  [`arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/ledger.md`](arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/ledger.md),
  [`arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/cc-prompt.md`](arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/cc-prompt.md),
  [`arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/closing-report.md`](arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/closing-report.md),
  [`arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/cdc-verification.md`](arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/cdc-verification.md).
  Slice06 binary-sanitizer-gates is closed and CDC-verified:
  [`arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/slice-doc.md`](arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/slice-doc.md),
  [`arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/ledger.md`](arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/ledger.md),
  [`arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/cc-prompt.md`](arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/cc-prompt.md),
  [`arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/closing-report.md`](arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/closing-report.md),
  [`arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/cdc-verification.md`](arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/cdc-verification.md).
  Slice07 tsan-and-ci-synthesis is closed and CDC-verified:
  [`arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/slice-doc.md`](arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/slice-doc.md),
  [`arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/ledger.md`](arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/ledger.md),
  [`arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/cc-prompt.md`](arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/cc-prompt.md),
  [`arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/closing-report.md`](arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/closing-report.md),
  [`arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/cdc-verification.md`](arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/cdc-verification.md).
  Arc06 closing report:
  [`arc06-ci-and-test-hardening/closing-report.md`](arc06-ci-and-test-hardening/closing-report.md).
  Arc06 is independently closed with reproduced local/CI matrix, expanded
  process fixtures, coverage, compiler-warning cleanup, heavier
  sanitizer/static-analysis gates, TSan disposition, and release-readiness
  handoff inputs needed by later release dry-runs.
- **arc07 - CC proposed done; awaiting CDC verification.** Detailed plan:
  [`arc07-source-quality-expansion/arc-plan.md`](arc07-source-quality-expansion/arc-plan.md).
  Slice01 source-classification-inventory is closed and CDC-verified:
  [`arc07-source-quality-expansion/slice01-source-classification-inventory/slice-doc.md`](arc07-source-quality-expansion/slice01-source-classification-inventory/slice-doc.md),
  [`arc07-source-quality-expansion/slice01-source-classification-inventory/ledger.md`](arc07-source-quality-expansion/slice01-source-classification-inventory/ledger.md),
  [`arc07-source-quality-expansion/slice01-source-classification-inventory/cc-prompt.md`](arc07-source-quality-expansion/slice01-source-classification-inventory/cc-prompt.md),
  [`arc07-source-quality-expansion/slice01-source-classification-inventory/closing-report.md`](arc07-source-quality-expansion/slice01-source-classification-inventory/closing-report.md),
  [`arc07-source-quality-expansion/slice01-source-classification-inventory/cdc-verification.md`](arc07-source-quality-expansion/slice01-source-classification-inventory/cdc-verification.md).
  Slice02 source-layout/build-surface normalization is closed and
  CDC-verified:
  [`arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/slice-doc.md`](arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/slice-doc.md),
  [`arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/ledger.md`](arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/ledger.md),
  [`arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/cc-prompt.md`](arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/cc-prompt.md),
  [`arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/closing-report.md`](arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/closing-report.md),
  [`arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/cdc-verification.md`](arc07-source-quality-expansion/slice02-source-layout-and-build-surface-normalization/cdc-verification.md).
  Slice03 first-party source naming normalization is closed and CDC-verified:
  [`arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/slice-doc.md`](arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/slice-doc.md),
  [`arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/ledger.md`](arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/ledger.md),
  [`arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/cc-prompt.md`](arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/cc-prompt.md),
  [`arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/closing-report.md`](arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/closing-report.md),
  [`arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/cdc-verification.md`](arc07-source-quality-expansion/slice03-first-party-source-naming-normalization/cdc-verification.md).
  Slice04 `first-party-quality-gate-scaffold` is closed and CDC-verified,
  building on the stable lower snake case source paths, selectors, compile
  database, coverage, and static-analysis surfaces established by Slices02-03:
  [`arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/slice-doc.md`](arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/slice-doc.md),
  [`arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/ledger.md`](arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/ledger.md),
  [`arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/cc-prompt.md`](arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/cc-prompt.md),
  [`arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/closing-report.md`](arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/closing-report.md),
  [`arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/cdc-verification.md`](arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/cdc-verification.md).
  Slice05 engine-first-party-quality-burndown is closed and CDC-verified as
  the first component source-quality burndown:
  [`arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/slice-doc.md`](arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/slice-doc.md),
  [`arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/ledger.md`](arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/ledger.md),
  [`arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/cc-prompt.md`](arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/cc-prompt.md),
  [`arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/closing-report.md`](arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/closing-report.md),
  [`arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/cdc-verification.md`](arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/cdc-verification.md).
  Slice05 established the engine component target pattern before parser
  generated-warning/root-cause work and later grounder/dependency follow-up.
  Slice06 parser-generated-warning-root-cause is closed and CDC-verified:
  [`arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/slice-doc.md`](arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/slice-doc.md),
  [`arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/ledger.md`](arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/ledger.md),
  [`arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/cc-prompt.md`](arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/cc-prompt.md),
  [`arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/closing-report.md`](arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/closing-report.md),
  [`arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/cdc-verification.md`](arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/cdc-verification.md).
  Slice06 made parser generated-warning evidence component-specific, closed
  Flex scanner helper warnings through the owned scanner input, and
  dispositioned parser gengetopt output without migrating to CLI11. Grounder
  first-party source and dependency-boundary follow-up is the recommended next
  scoped Arc07 ledger.
  Slice07 grounder-source-quality-boundary is closed and CDC-verified:
  [`arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/slice-doc.md`](arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/slice-doc.md),
  [`arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/ledger.md`](arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/ledger.md),
  [`arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/cc-prompt.md`](arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/cc-prompt.md),
  [`arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/closing-report.md`](arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/closing-report.md),
  [`arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/cdc-verification.md`](arc07-source-quality-expansion/slice07-grounder-source-quality-boundary/cdc-verification.md).
  Slice07 converted grounder source-quality scaffold rows into honest
  component targets, made the first-party/dependency boundary executable,
  resolved `rss.c` as first-party maintained copied-origin source with
  warnings fixed, and preserved managed grounder/pipeline behavior.
  Source-quality synthesis is the recommended next scoped Arc07 ledger.
  Slice08 source-quality-synthesis is CC proposed done, awaiting CDC
  verification:
  [`arc07-source-quality-expansion/slice08-source-quality-synthesis/slice-doc.md`](arc07-source-quality-expansion/slice08-source-quality-synthesis/slice-doc.md),
  [`arc07-source-quality-expansion/slice08-source-quality-synthesis/ledger.md`](arc07-source-quality-expansion/slice08-source-quality-synthesis/ledger.md),
  [`arc07-source-quality-expansion/slice08-source-quality-synthesis/cc-prompt.md`](arc07-source-quality-expansion/slice08-source-quality-synthesis/cc-prompt.md),
  [`arc07-source-quality-expansion/slice08-source-quality-synthesis/closing-report.md`](arc07-source-quality-expansion/slice08-source-quality-synthesis/closing-report.md).
  Slice08 composes the completed Arc07 source-quality evidence, reconciles
  the arc ledger A1-A9 as satisfied, classifies remaining work by release
  posture, and proposes Arc07 closure because the evidence honestly composes.
  Arc07 synthesis:
  [`arc07-source-quality-expansion/source-quality-synthesis.md`](arc07-source-quality-expansion/source-quality-synthesis.md).
  Arc07 closing report:
  [`arc07-source-quality-expansion/closing-report.md`](arc07-source-quality-expansion/closing-report.md).
  Arc07 classifies and expands first-party source-quality obligations across
  parser, grounder, engine, runtime, and Chengdu-owned generators/templates,
  while excluding third-party code from first-party obligations unless a later
  dependency audit accepts that boundary.
- **arc08 - roadmap only.** Arc08 remains downstream of Arc05/Arc06/Arc07
  proof and should create the user-facing pandaPI 0.3.0 tutorial and
  documentation suite: beginner HTN/PDDL/HDDL material,
  simple-to-intermediate project workflow examples, `pandapi-*` CLI guidance,
  README updates, architecture/dependency documentation for the new fork
  shape, and source-quality policy language that reflects Arc07's final
  classifications.
- **arc09 - roadmap only.** Arc09 remains downstream of Arc06, Arc07, and
  Arc08 and owns release preparation and publication: release assets,
  checksums, manifest/provenance, dependency licensing/NOTICE obligations,
  test-only dependency exclusion, source-quality release gate verification,
  wolong fetch/install/migration verification, and the actual `v0.3.0`
  release.

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
dependencies out of the runtime substrate. Arc05 adopted accepted helpers per
binary without expanding optional surfaces. Arc06 proved the expanded CI/test
surface. Arc07 must classify first-party versus generated/dependency source
quality obligations before public docs and release prep. Arc08 must explain
the accepted dependency, architecture, and source-quality posture in
user-facing docs; Arc09 must verify license, NOTICE, provenance, test-only
dependency exclusion, source-quality release gates, and release-asset shape.

## 5.2 CI and Tooling Findings Carried Forward

The accepted CI/tooling note
[`ci-notes.md`](ci-notes.md) is load-bearing for Arc05 ordering:

- CMake/CTest remains the C++ test runner for first-party runtime and seam
  tests; Arc07 must decide how parser, grounder, and engine internals enter
  first-party unit/seam coverage.
- Catch2 remains optional, local, and test-only until Arc07 revisits whether
  first-party internal unit tests require a pinned test dependency. It proves
  C++ seams, not managed-process conformance.
- Black-box process fixtures are the executable contract test framework for
  parser, grounder, and engine behavior.
- `clang-format` landed before large Arc05 C++ edits; Arc07 must expand
  formatting policy across first-party maintained source and decide generated
  output handling by generator ownership, root cause, correctness impact, and
  documented remediation path.
- ASan/UBSan runtime tests should become an early owned-code gate before
  executable behavior changes.
- `clang-tidy` and Clang Static Analyzer checks started with runtime and
  adapter/facade code; Arc07 must expand them across first-party parser,
  grounder, and engine code once compile databases isolate first-party files
  from generated and third-party noise.
- `llvm-cov` coverage should be planned in Arc05 but becomes most meaningful
  after migrated binaries are exercised by process fixtures.
- TSan remains a heavier later gate after subprocess, timeout/signal, and
  stream-draining fixture workloads are representative.
- Arc06 owns expanded test and CI proof: coverage, ThreadSanitizer, heavier
  sanitizer/static-analysis gates, and release-readiness evidence needed by
  later dry-runs.
- Arc07 owns first-party source-quality classification and expansion before
  tutorial/docs and release work.
- Arc09 owns release-package dry-runs, license/NOTICE checks, test-only
  dependency exclusion, wolong fetch/install/migration proof, checksums,
  manifests, and publication gates.

## 6. Project ledger

Composition rows verifying the definition of done; they open here and close
per-row in this project's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| P1 | Arc01 closes with complete parser, grounder, and engine audit reports plus a synthesis report; every report cites concrete file/line evidence, maps findings to C++ Core Guidelines rule IDs where applicable, and records clean checks as well as findings. | reproduced |
| P2 | Arc02 closes with parser, grounder, and engine library-research reports plus a combined recommendation report; every candidate is mapped to concrete audit defect classes, license/build/packaging consequences, maintenance evidence, and an adopt/pilot/hold/reject disposition. | reproduced |
| P3 | Arc03 closes with an accepted managed-process design covering CLI ergonomics, supervised-process behavior, binary naming, exit/status semantics, stdout/stderr/event output, buffering, ANSI/TTY, signals/resources, version/provenance, migration policy, and explicit incorporation or deferral of Arc02 dependency findings. | reproduced |
| P4 | Arc04 closes with any shared runtime/build substrate implemented, tested, and limited to the design-approved surface; duplicate process-policy code is routed through the shared substrate where adopted, with Arc02-selected dependencies entering only through approved facades/pilots. | reproduced |
| P5 | Arc05 closes with all three primary binaries conforming to the accepted process contract through namespaced `pandapi-*` entry points, with inherited-name shims deleted once native new-name builds and tests exist, without library availability expanding optional inherited surfaces. | reproduced |
| P6 | Arc06 closes with the full local and CI gate suite proving positive and negative behavior under both CLI and pipe-supervised invocation, including expanded process fixtures, coverage evidence, compiler-warning disposition/burndown, and heavier sanitizer/static-analysis gates where supported. | reproduced |
| P7 | Arc07 closes with parser, grounder, engine, runtime, and Chengdu-owned generators/templates classified and covered by explicit first-party source-quality policy: formatting, static analysis, coverage, unit/seam tests, warning policy, sanitizer triage, generated-code handling, generated-warning root-cause/correctness triage, and third-party exclusions or separately reported evidence. | reproduced |
| P8 | Arc08 closes with the pandaPI 0.3.0 tutorial and documentation suite: beginner HTN/PDDL/HDDL onboarding, simple-to-intermediate project workflow examples, `pandapi-*` CLI guidance, README updates, architecture docs, dependency rationale, source-quality posture, behavior-change table, and migration notes. | reproduced |
| P9 | `v0.3.0` is published only after release assets, checksums, manifest/provenance, dependency licensing/NOTICE obligations, test-only dependency exclusion, source-quality release gates, release docs, and the new wolong fetch/install/migration path are verified on supported platforms. | reproduced |

## 7. Open questions and risks

- **OQ1 - binary rename migration resolved.** Operator decision on
  2026-08-10 supersedes the earlier compatibility-transition assumption.
  `v0.3.0` is a breaking interface/behavior release with canonical
  `pandapi-*` names. Inherited `pandaPI*` command compatibility is not a
  release requirement; temporary shims should be deleted after the owning
  binary builds and passes tests under its new name. Arc08 must carry
  migration notes, and Arc09 must carry release-package updates and wolong
  verification for the new interface.
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
- **OQ6 - tutorial example selection.** Arc08 must choose examples that are
  beginner-friendly without being toy-only. The roadmap requirement is fixed:
  start with HTN/PDDL/HDDL fundamentals, then build from a simple pandaPI
  project workflow to an intermediate example that exercises the new 0.3.0
  CLI tools and documented architecture.
- **OQ7 - first-party source-quality scope.** Arc07 must classify parser,
  grounder, engine, runtime, generated source, generators/templates,
  dependency-internal source, third-party generated output, and copied build
  artifacts before expanding coverage/static-analysis/format/unit-test gates.
  The release policy is fixed: first-party maintained code is in scope;
  third-party source is excluded or separately reported unless a dependency
  audit explicitly accepts that boundary.

## 8. Version history

- **v1.72 - 2026-08-13.** Marked Arc07
  source-quality-expansion CC proposed done, awaiting CDC verification.
  Surfaced by: Arc07 Slice08 source-quality-synthesis. Why: Slice08 composes
  A1-A9, separates strict release evidence from measured baselines and
  deferred/post-0.3.0 budgets, preserves dependency boundaries, and gives
  Arc08/Arc09 concrete handoff responsibilities without changing public docs,
  release tooling, CI, Make/tooling, or implementation source.
- **v1.71 - 2026-08-13.** Opened Arc07 Slice08
  source-quality-synthesis. Surfaced by: Arc07 Slice07 CDC verification and
  closing-report bubble-up. Why: the completed Arc07 source-quality slices now
  need an evidence-composition pass to decide whether Arc07 can close, which
  remaining source-quality items are `v0.3.0` release blockers, which are
  measured baselines or post-0.3.0 budgets, and what Arc08 documentation plus
  Arc09 release prep must consume.
- **v1.70 - 2026-08-13.** Marked Arc07 Slice07
  grounder-source-quality-boundary closed and CDC-verified. Surfaced by:
  Slice07 CDC verification. Why: grounder now has Make-backed source-quality
  targets, explicit first-party/dependency selection, first-party warning
  count 0, `rss.c` warning closure, direct topsort seam/unit and ASan/UBSan
  evidence, managed grounder/pipeline regression proof, and separately routed
  cpddl/H2/nested third-party/generated/copied/build-output budgets. Arc07
  source-quality synthesis is the recommended next scoped ledger.
- **v1.69 - 2026-08-13.** Opened Arc07 Slice07
  grounder-source-quality-boundary. Surfaced by: Slice06 CDC verification.
  Why: parser generated scanner warnings are now closed, so Arc07 moves to
  grounder component gates and dependency-boundary proof before stronger
  source-quality claims are made over cpddl, H2, nested third-party, generated,
  copied-build, or build-output warning classes. The slice also forces the
  `rss.c` classification ambiguity to be resolved explicitly.
- **v1.68 - 2026-08-13.** Marked Arc07 Slice06
  parser-generated-warning-root-cause closed and CDC-verified. Surfaced by:
  CDC reproduction of CC commit `39c46101`. Why: parser Flex helper warnings
  are fixed at the scanner input without hand-editing generated output,
  parser and pipeline managed contracts still pass, first-party warning
  inventory dropped to 52 warning lines, and parser gengetopt/CLI11 work is
  preserved as separate re-entry while Arc07 moves toward grounder
  first-party/dependency-boundary follow-up.
- **v1.67 - 2026-08-13.** Opened Arc07 Slice06
  parser-generated-warning-root-cause. Surfaced by: Slice05 CDC verification.
  Why: Arc07 can now move from the engine component target pattern to the
  remaining parser generated-warning budget, using root-cause and correctness
  evidence before deciding whether to fix generator inputs, preserve CLI11
  re-entry, or route third-party skeleton output as an explicit exclusion.
- **v1.66 - 2026-08-12.** Marked Arc07 Slice05
  engine-first-party-quality-burndown closed and CDC-verified. Surfaced by:
  Slice05 CDC verification. Why: engine first-party source now has executable
  component source-quality targets, seam evidence for the primary warning
  burndown, sanitizer/coverage/warning baselines, and managed-contract
  regression proof, so Arc07 can move next to parser generated-warning
  root-cause analysis.
- **v1.65 - 2026-08-12.** Opened Arc07 Slice05
  engine-first-party-quality-burndown. Surfaced by: Slice04 CDC verification.
  Why: Arc07 can now move from scaffold to the first component burndown,
  starting with engine first-party maintained source, focused `visited_list`
  seam evidence, primary warning cleanup, and executable engine component gate
  targets.
- **v1.64 - 2026-08-12.** Marked Arc07 Slice04
  first-party-quality-gate-scaffold closed and CDC-verified, and selected
  engine first-party maintained source as the first Slice05 component
  burndown. Surfaced by: Slice04 CDC verification. Why: Arc07 now has
  Make-backed scaffold targets and durable re-entry evidence for first-party
  format, static analysis, coverage, unit/seam tests, warning inventory,
  generated-warning triage, sanitizer routing, and optional IPC corpus
  placement.
- **v1.63 - 2026-08-12.** Opened Arc07 Slice04
  first-party-quality-gate-scaffold. Surfaced by: Slice03 CDC verification and
  generated-warning policy clarification. Why: later component burndown slices
  need named Make-backed gate families, durable status/re-entry reports,
  generated-warning triage records, and optional IPC corpus placement before
  enforcing first-party quality findings.
- **v1.62 - 2026-08-12.** Made generated-warning root-cause and correctness
  triage explicit for Arc07. Surfaced by: operator clarification after Arc07
  Slice03 CDC verification. Why: generated-code warning work must analyze
  whether generated code is semantically correct and fix Chengdu-owned
  generators/templates at the source rather than merely suppress emitted
  warnings.
- **v1.61 - 2026-08-12.** Marked Arc07 Slice03
  first-party-source-naming-normalization closed and CDC-verified. Surfaced by:
  Slice03 CDC verification. Why: first-party maintained parser, grounder, and
  engine source names are now normalized and Make-checkable, allowing Slice04
  first-party-quality-gate-scaffold to open next.
- **v1.60 - 2026-08-12.** Opened Arc07 Slice03
  first-party-source-naming-normalization. Surfaced by: Slice02 CDC
  verification and operator acceptance of the naming-normalization insertion.
  Why: lower snake case first-party source names should be documented and
  Make-checkable before Arc07 quality gate scaffolding makes paths into
  enforcement surfaces.
- **v1.59 - 2026-08-12.** Marked Arc07 Slice02
  source-layout-and-build-surface-normalization closed and CDC-verified, and
  inserted first-party source naming normalization before the quality gate
  scaffold. Surfaced by: Slice02 CDC verification plus operator naming policy
  discussion. Why: first-party C/C++ file and directory paths should be
  normalized to lower snake case before Arc07 selectors, compile databases,
  coverage maps, and static-analysis paths become enforcement anchors.
- **v1.58 - 2026-08-12.** Opened Arc07 Slice02
  source-layout-and-build-surface-normalization. Surfaced by: operator
  acceptance of Slice01's recommendation. Why: broader first-party quality
  gates need Make-backed selectors, compile databases, profile/source mapping,
  and dependency/generated exclusions before thresholds.
- **v1.57 - 2026-08-12.** Marked Arc07 Slice01
  source-classification-inventory closed and CDC-verified. Surfaced by:
  Slice01 CDC verification. Why: the classification report accepts
  source-layout/build-surface normalization as the recommended next work before
  broader first-party quality gate expansion.
- **v1.56 - 2026-08-12.** Inserted Arc07 `source-quality-expansion`, opened
  Slice01 source-classification-inventory, and renumbered tutorial/docs to
  Arc08 and release-prep/publication to Arc09. Surfaced by: operator policy
  correction after Arc06 closure. Why: all in-tree pandaPI product code is
  Chengdu-owned in the fork/product sense, and first-party parser, grounder,
  engine, runtime, and Chengdu-owned generators/templates need explicit
  formatting, static-analysis, coverage, unit/seam-test, warning, sanitizer,
  generated-code, and third-party-exclusion policy before release.

- **v1.55 - 2026-08-12.** Marked Arc06 Slice07 tsan-and-ci-synthesis closed
  and CDC-verified, and closed Arc06. Surfaced by: CDC reproduction of CC
  commit `a4e52a4f`. Why: TSan is deferred with concrete workload/re-entry
  evidence, no symbolic TSan target was added, and CDC reproduced the composed
  Arc06 coverage, warning, sanitizer, static-analysis, managed fixture, smoke,
  provenance, workflow, and diff gates.

- **v1.54 - 2026-08-12.** Marked Arc06 CC proposed done after Slice07
  tsan-and-ci-synthesis. Surfaced by: CC Slice07 implementation. Why: the
  final Arc06 slice explicitly defers TSan until a representative supported
  multithreaded workload exists, records final CI/local/pre-release gate
  placement, and writes the Arc06 closing report with Arc07 documentation and
  Arc08 release-prep handoff inputs. CDC verification remains required before
  independent Arc06 closure.

- **v1.53 - 2026-08-12.** Marked Arc06 Slice06
  binary-sanitizer-gates closed and CDC-verified, and opened Arc06 Slice07
  tsan-and-ci-synthesis. Surfaced by: CDC reproduction of CC commit
  `95f98412`. Why: ASan/UBSan now runs through Make against canonical
  sanitizer-built `pandapi-*` binaries and representative managed fixtures,
  while macOS LSan is an explicit Linux/toolchain re-entry. The final Arc06
  task is TSan disposition and CI/handoff synthesis.

- **v1.52 - 2026-08-12.** Marked Arc06 Slice05
  compiler-warning-burndown closed and CDC-verified, and opened Arc06 Slice06
  binary-sanitizer-gates. Surfaced by: CDC reproduction of CC commits
  `fc8b671d` and `85a396ff`. Why: parser, grounder, engine, runtime,
  generated, and nested dependency warning classes are now inventoried by
  ownership tier, with low-risk primary warnings fixed and remaining budgets
  explicit enough for sanitizer gates to provide release-quality signal.

- **v1.51 - 2026-08-11.** Marked Arc06 Slice04 static-analysis-gate closed and
  CDC-verified, and opened Arc06 Slice05 compiler-warning-burndown. Surfaced
  by: CDC reproduction of CC commit `04491682`. Why: Make-backed owned runtime
  C++ static analysis is now real rather than deferred, while parser,
  grounder, and engine builds still emit visible inherited warning debt.
  Warning ownership and release budget must be settled before sanitizer and
  release-readiness gates depend on clean build signal.

- **v1.50 - 2026-08-11.** Marked Arc06 Slice03 coverage-gate closed and
  CDC-verified, and opened Arc06 Slice04 static-analysis-gate. Surfaced by:
  CDC reproduction of CC commit `dffa6baf`. Why: `make coverage` now produces
  scoped owned-runtime coverage reports with documented exclusions, generated
  report hygiene, and an explicit adoption-seam deferral. Static analysis is
  the next local/CI hardening layer before compiler-warning burndown and
  sanitizer ownership are treated as release-quality evidence.

- **v1.49 - 2026-08-11.** Marked Arc06 Slice02
  process-fixture-expansion closed and CDC-verified, and opened Arc06 Slice03
  coverage-gate. Surfaced by: CDC reproduction of CC commit `89991120` on top
  of warning-burndown planning commit `ca7cccae`. Why: managed parser,
  grounder, engine, and pipeline fixtures now provide a representative
  workload for scoped coverage reporting over `pandaPI/runtime` and stable
  chengdu-owned process-policy/adoption seams.

- **v1.48 - 2026-08-11.** Added explicit Arc06
  compiler-warning-burndown scope before release publication. Surfaced by:
  operator release-readiness review during Arc06. Why: Arc01 already recorded
  warning debt across parser `P-011`, grounder `G-015`, and engine `E-011`,
  but the roadmap only named static-analysis and sanitizer gates. Arc06 now
  carries warning inventory, primary-source burndown, suppression/budget
  policy, and release re-entry routing before Arc08 publication.

- **v1.47 - 2026-08-11.** Marked Arc06 Slice01 fixture-gap-inventory closed
  and CDC-verified, and opened Arc06 Slice02 process-fixture-expansion.
  Surfaced by: CDC reproduction of CC commit `91cc50b6`. Why: the inventory
  showed current managed component coverage is strong but managed pipeline
  fixtures are absent, so Arc06 can now expand representative process fixtures
  before coverage, sanitizer, static-analysis, and TSan gates.

- **v1.46 - 2026-08-11.** Opened Arc06 `ci-and-test-hardening` with
  Slice01 fixture-gap-inventory. Surfaced by: Arc05 close and post-Arc05
  source layout normalization. Why: primary native `pandapi-*` binary
  conformance is closed locally, so the next project ledger risk is release
  confidence: fixture coverage gaps, Make/CI proof, coverage evidence,
  heavier static/sanitizer gates, and TSan disposition before Arc07 docs and
  Arc08 publication.

- **v1.45 - 2026-08-11.** Closed Arc05
  binary-contract-adoption. Marked Slice04 grounder-contract-adoption
  CDC-verified by supersession, marked Slice08 engine-contract-adoption closed
  and CDC-verified after a CDC corrective sentinel-fencing patch, and recorded
  the Arc05 closing report as Slice09 synthesis. Surfaced by: CDC reproduction
  of native parser, grounder, and engine managed-contract evidence on
  `release/0.3.x`. Why: the 0.3.0 local primary binary surface is now
  canonical `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`, while
  CI hardening, tutorial/docs, release packaging, and wolong proof remain
  owned by downstream arcs.

- **v1.44 - 2026-08-10.** Marked Arc05 Slice07
  grounder-native-contract-cutover closed and CDC-verified, and opened Arc05
  Slice08 engine-contract-adoption. Surfaced by: CDC reproduction of CC
  implementation commit `43151b5f`. Why: parser and grounder now both have
  native canonical managed-process artifacts, leaving inherited
  `pandaPIengine` as the final primary-binary adoption target before Arc05
  synthesis.

- **v1.43 - 2026-08-10.** Marked Arc05 Slice06
  parser-native-contract-cutover closed and CDC-verified, and opened Arc05
  Slice07 grounder-native-contract-cutover. Surfaced by: CDC reproduction of
  CC implementation commit `b70b0825`. Why: parser no longer depends on the
  transition adapter or inherited old-name artifacts, and the remaining
  grounder shim/sidecar work now has its own ledgered slice before engine
  adoption resumes.

- **v1.42 - 2026-08-10.** Recorded Arc05 Slice05 as closed with deferred
  implementation and opened Arc05 Slice06 parser-native-contract-cutover.
  Surfaced by: Slice05 stop condition and CDC reproduction. Why: parser and
  grounder managed behavior still depended on Bash adapters plus raw sidecars;
  native parser cutover must happen before the parser shim and old parser
  artifacts can be deleted, with grounder receiving its own following native
  cutover slice.

- **v1.41 - 2026-08-10.** Inserted Arc05 Slice05
  canonical-binary-cutover and renumbered Arc05 engine/synthesis work after
  it. Surfaced by: operator scope correction after parser/grounder managed
  behavior existed under canonical names. Why: the project must delete
  parser/grounder transition shims and move active build, test, fixture,
  tooling, package-helper, README, and planning surfaces to canonical
  `pandapi-parser`/`pandapi-grounder` before continuing regular engine
  adoption work.

- **v1.40 - 2026-08-10.** Recorded operator override that `v0.3.0` is not an
  inherited-name bridge release. Surfaced by: operator correction during
  Arc05 Slice04 review. Why: `v0.2.0` is the transition release from
  upstream-style pandaPI to chengdu distribution; `v0.3.0` intentionally
  rewrites internal and external process behavior around `pandapi-*`, so
  shim-based `pandaPI*` compatibility would create technical debt without
  product value.

- **v1.39 - 2026-08-10.** Opened Arc05 Slice04
  grounder-contract-adoption. Surfaced by: Arc05 Slice03 CDC verification.
  Why: accepted parser managed-process evidence and make-targeted CI allow the
  next binary adoption slice to migrate normal `.htn` grounding through
  canonical `pandapi-grounder`, while fencing H2 and `cpddl`/FAM and keeping
  engine behavior baseline-only. Earlier inherited-command acceptance is
  superseded by v1.40 and v1.41.

- **v1.38 - 2026-08-10.** Marked Arc05 Slice03
  parser-contract-adoption closed and CDC-verified after independent review
  of CC implementation commit `5c807016`, make-target follow-up commit
  `1d91a1ae`, and CDC evidence repair commit `fe4efbfa`. Surfaced by:
  Slice03 CDC verification. Why: parser managed-process behavior is now
  accepted with canonical `pandapi-parser`, executable black-box contract
  fixtures, and make-targeted CI entrypoints; the earlier inherited-command
  acceptance is superseded by v1.40 and v1.41, and Slice04
  grounder-contract-adoption can open from accepted parser evidence.

- **v1.37 - 2026-08-10.** Opened Arc05 Slice03
  parser-contract-adoption. Surfaced by: Arc05 Slice02 CDC verification. Why:
  parser adoption can now begin from the accepted black-box fixture scaffold,
  making `pandapi-parser` the first executable managed-process contract
  surface. The earlier inherited-command assumption is superseded by v1.40 and
  v1.41.
- **v1.36 - 2026-08-10.** Marked Arc05 Slice02
  contract-fixture-scaffold closed and CDC-verified after independent review
  of implementation commit `836fa434` plus F-2 repair commit `f74a7f7a`.
  Surfaced by: Slice02 CDC verification. Why: Arc05 now has an accepted
  black-box baseline fixture scaffold for inherited parser, grounder, engine,
  and pipeline process behavior; Slice03 parser-contract-adoption can open
  from concrete executable baseline evidence.
- **v1.35 - 2026-08-10.** Opened Arc05 Slice02
  contract-fixture-scaffold. Surfaced by: Arc05 Slice01 CDC verification.
  Why: binary adoption now needs a repo-level black-box fixture scaffold that
  records inherited baseline behavior separately from future managed-process
  contract expectations.
- **v1.34 - 2026-08-10.** Marked Arc05 Slice01
  quality-tooling-runway closed and CDC-verified. Surfaced by: Slice01 CDC
  verification of commit `a701b32e9802a171936c5d07ddc4fd75c8f9af13`. Why:
  Arc05 now has an accepted owned-source format, runtime sanitizer,
  compile-command, and CI runway before binary behavior adoption begins;
  Slice02 `contract-fixture-scaffold` can open without a project-roadmap
  change.
- **v1.33 - 2026-08-10.** Split the former Arc06 release-hardening bucket
  into Arc06 `ci-and-test-hardening`, Arc07 `pandapi-tutorial-docs`, and
  Arc08 `release-prep-publication`; expanded the project DoD and ledger for
  the tutorial, README, architecture, dependency, migration, CI, and release
  proof surfaces. Surfaced by: operator roadmap correction after re-reading
  the 0.3.0 plan and accepting `ci-notes.md`. Why: 0.3.0's architecture and
  CLI changes are large enough that CI hardening, newcomer/tutorial docs, and
  release publication need separate arc-scale composition checks.
- **v1.32 - 2026-08-10.** Opened Arc05 binary-contract-adoption and Slice01
  quality-tooling-runway. Surfaced by: Arc04 close plus accepted
  `ci-notes.md`. Why: the project should put owned-source formatting,
  sanitizer, runtime/build, and CI runway in place before changing parser,
  grounder, or engine executable behavior, while keeping coverage, TSan,
  release packaging, license/NOTICE, and wolong proof routed to later Arc05
  or Arc06 gates.
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
  naming/version/migration semantics; 0.3.0 originally carried a transition
  assumption with
  canonical `pandapi-*` commands and inherited `pandaPI*` commands retained,
  while `--supervised`, `--status`, help/version/provenance, CLI parse-error,
  and color behavior are accepted. Superseded by v1.40 operator override:
  `v0.3.0` is not an inherited-name bridge release.
- **v1.18 - 2026-08-09.** Opened Arc03 slice04
  cli-naming-version-migration. Surfaced by: slice03 CDC verification. Why:
  Arc03 can now decide `pandapi-*` command names, inherited-name compatibility,
  help/version/provenance, CLI parse-error behavior, no-color/no-colour
  controls, machine-status enablement, CLI11 adoption gates, and wolong
  migration implications from accepted surface, status, and stream contracts.
  The inherited-name compatibility assumption is superseded by v1.40.
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
