# chengdu arc01 / slice03 — macos-workflow — CDC verification

> Independent verification per `LEDGER-DISCIPLINE.md` Section A (CDC
> protocol). Verifier: CDC (Cowork sandbox session), 2026-08-06.
> Inputs authenticated via a fresh clone of the public repo at
> `92b5459` (HEAD checksums matched against the operator's working tree
> file-by-file before use — the checksum-before-judging protocol from
> slice02's CDC-4, now standing procedure). Run `31065314655` fetched
> independently from the public repo.
>
> **Row count check: 8 opened, 8 dispositioned. No silent drops.**

## Per-row walk

**M-1 — pinned macOS leg, lints, brew line verbatim. REPRODUCED.**
CC's runner survey independently re-fetched from
`actions/runner-images`: `macos-15` and `macos-26` are the GA arm64
images, `macos-14` deprecated, `xcode-27` preview — CC's claims
corroborated exactly, and the oldest-maintained-GA choice mirrors the
OQ3 logic as specified. `actionlint` 1.7.12 locally: zero findings. The
brew step byte-matches the README macOS block (independent extraction).
No `macos-latest` anywhere in the workflow.

**M-2 — macOS leg green end-to-end. REPRODUCED at job level.** Run
`31065314655` independently fetched: overall success, `main`, commit
`fd4ae6a`, `dist-macos-arm64-macos-15-fd4ae6a` (1.09 MB) present
alongside both Linux artifacts. Step-level SUMMARY lines: attested +
the green-plus-fail-loud inference (unchanged since slice02 W-3, and
now stronger — the provenance gate adds another step that cannot pass
vacuously).

**M-3 — readme-verbatim-macos byte-identical. REPRODUCED.** Independent
extraction and comparison: MATCH. Job green in the verified run. CC's
disclosed self-caught regex overmatch during its own verification is
noted approvingly — the read-the-diff-before-trusting-it discipline is
exactly the house countermeasure, applied unprompted.

**M-4 — patch/compiler fidelity on the runner. REPRODUCED via M-6's
gate + attested log.** The four `applying patch:` lines and the Apple
clang provenance are CC-attested from log and artifact; the mechanical
half (patches list exact, compiler non-empty, SHA = pin) is enforced by
the green `Check provenance` step on the macos leg — machine-checked,
as the ledger row anticipated.

**M-5 — cross-compat-macos. REPRODUCED at job level.** Job green in the
verified run: the macos-15 artifact's three binaries execute under
macos-26. Authored in from the start per spec — slice02's lesson
applied at design time, confirmed by reading the workflow's history
(present in `fd4ae6a`, the slice's first implementation commit).

**M-6 — check-provenance.sh gates all legs, provably fails.
REPRODUCED.** Locally in the sandbox against a pins-exact fixture:
clean pass exit 0; three tamper tests each exit 1 with
correctly-attributed named complaints (grounder SHA mismatch quoting
both values; per-platform patch-list mismatch; empty compiler field);
restored clean exit 0. Step present and green on all three build legs
in the verified run. **Slice02's W-4/W-5 residue class is retired
permanently** — provenance is now a property of every green run.
`shellcheck` zero findings on all scripts at HEAD.
(Verifier disclosure: my first local run failed the clean pass —
correctly, because this sandbox's dist was never a complete
script-built set; the gate caught a genuinely incomplete provenance
file, which is evidence *for* it. And my exit-code capture repeated the
pipeline mistake a fourth time before being redone without pipes. Both
misfires were mine, not the work's.)

**M-7 — README CI section + badge. REPRODUCED.** CI section names all
macOS legs with the runner-choice rationale and the brew-float
accepted-risk note; single badge line unchanged at the billosys path.

**M-8 — no Linux regression. REPRODUCED at run level.** The verified
run is green overall with both Linux artifacts produced and the
provenance step green on both ubuntu legs — a regression in any Linux
job would have failed the run.

## Bubble-up check (per PROJECT-MANAGEMENT.md Part IV)

1. **Delivered as assigned?** Confirmed against arc-plan §2's slice03
   line, plus all explicitly-assigned extras. Accurate, not inflated.
2. **Silent-drop diff honest?** Yes — every out-of-scope item's
   non-delivery is individually accounted for, including the lipo
   non-investigation and the correctly-deferred CDC-5 bumps.
3. **Arc-plan change required?** No — concurred. OQ1–OQ3 were closed at
   v1.3; this slice confirms rather than changes. The `min_os=15.0`
   observation is correctly routed as arc02 input, not an arc-plan
   change.

## Verdict

**Close — recommended to the operator as: accept.** 8/8 rows
reproduced (or reproduced-at-job-level with the established inference),
none refuted, no residue left behind — this slice *retired* the last
one instead of creating any. First-push full-matrix green on a slice
with a new OS leg is the substrate discipline paying compound interest:
byte-verified scripts, local dry runs, and gates that cannot pass
vacuously.

With this acceptance, **arc01's last slice is closed and the arc close
follows** — see `../closing-report.md` (assembled by CDC; gated by the
operator, per Section B's the-composer-does-not-sign-off rule).

Closed rows: 8/8 dispositioned (8 reproduced). Verified by: CDC (Cowork
sandbox session), 2026-08-06.
