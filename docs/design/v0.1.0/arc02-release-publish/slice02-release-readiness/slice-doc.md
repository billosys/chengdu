# chengdu arc02 / slice02 — release-readiness — slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1).
> Parent: [`../arc-plan.md`](../arc-plan.md) (v1.0). Ledger:
> [`./ledger.md`](./ledger.md). Assignment: [`./cc-prompt.md`](./cc-prompt.md).
> **This is arc02's final slice** — its closing act (the real `v0.1.0`)
> is the arc's composition evidence, and its bubble-up feeds the arc
> close directly.

## 1. Goal

Make the release *right*, then make it real. The machinery exists
(slice01); this slice supplies the judgment half: an evidence-based
license/linkage audit of what the six shipped binaries actually contain,
a `THIRD-PARTY-LICENSES` release asset that satisfies whatever
obligations the audit finds, consumer install documentation (the path
wolong and project ledger P4 will walk), the outstanding plan
dispositions (D4 candidates, fallback re-evaluation), and finally the
tag that cuts **`v0.1.0`** — the first real release, and the arc's
composition evidence.

## 2. The audit comes first, and it is evidence-based

Arc-plan D5's rule: *verify, don't assume.* The audit answers, from
build evidence (link commands in build logs at the pins, `nm`/`otool`/
`ldd` on the artifacts) — never from README claims:

1. **What is compiled into each binary?** Per binary, per platform:
   which third-party components' objects are actually linked. Arc01
   evidence already suggests bliss and lpsolve are built but *not*
   linked; that suggestion is exactly what this audit confirms or
   refutes.
2. **What license governs each linked component?** Identified from the
   component's own LICENSE file at the pinned SHA, quoted by name and
   path. **Known-risk item, named up front: `h2-fd-preprocessor`** — its
   objects ARE on the grounder's link line (verified arc01), and its
   Fast Downward ancestry makes a GPL-family license plausible. If it
   (or anything) is copyleft, the grounder binary's distribution terms
   follow from that — this is the single most consequential possible
   finding, and the reason `v0.1.0` waits for this slice.
3. **What do the obligations require, and does the asset set satisfy
   them?** For permissive licenses: attribution texts in
   `THIRD-PARTY-LICENSES`. For copyleft (if confirmed): additionally the
   governing license text and a **Source availability** section in the
   release notes naming the exact upstream repos + SHAs + chengdu patch
   set (network-server source access — the sources are already public
   and pinned; compliance is documentation, not re-engineering). Per
   arc-plan RQ2, a confirmed-copyleft outcome is recorded as a tracked
   arc-plan amendment (the audit outcome + adopted compliance path), not
   improvised silently.

Audit artifact: **`docs/license-audit-v0.1.0.md`** in the repo (a new
artifact category — location proposed here for operator review with this
open set, per the confirmation protocol). It carries the per-binary
linkage tables, the per-component license identifications with evidence
pointers, and the obligations conclusion the release assets implement.

## 3. Scope

**In:**

- The audit + `docs/license-audit-v0.1.0.md` (§2).
- **`THIRD-PARTY-LICENSES`** emitted into the release asset set by
  `package-release.sh` (or a committed file it includes — CC's call,
  disclosed): per-binary sections, full license texts, generated
  consistently with the audit.
- **Tarball format growth, batched and disclosed before the freeze:**
  the tarballs gain a `fixtures/` directory (the byte-verified minimal
  pair, ~800 bytes) so an installed consumer can run a *real* verify
  smoke offline — this is what lets the install docs end in
  `pandaPIparser --verify` rather than `--help`, satisfies project
  ledger P4's literal wording, and gives wolong shipped test inputs.
  Together with `THIRD-PARTY-LICENSES`, this is the final asset-set
  shape; **after `v0.1.0`, formats are frozen** (arc-plan §4).
- **README "Install from release"**: per platform, ≤ 5 commands from
  download to a passing verify on the shipped fixtures (checksum
  verification included in the path). This section is written for
  wolong's arc03 to consume as its fetch specification.
- **D4 enacted:** `project-plan.md` §1 candidates updated
  (`macos-x86_64` declined, `linux-arm64` deferred) with a v1.2
  Version History entry per the plan-change discipline.
- **Fallback re-evaluation** (arc01 bubble-up item): proposed
  disposition — keep the fetch-only `--source pandadealer` code
  (cheap, harmless) but repoint the docs: restricted-network users
  should consume *release binaries* first, the vendored snapshot being
  a historical/source-inspection fallback only. Recorded in the
  arc-plan change log either way the operator decides.
- **The `v0.1.0` cut:** tag pushed only after every other row is done;
  full asset set (2 tarballs incl. fixtures, `SHA256SUMS`,
  `release-manifest.txt`, `THIRD-PARTY-LICENSES`); not pre-release;
  `v0.0.1-rc1` remains published as historical rehearsal evidence.
- **Consumer rehearsal:** immediately after the cut, walk the README
  install path verbatim from the real release on at least the macOS
  side (CC's machine); the Linux side is CDC's during verification and
  the arc close (arc ledger R-3).

**Out (disclosed, not dropped):**

- Signing/attestation — future-work note only (unchanged).
- Any build change in response to the audit (e.g. unlinking a copyleft
  component) — if the audit makes one *necessary* (not merely tidy),
  that is the RQ2 amendment path with operator involvement, not this
  slice's default scope. Documentation-based compliance is the default.
- wolong-side fetch code — wolong arc03's, against the frozen formats.
- Upstream contribution tickets (macOS fixes etc.) — still parallel
  work, still open.

## 4. Constraints

All standing constraints carry (scripts bash 3.2 + shellcheck;
actionlint; no logic in YAML; fail-loud; format changes only as
disclosed above). New: **the audit's claims must each carry an evidence
pointer** (a build-log line, an `nm` output, a LICENSE path at a SHA) —
an audit that asserts without pointing is the lemon-market failure mode
this project was founded against. And the cut is one-way per the
idempotency contract: `v0.1.0` publishes once; a botched cut is
delete-release-and-tag-then-retag, per `release.yml`'s header.

## 5. Verification approach

As established, plus: CDC re-derives the linkage audit independently on
the Linux side (link logs + `nm` in the sandbox at the pins) and checks
the license identifications against the pinned LICENSE files; the macOS
linkage half is CC-attested from build logs with CDC checking internal
consistency. The `v0.1.0` release is verified by download on clean
environments (CDC: Linux; operator/CC: macOS) — which doubles as arc
ledger R-3's reproduced-at-arc-scale composition evidence at arc close.

## 6. Exit criteria

The ledger's 9 rows — see [`./ledger.md`](./ledger.md). Shape: linkage
enumerated with evidence (L-1); licenses identified incl. the named
risk item (L-2); `THIRD-PARTY-LICENSES` complete and shipped (L-3);
obligations demonstrably satisfied, outcome recorded per RQ2 (L-4);
install docs real and fixture-backed (L-5); D4 enacted (L-6); fallback
disposition recorded (L-7); `v0.1.0` cut with the final asset set
(L-8); consumer rehearsal walked (L-9).
