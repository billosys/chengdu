# Slice 01: parser-library-research

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The reusable workflow for audit-driven C++ library research is recorded in `../arc-plan.md`. | `rg -n "Reusable workflow|Read the audit as the problem statement|Apply the standard-library-first screen|Assign a disposition" docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md` | serious | operator request; arc ledger A1 | done | attested: workflow section written in `../arc-plan.md`. | |
| F-2 | Parser research maps candidate categories back to concrete parser audit findings. | `rg -n "P-00[1-9]|P-01[0-5]|Audit mapping" docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/closing-report.md` | serious | slice scope | done | attested: closing report includes audit mapping and candidate tables. | |
| F-3 | Candidate assessment records license, maintenance/release evidence, integration mode, build/packaging impact, and disposition for each adopted or piloted candidate. | `rg -n "License|Maintenance|Integration|Build/packaging|Disposition|adopt|pilot" docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/closing-report.md` | serious | arc ledger A3 | done | attested: closing report records assessment axes and dispositions. | |
| F-4 | Parser recommendations explicitly distinguish standard-library fixes, near-term third-party adoption, pilot candidates, holds, and rejects. | `rg -n "Standard-library baseline|Adopt|Pilot|Hold|Reject" docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/closing-report.md` | correctness-grade | reusable workflow | done | attested: recommendation sections written. | |
| F-5 | The report records current upstream source links for candidates used in the assessment. | `rg -n "CLI11|fmt|spdlog|tl::expected|Boost.Outcome|Catch2|nlohmann/json|Bison|ANTLR|PEGTL|Sources" docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/closing-report.md` | correctness-grade | research evidence | done | attested: source links recorded in closing report. | |
| F-6 | No parser source, scripts, workflows, release assets, or build outputs are changed by this slice. | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` | serious | slice constraint | done | attested: verification command returns no paths after docs-only edit. | |

## What Worked

- Starting from audit finding IDs kept the candidate list grounded.
- Treating parser-generator replacement separately prevented the largest option
  from drowning out smaller, higher-leverage infrastructure wins.

## Closure

Closed as an attested research slice on 2026-08-09. Independent CDC
verification is still available as a follow-up if this arc becomes a formal
gate before Arc03 planning.
