# chengdu

[![build](https://github.com/billosys/chengdu/actions/workflows/build.yml/badge.svg)](https://github.com/billosys/chengdu/actions/workflows/build.yml)

[![][logo]][logo-large]

*We `make` PANDAs*

## Overview

CI build matrix and binary releases for the PANDA (pandaPI) HTN planning
toolchain — Linux + macOS. Primary consumer: `wolong`.

This repo mechanizes the PANDA Runbook: it fetches pandaPIparser,
pandaPIgrounder, and pandaPIengine from pinned upstream commits, applies a
small set of named patches, builds all three, and smoke-verifies the full
`parse → ground → solve → convert → verify` gate sequence against in-repo
fixtures. See `docs/design-v0.1.0/` for the design substrate and `CLAUDE.md`
for the patches-not-fork policy.

## Prerequisites

**Linux:**

```bash
sudo apt-get install -y g++ make cmake flex bison gengetopt zip git
```

**macOS:**

```bash
brew install gcc make cmake flex bison gengetopt zip
```

## Build + smoke test (5 commands)

```bash
./scripts/fetch-upstream.sh
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

## Continuous integration

`.github/workflows/build.yml` runs on every push, PR, and manual dispatch,
across the full support matrix:

- **`build` (Linux)** — matrixed over `ubuntu-22.04` (canonical — what
  arc02 will package) and `ubuntu-24.04` (forward-compat check only).
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

Every build leg runs the same fetch → three builds → `check-provenance.sh`
→ smoke (positive + negative) sequence as the local commands above, with
the resulting `dist/<platform>/` (including `provenance.txt`) uploaded as
a workflow artifact per runner. `check-provenance.sh` fails the run if any
component's SHA, patch list, or compiler field doesn't match what
`pins.env` and the platform actually require — the provenance file is no
longer just attested, it's CI-enforced. Two `readme-verbatim*` jobs
(`ubuntu-22.04` and `macos-15`) run this file's own prerequisite line and
five build commands, unmodified, on a clean runner per platform — if this
README and the workflow drift, those jobs go red. `actionlint` gates the
workflow file itself.

Brew-installed dependency versions (bison, flex, etc.) float with the
Homebrew formulae rather than being pinned — an accepted risk at 0.1.0;
the smoke gate is what catches any resulting breakage.

## Notes

- `fetch-upstream.sh --source pandadealer` clones the vendored IPC 2023
  competition snapshot instead of the canonical repos + submodules, for
  environments that cannot reach gitlab.com (the cpddl submodule's host).
  **This fallback is fetch-only**: the vendored snapshot ships no patch
  files, and its grounder/engine sources don't build cleanly on a modern
  Linux/GCC toolchain (pre-final concepts-TS syntax, a missing
  `<cstdint>` include) — `build-grounder.sh` refuses to run against it
  with a pointer back to this note. Use it to inspect or diff the
  competition snapshot, not to produce binaries; canonical mode is the
  only buildable source today.
- The macOS grounder compiler defaults to clang (`GROUNDER_CC=cc
  GROUNDER_CXX=c++`); override via those two env vars if you need brew gcc.
- `smoke-test.sh --corpus DIR` additionally runs the IPC 2023 Transport
  `pfile01` domain through the full chain, given an `ipc2023-domains`
  checkout at `DIR`. Optional — not part of the required gate.
- Upstream is consumed at pinned SHAs (`pins.env`), never forked silently:
  every delta is a named patch in `patches/`, applied by script.

[//]: ---Named-Links---

[logo]: assets/images/anshun-bridge-y250.png
[logo-large]: assets/images/anshun-bridge.png
