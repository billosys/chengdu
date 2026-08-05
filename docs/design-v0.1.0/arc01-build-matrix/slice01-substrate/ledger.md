# Slice 01: substrate

> Ledger per `LEDGER-DISCIPLINE.md` (v2.0), Section A. All rows open at
> slice start, 2026-08-05. Closer: CC. Verifier: CDC (Linux reproduction in
> the Cowork sandbox + independent re-walk). Platforms: rows marked *(both)*
> require evidence from `linux-x86_64` **and** `macos-arm64`.

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | `pins.env` exists and carries repo URLs + full-length commit SHAs for parser, grounder, engine, and the PandaDealer fallback ref, matching the runbook's provenance appendix (short forms `88c0995`, `b04ff7b`, `810f043`, `fde3245`). | `grep -E '^(PARSER\|GROUNDER\|ENGINE\|PANDADEALER)_(URL\|SHA)=' pins.env` and compare SHAs to runbook appendix | correctness | arc-plan §2; runbook §9 | open | | full 40-char SHAs in the file; short forms only for human reference |
| F-2 | `scripts/fetch-upstream.sh` produces `upstream/` checkouts whose `git rev-parse HEAD` equals each pin exactly (grounder incl. initialized submodules), and `--source pandadealer` produces the vendored-snapshot layout instead. | run it; `for d in upstream/*; do git -C $d rev-parse HEAD; done` vs `pins.env`; rerun with `--source pandadealer`; mismatch case exits nonzero (test by tampering a pin) | serious | arc-plan §2, OQ1 | open | | gitlab reachability is env-dependent; script must FAIL LOUDLY, not degrade silently |
| F-3 | `scripts/build-parser.sh` produces `dist/<platform>/pandaPIparser` that exits 0 on `--help`. *(both)* | run script; `dist/*/pandaPIparser --help; echo $?` | serious | runbook §4.1 | open | | macOS: BISON/FLEX brew-path handling per runbook §2 |
| F-4 | `scripts/build-grounder.sh` produces `dist/<platform>/pandaPIgrounder` that exits 0 on `--help`, applying: upstream `0002` cpddl patch (both), upstream `0001-*` boruvka patches (macOS, from the grounder checkout), chengdu's bliss patch (both), clang default on macOS. *(both)* | run script; check binary; `grep -c 'applying' build log` ≥ expected patch count per platform | serious | runbook §4.2 + §2-macOS; arc-plan OQ2 (resolved: clang) | open | | the rm-rf/unzip bliss recipe trap (runbook §2) must be handled — patch after extract, build via inner make |
| F-5 | `scripts/build-engine.sh` produces `dist/<platform>/pandaPIengine` that exits 0 on `--help` (cmake against `src/`, Release). *(both)* | run script; check binary | serious | runbook §4.3 | open | | `cmake ../src`, not `..` |
| F-6 | No inline mutation of upstream sources: every delta is a committed `patches/*.patch` applied by script. The bliss `__DATE__` fix exists as `patches/bliss-0.73-cxx11-string-literal.patch` and no build script contains an inline `sed` against upstream files. | `ls patches/`; `grep -rn 'sed.*-i' scripts/` returns nothing targeting `upstream/` | correctness | project-plan non-goals (patches-not-fork) | open | | |
| F-7 | `scripts/smoke-test.sh` on in-repo fixtures: the valid minimal pair passes all gates — validate, parse, ground, solve (`Status: Solved`), convert, verify (`result: true`) — and the script exits 0 with a per-gate PASS summary. *(both)* | run it; check exit code + summary lines | serious | runbook §6; arc ledger A1/A4 | open | | fixtures are the runbook's verified minimal pair — byte-for-byte |
| F-8 | Smoke negative gates, each with a distinct reported reason and correct detection: missing file → parser exit 2 path; broken-syntax fixture → 255 path; broken-reference fixture → 255 path; unsolvable fixture → engine exit 0 **but** `Status: Proven unsolvable` recognized as UNSOLVABLE (not success, not generic failure). Overall script exit nonzero when any gate misbehaves. *(both)* | run `smoke-test.sh --negative` (or equivalent mode); inspect the four outcomes; tamper one expectation to confirm the harness itself can fail | correctness | runbook §5 table + §6 trap; project DoD | open | | this row is the confident-plan-for-unsolvable firewall at build time |
| F-9 | After a full build, `dist/<platform>/provenance.txt` names: each component's upstream SHA, every patch applied, and the compiler used. *(both)* | `cat dist/*/provenance.txt`; cross-check against `pins.env` and build log | correctness | arc-plan "leaves for arc02" | open | | format simple `key=value`; arc02 will consume it |
| F-10 | `shellcheck` reports zero findings across `scripts/*.sh` (suppressions, if any, carry inline justification comments). | `shellcheck scripts/*.sh; echo $?` | polish | slice-doc §3 | open | | shellcheck available: brew (macOS) / apt (Linux/sandbox) |
| F-11 | Repo hygiene: `.gitignore` covers `upstream/`, `dist/`, scratch dirs; `README.md` documents a clean-machine local build+smoke in ≤ 5 commands that actually work. | `cat .gitignore`; count README commands; CDC executes them verbatim on the Linux reproduction | polish | project ledger P4 (precursor) | open | | README is the seed of arc02's install docs, not the final version |

## What Worked

_(At slice close.)_

## Closure

_Open. Rows: 11. Done: 0. Deferred: 0. No-op: 0._
