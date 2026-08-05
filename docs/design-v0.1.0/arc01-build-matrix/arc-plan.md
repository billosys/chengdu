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

- **OQ1 (slice01 → slice02):** upstream source strategy — canonical three
  repos at pinned SHAs (requires gitlab.com reachability for cpddl; GH
  runners can reach it, *asserted*) vs. the `ipc2023-htn/PandaDealer`
  vendored snapshot. Default: canonical + pinned SHAs, PandaDealer
  documented as fallback; **slice02** verifies gitlab reachability from a
  runner and records the result (deferred from slice01, where no runner
  existed). **Amended at v1.2 (CDC-1):** the fallback is fetch-verified but
  NOT build-viable on Linux/GCC-13 — the vendored snapshot ships no patch
  files, its grounder src uses pre-final concepts-TS syntax GCC-13 rejects,
  and its engine lacks `<cstdint>`. Decision owed at slice02 planning:
  (a) declare the fallback fetch-only and document the limit, (b) carry
  compat patches for the snapshot in `patches/`, or (c) retire the fallback
  once chengdu's own releases exist (self-solving after arc02).
- **OQ2 (slice03) — RESOLVED 2026-08-05:** grounder compiler on macOS is
  **clang** (`make -j CXX=c++ CC=cc`), confirmed by the operator's successful
  field build on Apple Silicon. The build script still takes the compiler as
  a parameter (brew gcc remains a documented fallback), but clang is the
  default and the CI configuration. *(Was: open pending the operator's
  macOS build result.)*
- **OQ3 (slice02):** glibc baseline for released Linux binaries — build on
  `ubuntu-latest` (newest) vs. oldest-supported LTS for compatibility.
  Parser/grounder link `-static` already; the engine does not. Decide in
  slice02; leans oldest-LTS.

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
