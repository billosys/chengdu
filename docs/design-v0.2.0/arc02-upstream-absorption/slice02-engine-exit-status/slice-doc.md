# chengdu arc02 / slice02 - engine-exit-status - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Absorb upstream pandaPIengine PR
[#14](https://github.com/panda-planner-dev/pandaPIengine/pull/14) and issue
[#12](https://github.com/panda-planner-dev/pandaPIengine/issues/12) as a
bounded exit-status fix in chengdu's in-tree engine fork. At slice close,
successful search still exits `0`, search failure / proven-unsolvable exits
`2`, error paths remain nonzero, the smoke harness proves the distinction, and
the source change is credited and traceable to the upstream PR/issue.

This is an upstream absorption slice, not the 0.3.0 status/stdio/API redesign.

## 2. Upstream snapshot at slice open

CDC refreshed primary-source state on 2026-08-08 before opening this slice:

| Item | Current state |
|------|---------------|
| PR | [`panda-planner-dev/pandaPIengine#14`](https://github.com/panda-planner-dev/pandaPIengine/pull/14), "Set exit status from search." |
| State | open, non-draft |
| Author / head repo | `rpgoldman` / `rpgoldman/pandaPIengine:set-exit-status` |
| Head SHA | `304048392e2b3ee53aca7f93d88b5a35230f638b` |
| Base | `panda-planner-dev/pandaPIengine:master` at `17e8d98cbbaed38ee25b136e8ead08332c673275` |
| Merge state | `dirty` |
| Size | 4 commits, 7 files, +387/-376 |
| PR commits | `cfc30f34` "Set exit status from search."; `a6e24ebe` "Ignore gengetopt and cmake outputs."; `b30cfeb2` "Return output status values."; `30404839` "initialize searchRes variable." |
| Commit author | Robert P. Goldman `<rpgoldman@sift.net>` for all four PR commits |
| Local applicability | `git apply --check --directory=pandaPI/pandaPIengine /tmp/chengdu-pr14.diff` conflicts in `src/SearchEngine.cpp`, `src/sat/sat_planner.cpp`, and `src/sat/sat_planner.h` |

Issue
[#12](https://github.com/panda-planner-dev/pandaPIengine/issues/12) remains
open. Its requested behavior is a designated nonzero status, specifically
`2`, on search failure while keeping `1` for error conditions. The maintainer
comment on 2023-11-14 says SAT solver states other than 10 or 20 are errors
except for unused termination modes, and the PR author points issue #12 to
PR #14.

## 3. Scope

**In:**

- Start from the Slice01 CDC-verified Arc02 queue and refresh PR #14 / issue
  #12 metadata at implementation time. Record the exact PR head SHA and
  authorship evidence in the closing report.
- Port the functional behavior of PR #14 into the in-tree engine source. The
  expected touched source files are:
  - `pandaPI/pandaPIengine/src/SearchEngine.cpp`
  - `pandaPI/pandaPIengine/src/sat/sat_planner.cpp`
  - `pandaPI/pandaPIengine/src/sat/sat_planner.h`
- Preserve upstream authorship and traceability. Prefer native
  `git cherry-pick -Xsubtree=...` with original author metadata for any commit
  that applies cleanly. Where conflicts require a manual port, credit Robert
  P. Goldman in the commit body/trailers and include upstream PR and issue
  URL trailers.
- Make the engine process exit reflect search outcome:
  - solved / successful search: exit `0`;
  - search failed / proven-unsolvable: exit `2`;
  - malformed input, usage/configuration errors, and internal errors: nonzero
    and not silently collapsed into success.
- Update `scripts/smoke-test.sh --negative` so the unsolvable fixture expects
  engine exit `2` together with `Status: Proven unsolvable`, and continues to
  report it as `UNSOLVABLE`, not generic failure and not success.
- Keep `scripts/smoke-test.sh` positive mode proving the minimal fixture exits
  `0` with `Status: Solved` and verifies true.
- Update README or docs text that still states the 0.2.0 active behavior as
  "engine exit 0 on Proven unsolvable", while preserving historical statements
  under `docs/design-v0.1.0/` and `docs/design-v0.3.0/notes.md`.
- Run and record the full local gate: parser build, grounder build, engine
  build, provenance check, positive smoke, negative smoke, package dry-run,
  maintained shell/static checks, and diff hygiene.

**Out:**

- No parser PR #21 work; that is slice03.
- No engine PR #9 SAT verifier absorption, engine PR #15 wrapper script,
  grounder PR #7 Docker work, grounder issue #8, parser #15/#29 expansion
  work, or parser unreproduced/no-op issues.
- No 0.3.0 CLI/API/status overhaul: no machine-readable output contract, no
  systematic stdout/stderr split, no multi-outcome enum API, no new wolong
  integration surface.
- No release publication or upstream courtesy issue filing; both belong to
  slice04.

## 4. Constraints

- Do not revive build-time source fetches, active patch files, or upstream
  clone paths. The build remains from in-tree `pandaPI/` source copies.
- Avoid broad formatting churn from PR #14. The refreshed PR diff contains
  substantial whitespace movement; this slice should port behavior, not make a
  noisy reformat.
- Do not commit generated engine build outputs, parser/grounder outputs,
  temporary PR worktrees, downloaded diffs, or trial cherry-pick state.
- Keep wolong's frozen release asset shape untouched: names, `SHA256SUMS`,
  `release-manifest.txt`, tarball contents, and 4-command install path.
- Keep shell changes bash-3.2 compatible.
- Assistant-authored commits use the current project footer convention:
  `Co-authored-by: Codex <noreply@openai.com>` and
  `Co-authored-by: Billo AI <ai-engineering@billo.systems>`.

## 5. Verification approach

CC verifies by recording the refreshed upstream snapshot, the chosen absorption
method, the bounded source diff, the smoke harness update, local build and
package gates, static checks, upstream-credit log evidence, and a clean final
worktree. CDC verifies by re-running practical gates, inspecting the diff for
scope and credit, checking the negative unsolvable outcome now combines exit
`2` with `Status: Proven unsolvable`, and confirming no 0.3.0 redesign or
release-shape change leaked into the slice.

## 6. Exit criteria

All ledger rows reach final status. Engine source exits `0` for solved and `2`
for search failure / proven-unsolvable. Positive and negative smoke tests pass
with the updated semantics. Provenance and packaging still pass. The commit log
credits upstream PR #14 / issue #12 and Robert P. Goldman. The closing report
walks every row and bubbles up any arc/project implications before CDC
verification begins.
