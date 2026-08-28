# CDC Verification: engine-audit

Date: 2026-08-09

Branch: `release/0.3.x`

CC commit:

- `6bb2a3fbaeb6c452881321cf49629350fa24d28e` - `Add pandaPI engine audit report`

Verdict: **accepted**. Slice03 is CDC-verified.

## Row Count

Opening ledger row count at `0c22e86c`: 14.

Closing ledger row count at `6bb2a3fb`: 14.

No ledger rows were dropped.

## Commit and Scope Check

`6bb2a3fb` changes only:

- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/slice03-engine-audit/closing-report.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/slice03-engine-audit/ledger.md`

The commit includes the required assistant co-author trailers.

Protected-path check:

```bash
git diff --name-only 6bb2a3fb^..6bb2a3fbaeb6c452881321cf49629350fa24d28e -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env
```

Result: no output. No engine source, scripts, workflows, README, release
manifest, `vendor.env`, or `pins.env` changed in the close commit.

Additional format check:

```bash
git diff --check 6bb2a3fb^..6bb2a3fbaeb6c452881321cf49629350fa24d28e
```

Result: no output.

## Ledger Verification

CDC reran representative ledger checks and independently inspected every ledger
row's evidence claim. All rows reproduce.

| Row | CDC disposition | Evidence |
|-----|-----------------|----------|
| F-1 | accepted | Report exists and identifies project root, branch, date, audited paths, audited source state, C++ knowledge-pack guides, Core Guidelines references, and `11-anti-patterns`. |
| F-2 | accepted | Report inventories `pandaPI/pandaPIengine/src`, `options.ggo`, `CMakeLists.txt`, `problemSolver.sh`, README, build wrappers, smoke test, and fixture posture. |
| F-3 | accepted | Report covers correctness/runtime safety, error handling, stdio/TTY/ANSI behavior, CLI/API, resource/signal/lifecycle behavior, build/tests, maintainability/shared-code, and modernization. |
| F-4 | accepted | Findings E-001 through E-016 use severity labels and exact `pandaPI/pandaPIengine/...:<line>` locations. |
| F-5 | accepted | Report covers deep `exit()` calls, status propagation, solved/unsolvable behavior, timeout behavior, input failure, and internal-error classification gaps. |
| F-6 | accepted | Report covers stdout/stderr mixing, plan-output contamination, ANSI/color policy, missing TTY policy, and pipe-supervision impact. |
| F-7 | accepted | Report covers `options.ggo`, inherited `pandaPIengine` package/version identity, help/version behavior, current binary naming, and future `pandapi-engine` migration implications. |
| F-8 | accepted | Report covers timeout polling, wrapper `alarm` / `kill 0`, SAT signal/thread behavior, raw search ownership, and translation temp/child-process behavior. |
| F-9 | accepted | Report covers CMake, gengetopt-generated files, warning policy, local/CI verification gaps, fixture coverage, and optional CUDD/BDD build posture. |
| F-10 | accepted | Report identifies shared process-status, stream-routing, color/TTY, version/provenance, RAII file/temp, and contract-test candidates. |
| F-11 | accepted | Report lists eight engine-scope clean checks. |
| F-12 | accepted | Report lists parser, grounder, and cross-codebase synthesis inputs for slice04. |
| F-13 | accepted | Source-quality findings map to C++ Core Guidelines rule IDs where applicable; process-contract findings are labeled as Chengdu-specific managed-process findings where no direct Core Guidelines rule applies. |
| F-14 | accepted | Protected-path diff check produced no output. |

Supporting count checks:

| Check | Result |
|-------|--------|
| `rg -c '^\\| F-' docs/design-v0.3.0/arc01-vendored-source-audit/slice03-engine-audit/ledger.md` | `14` |
| `rg -n '\\| open \\|' docs/design-v0.3.0/arc01-vendored-source-audit/slice03-engine-audit/ledger.md` | no output |
| `rg -c '^#### E-' docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md` | `16` |
| `rg -c '^\\d+\\. I did not find' docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md` | `8` |

## Build and Probe Reproduction

CDC reran:

```bash
./scripts/build-engine.sh
```

Result: exit 0, producing `dist/macos-arm64/pandaPIengine`. The warning profile
reproduced the report's important classes, including repeated
`-Wmismatched-tags`, unused fields/variables, missing `override`, integer to
pointer cast, duplicate link libraries, and unused `-lpthread` linker input
warnings.

CDC then reran:

```bash
./scripts/smoke-test.sh --negative
```

Result: exit 0. The negative smoke suite reported 4 passed, 0 failed, including
the engine unsolvable case: engine exit 2 with `Status: Proven unsolvable`.

## Source Sampling

CDC sampled cited locations from the report:

- `Model.cpp:1548-1568` reads model counts into raw arrays and relies on
  `assert()` for index validation.
- `SearchEngine.cpp:89-93` contains a non-`main` option parser path that prints
  to stdout and calls `exit(1)`.
- `SearchEngine.cpp:181-206` prints seed, time-limit, and input-file status to
  stdout before input-open failure can be reported on stderr.
- `Util.cpp:15-23` unconditionally wraps text in ANSI SGR escape codes.
- `options.ggo:1-4` preserves inherited `pandaPIengine` package/version/usage.
- `problemSolver.sh:12-19` and `problemSolver.sh:57` implement timeout via
  `alarm` and `kill 0`.

The sampled citations support the report's severities and recommendations.

## Bubble-Up Check

Delivered-as-assigned: yes. Slice03 delivered the engine audit assigned by
`arc-plan.md`: a diagnosis-only report with exact source citations, C++ Core
Guidelines rule mapping where applicable, managed-process findings, clean
checks, and synthesis inputs.

Silent-drop diff: complete. No planned scope was dropped. The audit did not
attempt source fixes, as required by the diagnosis-only boundary.

Arc-plan impact: no slice breakdown change is required. Arc01 now has all three
per-component audit reports closed and CDC-verified. Slice04 audit synthesis is
the remaining Arc01 work and should compare the parser, grounder, and engine
reports into prioritized cleanup, shared-substrate, dependency, and
managed-process design recommendations. Arc02 can now proceed from the accepted
engine audit where needed.

## What Worked

- The parser and grounder audit shape made engine row verification fast and
  comparable.
- Rebuilding the engine before CDC gave fresh warning evidence without changing
  tracked source.
- Sampling both static source citations and runtime smoke behavior kept the
  verification grounded in the two surfaces 0.3.0 must reconcile: source
  maintainability and managed-process behavior.
