# chengdu arc01 / slice03 - engine-audit - closing report

Closer: CC
Branch: `release/0.3.x`
Closed on: 2026-08-09
Source/audit HEAD: `0c22e86c`
Slice doc: [`slice-doc.md`](slice-doc.md)
Ledger: [`ledger.md`](ledger.md)
Report: [`../audit-results-pandapi-engine.md`](../audit-results-pandapi-engine.md)
Gate: proposed GO, pending CDC verification

## Slice Verdict

Delivered as assigned. The engine audit report exists at the planned arc-local
path and covers source/build/test inventory, correctness/runtime safety, error
handling, status/exit behavior, stdout/stderr/TTY/color behavior, CLI/API
surface, resource/signal behavior, build/test posture, maintainability,
modernization, synthesis inputs, and clean checks.

This was diagnosis only. No engine source, scripts, workflows, README,
release assets, or tracked build outputs were changed.

## Ledger Walk

Rows verified: 14. Done: 14. Deferred: 0. No-op: 0.

F-1: done. The report exists at
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
and §2 identifies project root, branch, date, audited paths, source/audit HEAD,
and C++ knowledge-pack guides including `11-anti-patterns`.

F-2: done. Report §2 inventories `pandaPI/pandaPIengine/src`,
`options.ggo`, `CMakeLists.txt`, `problemSolver.sh`, `README.md`,
`scripts/build-engine.sh`, `scripts/lib-platform.sh`, `scripts/smoke-test.sh`,
and root fixtures. Command evidence records no engine-local test/fixture
directories under the engine tree.

F-3: done. Report §3 contains findings grouped under correctness/runtime
safety, error handling, stdio/TTY/ANSI behavior, CLI/API surface,
resource/signal/lifecycle behavior, build/tests, maintainability/shared-code,
and modernization candidates.

F-4: done. Findings E-001 through E-016 each include Severity and Location
fields, and each cites exact `pandaPI/pandaPIengine/...:<line>` locations.

F-5: done. Findings E-002 and E-003 specifically audit deep `exit()` calls,
status propagation, solved/unsolvable, timeout, input failure, and internal
classification gaps.

F-6: done. Findings E-004 and E-005 specifically audit stdout/stderr mixing,
plan-output contamination, ANSI/color policy, missing TTY policy, and pipe
supervision implications. The missing-file probe observed status 1 with three
stdout lines before one stderr diagnostic.

F-7: done. Finding E-006 audits `options.ggo`, upstream `pandaPIengine` version
`0.1`, `--help`/missing `--version` implications, current binary naming, and
future `pandapi-engine` migration implications.

F-8: done. Findings E-008, E-009, E-012, and E-015 audit timeout polling,
`problemSolver.sh` alarm/`kill 0`, SAT signal/thread behavior, raw ownership,
and translation temp/child-process behavior.

F-9: done. Findings E-010, E-011, and E-016 audit CMake, gengetopt generated
files, warning policy, local/CI verification gaps, fixture coverage, and
optional CUDD/BDD build posture.

F-10: done. Finding E-014 and report §4 identify shared process-status,
stream-routing, color/TTY, version/provenance, RAII file/temp, and contract-test
candidates for cross-codebase synthesis.

F-11: done. Report §5 contains eight clean checks tied to engine audit scope.

F-12: done. Report §4 lists synthesis inputs comparing engine with parser and
grounder, plus shared/library and third-party dependency candidates.

F-13: done. Each source-quality finding maps to C++ Core Guidelines rule IDs
where applicable, and process-contract findings are labeled as
Chengdu-specific managed-process findings where no direct Core Guidelines rule
applies.

F-14: done. `git diff --name-only -- pandaPI scripts .github README.md
release-manifest.txt vendor.env pins.env` exited 0 with no output after the
build/probes and before closeout artifact edits.

## Commands Run

| Command | Exit | Evidence |
|---------|------|----------|
| `git status --short --branch` | 0 | `## release/0.3.x` |
| `rg --files pandaPI/pandaPIengine` | 0 | Inventoried engine source, CMake inputs, bundled CUDD source, docs, and wrapper script. |
| `find pandaPI/pandaPIengine -maxdepth 3 -type d | sort` | 0 | Listed top-level engine directories. |
| `find pandaPI/pandaPIengine -maxdepth 3 -type d \( -iname '*test*' -o -iname '*fixture*' \) -print | sort` | 0 | No output; no engine-local test/fixture dirs found. |
| `./scripts/build-engine.sh` | 0 | Produced `dist/macos-arm64/pandaPIengine`; warnings recorded in report §2 and E-011. |
| CLI probe script for `--help` and missing file | 0 | `--help` status 0; missing file status 1 with stdout metadata and stderr diagnostic. |
| `./scripts/smoke-test.sh --negative` | 0 | 4 passed, 0 failed; unsolvable engine exit 2 with `Status: Proven unsolvable`. |
| `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env` | 0 | No output. |

## Bubble-up to the Arc

Delivered-as-assigned: yes. Slice03 produced the planned engine report at the
arc-local path with concrete file/line findings, C++ Core Guidelines mappings
where applicable, clean checks, and cross-codebase synthesis inputs.

What the arc-plan did not anticipate: the engine's default progression path has
already absorbed the narrow 0.2.0 unsolvable exit-status fix, so the report
distinguishes that clean check from the broader remaining taxonomy gap. The
optional SAT and BDD paths are materially different enough that the synthesis
should decide whether they are in or out of the 0.3.0 managed-process contract
before implementation arcs inherit them accidentally.

Silent-drop diff: none. The assignment required the report, all 14 ledger rows,
at least five clean checks, explicit synthesis inputs, and no source/build or
release edits. The report landed with 16 findings, 8 clean checks, synthesis
inputs, and protected-path diff evidence. `cdc-verification.md` is intentionally
not written by CC.

## What Worked

- The parser-audit baseline gave this report a comparable category and severity
  vocabulary.
- Current build/probe evidence kept the report tied to this branch rather than
  only prior 0.2.0 notes.
- The existing negative smoke gate cleanly proved the one status behavior that
  was already repaired before this audit.
