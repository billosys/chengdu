# Parser Generated-Warning Root-Cause

Status: Slice06 implementation report
Platform: macos-arm64

## Scope

This report is parser-specific generated-warning evidence for Arc07 Slice06.
It covers Flex scanner output from `hddl-token.l` and parser gengetopt output from `options.ggo`.
Generated outputs `hddl-token.cpp`, `hddl.cpp`, `hddl.hpp`, `cmdline.c`, and `cmdline.h` are not hand-edited source.

## Parser Generated-Warning Records

| generated file | generator/template or skeleton | ownership class | root cause | correctness impact | chosen remedy | re-entry |
|----------------|--------------------------------|-----------------|------------|--------------------|---------------|----------|
| `pandaPI/parser/src/hddl-token.cpp` | `pandaPI/parser/src/hddl-token.l` through the Flex skeleton | first-party generator/template plus third-party skeleton output | Flex emits scanner helper functions `yyunput` and `yyinput` by default unless the scanner declares they are unused. The current scanner rules never call `unput()` or `input()`. | Accepted parser semantics do not require pushback or direct scanner input helper calls; rules return tokens from matched text, skip whitespace/comments, and assign semantic values from `yytext`. | Add `%option nounput` and `%option noinput` to `hddl-token.l`, so Flex omits the unused helpers when regenerating `hddl-token.cpp`. | Re-enter if scanner rules add `unput()`, `input()`, start-condition pushback behavior, or a stricter generated-code policy needs skeleton-level review. |
| `pandaPI/parser/src/cmdline.c`, `cmdline.h` | `pandaPI/parser/src/options.ggo` through gengetopt | first-party generator/template plus third-party generated output | gengetopt emits the parser option parser and human option prose used by the inherited legacy driver; it is not the current `yyunput`/`yyinput` warning source. | Managed native parser behavior is owned by `pandapi_parser_native.cpp`; generated human diagnostic prose is not final managed-process contract evidence. | Preserve `options.ggo` and generated `cmdline.*` behavior in this slice. Keep option semantics and generated-output strictness as separate follow-up work. | Re-enter for strict generated-code policy, option-spec semantic changes, or the accepted CLI11 replacement slice; do not fold that work into this root-cause cleanup. |

## Flex Evidence

- `pandaPI/parser/src/hddl-token.l` contains `%option noinput`, `%option nounput`, `%option noyywrap`, and `%option yylineno`.
- Repository grep found no scanner-rule use of `input()` or `unput()` in `hddl-token.l`.
- The selected remedy changes the generator/template input only. It does not hand-edit `hddl-token.cpp`.
- The warning-inventory verification gate is `make warning-inventory-first-party` followed by the Slice06 negative grep for `yyunput` and `yyinput`.

## Parser Behavior Boundary

The behavior proof remains black-box: `make build-parser`, `make test-contract-parser-managed`, and `make test-contract-pipeline-managed` must pass after the scanner input change. Generated scanner helper omission is accepted only because no scanner rule uses those helpers.
