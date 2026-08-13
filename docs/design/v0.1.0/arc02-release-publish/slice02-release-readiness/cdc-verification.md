# chengdu arc02 / slice02 — release-readiness — CDC verification

> Independent verification per `LEDGER-DISCIPLINE.md` Section A.
> Verifier: CDC (Cowork sandbox session), 2026-08-06. Inputs:
> authenticated clone at `7a56b64`; release `v0.1.0` and its assets
> downloaded directly from the public repo; independent re-derivations
> in the sandbox against this session's own canonical-pin artifacts.
>
> **Row count check: 9 opened, 9 dispositioned. No silent drops.**

## Per-row walk

**L-1 — linkage enumeration with evidence. REPRODUCED, and the audit's
one disclosed gap closed.** The audit's link-line evidence is quoted
from real logs (macOS local; a fresh CI run triggered when old logs had
aged out — the right move, disclosed). CDC's independent additions:
`nm` on this sandbox's *Linux* canonical-pin grounder — the check the
audit itself disclosed as not performed on Linux — shows the
h2-fd-preprocessor symbols present (6 matches) and zero
`bliss`/`lpsolve` symbols. Both platforms now have binary-level
confirmation. arc01's not-linked suggestion for bliss/lpsolve is
verified fact on both platforms.

**L-2 — license identification. REPRODUCED.** Independently confirmed:
`h2-fd-preprocessor` @ `e335433` has no LICENSE/COPYING anywhere in
history (git log over both filenames: empty); its README's Fast Downward
provenance statement matches the audit's quotation verbatim; Fast
Downward's canonical `LICENSE.md` is GPL-3.0 (fetched independently);
boruvka's `BSD-LICENSE` present in the vendored tree. The audit's
central conclusion — treat the twelve linked h2 objects as
GPL-3.0-covered, the grounder binary as GPL-obligated, and the
parser/engine as unaffected mere-aggregation — is **explicitly framed
as a conservative judgment with its reasoning shown**, which is exactly
the calibration D5 demanded. CDC concurs with both the conclusion and
the framing. (Noted for the operator's awareness, not as a defect: the
vendored copy's own missing license grant is an upstream hygiene gap —
a courteous upstream issue to `galvusdamor/h2-fd-preprocessor` would
serve the ecosystem; candidate for the contribution-ticket batch.)

**L-3 — THIRD-PARTY-LICENSES. REPRODUCED.** Downloaded from the real
release: per-binary sections (parser/grounder/engine BSD 3-Clause;
cpddl; boruvka; `h2-fd-preprocessor (linked into pandaPIgrounder) —
GPL-3.0` with the full GPL text). Component list diffs clean against
the audit's tables — nothing shipped is undocumented, nothing
documented is unshipped.

**L-4 — obligations satisfied. REPRODUCED.** The release body carries
the "Licensing and source availability" section: GPL-3.0 named for the
grounder, upstream repos at pinned commits linked (network-server
source access), BSD status for the other two stated. The RQ2 tracked
amendment is present in the arc-plan's change log. Compliance is
documentation-based as defaulted; no build change was made or needed.

**L-5 + L-8 + L-9 (Linux half) — the consumer path, walked for real.
REPRODUCED.** CDC executed the README's four Linux commands verbatim
against the published release in a clean directory: download →
`sha256sum -c` OK → extract (fixtures present in the tarball) → the
full five-gate chain on the shipped fixtures ending
`Plan verification result: true`, exit 0. Four commands, counted
literally. `v0.1.0` is a full release (not pre-release) with exactly
the five specified assets; `v0.0.1-rc1` remains as history. The macOS
rehearsal transcript (L-9's CC half) is attested and consistent.

**L-6 — D4 enacted. REPRODUCED.** project-plan v1.2 read: candidate
list updated, entry dated, attributed to this slice, rationale correct.

**L-7 — fallback disposition. REPRODUCED.** Arc-plan change log carries
it; README and `fetch-upstream.sh --help` consistent (release binaries
recommended first; vendored snapshot demoted to source-inspection
fallback).

**The would-have-shipped bug (upload gap):** commit `427d365` confirms
the fix and the presence check in `publish-release.sh`; the rehearsal
that caught it is exactly what the dependency order was for. Disclosed,
hardened, re-proven before the one-way cut. Exemplary.

## Bubble-up check

1. **Delivered as assigned?** Yes — and the bubble-up's claim that
   arc02 is functionally complete is correct; formal closure is the arc
   close, below.
2. **Silent-drop diff honest?** Yes, including the disclosed
   would-have-shipped bug and the fresh-CI-run detour when old logs
   proved inaccessible.
3. **Arc-plan change required?** Already made (RQ2 amendment); no
   further change. One new upstream-courtesy item (the h2 license-file
   gap) added to the contribution-ticket candidates — a note, not a
   plan change.

## Verdict

**Close — recommended to the operator as: accept.** 9/9 rows
reproduced, including independent confirmation of the audit's central
finding and CDC's closure of its one disclosed evidence gap. The
release this project exists to produce is real, public, license-clean,
and was consumed successfully by an independent party following only
the README.

Closed rows: 9/9 dispositioned (9 reproduced). Verified by: CDC (Cowork
sandbox session), 2026-08-06.
