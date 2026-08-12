# Arc06 Slice06: binary-sanitizer-gates

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Binary sanitizer entrypoint(s) are Make-backed and listed in help. | `set -e; make help | rg -n "sanitize|sanitizer|asan|ubsan|lsan" >/dev/null` | serious | Arc06 A8; Make entrypoint policy | open | | Existing runtime target may remain; any new binary target must be listed in help. |
| F-2 | Existing runtime ASan/UBSan gate still passes and remains scoped to `pandaPI/runtime`. | `make test-runtime-sanitize` | correctness-grade | Arc06 owned-runtime quality | open | | Do not regress the current runtime sanitizer baseline while adding binary coverage. |
| F-3 | Parser sanitizer build/workload is implemented or explicitly skipped with toolchain reason and re-entry condition. | `set -e; rg -n -- "parser|pandapi-parser|ASan|UBSan|LSan|skip|Re-entry" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates >/dev/null` | correctness-grade | Arc05 parser native contract; Arc06 A8 | open | | Prefer running managed parser fixtures against sanitizer-built `pandapi-parser`. |
| F-4 | Grounder sanitizer build/workload is implemented or explicitly skipped with toolchain reason and re-entry condition. | `set -e; rg -n -- "grounder|pandapi-grounder|ASan|UBSan|LSan|skip|Re-entry" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates >/dev/null` | correctness-grade | Arc05 grounder native contract; Arc06 A8 | open | | Dependency-internal findings must be classified, not blanket-suppressed. |
| F-5 | Engine sanitizer build/workload is implemented or explicitly skipped with toolchain reason and re-entry condition. | `set -e; rg -n -- "engine|pandapi-engine|ASan|UBSan|LSan|skip|Re-entry|VisitedList" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates >/dev/null` | correctness-grade | Arc05 engine native contract; Arc06 A8 | open | | The Slice05 `VisitedList.cpp` payload-cast budget must be re-evaluated if sanitizer evidence touches it. |
| F-6 | Representative managed process fixture workload runs under sanitizer-built binaries or a documented equivalent isolated sanitizer distribution. | `set -e; rg -n -- "managed|contract|pipeline|fixture|sanitizer" Makefile mk tests/contract docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates >/dev/null` | correctness-grade | Slice02 workload; Arc06 A3/A4/A8 | open | | At minimum, cover parser, grounder, engine, and representative pipeline behavior where supported. |
| F-7 | ASan, UBSan, and LSan support state is documented by platform/toolchain, including macOS leak-detection limits if present. | `set -e; rg -n -- "ASan|UBSan|LSan|macOS|Linux|toolchain|leak|Re-entry" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates pandaPI/runtime/README.md >/dev/null` | serious | Arc06 A8 | open | | A precise SKIP is acceptable when the toolchain cannot support reliable LSan. |
| F-8 | Sanitizer findings, skips, and suppressions are triaged by ownership tier using Slice05 warning-budget language. | `set -e; rg -n -- "owned|primary|generated|third-party|dependency-internal|suppression|suppressions|warning budget" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/warning-inventory.md >/dev/null` | correctness-grade | Slice05 warning ownership; Arc06 OQ3 | open | | No generic inherited-noise deferral; use concrete ownership classes. |
| F-9 | Any CI/GitHub Actions change invokes Make targets only and keeps workflow lint/safety gates passing. | `make safety-checks && make actionlint` | serious | CI entrypoint policy; Arc06 A10 | open | | If no workflow changes are made, record that explicitly. |
| F-10 | Existing quality and behavior gates still pass after sanitizer wiring or fixes. | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` | serious | no regression | open | | If `make check` is not run, closing evidence must say why and list the narrower reproduced gates. |
| F-11 | The slice stays inside Arc06 sanitizer scope and does not change release publication, Arc07, Arc08, or unsupported optional-surface behavior. | `set -e; ! git diff --cached --name-only -- README.md release tools/release .github/workflows/release.yml docs/design-v0.3.0/arc07-pandapi-tutorial-docs docs/design-v0.3.0/arc08-release-prep-publication | rg .` | serious | Arc06 boundary | open | | Sanitizer instrumentation must not promote optional planner surfaces to supported behavior. |

## What Good Looks Like

- Sanitizer evidence exercises the same canonical binary contract that wolong
  and users will consume, not only runtime helper tests.
- Runtime/adoption/primary-source findings block unless explicitly fixed or
  budgeted with a narrow release re-entry condition.
- Generated and nested third-party findings are not hidden; they are isolated
  enough for Arc08 release preparation to make an honest ship/no-ship call.

## What Worked Previously

- Slice02 made managed parser, grounder, engine, and pipeline fixtures
  representative enough for later quality gates.
- Slice05 reduced build warning noise to named ownership classes, making
  sanitizer findings easier to triage without accidental release overclaim.
