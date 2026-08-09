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

**Required first step:** no source redesign starts until Arc01 produces one
complete audit report for each of the three pandaPI repos now vendored in this
monorepo, followed by a synthesis report. The synthesis must address best
practices, duplicate code, shared-library/header opportunities, build-system
shape, and candidate third-party C/C++ libraries that would materially improve
the fork.

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

## 4. Arc roadmap

| Arc | Slug | Capability (one line) | Depends on |
|-----|------|----------------------|------------|
| arc01 | `vendored-source-audit` | Complete per-repo audits for parser, grounder, and engine, then synthesize cross-codebase recommendations for cleanup, shared code, process contracts, and dependency choices. | 0.2.0 closed fork baseline |
| arc02 | `cpp-library-research` | Research open source C++ libraries against the parser, grounder, and engine audit findings; produce per-component assessments and a final combined dependency recommendation set. | arc01 per-component audit reports; closes before managed-process design |
| arc03 | `managed-process-contract` | Produce the accepted design for CLI + supervised-process behavior: command naming, exit/status taxonomy, stdout/stderr/events, buffering, ANSI/TTY, signals/resources, version/provenance, and migration policy. | arc01, arc02 |
| arc04 | `shared-runtime-substrate` | Introduce the shared C/C++ runtime/build substrate selected by the design, with tests and no behavior changes beyond wiring. | arc03 |
| arc05 | `binary-contract-adoption` | Migrate parser, grounder, and engine onto the shared process contract and namespaced entry points, preserving or explicitly migrating compatibility surfaces. | arc04 |
| arc06 | `release-hardening` | Prove the new behavior locally and in CI, update docs/release assets/migration notes, and publish `v0.3.0` only after wolong-oriented consumer verification. | arc05 |

Detailed planning is intentionally open only for arc01 and arc02. Later arcs
will be planned late, from the audit, library-research, and design evidence,
not from today's guesses.

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
- **arc03 - ready for detailed planning.** Arc01 and Arc02 are closed, and
  their accepted synthesis/recommendation reports are now verified inputs.
- **arc04-arc06 - roadmap only.** Do not write their detailed arc plans until
  Arc03 closes or explicitly bubbles up a planning dependency.

## 6. Project ledger

Composition rows verifying the definition of done; they open here and close
per-row in this project's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| P1 | Arc01 closes with complete parser, grounder, and engine audit reports plus a synthesis report; every report cites concrete file/line evidence, maps findings to C++ Core Guidelines rule IDs where applicable, and records clean checks as well as findings. | reproduced |
| P2 | Arc02 closes with parser, grounder, and engine library-research reports plus a combined recommendation report; every candidate is mapped to concrete audit defect classes, license/build/packaging consequences, maintenance evidence, and an adopt/pilot/hold/reject disposition. | reproduced |
| P3 | Arc03 closes with an accepted managed-process design covering CLI ergonomics, supervised-process behavior, binary naming, exit/status semantics, stdout/stderr/event output, buffering, ANSI/TTY, signals/resources, version/provenance, and migration policy. | reproduced |
| P4 | Arc04 closes with any shared runtime/build substrate implemented, tested, and limited to the design-approved surface; duplicate process-policy code is routed through the shared substrate where adopted. | reproduced |
| P5 | Arc05 closes with all three primary binaries conforming to the accepted process contract, including namespaced `pandapi-*` entry points or an explicit compatibility transition. | reproduced |
| P6 | The full local and CI gate suite proves positive and negative behavior under both CLI and pipe-supervised invocation; release docs include a behavior-change table and wolong migration evidence. | reproduced |
| P7 | `v0.3.0` is published only after release assets, checksums, manifest/provenance, licensing, and the wolong fetch/install/migration path are verified on supported platforms. | reproduced |

## 7. Open questions and risks

- **OQ1 - binary rename migration.** The operator wants `pandapi-*` names to
  avoid system conflicts. Arc03 must decide whether 0.3.0 ships dual names for
  compatibility, wrappers/aliases, or an explicit breaking change to tarball
  contents and README command paths.
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
