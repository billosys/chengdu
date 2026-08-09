# Managed-Process Contract

Date: 2026-08-09

Arc: `arc03-managed-process-contract`

Contract version: `0.3.0`

## Executive Summary

This document is the normative Arc03 output for chengdu `0.3.0`. Arc04,
Arc05, and Arc06 implement, adopt, test, document, and release against this
contract.

The contract defines one managed-process behavior surface for three binaries:

- `pandapi-parser`
- `pandapi-grounder`
- `pandapi-engine`

The inherited executable names remain compatibility entry points for `0.3.0`:

- `pandaPIparser`
- `pandaPIgrounder`
- `pandaPIengine`

The supported product surface is intentionally narrow: parser normal HDDL
parse, grounder normal `.htn` grounding, and engine normal search. Legacy,
experimental, unsupported, and future surfaces are fenced and do not become
supported because code, flags, dependencies, or inherited behavior happen to
exist.

## Normative Status

Normative requirements in this document use these meanings:

- `MUST`: required for 0.3.0 conformance.
- `SHOULD`: expected unless a later implementation slice records a concrete
  reason and test-backed alternative.
- `MAY`: allowed but not required.

Informative material includes evidence references, rationale, examples,
implementation notes, and later-arc guidance. Earlier Arc03 reports remain
accepted evidence, but this document is the contract Arc04, Arc05, and Arc06
implement against. If a later implementation finds a contradiction, it must
route an amendment rather than silently choosing a weaker source.

Evidence inputs:

- Arc01: `audit-synthesis-pandapi.md` and Arc01 closing report.
- Arc02: `combined-library-recommendations.md` and Arc02 closing report.
- Arc03 slice01: `supported-surface-classification.md` and CDC verification.
- Arc03 slice02: `status-exit-signal-taxonomy.md` and CDC verification.
- Arc03 slice03: `stdio-event-tty-contract.md` and CDC verification.
- Arc03 slice04: `cli-naming-version-migration.md` and CDC verification.
- Arc03 slice05: `contract-test-matrix.md` and CDC verification.

## Surface Contract

| Component | Surface | Disposition | Normative requirement |
|-----------|---------|-------------|-----------------------|
| Parser | parser normal HDDL parse | supported | `pandapi-parser` MUST support normal HDDL domain/problem parsing and emit the selected parser artifact. |
| Parser | parser verifier/output helper paths | legacy | These paths MUST NOT be treated as supported managed-process surfaces. Supervised invocation MUST classify or fence them as `legacy_surface` unless a later contract accepts them. |
| Grounder | grounder normal `.htn` grounding | supported | `pandapi-grounder` MUST support parser-generated `.htn` input and emit the selected planner/grounder artifact. |
| Grounder | H2 | experimental | H2 MUST remain explicit opt-in or fenced as `experimental_surface`; it MUST NOT contaminate supported stdout/stderr behavior. |
| Grounder | `cpddl`/FAM | legacy | `cpddl`/FAM behavior MUST remain legacy advanced behavior and MUST NOT define default grounder support. |
| Engine | engine normal search | supported | `pandapi-engine` MUST support normal search over accepted engine input and report solved or `domain_no_plan`. |
| Engine | interactive | legacy | Interactive behavior MUST remain human legacy behavior and MUST NOT silently start under supervised or no-TTY invocation. |
| Engine | translation | experimental | Translation MUST remain explicit opt-in or fenced as `experimental_surface`; child-process failures are not part of the supported default path. |
| Engine | SAT | unsupported | SAT MUST be rejected or hidden for 0.3.0; visible invocation maps to `unsupported_feature`. |
| Engine | BDD/CUDD | future | BDD/CUDD MUST remain future work; visible invocation maps to `future_surface`. |

Re-entry conditions:

- Legacy parser verifier/output helpers re-enter only through a later slice
  that defines product value, status semantics, stream ownership, fixtures, and
  release documentation.
- H2 re-enters only after correctness, stream isolation, dependency boundary,
  and fixture work prove it can run without changing supported behavior.
- `cpddl`/FAM re-enters only after a boundary audit names user-facing
  behavior, dependency obligations, and tests.
- Interactive mode re-enters only through a TTY-specific design that keeps it
  out of supervised mode.
- Translation re-enters only after temp-file cleanup, child-process handling,
  status mapping, and artifact ownership are designed and tested.
- SAT re-enters only through a dedicated SAT capability slice with signal,
  threading, build, license, and fixture proof.
- BDD/CUDD re-enter only through a dedicated optional-dependency project with
  license, build, security, and planner-surface audit.

## Command Names And Release Shape

Canonical commands:

- `pandapi-parser`
- `pandapi-grounder`
- `pandapi-engine`

Inherited compatibility names:

- `pandaPIparser`
- `pandaPIgrounder`
- `pandaPIengine`

Requirements:

- New documentation, examples, tests, provenance identity, and Arc05 migration
  work MUST prefer `pandapi-*` names.
- Inherited `pandaPI*` names MUST remain executable in `0.3.0` release assets
  and installed trees.
- Compatibility invocation MUST behave like the canonical command for
  supported normal paths, status mapping, stdout/stderr ownership, color,
  help, version, provenance, and parse errors.
- Compatibility invocation MUST NOT emit a default deprecation warning because
  that warning would change stream behavior and could surprise wolong.
- `--version` and `--provenance` SHOULD expose both canonical command and
  invoked compatibility name when `argv[0]` is inherited.
- A future breaking removal of inherited names requires an explicit operator
  decision, release-note migration table, wolong verification, and a new design
  update.

Release shape:

- Release asset names remain `pandapi-<tag>-<platform>.tar.gz`.
- `SHA256SUMS` remains the checksum file covering release assets.
- `release-manifest.txt` remains the aggregated provenance manifest.
- The wolong four-command fetch/verify/extract/run path MUST remain compatible.
- Arc06 MUST document the compatibility transition while preserving an
  inherited-name compatibility example for wolong and existing users.

## Invocation Modes And Options

Modes:

| Mode | Audience | stdout | stderr | Machine observation |
|------|----------|--------|--------|---------------------|
| Human CLI | terminal user | selected data artifact, informational output, or empty | diagnostics, warnings, progress, statistics, summaries | exit code; optional tagged status |
| Supervised/process-manager | script, daemon, workflow runner | selected data artifact, tagged status when legal, or empty | quiet diagnostics and tagged status when selected | exit code plus optional tagged status |
| Machine-status | supervisor needing parseable observation | tagged status only when stdout has no data/human output | tagged status when stdout owns data | exit code plus tagged status |

Common options:

- `--help`: print help to stdout and exit `ok` / `0`; MUST NOT process input
  files.
- `--version`: print concise version/provenance identity to stdout and exit
  `ok` / `0`; MUST NOT process input files.
- `--provenance`: print detailed source/build/release provenance to stdout and
  exit `ok` / `0`; MUST NOT process input files.
- `--output PATH|-`: select output artifact destination. `-` selects stdout.
- `--supervised`: select supervised/process-manager behavior: quiet human
  prose, no progress spinner, no unstructured statistics, no ANSI, and
  deterministic parse-error handling. It does not by itself enable tagged
  status.
- `--status`: enable one final `PANDAPI_STATUS` record on stderr; equivalent to
  `--status=stderr`.
- `--status=stderr|stdout`: select the tagged status stream. `stderr` is the
  default and is compatible with stdout data artifacts. `stdout` is legal only
  when stdout is not carrying a data artifact or human informational output.
- `--quiet`: suppress human progress, banners, option dumps, and statistics on
  stderr; it MUST NOT suppress fatal diagnostics, final tagged status records,
  or exit classification.
- `--verbose`: MAY add human diagnostics/statistics to stderr; it MUST NOT add
  prose to stdout artifacts or tagged status records.
- `--color=auto|always|never`: select human stderr color policy. `auto` is the
  default. `always` is invalid with supervised or machine-parsed output.
  `never` is equivalent to `--no-color`.
- `--no-color`: canonical spelling to disable ANSI everywhere.
- `--no-colour`: compatibility spelling for `--no-color`.

Supported command synopsis:

```text
pandapi-parser [COMMON] [--output OUT.htn|-] DOMAIN.hddl PROBLEM.hddl
pandapi-grounder [COMMON] [--output OUT.sas|-] INPUT.htn
pandapi-engine [COMMON] [--output PLAN|-] INPUT.sas
```

Compatibility positional output forms MAY remain accepted for parser and
grounder, but using both `--output` and a positional output alias MUST be
`cli_usage_error` / exit `10`.

## Status Vocabulary And Exit Codes

Supervisors classify outcomes from the numeric exit code and stable status
name, not from diagnostic prose.

| Status | Exit code | Class | Normative meaning |
|--------|-----------|-------|-------------------|
| `ok` | `0` | success | Requested operation completed or successful informational command ran. |
| `domain_no_plan` | `2` | expected domain outcome | Valid search/grounding completed and proved no usable plan or reachable goal. |
| `cli_usage_error` | `10` | caller error | Invalid CLI shape, missing args, mutually exclusive options, too many inputs, or malformed option values before input/model processing. |
| `input_unavailable` | `20` | caller error | Required input, include, path, or input stream cannot be opened, read, or resolved. |
| `output_unavailable` | `21` | caller error | Output/temp artifact path cannot be created, opened, written, flushed, renamed, or cleaned up. |
| `input_invalid` | `22` | input/model error | Input was available but malformed, semantically invalid, truncated, inconsistent, or beyond checked model bounds. |
| `unsupported_feature` | `30` | policy failure | Rejected feature, mode, build option, or model construct was requested. |
| `legacy_surface` | `31` | policy failure | Legacy surface was invoked where managed-process supervision expected the supported contract. |
| `experimental_surface` | `32` | policy failure | Experimental surface was invoked without a final supported opt-in contract or failed inside fenced behavior. |
| `future_surface` | `33` | policy failure | Dormant/future surface was requested but is unavailable in this release. |
| `timeout` | `40` | external/policy failure | Configured wall-clock or CPU-time limit was exceeded. |
| `resource_limit` | `41` | external/policy failure | Configured memory, node, recursion, disk, file, or other resource policy was exceeded. |
| `interrupted` | `42` | external/policy failure | Binary caught SIGINT/SIGTERM or similar termination request and exited through the status mapper. |
| `dependency_failure` | `50` | external/dependency failure | In-process dependency or dependency adapter failed outside ordinary invalid user input. |
| `child_process_failure` | `51` | external/dependency failure | Child process failed, timed out, was signaled, could not start, or returned unmapped status. |
| `internal_error` | `60` | internal defect | Program invariant, unexpected exception, deep helper, or unclassified top-boundary failure. |
| `signal_terminated` | `128 + signal_number` | external/supervisor observation | OS terminated the process by signal before the binary could emit `interrupted`. |

Exit codes `64`-`78` are reserved for possible future BSD `sysexits`
compatibility aliases and MUST NOT be used in the primary `0.3.0` contract
without a new compatibility decision.

Payload classes:

- success;
- expected domain outcome;
- caller error;
- input/model error;
- policy/surface failure;
- timeout/resource/interruption failure;
- dependency/child-process failure;
- internal defect;
- supervisor-owned signal termination.

Status payloads MUST include enough stable fields for process managers to
distinguish solved, unsolvable, malformed input, unavailable input, unavailable
output, unsupported or non-supported surfaces, timeout, resource limit,
interrupted, dependency/child failure, internal defect, and unhandled signal
death.

`domain_no_plan` is not malformed input, timeout, resource exhaustion,
dependency failure, child-process failure, or internal error.

`interrupted` is binary-owned. `signal_terminated` is supervisor-owned and may
lack a final status line because the process may not flush.

## stdout, stderr, Status, And Partial Output

stdout ownership:

- stdout MUST be exactly one of: selected data artifact, informational
  help/version/provenance output, tagged status stream when legal, or empty.
- stdout MUST NOT mix human diagnostics, progress, statistics, color, or
  summaries with parser, grounder, or engine data artifacts.
- `domain_no_plan` produces no plan artifact by default; stdout is empty unless
  a later contract defines an explicit no-plan artifact.

stderr ownership:

- stderr owns human diagnostics, warnings, progress, statistics, and final
  human summaries.
- Diagnostic prose on stderr is non-normative. Wording, punctuation, wrapping,
  and ordering may change without changing the process contract.
- Supervisors MUST NOT rely on human diagnostic prose for classification.

Tagged status:

```text
PANDAPI_STATUS	status=ok	component=engine	surface=normal_search	surface_disposition=supported	exit_code=0	class=success	duration_ms=37
```

Requirements:

- Machine-readable status output is tagged text, not JSON Lines.
- `nlohmann/json` remains held for 0.3.0.
- A final `PANDAPI_STATUS` line, when enabled, MUST be emitted after artifact
  disposition is known and before process exit.
- The final status line MUST be flushed before returning from `main` or before
  an owned graceful `interrupted` exit.
- No status record may be emitted after the final status line.
- Values MUST be single-line and escaped where necessary.
- Unknown fields are omitted, not emitted with placeholder prose.

Required status fields:

- `status`
- `component`
- `surface`
- `surface_disposition`
- `exit_code`
- `class`

Conditional fields include artifact role/path, duration, input/output path and
operation, errno, location, rule, token, model section, outcome/proof,
timeout/resource/signal details, dependency/child details,
`partial_output_policy`, `cleanup_completed`, and `finding_refs`.

Partial-output policy:

- `ok`: selected artifact is complete, flushed, and closed before final status.
- `cli_usage_error`, `input_unavailable`, `input_invalid`,
  `unsupported_feature`, `legacy_surface`, `experimental_surface`, and
  `future_surface`: supported artifact contract is not entered.
- `output_unavailable`, `timeout`, `resource_limit`, `interrupted`,
  `dependency_failure`, `child_process_failure`, and `internal_error`: final
  status SHOULD report whether partial output was absent, retained, discarded,
  complete, or unknown.
- File artifacts SHOULD use temp plus commit/rename where Arc04/Arc05 can make
  that deterministic.

## Help, Version, Provenance, Parse Errors, Color, And TTY

Help:

- `--help` MUST exit `ok` / `0`.
- Help MUST write stdout informational output.
- Help MUST show canonical `pandapi-*` usage.
- Help SHOULD include a compatibility note for inherited `pandaPI*` names.
- Default help MUST NOT make legacy, experimental, unsupported, or future
  surfaces look supported.

Version/provenance:

- `--version` and `--provenance` MUST exit `ok` / `0`.
- Stable version fields include canonical command, invoked compatibility name
  when applicable, component, chengdu version, managed-process contract version
  `0.3.0`, upstream pandaPI project/commit, source prefix, chengdu build
  commit, platform, compiler, license pointer, and NOTICE pointer.
- `--provenance` SHOULD additionally include import commit, build timestamp if
  available, dependency license/provenance fields, and
  THIRD-PARTY-LICENSES pointer.
- Inherited upstream strings such as `pandaPIparser 0.1` MAY appear only as
  upstream provenance, not as the chengdu product version.

Parse errors:

- Malformed CLI shape MUST map to `cli_usage_error` / exit `10`.
- Parse errors MUST stop before input/model processing.
- Parse errors MUST NOT create output artifacts.
- Examples include missing required arguments, unknown options, malformed enum
  values, too many positionals, `--output` conflicts, and `--status=stdout`
  conflicts.

Color and TTY:

- ANSI escape sequences are permitted only in human stderr output when color is
  enabled and stderr is a suitable TTY.
- ANSI is forbidden in stdout artifacts, tagged status records, supervised
  output, machine output, files, pipes, and no-TTY output.
- `--color=auto` is default.
- `--color=never`, `--no-color`, `--no-colour`, and `NO_COLOR` disable ANSI
  everywhere.
- If both enabling and disabling controls are present, disabling wins.
- `--color=always` is human-only and invalid with `--supervised`,
  `--status=stdout`, or any machine-parsed stream.
- Legacy interactive TTY behavior is fenced as `legacy_surface` under
  supervised or no-TTY invocation.

Quiet and verbose affect human stderr only. They MUST NOT change exit codes,
stdout artifact ownership, or final tagged status fields.

## Contract Tests And Golden Output

Arc04 and Arc05 MUST implement the slice05 contract-test matrix before claiming
conformance.

Process fixture schema requirements include:

- command and argv;
- environment;
- stdin;
- TTY/no-TTY mode;
- timeout/resource policy;
- expected exit code;
- expected signal or wait status;
- expected stdout role;
- expected stderr role;
- expected artifacts;
- expected final `PANDAPI_STATUS` fields;
- normalization rules.

Golden-output tests freeze stable machine contract fields:

- command name and argv shape;
- exit code and status name;
- `PANDAPI_STATUS` tag spelling, field names, and required fields;
- stdout/stderr ownership roles;
- help/usage skeleton and option names once finalized;
- version/provenance field names;
- absence of default inherited-name deprecation warning;
- absence of ANSI leakage in machine streams.

Semantic predicates, not byte-golden comparisons, cover non-normative human
diagnostic prose, paths after normalization, durations, timestamps, build
metadata, compiler/platform fields, errno text, signal wording, and
algorithm-specific plan text where not guaranteed stable.

Required process probes include:

- parser positive file-backed and stdout-backed paths;
- grounder positive file-backed and stdout-backed paths;
- engine positive file-backed and stdout-backed paths;
- parser-to-grounder-to-engine pipeline composition;
- missing/unreadable input and missing include failures;
- unavailable output and write/flush/finalize failures where feasible;
- malformed HDDL, malformed `.htn`, and malformed engine input;
- engine solved and `domain_no_plan`;
- timeout, resource, interrupt, and signal termination;
- color/TTY/no-color/no-colour/NO_COLOR behavior;
- visible legacy, experimental, unsupported, and future surface fencing;
- canonical `pandapi-*` and inherited `pandaPI*` command behavior;
- `--help`, `--version`, `--provenance`, parse-error, and option-conflict
  behavior.

Black-box process fixtures own executable behavior. Catch2 seam tests own
internal helpers such as status mapping, tagged status writing/parsing,
diagnostics routing, color policy, CLI parser facade behavior, output
finalization, provenance assembly, and signal/status mapping. Catch2 seam
tests MUST NOT replace black-box process fixtures.

## Arc02 Dependency Gates

Arc02 dependency decisions are normative implementation gates:

- Standard-library modernization is the baseline for ownership, paths,
  strings, chrono/timeouts, bounds, streams, and local hash hygiene.
- fmt MAY be adopted only behind a chengdu diagnostics/process I/O facade.
  Direct planner-internal stdout/stderr formatting is forbidden.
- CLI11 is the selected CLI replacement candidate, but adoption MUST wait for
  golden-output/process tests covering help, version, provenance, parse errors,
  compatibility invocation names, color controls, `--supervised`, `--status`,
  and output/status conflicts.
- Catch2 is adopted test-only for C++ seam tests and MUST be paired with
  black-box process fixtures.
- `tl::expected` is a pilot only behind a local status/result substrate after
  this taxonomy is stable.
- reproc++ is a conditional pilot only behind one child-process adapter if a
  supported child-process boundary is later accepted. It is not required for
  current subprocess tests.
- `nlohmann/json` remains held because the selected status format is tagged
  text, not JSON Lines.
- Microsoft GSL, performance containers, google/benchmark, parser-generator
  alternatives, optional SAT/BDD/CUDD/H2/cpddl work, and optional dependency
  audits remain held until their re-entry criteria are met.
- Abseil and Boost.Process remain rejected for 0.3.0 foundations.
- Adopted dependencies require Arc06 license, NOTICE, manifest, provenance, and
  release-packaging verification. Test-only dependencies MUST be excluded from
  release tarballs.

## Arc04 Shared-Runtime Handoff

Arc04 should be planned from this contract, not from earlier guesses.

Arc04 shared runtime/substrate inputs:

- status/result types and status-to-exit-code mapper;
- diagnostics/process I/O facade;
- CLI parser wrapper, including any CLI11 adapter;
- tagged status writer and status parser;
- TTY/color policy helper;
- stdout/stderr ownership helpers;
- buffering/flushing and output finalization policy;
- provenance/version field assembler;
- fixture harness and normalization helpers;
- Catch2 seam-test substrate;
- safe CI timeout/resource/signal harness behavior;
- local facades for fmt, CLI11, Catch2, `tl::expected`, and any conditional
  child-process adapter.

Arc04 MUST NOT change planner behavior except where necessary to introduce the
shared substrate and tests under the design-approved surface.

## Arc05 Per-Binary Handoff

Arc05 adopts the contract per binary.

Parser adoption inputs:

- canonical `pandapi-parser` and inherited `pandaPIparser`;
- normal HDDL domain/problem parse;
- file-backed and stdout-backed output;
- `cli_usage_error`, `input_unavailable`, `output_unavailable`,
  `input_invalid`, `legacy_surface`, and `internal_error`;
- parser verifier/output helper fencing.

Grounder adoption inputs:

- canonical `pandapi-grounder` and inherited `pandaPIgrounder`;
- normal `.htn` grounding;
- file-backed and stdout-backed output;
- `domain_no_plan` where reachable through accepted grounder semantics;
- input/output/invalid statuses;
- H2 experimental fencing and `cpddl`/FAM legacy fencing.

Engine adoption inputs:

- canonical `pandapi-engine` and inherited `pandaPIengine`;
- normal search;
- file-backed and stdout-backed plan artifact behavior;
- solved `ok` and unsolvable `domain_no_plan`;
- timeout, resource, interrupted, signal_terminated, dependency/child failure,
  unsupported SAT, future BDD/CUDD, legacy interactive, and experimental
  translation fencing.

All binaries must adopt stream ownership, final status emission, color/TTY
behavior, help/version/provenance behavior, compatibility names, positive path
fixtures, negative status probes, and non-supported surface fencing. Library
availability MUST NOT expand supported behavior.

## Arc06 Release-Hardening Handoff

Arc06 release hardening inputs:

- behavior-change table for command names, status/exit codes, stdout/stderr
  behavior, color/TTY behavior, version/provenance, and non-supported surface
  handling;
- README and release notes updated for canonical `pandapi-*` commands while
  preserving inherited-name compatibility examples;
- wolong verification of fetch, checksum, extract, install, and run behavior;
- release asset shape verification, including tarballs, `SHA256SUMS`, and
  `release-manifest.txt`;
- provenance reconciliation between binary `--provenance`, `provenance.txt`,
  `release-manifest.txt`, source prefix, upstream commit, chengdu build commit,
  compiler, platform, license pointer, and NOTICE pointer;
- license/NOTICE/THIRD-PARTY-LICENSES updates for any adopted dependency;
- proof that Catch2 and other test-only dependency material is excluded from
  release tarballs;
- CI evidence for CLI and pipe-supervised positive and negative behavior.

Arc06 MUST NOT publish `v0.3.0` until release assets, checksums,
manifest/provenance, dependency licensing/NOTICE obligations, test-only
dependency exclusion, and the wolong migration path are verified.

## Deferrals And Re-Entry Conditions

Deferred or held items:

- Legacy parser verifier/output helpers: future product slice required.
- Legacy grounder `cpddl`/FAM: boundary audit and explicit supported-surface
  decision required.
- Legacy engine interactive: TTY-specific design required.
- Experimental H2: correctness, stream isolation, dependency, and fixture proof
  required.
- Experimental translation: child-process, temp-file, cleanup, and artifact
  contract required.
- Unsupported SAT: dedicated SAT capability project required.
- Future BDD/CUDD: optional-dependency audit and supported-surface project
  required.
- JSON/event expansion: later event-format decision required; `nlohmann/json`
  remains held.
- Optional dependency audits: future arc required for CUDD/BDD, H2/cpddl
  internals, parser-generator replacement, performance libraries, or
  google/benchmark.
- reproc++: re-enters only if a supported child-process boundary is accepted.
- Abseil and Boost.Process: rejected for 0.3.0 foundations; re-entry requires a
  new dependency-portfolio decision.

No deferred item may enter by drift through a library, build flag, hidden
option, or inherited code path.

## Arc03 Close Readiness

Arc03 is ready for arc-level CDC close after slice06 is independently
verified, subject to the normal arc-level composition check.

Readiness basis:

- slice01 classified supported and non-supported surfaces;
- slice02 defined status, exit, payload, timeout/resource/signal, dependency,
  child-process, and internal-error semantics;
- slice03 defined stdout/stderr/status ownership, tagged text status, flushing,
  partial-output, color, TTY, quiet, and verbosity policy;
- slice04 defined command names, inherited-name compatibility, help/version/
  provenance, CLI parse errors, supervised/status options, release-shape, and
  wolong migration policy;
- slice05 defined the contract test matrix, golden-output policy, fixture
  schema, and black-box/Catch2 split;
- slice06 synthesizes those accepted reports into this normative contract and
  routes Arc04, Arc05, and Arc06 obligations.

Remaining arc-level caveat: Arc03 is design-only. Arc03 close does not prove
implementation conformance, release packaging, or wolong runtime behavior.
Those are deliberately routed to Arc04, Arc05, and Arc06.

## Final Recommendation

Adopt this contract as the Arc03 managed-process design for chengdu `0.3.0`.
Proceed to Arc04 planning only after CDC verifies slice06 and the Arc03
arc-level close confirms that all six slices compose without silent drops.
