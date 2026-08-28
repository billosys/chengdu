# CC Prompt: Arc07 Slice01 - source-classification-inventory

You are implementing Arc07 Slice01 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/slice01-source-classification-inventory/slice-doc.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/slice01-source-classification-inventory/ledger.md`
6. Arc06 close evidence:
   - `docs/design-v0.3.0/arc06-ci-and-test-hardening/closing-report.md`
   - `docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md`
   - `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/warning-inventory.md`

## Policy

The operator clarified the policy after Arc06:

- parser, grounder, engine, and runtime are all Chengdu-owned product source;
- first-party maintained code needs formatting, static analysis, coverage,
  unit-test strategy, warning policy, and sanitizer triage;
- generated code from Chengdu-owned generators/templates should be fixed at
  the generator/template;
- generated code from third-party skeletons may be excluded or suppressed with
  rationale;
- vendored third-party/dependency-internal code is not a Chengdu
  coverage/static-analysis/unit-test obligation unless a later dependency
  audit accepts that boundary.

Do not use ambiguous "owned runtime versus inherited planner" language for
active policy. Use source classes: first-party maintained, first-party
generated, first-party generator/template, vendored third-party, third-party
generated, copied build artifact, generated build output, or another explicit
class if the tree requires it.

## Task

Create:

`docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`

The report must include:

1. Source-class taxonomy with definitions.
2. Path inventory table by component and class for:
   - `pandaPI/parser`
   - `pandaPI/grounder`
   - `pandaPI/engine`
   - `pandaPI/runtime`
3. Generated-code ownership table: generated file, generator/template,
   owner/class, warning/format policy.
4. Third-party/dependency exclusion table with rationale.
5. Existing gate map by component/source class.
6. Gap map for coverage, static analysis, formatting, warnings, sanitizers,
   and unit/seam tests.
7. Recommendation section:
   - whether Slice02 should restructure paths/builds;
   - proposed Make target families;
   - proposed Catch2/test dependency posture;
   - proposed warning/static-analysis/format policy;
   - proposed coverage baseline/floor strategy;
   - proposed component slice order.
8. Operator-decision section listing items CDC/operator should discuss before
   Slice02.

## Scope Constraints

This is inventory/report work only.

Do not change:

- production source under `pandaPI/`;
- `Makefile`, `mk/`, or CI workflows;
- release/package/publication tooling;
- README or public tutorial docs;
- Arc08 or Arc09 surfaces;
- supported/legacy/experimental/unsupported/future public-surface policy.

If you discover a tiny docs-only correction needed to keep the inventory
honest, include it only if it is directly tied to this slice and record it in
the closing report.

## Suggested Inspection Commands

Use targeted commands and keep generated/build output out of the durable
report unless it is evidence for classification:

```text
git status --short --branch
make help
rg --files pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime
find pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime -maxdepth 4 -type f
rg -n -- "generated|DO NOT EDIT|flex|bison|gengetopt|cmdline|lexer|parser|template|third.party|third-party|copyright|license" pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime
rg -n -- "add_executable|add_library|compile_commands|CMAKE_EXPORT_COMPILE_COMMANDS|clang-tidy|coverage|sanitize|warning" Makefile mk pandaPI
```

Run existing checks:

```text
make safety-checks
make actionlint
make static-analysis
make format-check
make warning-inventory
git diff --check
git diff --cached --check
```

If you intentionally use a narrower check set, explain exactly why.

## Closing Requirements

Before committing:

- Close every ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice01-source-classification-inventory/closing-report.md`.
- Include `Bubble-up to Arc07`, explicitly stating whether `arc-plan.md`
  should be amended before Slice02.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
