# chengdu 0.2.0 — project plan

> **chengdu 0.2.0**: pandaPI comes in-tree. The three planner components
> (and the grounder's former submodules) are vendored into this
> repository with full upstream history — a deliberate, public,
> credited fork — then tightened against their upstreams' own unmerged
> contributions and genuine open issues. From 0.2.0 onward chengdu is a
> classic multi-binary C++ monorepo that *contains* the planners, not a
> harness that fetches them.
>
> Plan-of-record at project scale, per the collaboration framework's
> `PROJECT-MANAGEMENT.md` (v2.1). Predecessor: `docs/design-v0.1.0/`
> (closed 2026-08-06, gate GO). **This plan formally supersedes the
> 0.1.0 non-goal "not a development fork of pandaPI"** — a tracked
> change, decided by the operator 2026-08-06 with the evidence basis in
> §2. As at 0.1.0, a separate design doc is deliberately not written:
> the 0.1.0 runbook + build scripts remain the build spec, and this plan
> carries the vendoring architecture. (Disclosed judgment; revisit if
> arc02 triage reveals design weight this plan can't carry.)

## 1. Definition of done, and boundaries

**Done means:** the pandaPI sources live under `pandaPI/` in this
repository with full upstream git history preserved and
identity-verified at import; the build matrix and release machinery
consume **only in-tree sources** (no network fetch of planner source at
build time); vetted upstream PRs and triaged genuine issues are
absorbed with full author credit; and a `v0.2.0` release ships binaries
built from the in-tree sources that pass the identical gate sequence
(positive five-gate chain + the four negative gates) on the full
support matrix, published in the **exact asset shape frozen at 0.1.0**
(wolong's fetch spec: asset naming, `SHA256SUMS`,
`release-manifest.txt`, tarball contents).

**Support matrix:** unchanged — `linux-x86_64`, `macos-arm64`.
`linux-arm64` remains a deferred candidate (carried from 0.1.0).

**Explicit non-goals (0.2.0):**

- **No behavioral overhaul.** Exit-code contracts, stdio discipline,
  machine-readable output, the CLI/API redesign — all deferred to
  0.3.0 (pre-planning notes: `docs/design-v0.3.0/notes.md`). 0.2.0
  absorption fixes are limited to what upstream PRs/issues already
  describe; the systematic redesign is not smuggled in piecemeal.
- **No promise of upstream tracking post-fork.** Divergence is now the
  point. If upstream wakes, cherry-picking remains *possible* (the
  histories stay connected) but is unplanned.
- **No new platforms, no Windows, no package-manager distribution, no
  HDDL tooling of our own, no Aries/SHOP3** — all carried unchanged
  from 0.1.0.

**Primary consumer:** `wolong`, unchanged. Its frozen fetch spec is a
hard constraint on the v0.2.0 release (ledger row P5).

## 2. Research basis — the fork decision record

Recorded here so the decision is in-repo at the correct contextual
level; the fuller session narrative lives in the operator's decision
record (AI Methodologies project, 2026-08-06 entry).

**Decision (operator, 2026-08-06):** fork — vendor all three repos
in-tree with full history, superseding patches-not-fork.

**Evidence, with strengths:**

- *Upstream dormancy — reproduced* (2026-08-06 sandbox clones):
  last commits — parser 2024-06-10, engine 2024-05-14, grounder
  2025-01-20 (a one-commit external PR merge). **Pins verified ≡
  upstream default-branch HEADs for all three repos** (same session):
  vendoring at our pins *is* vendoring at latest upstream.
- *Maintainer non-response — attested* (GitHub, fetched 2026-08-06):
  rpgoldman (SHOP2/3 author) holds four unmerged PRs since 2023 —
  including engine PR #14 "Set exit status from search," ~3 years
  unmerged, which is precisely wolong's top blocker; engine issue #12
  and grounder issue #8 (exit-status requests, 2023) unaddressed;
  documented contributor Songtuan-Lin's SAT-verifier PR #9 unmerged
  since 2023-07; parser issue #1 (license clarification) open since
  2020-11.
- *Behavioral warts — field-tested* (chengdu 0.1.0's own
  `smoke-test.sh` and runbook): exit-code chaos (255/2/1, and 0 on
  "Proven unsolvable"); status determinable only by grepping stdout;
  ANSI leakage past `--no-colour`; plan interleaved with logs on
  stdout; no buffering/flush discipline; engine spins unbounded
  (12+ GB RSS observed) on malformed `.sas` input.
- *Rationale:* the go/no-go was never "are the binaries ill-behaved"
  but "is wolong's needed delta expressible as a bounded patch set
  against a dormant upstream." The 0.3.0 ambitions (API/stdio
  overhaul) answer no by themselves — an overhaul cannot be run as
  patch files. The fork follows from the roadmap; the audit's role
  shifts from go/no-go to scoping.

## 3. Vendoring architecture (the load-bearing decisions)

- **Mechanism: `git subtree add --prefix=pandaPI/<component>
  <remote> <sha>` (no `--squash`)** — one import merge commit per
  component, full upstream history woven into chengdu's, zero content
  changes at import. No separate git worktrees are created or
  maintained at any point; from import onward this is a single
  repository, in the manner of classic multi-binary C++ monorepos.
- **Layout:** `pandaPI/pandaPIparser/`, `pandaPI/pandaPIgrounder/`,
  `pandaPI/pandaPIengine/`, plus the grounder's former submodules
  de-submoduled and imported the same way (`cpddl`,
  `h2-fd-preprocessor` — gitlab.com/danfis origins), so the fork has
  **zero build-time network dependency on gitlab.com**. Exact nesting
  (inside the grounder dir vs. sibling dirs) is an arc01 planning
  decision.
- **`patches/` dissolves:** the existing named patches land as regular
  in-tree commits (provenance in the commit messages) and the patch
  files are retired.
- **Absorption with credit:** upstream PRs are absorbed by fetching
  `refs/pull/N/head` from the original repos and
  `git cherry-pick -Xsubtree=…` — original author and date preserved
  natively in commit metadata, upstream PR URL in a commit trailer.
  Issue fixes reference their upstream issue URL in the commit message.
- **Provenance rework:** `pins.env` becomes historical (frozen as the
  import-point record); `check-provenance.sh` and the manifests
  re-anchor on the chengdu commit + the per-component import-merge
  SHAs. `retired upstream-fetch helper` retires.
- **Licensing:** vendoring GPL-3.0 source (h2-fd-preprocessor) publicly
  is compliant — but per-directory license delineation is required, and
  the audit gets a `license-audit-v0.2.0.md` revision. Each `pandaPI/*`
  subdir carries a NOTICE stating provenance (upstream URL, import SHA,
  divergence policy).
- **Upstream courtesy:** one announcement issue per upstream repo
  (forked at <sha>, absorbed PRs #N with credit, rationale), per
  `CONTRIBUTION-STYLE.md` voice. The 0.1.0 carried-forward ticket
  candidates (three macOS fixes; h2 license-file gap) ride along as
  courtesy notes rather than obligations — tracked, not dropped.

## 4. Arc roadmap

| Arc | Slug | Capability (one line) | Depends on |
|-----|------|----------------------|------------|
| arc01 | `vendor-identity` | pandaPI (and former submodules) live in-tree with full history, **identity-verified against upstream at the pins**, built and gated by CI from in-tree sources only — behaviorally 0.1.0-equivalent by the same gates. | — |
| arc02 | `upstream-absorption` | Vetted upstream PRs and triaged genuine issues are absorbed with full credit, gated by the in-tree CI; `v0.2.0` released in wolong's frozen asset shape. | arc01 |

Load-bearing note: arc01 re-points the build *before* any content
change, so every absorbed change in arc02 runs through the CI gates
0.1.0 already proved. "Move the code" and "change the code" never share
a commit.

Detailed arc planning is deferred per *plan late, plan deep*; arc01 is
next for detailed planning. Known arc02 triage inputs (not yet vetted,
listed to avoid re-discovery): PRs — engine #14 (exit status), #15
(driver script), #9 (SAT verifier); parser #21 (`make install`);
grounder #7 (Dockerfile, draft). Issues — parser #25 (segfault), #15
(non-termination), #28 (OOM kill), #29 (parsing explosion); grounder
#8 (exit status on unreachable goal); engine #12 (exit status; likely
subsumed by PR #14). Reproduce-at-pins before fixing; some may be
no-ops or 0.3.0 material.

## 5. Current status

- **arc01 — closed 2026-08-08.** Closing report:
  [`arc01-vendor-identity/closing-report.md`](arc01-vendor-identity/closing-report.md).
- **arc02 — planned at roadmap level only; ready for detailed planning.**

## 6. Project ledger

Composition rows verifying the DoD; they open here and close (per-row
walk) in this project's `closing-report.md`. Strength vocabulary per
`LEDGER-DISCIPLINE.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| P1 | For every imported component, the tree at its import commit is diff-identical to the upstream tree at its pinned SHA (`git diff <import>^2 <upstream-pin> -- .` empty, or equivalent), and full upstream history is reachable (`git log` within the subdir reaches the upstream root commits). | reproduced |
| P2 | A full CI matrix run builds and gates all three binaries from in-tree sources with zero network fetch of planner source (workflow logs show no clone of panda-planner-dev or gitlab.com). | reproduced |
| P3 | Every absorbed upstream PR commit preserves the original author identity and names its upstream PR URL; every absorbed issue fix names its upstream issue URL. Verified by `git log` inspection across `pandaPI/`. | reproduced |
| P4 | Licensing holds post-vendoring: per-directory delineation present, `license-audit-v0.2.0.md` landed, NOTICE files in every `pandaPI/*` subdir, `THIRD-PARTY-LICENSES` regenerated. | reproduced |
| P5 | The `v0.2.0` release is live, published only after a green full-matrix gate, in the exact asset shape frozen at 0.1.0 — verified by running wolong's 4-command fetch spec against it on a clean machine per platform. | reproduced |

## 7. Version history

- **v1.5 — 2026-08-08.** Closed arc01 after Slice04 CDC verification and
  arc-level composition check. No roadmap scope change. Surfaced by: arc01
  close. Why: arc01 delivered the in-tree, identity-verified, no-fetch,
  provenance/NOTICE/licensing baseline required before arc02 upstream
  absorption.
- **v1.4 — 2026-08-07.** Updated current status after slice03 closed and
  slice04 opened. No roadmap scope change. Surfaced by: slice04 planning.
  Why: status-only plan maintenance so the project plan points to the active
  slice artifacts.
- **v1.3 — 2026-08-07.** Updated current status after slice02 closed and
  slice03 opened. No roadmap scope change. Surfaced by: slice03 planning.
  Why: status-only plan maintenance so the project plan points to the active
  slice artifacts.
- **v1.2 — 2026-08-07.** Updated current status after slice01 closed and
  slice02 opened. No roadmap scope change. Surfaced by: slice02 planning.
  Why: status-only plan maintenance so the project plan points to the active
  slice artifacts.
- **v1.1 — 2026-08-07.** Opened arc01 detailed planning and slice01's
  open set. No roadmap scope change: this is the planned transition from
  "arc01 next" to "arc01 active" recorded so the project plan points to
  the active arc/slice artifacts. Surfaced by: operator instruction to
  begin arc01 planning. Why: status-only plan maintenance before slice01
  implementation starts.
- **v1.0 — 2026-08-06.** Initial roadmap. Source: the fork-decision
  working session of 2026-08-06 (evidence basis in §2; operator
  decisions: fork confirmed; 0.2.0 = two arcs, one release; vendor at
  the pins, which were verified ≡ upstream HEADs that session). Opened
  the same day chengdu 0.1.0 closed (gate GO); supersedes its
  patches-not-fork non-goal, tracked here. No child bubble-ups yet.
