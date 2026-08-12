# Arc06 Closing Report: ci-and-test-hardening

Status: CC proposed done
Verdict: GO for CDC review
Implementation commit: this commit

Arc06 can close for CDC review. No Arc06 row is missing from the CC evidence
set. This is not a release-publication claim: Arc08 still owns package assets,
checksums, manifests, license/NOTICE proof, test-only dependency exclusion,
wolong migration proof, and publication.

## Arc Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| A1 | done | Every planned Arc06 slice has a slice directory and closing evidence. Slices01-06 are CDC-verified; Slice07 is CC proposed done in `slice07-tsan-and-ci-synthesis/closing-report.md`. |
| A2 | done | `fixture-gap-inventory.md` inventories baseline and managed fixtures, maps Arc03 matrix coverage, and routes remaining gaps to no-op, deferred re-entry, Arc07, or Arc08. |
| A3 | done | Slice02 CDC verification records managed parser 298/0, grounder 269/0, engine 312/0, and pipeline 129/0 through canonical `pandapi-*` black-box process fixtures. |
| A4 | done | Slice02 CDC verification records positive `--supervised` fixtures for parser, grounder, engine, and pipeline, plus CI-safe fixture/smoke workloads. |
| A5 | done | Slice03 CDC verification records `make coverage` over owned `pandaPI/runtime` with 5/0 runtime CTest and 74.09% included line coverage; inherited/generated/third-party and adoption-seam limits are disclosed. |
| A6 | done | Slice04 CDC verification records Make-backed `make static-analysis-cpp` through `clang-tidy`/`clang-analyzer-*` over 14 runtime translation units, with adoption-seam deferral. |
| A7 | done | Slice05 CDC verification records `make warning-inventory`, 56 warning lines, primary parser/grounder/engine warning burndown where low-risk, and remaining warning budgets by ownership tier. |
| A8 | done | Slice06 CDC verification records `make test-runtime-sanitize` 5/0 and `make test-binary-sanitize` 1008/0 over isolated ASan/UBSan `pandapi-*` managed fixtures; macOS LSan is a Linux/toolchain Re-entry. |
| A9 | done | Slice07 defers TSan with concrete workload reason and Re-entry: add a Make-backed ThreadSanitizer target only after supported multithreaded chengdu-owned process observation, concurrent child-process adapter work, or promoted supported planner concurrency exists. |
| A10 | done | GitHub Actions and CI-equivalent targets invoke Make targets. Slice07 made no workflow changes, and `make safety-checks && make actionlint` passed. |
| A11 | done | Arc06 stayed out of release asset shape, wolong installation path, unsupported optional-surface support, and public tutorial/docs scope. Slice07 staged boundary check over README, release, release workflow, Arc07, and Arc08 paths passed. |
| A12 | done | `fixture-gap-inventory.md`, Slice07 closing, and this report route Arc07 documentation/tutorial inputs, Arc08 release-prep gates, coverage/static/sanitizer/warning dispositions, LSan, TSan, and remaining risks without silent drops. |

## Final CI And Local Gate Map

Always-on CI:

- `make actionlint`
- `make ci-linux`
- `make ci-macos`
- `make readme-verbatim`

Local CI-equivalent and pre-commit:

- `make safety-checks`
- `make static-analysis`
- `make format-check`
- `make test`
- `make provenance-check`

Scheduled or pre-release:

- `make coverage`
- `make warning-inventory`
- `make test-binary-sanitize`
- Linux binary sanitizer/LSan with `detect_leaks=1` once Arc08 or release-prep
  runs a LeakSanitizer-capable job.

Targeted local developer gates:

- `make test-runtime-sanitize` after runtime/process-policy changes.
- `make test-binary-sanitize` after adapter, inherited engine, sanitizer, or
  fixture-runner changes.
- Targeted `make test-contract-*` and smoke targets for behavior changes.

Deferred:

- TSan, until representative supported multithreaded process-observation or
  planner-concurrency workload exists.
- Adoption-seam coverage/static analysis, until managed-fixture profiles and
  compile commands isolate owned native files from inherited/generated and
  third-party code.

## Arc07 Documentation And Tutorial Handoff

Arc07 should write public docs/tutorial material from these inputs:

- canonical `pandapi-*` command names and managed-process behavior are the
  accepted 0.3.0 surface;
- fixture records provide examples for positive parse-ground-solve, stdout
  artifact handoff, `--supervised`, final `PANDAPI_STATUS`, ANSI/color policy,
  no-plan, input/output errors, and optional-surface fencing;
- examples should teach stable machine fields, status classes, stream
  ownership, artifact ownership, and semantic comparison policy;
- docs must not claim inherited planner algorithm coverage or warning cleanup
  beyond the scoped Arc06 evidence;
- behavior-change table and migration prose remain Arc07 work.

## Arc08 Release-Prep Handoff

Arc08 should verify before publication:

- Linux/macOS CI aggregates and README verbatim gates;
- pre-release coverage, static-analysis, warning-inventory, and binary
  sanitizer gates;
- Linux LSan with leak detection enabled;
- release package asset layout, executable bits, checksums, manifest and
  provenance;
- license/NOTICE obligations and test-only dependency exclusion;
- wolong fetch/install/migration for canonical `pandapi-*` binaries;
- publication workflow and release idempotency.

## Deferrals And Re-entry

- TSan deferred. Re-entry: accepted supported multithreaded chengdu-owned
  process observation, concurrent stream-draining child-process adapter, or
  supported planner concurrency.
- Linux LSan deferred from macOS evidence. Re-entry: run binary sanitizer on
  Linux or another LeakSanitizer-capable toolchain with `detect_leaks=1`.
- Adoption-seam coverage and static analysis deferred. Re-entry: isolate
  owned native files' profiles/compile commands from inherited/generated and
  nested third-party build volume.
- Output finalization and binary-owned timeout/resource status fixtures remain
  deferred until portable supported mechanisms exist.

## NO-GO Items

None for Arc06. The next iteration is not required unless CDC rejects a
Slice07 or Arc06 evidence row.
