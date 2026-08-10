# Closing Report: cli-tty-provenance-core

Date: 2026-08-09

Branch: `release/0.3.x`

Role: CC

## Capability Verdict

Delivered.

Arc04 slice04 now has a standard-library-only `pandapi::runtime` core for
common CLI policy, command identity, injected TTY/color policy, ANSI legality,
and caller-supplied version/provenance field assembly. The implementation is
inert with respect to parser, grounder, and engine binaries: there are no
`pandapi-*` entry points, no per-binary CLI adoption, no inherited-name
migration, and no release-shape changes.

## Implementation Decisions

- Header/source split: `cli_policy.hpp/.cpp` owns common option values,
  command identity, informational-command policy, and policy validation;
  `tty.hpp/.cpp` owns terminal observation, color controls, and `allows_color`;
  `provenance.hpp/.cpp` owns `ProvenanceRecord` and field-oriented formatters.
- Validation returns the existing `StatusResult<T>` facade. Invalid common CLI
  policy maps to `StatusCode::CliUsageError` and exit `10`, not to input/model
  status codes.
- TTY detection is represented as injected observation through
  `TerminalKind` and `detect_terminal(bool observed_is_tty)`, so tests do not
  depend on the current shell.
- Color disabling wins through `effective_color_mode`: `--color=never`,
  `--no-color`, `--no-colour`, and `NO_COLOR` neutralize enabling requests
  before supervised/machine-output validation.
- Provenance is caller supplied. The runtime formats stable `key=value` fields
  and rejects supplied placeholder prose such as `unknown`, while omitting
  absent optional fields instead of shelling out to git.
- The local CLI policy facade keeps CLI11 adoption possible later without
  importing, exposing, or depending on CLI11 in this slice.

## Verification

Commands run:

```text
./scripts/build-runtime.sh
./scripts/build-parser.sh && ./scripts/build-grounder.sh && ./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md
```

Results:

- Runtime build passed on macos-arm64 with 4/4 CTest tests.
- Parser, grounder, and engine builds passed on macos-arm64; output contained
  inherited-source compiler warnings only.
- Negative smoke passed: 4 passed, 0 failed.
- Whitespace checks passed.
- Protected-path diff check produced no output.
- Ledger row count is 24.
- CDC verification file remains absent.

## Ledger Walk

| Row | Status | Evidence |
|-----|--------|----------|
| F-1 | done | Slice open-set grep command passed. |
| F-2 | done | `pandapi-runtime/README.md` documents CLI/TTY/provenance, command identity, TTY/color, `NO_COLOR`, CLI11 gate, and no binary adoption. |
| F-3 | done | Added `cli_policy.hpp`, `tty.hpp`, and `provenance.hpp` with required public type names. |
| F-4 | done | Added typed common CLI policy enums and `CommonCliOptions`. |
| F-5 | done | Validation failures return `StatusResult` carrying `cli_usage_error` / exit `10`; seam tests exercise the cases. |
| F-6 | done | `--status=stdout` rejects stdout data artifacts and informational stdout via Slice03 status-stream semantics. |
| F-7 | done | `--output` plus inherited positional output alias maps to `cli_usage_error` before input/model processing. |
| F-8 | done | Informational commands are stdout-owned `ok` / `0` helpers and mutually exclusive as flags. |
| F-9 | done | Canonical and inherited parser, grounder, and engine names are recognized without default deprecation warnings. |
| F-10 | done | Color controls cover auto, always, never, no-color, no-colour, and `NO_COLOR`; disabling wins. |
| F-11 | done | `allows_color` permits only human stderr on a TTY and rejects stdout artifacts, tagged status, supervised, machine, file, pipe, and no-TTY output. |
| F-12 | done | Tests inject `TerminalKind` and call `detect_terminal(bool)` rather than probing the live shell. |
| F-13 | done | `ProvenanceRecord` fields are caller supplied; no shell/git metadata discovery calls exist. |
| F-14 | done | Version/provenance formatters emit stable field-oriented output and reject placeholder prose. |
| F-15 | done | Dependency-gate search found no disallowed imports, fetches, discovery, or exposure. |
| F-16 | done | Public-header hazard search found no global namespace using, mutable namespace state, raw ownership transfer, explicit allocation/deallocation, or namespace `std` additions. |
| F-17 | done | Non-failing mapping/policy helpers are `noexcept`; tests include all new public headers. |
| F-18 | done | CMake builds new sources and registers `pandapi_cli_tty_provenance_smoke`. |
| F-19 | done | `./scripts/build-runtime.sh` passed with 4/4 tests. |
| F-20 | done | Parser, grounder, and engine build scripts passed. |
| F-21 | done | Negative smoke passed with 4 passed, 0 failed. |
| F-22 | done | Protected-path diff command produced no output. |
| F-23 | done | This closing report contains the required sections. |
| F-24 | done | No `cdc-verification.md` file was created. |

## Silent-Drop Check

Scope as specified:

- common CLI policy values;
- command identity for canonical and inherited names;
- output/status conflict validation;
- informational-command helpers;
- TTY/color and ANSI legality helpers;
- provenance/version field assembly from caller-supplied metadata;
- runtime README, CMake, and seam-test updates;
- no binary adoption and no protected-path edits.

Scope as delivered matches the specified slice. Nothing was dropped or
deferred from slice04. Adoption into parser, grounder, and engine remains
outside this slice by design.

## Bubble-up to Arc04

Slice04 delivered the Arc04 slice-breakdown item for CLI/TTY/provenance core:
the runtime now exposes tested policy helpers that Arc05 can consume when the
per-binary migration starts.

The implementation did not reveal a need to change the Arc04 plan. Slice05 can
still proceed as planned against a process/contract test matrix: it should use
these helpers as seam-testable policy inputs, but black-box executable
conformance still belongs to later process fixtures and Arc05 adoption.

## Iteration 01 CDC Fix

CDC did not accept the first close and reported two blocking findings.

- **S04-CDC-001:** `format_version` accepted partial required version records.
  Repair: `version_fields` now requires canonical command, component, chengdu
  version, contract version, upstream project, upstream commit, source prefix,
  build commit, platform, compiler, license, and notice. `invoked_command`
  remains optional because Arc03 requires it only for inherited compatibility
  invocation. Optional detailed provenance-only fields may still be omitted.
- **S04-CDC-002:** `--color=always` with `--status=stderr` was accepted.
  Repair: `validate_common_cli_options` now rejects effective
  `ColorMode::Always` whenever any tagged status target is selected, including
  stderr, unless a disabling control makes the effective color mode `Never`.

New regression coverage in `pandapi_cli_tty_provenance_smoke`:

- missing required version field rejects as `cli_usage_error` / exit `10`;
- all required version fields succeed;
- `invoked_command` is omitted for canonical invocation;
- optional provenance-only fields remain omittable;
- placeholder values still reject;
- `--color=always --status=stderr`, `--color=always --status=stdout`, and
  `--color=always --supervised` reject as `cli_usage_error`;
- disabling status color through no-colour is accepted because the effective
  color mode is `Never`.

Iteration 01 verification commands:

```text
./scripts/build-runtime.sh
CDC provenance-required probe
CDC color-status-stderr probe
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md
```

Iteration 01 results:

- Runtime build passed on macos-arm64 with 4/4 CTest tests.
- Both CDC probes exited 0 and printed `rejected`.
- Parser, grounder, and engine builds passed on macos-arm64; output contained
  inherited-source compiler warnings only.
- Negative smoke passed: 4 passed, 0 failed.
- Whitespace checks passed.
- Protected-path diff check produced no output.
- Ledger row count remains 24 with no open rows.
- CDC verification file remains absent.
