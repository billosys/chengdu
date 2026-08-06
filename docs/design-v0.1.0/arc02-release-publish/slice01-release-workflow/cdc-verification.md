# chengdu arc02 / slice01 — release-workflow — CDC verification

> Independent verification per `LEDGER-DISCIPLINE.md` Section A (CDC
> protocol). Verifier: CDC (Cowork sandbox session), 2026-08-06. Inputs:
> authenticated clone of the public repo at `72e8ede`; the three linked
> evidence artifacts independently fetched (post-refactor run
> `31068904658`, release `v0.0.1-rc1` + run `31069094843`, refusal run
> `31069859859` + releases list); local reproductions in the sandbox.
>
> **Row count check: 8 opened, 8 dispositioned. No silent drops.**

## Per-row walk

**G-1 — reusable workflow, no duplication. REPRODUCED, one nit.**
`build-reusable.yml` carries all seven build/gate jobs; `build.yml` and
`release.yml` are thin callers (release adds only the `publish` job).
`actionlint` locally: zero findings. Nit (CDC-6a): the closing report
says "all four workflow files" — three exist; a miscount in prose, not
in the work.

**G-2 — package-release.sh correct and tamper-resistant. REPRODUCED**
(Linux tree locally; macOS half via the real rc1 assets). Local run:
tarball contains exactly the 4 files; `sha256sum -c` verifies clean;
corrupt-tarball tamper → `FAILED` + warning; pins-mismatch tamper →
refused with the named `check-provenance.sh` complaint before producing
output; notes render with both compat floors verbatim. The macOS
packaging path is additionally evidenced by the published rc1 assets
(fetched, names and provenance table correct). The `wc -l` → `awk NF`
patch-count fix is proven in production: rc1's notes show Linux
grounder 2 / macOS grounder 4. Observation (CDC-6b, non-blocking): the
script `rm -rf`s its output dir before the provenance gate runs, so a
refusal also clears the *previous* run's local output — harmless in CI
(fresh workspace), worth a comment someday.

**G-3 — happy-path test release. REPRODUCED.** Release page fetched:
`v0.0.1-rc1`, pre-release-marked, the four chengdu assets (plus
GitHub's automatic source archives — the "Assets 6" count is those two
extras, correctly not chengdu's doing), body rendered from the
template. Kept-published-as-evidence accepted per the slice-doc's
disclosed option.

**G-4 — refuse-to-publish. REPRODUCED.** Red run fetched independently:
failed at `Fetch upstream` on every build leg (exit 128, the tampered
all-zero SHA — the same fail-loud property verified back in slice01 of
arc01, now doing gate duty), `publish` 0s/never started. Releases list
fetched: only `v0.0.1-rc1` exists — absence confirmed. Restoration
commit present in history. **Project ledger P2's mechanism now has
reproduced building-block evidence.**

**G-5 — notes render. REPRODUCED.** Floors exactly per D2/D3; 6-row
provenance table; correct patch counts (see G-2).

**G-6 — CDC-5 retired. REPRODUCED.** `checkout@v7` /
`upload-artifact@v7` / `download-artifact@v8` across the workflows;
run `31068904658`'s annotations carry no Node.js deprecations (the two
Homebrew tap-trust notices are unrelated, as CC said).

**G-7 — no CI behavior change. REPRODUCED at run level.** The
post-refactor run is green with the same jobs now nested under
`call-build /`, three artifacts produced; the run-page render condenses
matrix legs (a known quirk from prior passes) but the artifact set and
job identities match arc01's close run.

**G-8 — docs current. REPRODUCED.** README "Releases" section states
trigger, whole-matrix gating, direct-publish default + draft toggle
pointer; `release.yml`'s header documents the idempotency contract
plainly (re-run of a released tag fails at `gh release create`,
intentionally; replace = delete release + tag, re-push) — documented
*and true by construction*.

**The timeout fix (landed independently, verified here anyway):**
`run_engine`'s watcher pattern is correctly bracketed (`set +e` around
the `wait`, RC captured, watcher reaped) — the CDC-3 shape CC reports
briefly reintroducing is absent from what landed. Hang simulation in
the sandbox (sleeping stub engine): killed at the timeout, exit mapped
to 124, labeled `FAIL: solve … got exit 124`, overall exit 1, restore
clean. Two minor notes (CDC-6c, non-blocking): `ENGINE_TIMEOUT_SECS=60`
is a hard assignment — not env-overridable, which my own test
discovered by being silently ignored (the sim therefore ran the full
60s — behavior still correct); and a genuine engine SIGKILL (137) from
any other cause would be conflated with a timeout. Neither matters for
the smoke gate's purpose; wolong owns real timeout policy via erlexec.

## Findings

**CDC-6 (minor, three parts):** (a) "four workflow files" prose
miscount; (b) refusal-clears-previous-output in `package-release.sh`;
(c) timeout constant not overridable + 137/timeout conflation. All
non-blocking; none warrants rework this arc.

## Bubble-up check (per PROJECT-MANAGEMENT.md Part IV)

1. **Delivered as assigned?** Confirmed — every arc-plan §3 slice01
   component plus the cc-prompt's explicit extras, nothing inflated.
2. **Silent-drop diff honest?** Yes. Both mid-slice bugs are disclosed
   with their fixes *and* their fix's own near-miss — the disclosure
   trail on the timeout fix (found via rehearsal, landed independently,
   almost reintroduced CDC-3, caught) is exemplary crash-report
   practice.
3. **Arc-plan change required?** No — concurred. The
   `THIRD-PARTY-LICENSES` asset-set growth is correctly routed to
   slice02 (already D5's assignment), with the format-freeze warning
   attached. RQ1 is implemented-as-decided; RQ2 remains slice02's.

## Verdict

**Close — recommended to the operator as: accept.** 8/8 rows
reproduced; three real, durable, independently-fetched evidence
artifacts; two real bugs found by the slice's own verification rather
than shipped; one minor three-part finding, non-blocking. The
refuse-to-publish demonstration means project P2 will close at project
scale on evidence that already exists.

Verifier disclosures, per the standing pattern: my first packaging run
failed against a stale `check-provenance.sh` in my own workspace (the
work was fine; my copy was old — re-cloned and re-ran), and the
pipe-around-exit-code slip made its fifth appearance before being
re-measured. Both mine.

**Slice02 (`release-readiness`) is the arc's remaining slice**: license
audit, `THIRD-PARTY-LICENSES`, install docs, D4's project-plan change,
fallback re-evaluation, and the real `v0.1.0` cut.

Closed rows: 8/8 dispositioned (8 reproduced). Verified by: CDC (Cowork
sandbox session), 2026-08-06.
