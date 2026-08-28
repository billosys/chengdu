# CDC Verification: combined-library-recommendations

Date: 2026-08-09

Branch: `release/0.3.x`

CC close commit:

- `5bfa0d51` - `docs: close combined library recommendations slice`

Verdict: **accepted**. Slice04 is CDC-verified.

## Row Count

Opening ledger row count at `757df527`: 8.

Closing ledger row count at `5bfa0d51`: 8.

No ledger rows were dropped.

## Commit and Scope Check

`5bfa0d51` changes only:

- `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice04-combined-library-recommendations/closing-report.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice04-combined-library-recommendations/ledger.md`

The commit includes the required assistant co-author trailers.

Protected-path check:

```bash
git diff --name-only 5bfa0d51^..5bfa0d51 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output. No planner source, dependency source, scripts, workflows,
README, release manifest, `vendor.env`, `pins.env`, `dist`, or `build` paths
changed in the close commit.

Additional format check:

```bash
git diff --check 5bfa0d51^..5bfa0d51
```

Result: no output.

## Ledger Verification

CDC reran every ledger Verify command. All rows reproduce.

| Row | CDC disposition | Evidence |
|-----|-----------------|----------|
| F-1 | accepted | Combined report exists and names `parser-library-research`, `grounder-library-research`, and `engine-library-research` inputs. |
| F-2 | accepted | Report maps the adopted and piloted recommendations to concrete `P-*`, `G-*`, and `E-*` audit finding IDs. |
| F-3 | accepted | Adopt and pilot candidates record license, maintenance/release evidence, integration mode, build/packaging impact, migration risk, and disposition. |
| F-4 | accepted | Report separates standard-library baseline, 0.3.0 adopt, 0.3.0 pilot, hold, and reject decisions. |
| F-5 | accepted | Architecture-impact section covers Arc03 managed-process design, Arc04 shared substrate, Arc05 adoption, release packaging, NOTICE/license, rollback, and deferral. |
| F-6 | accepted | Optional and supported-surface decisions for translation, interactive, SAT, BDD, CUDD, H2, and cpddl remain separate from library availability. |
| F-7 | accepted | Sources section records current source/release links for adopt/pilot candidates and broad held/rejected choices including Boost.Process, GSL, and Abseil. |
| F-8 | accepted | Protected-path diff check produced no output. |

Supporting count checks:

| Check | Result |
|-------|--------|
| `rg -c '^\\| F-' docs/design-v0.3.0/arc02-cpp-library-research/slice04-combined-library-recommendations/ledger.md` | `8` |
| `rg -n '\\| open \\|' docs/design-v0.3.0/arc02-cpp-library-research/slice04-combined-library-recommendations/ledger.md` | no output |

## Metadata Spot-Check

CDC refreshed the most important source metadata from primary project release
endpoints or official package/library pages on 2026-08-09.

| Candidate | Current evidence checked | Disposition impact |
|-----------|--------------------------|--------------------|
| CLI11 | GitHub latest release `v2.7.2`, published 2026-08-02; release assets include `CLI11.hpp`. | Supports 0.3.0 adoption through a golden-test pilot. |
| fmt | GitHub latest release `12.2.0`, published 2026-06-16. | Supports 0.3.0 adoption for diagnostics and formatted messages. |
| Catch2 | GitHub latest release `v3.15.3`, published 2026-07-26; release assets include amalgamated source/header artifacts. | Supports 0.3.0 adoption for C++ regression coverage. |
| `tl::expected` | GitHub latest release `v1.3.1`, published 2025-09-01. | Supports a contained pilot, not an immediate cross-codebase mandate. |
| reproc | GitHub latest release `v14.2.7`, published 2026-04-15; Conan Center shows `reproc/14.2.7` package data for Linux and macOS. | Supports conditional pilot behind Arc03 managed-process design needs. |
| nlohmann/json | GitHub latest release `v3.12.0`, published 2025-04-11. | Supports hold status until Arc03 proves JSON/event output is needed. |
| Microsoft GSL | GitHub latest release `v4.2.2`, published 2026-05-20. | Supports hold status while local RAII and type cleanup proceed first. |
| Abseil | GitHub latest release `20260526.0`, published 2026-06-01; release notes include a GCC 10+ requirement. | Supports rejection as a 0.3.0 foundation choice despite active maintenance. |
| Boost.Process | Official Boost library page reports Boost.Process in Boost `1.91.0`. | Supports rejection as the first process-management dependency. |
| google/benchmark | GitHub latest release `v1.9.5`, published 2026-01-21. | Supports hold status until benchmarking is a real implementation criterion. |
| robin-map / unordered_dense | Latest releases observed as `v1.4.1` and `v4.9.0`, respectively. | Supports hold status for performance-container work until profiler evidence exists. |

Search-index snippets initially disagreed with several latest-release values.
Primary GitHub API and official package/library pages were treated as
authoritative for this verification.

## Report Sampling

CDC sampled the combined recommendation report for the load-bearing claims:

- Section 2 names the three component-level library-research inputs.
- Sections 3 through 6 preserve audit finding IDs while comparing the
  standard-library baseline, fmt, CLI11, Catch2, `tl::expected`, and reproc.
- Section 7 keeps optional inherited surfaces behind Arc03 classification
  rather than letting library availability expand the supported product.
- Section 9 describes the architecture impact for Arc03, Arc04, Arc05, NOTICE
  work, release packaging, rollback, and deferral.
- Section 10 gives the final disposition matrix.
- Section 11 records source links for adopted, piloted, held, and rejected
  candidate families.

The sampled sections support the report's recommendation and bubble-up.

## Bubble-Up Check

Delivered-as-assigned: yes. Slice04 delivered the combined Arc02 artifact,
`combined-library-recommendations.md`, and reconciled parser, grounder, and
engine research into one 0.3.0 dependency portfolio.

Silent-drop diff: complete. No planned recommendation category was dropped,
and no planner source/build/release paths changed.

Arc-plan impact: Arc02 now has all four planned reports. Slice04 is
CDC-verified and can feed Arc03. The component-level research slices are
recorded as attested inputs; before an arc-level Arc02 close, either CDC should
verify those earlier component slices or the arc close should explicitly accept
their attested status as sufficient for research-only inputs.

Project-plan impact: Arc03 can now plan from the accepted combined portfolio:
standard-library modernization, fmt, CLI11, and Catch2 as the near-term
selection set; `tl::expected` and reproc as gated pilots; JSON, GSL,
performance containers, benchmarking, parser generators, and optional planner
subsystems held or rejected until their re-entry criteria are met.

## What Worked

- The combined report used the same disposition vocabulary as the component
  reports, so CDC could verify composition rather than normalize terminology.
- Refreshed primary-source metadata made release drift explicit.
- The recommendation keeps architecture ownership with Arc03 and Arc04 instead
  of letting dependency choices silently define the product surface.
