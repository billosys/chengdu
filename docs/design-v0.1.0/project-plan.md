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
Candidates recorded, not committed: `linux-arm64` (GH ARM runners exist;
build unverified), `macos-x86_64` (GH Intel runners deprecated — decide in
arc02, may be declined).

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

- **arc01 — active.** `arc01-build-matrix/arc-plan.md` written 2026-08-05.
- **arc02 — named only**, per *plan late, plan deep*. To be planned in detail
  when arc01 nears close.
- **arc03 — a slot, not a plan.**

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

- **v1.0 — 2026-08-05.** Initial roadmap. Source: the PANDA toolchain working
  session of 2026-08-05 — Linux build + full gate sequence reproduced in a
  clean sandbox; three macOS build failures found and fixed live (boruvka
  POSIX gaps, bliss literal-pasting, hardcoded `g++-11`); runbook written and
  field-tested. No child bubble-ups yet.
