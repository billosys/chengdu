# chengdu arc01 — build-matrix — arc plan

> Plan-of-record for arc01, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md).

## 1. Capability statement

Roadmap line: *Every push produces smoke-verified binaries for the full
support matrix, as workflow artifacts.*

Expanded: this arc turns the PANDA Runbook from a document a human executes
into scripts a machine executes, and puts those scripts under a GitHub
Actions matrix covering `linux-x86_64` and `macos-arm64`. At arc close, any
push to chengdu builds all three pandaPI components from pinned upstream
SHAs, applies the patch set, runs the full verified gate sequence (parse →
ground → solve → convert → verify, including the `Status:`-line trap for
proven-unsolvable), and uploads the binaries as workflow artifacts — on both
platforms, every time. The scripts run identically on a developer laptop and
a runner: CI is a caller, not a second implementation.

## 2. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `substrate` | Repo skeleton: pinned upstream SHAs + fetch script, `patches/` (the three macOS patches as named `.patch` files + application script), `scripts/build-*.sh` encoding runbook §4 per component, `scripts/smoke-test.sh` encoding runbook §5–§6 gates with correct exit-code/`Status:`-line semantics. Runnable locally on both OSes. | slice02, slice03 |
| slice02 | `linux-workflow` | `.github/workflows/build.yml` Linux job: deps via apt, calls the slice01 scripts, uploads artifacts, smoke gate green required. | slice03 (workflow skeleton), arc02 |
| slice03 | `macos-workflow` | macOS (arm64) job in the same workflow: brew deps, patch application, grounder compiler override, smoke gate, artifacts. | arc02 |

Sizing judgment: each slice is one context with ample headroom — slice01 is
the largest (the runbook is already its spec, so it is transcription +
hardening, not design); slices 02/03 are each a single workflow job plus
debugging margin for runner quirks (macOS runner reality is the main
unknown; that risk is why 02 and 03 are separate slices rather than one
"CI" slice).

## 3. Dependencies

**Consumes:** the PANDA Runbook (build commands: Linux path *reproduced*
2026-08-05, macOS path field-tested same day); the pinned upstream SHAs from
the runbook's provenance appendix (parser `88c0995`, grounder `b04ff7b`,
engine `810f043`) as the initial pins; the IPC 2023 Transport domain
(`pfile01`) as smoke-test input.

**Leaves for arc02:** per-platform verified artifacts with stable names; the
smoke-gate exit contract (scripts exit nonzero on any gate failure) that the
release workflow will rely on; the provenance data (SHAs + patch list)
emitted as a build byproduct so arc02 can package it into the manifest.

## 4. Open questions (named, owned by slices)

- **OQ1 — RESOLVED 2026-08-05 (v1.3):** upstream source strategy — canonical
  three repos at pinned SHAs vs. the `ipc2023-htn/PandaDealer` vendored
  snapshot. **Reachability half:** slice02's CI fetch step cloned
  `cpddl` from `gitlab.com/danfis/cpddl` cleanly on both `ubuntu-22.04`
  and `ubuntu-24.04` GH-hosted runners (linked run:
  https://github.com/oubiwann/chengdu/actions/runs/31060179607) — GH
  runners reach gitlab.com; no longer merely *asserted*. **Fallback
  disposition (CDC-1, held open at v1.2):** resolved as **(a) fetch-only**
  — `fetch-upstream.sh --help` and README document the limit;
  `build-grounder.sh` detects the PandaDealer symlink layout and fails
  fast with a pointer to that documentation (slice02 W-8). Compat patches
  for the snapshot (disposition b) are explicitly not built (low value:
  canonical works everywhere a runner runs); retiring the fallback into
  chengdu's own releases (disposition c) stays open for re-evaluation at
  arc02 close.
- **OQ2 (slice03) — RESOLVED 2026-08-05:** grounder compiler on macOS is
  **clang** (`make -j CXX=c++ CC=cc`), confirmed by the operator's successful
  field build on Apple Silicon. The build script still takes the compiler as
  a parameter (brew gcc remains a documented fallback), but clang is the
  default and the CI configuration. *(Was: open pending the operator's
  macOS build result.)*
- **OQ3 — RESOLVED 2026-08-05 (v1.3):** glibc baseline for released Linux
  binaries is **`ubuntu-22.04`** (glibc 2.35), confirmed forward-compatible:
  the 22.04-built artifact set's three binaries answer `--help` cleanly
  when run under an `ubuntu-24.04` job too (linked run, `cross-compat`
  job: https://github.com/oubiwann/chengdu/actions/runs/31060179607).
  `ubuntu-24.04` stays in the matrix as the forward-compat check; arc02
  packages only the 22.04 artifact set.

## 5. Arc ledger

Composition rows verifying the capability; open here, close (per-row walk)
in this arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | One workflow run, linked, green on both platforms, where each job's log ends with the smoke gate's PASS summary (all five gates). | reproduced |
| A2 | The artifacts downloaded from that run contain all three executables per platform, and each answers `--help` on its platform. | reproduced |
| A3 | `patches/` applies cleanly against the pinned SHAs from a fresh fetch — proven implicitly by every CI run, checked explicitly once. | reproduced |
| A4 | The smoke script encodes every runbook gate faithfully — including exit-code semantics (parser 0/2/255, verifier 0/1/2) and the engine `Status:`-line trap (`Proven unsolvable` exits 0). | attested by independent review against runbook §5–§6 |

## 6. Version history

- **v1.3 — 2026-08-05.** OQ1 and OQ3 resolved by slice02's implementation
  and CI evidence. OQ1: GH runners empirically reach gitlab.com
  (reachability half); the PandaDealer fallback's disposition is (a)
  fetch-only, implemented in `fetch-upstream.sh`/`build-grounder.sh`/
  README. OQ3: `ubuntu-22.04` is the glibc baseline for released
  binaries, confirmed forward-compatible with `ubuntu-24.04` via a
  dedicated `cross-compat` CI job. Surfaced by: slice02 (CC
  implementation + linked CI run). Why: both were named open questions
  this slice was assigned to close; closing them here (rather than
  leaving them implicit in slice02's ledger alone) keeps the arc-plan the
  single source of truth for the arc's open-question state.
- **v1.2 — 2026-08-05.** OQ1 amended with finding CDC-1 from slice01's CDC
  verification: the PandaDealer fallback is not build-viable on
  Linux/GCC-13 (three isolated blockers); disposition decision assigned to
  slice02 planning. Surfaced by: slice01 (CDC pass). Why: the fallback's
  implicit build promise was untested until the verifier ran it; slice02
  must not plan against the old assumption.
- **v1.1 — 2026-08-05.** OQ2 resolved: macOS grounder compiler is clang
  (`CXX=c++ CC=cc`), per the operator's successful field build. Surfaced by:
  operator report during plan review (pre-slice; no slice number). Why:
  removes slice03's largest unknown before execution.
- **v1.0 — 2026-08-05.** Initial slice breakdown. Source: same working
  session as the project plan; no slice bubble-ups yet. OQ2 held open
  pending the operator's macOS grounder result.
