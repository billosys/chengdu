# Arc03 Closing Report - managed-process-contract

Closer: CDC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Close commit: this report's adding commit
Gate review: slice06 CDC verification accepted the final contract and reported
Arc03 ready for arc-level close.

## Capability Verdict

Delivered. Arc03 promised:

> Produce the accepted design for CLI + supervised-process behavior: command
> naming, exit/status taxonomy, stdout/stderr/events, buffering, ANSI/TTY,
> signals/resources, version/provenance, and migration policy.

The six planned slices compose into that capability. Slice01 set the supported
surface boundary. Slice02 defined status, exits, signals, and failure classes.
Slice03 defined stream, event, buffering, and TTY/color behavior. Slice04
defined command names, inherited-name compatibility, help/version/provenance,
and migration policy. Slice05 translated the contract into executable test
obligations. Slice06 synthesized the final normative managed-process contract
and routed implementation, adoption, release, and wolong proof to Arc04, Arc05,
and Arc06.

This was design/documentation only. No planner source, dependency source,
scripts, workflows, release assets, build outputs, or vendored dependency files
were changed by the arc close.

## Accepted Contract

The normative Arc03 output is:

[`managed-process-contract.md`](managed-process-contract.md)

The accepted 0.3.0 contract is intentionally narrow:

- canonical commands are `pandapi-parser`, `pandapi-grounder`, and
  `pandapi-engine`;
- inherited `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` remain
  executable compatibility entry points in 0.3.0;
- supported surfaces are parser normal HDDL parse, grounder normal `.htn`
  grounding, and engine normal search;
- non-supported inherited surfaces are fenced with explicit legacy,
  experimental, unsupported, or future classifications and re-entry
  conditions;
- process managers classify outcomes from documented exit codes and optional
  final `PANDAPI_STATUS` tagged-text records, not human diagnostic prose;
- stdout is owned by one role at a time: data artifact, informational output,
  legal tagged status, or empty;
- human CLI behavior remains first-class, while `--supervised` selects quiet,
  deterministic, no-ANSI process-manager behavior;
- Arc02 dependency decisions are gates, not hidden product commitments.

## Slice Walk

The arc plan lists six slices. All six are closed and CDC-verified:

| Slice | Outcome | Evidence |
|-------|---------|----------|
| slice01 `supported-surface-classification` | delivered | [`slice01-supported-surface-classification/cdc-verification.md`](slice01-supported-surface-classification/cdc-verification.md) accepted the narrow supported surface at `7a0541d7`. |
| slice02 `status-exit-signal-taxonomy` | delivered | [`slice02-status-exit-signal-taxonomy/cdc-verification.md`](slice02-status-exit-signal-taxonomy/cdc-verification.md) accepted the shared status, exit, signal, and dependency-failure taxonomy. |
| slice03 `stdio-event-tty-contract` | delivered | [`slice03-stdio-event-tty-contract/cdc-verification.md`](slice03-stdio-event-tty-contract/cdc-verification.md) accepted stream ownership, tagged-text status output, flushing, partial-output, color, and TTY policy after CC's corrective close-set commit. |
| slice04 `cli-naming-version-migration` | delivered | [`slice04-cli-naming-version-migration/cdc-verification.md`](slice04-cli-naming-version-migration/cdc-verification.md) accepted canonical `pandapi-*` names, inherited-name compatibility, supervised/status options, help/version/provenance, parse-error behavior, and wolong migration policy. |
| slice05 `contract-test-matrix` | delivered | [`slice05-contract-test-matrix/cdc-verification.md`](slice05-contract-test-matrix/cdc-verification.md) accepted the black-box process fixture and Catch2 seam-test matrix at `e4a277f8`, with CDC verification committed at `18b7a42e`. |
| slice06 `managed-process-contract-synthesis` | delivered | [`slice06-managed-process-contract-synthesis/cdc-verification.md`](slice06-managed-process-contract-synthesis/cdc-verification.md) accepted the final normative managed-process contract at `e87d4881`, with CDC verification committed at `370a89f6`. |

Slice count matches the arc-plan breakdown: 6 planned, 6 closed, 0 missing.

## Composition Check

The slices compose in the intended order:

1. The supported-surface boundary prevents inherited optional modes from
   silently defining the 0.3.0 product.
2. The status taxonomy gives every later stream, CLI, and test decision stable
   status names and exit codes.
3. The stdio/event/TTY contract makes those statuses observable without
   stdout/stderr ambiguity or ANSI leakage.
4. The naming and migration contract introduces namespaced `pandapi-*`
   commands while preserving inherited `pandaPI*` compatibility for 0.3.0.
5. The test matrix turns the design into black-box process fixtures and seam
   tests that Arc04 and Arc05 must implement before claiming conformance.
6. The synthesis resolves the accepted slice reports into one normative
   contract and routes later work without silent drops.

No arc-level contradiction remains. The main caveat is explicit and accepted:
Arc03 is design-only. It does not prove implementation conformance, release
packaging, or wolong runtime behavior. Those are the purpose of Arc04, Arc05,
and Arc06.

Arc-level protected-path check:

```bash
git diff --name-only 7da0e95a^..HEAD -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output. Arc03 has not changed planner source, dependency source,
scripts, workflows, README, release manifest, `vendor.env`, `pins.env`, `dist`,
or `build` paths.

## Arc Ledger Walk

Rows verified: 8. Done: 8. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| A1 | Done, reproduced. All six planned Arc03 slices are closed and CDC-verified, with no missing slice from the breakdown. |
| A2 | Done, reproduced. The final contract classifies parser normal HDDL parse, parser verifier/output helpers, grounder normal `.htn`, H2, `cpddl`/FAM, engine normal search, interactive mode, translation mode, SAT, and BDD/CUDD. |
| A3 | Done, reproduced. The final contract defines the status vocabulary, exit codes, payload classes, timeout/resource/signal semantics, dependency failure, child-process failure, and internal-error behavior without requiring supervisors to scrape prose. |
| A4 | Done, reproduced. The final contract defines stdout/stderr ownership, tagged-text `PANDAPI_STATUS`, buffering/flushing, partial-output policy, quiet/progress behavior, ANSI/color/TTY policy, and supervised/machine-mode behavior. |
| A5 | Done, reproduced. The final contract defines `pandapi-*` binary names, inherited `pandaPI*` compatibility, help/version/provenance output, CLI parse-error behavior, release asset shape, and wolong migration obligations. |
| A6 | Done, reproduced. The contract test matrix covers positive and negative probes for parser, grounder, and engine, including help/version/provenance, missing args, unavailable input/output, malformed inputs, valid file/pipe behavior, `domain_no_plan`, timeout/resource/signal, color, and non-supported surfaces. |
| A7 | Done, reproduced. Arc04, Arc05, and Arc06 can be planned from the final contract: every accepted dependency, pilot, held surface, rejected dependency, compatibility decision, behavior-change obligation, license/NOTICE obligation, and release proof is routed. |
| A8 | Done, reproduced. Arc03 remained design-only; protected-path checks across the arc and close produced no planner source, dependency source, scripts, workflows, release asset, vendored dependency, `dist`, or `build` changes. |

## Accumulated Plan Changes

Arc03 plan changes were tracked in `arc-plan.md`:

- v1.0: Arc03 opened after Arc01 and Arc02 closed.
- v1.1: Slice01 CDC verification accepted the narrow supported-surface
  boundary.
- v1.2: Arc02 dependency findings were made explicit across the remaining
  Arc03 breakdown.
- v1.3: Slice02 CDC verification accepted the status/exit/signal taxonomy.
- v1.4: Slice03 opened from the accepted surface and status contracts.
- v1.5: Slice03 CDC verification accepted stdio/event/TTY behavior.
- v1.6: Slice04 opened for command naming, version, provenance, and migration.
- v1.7: Slice04 CDC verification accepted the naming and migration contract.
- v1.8: Slice05 opened for the contract-test matrix.
- v1.9: Slice05 CDC verification accepted the test matrix.
- v1.10: Slice06 opened for final synthesis.
- v1.11: Slice06 CDC verification accepted the final contract.
- v1.12: Arc03 close verified all six slices and closed the arc.

No untracked Arc03 findings remain. Deferred implementation, release, wolong,
optional-surface, and dependency work is explicit Arc04, Arc05, Arc06, or later
re-entry material, not hidden Arc03 scope.

## Bubble-up to Project

Arc03 delivered project ledger row P3: an accepted managed-process design
covering CLI ergonomics, supervised-process behavior, binary naming,
exit/status semantics, stdout/stderr/event output, buffering, ANSI/TTY,
signals/resources, version/provenance, migration policy, and explicit
incorporation or deferral of Arc02 dependency findings.

Project-plan change required: yes, and applied in this close. `project-plan.md`
moves Arc03 from active to closed and keeps Arc04-Arc06 as roadmap-only until
the operator opens the next detailed arc.

Arc04 should be planned from the final managed-process contract, not from
earlier design notes. In particular, Arc04 owns the shared runtime substrate,
status/result facade, diagnostics/process I/O facade, CLI parser wrapper,
status writer/parser, TTY/color policy helper, output finalization,
provenance/version assembly, fixture harness, Catch2 seam-test substrate, and
the dependency facades/pilots permitted by Arc02 and Arc03.

## What Worked

- Keeping supported-surface classification first prevented optional inherited
  modes from expanding the release.
- Arc02's dependency portfolio stayed visible all the way through the final
  contract instead of becoming side research.
- The final synthesis gives Arc04-Arc06 a concrete contract handoff while
  preserving the design-only boundary.
