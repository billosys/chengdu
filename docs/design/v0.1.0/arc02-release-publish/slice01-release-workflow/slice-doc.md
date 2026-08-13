# chengdu arc02 / slice01 — release-workflow — slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1).
> Parent: [`../arc-plan.md`](../arc-plan.md) (v1.0). Ledger:
> [`./ledger.md`](./ledger.md). Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Build the release machinery. At slice close, pushing a tag `v*` runs the
identical build+gate logic as every push (reused via `workflow_call`,
not copy-pasted), packages per-platform tarballs with checksums and an
aggregated provenance manifest, and publishes a GitHub Release from a
notes template carrying the provenance block and compat floors — with
the refuse-to-publish property *demonstrated*, not asserted: a
deliberately-broken build under a test tag must produce a red run and
no release. The slice also takes the CDC-5 action-version bumps, since
this is the planned workflow touch.

What this slice does NOT do: cut the real `v0.1.0` (that is slice02's
closing act, after the license audit), or write consumer install docs.

## 2. Scope

**In:**

- **Workflow refactor for reuse:** extract the build+gate matrix into a
  reusable workflow (`workflow_call`) consumed by both the existing
  push/PR workflow and the new release workflow. The no-logic-in-YAML
  rule holds — the refactor moves *callers*, not logic; scripts remain
  the implementation. Existing CI behavior must be provably unchanged
  (same jobs, same gates, matrix green).
- **`.github/workflows/release.yml`:** triggers on tag push `v*`;
  calls the reusable build; then (only after every leg is green)
  packages, checksums, manifests, and publishes via the GitHub CLI/API
  with the workflow's own token. Sequential structure IS the gate: the
  publish step is unreachable past a failure. Per arc-plan RQ1, direct
  publish is the default; the draft-mode toggle documented inline.
- **`scripts/package-release.sh`:** locally runnable (bash 3.2,
  shellcheck): takes a tag + the `dist/` trees, emits
  `pandapi-<tag>-<platform>.tar.gz` (each: 3 binaries +
  `provenance.txt`), `SHA256SUMS` covering all assets, and
  `release-manifest.txt` aggregating per-platform provenance (component
  SHAs, patch lists, compilers) + the chengdu commit + tag. Reuses
  `check-provenance.sh` as a pre-package gate.
- **Release-notes template** (`.github/release-notes.md.in` or similar):
  filled by script with tag, manifest summary, and the compat floors
  from arc-plan D2/D3 (macOS 15+ arm64; glibc 2.35+ x86_64).
- **Refuse-to-publish demonstration:** on a working branch, break a
  fixture (or gate expectation), push a test tag (e.g.
  `v0.0.0-rc-refusal-test`), observe red run + no release; restore,
  document, clean up the test tag. This is project ledger P2's
  building-block evidence, produced deliberately.
- **CDC-5:** bump `actions/checkout`, `upload-artifact`,
  `download-artifact` to current majors; a green run free of Node20
  deprecation warnings.
- **Happy-path test tag:** a pre-release-marked test tag (e.g.
  `v0.0.1-rc1`) exercising the full publish path end-to-end, so slice02
  can cut `v0.1.0` with zero new machinery. Cleaned up or clearly
  marked pre-release — CC's call, disclosed.

**Out (disclosed, not dropped):**

- License/linkage audit, `THIRD-PARTY-LICENSES` — slice02 (and the
  reason the real `v0.1.0` waits).
- README "Install from release" consumer docs — slice02.
- D4's project-plan candidate changes — slice02.
- Fallback retire-into-releases re-evaluation — slice02.
- Any signing/attestation (sigstore, GH artifact attestations) —
  recorded as a future-work idea only; not 0.1.0 scope.

## 3. Constraints

All standing constraints (no build logic in YAML; bash 3.2; shellcheck;
actionlint; pinned action majors; fail-loud with named messages;
byte-identical README mirroring untouched — do not break the verbatim
jobs). New: the release workflow must be idempotent-safe (re-running a
tag's workflow must not duplicate or corrupt a published release —
document the behavior, whatever it is, in the workflow header).

## 4. Verification approach

As established: CI runs primary, linked per row; public run pages +
release pages independently fetchable by CDC; `package-release.sh`
reproduced locally by CDC against sandbox-built dist trees (tarball
contents, checksum verification, manifest reconciliation vs `pins.env`);
refusal evidence verified by run page + releases-list absence; the
notes template rendering checked on the happy-path test release.

## 5. Exit criteria

The ledger's 8 rows — see [`./ledger.md`](./ledger.md). Shape: refactor
with no CI behavior change (G-1, G-7); packaging script correct and
tamper-resistant locally (G-2); test-tag release publishes with full
asset set (G-3); refusal demonstrated (G-4); notes render with
provenance + floors (G-5); CDC-5 clean (G-6); workflow docs current
(G-8).
