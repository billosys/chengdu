# CDC Verification: Arc07 Slice06 parser-generated-warning-root-cause

Date: 2026-08-13

Verified commit:
`39c46101f4bc438ba40c8de86a24aeaa88b8088e`

CDC verdict: verified closed.

Slice06 successfully closes the parser Flex generated-warning budget by
changing the owned scanner input instead of editing generated scanner output.
`hddl-token.l` now declares `%option noinput` and `%option nounput`; CDC
confirmed the scanner rules do not call `input()` or `unput()`, so omitting
the unused Flex helper functions is an appropriate generator-input remedy.

The slice also records parser gengetopt output separately. `options.ggo` and
generated `cmdline.c` / `cmdline.h` are not the current `yyunput` /
`yyinput` warning source, and this slice does not perform parser option
replacement, CLI11 migration, or public behavior changes.

## Scope Review

The reviewed commit changes only expected Slice06 surfaces:

- `pandaPI/parser/src/hddl-token.l`
- `mk/help.mk`
- `mk/source-quality.mk`
- `tools/source-quality/surface`
- Arc07 Slice06 docs and generated-warning evidence reports

CDC confirmed no `.github/`, README, release tooling, Arc08, Arc09, grounder,
or engine surfaces changed. CDC also confirmed generated parser outputs are
not tracked or changed as part of the fix.

## Reproduced Evidence

CDC reproduced the parser generated-warning evidence:

- `make help | rg 'generated-warning-triage-parser'`
- `make generated-warning-triage-parser`
- `rg -n -- 'generated-warning-triage-parser' Makefile mk tools`
- report grep for `hddl-token.cpp`, `hddl-token.l`, `cmdline`,
  `options.ggo`, root cause, correctness, chosen remedy, and re-entry
- scanner grep confirming `%option noinput` and `%option nounput`, with no
  `input()` or `unput()` calls in `hddl-token.l`

CDC reproduced warning and behavior gates:

- `make warning-inventory-first-party`
- exact negative grep for the old parser Flex `yyunput` / `yyinput` warnings
- `make build-parser`
- `make test-contract-parser-managed`
- `make test-contract-pipeline-managed`

Observed results:

- First-party warning inventory passed with `Warning lines: 52`.
- No residual `hddl-token.cpp` `yyunput` / `yyinput` warnings were present in
  `build/warnings/macos-arm64/build.log` or
  `build/warnings/macos-arm64/warning-inventory.txt`.
- Managed parser fixtures passed with `298 passed, 0 failed`.
- Managed pipeline fixtures passed with `129 passed, 0 failed`.

CDC reproduced the source-quality runway:

- `make generated-warning-triage`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-parser`
- `make compile-db-first-party`

CDC reproduced the regression chain:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Observed regression evidence:

- Runtime CTest passed: 5/0.
- Baseline fixtures passed: 38/0.
- Managed parser fixtures passed: 298/0.
- Managed grounder fixtures passed: 269/0.
- Managed engine fixtures passed: 312/0.
- Managed pipeline fixtures passed: 129/0.
- Positive smoke passed: 3/0.
- Negative smoke passed: 4/0.
- Provenance check passed for all three components on `macos-arm64`.
- Whitespace checks passed.

`make check` was not run as a single aggregate because this project has a
known macOS aggregate provenance mutation/noise path. CDC reproduced the exact
constituent gates required by F-10 instead.

## Ledger Verification

| Row | CDC status | Evidence |
|-----|------------|----------|
| F-1 | verified | `make help` lists `generated-warning-triage-parser`, and Make/tool wiring grep passed. |
| F-2 | verified | `make generated-warning-triage-parser` wrote durable and build evidence with generated files, root cause, correctness, chosen remedy, and re-entry fields. |
| F-3 | verified | Flex/root-cause evidence is present across `hddl-token.l`, the durable report, and generated build evidence. |
| F-4 | verified | `make warning-inventory-first-party` passed with `Warning lines: 52`; the exact negative grep for old parser Flex warnings passed. |
| F-5 | verified | Generated parser outputs are not tracked or changed; the fix is limited to the scanner input. |
| F-6 | verified | Gengetopt output is dispositioned separately with CLI11 and strict generated-code policy as re-entry work; no option replacement or public CLI behavior change landed. |
| F-7 | verified | `make build-parser` and `make test-contract-parser-managed` passed with 298/0 managed parser fixtures. |
| F-8 | verified | `make test-contract-pipeline-managed` passed with 129/0 managed pipeline fixtures. |
| F-9 | verified | Generated-warning and source-quality runway targets passed. |
| F-10 | verified | Safety, actionlint, static-analysis, format, test, provenance, and whitespace gates passed as constituent commands. |
| F-11 | verified | Boundary checks found no grounder, engine, CI, release, dependency, global threshold, public documentation, wolong, or option-replacement expansion. |
| F-12 | verified | Closing report walks every row and includes Arc07 bubble-up. |

## Findings

The parser warnings were not evidence of a parser semantic defect. They were
Flex skeleton defaults: unless told otherwise, Flex emits helper functions for
pushback and direct scanner input. This scanner does not use those helpers,
and the accepted parser behavior is tokenization from matched text, whitespace
and comment skipping, and semantic value assignment from `yytext`.

The right fix was therefore to update the scanner source with `%option
noinput` and `%option nounput`, so regenerated scanner output omits unused
helpers. That is the shape we wanted for generated-code cleanup: investigate
the warning, confirm generated semantics, fix the owned generator input when
safe, and leave generated output untouched.

The first-party warning inventory dropped from the prior 54-line budget to 52
lines after removing the two parser Flex helper warnings. The remaining
observed warning budget is outside this parser scanner surface and remains
routed to later Arc07 component/dependency-boundary work.

## Bubble-up

Arc07 may mark Slice06 closed and CDC-verified.

Parser Flex helper warnings are no longer an open warning-budget item for
`hddl-token.cpp`. Parser gengetopt/option replacement, CLI11 migration, and
any stricter generated-code policy remain separate re-entry work. The next
recommended slice is grounder first-party source and dependency-boundary
follow-up, unless the operator chooses to pull parser maintained-source
burndown forward first.
