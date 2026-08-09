# chengdu arc03 - managed-process-contract - arc plan

> Plan-of-record for arc03, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc begins after Arc01
> `vendored-source-audit` and Arc02 `cpp-library-research` are closed.

## 1. Capability statement

Roadmap line: *Produce the accepted design for CLI + supervised-process
behavior: command naming, exit/status taxonomy, stdout/stderr/events,
buffering, ANSI/TTY, signals/resources, version/provenance, and migration
policy.*

Expanded: this arc turns the verified audit synthesis and library
recommendations into the behavior contract that later implementation arcs must
obey. It decides what surfaces 0.3.0 supports, what surfaces remain legacy or
experimental, how humans and process supervisors observe the tools, how
statuses and exits are classified, how stdout/stderr/event output is owned, and
how `pandapi-*` names are introduced without silently breaking wolong's frozen
fetch/install shape.

Arc03 is design-only. It may create contract documents and planning updates; it
does not modify planner source, vendored dependency source, build scripts,
workflows, release assets, or packaging.

## 2. Design principles

1. **Supported surface first.** Do not define status, event, CLI, dependency,
   or process-library semantics for inherited paths until the arc classifies
   whether those paths are supported, legacy, experimental, unsupported, or
   future work.
2. **One contract, three binaries.** The parser, grounder, and engine may have
   component-specific result payloads, but they share one status vocabulary,
   stream policy, color/TTY policy, version/provenance policy, and supervision
   posture.
3. **Human CLI and managed process are both first-class.** Human mode may show
   friendly help and progress; supervised mode must be deterministic,
   parsable, and quiet where promised. The design must say how a caller chooses
   or observes each mode.
4. **stdout is owned.** For every command mode, stdout is either the selected
   data artifact, a documented machine-event stream, or intentionally empty.
   Human diagnostics and progress do not casually share it.
5. **Error classification beats prose.** A supervisor must classify success,
   usage defects, input/open errors, parse/model validation errors,
   unsupported features, unsolvable/unreachable results, timeout/resource
   policy failures, interrupted/signal termination, dependency/child-process
   failure, and internal defects without scraping unstructured messages.
6. **Dependencies do not decide product semantics.** CLI11, fmt, JSON,
   `tl::expected`, reproc++, Boost.Process, CUDD, H2, and cpddl availability
   cannot make an inherited path supported. Arc03 classifies the product
   surface first; later arcs choose implementation mechanisms second.
7. **Wolong compatibility is load-bearing.** Binary renaming is in scope, but
   release asset shape and install/migration behavior must remain compatible
   unless the operator explicitly accepts a breaking transition.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `supported-surface-classification` | Classify every inherited parser/grounder/engine surface as supported, legacy, experimental, unsupported, or future work for 0.3.0. | all later Arc03 slices; Arc04 substrate scope; Arc05 adoption scope |
| slice02 | `status-exit-signal-taxonomy` | Define process statuses, exit codes, error payload classes, timeout/resource/signal semantics, and dependency/child-process failure handling. | slice06 final contract; Arc04 status/result substrate |
| slice03 | `stdio-event-tty-contract` | Define stdout/stderr ownership, event-output mode, buffering/flushing, diagnostics/progress routing, quiet modes, and ANSI/color/TTY policy. | slice06 final contract; Arc04 diagnostics substrate; Arc05 behavior tests |
| slice04 | `cli-naming-version-migration` | Define `pandapi-*` command names, compatibility aliases/wrappers, help/version/provenance output, parse-error behavior, CLI11 migration constraints, and wolong migration policy. | slice06 final contract; Arc05 binary naming/adoption; Arc06 release docs |
| slice05 | `contract-test-matrix` | Define golden process fixtures and acceptance probes for human CLI and supervised-process behavior across parser, grounder, and engine. | slice06 final contract; Arc04 test harness; Arc05 per-binary gates |
| slice06 | `managed-process-contract-synthesis` | Produce the final accepted `managed-process-contract.md` and route explicit implementation inputs to Arc04, Arc05, and Arc06. | Arc04 detailed planning; project ledger P3 |

## 4. Dependencies

**Consumes:**

- Arc01 closing report:
  [`../arc01-vendored-source-audit/closing-report.md`](../arc01-vendored-source-audit/closing-report.md)
- Arc01 synthesis:
  [`../arc01-vendored-source-audit/audit-synthesis-pandapi.md`](../arc01-vendored-source-audit/audit-synthesis-pandapi.md)
- Arc02 closing report:
  [`../arc02-cpp-library-research/closing-report.md`](../arc02-cpp-library-research/closing-report.md)
- Arc02 combined library recommendations:
  [`../arc02-cpp-library-research/combined-library-recommendations.md`](../arc02-cpp-library-research/combined-library-recommendations.md)
- Current project plan definition of done and wolong frozen fetch/install
  constraint.

**Leaves for arc04:** a final managed-process contract, status taxonomy,
stream/event/color rules, CLI/naming/version/provenance policy, supported
surface matrix, test matrix, dependency exposure constraints, and explicit
implementation boundaries for the shared runtime substrate.

**Leaves for arc05:** binary-by-binary adoption order, compatibility/migration
requirements, accepted behavior-change table inputs, and per-binary contract
test obligations.

**Leaves for arc06:** release documentation, migration proof, asset-shape
guardrails, license/NOTICE implications for adopted dependencies, and wolong
consumer verification requirements.

## 5. Current status

- **slice01 supported-surface-classification - closed and CDC-verified.**
  Accepted report:
  [`supported-surface-classification.md`](supported-surface-classification.md).
  CDC verification:
  [`slice01-supported-surface-classification/cdc-verification.md`](slice01-supported-surface-classification/cdc-verification.md).
- **slice02 status-exit-signal-taxonomy - next unopened slice.** It should
  consume the accepted narrow supported surface: normal parser HDDL parse,
  normal grounder `.htn` grounding, and normal engine search. It also needs
  deterministic statuses for legacy, experimental, unsupported, and future
  surfaces that remain reachable.
- **slice03-slice06 - planned only.** Do not open their slice docs until their
  predecessors close and bubble up their design inputs.

## 6. Contract artifact paths

Arc03 uses arc-local design artifacts:

| Artifact | Planned path |
|----------|--------------|
| Supported surface classification | `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` |
| Final managed-process contract | `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md` |

Intermediate slices may produce additional arc-local reports only when their
ledger requires them. The final contract synthesis must either absorb those
reports or link them as normative appendices.

## 7. Arc ledger

Composition rows verifying the capability; open here and close per-row in this
arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Every planned Arc03 slice is closed and CDC-verified, with no missing slice from the breakdown. | reproduced |
| A2 | The supported surface matrix classifies normal parser, parser verifier/output helpers, normal grounder, H2, cpddl/FAM, normal engine search, interactive mode, translation mode, SAT, BDD, and CUDD, with rationale and re-entry conditions where not supported. | reproduced |
| A3 | The final contract defines process statuses, exit codes, error/event payload classes, timeout/resource/signal semantics, and dependency/child-process failure handling without relying on unstructured diagnostic text. | reproduced |
| A4 | The final contract defines stdout/stderr/event ownership, buffering/flushing rules, quiet/progress behavior, ANSI/color/TTY policy, and machine-mode behavior for both CLI and supervised invocation. | reproduced |
| A5 | The final contract defines `pandapi-*` binary naming, compatibility aliases or explicit migration, help/version/provenance output, CLI parse-error behavior, and wolong release-shape implications. | reproduced |
| A6 | The contract test matrix covers positive and negative probes for parser, grounder, and engine across help/version, missing args, missing input, malformed input, valid file/pipe behavior, unsolvable/unreachable, timeout/resource/signal, color, and unsupported/legacy surfaces. | reproduced |
| A7 | Arc04, Arc05, and Arc06 can be planned from the final contract without silent drops: every accepted dependency, deferred optional surface, compatibility decision, and behavior-change obligation is routed to an owning later arc or rejected/no-op with rationale. | reproduced |
| A8 | No planner source, dependency source, scripts, workflows, release assets, build outputs, or vendored dependency files are changed by this design arc except for explicitly planned docs/report artifacts. | reproduced |

## 8. Open questions and risks

- **OQ1 - compatibility strategy.** `pandapi-*` names avoid system conflicts,
  but current release assets still ship inherited binary names. Arc03 must
  decide whether 0.3.0 ships dual names, wrappers/aliases, or an explicit
  breaking change.
- **OQ2 - event format.** Machine-readable output may be JSON Lines, tagged
  text, or a narrower status-only side channel. Arc03 must choose behavior
  before nlohmann/json or any event writer is adopted.
- **OQ3 - optional surface gravity.** H2, cpddl/FAM, translation, interactive,
  SAT, BDD, and CUDD can expand architecture scope quickly. Slice01 must fence
  or accept each surface explicitly.
- **OQ4 - process-library pressure.** reproc++ remains conditional. Do not
  design a child-process dependency into Arc04 unless slice01 and slice02 prove
  a supported surface needs it.
- **OQ5 - test-first sequencing.** The design must produce a contract test
  matrix strong enough for Arc04 to build the proof harness before Arc05
  changes binary behavior.

## 9. Version history

- **v1.1 - 2026-08-09.** Marked slice01 supported-surface-classification
  closed and CDC-verified. Surfaced by: slice01 CDC verification. Why: Arc03
  can now open slice02 from an accepted product boundary: supported normal
  parser HDDL parse, normal grounder `.htn` grounding, and normal engine search;
  legacy parser helpers, grounder `cpddl`/FAM, and engine interactive mode;
  experimental H2 and engine translation; unsupported SAT; future BDD/CUDD.
- **v1.0 - 2026-08-09.** Initial Arc03 plan and slice01 open set. Source:
  closed Arc01 audit synthesis, closed Arc02 combined library recommendations,
  and project-plan v1.11. Why: the project is ready to turn verified audit and
  dependency evidence into the managed-process contract before implementation
  begins.
