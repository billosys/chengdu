# Arc07 Slice01 Closing Report: source-classification-inventory

Status: CC proposed done
Implementation commit: this commit

## Capability Verdict

Proposed done. The durable inventory report exists at
`docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
and classifies parser, grounder, engine, runtime, generated outputs,
generators/templates, vendored dependencies, copied build artifacts, and
generated build output.

## Main Findings

- Parser, grounder, engine, and runtime are all Chengdu-owned product source;
  source class, not fork ownership, controls gate inclusion.
- Current `format-check`, `static-analysis-cpp`, and `coverage` selectors are
  runtime-centered.
- `make warning-inventory` and `make test-binary-sanitize` exercise broader
  component builds, but their findings still need source-class mapping because
  they include generated output, copied source trees, and dependency internals.
- Slice02 should proceed as a real source-layout/build-surface normalization
  slice before expanding first-party quality thresholds.

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | `test -f docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md` passes. |
| F-2 | done | The report defines `first-party maintained`, `first-party generated`, `first-party generator/template`, `vendored third-party`, `third-party generated`, `copied build artifact`, and `generated build output`, and avoids the forbidden ambiguity. |
| F-3 | done | The path inventory names `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine`, and `pandaPI/runtime` with source classes. |
| F-4 | done | The `Generated-code ownership` section maps Flex, Bison, gengetopt, templates, third-party skeleton output, and Re-entry conditions. |
| F-5 | done | The third-party/dependency table classifies cpddl, H2, nested bliss/boruvka/lpsolve/opts, CUDD, copied build artifacts, generated build output, exclusions, separate reporting, and rationale. |
| F-6 | done | The existing gate map covers format, static analysis, coverage, warnings, sanitizers, unit/seam tests, and contract tests by component/source class. |
| F-7 | done | The recommendation section selects Slice02 restructuring for selectors, compile database isolation, profile isolation, and generated/dependency filtering. |
| F-8 | done | The operator/CDC decision section names Catch2/test dependency posture, threshold posture, release blocker posture, and coverage floor strategy. |
| F-9 | done | Boundary check after staging is expected to show no staged changes under `pandaPI`, `Makefile`, `mk`, `.github`, `README.md`, `release`, `tools/release`, Arc08, or Arc09 paths. |
| F-10 | done | Required checks are listed below and passed unless otherwise noted. |
| F-11 | done | Bubble-up below states the arc plan recommendation before Slice02. |

## Verification

Commands run:

- `git status --short --branch`
- `make help`
- `find pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime -maxdepth 3 -type d | sort`
- `git ls-files pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime`
- `rg -n -- "cmdline\\.c|hddl-token\\.cpp|hddl\\.cpp|hddl\\.hpp|options\\.ggo|gengetopt|flex|bison|prepare_build_source_copy" pandaPI/parser/makefile pandaPI/grounder/src/Makefile pandaPI/engine/src/CMakeLists.txt tools/shared/platform`
- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make warning-inventory`
- `git diff --check`
- `git diff --cached --check`

## Boundary Check

This slice changed Arc07 planning evidence only:

- `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/slice01-source-classification-inventory/ledger.md`
- `docs/design-v0.3.0/arc07-source-quality-expansion/slice01-source-classification-inventory/closing-report.md`

No production source, Make/CI, release, README, Arc08, or Arc09 surface was
changed.

## Bubble-up to Arc07

Recommendation: no arc plan amendment is required before Slice02 if
CDC/operator accept this report. The existing conditional Slice02 should be
activated as the normalization slice for source selectors, compile databases,
profile isolation, generated-code exclusions, copied build artifact mapping,
and third-party/dependency exclusions.

Amend the arc plan only if CDC/operator want to split Slice02 into smaller
selector and compile/profile slices, or if they reject the recommendation to
normalize build surfaces before expanding gates.
