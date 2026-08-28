# CC assignment - chengdu arc03 / slice01 - supported-surface-classification

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc03-managed-process-contract/arc-plan.md`
4. `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/slice-doc.md`
5. `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/ledger.md`
6. Arc01 close and synthesis:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/closing-report.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
7. Arc01 component reports as needed for finding details:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
8. Arc02 close and combined recommendations:
   - `docs/design-v0.3.0/arc02-cpp-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

## The work

Produce:

`docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`

This is the first Arc03 design artifact. It must classify the inherited product
surfaces before later slices define statuses, streams, CLI behavior, events, or
contract tests around them.

Classify at least:

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

Use exactly the dispositions from `slice-doc.md`: `supported`, `legacy`,
`experimental`, `unsupported`, and `future`. For each surface, record:

- disposition;
- rationale;
- audit evidence using `P-*`, `G-*`, and/or `E-*` finding IDs;
- user-facing behavior expectation for 0.3.0;
- effect on managed-process supervision;
- re-entry condition when the surface is not fully supported;
- later Arc03 slice inputs.

## Constraints

- Do not edit planner source, dependency source, scripts, workflows, release
  assets, build outputs, `vendor.env`, or `pins.env`.
- Do not adopt dependencies or change build policy.
- Do not let dependency availability decide supported product surface.
  reproc++ does not make H2 or translation supported; CUDD availability does
  not make BDD supported; nlohmann/json does not decide event mode.
- Preserve wolong's frozen fetch/install shape unless the operator explicitly
  accepts a breaking migration.
- Keep unknowns visible. If a surface needs a deeper dependency or optional
  capability audit, classify it honestly and give a re-entry condition.

## Suggested report shape

1. Executive summary.
2. Evidence base.
3. Classification vocabulary.
4. Surface classification matrix.
5. Per-surface notes and rationale.
6. Dependency non-inputs.
7. Inputs to later Arc03 slices.
8. Open questions and re-entry conditions.
9. Final recommendation.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc03, and leave `cdc-verification.md` for CDC.
