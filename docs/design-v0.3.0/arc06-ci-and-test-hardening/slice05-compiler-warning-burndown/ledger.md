# Arc06 Slice05: compiler-warning-burndown

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | A reproducible warning-inventory entrypoint or exact command sequence is documented; if a Make target is added, `make help` lists it. | `set -e; rg -n -- "warning inventory|compiler-warning|warning-burndown|make .*warning" Makefile mk docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md >/dev/null` | serious | Arc06 A7 | open | | Prefer Make for repeatability; do not add direct-script docs when a Make target exists. |
| F-2 | Current parser warnings are inventoried and classified by ownership tier, with `P-011` dispositioned. | `set -e; rg -n -- "P-011|parser|generated|primary|third-party|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | Arc01 parser audit | open | | Expected classes include generated Flex helpers and primary parser warnings such as sign-compare/range-loop/unused state. |
| F-3 | Current grounder warnings are inventoried and classified by ownership tier, with `G-015` dispositioned. | `set -e; rg -n -- "G-015|grounder|generated|primary|third-party|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | Arc01 grounder audit | open | | Separate grounder-owned warnings from cpddl, boruvka, h2, lpsolve, and platform semaphore warnings. |
| F-4 | Current engine warnings are inventoried and classified by ownership tier, with `E-011` dispositioned. | `set -e; rg -n -- "E-011|engine|generated|primary|third-party|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | Arc01 engine audit | open | | Include repeated `-lpthread`, tag mismatch, unused field/variable, and related engine classes. |
| F-5 | Runtime warnings are inventoried and must be zero or explicitly justified; owned runtime warnings are not budgeted silently. | `set -e; rg -n -- "pandaPI/runtime|runtime|zero|warning" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown pandaPI/runtime/README.md >/dev/null` | correctness-grade | Arc06 owned-runtime quality | open | | Runtime is chengdu-owned and should have the highest warning bar. |
| F-6 | Low-risk primary hand-written warning fixes are applied where safe, and any remaining primary warning budget has rationale plus release re-entry condition. | `set -e; rg -n -- "fixed|remaining|budget|Re-entry|primary hand-written|primary-source" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown >/dev/null` | correctness-grade | operator release-readiness review | open | | Do not fix warnings by changing planner semantics without targeted fixture evidence. |
| F-7 | Generated and nested third-party/dependency-internal warnings are isolated and are not hidden behind blanket suppressions or global `-Werror` exceptions. | `set -e; rg -n -- "generated|third-party|dependency-internal|suppression|suppressions|-Werror|blanket" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown Makefile mk pandaPI >/dev/null` | correctness-grade | warning ownership policy | open | | Suppressions, if any, must be narrow and justified. |
| F-8 | Any new CI or workflow behavior invokes Make targets rather than direct compiler/script commands. | `make safety-checks && make actionlint` | serious | CI entrypoint policy | open | | If no workflow changes are made, this still proves the safety check. |
| F-9 | Existing quality and behavior gates still pass after warning fixes or inventory wiring. | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` | serious | no regression | open | | `make check` may be used if it covers these gates on the local platform. |
| F-10 | The slice stays inside Arc06 warning-burndown scope and does not modify release publication, Arc07, Arc08, or unsupported optional-surface behavior. | `set -e; ! git diff --cached --name-only -- README.md release tools/release .github/workflows/release.yml docs/design-v0.3.0/arc07-pandapi-tutorial-docs docs/design-v0.3.0/arc08-release-prep-publication | rg .` | serious | Arc06 boundary | open | | Parser/grounder/engine/runtime warning fixes are allowed only when scoped and behavior-preserving. |

## What Good Looks Like

- The warning inventory is specific enough that a future reviewer can tell
  primary fork debt from generated or dependency-internal noise.
- Low-risk primary-source fixes reduce the visible warning stream without
  changing planner behavior.
- Remaining warning debt is budgeted honestly before sanitizer and release
  gates depend on the build signal.
