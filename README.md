# chengdu

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

## Notes

- `fetch-upstream.sh --source pandadealer` clones the vendored IPC 2023
  competition snapshot instead of the canonical repos + submodules, for
  environments that cannot reach gitlab.com (the cpddl submodule's host).
- The macOS grounder compiler defaults to clang (`GROUNDER_CC=cc
  GROUNDER_CXX=c++`); override via those two env vars if you need brew gcc.
- `smoke-test.sh --corpus DIR` additionally runs the IPC 2023 Transport
  `pfile01` domain through the full chain, given an `ipc2023-domains`
  checkout at `DIR`. Optional — not part of the required gate.
- Upstream is consumed at pinned SHAs (`pins.env`), never forked silently:
  every delta is a named patch in `patches/`, applied by script.
