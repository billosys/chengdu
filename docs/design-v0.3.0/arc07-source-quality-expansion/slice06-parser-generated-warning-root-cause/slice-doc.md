# Arc07 Slice06: parser-generated-warning-root-cause

Status: open
Opened: 2026-08-13

## Goal

Turn the parser generated-warning scaffold into concrete root-cause,
correctness, and remediation evidence for parser generated outputs.

This slice follows Slice05's engine foothold. Parser primary maintained-source
warnings are already clear, but the remaining parser generated warning budget
still needs the stricter Arc07 treatment: identify why the generated code emits
the warning, whether semantics are correct, which input/generator/skeleton owns
the emitted code, and whether the remedy is a generator/template fix, an input
option change, a compiler-boundary decision, or a documented third-party
skeleton exclusion.

## Background

Arc07 classifies parser generated outputs as follows:

- `pandaPI/parser/src/hddl-token.cpp` is generated from
  `pandaPI/parser/src/hddl-token.l` through Flex.
- `pandaPI/parser/src/hddl.cpp` and `hddl.hpp` are generated from
  `pandaPI/parser/src/hddl.y` through Bison.
- `pandaPI/parser/src/cmdline.c` and `cmdline.h` are generated from
  `pandaPI/parser/src/options.ggo` through gengetopt.

The current warning inventory reports two parser generated scanner warnings:

- `src/hddl-token.cpp:1567:17: warning: unused function 'yyunput'`
- `src/hddl-token.cpp:1612:16: warning: function 'yyinput' is not needed and
  will not be emitted`

The current generated-warning scaffold also records the gengetopt-generated
parser `cmdline.*` surface. That surface is not currently the source of the
two parser compiler warnings, but it still needs a parser-specific disposition
so a later strict generated-code or CLI11 slice does not inherit ambiguity.

## In Scope

- Add a parser-specific generated-warning Make target, expected name:
  `make generated-warning-triage-parser`.
- List the new target in `make help`.
- Produce durable parser generated-warning/root-cause evidence, expected
  report:
  `docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md`.
- Root-cause the Flex scanner warnings to the owned scanner input, Flex
  skeleton behavior, or both.
- If safe, fix the Flex warning at the generator/template input
  (`pandaPI/parser/src/hddl-token.l`) rather than by hand-editing generated
  `hddl-token.cpp`.
- Verify generated scanner semantics through managed parser fixtures and at
  least one parser/pipeline contract path that consumes parser output.
- Root-cause and disposition parser gengetopt output from
  `pandaPI/parser/src/options.ggo`, including whether it is a current warning
  source, a strict generated-code re-entry, or a CLI11 migration input.
- Update the Arc07 source-quality gate scaffold so parser generated-warning
  status reflects the accepted root-cause/remedy evidence.
- Preserve parser managed-process behavior.

## Out Of Scope

- Full parser first-party maintained source-quality burndown.
- Parser CLI11 migration.
- Broad option parser redesign or public CLI behavior changes.
- Hand-editing generated `hddl-token.cpp`, `hddl.cpp`, `hddl.hpp`,
  `cmdline.c`, or `cmdline.h`.
- Grounder or engine generated-warning remediation.
- Bison grammar redesign unless required to explain a parser generated warning
  in this slice.
- New dependencies, dependency pinning, FetchContent, or tool downloads.
- CI workflow edits unless the ledger is amended first. This slice should not
  need CI wiring.
- Public README/tutorial docs, Arc08, Arc09, release/package/publication, or
  wolong migration changes.
- Global `-Werror`, global warning policy, global coverage floors, or broad
  static-analysis thresholds.

## Required Target Shape

`generated-warning-triage-parser` must be Make-backed and non-vacuous. It
should:

- regenerate or consume current parser warning evidence through Make;
- write parser-specific generated-warning/root-cause evidence;
- record generated file, generator/template or skeleton, ownership class, root
  cause, correctness impact, chosen remedy, and re-entry for each parser
  generated-warning class;
- fail or stop the slice if the Flex warnings are claimed closed but the
  warning inventory still reports `yyunput` or `yyinput`;
- avoid changing generated output directly; and
- keep gengetopt disposition separate from the Flex warning closure.

If the Flex warnings cannot be fixed through `hddl-token.l`/Flex options
without changing scanner semantics, stop and request an amendment rather than
silencing the warning or weakening the ledger.

## Verification Approach

Parser generated-warning evidence:

- `make help`
- `make generated-warning-triage-parser`
- `make generated-warning-triage`
- `make warning-inventory-first-party`
- Negative grep for `yyunput` and `yyinput` parser warnings in
  `build/warnings/<platform>/build.log` and
  `build/warnings/<platform>/warning-inventory.txt`, if the Flex warning is
  closed.
- Grep the durable parser report for `hddl-token.cpp`, `hddl-token.l`,
  `yyunput`, `yyinput`, `options.ggo`, `cmdline`, `root cause`,
  `correctness`, `chosen remedy`, and `re-entry`.

Parser behavior evidence:

- `make build-parser`
- `make test-contract-parser-managed`
- `make test-contract-pipeline-managed`

Cross-surface regression evidence:

- `make source-quality-surface`
- `make source-quality-profile-map`
- `make source-quality-naming-check`
- `make compile-db-parser`
- `make compile-db-first-party`
- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

Run `make check` if practical. If a narrower set is used, the closing report
must state exactly what ran and why `make check` was not used.

## Exit Criteria

- `generated-warning-triage-parser` exists, is listed in `make help`, and
  writes durable parser-specific generated-warning evidence.
- The Flex scanner warnings are root-caused with correctness analysis.
- The Flex warnings are fixed through the generator/template input if safe, or
  the slice stops for amendment with evidence that they cannot be fixed safely.
- No generated parser output is hand-edited or committed as source.
- Parser gengetopt output is explicitly dispositioned without migrating to
  CLI11 or changing public CLI behavior in this slice.
- Parser managed contract fixtures and managed pipeline fixtures still pass.
- The source-quality scaffold reflects the parser generated-warning status and
  re-entry conditions.
- No grounder, engine, CI, release, README/tutorial, Arc08, Arc09, dependency,
  global threshold, or public behavior scope expands.
- Ledger rows close with evidence and Bubble-up states whether Arc07 should
  proceed to grounder first-party/dependency-boundary work, parser maintained
  source burndown, or another parser generated-code follow-up.
