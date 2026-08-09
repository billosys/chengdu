# Closing Report: contract-test-matrix

Date: 2026-08-09

Branch: `release/0.3.x`

## Result

Slice 05 is CC-complete. It produced the Arc03 contract test matrix report,
closed all 16 ledger rows, preserved the design-only boundary, and left
`cdc-verification.md` for CDC.

The main artifact is:

`docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`

## Delivered Scope

The slice report defines:

- a reusable process fixture/probe schema;
- golden-output and normalization policy;
- canonical `pandapi-*` and inherited `pandaPI*` command-name probes;
- `--help`, `--version`, and `--provenance` probes;
- CLI usage-error and option-conflict probes;
- supported parser, grounder, and engine positive paths;
- input unavailable, output unavailable, and invalid input failures;
- engine solved and `domain_no_plan` outcomes;
- timeout, resource-limit, interrupt, and signal-termination probes;
- stdout/stderr ownership and final `PANDAPI_STATUS` assertions;
- color, TTY, no-color/no-colour, and `NO_COLOR` probes;
- visible legacy, experimental, unsupported, and future surface probes;
- the split between black-box process fixtures and Catch2 seam tests;
- Arc02 dependency gates and later Arc04/Arc05/Arc06/slice06 consumers.

## Ledger Walk

The opening ledger contained 16 rows. All 16 are closed in
`slice05-contract-test-matrix/ledger.md`.

| Row | Status | CC evidence |
|-----|--------|-------------|
| F-1 | closed | The report exists and names Arc01, Arc02, slice01, slice02, slice03, slice04, `audit-synthesis-pandapi`, `combined-library-recommendations`, `supported-surface-classification`, `status-exit-signal-taxonomy`, `stdio-event-tty-contract`, and `cli-naming-version-migration` as inputs. |
| F-2 | closed | The report defines a fixture schema covering command, argv, environment, stdin, stdout, stderr, artifacts, exit code, wait status, signal, timeout, resource, TTY, color, and final `PANDAPI_STATUS` observation. |
| F-3 | closed | The report defines golden-output comparison rules, stable fields, semantic predicates, path/duration/build metadata/timestamp/errno/signal normalization, and a prohibition on relying on non-normative diagnostic prose. |
| F-4 | closed | The report covers canonical `pandapi-*` and inherited `pandaPI*` command probes, compatibility behavior, argv[0], and no default deprecation-warning requirement. |
| F-5 | closed | The report covers `--help`, `--version`, and `--provenance`, stdout ownership, `ok`/exit 0, no input/model processing, status-stream interactions, and stable provenance fields. |
| F-6 | closed | The report covers missing required args, unknown options, malformed option values, too many positionals, `--output` conflicts, `--status=stdout` conflicts, `cli_usage_error`/exit 10, and no input/model processing. |
| F-7 | closed | The report covers parser, grounder, and engine positive file-backed and stdout/pipe-backed paths, stdout/stderr ownership, final status records, and pipeline composition. |
| F-8 | closed | The report covers missing/unreadable inputs, missing includes, unavailable outputs, write/flush/finalize failures, malformed HDDL, malformed `.htn`, malformed engine input, and status mapping to `input_unavailable`, `output_unavailable`, and `input_invalid`. |
| F-9 | closed | The report covers engine `domain_no_plan`, solved search, unsolvable/unreachable outcomes, and distinctions from malformed input, timeout, resource limit, dependency failure, and internal error. |
| F-10 | closed | The report covers timeout, resource limit, interrupt, signal termination, safe CI bounds, cleanup, partial_output_policy, final status where binary-owned, and supervisor-visible `128 + signal_number`. |
| F-11 | closed | The report covers `--color=auto`, `--color=always`, `--color=never`, `--no-color`, `--no-colour`, `NO_COLOR`, TTY/no-TTY, ANSI, stderr, stdout artifacts, tagged status, supervised and machine output, files, and pipes. |
| F-12 | closed | The report covers parser verifier/output helpers, H2, cpddl/FAM, interactive, translation, SAT, BDD/CUDD, visible/hidden options, and no expansion to supported 0.3.0 surfaces. |
| F-13 | closed | The report splits black-box process fixtures from Catch2 seam tests and routes Arc04 substrate tests, Arc05 per-binary adoption gates, and Arc06 release/CI/wolong verification. |
| F-14 | closed | The report preserves CLI11, fmt, `nlohmann/json`, Catch2, `tl::expected`, reproc++, Abseil, and Boost.Process dependency gates from Arc02. |
| F-15 | closed | The report identifies slice06, Arc04, Arc05, and Arc06 consumers and routes fixture obligations, probe obligations, deferred optional surfaces, compatibility decisions, dependency gates, and behavior-change/release obligations without silent drop. |
| F-16 | closed | The protected-path check returned no paths, confirming no planner source, dependency source, scripts, workflows, release assets, build outputs, or vendored dependency files changed. |

## Verification Evidence

Row count:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/ledger.md
```

Result: `16`.

Open-row check:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/ledger.md
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

- process fixture/probe schema;
- golden-output comparison and normalization policy;
- canonical and inherited command-name probes;
- informational and CLI usage-error probes;
- supported positive parser, grounder, and engine probes;
- input/output/invalid-input negative probes;
- engine solved and `domain_no_plan` probes;
- timeout/resource/interrupt/signal probes;
- stdout/stderr/status/color/TTY assertions;
- visible non-supported surface probes;
- black-box versus Catch2 ownership split;
- Arc02 dependency gates;
- slice06 and Arc04/Arc05/Arc06 routing.

Scope intentionally not delivered:

- planner source, dependency source, scripts, workflows, release assets, build
  outputs, `vendor.env`, and `pins.env`: out of scope for Arc03 design-only
  work;
- generated fixture data files: routed to Arc04/Arc05;
- process harness implementation: routed to Arc04;
- dependency adoption or build-system policy changes: out of scope;
- final `managed-process-contract.md`: routed to slice06;
- release documentation and wolong verification: routed to Arc06.

No requested in-scope ledger row remains open.

## Bubble-Up To Arc03

Slice05 delivers the Arc03 A6 input: the final contract test matrix for the
managed-process contract.

Arc03 can now proceed to slice06 with stable evidence for:

- process fixture shape and comparison policy;
- command, status, stream, color, signal, and non-supported-surface test
  obligations;
- the black-box/Catch2 split;
- preserved Arc02 dependency gates;
- later-arc routing for implementation, adoption, and release verification.

No Arc03 slice breakdown change is required before slice06. The existing
arc-plan already routes final synthesis to slice06, shared substrate and
harness work to Arc04, per-binary adoption to Arc05, and release/CI/wolong
verification to Arc06.

## CDC Handoff

CDC should verify the row count, reproduce all 16 ledger Verify commands,
inspect that only the report, ledger, and this closing report changed, and
confirm that no planner source, dependency source, scripts, workflows, release
assets, build outputs, or vendored dependency files changed.
