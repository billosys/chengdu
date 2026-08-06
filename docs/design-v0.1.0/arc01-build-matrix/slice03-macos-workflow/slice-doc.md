# chengdu arc01 / slice03 — macos-workflow — slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md) (v1.3). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).
> **This is arc01's final slice** — at its close, the arc's composition
> check runs (arc ledger rows A1–A4) and the arc closes formally.

## 1. Goal

Extend the slice02 workflow to macOS (Apple Silicon). At slice close, every
push builds and smoke-verifies all three pandaPI components on a pinned
arm64 macOS runner — brew deps, the three-patch macOS set, the clang
grounder override, positive + negative smoke gates — and uploads
`dist/macos-arm64/` artifacts alongside the Linux ones. The slice also
retires slice02's one residue *mechanically*: a `check-provenance.sh` gate
that validates provenance content against `pins.env` inside CI itself, on
every platform, every run — converting "CC attested the provenance file"
into "no run is green unless the provenance is right."

The macOS path's *scripts* are already field-proven (the operator's local
builds; slice01). The new variable is the **runner environment**: brew
paths, preinstalled toolchains, Xcode CLT versions. That is where this
slice's iteration budget should expect to go.

## 2. Scope

**In:**

- **macOS build leg** in `.github/workflows/build.yml`: an arm64 macOS
  runner added to the build matrix (or an equivalent job if matrix
  `include` gets contorted — CC's call, actionlint-clean either way), with
  a brew setup step **mirroring the README's macOS prereq line
  byte-identically**, then the same script sequence as Linux: fetch →
  three builds → smoke positive → smoke negative → upload
  `dist-macos-arm64-<runner>-<sha>`.
- **Runner pinning:** explicit versions, never `macos-latest` — same
  pins-not-floats discipline as everything else. Choose the **oldest
  maintained arm64 macOS runner** for the build leg (the compat-floor
  logic of OQ3, applied to macOS) and the newest for the forward-compat
  check below. Record the chosen versions and their rationale in the
  ledger evidence and the closing report.
- **`cross-compat-macos` job:** binaries built on the older pinned runner
  execute `--help` on the newer pinned runner — the macOS analogue of
  slice02's leg-crossing lesson (bubble-up 2b, applied at authoring time
  this round rather than discovered mid-slice).
- **`readme-verbatim-macos` job:** the README's macOS prereq line + the
  same five commands, byte-identical, on a clean macOS runner. This is
  the macOS half of project ledger row P4, discharged early.
- **`scripts/check-provenance.sh`** (routed from slice02's W-4/W-5
  residue, per the operator's acceptance): reads `dist/<platform>/provenance.txt`,
  validates every component block — SHA equals the `pins.env` pin,
  patch list equals the expected per-platform set (Linux: `0002` +
  bliss; macOS: those plus the two boruvka patches), compiler field
  non-empty — and exits nonzero with a named complaint on any mismatch.
  Wired as a step after smoke on **all** build legs, both platforms.
  bash 3.2, shellcheck clean, fail-loud, self-testable (a tamper mode or
  documented tamper test).
- **min-OS observation:** the macOS build leg records the produced
  binaries' minimum-OS/deployment target (`otool -l | grep -A2 LC_BUILD_VERSION`
  or equivalent) into the provenance file or the job log — *observed and
  recorded*, not yet a policy. The formal macOS compat-floor decision is
  arc02's (release packaging), and this observation is its input.
- README: CI section updated to name the macOS legs; badge unchanged.

**Out (disclosed, not dropped):**

- Releases, tags, checksums, manifest — arc02 (now consuming both
  platforms' artifacts + the CI-validated provenance).
- `macos-x86_64` — project-plan candidate only; GH Intel runners are
  deprecated; the decision (likely: decline) is arc02's, with this
  slice's runner survey as input.
- Universal (fat) binaries via `lipo` — not in the support matrix;
  record as an arc02-or-later idea only if the runner survey makes it
  cheap. Do not implement speculatively.
- Brew-version pinning for deps (bison/flex versions float with the brew
  formulae) — accepted risk at 0.1.0; the smoke gate catches breakage;
  note it in the CI section.
- Action major-version bumps for the Node20 deprecation (CDC-5) — arc02's
  workflow touch, unless a bump is forced by runner incompatibility
  mid-slice (then it's in scope, disclosed).

## 3. Constraints

Slice02's all carry forward: no build logic in YAML (the provenance gate
is a script, called by a step); byte-identical README mirroring for the
verbatim job; bash 3.2 + shellcheck for any script touched; action
versions pinned; workflow changes actionlint-clean. New here: macOS
runner minutes are free on public repos but slower to schedule — batch
iteration pushes deliberately rather than push-per-tweak.

## 4. Verification approach

As slice02: CI runs are primary evidence, linked per row. CC implements
and iterates against real runs. CDC verifies from the sandbox: public run
pages independently fetched; workflow + scripts read at authenticated
HEAD; `check-provenance.sh` re-run locally against sandbox-built Linux
dist (including a tamper test); README-verbatim diff re-derived; macOS
job-level claims verified via run page + the green-plus-fail-loud
inference (established in slice02's W-3). The operator's local Mac is
available for spot-checks on downloaded macOS artifacts if a row's
evidence wants it (optional, not load-bearing).

## 5. Exit criteria

The ledger's 8 rows at final status — see [`./ledger.md`](./ledger.md).
Shape: macOS leg green end-to-end (M-1, M-2); README's macOS path proven
on a clean runner (M-3); clang + patch-set fidelity on the runner (M-4);
forward-compat across arm64 runners (M-5); the provenance gate live on
all legs with a proven failure mode (M-6); docs current (M-7); no
regression on the Linux legs (M-8).
