# CC assignment - chengdu arc03 / slice06 - managed-process-contract-synthesis

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc03-managed-process-contract/arc-plan.md`
4. `docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/slice-doc.md`
5. `docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/ledger.md`
6. Accepted Arc03 reports and CDC verification:
   - `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/cdc-verification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/cdc-verification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/cdc-verification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/cdc-verification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/cdc-verification.md`
7. Arc01 close and synthesis:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/closing-report.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
8. Arc02 close and combined recommendations:
   - `docs/design-v0.3.0/arc02-cpp-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
9. Current release/packaging docs as read-only evidence if needed:
   - `README.md`
   - `release-manifest.txt`
   - scripts under `scripts/`

## The work

Produce:

`docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`

This is the final Arc03 design artifact. It must synthesize the accepted
Arc03 slices into one normative 0.3.0 managed-process contract and route the
implementation/test/release obligations to Arc04, Arc05, and Arc06.

The contract must cover:

- normative versus informative material;
- supported, legacy, experimental, unsupported, and future surfaces;
- canonical `pandapi-*` names and inherited `pandaPI*` compatibility names;
- CLI and supervised-process modes;
- status names, numeric exit codes, payload classes, timeout/resource/signal
  semantics, dependency failures, child-process failures, and internal errors;
- stdout/stderr ownership, tagged text `PANDAPI_STATUS`, buffering/flushing,
  and partial-output policy;
- help, usage, examples, version, provenance, parse errors, color, TTY, quiet,
  verbose, and supervised behavior;
- contract test matrix obligations and golden-output policy;
- Arc02 dependency gates;
- Arc04 shared-runtime substrate inputs;
- Arc05 per-binary adoption inputs;
- Arc06 release-hardening inputs;
- explicit deferrals and re-entry conditions;
- Arc03 close readiness and remaining caveats.

## Decisions you must make

Decide what the final contract treats as normative. Earlier reports are
accepted evidence, but `managed-process-contract.md` should become the contract
Arc04-Arc06 implement against.

Decide how to present the contract without contradiction. If two prior reports
appear to conflict, do not silently choose one. Name the conflict, choose the
accepted controlling source if one exists, and route any necessary amendment.

Decide the implementation handoff shape:

- Arc04 shared substrate: status/result, diagnostics/process I/O, CLI parser
  wrapper, status writer/parser, TTY/color policy, output finalization,
  provenance, test harness, and seam tests.
- Arc05 adoption: parser, grounder, and engine migrations to the shared
  contract and dual command names.
- Arc06 release hardening: docs, behavior-change table, release assets,
  wolong verification, license/NOTICE, CI, and test-only dependency exclusion.

Decide which deferrals remain after Arc03: legacy parser helpers, grounder
`cpddl`/FAM, engine interactive, experimental H2/translation, unsupported SAT,
future BDD/CUDD, JSON/event expansion, held dependencies, and optional
dependency audits.

## Constraints

- Do not edit planner source, dependency source, scripts, workflows, release
  assets, build outputs, `vendor.env`, or `pins.env`.
- Do not implement tests, harnesses, shared libraries, wrappers, aliases,
  status writers, CLI parsers, diagnostics facades, output finalizers, signal
  handlers, or per-binary migrations.
- Do not adopt dependencies or change build policy.
- Do not change accepted status names, exit codes, tagged status field schema,
  stdout/stderr ownership, command names, option spellings, supported surfaces,
  release asset shape, or wolong migration policy.
- Do not write Arc04/Arc05/Arc06 detailed plans.
- Do not write the Arc03 arc closing report; leave arc-level close for CDC
  after this slice is independently verified.

## Suggested report shape

1. Executive summary.
2. Normative status of this contract.
3. Evidence base and accepted inputs.
4. Supported surface and non-supported surface matrix.
5. Command names, compatibility, and release-shape policy.
6. Invocation modes and CLI option semantics.
7. Status vocabulary, exit codes, payload classes, and signal semantics.
8. stdout/stderr/status ownership, buffering, flushing, and partial outputs.
9. Help, version, provenance, parse errors, color, TTY, quiet, and verbosity.
10. Contract test and golden-output obligations.
11. Arc02 dependency gates.
12. Arc04 shared-runtime substrate handoff.
13. Arc05 per-binary adoption handoff.
14. Arc06 release-hardening handoff.
15. Deferrals and re-entry conditions.
16. Arc03 close readiness.
17. Final recommendation.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc03, and leave `cdc-verification.md` for CDC. The bubble-up
should say whether Arc03 is ready for arc-level CDC close or whether any
blocking caveat remains.
