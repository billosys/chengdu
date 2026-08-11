# Arc05 Closing Report: binary-contract-adoption

Date: 2026-08-11

## Capability Verdict

Arc05 is closed.

The local 0.3.0 primary binary surface now conforms to the accepted
managed-process contract through canonical native `pandapi-*` commands:
`pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`. Inherited
`pandaPI*` command compatibility is not accepted 0.3.0 behavior, and the
parser/grounder transition shims and old-name sidecars have been retired by
their native cutover slices.

Arc05 does not close release publication, wolong migration proof, expanded CI
matrix hardening, or public tutorial/documentation. Those remain downstream.

## Slice Walk

- Slice01 closed and CDC-verified the quality tooling runway.
- Slice02 closed and CDC-verified the contract fixture scaffold.
- Slice03 closed and CDC-verified parser managed-contract adoption.
- Slice04 is CDC-verified by supersession: adapter-era inherited grounder
  compatibility was replaced by the no-compatibility policy and Slice07 native
  grounder cutover.
- Slice05 closed as a deferred discovery slice after proving parser/grounder
  managed behavior still depended on shims and raw sidecars.
- Slice06 closed and CDC-verified native `pandapi-parser` cutover.
- Slice07 closed and CDC-verified native `pandapi-grounder` cutover.
- Slice08 closed and CDC-verified native `pandapi-engine` adoption after a
  CDC corrective patch fenced the internal engine child-driver sentinel.
- Slice09 is satisfied by this arc-level closing report and has no separate
  implementation slice. Its planned synthesis content is represented here.

## Composition Evidence

CDC reproduced the current composed surface on `release/0.3.x`, macOS arm64:

- `make build-engine` passed.
- focused engine sentinel fixture passed: 10 passed, 0 failed.
- `make test-contract-parser-managed` passed: 285 passed, 0 failed.
- `make test-contract-grounder-managed` passed: 256 passed, 0 failed.
- `make test-contract-engine-managed` passed: 299 passed, 0 failed.
- `make smoke` passed: 3 passed, 0 failed.
- `make smoke-negative` passed: 4 passed, 0 failed.
- `make test` passed.
- `make provenance-check` passed.
- `make actionlint` passed.
- `git diff --check` passed.

No final release manifest, checksum set, publication workflow, or wolong
installation acceptance is claimed here.

## Arc Ledger Walk

- A1 closed: every planned Arc05 slice is either CDC-verified, superseded with
  explicit verification, deferred with a recorded stop condition, or satisfied
  by this arc-level synthesis.
- A2 closed: canonical parser, grounder, and engine commands are the accepted
  local primary surface.
- A3 closed: inherited parser and grounder shims/sidecars are removed from the
  accepted surface; inherited engine command compatibility was never accepted.
- A4 closed: normal parser, grounder, and engine success paths have executable
  contract fixtures.
- A5 closed: CLI usage, missing/unavailable input, invalid input, unavailable
  output, and domain no-plan behavior are classified through status records
  where reachable.
- A6 closed: stream ownership, artifact disposition, final status fields, and
  ANSI/color policy are fixture-checked for the primary binaries.
- A7 closed: optional and inherited surfaces are fenced as legacy,
  experimental, unsupported, or future, rather than promoted.
- A8 closed: active CI and local gates route through make targets.
- A9 closed: Arc05 did not accept a release package, release publication, or
  wolong installation proof.
- A10 closed: no new runtime dependency or default-path network fetch was
  accepted for binary contract adoption.
- A11 closed: downstream responsibilities are routed to Arc06, Arc07, and
  Arc08 below.

## Downstream Handoff

Arc06 should own expanded CI/test hardening: multi-platform proof, coverage,
heavier sanitizer/static-analysis gates, process-isolation and timeout
hardening, remaining fixture expansion, and any provenance race mitigation.

Arc07 should own public docs and tutorial work: behavior-change table, beginner
HTN/PDDL/HDDL material, canonical CLI examples, architecture/dependency notes,
and migration language that does not advertise inherited `pandaPI*`
compatibility.

Arc08 should own release readiness and publication: package dry-runs, release
assets, checksums, manifests, license/NOTICE verification, test-only dependency
exclusion evidence, wolong fetch/install/migration proof, and final publication
gates.

## Residual Risk

The native engine bridge still calls inherited engine code inside a forked
child. That is acceptable for Arc05 managed-contract behavior, but Arc06 should
decide whether deeper process supervision, timeout behavior, or isolation
needs a harder seam.
