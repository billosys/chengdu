# Arc04 Closing Report - shared-runtime-substrate

Closer: CDC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Close commit: this report's adding commit
Gate review: slice06 CDC verification accepted the substrate integration
readiness handoff and reported Arc04 ready for arc-level close.

## Capability Verdict

Delivered. Arc04 promised:

> Introduce the shared C/C++ runtime/build substrate selected by the design,
> with tests and no behavior changes beyond wiring.

The six planned slices compose into that capability. Slice01 created the
chengdu-owned `pandapi-runtime/` skeleton. Slice02 implemented status and
result helpers. Slice03 implemented diagnostics and tagged status I/O. Slice04
implemented CLI policy, TTY/color policy, and provenance helpers. Slice05
implemented fixture, normalization, and process-observation substrate. Slice06
added the inert all-build probe and final readiness handoff.

This arc added shared runtime code and build/test probes, but did not migrate
parser, grounder, or engine command behavior. No `pandapi-*` entry points,
inherited `pandaPI*` compatibility behavior, release assets, package scripts,
workflow files, top-level README, wolong-facing release shape, or planner
source under `pandaPI/` changed as part of the arc close.

## Accepted Substrate

The normative Arc04 handoff is:

[`substrate-integration-readiness.md`](substrate-integration-readiness.md)

The accepted substrate is intentionally narrow:

- runtime root: `pandapi-runtime/`;
- namespace: `pandapi::runtime`;
- CMake target: `pandapi_runtime`, with alias `pandapi::runtime`;
- build probes: `scripts/build-runtime.sh` and inert
  `scripts/build-all.sh`;
- public helpers for status/result mapping, diagnostics/status I/O,
  stdout/stderr role vocabulary, CLI policy, TTY/color behavior, provenance,
  fixtures, normalization, and process observation;
- standard-library-only production runtime as of Arc04 close;
- optional local Catch2 seam-test support only, with default builds passing
  when Catch2 is absent.

Arc05 owns executable adoption. Arc06 owns release packaging, license/NOTICE,
test-only exclusion, CI, wolong, and publication proof.

## Slice Walk

The arc plan lists six slices. All six are closed and CDC-verified:

| Slice | Outcome | Evidence |
|-------|---------|----------|
| slice01 `runtime-build-skeleton` | delivered | [`slice01-runtime-build-skeleton/cdc-verification.md`](slice01-runtime-build-skeleton/cdc-verification.md) accepted the runtime root, namespace, static CMake target, smoke test, dependency gates, and `scripts/build-runtime.sh`. |
| slice02 `status-result-core` | delivered | [`slice02-status-result-core/cdc-verification.md`](slice02-status-result-core/cdc-verification.md) accepted `ProcessStatus`, Arc03 status/exit mappings, status classes, components, surface dispositions, output roles, dynamic signal exit mapping, and local `StatusResult<T>`. |
| slice03 `diagnostics-status-io` | delivered | [`slice03-diagnostics-status-io/cdc-verification.md`](slice03-diagnostics-status-io/cdc-verification.md) accepted the status I/O facade, tagged-text `PANDAPI_STATUS`, stdout/stderr legality, final flush, partial-output policy, and overflow-safe status parsing after iteration. |
| slice04 `cli-tty-provenance-core` | delivered | [`slice04-cli-tty-provenance-core/cdc-verification.md`](slice04-cli-tty-provenance-core/cdc-verification.md) accepted CLI option policy, command identity, TTY/color behavior, ANSI legality, required version/provenance fields, and compatibility-name handling after iteration. |
| slice05 `fixture-and-seam-test-substrate` | delivered | [`slice05-fixture-and-seam-test-substrate/cdc-verification.md`](slice05-fixture-and-seam-test-substrate/cdc-verification.md) accepted fixture records, normalization helpers, process observation, wait-status/signal mapping, safety policy vocabulary, structured comparison, final status matching, and optional local Catch2 integration. |
| slice06 `substrate-integration-readiness` | delivered | [`slice06-substrate-integration-readiness/cdc-verification.md`](slice06-substrate-integration-readiness/cdc-verification.md) accepted the final readiness report, `scripts/build-all.sh`, runtime README readiness guidance, Arc02 dependency-state map, Arc03 coverage map, Arc05 adoption inputs, and Arc06 release/wolong/license obligations. |

Slice count matches the arc-plan breakdown: 6 planned, 6 closed, 0 missing.

## Composition Check

The slices compose in the intended order:

1. The build skeleton gives chengdu a shared runtime home outside `pandaPI/`
   without changing inherited planner source.
2. The status/result core turns Arc03 status taxonomy into reusable runtime
   values before diagnostics, CLI, and fixture layers depend on it.
3. The diagnostics/status I/O layer makes status observable through a legal
   tagged-text stream contract without requiring JSON or prose scraping.
4. The CLI/TTY/provenance layer centralizes command identity, color legality,
   supervised-mode policy, and version/provenance field assembly before any
   binary adopts CLI11 or `pandapi-*` names.
5. The fixture/seam-test substrate gives Arc05 the black-box and seam-test
   vocabulary needed to prove managed-process conformance per binary.
6. The integration-readiness slice records the final no-binary-adoption state,
   dependency gates, build evidence, Arc05 adoption map, and Arc06 release
   obligations.

No arc-level contradiction remains. The main caveat is explicit and accepted:
Arc04 is substrate work. It does not prove executable managed-process
conformance, canonical command availability, release packaging, wolong
behavior, or publication readiness.

## Arc Ledger Walk

Rows verified: 8. Done: 8. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| A1 | Done, reproduced. All six planned Arc04 slices are closed and CDC-verified, with no missing slice from the breakdown. |
| A2 | Done, reproduced. `pandapi-runtime/` now provides a chengdu-owned runtime layout outside `pandaPI/`, with public headers, implementation sources, tests, CMake target, README, `scripts/build-runtime.sh`, and `scripts/build-all.sh`. |
| A3 | Done, reproduced. The status/result substrate implements the Arc03 status vocabulary, exit-code mapping, payload classes, and local result facade without importing or leaking `tl::expected` or other third-party APIs. |
| A4 | Done, reproduced. The diagnostics/status I/O substrate implements stdout/stderr role handling, tagged-text `PANDAPI_STATUS`, buffering/flushing support, partial-output vocabulary, and standard-library-only formatting behind the approved facade. |
| A5 | Done, reproduced. The CLI/TTY/provenance substrate implements option-policy helpers, CLI11 gates, no-color/no-colour/`NO_COLOR` policy, TTY detection, ANSI legality, and provenance field assembly without replacing per-binary command behavior in Arc04. |
| A6 | Done, reproduced. The fixture/seam-test substrate provides fixture records, normalization, process observation, timeout/resource/signal policy vocabulary, final-status matching, and optional local Catch2 integration, with Catch2 fenced from default runtime builds. |
| A7 | Done, reproduced. `./scripts/build-all.sh` and `./scripts/smoke-test.sh --negative` pass while parser, grounder, engine, release assets, package scripts, workflows, and wolong-facing behavior remain unchanged. |
| A8 | Done, reproduced. Arc05 and Arc06 planning inputs are routed in `substrate-integration-readiness.md`: per-binary adoption, dependency/license obligations, release packaging constraints, CI gates, wolong proof, and the Slice05 stdin/full-duplex caveat. |

## Verification

Arc-level verification:

```text
./scripts/build-all.sh
```

Result: passed. Runtime CTest passed 5/5, and parser, grounder, and engine
builds completed successfully with inherited vendored-source warnings only.

```text
./scripts/smoke-test.sh --negative
```

Result: passed, 4 passed and 0 failed.

Additional checks:

- all six Arc04 `cdc-verification.md` files exist;
- Arc04 arc-plan status lists all six slices as closed and CDC-verified;
- `git diff --check` passed;
- `git diff --cached --check` passed;
- protected source, workflow, release, build-output, package, and publish path
  checks returned no output.

## Dependency and Release Boundary

Arc04 kept Arc02 dependency decisions as gates, not accidental imports:

- standard-library modernization is the production baseline;
- fmt is still not imported;
- CLI11 is still not imported;
- Catch2 is optional/local/test-only and absent from the default runtime build;
- `tl::expected` is still not imported because `StatusResult<T>` remains local;
- reproc++ is still not imported;
- nlohmann/json, Abseil, Boost.Process, GSL, google/benchmark, SAT/BDD/CUDD,
  H2, and cpddl-internal expansion remain held or rejected according to Arc02
  and Arc03 routing.

No release packaging obligation is satisfied by Arc04 alone. Arc06 must verify
the final production dependency set, notices, source provenance, test-only
exclusion, tarballs, `SHA256SUMS`, `release-manifest.txt`, and wolong install
path after Arc05 adopts the runtime in executable binaries.

## Accumulated Plan Changes

Arc04 plan changes were tracked in `arc-plan.md`:

- v1.0: Arc04 opened after Arc03 closed.
- v1.1: Slice01 CDC verification accepted the runtime build skeleton.
- v1.2: Slice02 opened from the accepted runtime root.
- v1.3: Slice02 CDC verification accepted status/result core.
- v1.4: Slice03 opened from status/result core.
- v1.5: Slice03 CDC verification accepted diagnostics/status I/O after
  iteration.
- v1.6: Slice04 opened from diagnostics/status I/O.
- v1.7: Slice04 CDC verification accepted CLI/TTY/provenance core after
  iteration.
- v1.8: Slice05 opened from CLI/TTY/provenance core.
- v1.9: Slice05 CDC verification accepted fixture/seam-test substrate.
- v2.0: Slice06 opened for integration readiness.
- v2.1: Slice06 CDC verification accepted the final readiness handoff.
- v2.2: Arc04 close verified all six slices and closed the arc.

No untracked Arc04 findings remain. Deferred executable adoption, release,
wolong, dependency, optional-surface, and full-duplex process-observation work
is explicit Arc05, Arc06, or later re-entry material, not hidden Arc04 scope.

## Bubble-up to Project

Arc04 delivered project ledger row P4: a shared runtime/build substrate was
implemented, tested, kept within the design-approved surface, and left ready
for duplicate process-policy code to route through shared helpers during
Arc05 adoption. Arc02-selected dependencies entered only through approved
facades or gates; no held or rejected dependency entered production runtime.

Project-plan change required: yes, and applied in this close. `project-plan.md`
moves Arc04 from active to closed and makes Arc05 eligible for detailed
planning from the accepted Arc04 close when the operator opens it.

Arc05 should now be planned from:

- Arc03 final managed-process contract;
- Arc03 contract-test matrix;
- Arc04 accepted substrate;
- Arc04 substrate integration readiness handoff;
- all Arc04 CDC caveats, especially no binary adoption, no default
  inherited-name deprecation warning, dependency gates, and the Slice05
  stdin/full-duplex re-entry condition.

## What Worked

- Keeping the runtime outside `pandaPI/` preserved upstream source history
  while creating a downstream-owned process-policy substrate.
- CDC iterations on status parsing and CLI/provenance policy caught defects
  before binary adoption, when fixes are cheaper and quieter.
- The readiness handoff keeps Arc05 brave but bounded: it can make the big
  architecture move with concrete substrate, tests, and release obligations
  already visible.
