# chengdu arc04 / slice05 - fixture-and-seam-test-substrate - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Implement the shared runtime test substrate that Arc05 will use to prove
managed-process conformance at executable boundaries.

This slice turns the accepted Arc03 contract-test matrix into reusable
fixture, normalization, process-observation, timeout/signal, and seam-test
support under the chengdu-owned runtime tree. It builds on the accepted
Slice02 status/result core, Slice03 tagged status I/O, and Slice04
CLI/TTY/provenance policy helpers. It still does not migrate parser,
grounder, or engine behavior and does not add `pandapi-*` entry points.

## 2. Scope

**In:**

- Add fixture/test-substrate APIs under
  `pandapi-runtime/include/pandapi/runtime/` for:
  - process fixture identity and metadata;
  - command/argv/environment specification;
  - stdin/stdout/stderr role expectations;
  - expected exit code, wait status, and final `PANDAPI_STATUS` observation;
  - output artifact expectation and cleanup policy;
  - golden versus semantic comparison policy;
  - safe timeout/resource/signal probe policy;
  - normalized comparison results.
- Add normalization helpers for:
  - path tokens such as fixture root, build root, temp root, and release root;
  - line-ending normalization to LF;
  - ANSI detection/rejection and optional stripping for human diagnostics;
  - duration, timestamp, build metadata, errno, and signal predicates;
  - final tagged status parsing through the accepted Slice03 status I/O API.
- Add a local process-observation substrate suitable for Linux and macOS test
  hosts. It must capture stdout, stderr, exit code, wait status, timeout
  disposition, and cleanup state without introducing reproc++, Boost.Process,
  Abseil, JSON, or a shell-dependent diagnostic parser.
- Add safe seam tests for the fixture substrate itself. Tests may use tiny
  local commands or helper test binaries, but must not invoke parser,
  grounder, or engine as conformance proof in this slice.
- Add CMake/test registration for the new substrate and update the runtime
  README.
- Add a test-only Catch2 integration gate without network fetching:
  - Catch2 may be used only for tests;
  - no Catch2 type may appear in public runtime headers;
  - the default build must not require network access;
  - if Catch2 is not locally available, the standard runtime tests must still
    build and run.
- Prove `pandapi-runtime` still builds through `scripts/build-runtime.sh`.
- Prove existing parser, grounder, and engine behavior is not changed by this
  slice.

**Out:**

- No edits under `pandaPI/`.
- No adoption of the fixture substrate by parser, grounder, or engine
  binaries.
- No `pandapi-*` wrappers, symlinks, copied binaries, renamed binaries, or
  inherited-name migration work.
- No per-binary Arc05 conformance fixtures for parser, grounder, or engine.
  This slice may include tiny harness self-tests only.
- No CLI11 import, parser replacement, or generated CLI migration.
- No fmt adoption unless it remains behind the existing diagnostics/process
  facade and no new direct formatting dependency appears in public headers.
- No `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, GSL,
  google/benchmark, SAT/BDD/CUDD, H2/cpddl dependency-internal work, or
  optional dependency audits.
- No `FetchContent`, `ExternalProject`, package-manager bootstrap, or network
  download in the default build.
- No release asset, manifest, checksum, wolong, workflow, top-level README,
  `vendor.env`, `pins.env`, `dist`, `build`, or `release` changes.
- No close-set files at open time other than CC's eventual
  `closing-report.md`; leave `cdc-verification.md` for CDC.

## 3. Required Decisions

CC must decide and record:

- the exact header/source split for fixture data, normalization helpers,
  process observation, and optional Catch2 gate support;
- whether the process-observation substrate is pure C++ plus POSIX shims, a
  small runtime test helper, or another local standard-library-first shape;
- how to represent wait status distinctly from process exit code;
- how to keep timeout and signal probes deterministic on Linux/macOS CI;
- how to mark resource-limit probes as safe, quarantine, or manual without
  creating flaky or destructive tests;
- how to report comparison failures without depending on human diagnostic
  regex archaeology;
- how Catch2 can be introduced as test-only substrate without leaking into
  production headers, default builds, release packaging, or public runtime
  APIs.

## 4. Constraints

- Use C++17 and the existing `pandapi::runtime` namespace.
- Build on Slice02 `ProcessStatus`, `StatusCode`, `StatusClass`,
  `Component`, `SurfaceDisposition`, `OutputRole`, and `StatusResult<T>`.
- Build on Slice03 `StatusRecord`, `parse_status_record`,
  `serialize_status_record`, status stream legality, and partial-output
  vocabulary.
- Build on Slice04 command identity, common CLI policy, TTY/color, ANSI, and
  provenance helpers where relevant.
- Preserve Arc03's black-box versus seam-test split:
  - black-box process fixtures own executable behavior, streams, wait status,
    files, timeouts, signals, and release-package behavior;
  - seam tests own local helper mappings, parsers, normalizers, and facades;
  - a seam test cannot replace executable conformance proof.
- Follow C++ Core Guidelines pressure:
  - explicit and strongly typed interfaces (`I.1`, `I.4`);
  - low-ambiguity parameters and result structs (`I.23`, `I.24`, `F.20`,
    `F.21`);
  - systematic error handling (`E.1`, `E.4`, `E.27`);
  - scoped resource management around child processes and file descriptors
    (`R.1`, `R.5`, `R.11`);
  - no global mutable state (`I.2`, `R.6`);
  - no raw ownership transfer in public APIs (`I.11`);
  - no unbounded casts or magic constants (`ES.45`, `ES.46`, `ES.48`);
  - self-contained headers with no global `using namespace` (`SF.2`, `SF.7`,
    `SF.11`);
  - standard-library vocabulary for strings, containers, chrono/timeouts, and
    streams (`SL.str.1`, `SL.str.2`, `SL.io.2`, `SL.io.3`).
- Keep tests CI-safe by default. Destructive, racy, platform-sensitive, or
  resource-heavy probes must be represented as quarantine/manual fixtures, not
  run by the default runtime build.
- Do not hide behavior changes in this substrate slice. Runtime tests can
  prove the harness and helper APIs, but parser/grounder/engine conformance
  waits for Arc05.

## 5. Verification Approach

CC verifies by adding focused runtime tests to `pandapi-runtime/tests/`,
updating `pandapi-runtime/CMakeLists.txt` and `pandapi-runtime/README.md`,
running `./scripts/build-runtime.sh`, and running the existing
parser/grounder/engine build plus negative smoke gates. CDC will rerun the
ledger commands, inspect the diff, and verify that the fixture substrate
matches the Arc03 matrix without changing inherited binaries, release assets,
or default dependency behavior.

## 6. Exit Criteria

The ledger reaches final status. `pandapi-runtime` exposes a tested fixture
and seam-test substrate that Arc05 can use for process-contract conformance:
fixture records, normalized comparisons, process observation, status matching,
timeout/signal/resource safety policy, and a test-only Catch2 gate. The
implementation remains inert with respect to inherited binaries, keeps
test-only dependencies out of public runtime APIs and release packaging, and
makes no user-visible command behavior change.
