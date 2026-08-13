# chengdu 0.1.0 — project closing report

> Project-level close per `PROJECT-MANAGEMENT.md` Part V and
> `LEDGER-DISCIPLINE.md` Section C (provisional tier). Assembled by the
> Cowork session of 2026-08-06 (evening), which also independently
> re-reproduced the reproducible P-rows in a fresh cloud sandbox that had
> never built or touched this project's artifacts. **Independence at the
> project gate: the assembler does not sign off — the operator is the
> gatekeeper.** Convened by the operator 2026-08-06 ("let's officially
> close 0.1.0"); gate decision recorded in §6.

## 1. Definition of done, restated — and the verdict

From `project-plan.md` §1: *a git tag on this repository produces a
GitHub Release carrying `pandaPIparser`, `pandaPIgrounder`, and
`pandaPIengine` binaries for every platform in the support matrix
(`linux-x86_64`, `macos-arm64`), where every published binary set has
passed the full gate sequence — parse → ground → solve → convert →
verify — on its own platform before publication, with SHA256 checksums
and a provenance manifest (upstream commit SHAs + the applied patch
list) attached to the release.*

**Verdict: met.** The public `v0.1.0` release is the standing
demonstration, and every DoD clause was re-verified at close (per-row
walk, §3).

## 2. The arc walk

Roadmap: 2 planned arcs + 1 explicitly unplanned slot. Closed: 2.
**Count check: match.**

| Arc | Outcome | Close evidence |
|-----|---------|----------------|
| arc01 `build-matrix` | **Delivered.** 3 slices, all CDC-verified; 4/4 arc-ledger composition rows reproduced; capability demonstrated by run `31065314655` (8-job two-platform matrix, one push, green). Operator gate GO 2026-08-06. | `arc01-build-matrix/closing-report.md` |
| arc02 `release-publish` | **Delivered.** 2 slices, all CDC-verified; 6/6 arc-ledger rows closed; capability demonstrated by the real public `v0.1.0` release, independently consumed end-to-end; refusal-to-publish demonstrated on a deliberately broken run. Operator gate GO 2026-08-06. | `arc02-release-publish/closing-report.md` |
| (arc03) `distribution` | **Not a drop.** An explicitly unplanned slot from the roadmap's first version; never promised for 0.1.0. Remains conceivable, unplanned. | `project-plan.md` §2 |

## 3. The composition check — project ledger, per-row walk

Rows opened in `project-plan.md` §4. Strength vocabulary per
`LEDGER-DISCIPLINE.md` (`asserted < attested < reproduced <
reconciled`). Rows: 4 opened, 4 walked. **No silent drops.**

**P1 — newest release yields all three binaries per platform + a
verifying `SHA256SUMS`. done, REPRODUCED at project scale
(2026-08-06, this close).** Fresh-sandbox re-run: both `v0.1.0`
tarballs plus `SHA256SUMS` and `release-manifest.txt` downloaded from
the public release; `sha256sum -c SHA256SUMS --ignore-missing` → OK for
every asset; both tarballs extracted; each contains exactly
`pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, `provenance.txt`,
`fixtures/` (macOS binaries confirmed Mach-O arm64 by inspection —
execution of the macOS set was reproduced on-platform at arc02 close,
not re-run here; disclosed, not implied).

**P2 — release workflow demonstrably refuses to publish when the smoke
gate fails. done, REPRODUCED (arc02 R-4), spot-checked at this
close.** Standing evidence: the deliberately-broken
`v0.0.0-rc-refusal-test` run — red at fetch on every leg, `publish`
never started, release absence confirmed. Re-checked today: the public
releases list carries no release for any refusal/rc-test tag.

**P3 — provenance manifest names the three upstream SHAs and every
applied patch, and matches what the build actually used. done,
REPRODUCED at project close (the row's stated target).** Fresh-sandbox
comparison: `release-manifest.txt` SHAs ≡ `pins.env`
(parser `88c0995c…`, grounder `b04ff7bc…`, engine `810f0438…`);
per-platform patch lists exact (Linux grounder: `0002-makefile.patch` +
`bliss-0.73-cxx11-string-literal.patch`; macOS grounder: those plus the
two boruvka/macOS patches); per-tarball `provenance.txt` agrees with
the aggregated manifest. The property is additionally CI-enforced by
`check-provenance.sh` on every leg of every run — standing invariant,
not point-in-time (strength note: the CI enforcement makes this row
effectively *reconciled*).

**P4 — a consumer following the README on a clean machine reaches a
working verify from release assets in ≤ 5 commands. done, REPRODUCED
at project scale (2026-08-06, this close).** Fresh sandbox (no build
tools invoked, no prior state): the README's Linux path verbatim —
download, checksum-verify, extract, run the five-gate chain — 4
commands to `Plan verification result: true`, exit 0. macOS leg:
walked at arc02 close (CC transcript + operator machine), attested
here, not re-run — disclosed.

**Project ledger closure: 4/4 rows done. No deferrals, no no-ops.**

## 4. Findings dispositioned (class c) and the carried-forward register

Arc bubble-ups routed during the project (per arc closing-reports and
`project-plan.md`'s version history v1.1–v1.3): all dispositioned at
the time — D4's candidate-list enactment (v1.2), the fallback's
fetch-only demotion, the RQ2 GPL finding (license audit + tracked
amendment). Nothing undispositioned remains.

Carried forward **out of** this project — named here so the close drops
nothing silently:

1. **Upstream courtesy items** — the three macOS build fixes and the
   h2-fd-preprocessor missing-license-file gap, held as
   contribution-ticket candidates since arc close. **Re-routed by the
   0.2.0 fork decision (operator, 2026-08-06):** the fixes land as
   in-tree commits when `patches/` dissolves at 0.2.0 arc01; upstream
   tickets are downgraded from obligation to courtesy and tracked in
   `docs/design-v0.2.0/project-plan.md` alongside the fork-announcement
   notices. Not dropped — re-homed, with rationale.
2. **`linux-arm64`** — remains a deferred candidate, carried to the
   0.2.0 candidate list unchanged.
3. **wolong's frozen consumer contract** — asset naming, `SHA256SUMS`,
   `release-manifest.txt` shape, 4-command install path. A standing
   contract on every future release, restated as a constraint in the
   0.2.0 plan.

## 5. What worked / what recurred (project-scale trending)

**Worked, across both arcs:** scripts-as-runbook (local ≡ CI) meant
arc02 published what arc01 had already proved, and the project close
could re-reproduce the consumer path in minutes; the
rehearsal-before-one-way-door discipline caught a would-have-shipped
release bug at zero cost; ledger rows written as *behaviors with
evidence pointers* (refusal runs, link-line audits) stayed checkable
months-of-context later. **Recurred:** the only recurrent defect class
project-wide was verifier's-own-tooling errors (smoke-test bracketing,
`wc → awk`, stale-log perishability) — a useful prior for 0.2.0: budget
review attention on the *verification machinery itself*, not only the
verified thing.

## 6. Project gate

Per Section C the close is a stage gate: go / adjust / kill against the
DoD, decided by the operator.

**Gate decision: GO — operator, 2026-08-06** (convened and accepted in
the working session of that date; the operator directed the close with
all four P-rows evidenced, P1/P3/P4 freshly re-reproduced at project
scale, P2 spot-checked against the live releases list).

**chengdu 0.1.0 is closed.** Successor: `docs/design-v0.2.0/`
(pandaPI vendored in-tree; opened same date), which formally supersedes
this design version's "not a development fork" non-goal — recorded
there, tracked, never silent.
