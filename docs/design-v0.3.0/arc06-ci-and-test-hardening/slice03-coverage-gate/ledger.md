# Arc06 Slice03: coverage-gate

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Make exposes coverage target(s), and `make help` lists the public coverage entrypoint. | `set -e; make help | rg -n "coverage"` | serious | Make entrypoint policy | open | | Expected public entrypoint: `make coverage`; helper targets may exist under `mk/*`. |
| F-2 | The coverage implementation uses Clang source-based coverage when available and fails or skips explicitly when required LLVM coverage tools are unavailable. | `set -e; rg -n -- "-fprofile-instr-generate|-fcoverage-mapping|llvm-profdata|llvm-cov|coverage" Makefile mk pandaPI/runtime docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate >/dev/null` | correctness-grade | `ci-notes.md`; Arc06 plan | open | | Skip messages must be explicit; silent no-op coverage is not acceptable. |
| F-3 | Runtime coverage builds `pandaPI/runtime` with coverage instrumentation, runs the runtime CTest workload, merges profiles, and emits a deterministic report. | `make coverage` | serious | Arc06 A5 | open | | If helper targets are added, `make coverage` must drive or document them. |
| F-4 | The coverage report includes owned runtime source and test paths under `pandaPI/runtime/`, not the retired runtime source path. | `set -e; make coverage; rg -n -- "pandaPI/runtime" build docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate pandaPI/runtime/README.md >/dev/null; ! rg -n -- "pandapi[-]runtime/" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate pandaPI/runtime/README.md` | serious | post-layout source policy | open | | Historical design docs outside this slice are not part of this grep. |
| F-5 | Generated, inherited planner, nested third-party, build, and distribution paths are excluded or reported separately by written policy. | `set -e; for term in "generated" "inherited" "third-party" "build" "dist" "excluded"; do rg -n -- "$term" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate pandaPI/runtime/README.md >/dev/null; done` | correctness-grade | Arc06 coverage scope principle | open | | Do not present inherited planner algorithm coverage as owned-process coverage. |
| F-6 | Chengdu-owned adoption seams are either included in coverage evidence or explicitly deferred with a technical reason and re-entry condition. | `set -e; rg -n -- "adoption seam|pandapi_.*_native|defer|Re-entry" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice03-coverage-gate docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md >/dev/null` | correctness-grade | Arc06 A5 | open | | Deferral is acceptable only if instrumentation/profile collection is not stable in this slice. |
| F-7 | Coverage output is written under generated output directories and does not add committed report artifacts. | `set -e; make coverage; ! git status --short -- build dist | rg .` | serious | repo hygiene | open | | If a generated report path is ignored, document where to find it locally. |
| F-8 | Any new CI or workflow behavior invokes Make targets rather than direct scripts or harness commands. | `make safety-checks && make actionlint` | serious | CI entrypoint policy | open | | If no workflow changes are made, this still proves the safety check. |
| F-9 | Existing quality and behavior gates still pass after coverage wiring. | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` | serious | no regression | open | | `make check` may be used instead if it covers these gates on the local platform. |
| F-10 | The slice stays inside Arc06 coverage scope and does not modify product behavior, release packaging/publication, README migration prose, Arc07, or Arc08 surfaces. | `set -e; ! git diff --cached --name-only -- README.md release tools/release .github/workflows/release.yml docs/design-v0.3.0/arc07-pandapi-tutorial-docs docs/design-v0.3.0/arc08-release-prep-publication | rg .` | serious | Arc06 boundary | open | | If workflow coverage is added, it must be Make-backed and reviewed separately from release workflow changes. |

## What Good Looks Like

- A fresh contributor can run `make help`, find the coverage target, run it,
  and open or read the generated report without knowing the internal CMake or
  LLVM commands.
- The report is honest about what it measures: owned runtime/process-policy
  code first, adoption seams only where stable, inherited and third-party code
  excluded or separated.
- Any coverage limitation becomes a routed Arc06 fact, not a hidden
  percentage gap.
