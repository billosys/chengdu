# chengdu arc03 - managed-process-contract - arc plan

> Plan-of-record for arc03, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc begins after Arc01
> `vendored-source-audit` and Arc02 `cpp-library-research` are closed.
>
> Status: **closed** on 2026-08-09. Closing report:
> [`closing-report.md`](closing-report.md).

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
8. **Arc02 findings are design constraints.** Standard-library modernization,
   fmt, CLI11, Catch2, `tl::expected`, conditional reproc++, and held/rejected
   candidates must be routed through the contract deliberately. Arc03 may
   define the semantics those libraries would implement; it must not let a
   library choice pull status, stream, CLI, test, or optional-surface semantics
   across slice boundaries.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `supported-surface-classification` | Classify every inherited parser/grounder/engine surface as supported, legacy, experimental, unsupported, or future work for 0.3.0. | all later Arc03 slices; Arc04 substrate scope; Arc05 adoption scope |
| slice02 | `status-exit-signal-taxonomy` | Define process statuses, exit codes, error payload classes, timeout/resource/signal semantics, and dependency/child-process failure handling, while preserving Arc02 gates around `tl::expected`, reproc++, JSON, CLI11, fmt, and test dependencies. | slice06 final contract; Arc04 status/result substrate |
| slice03 | `stdio-event-tty-contract` | Define stdout/stderr ownership, event-output mode, buffering/flushing, diagnostics/progress routing, quiet modes, and ANSI/color/TTY policy, including whether nlohmann/json re-enters for event output and how fmt remains behind a facade. | slice06 final contract; Arc04 diagnostics substrate; Arc05 behavior tests |
| slice04 | `cli-naming-version-migration` | Define `pandapi-*` command names, compatibility aliases/wrappers, help/version/provenance output, parse-error behavior, CLI11 migration constraints, and wolong migration policy with golden-output compatibility rules. | slice06 final contract; Arc05 binary naming/adoption; Arc06 release docs |
| slice05 | `contract-test-matrix` | Define golden process fixtures and acceptance probes for human CLI and supervised-process behavior across parser, grounder, and engine, plus the split between process fixtures and Catch2 seam tests. | slice06 final contract; Arc04 test harness; Arc05 per-binary gates |
| slice06 | `managed-process-contract-synthesis` | Produce the final accepted `managed-process-contract.md` and route explicit Arc02-informed implementation inputs to Arc04, Arc05, and Arc06. | Arc04 detailed planning; project ledger P3 |

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

Arc02-specific Arc04 inputs must include: standard-library modernization as the
default C++17 baseline; fmt only under a diagnostics/process I/O facade; CLI11
only behind accepted CLI golden tests; Catch2 as test-only seam coverage paired
with process fixtures; `tl::expected` only behind a local status/result alias
after slice02; and reproc++ only behind a child-process adapter if a supported
surface needs it.

**Leaves for arc05:** binary-by-binary adoption order, compatibility/migration
requirements, accepted behavior-change table inputs, and per-binary contract
test obligations. Arc05 must keep algorithmic code separated from
process-contract code and must not un-fence optional surfaces through library
availability.

**Leaves for arc06:** release documentation, migration proof, asset-shape
guardrails, license/NOTICE implications for adopted or piloted dependencies,
proof that test-only dependencies do not enter release tarballs, and wolong
consumer verification requirements.

## 5. Current status

- **slice01 supported-surface-classification - closed and CDC-verified.**
  Accepted report:
  [`supported-surface-classification.md`](supported-surface-classification.md).
  CDC verification:
  [`slice01-supported-surface-classification/cdc-verification.md`](slice01-supported-surface-classification/cdc-verification.md).
- **slice02 status-exit-signal-taxonomy - closed and CDC-verified.** Accepted
  report:
  [`status-exit-signal-taxonomy.md`](status-exit-signal-taxonomy.md).
  CDC verification:
  [`slice02-status-exit-signal-taxonomy/cdc-verification.md`](slice02-status-exit-signal-taxonomy/cdc-verification.md).
- **slice03 stdio-event-tty-contract - closed and CDC-verified.** Accepted
  report:
  [`stdio-event-tty-contract.md`](stdio-event-tty-contract.md).
  CDC verification:
  [`slice03-stdio-event-tty-contract/cdc-verification.md`](slice03-stdio-event-tty-contract/cdc-verification.md).
- **slice04 cli-naming-version-migration - closed and CDC-verified.**
  Accepted report:
  [`cli-naming-version-migration.md`](cli-naming-version-migration.md).
  CDC verification:
  [`slice04-cli-naming-version-migration/cdc-verification.md`](slice04-cli-naming-version-migration/cdc-verification.md).
- **slice05 contract-test-matrix - closed and CDC-verified.** Accepted report:
  [`contract-test-matrix.md`](contract-test-matrix.md).
  CDC verification:
  [`slice05-contract-test-matrix/cdc-verification.md`](slice05-contract-test-matrix/cdc-verification.md).
- **slice06 managed-process-contract-synthesis - closed and CDC-verified.**
  Accepted report:
  [`managed-process-contract.md`](managed-process-contract.md).
  CDC verification:
  [`slice06-managed-process-contract-synthesis/cdc-verification.md`](slice06-managed-process-contract-synthesis/cdc-verification.md).
- **Arc03 - closed.** Closing report:
  [`closing-report.md`](closing-report.md).
  Final accepted contract:
  [`managed-process-contract.md`](managed-process-contract.md).

## 6. Contract artifact paths

Arc03 uses arc-local design artifacts:

| Artifact | Planned path |
|----------|--------------|
| Supported surface classification | `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` |
| Status, exit, and signal taxonomy | `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md` |
| Stdio, event, and TTY contract | `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md` |
| CLI naming, version, and migration contract | `docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md` |
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

- **OQ1 - compatibility strategy resolved.** Slice04 selected canonical
  `pandapi-*` names while keeping inherited `pandaPI*` names executable in
  0.3.0 as a compatibility transition. A future breaking removal requires an
  explicit operator decision, release-note migration table, wolong
  verification, and a new design update.
- **OQ2 - event format resolved.** Slice03 selected tagged text status-only
  output for 0.3.0, not JSON Lines. `nlohmann/json` remains held unless a
  later accepted event-format decision reopens JSON.
- **OQ3 - optional surface gravity.** H2, cpddl/FAM, translation, interactive,
  SAT, BDD, and CUDD can expand architecture scope quickly. Slice01 must fence
  or accept each surface explicitly.
- **OQ4 - process-library pressure.** reproc++ remains conditional. Do not
  design a child-process dependency into Arc04 unless slice01 and slice02 prove
  a supported surface needs it.
- **OQ5 - test-first sequencing.** The design must produce a contract test
  matrix strong enough for Arc04 to build the proof harness before Arc05
  changes binary behavior.
- **OQ6 - Arc02 implementation gates.** Arc03 must decide semantics before
  implementation libraries enter. `tl::expected` is gated by the accepted
  status taxonomy, fmt waits for an Arc04 diagnostics/process I/O facade,
  nlohmann/json is held by the slice03 tagged-text decision, CLI11 waits for
  slice04 golden compatibility decisions, and Catch2/process fixtures wait for
  slice05/Arc04 test-substrate decisions.

## 9. Version history

- **v1.12 - 2026-08-09.** Closed Arc03 after arc-level composition review.
  Surfaced by: slice06 CDC verification and arc ledger A1-A8 reproduction.
  Why: all six planned slices are closed and CDC-verified, the final
  managed-process contract satisfies the arc capability, and remaining
  implementation/release/wolong proof is explicitly routed to Arc04-Arc06.
- **v1.11 - 2026-08-09.** Marked slice06
  managed-process-contract-synthesis closed and CDC-verified. Surfaced by:
  slice06 CDC verification. Why: Arc03 now has a final accepted normative
  managed-process contract and can run the arc-level composition close without
  re-discovering implementation, release, or wolong caveats.
- **v1.10 - 2026-08-09.** Opened slice06
  managed-process-contract-synthesis. Surfaced by: slice05 CDC verification.
  Why: Arc03 now has accepted supported-surface, status/exit/signal,
  stdio/event/TTY, CLI naming/version/migration, and contract-test-matrix
  inputs ready to synthesize into the final managed-process contract and route
  to Arc04/Arc05/Arc06.
- **v1.9 - 2026-08-09.** Marked slice05 contract-test-matrix closed and
  CDC-verified. Surfaced by: slice05 CDC verification. Why: slice06 can now
  synthesize the final managed-process contract from accepted surface,
  status/exit/signal, stdio/event/TTY, CLI naming/version/migration, and
  contract-test-matrix inputs.
- **v1.8 - 2026-08-09.** Opened slice05 contract-test-matrix. Surfaced by:
  slice04 CDC verification. Why: Arc03 can now convert the accepted surface,
  status/exit, stdio/event/TTY, and CLI naming/version/migration contracts
  into golden process fixture requirements, black-box versus Catch2 test
  ownership, and later Arc04/Arc05/Arc06 gates.
- **v1.7 - 2026-08-09.** Marked slice04 cli-naming-version-migration closed
  and CDC-verified. Surfaced by: slice04 CDC verification. Why: slice05 can
  now define golden process fixtures against stable command names, inherited
  compatibility entries, `--supervised`, `--status`, help/version/provenance,
  parse-error behavior, and color controls.
- **v1.6 - 2026-08-09.** Opened slice04 cli-naming-version-migration.
  Surfaced by: slice03 CDC verification. Why: command names, compatibility
  aliases/wrappers, help/version/provenance, CLI parse-error behavior,
  no-color/no-colour controls, machine-status enablement, and CLI11 adoption
  gates can now be planned against the accepted supported-surface,
  status/exit, and stdio/event/TTY contracts.
- **v1.5 - 2026-08-09.** Marked slice03 stdio-event-tty-contract closed and
  CDC-verified. Surfaced by: slice03 CDC verification after CC's corrective
  close-set commit. Why: slice04 can now map CLI names, flags, aliases,
  help/version text, no-color/no-colour behavior, and machine-status
  enablement from the accepted stream/event/TTY contract; nlohmann/json remains
  held because slice03 selected tagged text status-only output rather than
  JSON Lines.
- **v1.4 - 2026-08-09.** Opened slice03 stdio-event-tty-contract. Surfaced by:
  slice02 CDC verification. Why: stdout/stderr/event/TTY/color/buffering can
  now be designed against the accepted surface matrix and status taxonomy,
  including the Arc02 gates for fmt and nlohmann/json.
- **v1.3 - 2026-08-09.** Marked slice02 status-exit-signal-taxonomy closed and
  CDC-verified. Surfaced by: slice02 CDC verification. Why: slice03 can now
  define stdio/event/TTY/color/buffering from an accepted status vocabulary and
  exit-code map, while preserving Arc02 gates around fmt and nlohmann/json.
- **v1.2 - 2026-08-09.** Opened slice02 status-exit-signal-taxonomy and made
  Arc02 dependency findings explicit across the remaining Arc03 slice
  breakdown. Surfaced by: operator request to verify that Arc02's inserted
  research arc was fully incorporated before handing slice02 to CC. Why:
  status taxonomy must become the semantic basis for later `tl::expected`,
  JSON/event, CLI11, fmt, Catch2, and reproc++ implementation choices without
  letting those choices decide the product contract.
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
