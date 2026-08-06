# chengdu arc01 — build-matrix — arc closing report

> Arc-level close per `PROJECT-MANAGEMENT.md` Part V and
> `LEDGER-DISCIPLINE.md` Section B. Assembled by CDC (Cowork sandbox
> session, 2026-08-06), who ran all three per-slice verifications and
> therefore knows the arc. **Independence at the arc gate: the composer
> does not sign off — the operator is the arc gatekeeper.** Gate
> decision recorded at the end of this document.

## 1. The capability, restated — and the verdict

From `arc-plan.md` (v1.3): *"Every push produces smoke-verified binaries
for the full support matrix, as workflow artifacts"* — expanded: any push
builds all three pandaPI components from pinned upstream SHAs, applies
the patch set, runs the full verified gate sequence (parse → ground →
solve → convert → verify, including the proven-unsolvable trap), and
uploads the binaries as workflow artifacts, on both `linux-x86_64` and
`macos-arm64`, every time, with CI as a caller of locally-runnable
scripts rather than a second implementation.

**Verdict: delivered.** Evidence: run `31065314655` — one push, the
complete 8-job matrix green, three artifacts (2 Linux + 1 macOS), every
build leg gated by smoke (positive + negative) and by the mechanical
provenance check. Independently fetched and verified by CDC from the
public repo.

## 2. The slice walk

Arc-plan breakdown: 3 slices. Delivered: 3. **Count check: match — no
arc-scale silent drop.**

| Slice | Outcome | Close evidence |
|-------|---------|----------------|
| slice01 `substrate` | **Delivered.** Runbook mechanized: pins, fetch, patches-as-artifacts, three build scripts, five-gate smoke with correct exit-code/`Status:` semantics. Closed conditionally (2 rows environment-blocked on Linux), later discharged in full by slice02's CI run. | `slice01-substrate/closing-report.md` + `cdc-verification.md` (accepted 2026-08-05; conditional rows discharged per slice02 W-4) |
| slice02 `linux-workflow` | **Delivered.** GH Actions matrix (ubuntu-22.04/24.04), readme-verbatim tripwire, actionlint gate, cross-compat job, CDC-3 fix, fetch-only fallback disposition. One residue (provenance attestation), retired by slice03. | `slice02-linux-workflow/closing-report.md` + `cdc-verification.md` (accepted 2026-08-06) |
| slice03 `macos-workflow` | **Delivered.** macOS arm64 leg (macos-15 floor / macos-26 forward-compat), macOS verbatim + cross-compat jobs, `check-provenance.sh` on all legs, min-OS observation. First-push full-matrix green. | `slice03-macos-workflow/closing-report.md` + `cdc-verification.md` (this close) |

## 3. The composition check — arc ledger, per-row walk

Rows opened in `arc-plan.md` §5; walked to closure here, per Section B
(composition rows *reproduced at arc scale*, never inherited).

**A-1 — one linked run, green on both platforms, smoke PASS terminal
lines. CLOSED: done, reproduced.** Run `31065314655`: overall success
independently fetched; all 8 jobs; both platforms; every build leg ends
in the smoke gates (which exit nonzero on any deviation — property
reproduced in-sandbox across three slices) plus the provenance gate.
This is a single arc-scale demonstration, not an inheritance from slice
closes: the run exercises fetch→build→gate→artifact end-to-end on both
platforms in one artifact of evidence.

**A-2 — artifacts contain all three executables per platform, each
answering `--help`. CLOSED: done, reproduced.** The two cross-compat
jobs *execute* all three binaries from each canonical artifact on a
newer runner of the same platform family (green in the verified run);
the build legs' own smoke runs execute them far beyond `--help` on the
build runner. Artifact completeness is enforced by
`if-no-files-found: error` plus the provenance gate; the macOS
artifact's contents additionally spot-verified by CC at slice03 close.

**A-3 — patches apply cleanly against pinned SHAs from a fresh fetch.
CLOSED: done, reproduced.** Every CI run re-fetches from scratch and
applies the patch set (four `applying patch:` events on macOS, two on
Linux, counts verified at slice01/slice03); the explicit one-time check
was performed in slice01's CDC pass. The patch set has been stable since
slice01 — zero patch churn across the arc.

**A-4 — smoke script encodes every runbook gate faithfully, including
exit-code semantics and the `Status:`-line trap. CLOSED: done, attested
by independent review (as targeted), exceeded in practice.** CDC's
slice01 pass walked all four negative outcomes against the runbook
tables and tampered the harness to prove it can fail; slice02's pass
added crash simulations (labeled FAILs, no silent aborts); fixtures are
byte-identical to the runbook's verified pair. The
confident-plan-for-unsolvable firewall is tested on every push, on
every platform.

**Arc ledger closure: 4/4 rows done. No deferrals, no no-ops.**

## 4. Accumulated arc-plan change log

`arc-plan.md` v1.0 → v1.3 over the arc's life — every change dated,
attributed, and tracked (the Version History is the authoritative log):

- **v1.1** — OQ2 (macOS grounder compiler) resolved: clang. Source:
  operator field build, pre-slice.
- **v1.2** — OQ1 amended with CDC-1: PandaDealer fallback not
  build-viable on Linux/GCC-13. Source: slice01 CDC pass. Disposition
  decision assigned to slice02 planning.
- **v1.3** — OQ1 resolved (gitlab reachable from runners; fallback
  declared fetch-only) and OQ3 resolved (ubuntu-22.04 glibc baseline,
  forward-compat proven). Source: slice02 implementation + CI evidence.

A plan whose history grew three times in one arc, each entry naming the
child that surfaced it, is the loop working as designed.

## 5. What recurred / what worked (cross-slice trending)

**Recurred (systemic, worth carrying forward):** (1) *Verifier-side
tooling errors* — pipeline exit-code capture and extraction one-liners
misfired repeatedly across CDC passes (and once in CC's own M-3 check);
countermeasures now standing: no pipes around exit-code capture,
read-the-diff-before-trusting-it, checksum-staged-files-before-judging.
(2) *Evidence-channel unreliability* (stale mount cache) — solved by the
authenticated-clone protocol. Neither recurrence was ever in the work
under review; both were in the reviewing apparatus. (3) *The
leg-crossing lesson* — a matrix proves each leg only on itself;
cross-leg claims need cross-leg jobs. Discovered slice02, applied at
authoring time slice03, now a slice-doc authoring rule.

**Worked:** fail-loud everywhere made green runs evidence-bearing;
gates that must demonstrably be able to fail (smoke tamper, provenance
tamper, crash sims) kept every green honest; the runbook-as-spec made
slice01 transcription rather than invention, and everything downstream
inherited its verified exit-code contracts.

## 6. Bubble-up to the project

**1. Did this arc deliver its capability as `project-plan.md` defined
it?** Yes. Roadmap line: *"Every push produces smoke-verified binaries
for the full support matrix, as workflow artifacts"* — demonstrated, at
the roadmap's support matrix (`linux-x86_64`, `macos-arm64`), with the
load-bearing note honored (scripts run identically locally and in CI;
arc02 publishes what arc01 proves, adding no build logic).

**2. What did this arc reveal that the project plan did not
anticipate?** Nothing that changes the roadmap, DoD, or arc sequence.
Three inputs flow to arc02's planning, none structural: (a) the macOS
compat floor question now has data — `min_os=15.0` as built on
`macos-15`; (b) `macos-x86_64` looks decline-shaped (Intel runners
deprecated; macos-26-intel exists but the candidate stays a candidate —
arc02 decides); (c) housekeeping inherited: CDC-5 action-version bumps,
and the fallback's planned re-evaluation (retire-into-releases) at
arc02 close. Project ledger note: rows P1–P4 remain open (they close at
project close), but P2's mechanism (a gate that refuses to publish)
now has its building blocks proven, and P4's per-platform README paths
are already CI-enforced by the two readme-verbatim jobs.

**3. Silent-drop diff at arc scale:** nothing the roadmap expected from
this arc failed to land. The support matrix's two committed platforms
are both delivered; the two candidates (`linux-arm64`, `macos-x86_64`)
were never this arc's scope and remain candidates.

**Project-plan change:** status section updated (arc01 closed → arc02
active-for-planning) with a v1.1 Version History entry — a status
change, not a scope change.

## 7. Arc gate

Per Section B, the gate reviewer must be independent of the composer.
CDC assembled this close; the **operator** gates it.

**Gate decision: PENDING — awaiting operator review of this document.**
The recommendation from the composer's seat is *go*: capability
delivered, 4/4 composition rows closed with reproduced evidence, all
three slices closed and verified, change log complete, nothing
structural bubbling up. On operator acceptance, arc01 closes and arc02
(`release-publish`) detailed planning begins — *plan late, plan deep*
now makes it "near." On adjust/kill, the operator's direction is
recorded here and remediation is planned per Section B.
