# chengdu — project plan

> **chengdu**: CI build matrix and binary releases for the PANDA (pandaPI) HTN
> planning toolchain — Linux + macOS. *(Named for the Chengdu Research Base of
> Giant Panda Breeding: the facility that produces pandas.)*
>
> Plan-of-record at project scale, per the collaboration framework's
> `PROJECT-MANAGEMENT.md` (v2.1). Design substrate: **"PANDA Runbook — Setup,
> Validation, and Execution"** and **"Planner Toolchain Selection — HDDL,
> pandaPI, and the JSON Bridge"** (both: Dropbox, Duncan's Research/LLMs).
> This project mechanizes the runbook; a separate design doc is deliberately
> not written — for a CI project this size, the runbook *is* the build spec,
> and this plan carries the architecture. (Disclosed judgment, not an
> oversight; revisit if arc02 reveals design weight the plan can't carry.)

## 1. Definition of done, and boundaries

**Done means:** a git tag on this repository produces a GitHub Release
carrying `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` binaries for
every platform in the support matrix, where every published binary set has
passed the full gate sequence — parse → ground → solve → convert → **verify**
— on its own platform before publication, with SHA256 checksums and a
provenance manifest (upstream commit SHAs + the applied patch list) attached
to the release.

**Support matrix at 0.1.0:** `linux-x86_64`, `macos-arm64`.
Candidates, per arc02 D4 (2026-08-06): `linux-arm64` **deferred** — GH
ARM runners exist, remains a candidate, add when a consumer wants it.
`macos-x86_64` **declined** for 0.1.0 — GH Intel runners deprecated, no
consumer demand; revisitable if that changes.

**Explicit non-goals (0.1.0):**

- **Not a development fork of pandaPI.** Upstream is consumed at pinned SHAs;
  every delta lives as an explicit, named patch file in `patches/`.
  Upstreamable fixes (the macOS set is a clear candidate) get contribution
  tickets per `CONTRIBUTION-STYLE.md` — never silent divergence.
- **No Windows.**
- **No package-manager distribution** (Homebrew tap, containers, install
  scripts beyond the README) — conceivable arc03, deliberately unplanned.
- **No Aries, no SHOP3** — pandaPI only, per the toolchain-selection decision
  record.
- **No HDDL tooling of our own** — the JSON bridge, linter, and corpus suite
  belong to the Lykn bridge project, not here. chengdu ships planners.

**Primary consumer:** `wolong` (the LFE/OTP supervision app), which fetches
release binaries rather than building from source. The DoD's install-path row
(P4) is written from wolong's point of view.

## 2. Arc roadmap

| Arc | Slug | Capability (one line) | Depends on |
|-----|------|----------------------|------------|
| arc01 | `build-matrix` | Every push produces smoke-verified binaries for the full support matrix, as workflow artifacts. | — |
| arc02 | `release-publish` | A git tag turns verified artifacts into a GitHub Release with checksums and a provenance manifest. | arc01 |
| (arc03) | `distribution` | *Conceivable, unplanned:* Homebrew tap / container image / install script. | arc02 |

Load-bearing note: arc01's build scripts are written to run identically
locally and in CI — they are the runbook, mechanized — so arc02 publishes
what arc01 already proved, adding no new build logic.

## 3. Current status

- **arc01 — closed** (pending operator gate on
  `arc01-build-matrix/closing-report.md`, assembled 2026-08-06): all three
  slices delivered and CDC-verified; 4/4 arc-ledger composition rows
  reproduced; capability demonstrated by run `31065314655` (8-job
  two-platform matrix, one push, green). *(Was: active.)*
- **arc02 — closed** (pending operator gate on
  `arc02-release-publish/closing-report.md`, assembled 2026-08-06): both
  slices delivered and CDC-verified; 6/6 arc-ledger rows closed; the
  capability demonstrated by the real, public `v0.1.0` release, consumed
  independently end-to-end. *(Was: next for detailed planning; arc01's
  gate was subsequently recorded GO 2026-08-06.)*
- **arc03 — a slot, not a plan.**
- **PROJECT CLOSE IS RIPE:** all roadmap arcs closed; ledger rows P1–P4
  each have standing evidence (arc02 closing-report §6). Awaiting the
  operator convening the project close.

## 4. Project ledger

Composition rows verifying the DoD; they open here and close (per-row walk)
in this project's `closing-report.md`. Strength vocabulary per
`LEDGER-DISCIPLINE.md` (`asserted < attested < reproduced < reconciled`).

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| P1 | Downloading the newest release's assets yields all three binaries for every support-matrix platform, plus a `SHA256SUMS` that verifies against them. | reproduced |
| P2 | The release workflow demonstrably refuses to publish when the smoke gate fails (evidence: one deliberately-broken run, linked). | reproduced |
| P3 | Every release's provenance manifest names the three upstream commit SHAs and every applied patch, and the manifest matches what the build actually used. | reproduced at project close |
| P4 | A consumer following the README on a clean machine (per platform) reaches a working `pandaPIparser --verify` from release assets in ≤ 5 commands. | reproduced |

## 5. Version history

- **v1.3 — 2026-08-06.** Status: arc02 closed (gate pending at write
  time); project close declared ripe with P1–P4 evidence standing.
  Surfaced by: arc02 (arc close). Why: the project's own close is now
  the remaining work; the plan should say so.
- **v1.2 — 2026-08-06.** §1's candidate list enacted per arc02 D4:
  `macos-x86_64` declined for 0.1.0 (GH Intel runners deprecated, no
  consumer demand); `linux-arm64` deferred (stays a candidate). Surfaced
  by: arc02 slice02 (`release-readiness`), per arc-plan D4. Why: D4 was
  proposed at arc02 planning and enacted here once slice02 reached the
  point in its dependency order where the plan-change discipline calls
  for it — no scope or DoD change, a candidate-list update only.
- **v1.1 — 2026-08-06.** Status update: arc01 closed (see its
  closing-report; operator gate pending at write time), arc02 promoted to
  next-for-detailed-planning with its bubble-up inputs listed. Surfaced
  by: arc01 (arc close). Why: the roadmap's status must reflect the close
  before arc02 plans against it. No scope, DoD, or sequence change.
- **v1.0 — 2026-08-05.** Initial roadmap. Source: the PANDA toolchain working
  session of 2026-08-05 — Linux build + full gate sequence reproduced in a
  clean sandbox; three macOS build failures found and fixed live (boruvka
  POSIX gaps, bliss literal-pasting, hardcoded `g++-11`); runbook written and
  field-tested. No child bubble-ups yet.
