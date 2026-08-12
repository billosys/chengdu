# Arc06 Slice05: compiler-warning-burndown

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | A reproducible warning-inventory entrypoint or exact command sequence is documented; if a Make target is added, `make help` lists it. | `set -e; rg -n -- "warning inventory|compiler-warning|warning-burndown|make .*warning" Makefile mk docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md >/dev/null` | serious | Arc06 A7 | done | `COMMIT_TBD`; `make warning-inventory` passed: `Warning lines: 56`, `Full build log: build/warnings/macos-arm64/build.log`, `Warning inventory: build/warnings/macos-arm64/warning-inventory.txt`; verify grep passed. | Make-backed target added and listed in help; no direct-script workflow documented. |
| F-2 | Current parser warnings are inventoried and classified by ownership tier, with `P-011` dispositioned. | `set -e; rg -n -- "P-011|parser|generated|primary|third-party|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | Arc01 parser audit | done | `COMMIT_TBD`; verify grep passed; `warning-inventory.md` records generated Flex helpers as the only remaining parser warning class. | Primary parser warnings in `hpdlWriter.cpp`, `plan.cpp`, and `verify.cpp` were fixed. |
| F-3 | Current grounder warnings are inventoried and classified by ownership tier, with `G-015` dispositioned. | `set -e; rg -n -- "G-015|grounder|generated|primary|third-party|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | Arc01 grounder audit | done | `COMMIT_TBD`; verify grep passed; `warning-inventory.md` separates primary grounder fixes from cpddl/boruvka/opts/lpsolve/RSS/H2 dependency-internal warnings. | Primary grounder warnings observed in this slice were fixed or marked as no-op/debug-only; dependency-internal warnings remain budgeted. |
| F-4 | Current engine warnings are inventoried and classified by ownership tier, with `E-011` dispositioned. | `set -e; rg -n -- "E-011|engine|generated|primary|third-party|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | Arc01 engine audit | done | `COMMIT_TBD`; verify grep passed; `warning-inventory.md` budgets the remaining engine payload-cast and duplicate-library warnings with re-entry conditions. | Low-risk `-lpthread`, tag mismatch, override, and debug/layout warning fixes were applied. |
| F-5 | Runtime warnings are inventoried and must be zero or explicitly justified; owned runtime warnings are not budgeted silently. | `set -e; rg -n -- "pandaPI/runtime|runtime|zero|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown pandaPI/runtime/README.md >/dev/null` | correctness-grade | Arc06 owned-runtime quality | done | `COMMIT_TBD`; verify grep passed; `make warning-inventory` observed zero runtime warning lines. | Runtime remains the zero-warning owned surface. |
| F-6 | Low-risk primary hand-written warning fixes are applied where safe, and any remaining primary warning budget has rationale plus release re-entry condition. | `set -e; rg -n -- "fixed|remaining|budget|Re-entry|primary hand-written|primary-source" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | operator release-readiness review | done | `COMMIT_TBD`; verify grep passed; `warning-inventory.md` lists fixed classes and the remaining engine primary budget. | Engine `VisitedList.cpp` payload cast remains budgeted because representation rewrite needs targeted fixture/sanitizer evidence. |
| F-7 | Generated and nested third-party/dependency-internal warnings are isolated and are not hidden behind blanket suppressions or global `-Werror` exceptions. | `set -e; rg -n -- "generated|third-party|dependency-internal|suppression|suppressions|-Werror|blanket" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown Makefile mk pandaPI >/dev/null` | correctness-grade | warning ownership policy | done | `COMMIT_TBD`; verify grep passed; no blanket suppressions or global warning-error promotion were added. | Remaining generated/dependency warnings have explicit re-entry criteria. |
| F-8 | Any new CI or workflow behavior invokes Make targets rather than direct compiler/script commands. | `make safety-checks && make actionlint` | serious | CI entrypoint policy | done | `COMMIT_TBD`; `make safety-checks` passed; `make actionlint` passed. | No workflow change was made; new local behavior is Make-backed. |
| F-9 | Existing quality and behavior gates still pass after warning fixes or inventory wiring. | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` | serious | no regression | done | `COMMIT_TBD`; `make static-analysis`, `make format-check`, `make test`, `make provenance-check`, `git diff --check`, and `git diff --cached --check` passed. | `make test` covered runtime, contract, managed, smoke, and negative smoke gates. |
| F-10 | The slice stays inside Arc06 warning-burndown scope and does not modify release publication, Arc07, Arc08, or unsupported optional-surface behavior. | `set -e; ! git diff --cached --name-only -- README.md release tools/release .github/workflows/release.yml docs/design-v0.3.0/arc07-pandapi-tutorial-docs docs/design-v0.3.0/arc08-release-prep-publication | rg .` | serious | Arc06 boundary | done | `COMMIT_TBD`; boundary check passed with no matching staged paths. | Parser/grounder/engine/runtime source edits are scoped to warning burndown; no release, Arc07, or Arc08 surface changed. |

## What Good Looks Like

- The warning inventory is specific enough that a future reviewer can tell
  primary fork debt from generated or dependency-internal noise.
- Low-risk primary-source fixes reduce the visible warning stream without
  changing planner behavior.
- Remaining warning debt is budgeted honestly before sanitizer and release
  gates depend on the build signal.

## What Worked

- Capturing the full build once through `make warning-inventory` made the
  primary/generated/dependency split concrete enough to fix low-risk warnings
  without chasing nested third-party code.
- Keeping generated reports under ignored `build/warnings/<platform>/` avoided
  committing volatile copied build-tree paths while preserving reproducible
  local evidence for CDC.
