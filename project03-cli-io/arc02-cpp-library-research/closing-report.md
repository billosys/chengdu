# Arc02 Closing Report - cpp-library-research

Closer: CDC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Close commit: this report's adding commit
Gate review: operator requested full Arc02 CDC verification after slice04
verification identified the remaining attested component slices.

## Capability Verdict

Delivered. Arc02 promised:

> Research open source C++ libraries against the parser, grounder, and engine
> audit findings; produce per-component assessments and a final combined
> dependency recommendation set.

The four planned slices compose into that capability. Slice01 defined the
audit-driven workflow and applied it to the parser. Slice02 applied the same
workflow to the grounder and added dependency-boundary containment. Slice03
applied it to the engine and added supported-surface containment for
translation, SAT, BDD, and CUDD. Slice04 compared all three component reports,
refreshed current metadata, and produced the accepted dependency portfolio for
Arc03/Arc04 planning.

This was research/documentation only. No planner source, dependency source,
scripts, workflows, release assets, build outputs, or vendored dependency files
were changed by the arc close.

## Slice Walk

The arc plan lists four slices. All four are closed and CDC-verified:

| Slice | Outcome | Evidence |
|-------|---------|----------|
| slice01 `parser-library-research` | delivered | [`slice01-parser-library-research/cdc-verification.md`](slice01-parser-library-research/cdc-verification.md) accepted six parser research rows at `45360391`. |
| slice02 `grounder-library-research` | delivered | [`slice02-grounder-library-research/cdc-verification.md`](slice02-grounder-library-research/cdc-verification.md) accepted six grounder research rows at `ad4ff471`. |
| slice03 `engine-library-research` | delivered | [`slice03-engine-library-research/cdc-verification.md`](slice03-engine-library-research/cdc-verification.md) accepted six engine research rows at `cd669e00`. |
| slice04 `combined-library-recommendations` | delivered | [`slice04-combined-library-recommendations/cdc-verification.md`](slice04-combined-library-recommendations/cdc-verification.md) accepted eight combined recommendation rows at `5bfa0d51`, with CDC verification committed at `52b00a24`. |

Slice count matches the arc-plan breakdown: 4 planned, 4 closed, 0 missing.

## Composition Check

The slices compose in the intended order:

1. Slice01 established the reusable workflow and parser candidate map.
2. Slice02 reused the workflow and added grounder-specific dependency-boundary
   evidence.
3. Slice03 reused the workflow and added engine-specific supported-surface
   evidence.
4. Slice04 compared all three inputs and selected one combined portfolio rather
   than blindly unioning candidates.

The final portfolio is coherent:

- **Adopt for 0.3.0:** standard-library modernization, fmt, CLI11 through
  golden-test migration, and Catch2 for C++ regression coverage.
- **Pilot for 0.3.0:** `tl::expected` behind the shared status taxonomy and
  conditional `reproc++` only if Arc03 keeps a supported subprocess-boundary
  path that needs it.
- **Hold:** nlohmann/json, Microsoft GSL, google/benchmark, performance
  containers, parser-generator replacement, CUDD/BDD enablement, and optional
  dependency audits until their re-entry criteria are met.
- **Reject as 0.3.0 foundations:** Abseil and Boost.Process, because their
  breadth/build impact is too large for the evidenced 0.3.0 infrastructure
  needs.

No arc-level contradiction remains. The component reports contain some older
release metadata because they were written as incremental research slices. The
verified combined report is the current recommendation authority and explicitly
refreshed the load-bearing metadata before choosing dispositions.

Arc-level protected-path check:

```bash
git diff --name-only 45360391^..HEAD -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output. Arc02 has not changed planner source, dependency source,
scripts, workflows, README, release manifest, `vendor.env`, `pins.env`, `dist`,
or `build` paths.

## Arc Ledger Walk

Rows verified: 5. Done: 5. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| A1 | Done, reproduced. `arc-plan.md` section 2 records a repeatable workflow: read the audit as the problem statement, map defect classes to capability buckets, prefer standard-library fixes first, collect current evidence, score against chengdu constraints, assign dispositions, record non-adoptions, and bubble up cross-component questions. |
| A2 | Done, reproduced. Parser, grounder, and engine each have CDC-verified component-level research slices mapping accepted audit findings to candidate libraries and dispositions. |
| A3 | Done, reproduced. Adopted and piloted candidates in the component reports and final combined report record license, maintenance/release evidence, integration mode, build/packaging impact, migration risk, and concrete audit defect classes. The combined report is the authoritative refreshed metadata surface. |
| A4 | Done, reproduced. `combined-library-recommendations.md` compares all three component reports and separates standard-library baseline, 0.3.0 adoption candidates, 0.3.0 pilots, held candidates, and rejected foundation choices. |
| A5 | Done, reproduced. Arc03 can plan from the combined report without silent drops: accepted dependencies have Arc03/Arc04/Arc05 ownership, pilots have gates, holds have re-entry criteria, and rejected/no-op choices have rationale. |

## Accumulated Plan Changes

Arc02 plan changes were tracked in `arc-plan.md`:

- v1.0: Arc02 opened with the reusable workflow and parser research slice.
- v1.1: Slice02 grounder research status landed after the accepted grounder
  audit.
- v1.2: Slice03 engine research status landed after the accepted engine audit.
- v1.3: Slice04 combined recommendations opened with explicit
  architecture-impact and supported-surface criteria.
- v1.4: Slice04 CDC verification accepted the combined recommendations and
  left the component slices for this full Arc02 CDC pass.
- v1.5: Arc02 close verified all four slices and closed the arc.

No untracked Arc02 findings remain. Deferred work is explicit Arc03/Arc04 or
later material, not hidden Arc02 scope.

## Bubble-up to Project

Arc02 delivered project ledger row P2's research input: parser, grounder, and
engine library-research reports plus a combined recommendation report. Every
candidate family is mapped to concrete audit defect classes, license/build/
packaging consequences, maintenance evidence, and an adopt/pilot/hold/reject
disposition.

Arc03 should consume the combined report as a design constraint, not as an
implementation shortcut. In particular:

- CLI11 adoption changes help text, parse errors, defaults, and aliases; it
  needs golden CLI/process tests.
- fmt should sit under a chengdu diagnostics/process facade, not sprawl through
  planner internals.
- `tl::expected` must wait for the status/error taxonomy before public API
  exposure.
- `reproc++` remains conditional on Arc03 supported-surface decisions for H2 or
  translation containment.
- JSON/event output, GSL, performance libraries, parser-generator replacement,
  CUDD/BDD, SAT, H2, and cpddl work remain gated by explicit re-entry criteria.

Project-plan change required: yes, and applied in this close. `project-plan.md`
moves Arc02 from active to closed and keeps Arc03-Arc06 as roadmap-only until
Arc01 is arc-closed too.

## What Worked

- The same audit-driven workflow across all three components made the final
  synthesis comparable.
- The final combined report refreshed metadata and resolved older per-slice
  drift before making decisions.
- Keeping supported-surface classification separate from library availability
  protected Arc03 from inheriting accidental product commitments.
