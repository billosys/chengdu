# chengdu arc01 / slice01 — substrate — closing report

> Written by CC (this session, macOS/Apple Silicon). Per-row walk of
> `ledger.md`'s 11 rows, then the bubble-up to `arc-plan.md`, per
> `PROJECT-MANAGEMENT.md` Part IV and `LEDGER-DISCIPLINE.md` Section A.
> CDC's independent Linux reproduction and re-walk (`cdc-verification.md`)
> is the next step, not yet performed.

## Per-row walk

Rows: 11. Done: 11. Deferred: 0. No-op: 0. All evidence strength `attested`
(macOS) — CDC's Linux reproduction upgrades the `*(both)*` rows to
`reproduced`, per slice-doc §4's split-verification design.

**F-1** — `pins.env`. **done.** All four `_URL`/`_SHA` pairs present; the
required grep (`^(PARSER|GROUNDER|ENGINE|PANDADEALER)_(URL|SHA)=`) returns
all 8 lines. Each 40-char SHA was resolved via `git ls-remote <repo> HEAD`
and cross-checked against the runbook §9 short forms before being written —
all four repos' HEAD still matched the runbook's pinned short SHA exactly at
fetch time (2026-08-05); none have moved.

**F-2** — `scripts/fetch-upstream.sh`. **done.** Canonical mode: ran clean,
all three `upstream/*` dirs' `git rev-parse HEAD` matched `pins.env` exactly,
grounder submodules (cpddl, h2-fd-preprocessor) initialized and non-empty.
`--source pandadealer` mode: produced `upstream/PandaDealer` plus three
symlinks (`pandaPIparser → PandaDealer/00-parser`, etc.), SHA verified.
Fail-loud test: tampered `PARSER_SHA` to `000…0`, re-ran — script aborted
immediately with a clear `fatal: reference is not a tree` and nonzero exit
(128), no silent continuation; `pins.env` restored and re-verified clean
afterward.

**F-3** — `scripts/build-parser.sh`. **done.** `make -j` builds clean (only
upstream's own documented `yyunput` warnings). `dist/macos-arm64/pandaPIparser
--help` exits 0 and prints usage. Linux side pending CDC.

**F-4** — `scripts/build-grounder.sh`. **done.** All required patches apply
and are recorded: `0002-makefile.patch` (both platforms), the two
`0001-boruvka-*.patch` files (macOS only, applied from the grounder
checkout), and `patches/bliss-0.73-cxx11-string-literal.patch` (both
platforms, applied via the manual extract-then-patch path that bypasses
cpddl's rm-rf/re-unzip `bliss` target). Grounder compiler defaults to clang
(`GROUNDER_CC=cc GROUNDER_CXX=c++`), confirmed via `provenance.txt`.
`dist/macos-arm64/pandaPIgrounder --help` exits 0. **Verify-command
adaptation, disclosed:** the ledger's suggested `grep -c 'applying' build
log` doesn't fit as written — this script applies patches with BSD `patch
-p1` (which prints `patching file …`), not `git apply` (whose `Applying:`
line is an `am`-only behavior this script doesn't use). The script emits its
own `applying patch: <name>` log line before each application instead;
`grep -c 'applying patch:'` on a captured build log returns 4 on macOS,
matching the expected count. See bubble-up below for the real finding this
row's verification surfaced.

**F-5** — `scripts/build-engine.sh`. **done.** `cmake ../src
-DCMAKE_BUILD_TYPE=Release && make -j` builds clean (only upstream's own
`-Winconsistent-missing-override` / unused-variable warnings).
`dist/macos-arm64/pandaPIengine --help` exits 0. Linux side pending CDC.

**F-6** — patches-not-fork discipline. **done.** `patches/` contains exactly
`bliss-0.73-cxx11-string-literal.patch`, generated from a real diff against
the extracted bliss 0.73 source (taken *after* applying upstream's own
memleak patch, matching build-order) and dry-run-verified against a fresh
re-extraction before being trusted. `grep -rn 'sed.*-i' scripts/` returns
nothing — no inline mutation of upstream sources anywhere in the scripts.

**F-7** — smoke test, positive path. **done.**
`fixtures/minimal/{domain,problem}.hddl` confirmed byte-for-byte identical
to the runbook §6 code blocks (scripted diff, zero bytes of difference).
`./scripts/smoke-test.sh` (default mode): 5/5 gates PASS (parse, ground,
solve/`Status: Solved`, convert, verify/`Plan verification result: true`),
script exits 0. **Disclosed reconciliation:** slice-doc §2/§5 lists
"validate, parse, ground, solve, convert, verify" (six names) where arc-plan
A1 says "all five gates." Implemented as five: `validate` is not a distinct
pipeline stage but the *purpose* the `parse` gate serves, per runbook §5's
own framing ("the parser IS the validator" — same binary, same
invocation shape as the worked `parse` example, differing only in the
discarded-output path `/dev/null`). This matches arc-plan A1's authoritative
"all five gates" wording. Linux side pending CDC.

**F-8** — smoke test, negative path. **done.**
`./scripts/smoke-test.sh --negative`: 4/4 distinct outcomes PASS, script
exits 0 — missing file (parser exit 2, `"I can't open"`), broken-syntax
fixture (exit 255, `"Parse error in file"`), broken-reference fixture (exit
255, `"...which is not declared"`, explicitly checked to *not* also match
the syntax-path string, keeping the two 255 outcomes distinguishable),
unsolvable fixture (engine exit 0, `Status: Proven unsolvable`, classified
UNSOLVABLE — not success, not a generic failure). Harness self-test
performed per the Verify command: tampered the missing-file expectation to
an unmatchable string, re-ran — the harness correctly reported `FAIL` and a
nonzero overall exit; reverted and re-verified clean. **One real bug found
and fixed during this row's own verification:** upstream's `--no-colour`
flag does not reliably strip ANSI SGR codes on the syntax-error path (the
"Parse error"/"in file"/"in line N" fragments arrive split by color
escapes even with `-C`), which silently broke the substring match on first
run. `smoke-test.sh` now strips ANSI codes from all captured output
unconditionally rather than trusting `-C`'s coverage. Linux side pending
CDC.

**F-9** — provenance byproduct. **done.**
`dist/macos-arm64/provenance.txt` has one `key=value` block per component;
all three SHAs match `pins.env` exactly; the grounder's `patches=` field
lists all four applied patches; `compiler=` is `Apple clang version 17.0.0`
for all three, consistent with the clang default. Linux side pending CDC.

**F-10** — shellcheck. **done.** First run: one finding, SC2148 on
`scripts/lib-platform.sh` (sourced-not-executed, no shebang, shell
undetectable). Resolved with a `# shellcheck shell=bash` directive — not a
suppression, since the file is genuinely never executed directly and a real
shebang would be misleading. Re-run: `shellcheck scripts/*.sh` exits 0,
zero findings, across all six scripts.

**F-11** — repo hygiene. **done.** `.gitignore` covers `/upstream/`,
`/dist/`, `*.o`, `*.log`, `.DS_Store`. README's "Build + smoke test" section
lists exactly five commands. Ran verbatim from a fully clean state (`rm -rf
upstream dist` first): all five succeeded end-to-end, ending in the smoke
test's 5/5 PASS summary; `git status --short` afterward shows no
`upstream/`/`dist/` leakage.

## Bubble-up to the arc

**1. Did this slice deliver the piece of the arc's capability the arc-plan
assigned it?** Yes, against arc-plan's slice breakdown (§2): "Repo skeleton:
pinned upstream SHAs + fetch script, `patches/`..., `scripts/build-*.sh`
encoding runbook §4 per component, `scripts/smoke-test.sh` encoding runbook
§5–§6 gates with correct exit-code/`Status:`-line semantics. Runnable
locally on both OSes." All of it is present and runs end-to-end on macOS;
the "runnable on both OSes" half of that claim is `attested`, not yet
`reproduced` — that's CDC's Linux pass, explicitly the next step, not a gap
in this slice's delivery.

**2. What did implementing this slice reveal that the arc-plan did not
anticipate?** One real finding, no arc-plan change needed:
`fetch-upstream.sh`'s `git submodule update --init --recursive` pre-populates
boruvka's `Makefile` file, which means cpddl's own `third-party/boruvka/Makefile`
file-target recipe — the one `0002-makefile.patch` extends to auto-`git
apply` the two macOS boruvka patches — never fires (`make` sees the target
file already exists and skips the recipe entirely). `build-grounder.sh`
works around this by applying the two macOS patches explicitly itself,
which is both correct and exactly what slice-doc §2 already specified
("the build script applies them from the checkout"). This is an
implementation-level interaction between our fetch strategy and upstream's
own patch design, not a scope or sequencing surprise — it doesn't change
`arc-plan.md`'s capability statement, slice breakdown, or open questions,
so no update was made there. Secondary, minor: a small shared helper,
`scripts/lib-platform.sh` (platform detection, provenance-append, compiler-id
resolution), was added beyond cc-prompt's literal file list to avoid
duplicating that logic four times across `build-parser.sh`,
`build-grounder.sh`, `build-engine.sh`, and `smoke-test.sh`. It is sourced,
not a new top-level deliverable, and is covered by the same shellcheck gate
(F-10).

**3. The silent-drop diff at slice scale.** Scope-as-delivered matches
scope-as-specified (slice-doc §2) exactly. In-scope items — `pins.env`,
`fetch-upstream.sh`, the bliss patch, all three build scripts with the
provenance byproduct, `smoke-test.sh` with the four fixture sets (including
the optional `--corpus DIR` flag), `README.md`, `.gitignore` — are all
present and verified (F-1 through F-11 above). Out-of-scope items are all
explicitly named in slice-doc §2's "Out" list and none were silently
started or silently dropped: GitHub Actions workflows (slice02/03),
release/checksum/manifest machinery (arc02), bundling a corpus checkout
(the `--corpus` flag exists; no `ipc2023-domains` checkout is vendored,
correctly), gitlab.com-reachability-from-a-GH-runner verification
(deferred to slice02 per slice-doc; this session's local macOS fetch did
reach gitlab.com successfully, which is a positive informal signal for
slice02 but not a substitute for the runner-based check), and upstream
contribution tickets for the macOS fixes (parallel work, not gated by this
slice, not started here). Nothing is missing from either list.
