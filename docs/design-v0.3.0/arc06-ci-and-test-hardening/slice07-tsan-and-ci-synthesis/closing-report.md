# Arc06 Slice07 Closing Report: tsan-and-ci-synthesis

Status: CC proposed done
Implementation commit: this commit

## TSan Decision

TSan deferred. No TSan target was added.

Reason: the current supported workload does not exercise representative
multithreaded chengdu-owned code. `pandaPI/runtime/src/process_fixture.cpp`
observes subprocesses with a single-threaded `fork`/`pipe`/`waitpid` polling
loop and a `std::this_thread::sleep_for` delay. The shell contract harness
uses background timeout processes, but that is outside TSan's C++ shared-memory
race model. Parser, grounder, engine, and pipeline managed fixtures exercise
process behavior, supervised mode, status streams, and harness timeouts, but
not concurrent stream-draining or shared-state threading in chengdu-owned code.

The only clear in-tree thread primitives found by the Slice07 probe are in the
engine SAT path (`pandaPI/engine/src/sat/sat_planner.cpp`), and the managed
engine contract fences SAT as `unsupported_feature`. Adding a TSan build over
that path would promote an unsupported optional surface or produce symbolic
evidence over code the release does not support.

Owner: Arc06/CI hardening for the gate shape; future runtime or binary surface
slice for the representative workload.

Re-entry: add a Make-backed ThreadSanitizer target only after an accepted
supported workload includes real multithreaded chengdu-owned process
observation, a child-process adapter with concurrent stream draining, or
promoted supported planner concurrency. TSan must remain separate from
ASan/UBSan.

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | Workload inventory recorded here and in `fixture-gap-inventory.md`. Probe evidence: `rg -n -- "std::thread|pthread_create|pthread_mutex|fork\\(|pipe\\(|waitpid|sleep_for|run_process_fixture|run_capture_timeout|--supervised" pandaPI/runtime/src/process_fixture.cpp pandaPI/runtime/tests/fixture_substrate_smoke.cpp tests/contract/run pandaPI/engine/src/sat/sat_planner.cpp docs/design-v0.3.0/ci-notes.md` found runtime fork/pipe/waitpid process observation, shell timeout/supervised fixtures, and SAT-only pthread usage. |
| F-2 | done | TSan deferred above with concrete reason, owner, and Re-entry condition. Verify command for `TSan`, `ThreadSanitizer`, `deferred`, and `Re-entry` passed. |
| F-3 | done | No TSan target was added; `make help | rg -n "tsan|ThreadSanitizer|race"` produced no match, and the no-target decision is documented here. |
| F-4 | done | `fixture-gap-inventory.md` records runtime sanitizer as always-on Linux CI through `make ci-linux`, binary sanitizer as scheduled/pre-release/local, and Linux LSan re-entry. |
| F-5 | done | `fixture-gap-inventory.md` now maps `make coverage`, `make static-analysis-cpp`, `make warning-inventory`, `make test-binary-sanitize`, runtime sanitizer, Linux LSan, and TSan disposition. |
| F-6 | done | No workflow change was made. `make safety-checks && make actionlint` passed. |
| F-7 | done | `make static-analysis`, `make format-check`, `make test`, `make provenance-check`, `git diff --check`, and `git diff --cached --check` passed. `make check` was not run because Slice07 changed docs/planning synthesis only, did not change Make plumbing or release surfaces, and the prompt's narrower gates reproduce the required behavior and safety evidence without adding macOS provenance mutation. |
| F-8 | done | Arc07 handoff inputs are named below and in the Arc06 closing report; no Arc07 files were edited. |
| F-9 | done | Arc08 release-prep gates are named below and in the Arc06 closing report; no release package/publication surfaces were edited. |
| F-10 | done | Staged boundary check over README, release, `tools/release`, release workflow, Arc07, and Arc08 paths passed. |
| F-11 | done | Arc06 can close for CDC review. The arc-level closing report exists at `../closing-report.md` and records GO, no missing Arc06 row, and no next iteration. |

## Final Gate Map

Always-on CI:

- `make actionlint` through the lint job.
- `make ci-linux`, including `format-check`, `sanitize-runtime`, `test`, and
  `provenance-check`.
- `make ci-macos`, including `format-check`, `test`, macOS min-OS recording,
  and `provenance-check`.
- `make readme-verbatim` on Linux and macOS.

Scheduled or pre-release:

- `make coverage`
- `make static-analysis`
- `make warning-inventory`
- `make test-binary-sanitize`
- Linux binary sanitizer with LeakSanitizer enabled once the release-prep job
  runs on a LeakSanitizer-capable toolchain.

Local developer:

- Targeted `make test-contract-*`, `make smoke`, and `make smoke-negative`.
- `make test-runtime-sanitize` after runtime/process-policy changes.
- `make test-binary-sanitize` after adapter, inherited engine, sanitizer, or
  fixture-runner changes.

Deferred:

- TSan until a representative supported multithreaded workload exists.
- Adoption-seam coverage/static analysis until owned native file profiles and
  compile commands can be isolated from inherited/generated/dependency code.
- Packaged-release behavior, checksums, licenses/NOTICE, test-only dependency
  exclusion, wolong migration proof, and publication remain Arc08.

## Arc07 Handoff

Arc07 can start from these documentation/tutorial inputs:

- canonical commands are `pandapi-parser`, `pandapi-grounder`, and
  `pandapi-engine`;
- managed contract examples exist for parser, grounder, engine, pipeline,
  supervised positive paths, stdout artifact composition, final status, color
  policy, and optional-surface fencing;
- behavior examples should use stable machine fields and semantic predicates,
  not human diagnostic prose as final contract evidence;
- coverage/static-analysis/warning/sanitizer status is scoped and should not
  be presented as inherited planner algorithm cleanup;
- remaining public-doc gaps include behavior-change table, tutorial examples,
  architecture/dependency rationale, and migration prose.

## Arc08 Handoff

Arc08 receives these release-prep gates:

- rerun always-on CI aggregates for Linux and macOS;
- run pre-release `make coverage`, `make static-analysis`,
  `make warning-inventory`, and `make test-binary-sanitize`;
- add or run Linux LSan binary sanitizer proof with `detect_leaks=1`;
- verify release package asset shape, checksums, manifest/provenance,
  license/NOTICE obligations, test-only dependency exclusion, and wolong
  fetch/install/migration;
- do not treat Arc06 as package publication proof.

## Bubble-up to Arc06

Arc06 can close for CDC review. No Arc06 row is missing from the CC evidence
set. The only dynamic-analysis deferral is TSan, and it is a deliberate
workload-based deferral with a concrete Re-entry condition rather than a
missing gate.
