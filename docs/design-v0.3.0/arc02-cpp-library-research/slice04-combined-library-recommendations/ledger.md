# Slice 04: combined-library-recommendations

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The combined recommendation report exists at `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` and names the three component library-research inputs. | `test -f docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md && rg -n "parser-library-research|grounder-library-research|engine-library-research" docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` | serious | arc ledger A2 | open | | |
| F-2 | The report maps every adopt or pilot recommendation to concrete parser, grounder, and/or engine audit finding IDs. | `rg -n "P-00[1-9]|P-01[0-5]|G-00[1-9]|G-01[0-9]|E-00[1-9]|E-01[0-6]" docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` | correctness-grade | arc ledger A3 | open | | |
| F-3 | Adopt/pilot candidates record current license, maintenance/release evidence, integration mode, build/packaging impact, migration risk, and disposition. | `rg -n "License|Maintenance|Release|Integration|Build/packaging|Migration risk|Disposition|adopt|pilot" docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` | correctness-grade | arc ledger A3 | open | | |
| F-4 | The report explicitly separates standard-library baseline, 0.3.0 adopt, 0.3.0 pilot, hold, and reject decisions. | `rg -n "Standard-library baseline|Adopt for 0\\.3\\.0|Pilot for 0\\.3\\.0|Hold|Reject" docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` | serious | arc ledger A4 | open | | |
| F-5 | The report includes an architecture-impact section showing how dependency choices affect Arc03 managed-process design, Arc04 shared substrate, Arc05 adoption, and release packaging. | `rg -n "Architecture impact|Arc03|Arc04|Arc05|shared substrate|managed-process|release packaging|NOTICE|license" docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` | correctness-grade | operator architecture guidance; arc ledger A5 | open | | |
| F-6 | The report keeps optional/supported-surface decisions separate from library availability for translation, interactive, SAT, BDD, CUDD, H2, and cpddl paths. | `rg -n "supported surface|translation|interactive|SAT|BDD|CUDD|H2|cpddl|optional|experimental" docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` | serious | engine/grounder bubble-up | open | | |
| F-7 | The report records current upstream source links for all adopt/pilot candidates and any rejected broad foundation choices. | `rg -n "Sources|CLI11|fmt|tl::expected|Catch2|nlohmann/json|reproc|Boost\\.Process|GSL|Abseil" docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md` | correctness-grade | current-library evidence | open | | |
| F-8 | No planner source, dependency source, scripts, workflows, release assets, build outputs, or vendored dependency files are changed by this slice. | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` | serious | slice constraint | open | | |

## What Worked

Open. Fill at close with patterns that made the combined recommendation reliable.
