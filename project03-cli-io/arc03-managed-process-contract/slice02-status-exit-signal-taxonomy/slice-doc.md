# chengdu arc03 / slice02 - status-exit-signal-taxonomy - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Produce the Arc03 status, exit, signal, timeout, resource, dependency, and
child-process taxonomy report:

`docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`

This report defines the process-status semantics that later slices and arcs
must use for the three supported 0.3.0 surfaces:

- parser normal HDDL parse;
- grounder normal `.htn` grounding;
- engine normal search.

It must also define deterministic treatment for visible legacy, experimental,
unsupported, and future surfaces classified by slice01, without expanding the
supported product contract.

## 2. Scope

**In:**

- Accepted slice01 surface classifications from
  `../supported-surface-classification.md`.
- Arc01 process-contract defect evidence for status propagation, exit codes,
  timeout/signal behavior, deep exits, usage errors, malformed input, output
  open/write failures, dependency failures, and unsolvable/unreachable results.
- Arc02 combined recommendation constraints:
  - status taxonomy must be defined before any `tl::expected` or local
    `StatusResult<T>` facade can be designed in Arc04;
  - dependency or child-process status semantics must not force reproc++,
    Boost.Process, or another process library;
  - event payload classes may be named, but JSON and nlohmann/json are held
    until slice03 decides the event/stdio contract;
  - CLI parse-error status semantics may be named, but CLI11 migration,
    help/version text, aliases, and option compatibility belong to slice04;
  - test obligations may be listed, but Catch2/process-fixture adoption belongs
    to slice05 and Arc04;
  - Abseil, Boost.Process, performance containers, parser-generator
    replacements, SAT/BDD/CUDD enablement, H2/cpddl dependency-internal work,
    and broad GSL adoption must remain rejected or held unless the report gives
    an explicit re-entry condition owned by a later arc.
- A single cross-binary process status vocabulary.
- A numeric exit-code table with rationale and reserved ranges or reserved
  meanings where needed.
- Component-specific application notes for parser, grounder, and engine.
- Timeout/resource/signal/interruption semantics for supervised invocation.
- Dependency and child-process failure semantics for legacy or experimental
  paths that remain visible.
- Later-slice inputs for slice03 stdio/events/TTY, slice04 CLI naming/version/
  migration, slice05 contract tests, and Arc04 shared status/result substrate.

**Out:**

- No stdout/stderr routing, event format, JSON Lines schema, buffering,
  flushing, TTY, or color policy; slice03 owns those.
- No final CLI option spelling, command names, aliases, help text, version text,
  or CLI11 migration plan; slice04 owns those.
- No final test matrix, fixture inventory, Catch2 harness design, or process
  runner design; slice05 and Arc04 own those.
- No dependency vendoring, build changes, planner source changes, scripts,
  workflows, release assets, or packaging changes.
- No new supported product surface beyond slice01's accepted surface.

## 3. Required Status Families

The report must define at least these status families, either with these exact
names or with a clearly justified equivalent vocabulary:

- successful completion;
- CLI usage error;
- input/open/read error;
- output/create/write error;
- parse or model validation error;
- unsupported feature or unsupported visible mode;
- legacy surface invocation under supervised mode;
- experimental surface invocation or experimental failure;
- future/dormant surface invocation if reachable;
- proven unsolvable or unreachable result;
- timeout or resource-policy failure;
- interrupted or signal termination;
- dependency or child-process failure;
- internal defect or invariant failure.

The vocabulary should make success, unsolvable/unreachable, and failure
distinguishable without scraping diagnostic prose.

## 4. Arc02 Incorporation Rules

Arc02 is a design constraint for this slice, not a background citation.

- `tl::expected` is only an implementation carrier. This slice must define the
  status and payload semantics that a future `StatusResult<T>` would carry,
  without designing the C++ API or exposing `tl::expected` directly.
- reproc++ remains conditional. Since slice01 classified H2 and translation as
  experimental rather than supported, this slice must not make child-process
  containment a default supported-path requirement. It may define how
  experimental child-process failures would be classified if the path remains
  visible.
- nlohmann/json remains held. This slice may name semantic payload fields and
  event classes, but must not choose JSON Lines or a concrete serialized event
  format.
- CLI11 remains a later implementation candidate. This slice may define parse
  error and usage-error statuses, but must leave option grammar, help/version
  output, aliases, and namespacing to slice04.
- fmt belongs under a future diagnostics/process facade. This slice must not
  define formatting mechanics or stream ownership beyond status payload
  semantics needed by slice03.
- Catch2 is test-only and process fixtures are separate. This slice may route
  test obligations, but must not make unit tests a substitute for process
  behavior probes.
- Held/rejected Arc02 candidates must stay held or rejected unless a later
  re-entry condition is explicitly named.

## 5. Verification Approach

CC verifies by producing the taxonomy report, updating the ledger, and writing
a closing report with a row-by-row walk and bubble-up to Arc03. CDC verifies
row count, status-family coverage, surface-classification coverage, Arc02
constraint incorporation, later-slice routing, and protected-path cleanliness.

## 6. Exit Criteria

The ledger reaches final status. The taxonomy report exists at the planned
path, defines a cross-binary status vocabulary and numeric exit-code table,
applies it to the three supported normal paths and visible non-supported
surfaces, handles timeout/resource/signal and dependency/child-process failure,
keeps Arc02 implementation candidates correctly gated, identifies inputs to
later slices/arcs, and leaves source/build/release paths untouched.
