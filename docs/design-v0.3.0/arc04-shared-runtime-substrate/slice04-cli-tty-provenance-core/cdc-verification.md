# CDC Verification: cli-tty-provenance-core

Date: 2026-08-09
Branch: `release/0.3.x`
Role: CDC
Verified commit: `56966f989888f39c20030b0c99ca32c4a5d3f8d8`
Initial implementation commit: `5b523e7c8464c3c9b44a759d5a3420c6468119d7`
CDC iteration prompt commit: `4226855b662a45d93b3001242f6f755ab0c87cd8`

## Verdict

Accepted after Iteration 01.

Arc04 slice04 now has a CDC-verified, standard-library-only
`pandapi::runtime` CLI/TTY/provenance core. The accepted implementation adds
typed common CLI policy, command identity for canonical and inherited names,
informational-command policy, output/status conflict validation, TTY/color
helpers, ANSI legality helpers, and caller-supplied version/provenance
formatting without binary adoption or external dependency import.

The first close was blocked because `format_version` silently accepted partial
required version records and because `--color=always` was accepted with
`--status=stderr`. CC repaired both defects, added seam regressions, and reran
the original gates. CDC reproduced both focused probes and found no remaining
blocking issue.

## Source and Scope Checks

| Check | Result | Evidence |
|-------|--------|----------|
| Worktree clean before CDC docs | accepted | `git status --short --branch` showed only `## release/0.3.x`. |
| Iteration commit scope | accepted | `git show --stat --oneline --summary 56966f98...` showed edits to `provenance.cpp`, `cli_policy.cpp`, `cli_tty_provenance_smoke.cpp`, runtime README, the slice ledger, and CC closing report only. |
| Commit trailers | accepted | `git show --format=fuller --name-only --no-renames 56966f98...` showed both required co-author trailers. |
| No preexisting CDC verification | accepted | `test ! -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md` passed before this file was created. |
| Protected source/release paths | accepted | `git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release` produced no output during CDC verification. |

## Independent Verification

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | accepted | Slice open set still documents CLI/TTY/provenance scope, Arc03 help/version/provenance and color requirements, no-binary-adoption boundary, CLI11 gate, and C++ Core Guidelines pressure. |
| F-2 | accepted | `pandapi-runtime/README.md` documents CLI/TTY/provenance, command identity, TTY/color, `NO_COLOR`, provenance, CLI11 gate, and no binary adoption. |
| F-3 | accepted | Public headers exist for `cli_policy.hpp`, `tty.hpp`, and `provenance.hpp`; required public vocabulary reproduces. |
| F-4 | accepted | Common CLI policy types cover invocation mode, status target, output target, color mode, output conflict, informational command, and common options. |
| F-5 | accepted | Validation failures return `StatusResult` carrying `cli_usage_error` / exit `10`; the status-stderr color regression is covered. |
| F-6 | accepted | `--status=stdout` rejects stdout data artifacts and informational stdout through Slice03 status-stream semantics. |
| F-7 | accepted | `--output` plus inherited positional output alias maps to `cli_usage_error` before input/model processing. |
| F-8 | accepted | Informational commands are stdout-owned `ok` / `0` helpers and mutually exclusive as flags. |
| F-9 | accepted | Canonical and inherited parser, grounder, and engine names are recognized without default deprecation warnings. |
| F-10 | accepted | Color controls cover auto, always, never, no-color, no-colour, and `NO_COLOR`; disabling controls still win when status is selected. |
| F-11 | accepted | ANSI legality allows only human stderr on a TTY and rejects stdout artifacts, tagged status, supervised, machine, file, pipe, and no-TTY output. |
| F-12 | accepted | Tests inject `TerminalKind` and call `detect_terminal(bool)` rather than probing the live shell. |
| F-13 | accepted | `ProvenanceRecord` fields are caller supplied; required version fields are enforced; no shell/git metadata discovery calls exist. |
| F-14 | accepted | Version/provenance formatters emit stable field-oriented output, reject placeholder prose, reject partial required version records, and permit absent optional provenance-only fields. |
| F-15 | accepted | Dependency-gate search found no CLI11, fmt, Catch2, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, or GSL import/exposure. |
| F-16 | accepted | Public-header hazard search found no global `using namespace`, mutable namespace state, raw ownership transfer, explicit allocation/deallocation, or namespace `std` additions. |
| F-17 | accepted | Non-failing mapping/policy helpers include `noexcept`; tests include all new public headers. |
| F-18 | accepted | CMake builds `cli_policy.cpp`, `tty.cpp`, and `provenance.cpp` and registers `pandapi_cli_tty_provenance_smoke`. |
| F-19 | accepted | `./scripts/build-runtime.sh` passed on macos-arm64 with 4/4 CTest tests. |
| F-20 | accepted | `./scripts/build-parser.sh`, `./scripts/build-grounder.sh`, and `./scripts/build-engine.sh` passed on macos-arm64. |
| F-21 | accepted | `./scripts/smoke-test.sh --negative` passed: 4 passed, 0 failed. |
| F-22 | accepted | Protected-path diff check produced no output. |
| F-23 | accepted | CC closing report contains capability verdict, implementation decisions, verification, ledger walk, silent-drop check, bubble-up, and Iteration 01 CDC fix sections. |
| F-24 | accepted | CC did not create `cdc-verification.md`. |

CDC also reproduced the ledger row count and open-row checks:

```text
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md
24
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md
```

## CDC Probes

### Required provenance fields

CDC reran the required-field probe from `cc-iteration01-prompt.md`:

```text
printf ... | c++ -std=c++17 \
  -Ipandapi-runtime/include \
  pandapi-runtime/src/status.cpp pandapi-runtime/src/provenance.cpp \
  -x c++ - -o /private/tmp/provenance_required_probe &&
  /private/tmp/provenance_required_probe
```

Result:

```text
rejected
```

The probe exited 0. `format_version` now rejects a record with only
`canonical_command` populated as `cli_usage_error` / exit `10`.

### Color always with status stderr

CDC reran the color/status probe from `cc-iteration01-prompt.md`:

```text
printf ... | c++ -std=c++17 \
  -Ipandapi-runtime/include \
  pandapi-runtime/src/status.cpp pandapi-runtime/src/status_io.cpp \
  pandapi-runtime/src/tty.cpp pandapi-runtime/src/cli_policy.cpp \
  -x c++ - -o /private/tmp/color_status_stderr_probe &&
  /private/tmp/color_status_stderr_probe
```

Result:

```text
rejected
```

The probe exited 0. `validate_common_cli_options` now treats any selected
tagged status target, including stderr, as a machine-parsed stream for
effective `ColorMode::Always`, unless disabling controls force `Never`.

## Code Review Notes

No blocking findings remain.

The provenance repair is narrow and contract-aligned: required version fields
flow through an explicit required-field helper, while optional compatibility
and detailed provenance fields remain omittable. The color repair is likewise
narrow: the status-target predicate now covers both stdout and stderr, while
existing supervised/machine-output and disabling-wins behavior remains intact.

Arc04 still has the intended no-binary-adoption boundary. This verification
does not assert managed-process conformance at executable boundaries; that
proof remains routed to Arc05 and Arc06.

## Bubble-up to Arc04

Arc04 can proceed to slice05 from these accepted inputs:

- common CLI policy values and validation helpers;
- canonical and inherited command identity helpers;
- informational-command helpers for help, version, and provenance;
- status/stdout conflict validation and color/status validation;
- TTY/color and ANSI legality helpers;
- caller-supplied version/provenance field assembly with required stable
  version fields enforced;
- seam coverage proving the helpers without CLI11, binary adoption, or live
  shell TTY dependence.

Slice05 should consume these helpers when building fixture and seam-test
substrate support. CLI11 remains gated by golden-output/process tests, and
per-binary adoption remains Arc05 work.
