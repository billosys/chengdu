# Arc07 Slice06 Closing Report: parser-generated-warning-root-cause

Status: CC proposed done
Date: 2026-08-13

## Summary

Slice06 closes the parser Flex generated-warning budget by changing the owned
scanner input, not the generated scanner output. `hddl-token.l` now declares
`%option noinput` and `%option nounput`; the scanner rules do not call
`input()` or `unput()`, so Flex can omit the unused helper functions safely.

The new Make target `generated-warning-triage-parser` writes parser-specific
root-cause evidence to:

- `build/source-quality/<platform>/generated-warning-triage-parser.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md`

The report also dispositions `options.ggo` and generated `cmdline.c` /
`cmdline.h` separately from the Flex fix. Those files are not the current
`yyunput` / `yyinput` warning source, and this slice preserves managed parser
behavior without doing parser option replacement work.

## Ledger Evidence

- F-1: `make help` prints `make generated-warning-triage-parser`, and `rg`
  found target wiring in `mk` and `tools`.
- F-2: `make generated-warning-triage-parser` passed and wrote both durable
  and build reports; the required report grep for generated files, root cause,
  correctness, chosen remedy, and re-entry passed.
- F-3: The Flex/root-cause grep passed across `hddl-token.l`, the durable
  report, and the build evidence report.
- F-4: `make warning-inventory-first-party` passed with `Warning lines: 52`;
  the exact negative grep for the old parser `yyunput` / `yyinput` warnings
  passed.
- F-5: Generated parser outputs are not changed in the worktree or staged
  diff; the fix is limited to `hddl-token.l`.
- F-6: The parser report records `options.ggo`, `cmdline.*`, gengetopt,
  managed native parser ownership, CLI11 re-entry, and no option behavior
  replacement in this slice.
- F-7: `make build-parser` and `make test-contract-parser-managed` passed;
  managed parser fixtures reported `298 passed, 0 failed`.
- F-8: `make test-contract-pipeline-managed` passed; managed pipeline fixtures
  reported `129 passed, 0 failed`.
- F-9: `make generated-warning-triage`, `make source-quality-surface`,
  `make source-quality-profile-map`, `make source-quality-naming-check`,
  `make compile-db-parser`, and `make compile-db-first-party` passed.
- F-10: `make safety-checks`, `make actionlint`, `make static-analysis`,
  `make format-check`, `make test`, `make provenance-check`,
  `git diff --check`, and `git diff --cached --check` passed.
- F-11: The staged boundary checks passed after staging; no grounder, engine,
  CI, release, dependency, global threshold, public documentation, or option
  replacement scope was added.
- F-12: This closing report walks F-1 through F-12 and includes Bubble-up to
  Arc07.

## Make Check Note

The aggregate `make check` was not run. The slice-required constituent gates
were run directly, including safety, actionlint, static analysis, format, full
test, provenance, and whitespace checks. This keeps the macOS provenance step
explicit while still covering the required regression surface.

## Bubble-up to Arc07

Parser generated warnings are no longer an open warning-budget item for
`hddl-token.cpp`. Arc07 can proceed to the next slice with parser Flex helper
warnings closed, while keeping parser gengetopt/option replacement and any
stricter generated-code policy as separate re-entry work.
