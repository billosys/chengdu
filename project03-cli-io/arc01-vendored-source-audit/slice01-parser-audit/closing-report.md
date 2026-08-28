# Slice 01 Closing Report: parser-audit

Date: 2026-08-09

Branch: `release/0.3.x`

Report: `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`

## Result

Delivered as assigned. The slice produced a diagnosis-only audit of
`pandaPI/pandaPIparser` as a vendored C/C++ command-line component intended for
future high-quality CLI and supervised-process use. No parser source, scripts,
workflows, release assets, or build outputs were changed.

## Per-Row Walk

F-1: done. The report exists at the planned arc-local path and records date,
project root, branch, audited paths, commands, the C++ knowledge-pack guides,
`11-anti-patterns`, and Core Guidelines rule substrate.

F-2: done. The report inventories `pandaPI/pandaPIparser/src`,
`src/options.ggo`, `makefile`, `doc`, and `tests`, and records observed counts
for source files and fixtures.

F-3: done. Findings are grouped under correctness/runtime safety, error
handling and exit behavior, stdio/TTY/ANSI/buffering, CLI/API, build/tests,
maintainability/shared-code, and modernization.

F-4: done. Findings P-001 through P-015 each include severity and exact
`pandaPI/pandaPIparser/...:<line>` locations.

F-5: done. Findings P-004 and P-005 cover parser status propagation, deep
`exit()` calls, `exit(-1)` shell status 255, verifier `exit(0)`, and invalid
argument handling that continues after printing an error.

F-6: done. Findings P-006 and P-007 cover stdout/stderr discipline, parse
progress on stdout, ANSI output under `--no-colour`, color policy, pipe safety,
and TTY implications.

F-7: done. Finding P-008 covers the inherited `pandaPIparser` package/version
and binary publication surface, plus implications for future `pandapi-*`
namespacing and provenance.

F-8: done. Finding P-014 and the synthesis section identify parser-side
duplicate output-file, diagnostic, color, status, and shared helper candidates.

F-9: done. The report includes eight parser-scope clean checks under "Things I
Looked For and Did Not Find."

F-10: done. The report lists explicit grounder, engine, and shared/library
comparison inputs for later synthesis.

F-11: done. Source-quality findings cite applicable Core Guidelines rule IDs,
including `E.4`, `SL.io.2`, `ES.65`, `R.1`, `I.30`, `I.2`, `R.11`, `R.20`,
`I.11`, and `SF.7`; managed-process findings are labeled as Chengdu-specific
where no direct rule applies.

F-12: done. The protected-path diff check produced no parser source, script,
workflow, README, release-manifest, `vendor.env`, or `pins.env` changes.

## Verification

Commands recorded in the audit report:

- `./scripts/build-parser.sh` - exit 0.
- Parser CLI probes - probe command exit 0, with observed subprocess statuses
  for missing args, missing file, malformed input, malformed `--no-colour`, and
  valid fixture parse.
- `git status --short --branch` after build/probes - exit 0 and clean before
  report artifacts were written.

Closeout commands:

- `test -f docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md && rg -n "Project root|Branch|Audited paths|C\\+\\+ knowledge|Core Guidelines|11-anti-patterns" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "pandaPI/pandaPIparser/src|options\\.ggo|makefile|tests" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "correctness|runtime safety|error handling|stdio|TTY|CLI|API|build|tests|maintainability|modernization" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "Severity|Location|pandaPI/pandaPIparser/.+:[0-9]+" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "exit|status|return code|error handling|supervised process|managed process" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "stdout|stderr|buffer|flush|ANSI|colour|color|isatty|pipe|TTY" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "options\\.ggo|--help|--version|pandapi-|binary name|namespac" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "duplicate|shared|common|library|header|helper|candidate" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "Things I looked for|did not find|Clean checks" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "Synthesis inputs|Compare with grounder|Compare with engine|cross-codebase" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `rg -n "C\\+\\+ Core Guidelines|Rule ID|I\\.[0-9]+|F\\.[0-9]+|R\\.[0-9]+|E\\.[0-9]+|ES\\.[0-9]+|SF\\.[0-9]+|SL\\." docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
- `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env`
- `git diff --check`

## Bubble-up to the Arc

Delivered-as-assigned: yes. The parser audit is complete, evidence-backed, and
ready for comparison with the grounder and engine audits.

What the arc-plan did not anticipate: the `--no-colour` defect is narrower than
general color scattering. A central `color()` helper exists and is used in many
places; the concrete defect is that `yyerror()` bypasses it with hard-coded ANSI
escapes. The audit also found a specific fatal CLI-validation bug where an
ordinary-output arity error is printed but parsing continues.

Silent-drop diff: no planned scope was dropped. The audit did not refresh
upstream/GitHub state because the assignment explicitly allowed the in-tree fork
baseline. No source fixes were attempted because this slice was diagnosis-only.
