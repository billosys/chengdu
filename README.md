# chengdu

[![build](https://github.com/billosys/chengdu/actions/workflows/build.yml/badge.svg)](https://github.com/billosys/chengdu/actions/workflows/build.yml)

[![][logo]][logo-large]

*We `make` PANDAs*

## Overview

CI build matrix and binary releases for the [PANDA (pandaPI) HTN planning
toolchain][panda] — Linux + macOS. Primary consumer: `wolong`.

This repo vendors pandaPIparser, pandaPIgrounder, and pandaPIengine in-tree,
builds all three from that source, and smoke-verifies the full
`parse → ground → solve → convert → verify` gate sequence against in-repo
fixtures. See `docs/design-v0.2.0/` for the current closed fork plan and
`docs/design-v0.1.0/` for the original design substrate.

## Install from the `chengdu` Release:

No build tools required. Download → checksum-verify → extract → run a
real `--verify` against the shipped fixtures, in 4 commands. Replace
`v0.2.0` with the release you want; asset names, `SHA256SUMS`, and this
command shape are frozen as of v0.1.0 — this is wolong's fetch spec.

**Linux (x86_64):**

```bash
curl \
  -LO https://github.com/billosys/chengdu/releases/download/v0.2.0/pandapi-v0.2.0-linux-x86_64.tar.gz \
  -LO https://github.com/billosys/chengdu/releases/download/v0.2.0/SHA256SUMS
sha256sum -c SHA256SUMS --ignore-missing
tar xzf pandapi-v0.2.0-linux-x86_64.tar.gz
```

**macOS (arm64):**

```bash
curl \
  -LO https://github.com/billosys/chengdu/releases/download/v0.2.0/pandapi-v0.2.0-macos-arm64.tar.gz \
  -LO https://github.com/billosys/chengdu/releases/download/v0.2.0/SHA256SUMS
shasum -a 256 -c SHA256SUMS --ignore-missing
tar xzf pandapi-v0.2.0-macos-arm64.tar.gz
```

Then make sure everything's working:

```bash
./pandaPIparser fixtures/domain.hddl fixtures/problem.hddl plan.htn && \
./pandaPIgrounder plan.htn plan.sas && \
./pandaPIengine plan.sas > plan.raw && \
./pandaPIparser -c plan.raw plan.txt && \
./pandaPIparser --verify fixtures/domain.hddl fixtures/problem.hddl plan.txt
```

On success, the last line is `Plan verification result: true` and the
command exits 0. `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`,
and `provenance.txt` are now in the current directory. See
`THIRD-PARTY-LICENSES` (downloadable from the same release) for
licensing, and `docs/license-audit-v0.2.0.md` for the current audit
behind it.


## Build from Source

If you don't want to use the pre-built binaries, you can build from scratch on
Linux or macOS by following the instructions in this section.

### Prerequisites

**Linux:**

```bash
sudo apt-get install -y g++ make cmake flex bison gengetopt zip git
```

**macOS:**

```bash
brew install gcc make cmake flex bison gengetopt zip
```

### Build + smoke test (4 commands)

```bash
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh
```

On success, `dist/<platform>/` (`linux-x86_64` or `macos-arm64`) contains
`pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, and `provenance.txt`.

Negative-gate check (missing file, broken syntax, broken reference,
provably-unsolvable — each a distinct outcome, none collapsed into a
generic failure):

```bash
./scripts/smoke-test.sh --negative
```

### Notes

- Source builds use the in-tree `pandaPI/` source and do not clone planner
  source.
- The macOS grounder compiler defaults to clang (`GROUNDER_CC=cc
  GROUNDER_CXX=c++`); override via those two env vars if you need brew gcc.
- `smoke-test.sh --corpus DIR` additionally runs the IPC 2023 Transport
  `pfile01` domain through the full chain, given an `ipc2023-domains`
  checkout at `DIR`. Optional — not part of the required gate.
- Vendored source identity is recorded in `vendor.env`: the current build
  source is the chengdu commit, and upstream SHAs are import identities.
  `pins.env` remains historical 0.1.0/import-point evidence only.

## Maintaining `chengdu`

### Continuous integration

`.github/workflows/build.yml` runs on every push, PR, and manual dispatch,
across the full support matrix:

- **`build` (Linux)** — matrixed over `ubuntu-22.04` (canonical — what
  the release workflow packages) and `ubuntu-24.04` (forward-compat
  check only).
- **`build (macos-15)`** — the macOS arm64 leg, pinned to `macos-15`
  (the oldest currently-maintained GA arm64 GitHub-hosted image;
  `macos-14` is deprecated, `macos-latest`/`macos-26` is the newest —
  `macos-15` is this project's macOS build/compat floor, chosen the same
  way `ubuntu-22.04` was for Linux).
- **`cross-compat` / `cross-compat-macos`** — the canonical-runner
  artifact (`ubuntu-22.04` / `macos-15`) proven to also run `--help`
  cleanly on the newest maintained runner of its platform
  (`ubuntu-24.04` / `macos-26`), on a real GitHub-hosted runner rather
  than an emulated one.

Every build leg runs the same three builds → `check-provenance.sh` →
smoke (positive + negative) sequence as the local commands above, with
the resulting `dist/<platform>/` (including `provenance.txt`) uploaded as
a workflow artifact per runner. `check-provenance.sh` fails the run if any
component's `chengdu_commit`, `source_prefix`, import identity,
`patches=none`, or compiler field does not match `vendor.env` and Git
state — the provenance file is no longer just attested, it's
CI-enforced. Two `readme-verbatim*` jobs
(`ubuntu-22.04` and `macos-15`) run this file's own prerequisite line and
four build commands, unmodified, on a clean runner per platform — if this
README and the workflow drift, those jobs go red. `actionlint` gates the
workflow file itself.

Brew-installed dependency versions (bison, flex, etc.) float with the
Homebrew formulae rather than being pinned — an accepted risk at 0.1.0;
the smoke gate is what catches any resulting breakage.

Both `build.yml` and `.github/workflows/release.yml` call the same
reusable workflow (`build-reusable.yml`) for this matrix — no build
logic is duplicated between "every push" and "on a release tag."

### Releases

`.github/workflows/release.yml` triggers on pushing a tag matching
`v*`. It runs the identical build+gate matrix described above; only if
every leg is green does it package per-platform tarballs (binaries +
`provenance.txt` + `fixtures/`), `SHA256SUMS`, `release-manifest.txt`
(the aggregated provenance manifest), and `THIRD-PARTY-LICENSES`, then
publish a GitHub Release — a red build has no path to a published release, by
construction (the publish job depends on the whole matrix succeeding).
Publishing is direct (the gates are the approval); switching to a
draft-then-promote model is a one-line change in
`scripts/publish-release.sh`. Re-running a tag's workflow after its
release already exists fails loudly rather than overwriting anything —
see the workflow file's header for the exact contract. Every release's
licensing is backed by an evidence-based linkage audit:
[`docs/license-audit-v0.2.0.md`](docs/license-audit-v0.2.0.md).

[//]: ---Named-Links---

[logo]: assets/images/anshun-bridge-y250.png
[logo-large]: assets/images/anshun-bridge.png
[panda]: https://panda-planner-dev.github.io/
