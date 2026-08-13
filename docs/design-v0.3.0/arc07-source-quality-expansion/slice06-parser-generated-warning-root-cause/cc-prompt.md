# CC Prompt: Arc07 Slice06 - parser-generated-warning-root-cause

You are implementing Arc07 Slice06 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`
6. `docs/design-v0.3.0/arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/cdc-verification.md`
7. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/warning-inventory.md`
8. `docs/design-v0.3.0/arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/slice-doc.md`
9. `docs/design-v0.3.0/arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/ledger.md`

Also inspect current parser/source-quality surfaces before editing:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `tools/source-quality/surface`
- `pandaPI/parser/makefile`
- `pandaPI/parser/src/hddl-token.l`
- `pandaPI/parser/src/hddl.y`
- `pandaPI/parser/src/options.ggo`
- `pandaPI/parser/src/pandapi_parser_native.cpp`

## Policy

This is a parser generated-warning root-cause slice, not a broad parser
modernization slice.

The current parser compiler-warning budget is generated scanner output from
Flex:

- generated file: `pandaPI/parser/src/hddl-token.cpp`
- generator/template input: `pandaPI/parser/src/hddl-token.l`
- warnings: unused `yyunput` and unneeded `yyinput`

Do the root-cause work before changing anything. If the warnings can be fixed
by changing the owned scanner input or Flex options without changing scanner
semantics, make that generator/template fix. Do not hand-edit generated
`hddl-token.cpp`.

Also disposition parser gengetopt output from `options.ggo` to `cmdline.c` and
`cmdline.h`. It is part of the parser generated-code surface, but this slice
must not migrate the parser to CLI11 or change public CLI behavior. Preserve
CLI11 as the accepted future direction/re-entry, not as work hidden inside
this slice.

## Task

Implement Slice06:

1. Add a parser-specific Make target, expected name:
   `generated-warning-triage-parser`.
2. List the target in `make help`.
3. Make the target generate parser-specific root-cause evidence, expected
   build report:
   `build/source-quality/<platform>/generated-warning-triage-parser.md`.
4. Add/update the durable report:
   `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md`.
5. Root-cause the Flex scanner warnings:
   - generated file;
   - generator/template input;
   - Flex skeleton behavior;
   - why `yyunput`/`yyinput` are emitted;
   - whether accepted parser semantics require them;
   - chosen remedy and re-entry.
6. If safe, fix the warning through `pandaPI/parser/src/hddl-token.l` or
   equivalent owned generator/template input. Do not edit generated
   `hddl-token.cpp`.
7. Verify that warning inventory no longer reports parser `yyunput`/`yyinput`
   warnings after rebuild.
8. Disposition parser gengetopt `cmdline.*` output separately from the Flex
   fix, preserving CLI11/strict-generated-code re-entry.
9. Update `source-quality-gate-scaffold.md` so parser generated-warning status
   reflects this slice.
10. Preserve managed parser and managed pipeline behavior.

## Scope Constraints

Allowed surfaces:

- `Makefile`
- `mk/`
- `tools/source-quality/`
- focused parser generator/template inputs, especially
  `pandaPI/parser/src/hddl-token.l`
- Arc07 source-quality reports
- Arc07 Slice06 docs/ledger/closing report

Do not change:

- generated parser outputs: `hddl-token.cpp`, `hddl.cpp`, `hddl.hpp`,
  `cmdline.c`, `cmdline.h`;
- grounder or engine source/build/test behavior;
- public `pandapi-*` CLI behavior;
- parser CLI11 migration;
- CI workflows unless a ledger row is amended to accept one;
- public README/tutorial docs;
- Arc08 or Arc09 docs;
- release/package/publication tooling;
- wolong migration guidance;
- dependency pins/fetch behavior;
- global `-Werror`, global warning fail policy, coverage floors, or
  static-analysis thresholds.

Do not add a new `scripts/` entrypoint. All new automation must be reachable
through Make.

## Required Verification

Parser generated-warning evidence:

```text
make help
make generated-warning-triage-parser
make generated-warning-triage
make warning-inventory-first-party
```

Run a negative grep proving the old parser Flex warnings are absent after the
fix:

```text
! rg -n -- "hddl-token\\.cpp.*yyunput|hddl-token\\.cpp.*yyinput|unused function 'yyunput'|function 'yyinput' is not needed" build/warnings/*/build.log build/warnings/*/warning-inventory.txt
```

Parser behavior evidence:

```text
make build-parser
make test-contract-parser-managed
make test-contract-pipeline-managed
```

Cross-surface regression evidence:

```text
make source-quality-surface
make source-quality-profile-map
make source-quality-naming-check
make compile-db-parser
make compile-db-first-party
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

Run `make check` if practical. If you do not run it, say exactly why and list
the narrower Make-backed gates that substitute for this slice.

Also run the ledger boundary checks:

- no generated parser outputs changed or committed;
- no grounder/engine changes;
- no `.github`, README, release, `tools/release`, Arc08, or Arc09 changes;
- no dependency pinning or fetch behavior;
- no global coverage floor, global warning failure policy, or global
  static-analysis threshold;
- no parser CLI11 migration or public CLI behavior expansion.

## Stop Conditions

Stop and request an amendment if:

- the Flex warnings cannot be fixed through `hddl-token.l` or generator
  options without changing scanner semantics;
- the only available fix is to hand-edit generated output;
- warning cleanup requires Bison grammar redesign or broad parser behavior
  changes;
- gengetopt cleanup would become a CLI11 migration;
- `generated-warning-triage-parser` would be an empty report rather than a
  real root-cause/evidence target;
- any grounder, engine, CI, public docs, release, dependency, global threshold,
  or public CLI behavior change appears necessary.

## Closing Requirements

Before committing:

- Close every ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice06-parser-generated-warning-root-cause/closing-report.md`.
- Include `Bubble-up to Arc07`, explicitly stating whether the next slice
  should proceed to grounder first-party/dependency-boundary work, parser
  maintained-source burndown, or remaining parser generated-code remediation.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
