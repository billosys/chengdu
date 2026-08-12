# Arc06 Slice07: tsan-and-ci-synthesis

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Current concurrency and process-observation workload is inventoried before adding or deferring TSan. | `set -e; rg -n -- "concurrency|process-observation|supervised|TSan|ThreadSanitizer|workload" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis docs/design-v0.3.0/ci-notes.md >/dev/null` | correctness-grade | Arc06 OQ2; ci-notes 8.3 | open | | TSan must not be symbolic. |
| F-2 | TSan is either added as a Make-backed target or explicitly deferred with concrete reason and re-entry condition. | `set -e; rg -n -- "TSan|ThreadSanitizer|make .*tsan|defer|deferred|Re-entry" Makefile mk docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis >/dev/null` | serious | Arc06 A9 | open | | Do not combine TSan with ASan/UBSan. |
| F-3 | Any new TSan target is listed in `make help`, or the no-target decision is documented. | `set -e; { make help | rg -n "tsan|ThreadSanitizer|race" || rg -n -- "no TSan target|TSan.*deferred" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis; } >/dev/null` | serious | Make entrypoint policy | open | | |
| F-4 | Existing runtime sanitizer and binary sanitizer placement is synthesized for CI/release use. | `set -e; rg -n -- "test-runtime-sanitize|sanitize-runtime|test-binary-sanitize|sanitize-binaries|always-on|scheduled|pre-release|local-only" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md >/dev/null` | serious | Slice06 bubble-up; Arc06 A8/A10 | open | | Include Linux LSan re-entry. |
| F-5 | Coverage, static-analysis, warning-inventory, sanitizer, and TSan dispositions are reflected in the fixture/gate inventory. | `set -e; for term in "make coverage" "make static-analysis-cpp" "make warning-inventory" "make test-binary-sanitize" "TSan"; do rg -n -- "$term" docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md >/dev/null; done` | serious | Arc06 A12 | open | | |
| F-6 | Any CI/GitHub Actions change invokes Make targets only and keeps workflow lint/safety gates passing. | `make safety-checks && make actionlint` | serious | CI entrypoint policy; Arc06 A10 | open | | If no workflow change is made, record that explicitly. |
| F-7 | Existing quality and behavior gates still pass after TSan/CI synthesis changes. | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` | serious | no regression | open | | If `make check` is not run, explain why. |
| F-8 | Arc06 handoff to Arc07 names documentation/tutorial inputs without writing Arc07 public docs. | `set -e; rg -n -- "Arc07|tutorial|documentation|behavior|examples|handoff" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md >/dev/null` | serious | Arc06 A12 | open | | |
| F-9 | Arc06 handoff to Arc08 names release-prep gates without changing release package/publication surfaces. | `set -e; rg -n -- "Arc08|release|LSan|pre-release|package|publication|wolong|handoff" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis docs/design-v0.3.0/arc06-ci-and-test-hardening/arc-plan.md >/dev/null` | serious | Arc06 A12 | open | | |
| F-10 | The slice stays inside Arc06 synthesis scope and does not edit release publication, Arc07, Arc08, README, or unsupported optional-surface behavior. | `set -e; ! git diff --cached --name-only -- README.md release tools/release .github/workflows/release.yml docs/design-v0.3.0/arc07-pandapi-tutorial-docs docs/design-v0.3.0/arc08-release-prep-publication | rg .` | serious | Arc06 boundary | open | | |
| F-11 | Closing report states whether Arc06 can close and identifies any missing row or next iteration if it cannot. | `set -e; rg -n -- "Arc06 can close|Arc06 cannot close|missing|GO|NO-GO|next iteration" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis/closing-report.md docs/design-v0.3.0/arc06-ci-and-test-hardening/closing-report.md 2>/dev/null` | correctness-grade | project close discipline | open | | Slice07 may also create the Arc06 closing report if all rows are ready. |

## What Good Looks Like

- TSan is treated as a workload-dependent race detector, not a badge.
- CI synthesis leaves no ambiguity about which heavy gates are always-on,
  scheduled, pre-release, local-only, or deferred.
- Arc07 and Arc08 can start from concrete evidence instead of re-discovering
  fixture, coverage, static-analysis, warning, and sanitizer status.

## What Worked Previously

- Slice02 supplied the representative managed fixture workload.
- Slice03, Slice04, Slice05, and Slice06 each produced scoped quality gates
  with explicit ownership boundaries and deferrals.
