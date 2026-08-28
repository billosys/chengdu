# chengdu arc03 / slice01 - supported-surface-classification - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Produce the Arc03 supported-surface classification report:

`docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`

This report decides which inherited parser, grounder, and engine surfaces are
in the supported 0.3.0 managed-process contract and which are legacy,
experimental, unsupported, or future work. It is the first Arc03 design slice
because status, stream, CLI, event, dependency, and test semantics must not be
written around surfaces the product has not accepted.

## 2. Scope

**In:**

- Arc01 audit synthesis section 4 supported-surface table and section 8 Arc03
  inputs/non-inputs.
- Arc01 parser, grounder, and engine audit findings that affect product
  surface: `P-001` through `P-015`, `G-001` through `G-019`, and `E-001`
  through `E-016` where relevant.
- Arc02 combined recommendations for optional/dependency-heavy paths:
  reproc++, nlohmann/json, CUDD/BDD/SAT, H2/cpddl, parser generators, CLI11,
  and fmt where they affect surface classification.
- Classification of at least these surfaces:
  - parser normal HDDL parse path;
  - parser verifier/output helper paths;
  - grounder normal `.htn` to planner-output path;
  - grounder H2 path;
  - grounder `cpddl`/FAM integration;
  - engine normal search path;
  - engine interactive mode;
  - engine translation mode;
  - engine SAT path;
  - engine BDD/CUDD path.
- For every non-supported or conditional surface: rationale, user-facing
  behavior expectation, re-entry condition, and owning future arc/slice if
  re-entered.
- Arc03 bubble-up: which later Arc03 slices must account for accepted or
  deferred surfaces.

**Out:**

- No final status/exit-code taxonomy.
- No final stdout/stderr/event schema.
- No final CLI naming/version/provenance design.
- No dependency adoption decision beyond surface-classification consequences.
- No planner source changes, build changes, scripts, workflows, release assets,
  or dependency vendoring.

## 3. Classification vocabulary

Use exactly these dispositions:

- `supported`: part of the 0.3.0 managed-process contract.
- `legacy`: retained for compatibility or historical CLI behavior but not part
  of the supervised-process contract unless explicitly invoked.
- `experimental`: visible only behind explicit flags or docs warnings; no
  stability promise.
- `unsupported`: rejected from the 0.3.0 contract.
- `future`: potentially valuable, but deferred to a later explicit slice or
  project.

If a surface is conditional, choose the most honest primary disposition and put
the condition in the rationale/re-entry fields rather than inventing a sixth
label.

## 4. Constraints

- Do not let dependency availability decide product semantics. For example,
  reproc++ cannot make H2 or translation supported, and CUDD availability cannot
  make BDD supported.
- Preserve wolong's frozen fetch/install shape unless an explicit operator
  decision accepts a migration.
- Treat normal parser, normal grounder, and normal engine search paths as the
  default supported candidates unless the report finds a concrete blocker.
- Be conservative with inherited optional paths. If a path cannot be made
  deterministic, supervised, documented, and testable in 0.3.0, classify it
  as legacy, experimental, unsupported, or future with re-entry conditions.
- Record unknowns explicitly. Do not convert "not audited deeply enough" into
  "supported" by silence.

## 5. Verification Approach

CC verifies by producing the classification report, updating the ledger, and
writing a closing report with a row-by-row walk and bubble-up to Arc03. CDC
verifies row count, required surface coverage, audit finding citations,
dependency non-determinism boundaries, re-entry conditions, and protected-path
cleanliness.

## 6. Exit Criteria

The ledger reaches final status. The classification report exists at the
planned path, covers every required surface, cites Arc01/Arc02 evidence,
uses the agreed classification vocabulary, records rationale and re-entry
conditions for non-supported/conditional surfaces, identifies inputs to later
Arc03 slices, and leaves source/build/release paths untouched.
