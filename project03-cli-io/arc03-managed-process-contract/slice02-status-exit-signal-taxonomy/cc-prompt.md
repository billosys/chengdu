# CC assignment - chengdu arc03 / slice02 - status-exit-signal-taxonomy

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc03-managed-process-contract/arc-plan.md`
4. `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/slice-doc.md`
5. `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/ledger.md`
6. Accepted Arc03 slice01 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/cdc-verification.md`
7. Arc01 close and synthesis:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/closing-report.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
8. Arc01 component reports as needed for finding details:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
9. Arc02 close and combined recommendations:
   - `docs/design-v0.3.0/arc02-cpp-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

## The work

Produce:

`docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`

This is the second Arc03 design artifact. It must define the common status
vocabulary and exit/signal/resource semantics that later slices use for
stdio/events, CLI naming/version/migration, tests, and the Arc04 shared
runtime substrate.

The taxonomy must cover the three supported 0.3.0 surfaces:

- parser normal HDDL parse;
- grounder normal `.htn` grounding;
- engine normal search.

It must also define deterministic treatment for visible non-supported surfaces:

- parser verifier/output helper paths: `legacy`;
- grounder H2: `experimental`;
- grounder `cpddl`/FAM: `legacy`;
- engine interactive mode: `legacy`;
- engine translation mode: `experimental`;
- engine SAT: `unsupported`;
- engine BDD/CUDD: `future`.

For each status family, record:

- status name;
- meaning;
- applicable binaries/surfaces;
- numeric exit-code mapping;
- human-facing summary requirement, without designing final stream placement;
- machine-facing payload fields, without choosing JSON or event serialization;
- whether the status represents success, expected domain outcome, caller error,
  input/model error, policy failure, external/dependency failure, or internal
  defect;
- later slice/arc consumers.

## Arc02 constraints you must preserve

Arc02 was inserted after the original plan, and its findings are now
load-bearing:

- `tl::expected` is only a future carrier. Define the status semantics first;
  do not design or expose the C++ API in this slice.
- reproc++ is conditional. H2 and translation are experimental, not supported,
  so child-process containment must not become a default supported-path
  requirement. Define status behavior for experimental child-process failures
  only if those paths remain visible.
- nlohmann/json is held. You may name payload fields and event classes, but
  slice03 decides whether events are JSON Lines, tagged text, or something
  else.
- CLI11 is a migration candidate. Define usage/parse-error status semantics,
  but leave option grammar, help/version text, aliases, and `pandapi-*`
  migration to slice04.
- fmt belongs behind a diagnostics/process facade. Do not specify direct
  formatting calls, logging APIs, or stdout/stderr routing in this slice.
- Catch2 is test-only, and process fixtures are separate. List test obligations
  for slice05/Arc04, but do not treat unit tests as process-contract proof.
- Abseil and Boost.Process are rejected as 0.3.0 foundation choices; GSL,
  performance containers, google/benchmark, parser-generator replacement,
  SAT/BDD/CUDD enablement, H2/cpddl dependency-internal work, and broad optional
  dependency audits remain held unless you name explicit re-entry conditions.

## Constraints

- Do not edit planner source, dependency source, scripts, workflows, release
  assets, build outputs, `vendor.env`, or `pins.env`.
- Do not adopt dependencies or change build policy.
- Do not decide stdout/stderr/event/TTY/color/buffering policy; route those
  inputs to slice03.
- Do not decide final CLI option names, aliases, help/version text, or CLI11
  migration; route those inputs to slice04.
- Do not write the final contract test matrix; route those inputs to slice05.
- Preserve wolong's frozen fetch/install shape unless the operator explicitly
  accepts a breaking migration.

## Suggested report shape

1. Executive summary.
2. Evidence base: Arc01, Arc02, and slice01.
3. Status vocabulary.
4. Numeric exit-code table and rationale.
5. Per-binary/surface application notes.
6. Timeout, resource, signal, and interruption semantics.
7. Dependency and child-process failure semantics.
8. Arc02 dependency-gating implications.
9. Later slice and arc inputs.
10. Final recommendation.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc03, and leave `cdc-verification.md` for CDC.
