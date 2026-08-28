# Closing Report: managed-process-contract-synthesis

Date: 2026-08-09

Branch: `release/0.3.x`

## Result

Slice 06 is CC-complete. It produced the final Arc03 managed-process contract,
closed all 17 ledger rows, preserved the design-only boundary, and left
`cdc-verification.md` for CDC.

The main artifact is:

`docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`

## Delivered Scope

The contract defines:

- normative versus informative material;
- supported, legacy, experimental, unsupported, and future surfaces;
- canonical `pandapi-*` names and inherited `pandaPI*` compatibility names;
- CLI, human, supervised, and machine-status modes;
- status names, numeric exit codes, payload classes, dependency failures,
  child-process failures, timeout/resource/interruption, internal errors, and
  signal termination;
- stdout/stderr ownership, tagged text `PANDAPI_STATUS`, buffering/flushing,
  and partial-output policy;
- help, usage, examples, version, provenance, parse errors, color, TTY, quiet,
  verbose, and supervised behavior;
- contract-test and golden-output obligations;
- Arc02 dependency gates;
- Arc04 shared-runtime substrate inputs;
- Arc05 per-binary adoption inputs;
- Arc06 release-hardening inputs;
- explicit deferrals and re-entry conditions;
- Arc03 close readiness and remaining caveat.

## Ledger Walk

The opening ledger contained 17 rows. All 17 are closed in
`slice06-managed-process-contract-synthesis/ledger.md`.

| Row | Status | CC evidence |
|-----|--------|-------------|
| F-1 | closed | The contract exists and names Arc01, Arc02, slice01, slice02, slice03, slice04, slice05, `audit-synthesis-pandapi`, `combined-library-recommendations`, `supported-surface-classification`, `status-exit-signal-taxonomy`, `stdio-event-tty-contract`, `cli-naming-version-migration`, and `contract-test-matrix`. |
| F-2 | closed | The contract distinguishes normative requirements from informative evidence, rationale, examples, implementation notes, and later-arc guidance using `MUST`, `SHOULD`, and `MAY`. |
| F-3 | closed | The contract preserves the supported-surface boundary and classifies parser normal HDDL parse, parser verifier/output helpers, grounder normal `.htn`, H2, cpddl/FAM, engine normal search, interactive, translation, SAT, and BDD/CUDD. |
| F-4 | closed | The contract defines canonical `pandapi-*` names, inherited `pandaPI*` compatibility names, release asset/install-shape compatibility, wolong preservation, and future breaking-removal requirements. |
| F-5 | closed | The contract defines human CLI, supervised/process-manager, and machine-status modes plus `--supervised`, `--status`, `--status=stderr`, `--status=stdout`, stdout conflicts, quiet, verbose, progress, statistics, diagnostics, and machine behavior. |
| F-6 | closed | The contract defines the full status vocabulary and numeric exit-code table, including all slice02 statuses and `128 + signal_number`. |
| F-7 | closed | The contract defines stdout/stderr ownership, final tagged text `PANDAPI_STATUS`, required fields, buffering/flushing, partial-output policy, and the non-use of JSON Lines / `nlohmann/json` hold. |
| F-8 | closed | The contract defines help, usage, examples, version, provenance, parse-error behavior, `cli_usage_error`/exit `10`, no input/model processing, and stable provenance fields. |
| F-9 | closed | The contract defines color, no-color/no-colour, `NO_COLOR`, TTY/no-TTY, ANSI restrictions, supervised/machine output restrictions, and legacy interactive TTY fencing. |
| F-10 | closed | The contract incorporates the contract-test matrix, including fixture schema, golden-output policy, normalization, positive and negative probes, timeout/resource/signal/color/non-supported probes, and black-box versus Catch2 split. |
| F-11 | closed | The contract preserves Arc02 gates for standard-library modernization, fmt, CLI11, Catch2, `tl::expected`, reproc++, `nlohmann/json`, Abseil, Boost.Process, and held optional dependencies. |
| F-12 | closed | The contract routes Arc04 shared-runtime substrate inputs: status/result, diagnostics/process I/O, CLI parser wrapper, status writer/parser, TTY/color policy, output finalization, provenance, fixture harness, and seam tests. |
| F-13 | closed | The contract routes Arc05 per-binary adoption inputs for parser, grounder, and engine, including command names, compatibility names, positive paths, negative statuses, stream ownership, status emission, color/TTY, and non-supported surface fencing. |
| F-14 | closed | The contract routes Arc06 release-hardening inputs, including behavior-change table, README/release notes, wolong verification, asset/manifest/checksum/provenance, license/NOTICE/THIRD-PARTY-LICENSES, CI evidence, and test-only dependency exclusion. |
| F-15 | closed | The contract records deferrals and re-entry conditions for legacy, experimental, unsupported, future, optional dependency, JSON/event, SAT, BDD/CUDD, H2/cpddl, translation, and interactive surfaces. |
| F-16 | closed | The contract identifies Arc03 close readiness and the remaining caveat that implementation, release packaging, and wolong runtime behavior are routed to Arc04-Arc06. |
| F-17 | closed | The protected-path check returned no paths, confirming no planner source, dependency source, scripts, workflows, release assets, build outputs, or vendored dependency files changed. |

## Verification Evidence

Row count:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/ledger.md
```

Result: `17`.

Open-row check:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/ledger.md
```

Result: no output.

Protected-path check:

```bash
git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Whitespace check:

```bash
git diff --check
```

Result: no output.

## Silent-Drop Diff

Scope requested and delivered:

- final normative `managed-process-contract.md`;
- incorporation of Arc01, Arc02, and Arc03 slice01-05 accepted inputs;
- surface, status, stream, CLI, color/TTY, provenance, dependency, test, and
  later-arc routing contract;
- explicit deferrals and re-entry conditions;
- Arc03 close-readiness statement.

Scope intentionally not delivered:

- planner source, dependency source, scripts, workflows, release assets, build
  outputs, `vendor.env`, and `pins.env`: out of scope for Arc03 design-only
  work;
- tests, harnesses, wrappers, aliases, shared libraries, status writers, CLI
  parsers, diagnostics facades, output finalizers, signal handlers, or
  per-binary migrations: routed to Arc04 and Arc05;
- dependency adoption or build-policy changes: routed to later implementation
  planning under Arc02 gates;
- Arc04/Arc05/Arc06 detailed plans: explicitly out of scope for slice06;
- Arc03 arc closing report: left for CDC/operator after slice06 verification.

No requested in-scope ledger row remains open.

## Bubble-Up To Arc03

Slice06 delivers the final Arc03 design artifact: the normative
`managed-process-contract.md`.

Arc03 is ready for arc-level CDC close after this slice is independently
verified. The remaining caveat is not a blocker: Arc03 is design-only, so
implementation conformance, release packaging, and wolong runtime proof remain
owned by Arc04, Arc05, and Arc06.

No Arc03 slice breakdown change is required before arc-level close. The arc
closing report should verify that all six planned slices are CDC-verified and
that the final contract satisfies arc ledger rows A1-A8 without protected-path
changes.

## CDC Handoff

CDC should verify the row count, reproduce all 17 ledger Verify commands,
inspect that only the contract, ledger, and this closing report changed, and
decide at arc level whether Arc03 can close or whether a blocking caveat
remains.
