# Arc04 Substrate Integration Readiness

Date: 2026-08-09

Arc: `arc04-shared-runtime-substrate`

## Executive Summary And Verdict

Arc04 is ready for arc-level CDC close after Slice06 CDC verification.

The accepted Arc04 substrate is buildable, tested, documented, and still
inert with respect to parser, grounder, engine, release packaging, workflows,
and wolong-facing behavior. It gives Arc05 the shared `pandapi::runtime`
facilities needed for per-binary managed-process adoption, and it gives Arc06
the release, license, NOTICE, test-only exclusion, CI, and wolong proof inputs
needed after Arc05 changes executable behavior.

This readiness verdict is deliberately narrow. Arc04 does not prove executable
managed-process conformance. That proof remains Arc05 work through black-box
contract fixture adoption and Arc06 release-package verification.

## Evidence Inputs

- Arc02 combined library recommendations:
  `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`.
- Arc03 normative managed-process contract:
  `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`.
- Arc03 contract-test matrix:
  `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`.
- Arc04 slice01 runtime-build-skeleton and CDC verification.
- Arc04 slice02 status-result-core and CDC verification.
- Arc04 slice03 diagnostics-status-io and CDC verification.
- Arc04 slice04 cli-tty-provenance-core and CDC verification.
- Arc04 slice05 fixture-and-seam-test-substrate and CDC verification.

## Runtime Surface Inventory

Runtime root: `pandapi-runtime/`.

Namespace: `pandapi::runtime`.

CMake target: `pandapi_runtime`, with alias target `pandapi::runtime`.

Build entrypoints:

- `scripts/build-runtime.sh`: builds `pandapi-runtime/` and runs CTest.
- `scripts/build-all.sh`: build-only local probe that runs runtime, parser,
  grounder, and engine builds in sequence.

Public headers under `include/pandapi/runtime`:

- `runtime.hpp`
- `status.hpp`
- `result.hpp`
- `status_io.hpp`
- `cli_policy.hpp`
- `tty.hpp`
- `provenance.hpp`
- `fixture.hpp`
- `normalization.hpp`
- `process_fixture.hpp`

Implementation sources under `pandapi-runtime/src`:

- `runtime.cpp`
- `status.cpp`
- `status_io.cpp`
- `cli_policy.cpp`
- `tty.cpp`
- `provenance.cpp`
- `fixture.cpp`
- `normalization.cpp`
- `process_fixture.cpp`

Tests under `pandapi-runtime/tests`:

- `runtime_smoke.cpp`
- `status_result_smoke.cpp`
- `status_io_smoke.cpp`
- `cli_tty_provenance_smoke.cpp`
- `fixture_substrate_smoke.cpp`
- `fixture_catch2_smoke.cpp`

Documentation surface: `pandapi-runtime/README.md` now records the final
integration readiness boundary, build-all probe, Arc05 handoff, Arc06 handoff,
and no binary adoption state.

## Arc03 Coverage Map

| Arc03 area | Arc04 runtime facility | Runtime files | Readiness |
|------------|------------------------|---------------|-----------|
| status/result | Status vocabulary, status class, component, surface disposition, exit-code mapping, dynamic signal mapping, local result facade | `status.hpp`, `result.hpp`, `status.cpp`, `status_result_smoke.cpp` | Ready for Arc05 adoption behind binary top-level error mapping. |
| diagnostics/status I/O | Typed `StatusRecord`, stdout/stderr status-stream legality, `PANDAPI_STATUS` writer/parser, final flush, partial-output vocabulary | `status_io.hpp`, `status_io.cpp`, `status_io_smoke.cpp` | Ready for Arc05 final-status emission and status parsing tests. |
| stdout/stderr | Output role vocabulary, status-stream legality, ANSI rejection in status fields, flush behavior | `status.hpp`, `status_io.hpp`, `tty.hpp` | Ready for Arc05 stream ownership wiring. |
| CLI/TTY/provenance | Common options vocabulary, command identity, status/output conflict checks, TTY/color controls, provenance/version formatters | `cli_policy.hpp`, `tty.hpp`, `provenance.hpp`, matching sources/tests | Ready for Arc05 CLI migration design and compatibility tests; CLI11 remains gated. |
| fixture | Typed fixture records for command, argv, environment, stdin, stream roles, artifacts, expected exit, wait status, final status, normalization, and safety policy | `fixture.hpp`, `fixture.cpp`, `fixture_substrate_smoke.cpp` | Ready as Arc05 fixture schema substrate. |
| normalization | Path tokens, LF line endings, ANSI detection/stripping, duration/timestamp/build metadata predicates, errno, signals, structured comparison | `normalization.hpp`, `normalization.cpp`, fixture tests | Ready for Arc05 golden and semantic comparisons. |
| process observation | Local Linux/macOS process observation for stdout, stderr, exit code, wait status, timeout disposition, cleanup state | `process_fixture.hpp`, `process_fixture.cpp`, fixture tests | Ready for small-stdin CI-safe fixtures, with the full-duplex caveat below. |

## No Binary Adoption And Protected Surface Proof

No binary adoption occurred in Arc04. The runtime exists outside `pandaPI/`,
and parser, grounder, and engine do not link to or call `pandapi::runtime`
helpers yet.

Protected surfaces remain unchanged by this slice:

- no edits under `pandaPI/`;
- no parser, grounder, or engine managed-process conformance fixtures beyond
  existing build and smoke gates;
- no `pandapi-*` wrapper, symlink, copied binary, renamed binary, release entry
  point, package change, or inherited-name migration;
- no workflow change;
- no top-level README change;
- no release manifest, release asset, `vendor.env`, `pins.env`, `dist`,
  `build`, or `release` source change;
- no `scripts/package-release.sh` or `scripts/publish-release.sh` change;
- no wolong-facing release shape change.

## Arc02 Dependency Gate State

| Dependency area | Arc02 disposition | Actual Arc04 state |
|-----------------|-------------------|--------------------|
| standard-library | adopted baseline | Adopted. Runtime uses C++17 standard-library vocabulary and local facades. |
| fmt | adopted only behind diagnostics/process facade | Not imported in Arc04. The diagnostics/status I/O facade remains standard-library-only and keeps the gate available for later use. |
| CLI11 | selected candidate behind golden process tests | Not imported in Arc04. CLI policy helpers exist, but parser/grounder/engine CLI migration remains Arc05 work. |
| Catch2 | optional/local/test-only seam coverage | Optional local CMake test gate only. Default runtime build passes when Catch2 is absent. |
| `tl::expected` | pilot only behind local status/result facade | Not imported. `StatusResult<T>` is local and standard-library-only. |
| reproc++ | conditional pilot only behind accepted child-process adapter | Not imported. Slice05 uses a local POSIX observation substrate for tests. |
| `nlohmann/json` | held | Not imported; `PANDAPI_STATUS` is tagged text. |
| Abseil | rejected for 0.3.0 foundations | Not imported. |
| Boost.Process | rejected as first process dependency | Not imported. |
| GSL | held globally, narrow helper pilot only by evidence | Not imported. |
| google/benchmark | held | Not imported. |
| SAT, BDD, CUDD | held behind dedicated optional-capability work | Not enabled or imported by Arc04. |
| H2, cpddl internals | held behind supported-surface and boundary decisions | No dependency-internal work was done. |

No held or rejected library may re-enter through convenience, build
availability, hidden option paths, or inherited optional code.

## Arc05 Adoption Map

Arc05 adoption map:

Arc05 should adopt one binary at a time, with current behavior captured before
each migration and the accepted Arc03 behavior proved after each migration.
Recommended order:

1. Parser, because parser normal HDDL parse is the upstream source of the
   pipeline and exercises canonical/inherited command identity, output
   destination, parse errors, input unavailable, output unavailable, and
   `PANDAPI_STATUS` without engine search complexity.
2. Grounder, because grounder normal `.htn` grounding consumes parser output,
   exercises compatibility positional output, and must fence H2 plus
   cpddl/FAM before pipeline composition claims are made.
3. Engine, because engine normal search has the broadest outcome set: solved,
   domain no plan, malformed input, timeout/resource/interruption,
   signal_terminated observation, unsupported SAT, future BDD/CUDD, legacy
   interactive, and experimental translation fencing.

Parser normal:

- canonical command: `pandapi-parser`;
- inherited compatibility command: `pandaPIparser`;
- supported surface: normal HDDL domain/problem parse;
- runtime inputs: status/result, status I/O, CLI policy, TTY/color,
  provenance, normalization, fixture records;
- required proof: file-backed and stdout-backed output, CLI usage errors,
  unavailable input/output, malformed HDDL, inherited positional output
  compatibility, no default deprecation warning.

Grounder normal:

- canonical command: `pandapi-grounder`;
- inherited compatibility command: `pandaPIgrounder`;
- supported surface: normal `.htn` grounding;
- runtime inputs: same shared status, stream, CLI, provenance, fixture, and
  normalization helpers;
- required proof: file-backed and stdout-backed output, parser-to-grounder
  composition, missing or malformed `.htn`, output failures, H2 experimental
  fencing, cpddl/FAM legacy fencing, no default deprecation warning.

Engine normal:

- canonical command: `pandapi-engine`;
- inherited compatibility command: `pandaPIengine`;
- supported surface: normal search;
- runtime inputs: status/result, status I/O, TTY/color, provenance, fixture
  process observation, normalization, wait-status and signal helpers;
- required proof: solved `ok`, unsolvable `domain_no_plan`, malformed engine
  input, timeout/resource/interruption, signal_terminated, unsupported SAT,
  future BDD/CUDD, legacy interactive fencing, experimental translation
  fencing, no default deprecation warning.

Canonical `pandapi-*` entry points remain Arc05 work. Inherited `pandaPI*`
compatibility remains required for 0.3.0, and compatibility invocation must
not emit a default deprecation warning because that would change stream
behavior.

## Arc05 Contract Fixture Plan

Arc05 should turn the Arc03 contract-test matrix into executable fixtures in
these categories:

- positive parser, grounder, engine normal surfaces, including file-backed and
  stdout-backed artifacts;
- negative fixtures for CLI usage errors, missing input, unreadable input,
  unavailable output, malformed HDDL, malformed `.htn`, and malformed engine
  input;
- status fixtures for exit code, status name, status class, required
  `PANDAPI_STATUS` fields, final flush, and stderr/stdout legality;
- stream fixtures for stdout artifact ownership, informational stdout,
  tagged-status stdout legality, stderr diagnostics, and absence of diagnostic
  regex classification;
- color fixtures for `--color=auto`, `--color=always`, `--color=never`,
  `--no-color`, `--no-colour`, `NO_COLOR`, TTY, no-TTY, files, pipes, and
  supervised output;
- signal fixtures for binary-owned interruption and supervisor-observed
  signal termination;
- timeout/resource fixtures with CI-safe bounds, cleanup policy, artifact
  state, and binary-owned versus harness-owned responsibility made explicit;
- provenance fixtures for `--help`, `--version`, `--provenance`, canonical
  command names, inherited invoked command names, source prefix, upstream
  commit, chengdu build commit, license, and NOTICE fields;
- non-supported surface probes for parser legacy helpers, grounder H2,
  grounder cpddl/FAM, engine interactive, engine translation, engine SAT, and
  engine BDD/CUDD.

Catch2 seam tests can prove internal helpers. They cannot replace black-box
contract fixture proof for command execution, stdout/stderr ownership,
artifacts, process exit, or release-package behavior.

## Slice05 Stdin And Full-Duplex Caveat

Slice05 CDC verification accepted the process runner for current zero/small
stdin fixtures but recorded a caveat: `stdin` is represented in the fixture
model, while the runner writes stdin synchronously before entering its
stdout/stderr drain loop.

Arc05 must not treat `run_process_fixture` as a general full-duplex transport
for large stdin payloads. Re-entry condition: before stdin-heavy fixtures rely
on it, add focused regression coverage or move stdin writes into the same
nonblocking observation loop that drains stdout and stderr.

## Arc06 Handoff

Arc06 must verify release shape and wolong behavior after Arc05 adoption:

- release shape: tarballs, `SHA256SUMS`, `release-manifest.txt`,
  `provenance.txt`, source prefixes, and inherited-name compatibility;
- wolong proof: fetch, checksum, extract, install/run path, and migration
  examples using both canonical and inherited names where required;
- behavior-change table inputs: command names, status/exit mapping,
  stdout/stderr ownership, status records, color/TTY behavior, version,
  provenance, and non-supported surface fencing;
- license and NOTICE: production dependency notices, source provenance, and
  THIRD-PARTY-LICENSES updates for any dependency actually adopted after
  Arc04;
- test-only exclusion: Catch2 and other test-only material must not enter
  release tarballs or runtime installation surfaces;
- CI and release gate: local build-all, positive and negative process
  fixtures, release-package probes, checksum/manifest verification, and
  wolong-oriented install verification.

Arc06 remains the owner of publication readiness. Arc04 readiness does not
authorize a release.

## Open Risks And Amendment Triggers

Open risks:

- CLI11 amendment trigger: any CLI migration that changes help shape,
  parse-error semantics, compatibility aliases, color controls, status stream
  legality, or output conflicts before golden/process fixtures exist.
- fmt amendment trigger: direct formatter use from planner internals or any
  formatting path that can bypass stdout/stderr and ANSI policy.
- Catch2 amendment trigger: treating seam tests as a replacement for black-box
  process fixtures, or routing Catch2 into release surfaces.
- `tl::expected` amendment trigger: exposing the pilot type beyond the local
  status/result facade or letting it define the external process contract.
- reproc++ amendment trigger: importing a process library without an accepted
  supported child-process boundary and license/release plan.
- optional surfaces amendment trigger: any H2, cpddl/FAM, translation,
  interactive, SAT, BDD, or CUDD behavior becoming supported by availability
  rather than by an accepted product decision.
- full-duplex amendment trigger: stdin-heavy fixtures relying on the current
  synchronous stdin writer without focused runner hardening.

## Arc04 Close Recommendation

Arc04 close:

Arc04 is ready for arc-level CDC close after Slice06 CDC verification.

What remains explicitly routed:

- Arc05: per-binary parser, grounder, and engine adoption; canonical
  `pandapi-*` entry points; inherited `pandaPI*` compatibility behavior;
  black-box contract fixtures; CLI migration; executable status, stream,
  color, signal, timeout/resource, provenance, and non-supported-surface proof.
- Arc06: release shape preservation, wolong proof, behavior-change
  documentation, dependency license/NOTICE updates, test-only dependency
  exclusion, CI evidence, and publication gate.

No Arc04 amendment is required from Slice06 implementation evidence.
