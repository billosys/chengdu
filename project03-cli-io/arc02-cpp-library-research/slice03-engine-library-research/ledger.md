# Slice 03: engine-library-research

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | Engine research maps candidate categories back to concrete engine audit findings. | `rg -n "E-00[1-9]|E-01[0-6]|Audit mapping" docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/closing-report.md` | serious | slice scope; arc ledger A2 | done | attested: closing report maps E-001 through E-016 to candidate buckets and cites the same finding IDs in assessments. | |
| F-2 | Candidate assessment records license, maintenance/release evidence, integration mode, build/packaging impact, and disposition for adopted or piloted candidates. | `rg -n "License|Maintenance|Integration|Build/packaging|Disposition|adopt|pilot" docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/closing-report.md` | serious | arc ledger A3 | done | attested: closing report records assessment axes and dispositions for adopted/piloted candidates. | |
| F-3 | Engine recommendations distinguish standard-library fixes, near-term shared candidates, engine-specific pilots, holds, and rejects. | `rg -n "Standard-library baseline|Adopt|Adopt or Pilot|Pilot|Hold|Reject" docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/closing-report.md` | correctness-grade | reusable workflow | done | attested: recommendation portfolio is separated by disposition. | |
| F-4 | The report explicitly covers translation/SAT/BDD child-process and optional-dependency boundaries without silently expanding into dependency-internal audit scope. | `rg -n "translation|SAT|BDD|CUDD|reproc|Boost\\.Process|child-process|optional dependency|supported surface" docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/closing-report.md` | serious | engine audit boundary findings | done | attested: supported-surface containment and optional-dependency boundaries are explicit. | |
| F-5 | The report records current upstream source links for candidates used in the assessment. | `rg -n "CLI11|fmt|tl::expected|Catch2|reproc|Boost\\.Process|nlohmann/json|Abseil|GSL|robin-map|unordered_dense|google/benchmark|CUDD|Sources" docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/closing-report.md` | correctness-grade | research evidence | done | attested: source links recorded in closing report. | |
| F-6 | No engine source, dependency source, scripts, workflows, release assets, or build outputs are changed by this slice. | `git diff --name-only -- pandaPI/pandaPIengine scripts .github README.md release-manifest.txt vendor.env pins.env dist build` | serious | slice constraint | done | attested: verification command returned no paths after docs-only edits. | Existing unrelated Arc01 audit docs are outside this guard. |

## What Worked

- The parser and grounder reports made the shared candidates stable enough that
  the engine pass could focus on what changed: supported-surface containment
  for translation/SAT/BDD/CUDD and stronger measurement gates for performance
  libraries.
- Treating optional engine modes as product-surface decisions prevented a
  dependency from silently making inherited experimental paths first-class.

## Closure

Closed as an attested research slice on 2026-08-09. Independent CDC
verification is still available as a follow-up if this arc becomes a formal
gate before Arc03 planning.
