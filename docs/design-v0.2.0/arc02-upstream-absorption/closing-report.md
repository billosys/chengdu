# Arc02 Closing Report - upstream-absorption

Closer: CDC
Branch: `release/0.2.x`
Closed on: 2026-08-09
Close commit: this report's adding commit
Gate review: separate read-only agent review, 2026-08-09, verdict GO with
A2 credit-wording caveat incorporated below.

## Capability Verdict

Delivered. Arc02 promised:

> Vetted upstream PRs and triaged genuine issues are absorbed with full credit,
> gated by the in-tree CI; `v0.2.0` released in wolong's frozen asset shape.

The four planned slices compose into that capability. Slice01 refreshed the
upstream queue and separated absorb/defer/no-op work. Slice02 absorbed the
engine exit-status change from PR #14 / issue #12. Slice03 absorbed parser PR
#21. Slice04 published the public `v0.2.0` release from the resulting in-tree
fork, verified the supported-platform consumer path, and filed upstream
courtesy issues.

## Slice Walk

The arc plan lists four slices. All four are closed and CDC-verified:

| Slice | Outcome | Evidence |
|-------|---------|----------|
| slice01 `upstream-triage` | delivered | `slice01-upstream-triage/cdc-verification.md` accepted rows F-1 through F-12 at `e3264eb7`; it updated the arc queue and slice breakdown with no silent drops. |
| slice02 `engine-exit-status` | delivered | `slice02-engine-exit-status/cdc-verification.md` accepted the bounded engine PR #14 / issue #12 port at `4da38819` and docs close at `1ea60387`. |
| slice03 `parser-makefile-pr` | delivered | `slice03-parser-makefile-pr/cdc-verification.md` accepted the native author-preserving parser PR #21 absorption at `49c02f7f` and docs close at `8810395c`. |
| slice04 `release-publication` | delivered | `slice04-release-publication/cdc-verification.md` accepted the public release tag `v0.2.0` at `d38e30d396d435d5a80992106f59b403c20909e9`, with close docs at `d0d8ef63` and CDC close at `f5e427fb`. |

Slice count matches the arc-plan breakdown: 4 planned, 4 closed, 0 missing.

## Composition Check

The arc-level delivered diff is intentionally narrow. From Arc02 open
`8e282675` through Slice04 CDC verification `f5e427fb`, the release-relevant
changed files outside planning docs are:

```text
README.md
pandaPI/pandaPIengine/src/SearchEngine.cpp
pandaPI/pandaPIengine/src/sat/sat_planner.cpp
pandaPI/pandaPIengine/src/sat/sat_planner.h
pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h
pandaPI/pandaPIengine/src/translation/translationController.cpp
pandaPI/pandaPIengine/src/translation/translationController.h
pandaPI/pandaPIparser/makefile
scripts/smoke-test.sh
```

That set matches the selected absorption queue plus the release-facing README
update. No grounder source, workflow, package-shape, publish-script, SAT
verifier, Docker, parser normalization/resource, or 0.3.0 CLI/API redesign work
was smuggled into this arc.

The source-credit trail is intact, with one explicit manual-port exception:

- `4da38819 engine: propagate search exit status` is a bounded manual port
  because upstream PR #14 was dirty against the in-tree subtree. It does not
  preserve Robert P. Goldman as native Git author; instead it records
  `Upstream-author: Robert P. Goldman <rpgoldman@sift.net>` and includes
  `Upstream-PR: https://github.com/panda-planner-dev/pandaPIengine/pull/14`
  plus
  `Upstream-Issue: https://github.com/panda-planner-dev/pandaPIengine/issues/12`.
- `49c02f7f Fix install dependency.` preserves Robert P. Goldman as Git author
  and includes
  `Upstream-PR: https://github.com/panda-planner-dev/pandaPIparser/pull/21`.
- Both implementation commits include the required Codex and Billo AI
  co-author trailers.

The release evidence was refreshed during arc close:

| Claim | Evidence |
|-------|----------|
| Public release | GitHub API reports `v0.2.0` public, non-draft, non-prerelease, published `2026-08-09T04:39:12Z`: https://github.com/billosys/chengdu/releases/tag/v0.2.0 |
| Tag target | Remote ref `refs/tags/v0.2.0` is object type `commit`, SHA `d38e30d396d435d5a80992106f59b403c20909e9`. |
| Pre-tag matrix | Build run `31294772117` completed `success` at head SHA `d38e30d396d435d5a80992106f59b403c20909e9`. |
| Release workflow | Release run `31294905237` completed `success` at the same head SHA. |
| Asset shape | Release assets are exactly `pandapi-v0.2.0-linux-x86_64.tar.gz`, `pandapi-v0.2.0-macos-arm64.tar.gz`, `release-manifest.txt`, `SHA256SUMS`, and `THIRD-PARTY-LICENSES`. |
| Consumer path | Slice04 CDC reproduced public macOS arm64 and Linux x86_64 consumer walks; both ended with `Plan verification result: true`. |

## Arc Ledger Walk

Rows verified: 7. Done: 7. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| A1 | Done, reproduced. Slice01 CDC verification accepted the refreshed upstream PR/issue disposition covering selected PRs, planned issues, and broader open upstream issue lists. The arc-plan queue was updated to v1.1 with absorb/defer/no-op decisions and no silent drops. |
| A2 | Done, reproduced with explicit caveat. Parser PR #21 preserves Robert P. Goldman as native Git author and names the upstream PR URL. Engine PR #14 / issue #12 could not be natively cherry-picked because the PR was dirty against the in-tree subtree; the accepted implementation is a bounded manual port that records Robert P. Goldman with `Upstream-author`, names the upstream PR and issue URLs, and carries the required assistant co-author trailers. |
| A3 | Done, reproduced. Every candidate from the arc input set has a final disposition in `arc-plan.md`: absorb for engine #14/#12 and parser #21; defer for engine #9, parser #15/#29, grounder #8, and engine #16 with re-entry conditions; no-op for the remaining support/build/Docker/licensing/status items with rationale. |
| A4 | Done, reconciled. Slice02, Slice03, and Slice04 CDC verifications reproduced local parser/grounder/engine builds, provenance, positive smoke, negative smoke, package dry-runs, shell/static checks, and diff hygiene. Live GitHub refresh confirms the pre-tag build matrix and tag release workflow both completed `success` at the release commit. |
| A5 | Done, reproduced. Active build/release paths consume in-tree `pandaPI/` sources only; Slice04 CDC verified release manifests/provenance naming the chengdu release commit, source prefixes, upstream SHAs, import commits, and `patches=none`, with no revived upstream fetches or active patch files. |
| A6 | Done, reproduced. `v0.2.0` is public in wolong's frozen asset shape; public assets checksum, unpack, and reconcile; public macOS arm64 and Linux x86_64 consumer walks pass from release tarballs. |
| A7 | Done, reproduced. Upstream courtesy issues are filed and live: engine https://github.com/panda-planner-dev/pandaPIengine/issues/18, parser https://github.com/panda-planner-dev/pandaPIparser/issues/34, grounder https://github.com/panda-planner-dev/pandaPIgrounder/issues/15. |

## Accumulated Plan Changes

Arc02 plan changes were tracked in `arc-plan.md`:

- v1.1: Slice01 replaced the provisional queue with the final
  absorb/defer/no-op dispositions and reduced the remaining work to
  engine exit status, parser PR #21, and release publication.
- v1.2: Slice02 was opened from the triage queue.
- v1.3: Slice03 was opened after Slice02 CDC verification.
- v1.4: Slice04 was opened after Slice03 CDC verification.
- v1.5: Slice04 CDC verification updated the arc status to ready for close.
- v1.6: Arc02 close gate review tightened the A2/source-credit wording for
  the accepted engine PR #14 manual-port exception.

No untracked arc-plan findings remain. Deferred work is explicit 0.3.0/backlog
material, not hidden Arc02 scope.

## Bubble-up to Project

Arc02 delivered the project roadmap capability as written. It turns Arc01's
in-tree fork baseline into the releasable `v0.2.0` fork: selected upstream
work is credited and traceable, the release exists publicly in the frozen
wolong asset shape, the supported-platform consumer path passes, and upstream
courtesy breadcrumbs are filed.

What Arc02 revealed that the project plan did not anticipate: no new project
arc is required. The one material slice-level handoff, Linux public consumer
execution, was discharged during Slice04 CDC verification. Deferred parser
normalization/resource work, grounder status policy, SAT verifier absorption,
and broader CLI/API redesign remain outside v0.2.0 and belong to 0.3.0/backlog
planning.

Silent-drop diff at arc scale: complete. The project roadmap expected vetted
upstream absorption, issue triage, release publication, wolong-compatible
assets, and courtesy announcements. All landed. The deferred/no-op upstream
items are named with rationale and re-entry conditions where applicable.

Project-plan change required: yes, and applied in this close. `project-plan.md`
moves Arc02 from ready-for-close to closed and marks the project ready for final
project close/status synthesis. It also tightens the upstream-credit wording so
project close does not imply that every absorbed PR landed as a native
author-preserving cherry-pick: parser PR #21 did, while engine PR #14 was an
accepted credited manual-port exception. No definition-of-done, platform
matrix, asset shape, or roadmap-scope change is required.

## What Worked

- The slice01 triage gate kept source absorption small: only engine #14/#12 and
  parser #21 entered implementation.
- Native subtree history plus upstream trailers made credit verification easy
  at arc scale.
- Release verification used the public artifacts on both supported platforms,
  which turns the wolong fetch contract from an assertion into reproduced
  evidence.
