# chengdu arc02 — release-publish — arc plan

> Plan-of-record for arc02, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md) (v1.1). Written at arc01
> close, per *plan late, plan deep* — arc02 is now the active work.

## 1. Capability statement

Roadmap line: *A git tag turns verified artifacts into a GitHub Release
with checksums and a provenance manifest.*

Expanded: this arc turns arc01's proven build matrix into a consumable
distribution channel. At arc close, pushing a tag `vX.Y.Z` runs the same
build+gate logic arc01 verified (reused, not duplicated), packages
per-platform tarballs, generates `SHA256SUMS` and an aggregated
provenance manifest, and publishes a GitHub Release with human-readable
notes naming the compat floors — **and demonstrably refuses to publish
when any gate fails** (project ledger P2's mechanism). A consumer — wolong
foremost — can then fetch pinned, checksummed, provenance-carrying
pandaPI binaries for both platforms from a release URL, with license
obligations satisfied. Arc02 adds no build logic: it packages what arc01
proves.

## 2. Decisions proposed at planning (operator reviews with this doc)

- **D1 — Versioning:** chengdu tags are `vX.Y.Z`, owned by chengdu (they
  version the *distribution*: pins bumps, patch changes, packaging
  changes — not upstream's versions, which have none). The manifest maps
  each release to the exact upstream SHAs + patches. First cut: `v0.1.0`.
- **D2 — macOS compat floor:** documented as **macOS 15+ (arm64)**, per
  arc01's `min_os=15.0` observation. Lowering via
  `MACOSX_DEPLOYMENT_TARGET` is recorded as possible future work, not
  attempted at 0.1.0.
- **D3 — Linux compat floor:** documented as **glibc 2.35+**
  (ubuntu-22.04 baseline, per arc01 OQ3), release assets packaged from
  the 22.04 leg only.
- **D4 — Candidates dispositioned:** `macos-x86_64` — **declined** for
  0.1.0 (Intel runners deprecated; no consumer demand; revisitable).
  `linux-arm64` — **deferred**, remains a candidate (GH arm runners
  exist; add when a consumer wants it). On operator acceptance of this
  plan, `project-plan.md` §1's candidate list gets the tracked change.
- **D5 — Licensing is a first-class release requirement:** public
  binary redistribution requires a license inventory of what is
  actually compiled in and shipped (upstream components BSD-family, but
  the vendored third-party set includes LGPL/GPL-family code — bliss,
  lpsolve — that arc01 evidence suggests is *not linked* into the
  shipped binaries; that suggestion must be verified, not assumed). A
  `THIRD-PARTY-LICENSES` asset ships with every release.

## 3. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `release-workflow` | The machinery: build logic refactored for reuse (`workflow_call`), tag-triggered `release.yml`, `package-release.sh` (tarballs + `SHA256SUMS` + aggregated manifest), release-notes template with provenance + compat floors, refuse-to-publish proven with a deliberately-broken test tag, CDC-5 action bumps taken during the workflow touch. | slice02, arc close |
| slice02 | `release-readiness` | The judgment half: license/linkage audit (what is actually in the binaries), `THIRD-PARTY-LICENSES`, README "Install from release" section (project P4's ≤5-command path, both platforms), D4's project-plan tracked change, fallback re-evaluation (retire-into-releases), then **cut `v0.1.0` for real** — the arc's composition evidence. | arc close; wolong arc03 |

Sizing judgment: slice01 is machinery over proven ground (arc01's
scripts + one new packaging script + workflow surgery) — one context
with headroom, iteration budget expected to go to release-workflow
mechanics (tag events, token permissions, draft semantics). slice02 is
smaller in code but carries the audit judgment and the real release cut;
separating it keeps license diligence from being rushed by workflow
debugging.

## 4. Dependencies

**Consumes:** arc01's entire substrate unchanged (scripts, gates,
provenance enforcement, both platform legs); the bubble-up inputs
(min_os datum, candidate questions, CDC-5, fallback re-evaluation).

**Leaves for arc03 (slot) / wolong:** stable release-asset URLs and
naming (`pandapi-<tag>-<platform>.tar.gz`), `SHA256SUMS`, and the
manifest format — wolong arc03's fetch-and-verify consumes exactly
these; the formats are frozen at v0.1.0 and versioned thereafter.

## 5. Open questions

- **RQ1 (slice01):** release event model — publish on tag push directly,
  or tag → draft → operator promotes? Default: direct publish (the gates
  are the approval); draft mode documented as a one-line workflow toggle.
- **RQ2 (slice02):** the license/linkage audit's outcome decides whether
  any LGPL component is actually shipped; if one is, the remediation
  (dynamic-link exception documentation, source-offer note, or build
  change) is planned as an amendment to this arc — not improvised.

## 6. Arc ledger

Composition rows open here; close (per-row walk) in this arc's
`closing-report.md`. Class-(a) rows accrue as slices close.

| Row | Class | Criterion | Target strength |
|-----|-------|-----------|-----------------|
| R-1 | a | slice01 closed cleanly. | attested (ptr to cdc-verification) |
| R-2 | a | slice02 closed cleanly. | attested (ptr to cdc-verification) |
| R-3 | b | A real tag (`v0.1.0`) produced a GitHub Release whose assets are exactly: two platform tarballs, `SHA256SUMS`, the manifest, `THIRD-PARTY-LICENSES`; downloaded on clean machines (one per platform), checksums verify, and the binaries pass the runbook's §6-style gate sequence locally. | reproduced at arc scale |
| R-4 | b | Refuse-to-publish demonstrated: a deliberately-broken build under a test tag produced a red run and NO release (linked run + absence evidence). | reproduced |
| R-5 | b | The manifest reconciles: upstream SHAs = `pins.env`, per-platform patch lists exact, compiler lines present, and each tarball's embedded `provenance.txt` agrees with the manifest. | reproduced |
| R-6 | c | Every slice bubble-up finding dispositioned via this plan's change log. | per finding |

## 7. Version history

- **v1.0 — 2026-08-06.** Initial detailed plan, written at arc01 close.
  Sources: project-plan v1.1 roadmap line; arc01 closing-report bubble-up
  (min_os datum → D2; candidates → D4; CDC-5 + fallback re-evaluation →
  slice scopes). Decisions D1–D5 proposed for operator review with this
  document.
