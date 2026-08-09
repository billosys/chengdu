# Slice 02 Closing Report - Status, Exit, and Signal Taxonomy

## 1. Outcome

Slice02 delivered the Arc03 status, exit, signal, timeout, resource,
dependency, and child-process taxonomy report at:

`docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`

The report defines a cross-binary status vocabulary, numeric exit-code table,
per-binary application notes, non-supported-surface treatment, timeout/resource
and signal semantics, dependency/child-process failure semantics, Arc02
dependency gates, and later Arc03/Arc04/Arc05/Arc06 consumers.

No planner source, dependency source, scripts, workflows, release assets, build
outputs, `vendor.env`, or `pins.env` were changed.

## 2. Ledger Row Walk

| ID | Final status | Evidence |
|----|--------------|----------|
| F-1 | done | The report exists and `test -f ... && rg -n "Arc01|Arc02|slice01|audit-synthesis-pandapi|combined-library-recommendations|supported-surface-classification" ...` returned Arc01, Arc02, slice01, and source artifact references. |
| F-2 | done | `rg -n "success|usage|input|open|read|output|write|parse|validation|unsupported|legacy|experimental|future|unsolvable|unreachable|timeout|resource|signal|interrupted|dependency|child-process|internal" ...` returned the status vocabulary and required status families. |
| F-3 | done | `rg -n "exit code|numeric|table|rationale|diagnostic prose|status family|maps? to" ...` returned the numeric exit-code table, rationale, and diagnostic-prose non-reliance rule. |
| F-4 | done | `rg -n "parser normal|HDDL parse|grounder normal|\\.htn|engine normal|normal search|solved|success|malformed|open|read|write|validation|unsolvable|unreachable" ...` returned the three supported normal-path application sections. |
| F-5 | done | `rg -n "parser verifier|output helper|H2|cpddl|FAM|interactive|translation|SAT|BDD|CUDD|legacy|experimental|unsupported|future" ...` returned deterministic treatment for every visible non-supported surface from slice01. |
| F-6 | done | `rg -n "timeout|resource|signal|SIGINT|SIGTERM|interrupt|interruption|supervised|internal defect|unsolvable|unreachable" ...` returned the timeout/resource/signal/interruption section and distinctions from no-plan and internal-defect outcomes. |
| F-7 | done | `rg -n "tl::expected|StatusResult|reproc|nlohmann/json|JSON|CLI11|fmt|Catch2|process fixture|Abseil|Boost\\.Process|held|rejected|conditional|slice03|slice04|slice05|Arc04" ...` returned the Arc02 dependency-gating section. |
| F-8 | done | `rg -n "slice03|stdio|event|payload|slice04|CLI|parse error|slice05|contract test|Arc04|status/result|shared substrate|Arc05|per-binary|Arc06|release docs|behavior-change" ...` returned later slice and arc consumer sections. |
| F-9 | done | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output. |

No ledger rows were dropped, deferred, or marked no-op.

## 3. Delivered Taxonomy

The taxonomy establishes these status names:

- `ok`
- `domain_no_plan`
- `cli_usage_error`
- `input_unavailable`
- `output_unavailable`
- `input_invalid`
- `unsupported_feature`
- `legacy_surface`
- `experimental_surface`
- `future_surface`
- `timeout`
- `resource_limit`
- `interrupted`
- `dependency_failure`
- `child_process_failure`
- `internal_error`
- `signal_terminated`

It maps those statuses to numeric exit codes with explicit ranges:
success/domain outcome, caller/input, policy/surface, runtime/resource,
dependency/child, internal defect, and supervisor-visible signal termination.

## 4. Bubble-up to the Arc

### Assigned Arc Capability

Arc03 assigned slice02 to define process statuses, exit codes, error payload
classes, timeout/resource/signal semantics, and dependency/child-process
failure handling while preserving Arc02 gates. This slice delivered that
piece of the arc capability and gives slice03 through slice05 a concrete
status vocabulary to consume.

### New Arc Findings

The slice sharpened several later decisions:

- Exit code `1` should be reserved as a compatibility fallback during
  migration, not used by the new 0.3.0 contract for classified outcomes.
- `domain_no_plan` should keep a stable nonzero code distinct from
  `timeout` and `resource_limit`.
- `interrupted` is binary-owned and maps through the status mapper; unhandled
  OS termination remains supervisor-owned as `128 + signal_number`.
- H2 and translation child/dependency failures are defined only for visible
  experimental paths and do not make reproc++ a default requirement.
- slice03 must choose where the named status payload fields are emitted before
  nlohmann/json can re-enter.

These findings fit the existing Arc03 slice breakdown. They do not require an
`arc-plan.md` body change before slice03, because the current plan already has
slice03 consuming status payloads for stdio/event/TTY policy and Arc04
consuming the final taxonomy for the shared status/result substrate.

### Silent-Drop Diff

Scope as specified:

- Create the status/exit/signal taxonomy report.
- Cover all required status families.
- Apply the taxonomy to supported parser, grounder, and engine normal paths.
- Define deterministic treatment for visible legacy, experimental,
  unsupported, and future surfaces from slice01.
- Define timeout, resource, signal, interruption, dependency, and child-process
  semantics.
- Preserve Arc02 dependency gates.
- Route later slice and arc consumers.
- Leave source/build/release/protected paths untouched.

Scope as delivered:

- All specified items were delivered.
- No source, script, workflow, release, build, `vendor.env`, or `pins.env`
  path was changed.
- CDC verification remains intentionally unfilled for the independent reviewer.

No silent drops identified.
