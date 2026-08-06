# chengdu arc01 / slice02 — linux-workflow — CDC verification

> Independent verification per `LEDGER-DISCIPLINE.md` Section A (CDC
> protocol). Verifier: CDC (Cowork sandbox session), 2026-08-06,
> independent of the implementing CC session. Inputs: repo state at
> HEAD `8a8c3c8` (authenticated file-by-file against git via checksum —
> see *Verification-integrity note* below), CC's `closing-report.md`
> (checksum-verified), the opening `ledger.md` (checksum-verified), and
> workflow run `31060179607`, independently fetched from the public
> `billosys/chengdu` repo after transfer.
>
> **Row count check: 9 opened, 9 dispositioned. No silent drops.**

## Verification-integrity note (read this — it bit the verifier)

The device-mount staging used for file access served **stale cached
snapshots** of four files (README.md and three scripts), which briefly
produced a false discrepancy: the staged README lacked the badge, CI
section, and fetch-only note that ledger rows W-8/W-9 claim. Git
adjudicated: CC's close commit `325c054` contains all three, the
working tree matches HEAD, and the "missing" content was an artifact of
the verifier's own evidence channel. All inputs to this verification
were subsequently authenticated by md5 against `git show HEAD:<file>`
before use. **Protocol for future CDC passes in this environment:
checksum every staged file against git before judging it.** Two further
verifier-side tooling errors (a pipeline exit-code capture, a wrong
README block index) were caught and corrected before becoming findings;
the run of three is a pattern worth naming: the least reliable
component in this verification loop was the verifier's own quick
one-liners, not the work under review.

## Per-row walk

**W-1 — workflow exists, lints. REPRODUCED.** Triggers and `@vN` pins
verified by read of the HEAD file; `actionlint` 1.7.12 run locally in
the sandbox: exit 0, zero findings. The `lint` job is also green in the
verified run.

**W-2 — matrix green end-to-end. REPRODUCED.** Run `31060179607`
fetched independently from the public repo post-transfer: overall
success, commit `808579a`, both `build (ubuntu-22.04)` and
`build (ubuntu-24.04)` jobs green, 3m36s. Step-level PASS terminal
lines: attested by CC and structurally implied (the smoke script exits
nonzero on any gate failure — property re-verified in this sandbox —
so a green job cannot contain a failed gate).

**W-3 — gitlab reachability recorded. REPRODUCED (by inference), with
the inference stated:** raw step logs are auth-gated even on public
repos, so the specific `gitlab.com/danfis/cpddl` log line is
CC-attested. But the fetch script's fail-loud property is
sandbox-reproduced (canonical fetch *cannot* succeed without reaching
gitlab — demonstrated here, where it correctly dies), and the run's
fetch step succeeded on both runners. Green + fail-loud ⇒ gitlab
reached. `arc-plan.md` v1.3 records the resolution properly (verified:
dated entry, names slice02, correct content).

**W-4 — slice01 F-4/F-9 re-entry discharged. REPRODUCED at job level,
one named residue.** The grounder built canonically on both Linux
runners in the verified run — this formally discharges slice01's
environment-blocked Linux halves (F-4, F-9, and F-11 via W-6 below);
**slice01's conditional close is now unconditional.** Residue:
`provenance.txt` *content* inside the artifact remains CC-attested (two
correct `applying patch:` entries, SHA = pin, g++ 11.4) — artifact
downloads are auth-gated from this sandbox. Discharge: drop the 22.04
artifact zip into the repo folder for a five-minute check, or let arc02
discharge it naturally when the release manifest consumes the file.
Non-blocking.

**W-5 — artifact contents + cross-runner compat. REPRODUCED at job
level.** Both artifacts confirmed present on the run page with distinct
names and plausible sizes (9.08 / 8.85 MB). The `cross-compat` job —
green — downloads the 22.04 artifact and executes all three binaries'
`--help` on a real `ubuntu-24.04` runner, which is *stronger* evidence
than the ledger's suggested local-container check (a real runner beats
an emulated one; CC's disclosed substitution is accepted and endorsed).
The provenance-file-presence sliver shares W-4's residue.

**W-6 — readme-verbatim byte-identical. REPRODUCED.** Independent
extraction and comparison of the README's Linux-prereq + five-command
blocks against the job's `run:` block: byte-identical MATCH. Job green
in the verified run on a clean runner.

**W-7 — CDC-3 remediated. REPRODUCED.** Crash simulation performed
independently in the sandbox (stub engine, exit 3): positive mode
produces `FAIL: solve … got exit 3` (+ a correctly cascading verify
FAIL) with overall exit 1; negative mode produces the labeled
unsolvable-gate FAIL with exit 1. No bash abort, no silent path.
Restored binary, both modes re-run clean (5/5, 4/4). `shellcheck`
exit 0 on all scripts at HEAD.

**W-8 — fetch-only fallback guard. REPRODUCED.** `fetch-upstream.sh
--help` documents FETCH-ONLY with CDC-1's reasons; README carries the
same ("This fallback is fetch-only…"); and the guard was exercised
live: pandadealer fetch + `build-grounder.sh` → immediate, correct,
pointer-bearing failure, exit 1 — CDC-1's misleading error path is
gone.

**W-9 — badge + CI section. REPRODUCED, one post-transfer note.** Badge
and "Continuous integration" section present at HEAD; the operator's
commit `8a8c3c8` already corrected the badge URL from `oubiwann/` to
`billosys/` (and added the Anshun Bridge logo — on-theme). Note: the
badge will not render on the repo's front page until `main` receives
the slice02 merge; see *Operator actions* below.

## Findings

**CDC-4 (process, environment): stale mount-cache snapshots during CDC
staging.** Described in the integrity note. Remedy adopted: checksum
staged files against git HEAD before use. Recorded here so the next
verification session inherits the protocol rather than the surprise.

**CDC-5 (maintenance, minor): GH Actions Node20 deprecation warnings**
on `checkout@v4`/`upload-artifact@v4` in the verified run. Bump to the
next majors during arc02's workflow touch. Non-blocking.

## Bubble-up check (per PROJECT-MANAGEMENT.md Part IV)

1. **Delivered as assigned?** Confirmed against arc-plan §2's slice02
   line — delivered, plus the explicitly-assigned extras (tripwire job,
   actionlint gate, CDC-3 fix, W-8 disposition) and one disclosed
   mid-slice addition (`cross-compat`), which the bubble-up correctly
   analyzes as a slice-doc-authoring lesson rather than an arc-plan
   change.
2. **Silent-drop diff honest?** Yes — in/out lists both check out
   against the tree; nothing started or dropped silently. The one
   process deviation of the slice — the repo initially created at
   `oubiwann/` private rather than `billosys/` per plan — was caught at
   review, adjudicated by the operator, corrected by transfer, and
   recorded in `CLAUDE.md`; it is a process finding already disposed,
   not a scope drop.
3. **Arc-plan change required?** Already made and verified: CC's v1.3
   correctly records OQ1 and OQ3 resolutions with evidence, per the
   plan-change discipline. No further change needed from this pass.

## Verdict

**Close — recommended to the operator as: accept.** 8 of 9 rows fully
`reproduced` (or stronger-than-specified); W-4/W-5 carry one small,
named, non-blocking residue (provenance-file content inside the
artifact — CC-attested, dischargeable in minutes with the zip or
naturally at arc02). Slice01's conditional close is discharged in full
by this slice's run. No row refuted. Arc01 status after acceptance:
slices 01–02 closed; **slice03 (macos-workflow) is the remaining slice
before arc close.**

## Operator actions (small, non-gating)

- Push the two local image commits (`5b9e89b`, `8a8c3c8`) and merge
  `slice02-linux-workflow` → `main` per the direct-to-main convention —
  local is currently ahead of origin and `main` predates the slice.
  The badge goes live on the front page with that merge.
- Optional residue discharge: drop `dist-linux-x86_64-ubuntu-22.04-808579a.zip`
  anywhere in the chengdu folder and ping CDC.

## What Worked

- **Public-repo verification is materially stronger:** the run page was
  independently fetchable, turning "trust CC's summary" into "read the
  run" — the transfer+public decision paid for itself within one slice.
- **Green + fail-loud composes into proof:** because slice01 made every
  script fail loudly, a green CI run carries real information — W-3
  closed by inference from two independently verified properties, which
  only works when nothing can fail silently.
- **The checksum-before-judging protocol** (born from CDC-4) converted
  a would-be false accusation into a five-minute adjudication. Keep it.

Closed rows: 9/9 dispositioned (8 reproduced, 1 reproduced-at-job-level
with named attested residue). Verified by: CDC (Cowork sandbox
session), 2026-08-06.
