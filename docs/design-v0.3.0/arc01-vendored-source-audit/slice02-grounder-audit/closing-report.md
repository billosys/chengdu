# Slice 02: grounder-audit - closing report

Date: 2026-08-09

Branch: `release/0.3.x`

## Per-row walk

| ID | Final status | Evidence |
|----|--------------|----------|
| F-1 | closed | Report exists at `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`; section 2 records project root, branch, date, audited paths, and C++ knowledge-pack/Core Guidelines substrate. |
| F-2 | closed | Report section 2 inventories `pandaPI/pandaPIgrounder/src`, `options.ggo`, `src/Makefile`, `build.sh`, docs, and test/fixture posture. |
| F-3 | closed | Report sections 2 and 4 identify `cpddl` and `h2-fd-preprocessor`, distinguish grounder-owned integration findings from dependency-internal findings, and give re-entry conditions. |
| F-4 | closed | Report section 3 covers correctness/runtime safety, error handling, stdio/TTY behavior, CLI/API surface, build/tests, maintainability, and modernization/resource candidates. |
| F-5 | closed | Every grounder-owned finding uses Blocker/High/Medium/Low style severity and cites exact `pandaPI/pandaPIgrounder/...:<line>` locations. |
| F-6 | closed | Findings G-004, G-005, and G-006 audit exit/status propagation and managed-process classification risks. |
| F-7 | closed | Findings G-007, G-008, and G-009 audit stdout/stderr, buffering/flush risk, ANSI/color, and TTY/pipe behavior. |
| F-8 | closed | Findings G-010 and G-011 audit `options.ggo`, `--help`, `--version`, binary-name migration, and `pandapi-*` naming implications. |
| F-9 | closed | Findings G-012, G-013, G-014, and G-015 audit Makefile/build.sh posture, generated `gengetopt` assumptions, dependency build coupling, CI/test gaps, and fixture absence. |
| F-10 | closed | Findings G-013, G-017, and the synthesis section identify adapter/shared-code candidates; the report also states no algorithmic shared-library extraction is ready before parser/engine synthesis. |
| F-11 | closed | Report section 6 records eight clean checks tied to the grounder audit scope. |
| F-12 | closed | Report section 5 lists cross-codebase synthesis inputs for parser/engine comparison and slice04. |
| F-13 | closed | Findings map source-quality issues to C++ Core Guidelines rule IDs where applicable and label Chengdu managed-process findings where no direct Core Guidelines rule applies. |
| F-14 | closed | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env` produced no output. |

## Verification commands

- `test -f docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md && rg -n "Project root|Branch|Audited paths|C\\+\\+ knowledge|Core Guidelines|11-anti-patterns" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "pandaPI/pandaPIgrounder/src|options\\.ggo|src/Makefile|build\\.sh|doc|tests|fixtures" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "cpddl|h2-fd-preprocessor|dependency|integration|dependency-internal|grounder-owned" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "correctness|runtime safety|error handling|stdio|TTY|CLI|API|build|tests|maintainability|modernization" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "Severity|Location|pandaPI/pandaPIgrounder/.+:[0-9]+" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "exit|status|return code|error handling|supervised process|managed process" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "stdout|stderr|buffer|flush|ANSI|colour|color|isatty|pipe|TTY" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "options\\.ggo|--help|--version|pandapi-|binary name|namespac" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "Makefile|build\\.sh|generated|gengetopt|dependency build|cpddl|h2-fd-preprocessor|CI|test|fixture" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "duplicate|shared|common|library|header|helper|candidate" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "Things I looked for|did not find|Clean checks" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "Synthesis inputs|Compare with parser|Compare with engine|cross-codebase" docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `rg -n "C\\+\\+ Core Guidelines|Rule ID|I\\.[0-9]+|F\\.[0-9]+|R\\.[0-9]+|E\\.[0-9]+|ES\\.[0-9]+|SF\\.[0-9]+|SL\\.|CP\\." docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` - exit 0.
- `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env` - exit 0, no output.

## Bubble-up to the arc

Delivered-as-assigned: yes. The grounder audit report landed at the planned
arc-local path with concrete findings, clean checks, C++ Core Guidelines mapping
where applicable, and dependency-boundary notes.

What the arc-plan did not anticipate: the H2 path contains a concrete
string-literal pointer arithmetic bug in grounder-owned code, and quiet mode
suppresses H2 output by mutating global `std::cout` state. Those are more
specific than the initial generic dependency-coupling risk.

Silent-drop diff: no planner source, dependency source, scripts, workflows, or
release assets changed. CDC remains independent and should write
`cdc-verification.md` separately.
