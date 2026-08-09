# CC assignment - chengdu arc03 / slice05 - contract-test-matrix

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc03-managed-process-contract/arc-plan.md`
4. `docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/slice-doc.md`
5. `docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/ledger.md`
6. Accepted Arc03 slice01 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/cdc-verification.md`
7. Accepted Arc03 slice02 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/cdc-verification.md`
8. Accepted Arc03 slice03 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/cdc-verification.md`
9. Accepted Arc03 slice04 artifacts:
   - `docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`
   - `docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/cdc-verification.md`
10. Arc01 close and synthesis:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/closing-report.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
11. Arc02 close and combined recommendations:
   - `docs/design-v0.3.0/arc02-cpp-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`
12. Current release/packaging docs as read-only evidence if needed:
   - `README.md`
   - `release-manifest.txt`
   - scripts under `scripts/`

## The work

Produce:

`docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`

This is the fifth Arc03 design artifact. It must translate the accepted
managed-process design into concrete golden process fixtures and acceptance
probes for later implementation.

The matrix must cover:

- the process fixture/probe schema;
- golden-output comparison and normalization policy;
- canonical `pandapi-*` and inherited `pandaPI*` command names;
- `--help`, `--version`, and `--provenance`;
- CLI usage errors and option conflicts;
- supported parser, grounder, and engine positive paths;
- input unavailable, output unavailable, and invalid input failures;
- engine solved and `domain_no_plan` behavior;
- timeout, resource, interrupt, and signal termination behavior;
- stdout/stderr ownership and final `PANDAPI_STATUS` records;
- color, TTY, no-color/no-colour, and `NO_COLOR` behavior;
- visible legacy, experimental, unsupported, and future surfaces;
- split between black-box process fixtures and Catch2 seam tests;
- Arc04, Arc05, Arc06, and slice06 consumers.

## Decisions you must make

Decide the fixture schema. A future harness should be able to represent:
command, argv, environment, stdin, TTY/no-TTY mode, timeout/resource policy,
expected exit code, expected signal or wait status, expected stdout role,
expected stderr role, expected artifacts, expected final `PANDAPI_STATUS`
fields, and normalization rules.

Decide what is golden and what is semantic. Stable machine/status fields,
canonical usage shape, and required provenance fields may be golden. Human
diagnostic prose should usually be checked by semantic predicates rather than
exact text unless the report intentionally freezes a narrow phrase.

Decide the minimal positive fixture set for parser, grounder, and engine,
including file-backed and stdout/pipe-backed artifacts and at least one
parser-to-grounder-to-engine composition path.

Decide the minimal negative fixture set for CLI usage, missing/unreadable
inputs, unavailable outputs, malformed inputs, no-plan outcomes, timeout,
resource, interrupt/signal, color/TTY leakage, and non-supported surfaces.

Decide where each test belongs:

- black-box process fixtures;
- Catch2 seam tests;
- Arc04 shared substrate tests;
- Arc05 per-binary adoption gates;
- Arc06 release/CI/wolong verification.

Decide which checks gate dependency adoption:

- CLI11 must wait for golden-output/process tests;
- fmt must stay behind diagnostics/process facade tests;
- Catch2 remains test-only;
- `tl::expected` remains behind status/result substrate tests;
- reproc++ remains conditional and requires an accepted child-process boundary;
- nlohmann/json remains held because Arc03 selected tagged text status-only
  output;
- Abseil and Boost.Process remain rejected.

## Constraints

- Do not edit planner source, dependency source, scripts, workflows, release
  assets, build outputs, `vendor.env`, or `pins.env`.
- Do not create fixture data files or implement the test harness.
- Do not adopt dependencies or change build policy.
- Do not change status names, numeric exit codes, tagged status field schema,
  stdout/stderr ownership, command names, option spellings, supported surfaces,
  release asset shape, or wolong migration policy.
- Do not write the final `managed-process-contract.md`; slice06 owns synthesis.
- Do not make legacy, experimental, unsupported, or future surfaces look
  supported by giving them ordinary positive fixture treatment.
- Keep timeout, resource, and signal probes safe for CI by specifying bounded
  semantics, cleanup requirements, and deferral conditions where needed.

## Suggested report shape

1. Executive summary.
2. Evidence base: Arc01, Arc02, slice01, slice02, slice03, and slice04.
3. Test philosophy and non-goals.
4. Process fixture/probe schema.
5. Golden-output and normalization rules.
6. Cross-binary command and informational probes.
7. CLI usage-error and conflict probes.
8. Parser normal-path fixture matrix.
9. Grounder normal-path fixture matrix.
10. Engine normal-path fixture matrix.
11. Status, stdout/stderr, and `PANDAPI_STATUS` assertion rules.
12. Timeout, resource, interrupt, and signal probes.
13. Color and TTY probes.
14. Legacy, experimental, unsupported, and future surface probes.
15. Black-box process fixtures versus Catch2 seam tests.
16. Arc02 dependency-gating implications.
17. Later slice and arc inputs.
18. Final recommendation.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc03, and leave `cdc-verification.md` for CDC.
