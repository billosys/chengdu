# chengdu arc01 / slice01 — substrate — CDC verification

> Independent verification per `LEDGER-DISCIPLINE.md` Section A (CDC
> protocol), performed in the Cowork cloud sandbox (clean Ubuntu 24.04,
> g++ 13.3.0 — the environment the runbook's Linux path was originally
> reproduced in). Verifier: CDC session, 2026-08-05, independent of the
> implementing CC session. Inputs: the repo working tree at CC's close
> (staged from the operator's machine), CC's `closing-report.md`, the
> opening `ledger.md`.
>
> **Row count check: 11 opened, 11 dispositioned in the closing report.
> No silent drops at the count level.**
>
> **Environment constraint, disclosed up front:** this sandbox cannot
> reach gitlab.com, so the *canonical-mode grounder* build cannot run
> here (its cpddl submodule is gitlab-hosted). Canonical parser and
> engine CAN and did script-build here. This is the same constraint the
> original runbook session disclosed; the first ubuntu CI run in slice02
> discharges it mechanically.

## Per-row walk

**F-1 — pins.env. REPRODUCED.** Required grep returns exactly 8 lines.
All four full-length SHAs independently verified against this sandbox's
own clones of the four repos (made earlier from the same pins the runbook
records): parser `88c0995c…ed81`, grounder `b04ff7b…5afc`, engine
`810f043…1ac0`, PandaDealer `fde3245…384d` — all MATCH.

**F-2 — fetch-upstream.sh. REPRODUCED** (with one env-limited half).
`--source pandadealer`: clean run, exit 0, SHA verified, all three
symlinks correct. Tamper test (SHA → `000…0`): aborted at
`fatal: reference is not a tree`, exit 128, no silent continuation —
matches CC's reported behavior exactly. Canonical mode in this
environment: fails loudly at the cpddl submodule (gitlab blocked), exit
1 — which is the *specified* fail-loud behavior under a broken network,
observed as such. The canonical *happy path* remains macOS-attested;
re-entry: slice02's first CI run.

**F-3 — build-parser.sh. REPRODUCED.** Canonical-mode script build on
Linux (parser cloned at pin via github): exit 0,
`dist/linux-x86_64/pandaPIparser --help` exit 0. Bonus datapoint: the
pandadealer-mode parser also builds and runs on Linux.

**F-4 — build-grounder.sh. CONFIRMED WITH NAMED GAP + NEW FINDING.**
macOS canonical evidence (CC, attested): credible and consistent with
the field-confirmed runbook path; not contradicted by anything found
here. Linux canonical: **environment-blocked** (gitlab), not refuted —
re-entry: slice02 CI. Patch logic partially reproduced on Linux: `0002`
apply/already-applied branching and the chengdu bliss patch both applied
cleanly in the diagnostic run (`applying patch:` count = 2, correct for
Linux). **New finding (CDC-1, below): the pandadealer fallback is not
build-viable for the grounder on Linux/GCC-13** — outside this row's
macOS-canonical evidence, but inside the fallback's implicit promise.

**F-5 — build-engine.sh. REPRODUCED.** Canonical-mode script build on
Linux: exit 0, `--help` exit 0, provenance SHA exact.

**F-6 — patches-not-fork. REPRODUCED.** `patches/` contains exactly the
bliss patch; `grep -rn 'sed.*-i' scripts/` clean; the patch file's diff
inspected (correct fix, correct application notes) and observed applying
cleanly during the Linux grounder diagnostic.

**F-7 — smoke positive. REPRODUCED.** Fixtures independently confirmed
**byte-for-byte identical** to the runbook's two lisp blocks (scripted
comparison). 5/5 gates PASS, exit 0, on Linux. Binary provenance for
this run, disclosed: parser + engine script-built canonical in this
sandbox; grounder binary from this session's earlier canonical-pin build
(same SHA, same commands — the script could not produce it here per F-4's
environment block).

**F-8 — smoke negative. REPRODUCED, plus harness self-test.** 4/4
distinct outcomes PASS on Linux, exit 0 — including the unsolvable
fixture classified UNSOLVABLE off the `Status:` line at engine exit 0.
Harness self-test per the Verify command: tampered the missing-file
expectation → harness correctly FAILs with exit 1; reverted. Corpus
bonus: `--corpus` Transport pfile01 → 7/7 including solve + verify.
One spec-keeping note (CDC-2): the unsolvable fixture's construction
differs from the slice-doc's named variant — see findings.

**F-9 — provenance. REPRODUCED** for parser + engine on Linux (SHAs
match pins exactly, compiler line recorded); grounder line macOS-attested
only (same environment block as F-4). Observation, non-blocking: in
pandadealer mode all components record the PandaDealer snapshot SHA —
correct provenance for that source, worth a one-line comment in the file
format when arc02 formalizes the manifest.

**F-10 — shellcheck. REPRODUCED.** shellcheck 0.9.0: exit 0, zero
findings, all six scripts. The single `shell=bash` directive is a
legitimate classification, not a suppression.

**F-11 — hygiene. PARTIALLY REPRODUCED.** `.gitignore` contents verified.
README's build section is exactly 5 commands. The verbatim clean-machine
run on Linux is blocked at command 1 (canonical fetch → gitlab); the
macOS verbatim run is CC-attested. Re-entry: slice02 CI executes the
README path on a genuinely clean runner.

## Findings

**CDC-1 (correctness-grade, fallback path): `--source pandadealer` is
fetch-verified but NOT build-viable on Linux/GCC-13.** Three independent
blockers, each isolated by diagnostic:
1. The vendored snapshot ships **no patch files** — `build-grounder.sh`
   resolves `0002-makefile.patch` relative to the grounder checkout,
   which only holds in canonical layout (fails loudly, exit 1).
2. With the patch supplied diagnostically, the vendored grounder `src/`
   uses pre-final concepts-TS syntax (`concept bool`,
   return-type-requirements) that GCC-13 under `gnu++20` rejects.
3. The vendored engine lacks `#include <cstdint>` in `VisitedList.h` —
   fails on GCC-13; a one-line diagnostic fix makes it build clean.
The 2024–25 canonical pins have none of these problems (parser/engine
proven here; grounder proven in the runbook session). CC could not have
seen this: the ledger asked for fallback *fetch* behavior only, and the
macOS/clang fallback build was never claimed. Disposition: **routed to
arc-plan as a tracked change** (v1.2) — the fallback's role needs an
operator decision at slice02 planning: fetch-only fallback (document the
limit), chengdu-carried compat patches for the snapshot, or retiring the
fallback once chengdu's own releases exist (self-solving after arc02).

**CDC-2 (minor, spec-keeping): unsolvable fixture construction differs
from spec.** Slice-doc §2 names "the circular-precondition unsolvable
variant" (runbook's: `draft` requires `reviewed`); the delivered fixture
instead removes `draft` from the method so `review`'s precondition is
unestablishable. Functionally equivalent for the gate (verified: engine
exit 0, `Proven unsolvable`), arguably cleaner, and clearly commented in
the fixture itself — but the substitution went undisclosed in the
closing report. Remedy: this note; no rework.

**CDC-3 (minor, robustness): bare-command engine invocations under
`set -e`.** `smoke-test.sh` runs `"$E" … ; RC=$?` (positive solve gate)
and three bare parser/grounder/engine calls (negative gate d) outside
`run_and_capture`; a nonzero exit there (engine crash, OOM) aborts the
script mid-run without the per-gate FAIL message. Exit code is still
nonzero, so CI never sees a false green — the gap is diagnostic quality,
not gate integrity. Recommend the `|| RC=$?` pattern in slice02's touch
of this file. Non-blocking.

## Bubble-up check (per PROJECT-MANAGEMENT.md Part IV)

1. **Delivered as assigned?** Confirmed against arc-plan §2's slice01
   line. The scope claim in CC's bubble-up is accurate, including its
   honest labeling of "both OSes" as attested-not-reproduced.
2. **Silent-drop diff honest?** Yes at item level — every in/out item
   accounted for, and CC's report of its own two mid-verification bug
   finds (ANSI stripping; submodule-defeats-auto-apply) checks out
   against the code, which contains both the fix and an accurate comment
   trail. One disclosure miss at detail level (CDC-2), recorded.
3. **Arc-plan change required?** **YES — CDC-1.** Applied as arc-plan
   v1.2 with a Version History entry naming this slice's verification as
   the source. CC's own "no arc-plan change needed" was correct on the
   evidence CC had; the change is driven by CDC-1, found only when the
   fallback was built rather than fetched.

## Verdict

**Conditional close — recommended to the operator as: accept.**
9 of 11 rows fully `reproduced` (or better) on Linux + macOS-attested;
F-4 and F-11 (and F-9's grounder line) carry an environment-blocked
Linux half with a concrete, near-term re-entry: **the first green ubuntu
run in slice02 discharges all three mechanically** (it is also OQ1's
gitlab-reachability test). No row refuted; no silent drops; one
correctness-grade finding routed to the arc plan; two minor findings
recorded, non-blocking.

## What Worked

- The split-verification design (CC attests macOS / CDC reproduces
  Linux) surfaced CDC-1 precisely *because* the verifier ran a different
  path than the implementer — independence caught what self-review
  structurally could not.
- Fail-loud discipline in `fetch-upstream.sh` made every failure in this
  pass legible: nothing needed forensics.
- The harness self-test row (F-8's tamper clause) proved the smoke
  harness can fail — the vacuous-test countermeasure, working exactly as
  the ledger intended.
- Byte-for-byte fixture provenance (runbook → fixtures) made the F-7
  check a mechanical diff instead of a judgment.

Closed rows: 11/11 dispositioned (9 reproduced, 2 conditional with
re-entry). Verified by: CDC (Cowork sandbox session), 2026-08-05.
