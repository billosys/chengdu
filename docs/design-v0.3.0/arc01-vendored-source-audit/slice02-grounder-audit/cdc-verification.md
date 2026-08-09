# CDC Verification: grounder-audit

Date: 2026-08-09

Branch: `release/0.3.x`

CC commit:

- `af584cae070fcc49c8fd6336cf4425ed93e52ab4` - `docs: close grounder audit slice`

Verdict: **accepted**. Slice02 is CDC-verified.

## Row Count

Opening ledger row count at `0c22e86c`: 14.

Closing ledger row count at `af584cae`: 14.

No ledger rows were dropped.

## Commit and Scope Check

`af584cae` changes only:

- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/slice02-grounder-audit/closing-report.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/slice02-grounder-audit/ledger.md`

The commit includes the required assistant co-author trailers.

Protected-path check:

```bash
git diff --name-only 0c22e86c..af584cae070fcc49c8fd6336cf4425ed93e52ab4 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env
```

Result: no output. No grounder source, dependency source, scripts, workflows,
README, release manifest, `vendor.env`, or `pins.env` changed in the close
commit.

Current worktree note: at CDC time, unrelated engine-audit and Arc02
library-research files were already modified/untracked by another task. CDC
did not inspect or stage those files.

Additional format check:

```bash
git diff --check af584cae070fcc49c8fd6336cf4425ed93e52ab4^..af584cae070fcc49c8fd6336cf4425ed93e52ab4
```

Result: no output.

## Ledger Verification

CDC reran every ledger Verify command. All rows reproduce.

One protocol normalization was applied during CDC: CC used `closed` as the
positive final status, but the ledger discipline names `done` as the positive
final status. Because the evidence reproduced for all rows, CDC normalized
`closed` to `done` in the ledger and closing report rather than returning the
slice for a terminology-only iteration.

| Row | CDC disposition | Evidence |
|-----|-----------------|----------|
| F-1 | accepted | Report exists and identifies project root, branch, audited paths, C++ knowledge-pack guides, Core Guidelines references, and `11-anti-patterns`. |
| F-2 | accepted | Report inventories `pandaPI/pandaPIgrounder/src`, `options.ggo`, `src/Makefile`, `build.sh`, docs, and test/fixture posture. |
| F-3 | accepted | Report inventories `cpddl` and `h2-fd-preprocessor`, distinguishes grounder-owned integration from dependency-internal findings, and gives re-entry conditions. |
| F-4 | accepted | Report covers correctness/runtime safety, error handling, stdio/TTY, CLI/API, build/tests, maintainability, and modernization. |
| F-5 | accepted | Findings G-001 through G-019 use severity and exact `pandaPI/pandaPIgrounder/...:<line>` locations. |
| F-6 | accepted | Report covers status/exit behavior, including missing arguments, collapsed status 1 failures, and deep `exit(-1)` / `_exit(0)` paths. |
| F-7 | accepted | Report covers stdout/stderr contamination, buffering/pipe implications, ANSI/color, and TTY policy. |
| F-8 | accepted | Report covers `options.ggo`, inherited `pandaPIgrounder 0.1` identity, help/version behavior, and `pandapi-*` migration implications. |
| F-9 | accepted | Report covers `Makefile`, `build.sh`, generated `gengetopt`, dependency build coupling, CI/test gaps, and fixture posture. |
| F-10 | accepted | Report identifies adapter/shared-code candidates and defers algorithmic shared-library extraction until parser/grounder/engine synthesis. |
| F-11 | accepted | Report lists eight grounder-scope clean checks. |
| F-12 | accepted | Report lists parser, engine, and cross-codebase synthesis inputs for slice04. |
| F-13 | accepted | Report maps source-quality findings to C++ Core Guidelines rule IDs where applicable and labels Chengdu-specific managed-process findings separately. |
| F-14 | accepted | Protected-path diff check produced no output. |

## Build and Probe Reproduction

CDC reran:

```bash
./scripts/build-grounder.sh
```

Result: exit 0, producing `dist/macos-arm64/pandaPIgrounder`. The warning
profile reproduced the report's important classes, including dependency-internal
warnings and the primary grounder `h2mutexes.cpp` string-plus-int warning.

CDC then generated a `.htn` fixture with the parser and probed the rebuilt
grounder:

| Probe | Observed result |
|-------|-----------------|
| Parser fixture generation | status 0; `.htn` file created. |
| Grounder with no positional arguments | status 1; stdout contains configuration text; stderr reports stdin parse/EOF. |
| Grounder with missing input file | status 1; stdout contains configuration text; stderr reports open failure. |
| Grounder with malformed `.htn` and output path | status 1; stdout contains configuration text; stderr reports parse failure. |
| Grounder with valid input and default stdout output | status 0; stdout begins with configuration text and mixes generated output with human/progress output. |
| Grounder with valid input, `--quiet`, and explicit output file | status 0; stdout/stderr empty; output file created. |

These probes support the report's supervised-process findings.

## Source Sampling

CDC sampled cited locations from the report:

- `h2mutexes.cpp:60-65` contains `var.name = "var" + internal_variables.size();`.
- `parser.cpp:32-34` reads an unbounded `size_t count` and passes it to `readN`.
- `postprocessing.cpp:252-292` indexes derived fact/task vectors from parsed
  data and emits debug output from the same path.
- `main.cpp:128-136` enters parsing after stream setup and then selects stdout
  output behavior.
- `h2mutexes.cpp:212-218` suppresses H2 output by setting `std::cout` failbit
  under quiet mode.
- `options.ggo:1-4` preserves inherited `pandaPIgrounder` package/version/usage.
- `src/Makefile:74-87` compiles H2 and cpddl dependency internals into the
  grounder build.

The sampled citations support the report's severities and recommendations.

## Bubble-Up Check

Delivered-as-assigned: yes. Slice02 delivered the grounder audit assigned by
`arc-plan.md`: a diagnosis-only report with exact source citations, C++ Core
Guidelines rule mapping where applicable, managed-process findings,
dependency-boundary notes, clean checks, and synthesis inputs.

Silent-drop diff: complete. No planned scope was dropped. The audit did not
attempt source fixes, as required by the diagnosis-only boundary.

Arc-plan impact: no slice breakdown change is required. The grounder audit
adds concrete Arc02/Arc03 inputs around dependency boundaries, H2/cpddl adapter
shape, stdout/stderr policy, argument/status handling, and primary test fixture
needs.

## What Worked

- The dependency-boundary section kept grounder-owned integration findings
  separate from deeper `cpddl` / `h2-fd-preprocessor` audit work.
- The build/probe evidence tied process-contract findings to observable
  statuses and streams instead of relying only on static source inspection.
- The report preserved the parser comparison shape, making slice04 synthesis
  and Arc02 library research easier to line up.
