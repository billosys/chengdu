# chengdu arc02 - upstream-absorption - arc plan

> Plan-of-record for arc02, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). Arc01 closed at
> [`../arc01-vendor-identity/closing-report.md`](../arc01-vendor-identity/closing-report.md).

## 1. Capability statement

Roadmap line: *Vetted upstream PRs and triaged genuine issues are absorbed
with full credit, gated by the in-tree CI; `v0.2.0` released in wolong's
frozen asset shape.*

Expanded: this arc starts from arc01's public in-tree fork baseline and turns
it into the releasable 0.2.0 fork. It refreshes the live upstream PR/issue
state, decides which changes are genuinely in scope for 0.2.0, absorbs the
selected upstream PRs and issue fixes as ordinary in-tree chengdu history, and
ships `v0.2.0` without changing wolong's fetch/install contract. Every
absorbed upstream PR keeps original author identity and names the upstream PR
URL; every issue fix names the upstream issue URL. The full no-fetch
build/provenance/smoke/package gate remains the behavioral boundary.

Arc02 is allowed to change planner source only where the change is traceable
to a vetted upstream PR or a reproduced genuine upstream issue. It is not the
0.3.0 CLI/API redesign arc.

## 2. Current upstream snapshot

CDC refreshed primary-source GitHub state on 2026-08-08 while opening this
arc. Slice01 must refresh it again before making dispositions.

Known open pull requests:

| Repo | PR | State | Title | Head SHA | Notes |
|------|----|-------|-------|----------|-------|
| `pandaPIengine` | [#14](https://github.com/panda-planner-dev/pandaPIengine/pull/14) | open | Set exit status from search. | `304048392e2b3ee53aca7f93d88b5a35230f638b` | Current detailed API check reports `mergeable_state=dirty`; likely absorbs or informs the exit-status slice. |
| `pandaPIengine` | [#15](https://github.com/panda-planner-dev/pandaPIengine/pull/15) | open | Easier-to-user Panda script. | `bb8a4c7acf393eee97a910d9551af6fc30e9480a` | Needs triage against chengdu's existing wrapper/release shape. |
| `pandaPIengine` | [#9](https://github.com/panda-planner-dev/pandaPIengine/pull/9) | open | merge SAT verifier | `68747acbcaa611818331a49ebe79801346bb86df` | Older base; needs applicability and release-surface review. |
| `pandaPIparser` | [#21](https://github.com/panda-planner-dev/pandaPIparser/pull/21) | open | Add "make install" target | `9735763ffd694d95c287766a447deba980d7deaa` | May be no-op for chengdu release packaging; must be explicitly dispositioned. |
| `pandaPIgrounder` | [#7](https://github.com/panda-planner-dev/pandaPIgrounder/pull/7) | open draft | Dockerfile for grounder. | `f289c5c8033785eb5845842143726d86834bc8fe` | Draft and likely outside chengdu's binary-release contract; must be explicitly dispositioned. |

Known issue candidates from the project plan:

| Repo | Issue | Title | Initial routing |
|------|-------|-------|-----------------|
| `pandaPIengine` | [#12](https://github.com/panda-planner-dev/pandaPIengine/issues/12) | Set exit status based on success/failure? | Likely covered by PR #14 if reproducible and applicable. |
| `pandaPIgrounder` | [#8](https://github.com/panda-planner-dev/pandaPIgrounder/issues/8) | Feature request: set exit status when goal unreachable | Candidate only if bounded to upstream-described behavior; otherwise 0.3.0. |
| `pandaPIparser` | [#25](https://github.com/panda-planner-dev/pandaPIparser/issues/25) | Segmentation fault running parser to check plan | Reproduce before fixing. |
| `pandaPIparser` | [#15](https://github.com/panda-planner-dev/pandaPIparser/issues/15) | Domain with conditional effects causes parser to fail to terminate | Reproduce before fixing; may be 0.3.0/non-bounded. |
| `pandaPIparser` | [#28](https://github.com/panda-planner-dev/pandaPIparser/issues/28) | pandaPIparser run "Killed" | Reproduce before fixing; may be resource-limit/0.3.0. |
| `pandaPIparser` | [#29](https://github.com/panda-planner-dev/pandaPIparser/issues/29) | Domain and problem that cause parsing explosion | Reproduce before fixing; may be resource-limit/0.3.0. |

Broader open upstream issues still require slice01 review so nothing is
silently dropped, but the table above is the planned 0.2.0 candidate set.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `upstream-triage` | Refresh live upstream PR/issue state, dry-run PR applicability against the in-tree subtrees, attempt issue reproduction from upstream evidence, and update this arc plan with an explicit absorb/defer/no-op queue before any source changes. | all later arc02 slices; project ledger P3 |
| slice02 | `engine-exit-status` | Expected first absorption slice: absorb or reimplement the bounded engine exit-status change from PR #14 / issue #12 with full upstream credit, tests/smoke evidence, and no 0.3.0 CLI redesign. Final scope is set by slice01. | release slice; wolong usability |
| slice03 | `vetted-pr-carry` | Absorb or explicitly no-op the remaining vetted PRs (`engine` #15/#9, `parser` #21, `grounder` #7) according to the slice01 queue; split if triage shows the work will not fit one context. | release slice; project ledger P3 |
| slice04 | `bounded-issue-fixes` | Fix only reproduced, bounded upstream issues selected by slice01 that are not already covered by PR absorption; defer nontermination/OOM/API redesign material to 0.3.0 with re-entry conditions. | release slice; project ledger P3 |
| slice05 | `release-publication` | Run the final full no-fetch gate, update provenance/license/release docs if source changes require it, publish `v0.2.0` in wolong's frozen asset shape, verify wolong's 4-command fetch path, and file upstream courtesy announcements. | project close |

Slices 02-04 are intentionally provisional until slice01 closes. If slice01
finds a PR is no-op, conflict-heavy, superseded by chengdu, or out of 0.2.0
scope, the arc plan must be updated before planning the next implementation
slice.

## 4. Dependencies

**Consumes:** arc01's in-tree fork baseline; `vendor.env`; `docs/license-audit-v0.2.0.md`;
`README.md`, `.github/`, `scripts/`, and release packaging from the no-fetch
baseline; live upstream PR/issue state from GitHub.

**Leaves for project close:** a published `v0.2.0` release whose source changes
are credited and traceable, whose CI/release evidence is green, whose source
availability and license/provenance story remain correct, and whose assets are
fetchable by wolong without changing wolong's frozen spec.

## 5. Open questions and risks

- **OQ1 - absorption queue is not yet final.** Slice01 owns the live
  disposition. A candidate remains "planned input," not "accepted work," until
  slice01 refreshes upstream, dry-runs applicability, and records absorb /
  defer / no-op.
- **OQ2 - conflict risk.** Engine PR #14 currently reports a dirty merge state
  against upstream. That does not decide chengdu applicability, but it means
  slice02 must be planned from a dry-run, not from hope.
- **OQ3 - 0.2.0 versus 0.3.0 boundary.** Exit-status fixes described by
  upstream issues/PRs are in scope. Systematic stdout/stderr, machine-readable
  status, unbounded resource policy, and CLI/API redesign remain 0.3.0 unless
  the operator amends the project boundary.
- **OQ4 - release-source identity after source changes.** Arc01 provenance
  already names chengdu commit plus import identities. Arc02 source changes do
  not require new import identities, but release notes/audit wording must not
  imply byte identity with upstream pins after absorption.
- **OQ5 - upstream courtesy timing.** The project plan requires announcement
  issues per upstream repo. Slice05 should prepare/file them after the release
  identity and absorbed-change list are final.

## 6. Arc ledger

Composition rows verifying the capability; open here, close (per-row walk) in
this arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Slice01 closes with a refreshed upstream PR/issue disposition covering all known candidate PRs/issues and the broader open upstream issue lists, with no silent drops. | reproduced |
| A2 | Every absorbed upstream PR commit in `pandaPI/` preserves original author identity and names its upstream PR URL; every issue fix names its upstream issue URL. | reproduced |
| A3 | Every candidate from the arc input set has a final absorb/defer/no-op disposition with rationale and, for deferrals, a re-entry condition. | reproduced |
| A4 | After all selected absorptions/fixes, the full local and CI gates pass: parser, grounder, engine, `check-provenance`, positive smoke, negative smoke, package dry-run, maintained static checks, and full matrix workflow. | reconciled |
| A5 | Active build/release paths still consume only in-tree source, and release provenance/manifests remain anchored on chengdu commit plus import identities without reviving upstream fetches or patch files. | reproduced |
| A6 | `v0.2.0` is published in wolong's frozen asset shape; `SHA256SUMS` verifies; tarball contents remain the expected fixtures, three binaries, and `provenance.txt`; wolong's 4-command fetch spec succeeds on clean supported platforms. | reproduced |
| A7 | Upstream courtesy announcements for parser, grounder, and engine are filed, with URLs recorded, naming fork rationale and absorbed upstream PRs/issues with credit. | reproduced |

## 7. Version history

- **v1.0 - 2026-08-08.** Initial arc02 breakdown and slice01 open set. Source:
  project plan v1.6, arc01 closing report, and primary-source GitHub API
  refresh for upstream PR/issue state. No slice bubble-ups yet.
