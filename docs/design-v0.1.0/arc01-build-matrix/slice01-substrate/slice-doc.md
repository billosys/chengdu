# chengdu arc01 / slice01 — substrate — slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Mechanize the PANDA Runbook. At slice close, the chengdu repo contains
pinned upstream references, the patch set as committed artifacts, and shell
scripts that fetch, build, and smoke-verify all three pandaPI components —
runnable identically on a Linux box and a macOS (Apple Silicon) box with no
CI involved. This slice is transcription-plus-hardening of an already-
verified procedure, not design: every command has either been reproduced in
the 2026-08-05 sandbox session (Linux) or field-run by the operator (macOS).

## 2. Scope

**In:**

- `pins.env` — the upstream pins, sourceable by scripts: repo URLs and
  commit SHAs for pandaPIparser (`88c0995`), pandaPIgrounder (`b04ff7b`),
  pandaPIengine (`810f043`), plus the `ipc2023-htn/PandaDealer` ref
  (`fde3245`) as the documented fallback source.
- `scripts/fetch-upstream.sh` — clones the three canonical repos at exactly
  the pinned SHAs into `upstream/` (grounder with submodules — cpddl lives
  on gitlab.com; that reachability is the environment's problem, and
  `--source pandadealer` is the fallback for networks that block it).
  Verifies post-checkout SHAs against the pins and fails loudly on mismatch.
- `patches/bliss-0.73-cxx11-string-literal.patch` — the `bliss.cc:62`
  `__DATE__` fix as a proper committed patch (currently a sed in the
  runbook). Applied on **both** platforms for a single code path — the fix
  is valid C++ everywhere; only clang enforces it. The two boruvka macOS
  patches are NOT vendored here: they ship inside the pandaPIgrounder repo
  itself and the build script applies them from the checkout (macOS only,
  matching upstream intent).
- `scripts/build-parser.sh`, `scripts/build-grounder.sh`,
  `scripts/build-engine.sh` — runbook §4 + §2-macOS, mechanized. Grounder
  compiler defaults to clang on macOS (`CXX=c++ CC=cc`, the field-confirmed
  path; arc-plan OQ2), overridable via env. Outputs land in
  `dist/<platform>/` (`linux-x86_64`, `macos-arm64`).
- Each build script's final act: append to `dist/<platform>/provenance.txt`
  — component, upstream SHA, patches applied, compiler — the byproduct
  arc02's release manifest will package (arc-plan "leaves for arc02").
- `scripts/smoke-test.sh` — runbook §5–§6 as executable gates, run against
  **in-repo fixtures** (`fixtures/`): the runbook's minimal pair, its
  broken-reference and broken-syntax variants, and the circular-precondition
  unsolvable variant. Gates: validate → parse → ground → solve → convert →
  verify, with the runbook's exact exit-code contract (parser 0/2/255,
  verifier 0/1/2) and the **`Status:`-line trap** — `Proven unsolvable`
  with exit 0 must be recognized as unsolvable, never as success-with-no-plan
  or as failure. Nonzero exit on any gate deviation. An optional
  `--corpus DIR` flag runs the Transport pfile01 loop when an
  `ipc2023-domains` checkout is present; not required for the gate.
- `README.md` (concise) — local build in ≤ 5 commands, per project ledger
  row P4's eventual shape. `.gitignore` for `upstream/`, `dist/`, scratch.

**Out (disclosed, not dropped):**

- GitHub Actions workflows — slices 02/03.
- Release/tag machinery, checksums, manifest packaging — arc02.
- Corpus repos as a dependency — optional flag only, per above.
- gitlab.com-reachability-from-GH-runners verification (arc-plan OQ1) —
  deferred to slice02, where a runner exists to test from. Re-entry: slice02
  slice-doc.
- Upstream contribution tickets for the macOS fixes — parallel work, not
  gated by this slice.

## 3. Constraints

- Scripts are bash, `#!/usr/bin/env bash`, `set -euo pipefail`, and must run
  on **stock macOS bash 3.2** (no associative arrays, no `mapfile`) as well
  as modern Linux bash — the operator's machine and the runners both count.
- `shellcheck` clean (any suppression carries an inline justification).
- The patches-not-fork policy (project CLAUDE.md): no modification of
  upstream sources except via named patch files applied by script.
- No gate softening: the smoke test's failure modes must stay distinct — a
  collapsed "something failed" message is a spec violation, not a style
  choice.

## 4. Verification approach

Two-platform split, mirroring how the substrate was originally verified:
CC implements and runs the full ledger's Verify commands on the operator's
macOS machine (evidence: `attested`). The Linux reproduction runs in the
Cowork cloud sandbox (clean Ubuntu 24.04 — the same environment the runbook
was verified in), driven by the CDC session, which upgrades rows to
`reproduced`. CDC verification also re-walks the ledger per
`LEDGER-DISCIPLINE.md` Section A.

## 5. Exit criteria

The ledger's 11 rows at final status — see [`./ledger.md`](./ledger.md).
Summary shape: pins + fetch correct (F-1, F-2); three components build on
both platforms (F-3–F-5); patch discipline holds (F-6); smoke gates pass
positive and negative fixtures including the unsolvable trap (F-7, F-8);
provenance byproduct emitted (F-9); shellcheck clean (F-10); repo hygiene
(F-11).

## 6. Reference: the exit-code contract (from the runbook, evidence: reproduced)

| Tool | 0 | 1 | 2 | 255 |
|------|---|---|---|-----|
| `pandaPIparser` (parse/validate) | clean | — | input file missing | syntax or semantic error |
| `pandaPIparser --verify` | plan verified | plan well-formed but wrong | malformed plan file | HDDL parse error |
| `pandaPIengine` | always (!) | — | — | — |

Engine success/failure is carried in the `Status:` line: `Solved` vs
`Proven unsolvable` — **both exit 0**. The full tables and transcripts:
"PANDA Runbook — Setup, Validation, and Execution" §5–§6 (operator's
Dropbox, `Duncan's Research/LLMs/`).
