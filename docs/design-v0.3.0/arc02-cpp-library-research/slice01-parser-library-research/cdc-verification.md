# CDC Verification: parser-library-research

Date: 2026-08-09

Branch: `release/0.3.x`

CC close commit:

- `45360391` - `docs: open cpp library research arc`

Verdict: **accepted**. Slice01 is CDC-verified.

## Row Count

Ledger row count: 6.

Closing report row count: 6.

No ledger rows were dropped.

## Commit and Scope Check

`45360391` introduced the Arc02 plan plus the parser library-research slice. It
changes only design/planning files:

- `docs/design-v0.3.0/arc01-vendored-source-audit/arc-plan.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/cc-prompt.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/closing-report.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/ledger.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/slice-doc.md`
- `docs/design-v0.3.0/project-plan.md`

The commit includes the required assistant co-author trailers.

Protected-path check:

```bash
git diff --name-only 45360391^..45360391 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output. No planner source, dependency source, scripts, workflows,
README, release manifest, `vendor.env`, `pins.env`, `dist`, or `build` paths
changed in the close commit.

Additional format check:

```bash
git diff --check 45360391^..45360391
```

Result: no output.

## Ledger Verification

CDC reran every ledger Verify command. All rows reproduce.

| Row | CDC disposition | Evidence |
|-----|-----------------|----------|
| F-1 | accepted | Arc02 `arc-plan.md` records the reusable workflow, including audit-first research, standard-library-first screening, and disposition assignment. |
| F-2 | accepted | Closing report maps parser findings `P-001` through `P-015` to candidate buckets. |
| F-3 | accepted | Adopted or piloted candidates record license, maintenance/release evidence, integration mode, build/packaging impact, risk, and disposition. |
| F-4 | accepted | Recommendations distinguish standard-library baseline, adopt, adopt/pilot, pilot, hold, and reject-for-0.3.0 decisions. |
| F-5 | accepted | Sources section records candidate links for CLI11, fmt, spdlog, `tl::expected`, Boost.Outcome, Catch2, nlohmann/json, Bison, ANTLR, PEGTL, and lexy evidence. |
| F-6 | accepted | Protected-path diff check produced no output. |

Supporting count checks:

| Check | Result |
|-------|--------|
| `rg -c '^\\| F-' docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/ledger.md` | `6` |
| `rg -n '\\| open \\|' docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/ledger.md` | no output |

## Report Sampling

CDC sampled the parser report for its load-bearing research claims:

- Section 2 maps every parser audit finding to a candidate bucket.
- Section 3 establishes standard-library modernization as the baseline.
- Section 4 assesses CLI11, fmt, spdlog, `tl::expected`, Boost.Outcome, Catch2,
  nlohmann/json, Bison C++ skeleton/variants, ANTLR, PEGTL, and lexy.
- Section 5 separates adopt, adopt-or-pilot, pilot, hold, and reject decisions.
- Section 6 bubbles up the need for a minimal cross-component portfolio rather
  than a union of every plausible candidate.

The report predates later metadata refreshes for some candidates. That is not a
slice failure: this slice is accepted as parser problem mapping, while
`slice04-combined-library-recommendations` is the current recommendation
authority for final library metadata and dispositions.

## Bubble-Up Check

Delivered-as-assigned: yes. Slice01 created the reusable Arc02 workflow and
applied it to the accepted parser audit.

Silent-drop diff: complete. No requested parser research category was dropped,
and no planner source/build/release paths changed.

## What Worked

- The audit-finding mapping made the parser report usable as a later
  composition input.
- Holding parser-generator replacement kept immediate process-contract cleanup
  separate from a semantic parser rewrite.
