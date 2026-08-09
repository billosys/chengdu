# CDC Verification: grounder-library-research

Date: 2026-08-09

Branch: `release/0.3.x`

CC close commit:

- `ad4ff471` - `docs: add grounder library research slice`

Verdict: **accepted**. Slice02 is CDC-verified.

## Row Count

Ledger row count: 6.

Closing report row count: 6.

No ledger rows were dropped.

## Commit and Scope Check

`ad4ff471` changes only Arc02 design/planning files:

- `docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/cc-prompt.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/ledger.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/slice-doc.md`

The commit includes the required assistant co-author trailers.

Protected-path check:

```bash
git diff --name-only ad4ff471^..ad4ff471 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output. No planner source, dependency source, scripts, workflows,
README, release manifest, `vendor.env`, `pins.env`, `dist`, or `build` paths
changed in the close commit.

Additional format check:

```bash
git diff --check ad4ff471^..ad4ff471
```

Result: no output.

## Ledger Verification

CDC reran every ledger Verify command. All rows reproduce.

| Row | CDC disposition | Evidence |
|-----|-----------------|----------|
| F-1 | accepted | Closing report maps grounder findings `G-001` through `G-019` to candidate buckets. |
| F-2 | accepted | Adopted or piloted candidates record license, maintenance/release evidence, integration mode, build/packaging impact, risk, and disposition. |
| F-3 | accepted | Recommendations distinguish standard-library baseline, adopt, adopt/pilot, pilot, hold, and reject decisions. |
| F-4 | accepted | Report covers H2/cpddl boundary options, adapter-first sequencing, `reproc++`, Boost.Process, subprocess containment, and dependency-internal scope limits. |
| F-5 | accepted | Sources section records candidate links for CLI11, fmt, `tl::expected`, Catch2, reproc, Boost.Process, nlohmann/json, GSL, Abseil, robin-map, and google/benchmark evidence. |
| F-6 | accepted | Protected-path diff check produced no output. |

Supporting count checks:

| Check | Result |
|-------|--------|
| `rg -c '^\\| F-' docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/ledger.md` | `6` |
| `rg -n '\\| open \\|' docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/ledger.md` | no output |

## Report Sampling

CDC sampled the grounder report for its load-bearing research claims:

- Section 1 identifies dependency-process containment as the grounder-specific
  addition to the parser candidate set.
- Section 2 maps every grounder audit finding to a candidate bucket.
- Section 3 establishes standard-library modernization as the baseline.
- Section 4 assesses shared candidates and grounder-specific boundary
  candidates, including `reproc++`, Boost.Process, GSL, Abseil, performance
  containers, and google/benchmark.
- Section 5 separates adopt, adopt-or-pilot, pilot, hold, and reject decisions.
- Section 6 bubbles up H2/cpddl containment and measurement gates for the final
  combined report.

The report predates later metadata refreshes for some candidates. That is not a
slice failure: this slice is accepted as grounder problem mapping, while
`slice04-combined-library-recommendations` is the current recommendation
authority for final library metadata and dispositions.

## Bubble-Up Check

Delivered-as-assigned: yes. Slice02 applied the Arc02 workflow to the accepted
grounder audit and identified dependency-boundary containment as a distinct
architecture concern.

Silent-drop diff: complete. No requested grounder research category was
dropped, and no planner source/build/release paths changed.

## What Worked

- Reusing the parser report vocabulary made the cross-component comparison
  mechanical instead of rhetorical.
- The adapter-first sequence kept H2/cpddl process containment from becoming an
  automatic process-library adoption.
