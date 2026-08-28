# Slice 04: audit-synthesis - closing report

Date: 2026-08-09

Branch: `release/0.3.x`

## Per-row walk

| ID | Final status | Evidence |
|----|--------------|----------|
| F-1 | done | Synthesis report exists at `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`; sections 1 and 2 identify parser, grounder, engine, and their `cdc-verification` inputs. |
| F-2 | done | Report section 3 compares shared defect classes across parser, grounder, and engine with concrete `P-*`, `G-*`, and `E-*` finding IDs. |
| F-3 | done | Report section 5 identifies shared runtime/header/library candidates: process facade, status, diagnostics, stdout/stderr, TTY/color, version/provenance, filesystem/resource helpers, timeout/resource test harness, and warning tiers. |
| F-4 | done | Report section 6 contains conservative, moderate, and big-change architecture options with benefits, costs, risks, prerequisites, implementation sequence, and re-entry conditions. |
| F-5 | done | Report section 4 distinguishes supported surface from optional/legacy/experimental paths, including H2, cpddl, translation, interactive, SAT, BDD, and CUDD. |
| F-6 | done | Report sections 5 and 8 provide inputs and non-inputs to Arc02 combined library recommendations and Arc03 managed-process design without making final decisions owned by those arcs. |
| F-7 | done | Report section 7 provides implementation sequencing candidates for Arc02 through Arc06, including contract first, test first, Arc04 shared substrate, and Arc05 binary adoption. |
| F-8 | done | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output. |

## Verification commands

- `test -f docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md && rg -n "audit-results-pandapi-parser|audit-results-pandapi-grounder|audit-results-pandapi-engine|cdc-verification" docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` - exit 0.
- `rg -n "P-00[1-9]|P-01[0-5]|G-00[1-9]|G-01[0-9]|E-00[1-9]|E-01[0-6]|shared defect|cross-codebase" docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` - exit 0.
- `rg -n "shared runtime|shared header|process facade|status|diagnostics|stdout|stderr|TTY|color|version|provenance|filesystem|resource|timeout|test harness" docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` - exit 0.
- `rg -n "Architecture options|conservative|moderate|big-change|cost|risk|benefit|sequenc" docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` - exit 0.
- `rg -n "supported surface|optional|legacy|experimental|H2|cpddl|translation|interactive|SAT|BDD|CUDD" docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` - exit 0.
- `rg -n "Arc02|combined library|Arc03|managed-process|input|non-input|owned by" docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` - exit 0.
- `rg -n "implementation sequencing|Arc04|Arc05|shared substrate|binary adoption|dependency|test first|contract first" docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` - exit 0.
- `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` - exit 0, no output.

## Bubble-up to Arc01

Delivered-as-assigned: yes. Slice04 delivered the remaining Arc01 artifact:
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`.
The report compares all three accepted audits, preserves CDC provenance,
identifies shared/runtime/library candidates, names supported-surface
boundaries, and recommends the moderate shared runtime substrate path.

What the arc-plan did not anticipate: the synthesis makes supported-surface
classification the first Arc03 design gate. That is sharper than the original
arc-plan wording because engine translation/interactive/SAT/BDD/CUDD and
grounder H2/cpddl boundaries can otherwise smuggle optional dependency and
process-library decisions into 0.3.0.

Silent-drop diff: no planner source, dependency source, scripts, workflows,
release assets, `dist`, or `build` outputs changed. This slice did not write
`cdc-verification.md`; CDC remains independent.
