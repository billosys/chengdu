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
arc. CC refreshed it again on 2026-08-08 during slice01 before making
dispositions.

Current open pull requests:

| Repo | PR | State | Draft | Title | Head SHA | Merge state | Patch check against in-tree subtree | Disposition |
|------|----|-------|-------|-------|----------|-------------|-----------------------------------|-------------|
| `pandaPIengine` | [#14](https://github.com/panda-planner-dev/pandaPIengine/pull/14) | open | false | Set exit status from search. | `304048392e2b3ee53aca7f93d88b5a35230f638b` | dirty | conflicts in `SearchEngine.cpp`, `sat_planner.cpp`, `sat_planner.h` | **absorb** by bounded port in slice02; preserves Robert P. Goldman authorship/trailers where cherry-pickable and records PR URL. |
| `pandaPIengine` | [#15](https://github.com/panda-planner-dev/pandaPIengine/pull/15) | open | false | Easier-to-user Panda script. | `bb8a4c7acf393eee97a910d9551af6fc30e9480a` | clean | clean, whitespace warnings only | **no-op** for v0.2.0; adds a convenience driver script outside wolong's frozen release contract and chengdu's existing build/smoke/release entry points. |
| `pandaPIengine` | [#9](https://github.com/panda-planner-dev/pandaPIengine/pull/9) | open | false | merge SAT verifier | `68747acbcaa611818331a49ebe79801346bb86df` | clean | clean, whitespace warnings only | **defer** to 0.3.0/backlog; large verifier subsystem (13 commits, 39 files, vendored Catch2) changes build/license/test surface beyond 0.2.0 release hardening. Re-entry: plan a verifier/SAT capability slice with licensing and CI scope. |
| `pandaPIparser` | [#21](https://github.com/panda-planner-dev/pandaPIparser/pull/21) | open | false | Add "make install" target | `9735763ffd694d95c287766a447deba980d7deaa` | clean | clean | **absorb** in slice03; one Robert P. Goldman commit fixes the `all` dependency and adds an install target without changing runtime behavior. |
| `pandaPIgrounder` | [#7](https://github.com/panda-planner-dev/pandaPIgrounder/pull/7) | open | true | Dockerfile for grounder. | `f289c5c8033785eb5845842143726d86834bc8fe` | clean | clean, whitespace warnings only | **no-op** for v0.2.0; draft Docker/workflow/script surface is outside the binary-release contract. Grounder build portability is already handled in-tree by arc01. |

Planned issue candidate dispositions:

| Repo | Issue | Title | Evidence | Disposition |
|------|-------|-------|----------|-------------|
| `pandaPIengine` | [#12](https://github.com/panda-planner-dev/pandaPIengine/issues/12) | Set exit status based on success/failure? | Issue author points to PR #14; maintainer comments define SAT return-code direction. | **absorb** through slice02 with PR #14. |
| `pandaPIgrounder` | [#8](https://github.com/panda-planner-dev/pandaPIgrounder/issues/8) | Feature request: set exit status when goal unreachable | No reproducer or PR; policy question mentions possible `--error-on-fail`. | **defer** to 0.3.0/status contract. Re-entry: provide a grounding-stage unreachable-goal fixture and decide whether status change is default or opt-in. |
| `pandaPIparser` | [#25](https://github.com/panda-planner-dev/pandaPIparser/issues/25) | Segmentation fault running parser to check plan | Referenced gist downloaded; `pandaPIparser -v domain problem plan` exits 0 and verifies true on current macOS fork. | **no-op** for v0.2.0; unreproduced and likely covered by current upstream/chengdu baseline. |
| `pandaPIparser` | [#15](https://github.com/panda-planner-dev/pandaPIparser/issues/15) | Domain with conditional effects causes parser to fail to terminate | Referenced gist downloaded; `pandaPIparser -k domain problem` was still running at 15s and killed by alarm. | **defer** to 0.3.0/parser-normalization work. Re-entry: design static-antecedent or disjunction/forall handling with bounded output expectations. |
| `pandaPIparser` | [#28](https://github.com/panda-planner-dev/pandaPIparser/issues/28) | pandaPIparser run "Killed" | Body has command/output only; no domain/problem files attached. | **no-op** for v0.2.0 due unavailable reproducer. Re-open if exact inputs are provided; likely related to #29/resource exhaustion. |
| `pandaPIparser` | [#29](https://github.com/panda-planner-dev/pandaPIparser/issues/29) | Domain and problem that cause parsing explosion | Attached files downloaded; `pandaPIparser -k domain p06` was still running at 30s and killed by alarm. | **defer** to 0.3.0/parser-normalization work with #15. Re-entry: same static goal/disjunction/quantifier design, plus an output-size guard. |

Broader open upstream issues were reviewed during slice01. They do not add
new 0.2.0 source absorption work:

| Repo | Issues | Disposition |
|------|--------|-------------|
| `pandaPIengine` | #17 PDDL support/examples; #10 multiple plans | **no-op**: support/API expansion outside 0.2.0. |
| `pandaPIengine` | #16 infinite planning time | **defer** to 0.3.0/performance backlog. Re-entry: exact fixture plus agreed timeout/resource target. |
| `pandaPIengine` | #6 Windows build; #3 optional SAT dependency build | **no-op**: outside supported matrix and current SAT-off release path. |
| `pandaPIparser` | #24 verifier diagnostic/scoping; #23 task-index diagnostic | **no-op** for v0.2.0: unreproduced/diagnostic behavior, related to #25. |
| `pandaPIparser` | #16 build after `make clean`; #10 macOS flex/bison build | **no-op**: current import/chengdu build path already covers the build fixes. |
| `pandaPIparser` | #11 case sensitivity; #1 MIT relicensing | **no-op**: language semantics/licensing requests outside 0.2.0. |
| `pandaPIgrounder` | #12 bliss Mac build; #2 boruvka Mac build | **no-op**: arc01 in-tree grounder/cpddl/boruvka/bliss build path now passes macOS. |
| `pandaPIgrounder` | #11 and #10 bad parser output/diagnostics | **no-op** for 0.2.0: upstream comments say parser safety checks were added; no current chengdu fixture remains. Re-enter only with a failing current fixture. |
| `pandaPIgrounder` | #5 Dockerfile; #3 Windows/cpddl build | **no-op**: covered by draft PR #7 disposition or outside supported matrix. |

The 0.2.0 absorption queue is complete: engine #14/#12 landed in slice02,
parser #21 landed in slice03, and `v0.2.0` was published in slice04. Deferred
parser expansion/resource work and SAT verifier work are explicit
0.3.0/backlog candidates rather than silent drops.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `upstream-triage` | Refresh live upstream PR/issue state, dry-run PR applicability against the in-tree subtrees, attempt issue reproduction from upstream evidence, and update this arc plan with an explicit absorb/defer/no-op queue before any source changes. | all later arc02 slices; project ledger P3 |
| slice02 | `engine-exit-status` | Port/absorb engine PR #14 and issue #12 as a bounded exit-status change, with upstream author credit, PR/issue URL trailers, tests/smoke evidence, and no 0.3.0 CLI redesign. | release slice; wolong usability; project ledger P3 |
| slice03 | `parser-makefile-pr` | Absorb parser PR #21 as a small build-system fix with upstream author credit and PR URL trailer; confirm engine #15, engine #9, and grounder #7 remain no-op/deferred as triaged. | release slice; project ledger P3 |
| slice04 | `release-publication` | Run the final full no-fetch gate, update provenance/license/release docs if source changes require it, publish `v0.2.0` in wolong's frozen asset shape, verify wolong's 4-command fetch path, and file upstream courtesy announcements. | project close |

Slices 01-04 are closed and CDC-verified. Deferred work is not part of the
v0.2.0 release unless the operator explicitly changes the project boundary.

## 4. Dependencies

**Consumes:** arc01's in-tree fork baseline; `vendor.env`; `docs/license-audit-v0.2.0.md`;
`README.md`, `.github/`, `scripts/`, and release packaging from the no-fetch
baseline; live upstream PR/issue state from GitHub.

**Leaves for project close:** a published `v0.2.0` release whose source changes
are credited and traceable, whose CI/release evidence is green, whose source
availability and license/provenance story remain correct, and whose assets are
fetchable by wolong without changing wolong's frozen spec.

## 5. Open questions and risks

- **OQ1 - absorption queue resolved by slice01.** Accepted v0.2.0 source
  changes are engine #14/#12 and parser #21. Engine #9 and parser #15/#29 are
  deferred with re-entry conditions; engine #15, grounder #7, parser #25/#28,
  and broader support/build/Docker/licensing issues are no-op for this release.
- **OQ2 - conflict risk.** Engine PR #14 reports `mergeable_state=dirty`, and
  slice01's patch check conflicts in engine source files. Slice02 must plan a
  bounded port/adaptation, not a mechanical cherry-pick.
- **OQ3 - 0.2.0 versus 0.3.0 boundary.** Exit-status fixes described by
  upstream issues/PRs are in scope. Systematic stdout/stderr, machine-readable
  status, unbounded resource policy, and CLI/API redesign remain 0.3.0 unless
  the operator amends the project boundary.
- **OQ4 - release-source identity after source changes.** Arc01 provenance
  already names chengdu commit plus import identities. Arc02 source changes do
  not require new import identities, but release notes/audit wording must not
  imply byte identity with upstream pins after absorption.
- **OQ5 - upstream courtesy timing.** The project plan requires announcement
  issues per upstream repo. Slice04 should prepare/file them after the release
  identity and absorbed-change list are final.

- **OQ6 - deferred parser expansion issues.** Parser #15 and #29 reproduce as
  long-running parse/expansion behavior, but the fix is a parser-normalization
  design problem, not a release-hardening patch. Carry both to 0.3.0/backlog
  together with a bounded output-size/time expectation.

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

- **v1.5 - 2026-08-09.** Updated arc status after Slice04 CDC verification
  accepted the public `v0.2.0` release publication and reproduced both
  supported-platform consumer walks. No queue change. Surfaced by: Slice04 CDC
  verification. Why: all planned Arc02 slices are now closed and verified, so
  this arc is ready for composition close.
- **v1.4 - 2026-08-08.** Opened slice04's release-publication implementation
  plan after slice03 CDC verification. No queue change. Surfaced by: operator
  instruction to do slice04. Why: engine PR #14 / issue #12 and parser PR #21
  are now absorbed and verified, leaving the real `v0.2.0` release,
  wolong-frozen asset verification, and upstream courtesy announcements as the
  final Arc02 work.
- **v1.3 - 2026-08-08.** Opened slice03's parser-makefile-pr implementation
  plan after slice02 CDC verification. No queue change. Surfaced by: operator
  instruction to draw up the next slice docs. Why: parser PR #21 remains the
  final selected upstream PR absorption before release publication.
- **v1.2 - 2026-08-08.** Opened slice02's engine-exit-status implementation
  plan from the slice01 absorption queue. No queue change. Surfaced by:
  operator instruction to dive into slice02. Why: engine PR #14 / issue #12 is
  the first selected source absorption and needs a bounded port plan because
  the upstream PR is still dirty against the in-tree subtree.
- **v1.1 - 2026-08-08.** Slice01 refreshed live upstream PR/issue state and
  replaced the provisional queue with explicit dispositions. Source absorption
  narrowed to engine #14/#12 and parser #21; engine #9, parser #15/#29, and
  grounder #8 are deferred with re-entry conditions; wrapper/Docker/support/
  build-only/licensing items are no-op for v0.2.0. Slice breakdown reduced to
  engine exit status, parser makefile PR, and release publication. Surfaced by:
  slice01 upstream triage. Why: current primary-source evidence and local
  applicability/reproduction attempts separated release-hardening work from
  0.3.0/backlog material.
- **v1.0 - 2026-08-08.** Initial arc02 breakdown and slice01 open set. Source:
  project plan v1.6, arc01 closing report, and primary-source GitHub API
  refresh for upstream PR/issue state. No slice bubble-ups yet.
