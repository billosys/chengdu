# chengdu arc02 / slice04 - combined-library-recommendations - closing report

Closer: CC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Research HEAD: `757df527`
Slice doc: [`slice-doc.md`](slice-doc.md)
Ledger: [`ledger.md`](ledger.md)
Report: [`../combined-library-recommendations.md`](../combined-library-recommendations.md)
Gate: proposed GO, pending CDC verification

## Slice Verdict

Delivered as assigned. The combined Arc02 library portfolio exists at the
planned arc-local path and compares the parser, grounder, and engine
library-research reports against accepted Arc01 audit defects and refreshed
upstream candidate metadata.

This was research/documentation only. No planner source, dependency source,
scripts, workflows, release assets, build outputs, or vendored dependency files
were changed.

## Ledger Walk

Rows verified: 8. Done: 8. Deferred: 0. No-op: 0.

F-1: done. The report exists at
`docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
and §2 names the three component inputs:
`slice01-parser-library-research`, `slice02-grounder-library-research`, and
`slice03-engine-library-research`.

F-2: done. Report §§3 through 6 map all adopt or pilot recommendations to
concrete audit finding IDs. Standard-library, fmt, CLI11, Catch2,
`tl::expected`, and `reproc++` are each tied to parser, grounder, and/or engine
finding IDs.

F-3: done. Report §§5 and 6 record Disposition, License,
Maintenance/Release, Integration, Build/packaging, Migration risk, source
links, and Arc ownership for the adopt and pilot candidates. Metadata was
refreshed with GitHub API and license `curl -L` commands on 2026-08-09.

F-4: done. Report §§4 through 8 explicitly separate `Standard-library
baseline`, `Adopt for 0.3.0`, `Pilot for 0.3.0`, `Hold`, and `Reject for
0.3.0`, with §10 giving the final matrix.

F-5: done. Report §9 covers Architecture impact, including Arc03
managed-process design, Arc04 shared runtime substrate, Arc05 per-binary
adoption, NOTICE/license work, release packaging, rollback, and deferral
options.

F-6: done. Report §7 separates supported-surface decisions from library
availability for translation, interactive mode, SAT, BDD, CUDD, H2, and cpddl.
The recommendation explicitly leaves those classifications to Arc03 or later
dedicated dependency-audit slices.

F-7: done. Report §11 lists current upstream source/release links for all
adopt/pilot candidates and broad held/rejected choices, including CLI11, fmt,
`tl::expected`, Catch2, nlohmann/json, reproc, Boost.Process, GSL, Abseil,
performance containers, google/benchmark, and CUDD.

F-8: done. `git diff --name-only -- pandaPI scripts .github README.md
release-manifest.txt vendor.env pins.env dist build` produced no output after
the report, ledger, and closeout edits.

## Commands Run

| Command | Exit | Evidence |
|---------|------|----------|
| `git status --short --branch` | 0 | Initial status was clean on `release/0.3.x`. |
| `git rev-parse --short HEAD` | 0 | Research HEAD `757df527`. |
| `rg -n "chengdu|cpp-guidelines|parser-audit|engine-audit|library|arc02|slice04" /Users/oubiwann/.codex/memories/MEMORY.md` | 0 | Located prior Chengdu audit context. |
| `curl -L https://api.github.com/repos/CLIUtils/CLI11/releases/latest` | 0 | Latest release `v2.7.2`, published 2026-08-02. |
| `curl -L https://api.github.com/repos/fmtlib/fmt/releases/latest` | 0 | Latest release `12.2.0`, published 2026-06-16. |
| `curl -L https://api.github.com/repos/TartanLlama/expected/releases/latest` | 0 | Latest release `v1.3.1`, published 2025-09-01. |
| `curl -L https://api.github.com/repos/catchorg/Catch2/releases/latest` | 0 | Latest release `v3.15.3`, published 2026-07-26. |
| `curl -L https://api.github.com/repos/DaanDeMeyer/reproc/releases/latest` | 0 | Latest release `v14.2.7`, published 2026-04-15. |
| `curl -L https://api.github.com/repos/nlohmann/json/releases/latest` | 0 | Latest release `v3.12.0`, published 2025-04-11. |
| `curl -L https://api.github.com/repos/microsoft/GSL/releases/latest` | 0 | Latest release `v4.2.2`, published 2026-05-20. |
| `curl -L https://api.github.com/repos/abseil/abseil-cpp/releases/latest` | 0 | Latest release `20260526.0`, published 2026-06-01. |
| `curl -L https://api.github.com/repos/google/benchmark/releases/latest` | 0 | Latest release `v1.9.5`, published 2026-01-21. |
| `curl -L https://api.github.com/repos/Tessil/robin-map/releases/latest` | 0 | Latest release `v1.4.1`, published 2025-11-02. |
| `curl -L https://api.github.com/repos/martinus/unordered_dense/releases/latest` | 0 | Latest release `v4.9.0`, published 2026-08-07. |
| `curl -L` license fetches for selected candidates | 0 | Confirmed license text for CLI11, fmt, reproc, GSL, nlohmann/json, Abseil, and google/benchmark where needed. |
| `rg ... combined-library-recommendations.md` ledger verifies | 0 | All F-1 through F-7 report checks reproduced. |
| `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` | 0 | No output. |

## Bubble-up to Arc02

Delivered-as-assigned: yes. Slice04 produced the combined portfolio and made
clear 0.3.0 recommendations: standard-library modernization, fmt, CLI11, and
Catch2 are the near-term selection set; `tl::expected` and `reproc++` are
pilots behind explicit design gates; JSON, GSL, performance containers,
google/benchmark, parser generators, SAT, BDD, CUDD, H2, and cpddl stay held
or rejected until their re-entry criteria are met.

What the arc plan did not anticipate: CLI11 is strong enough to select, but not
safe enough to swap in without a golden-test pilot because help text, parse
errors, defaults, and compatibility aliases are product behavior. The report
therefore recommends "adopt through pilot" rather than a direct rewrite.
Abseil also had stronger current maintenance evidence than expected, but its
compiler-floor and foundation-library breadth still make it a poor 0.3.0 fit.

Silent-drop diff: none. The assignment required comparison of all three
component reports, dispositions for every named candidate family, refreshed
metadata for adopt/pilot candidates, architecture impact, supported-surface
separation, ledger update, closeout report, and no protected-path changes. All
are present. `cdc-verification.md` is intentionally not written by CC.

## What Worked

- The component reports used the same candidate and disposition vocabulary.
- GitHub API release checks made metadata drift explicit without adding local
  scratch files.
- The final matrix kept product-surface decisions separate from library
  availability, which avoids smuggling optional inherited paths into 0.3.0.
