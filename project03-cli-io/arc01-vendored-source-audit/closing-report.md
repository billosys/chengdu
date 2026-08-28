# Arc01 Closing Report - vendored-source-audit

Closer: CDC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Close commit: this report's adding commit
Gate review: operator requested Arc01 arc-level close after all four slices were
CDC-verified and Arc02 was closed.

## Capability Verdict

Delivered. Arc01 promised:

> Complete per-repo audits for parser, grounder, and engine, then synthesize
> cross-codebase recommendations for cleanup, shared code, process contracts,
> and dependency choices.

The four planned slices compose into that capability. Slice01 audited
`pandaPIparser`; slice02 audited `pandaPIgrounder` and its grounder-owned
`cpddl` / `h2-fd-preprocessor` integration boundary; slice03 audited
`pandaPIengine`; slice04 synthesized the three accepted reports into
cross-codebase architecture, cleanup, dependency, and managed-process inputs.

This was diagnosis/documentation only. No planner source, dependency source,
scripts, workflows, release assets, build outputs, or vendored dependency files
were changed by the arc close.

## Slice Walk

The arc plan lists four slices. All four are closed and CDC-verified:

| Slice | Outcome | Evidence |
|-------|---------|----------|
| slice01 `parser-audit` | delivered | [`slice01-parser-audit/cdc-verification.md`](slice01-parser-audit/cdc-verification.md) accepted 12 parser audit rows at `afe18cd4`; the durable accepted report is [`audit-results-pandapi-parser.md`](audit-results-pandapi-parser.md). |
| slice02 `grounder-audit` | delivered | [`slice02-grounder-audit/cdc-verification.md`](slice02-grounder-audit/cdc-verification.md) accepted 14 grounder audit rows at `af584cae`; the durable accepted report is [`audit-results-pandapi-grounder.md`](audit-results-pandapi-grounder.md). |
| slice03 `engine-audit` | delivered | [`slice03-engine-audit/cdc-verification.md`](slice03-engine-audit/cdc-verification.md) accepted 14 engine audit rows at `6bb2a3fb`; the durable accepted report is [`audit-results-pandapi-engine.md`](audit-results-pandapi-engine.md). |
| slice04 `audit-synthesis` | delivered | [`slice04-audit-synthesis/cdc-verification.md`](slice04-audit-synthesis/cdc-verification.md) accepted 8 synthesis rows at `6b6d66ab`; the accepted synthesis is [`audit-synthesis-pandapi.md`](audit-synthesis-pandapi.md). |

Slice count matches the arc-plan breakdown: 4 planned, 4 closed, 0 missing.

## Composition Check

The slices compose in the intended order:

1. Slice01 established the audit shape on the parser and promoted the accepted
   report into the arc-local evidence path.
2. Slice02 reused that shape for the grounder and made dependency-boundary
   scope explicit for `cpddl` and `h2-fd-preprocessor`.
3. Slice03 completed the three primary vendored-repo audit set with engine
   evidence focused on status, stdout/stderr, timeout/signal, optional modes,
   build/test posture, and source quality.
4. Slice04 compared all three reports and produced prioritized cross-codebase
   recommendations for Arc02 through Arc06.

The report set is complete:

| Report | Findings | Clean checks | Composition role |
|--------|----------|--------------|------------------|
| Parser | 15 findings, `P-001` through `P-015` | 8 clean checks | Establishes parser process-contract, CLI, status, stream, and parser API risks. |
| Grounder | 19 findings, `G-001` through `G-019` | 9 clean checks | Adds grounder dependency-boundary and H2/cpddl integration risks. |
| Engine | 16 findings, `E-001` through `E-016` | at least 8 clean checks | Adds timeout/signal, translation, interactive, SAT, BDD/CUDD, and engine adoption risks. |
| Synthesis | 8 accepted ledger rows | n/a | Recombines all findings into architecture and sequencing recommendations. |

The synthesis resolves the main arc-level integration questions:

- Shared error/status defects recur across parser `P-004`/`P-005`, grounder
  `G-004`/`G-005`/`G-006`, and engine `E-002`/`E-003`/`E-008`/`E-009`.
- Shared stdout/stderr/TTY/color defects recur across parser `P-006`/`P-007`,
  grounder `G-007`/`G-008`/`G-009`, and engine `E-004`/`E-005`.
- CLI identity and `pandapi-*` naming work is routed to Arc03, with binary
  adoption routed to Arc05.
- Shared runtime candidates are identified for process/status, diagnostics,
  color/TTY, version/provenance, filesystem/output/temp RAII, checked
  parsing/model validation, process fixtures, and warning-policy tiers.
- Optional and dependency-heavy surfaces are explicitly fenced: H2/cpddl,
  translation, interactive mode, SAT, BDD, and CUDD do not become supported
  0.3.0 surfaces merely because libraries or inherited paths exist.
- Architecture sequencing is explicit: Arc03 contract first, Arc04 test/shared
  substrate second, Arc05 per-binary adoption third, Arc06 release hardening
  last.

Arc-level protected-path check:

```bash
git diff --name-only 948c54d3^..HEAD -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output. Arc01 has not changed planner source, dependency source,
scripts, workflows, README, release manifest, `vendor.env`, `pins.env`, `dist`,
or `build` paths.

## Arc Ledger Walk

Rows verified: 6. Done: 6. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| A1 | Done, reproduced. Parser, grounder, and engine audit reports exist at the planned arc-local paths. CDC counted 15 parser findings, 19 grounder findings, and 16 engine findings; each report includes file/line-cited findings, C++ Core Guidelines rule IDs where applicable, Chengdu-specific managed-process labels where needed, and at least five clean checks. |
| A2 | Done, reproduced. All three per-repo reports cover correctness/runtime safety, error handling, stdio/TTY behavior, CLI/API surface, build/test posture, maintainability/shared-code candidates, and modernization candidates. |
| A3 | Done, reproduced. The grounder report distinguishes grounder-owned integration from dependency-internal `cpddl` / `h2-fd-preprocessor` findings and routes deeper dependency audits behind explicit re-entry conditions. |
| A4 | Done, reproduced. `audit-synthesis-pandapi.md` compares parser, grounder, and engine and produces prioritized recommendations for error handling, stdio/process behavior, CLI/binary naming, duplicate-code removal, shared code/header/library candidates, third-party dependency inputs, and implementation sequencing. |
| A5 | Done, reproduced. Arc02 and Arc03 can be planned from the synthesis without silent drops: high-risk findings are routed to later arcs, optional surfaces have classification gates, dependency choices are left to Arc02, and the managed-process contract remains owned by Arc03. |
| A6 | Done, reproduced. Protected-path checks over the Arc01 span and individual slice close commits produced no output; Arc01 is documentation-only. |

## Accumulated Plan Changes

Arc01 plan changes were tracked in `arc-plan.md`:

- v1.0: Arc01 opened with parser audit first.
- v1.1: The C++ Core Guidelines knowledge pack became the mandatory audit
  substrate.
- v1.2: Slice01 parser audit was marked CDC-verified.
- v1.3: Parser report was promoted to the durable arc-local report path, and
  grounder/engine audit open sets were created.
- v1.4: Arc02 library research was inserted between audit synthesis and the
  managed-process design.
- v1.5: Slice02 grounder audit was marked CDC-verified.
- v1.6: Slice03 engine audit was marked CDC-verified.
- v1.7: Slice04 audit synthesis was opened with architecture-option and
  supported-surface criteria.
- v1.8: Slice04 audit synthesis was marked CDC-verified and Arc01 became ready
  for arc-level close.
- v1.9: Arc01 closed.

No untracked Arc01 findings remain. Deferred work is explicit Arc02, Arc03,
Arc04, Arc05, Arc06, or later optional-dependency work, not hidden Arc01 scope.

## Bubble-up to Project

Arc01 delivered project ledger row P1's research input: complete parser,
grounder, and engine audit reports plus a synthesis report. The synthesis now
joins the already-closed Arc02 dependency portfolio as an accepted input to
Arc03.

Arc03 should start from two verified documents:

- [`audit-synthesis-pandapi.md`](audit-synthesis-pandapi.md)
- [`../arc02-cpp-library-research/combined-library-recommendations.md`](../arc02-cpp-library-research/combined-library-recommendations.md)

The first Arc03 design decision should be supported-surface classification:
normal parser path, normal grounder path, normal engine search path, parser
verifier/output helpers, H2, cpddl/FAM, interactive mode, translation, SAT,
BDD, and CUDD. Only after that should Arc03 lock the status taxonomy,
stdout/stderr/event ownership, binary naming/compatibility policy, timeout/
signal behavior, and dependency exposure boundaries.

Project-plan change required: yes, and applied in this close. `project-plan.md`
moves Arc01 from active to closed and marks Arc03 as ready for detailed
planning.

## What Worked

- Using one C++ Core Guidelines substrate plus the chengdu managed-process
  rubric made the three reports comparable.
- The CDC reports preserved both static source citation checks and runtime
  process probes, which gave the synthesis a stronger evidence base than prose
  alone.
- Keeping diagnosis separate from repair protected the arc from drifting into
  source cleanup before the architecture and dependency decisions were ready.
