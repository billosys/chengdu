# Slice 02: grounder-library-research

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Grounder research maps candidate categories back to concrete grounder audit findings. | `rg -n "G-00[1-9]|G-01[0-9]|Audit mapping" docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md` | serious | slice scope; arc ledger A2 | done | attested: closing report includes audit mapping for G-001 through G-019. | |
| F-2 | Candidate assessment records license, maintenance/release evidence, integration mode, build/packaging impact, and disposition for adopted or piloted candidates. | `rg -n "License|Maintenance|Integration|Build/packaging|Disposition|adopt|pilot" docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md` | serious | arc ledger A3 | done | attested: closing report records assessment axes and dispositions. | |
| F-3 | Grounder recommendations distinguish standard-library fixes, near-term shared candidates, grounder-specific pilots, holds, and rejects. | `rg -n "Standard-library baseline|Adopt|Adopt or Pilot|Pilot|Hold|Reject" docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md` | correctness-grade | reusable workflow | done | attested: recommendation portfolio is separated by disposition. | |
| F-4 | The report explicitly covers dependency-boundary candidates for H2/cpddl integration and does not silently expand into dependency-internal audit scope. | `rg -n "H2|cpddl|dependency|reproc|Boost\\.Process|adapter|subprocess|dependency-internal" docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md` | serious | grounder audit dependency boundary | done | attested: dependency-boundary section written with re-entry conditions. | |
| F-5 | The report records current upstream source links for candidates used in the assessment. | `rg -n "CLI11|fmt|tl::expected|Catch2|reproc|Boost\\.Process|nlohmann/json|Abseil|GSL|robin-map|google/benchmark|Sources" docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md` | correctness-grade | research evidence | done | attested: source links recorded in closing report. | |
| F-6 | No grounder source, dependency source, scripts, workflows, release assets, or build outputs are changed by this slice. | `git diff --name-only -- pandaPI/pandaPIgrounder scripts .github README.md release-manifest.txt vendor.env pins.env dist build` | serious | slice constraint | done | attested: verification command returns no paths after docs-only edit. | Existing unrelated engine-audit docs are outside this guard. |

## What Worked

- The parser-library report gave a stable comparison vocabulary, so the grounder
  pass could focus on what changed: dependency containment and performance
  measurement.
- Keeping performance libraries behind a measurement gate prevented premature
  adoption based on intuition alone.

## Closure

Closed as an attested research slice on 2026-08-09. Independent CDC
verification is still available as a follow-up if this arc becomes a formal
gate before Arc03 planning.
