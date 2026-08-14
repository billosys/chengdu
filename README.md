# chengdu

[![build](https://github.com/billosys/chengdu/actions/workflows/build.yml/badge.svg)](https://github.com/billosys/chengdu/actions/workflows/build.yml)

[![][logo]][logo-large]

*We `make` PANDAs*

## Overview

chengdu is the public fork and release home for the pandaPI HTN planning
toolchain. It keeps the product source in tree under `pandaPI/` and builds the
three-stage HDDL workflow:

1. `pandapi-parser` reads HDDL domain/problem files and writes a parser
   artifact.
2. `pandapi-grounder` reads the parser artifact and writes a grounded planner
   artifact.
3. `pandapi-engine` reads the grounded artifact and writes a plan when search
   finds one.

The active source roots are `pandaPI/parser`, `pandaPI/grounder`,
`pandaPI/engine`, and `pandaPI/runtime`. The runtime provides shared
managed-process behavior: clear stdout/stderr ownership, final
`PANDAPI_STATUS` records, color/TTY policy, version/provenance output, and
consistent local/CI behavior through Make.

## Documentation

Start here:

- [Documentation home](docs/index.md)
- [HTN and HDDL onboarding](docs/tutorial/01-htn-hddl-onboarding.md)
- [First project workflow](docs/tutorial/02-first-project-workflow.md)
- [Model a software feature as an HTN](docs/tutorial/03-model-a-feature-as-htn.md)
- [Write HDDL files for a small HTN project](docs/tutorial/04-write-hddl-files.md)
- [Managed process integration](docs/managed-process.md)
- [CLI command reference](docs/reference/cli.md)
- [Migration from 0.2.0 to 0.3.0](docs/migration.md)
- [Architecture and source quality](docs/architecture.md)

## Published Release Install

The currently published install example is v0.2.0. These assets use the
published v0.2.0 release shape and checksum flow.

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

The extracted release contains planner executables, fixtures,
`provenance.txt`, and release license files. See
[`THIRD-PARTY-LICENSES`](https://github.com/billosys/chengdu/releases/download/v0.2.0/THIRD-PARTY-LICENSES).

The 0.3.0 release assets are not published yet. 0.3.0 checksums, manifest,
license/NOTICE bundles, and wolong fetch/install proof remain release
preparation work until the release. When released, 0.3.0 install instructions
should be verified against the published assets.

## Build From Source

Use this path for the current 0.3.0 source tree.

### Prerequisites

**Linux:**

```bash
sudo apt-get install -y g++ make cmake flex bison gengetopt zip git
```

**macOS:**

```bash
brew install gcc make cmake flex bison gengetopt zip
```

### Build and Smoke Test

```bash
make readme-verbatim
```

On success, `./bin/` contains `pandapi-parser`, `pandapi-grounder`, and
`pandapi-engine`.

Run the local pipeline:

```bash
tmp="$(mktemp -d)"

./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl

./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/minimal.sas" \
  "$tmp/minimal.htn"

./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/minimal.plan" \
  "$tmp/minimal.sas"

cat "$tmp/minimal.plan"
```

Run positive and negative smoke gates:

```bash
make smoke
make smoke-negative
```

Run the full local test suite:

```bash
make test
```

Strict developer mode persists an ignored local Make config under
`build/make/` and enables stricter local gates when developer-only
dependencies such as Catch2 are installed:

```bash
make enable-dev-strict
make dev-strict-status
make test
```

Standard source builds and release users do not need those test-only
dependencies. Run `make help` for the current build, test, quality, CI, and
release-preparation entrypoints.

## Maintaining chengdu

Make is the local and CI entrypoint. CI runs Make-backed build, test, smoke,
format, safety, actionlint, provenance, and README-verbatim gates on Linux and
macOS. Release workflows reuse the same Make-backed build matrix before any
publication step.

For architecture, dependency, source-quality, generated-code, and
third-party-boundary details, see
[Architecture and source quality](docs/architecture.md). For command-name and
behavior changes between 0.2.0 and 0.3.0, see
[Migration from 0.2.0 to 0.3.0](docs/migration.md).

[//]: ---Named-Links---

[logo]: assets/images/anshun-bridge-y250.png
[logo-large]: assets/images/anshun-bridge.png
