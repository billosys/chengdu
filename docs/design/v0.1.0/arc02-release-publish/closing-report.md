# chengdu arc02 — release-publish — arc closing report

> Arc-level close per `PROJECT-MANAGEMENT.md` Part V and
> `LEDGER-DISCIPLINE.md` Section B. Assembled by CDC (Cowork sandbox
> session, 2026-08-06). **Independence at the arc gate: the composer
> does not sign off — the operator is the gatekeeper.** Gate decision
> recorded at the end.

## 1. The capability, restated — and the verdict

From `arc-plan.md` (v1.0 + amendments): *A git tag turns verified
artifacts into a GitHub Release with checksums and a provenance
manifest* — expanded: identical build+gate logic reused (not
duplicated), per-platform tarballs, `SHA256SUMS`, aggregated provenance
manifest, human-readable notes with compat floors, license obligations
satisfied, and a demonstrated refusal to publish on any gate failure.

**Verdict: delivered.** The evidence is a real, public, full release —
`v0.1.0`, five assets — whose consumer path CDC walked independently
from download to a passing plan verification, plus a permanently
viewable refusal run with confirmed release absence.

## 2. The slice walk

Arc-plan breakdown: 2 slices. Delivered: 2. **Count check: match.**

| Slice | Outcome | Close evidence |
|-------|---------|----------------|
| slice01 `release-workflow` | **Delivered.** Reusable-workflow refactor (zero CI behavior change), tag-triggered release machinery, packaging + checksums + manifest + notes template, refusal demonstrated, CDC-5 retired, rc1 rehearsal published. | `slice01-release-workflow/closing-report.md` + `cdc-verification.md` (accepted 2026-08-06) |
| slice02 `release-readiness` | **Delivered.** Evidence-based license/linkage audit (central finding: h2-fd-preprocessor GPL-3.0, grounder obligated, others mere-aggregation; bliss/lpsolve confirmed not linked), `THIRD-PARTY-LICENSES`, fixtures-in-tarball, 4-command install docs, D4 + fallback dispositions, and the real `v0.1.0` cut — after a rehearsal caught a would-have-shipped missing-asset bug. | `slice02-release-readiness/closing-report.md` + `cdc-verification.md` (this close) |

## 3. The composition check — arc ledger, per-row walk

**R-1 — slice01 closed. done, attested** (pointer above; operator-
accepted).

**R-2 — slice02 closed. done, attested** (pointer above; CDC-verified
9/9, operator gate with this arc close).

**R-3 — a real tag produced a real release, consumed on clean machines.
done, REPRODUCED at arc scale.** CDC, acting as a genuine independent
consumer, downloaded `v0.1.0`'s assets from the public release page,
verified `SHA256SUMS`, extracted the Linux tarball, and ran the full
five-gate chain on the shipped fixtures to `Plan verification result:
true` — using only the README's four commands. The macOS-side walk: CC's
transcript (attested), operator's machine available for a repeat at
will. Asset set exact: 2 tarballs (3 binaries + provenance + fixtures
each), `SHA256SUMS`, `release-manifest.txt`, `THIRD-PARTY-LICENSES`.

**R-4 — refuse-to-publish. done, REPRODUCED.** The deliberately-broken
`v0.0.0-rc-refusal-test` run: red at fetch on every leg, `publish`
never started, release absence confirmed against the releases list —
all independently fetched. Additionally hardened by slice02's
missing-asset presence check, itself born from a rehearsal that caught
the gap the hard way (the safe way).

**R-5 — manifest reconciles. done, REPRODUCED.** `release-manifest.txt`
SHAs = `pins.env` (checked at slice01's packaging reproduction and
again against the real release); per-platform patch lists exact (2
Linux / 4 macOS, rendered correctly in the notes after slice01's
`wc → awk` fix); embedded per-tarball `provenance.txt` agrees — and the
whole property is CI-enforced by `check-provenance.sh` on every leg of
every run, so R-5 is not a point-in-time check but a standing
invariant.

**R-6 — bubble-up findings dispositioned. done.** Slice01's asset-set
growth warning → planned into slice02's scope (fixtures +
THIRD-PARTY-LICENSES, batched before the freeze). Slice02's RQ2
amendment (GPL finding + documentation-based compliance) → recorded in
the arc-plan change log. The timeout fix → landed independently with
its own disclosure trail. Nothing undispositioned.

**Arc ledger closure: 6/6 rows done. No deferrals, no no-ops.**

## 4. Accumulated arc-plan change log

- **RQ1** resolved by implementation (direct publish; draft toggle
  documented).
- **RQ2** resolved by the audit + tracked amendment: h2-fd-preprocessor
  GPL-3.0 finding, documentation-based compliance adopted, no build
  change.
- **D4** enacted at slice02 (project-plan v1.2).
- Fallback re-evaluation dispositioned (code kept fetch-only, docs
  repointed to releases).

## 5. What recurred / what worked

**Worked:** the rehearsal-before-the-one-way-door discipline caught a
would-have-shipped bug (the missing `THIRD-PARTY-LICENSES` upload) at
zero cost; the evidence-pointer rule made the license audit
independently checkable in minutes rather than arguable for days; the
freeze-with-disclosed-growth pattern gave wolong a stable target
without premature rigidity. **Recurred:** stale logs (GitHub retention)
forced a fresh evidence run — a reminder that link-line evidence is
perishable and audits should trigger their own runs, which the audit
did; the verifier's-own-tooling error class continued its streak of
being this project's only recurrent defect source.

## 6. Bubble-up to the project

**1. Did this arc deliver its capability as `project-plan.md` defined
it?** Yes — the roadmap line verbatim, plus the license obligations the
roadmap didn't know it needed (D5's addition at arc planning, vindicated
by the audit's central finding).

**2. What did this arc reveal that the project plan did not
anticipate?** One thing of substance: **the project's DoD is now fully
evidenced, and project close is ripe.** P1 (release assets complete,
checksums verify — walked), P2 (refusal demonstrated — reproduced),
P3 (manifest matches build — CI-enforced), P4 (clean-machine README
path to a working verify in ≤5 commands — 4, walked on Linux by CDC,
macOS by CC). The remaining work of *the project* is the project-close
ceremony itself: the P-row walk, the operator's go/adjust/kill, and a
project `closing-report.md`. Also carried forward: the
upstream-courtesy items (the three macOS fixes; now plus the h2
license-file gap) remain open as contribution-ticket candidates —
they belong to no arc and should not be silently dropped at project
close.

**3. Silent-drop diff at arc scale:** nothing the roadmap expected from
this arc failed to land. arc03 remains what it always was: an
explicitly unplanned slot, not a drop.

**Project-plan change:** status updated (arc02 closed pending this
gate; project close ripe) — v1.3, tracked.

## 7. Arc gate

Per Section B, the composer does not sign off. **Gate decision: GO —
accepted by operator, 2026-08-06.** *(Was: pending at assembly; the
composer's recommendation was go.)* Arc02 is closed. The **project
close** for chengdu 0.1.0 is ripe and awaits the operator's convening —
the P-row walk is ready the moment you are.
