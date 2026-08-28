# Arc03 Slice 04: CLI Naming, Version, and Migration Contract

## Executive Summary

0.3.0 introduces canonical namespaced commands:

- `pandapi-parser`
- `pandapi-grounder`
- `pandapi-engine`

0.3.0 is a compatibility transition, not a breaking rename. The inherited
commands `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` remain
executable in release assets and installed trees. Human-facing documentation
should prefer the `pandapi-*` names, but compatibility commands must not emit a
runtime deprecation warning by default because stderr noise would break the
managed-process stream contract and could surprise wolong.

The release asset contract remains externally compatible: asset names stay
`pandapi-<tag>-<platform>.tar.gz`; `SHA256SUMS` and `release-manifest.txt`
retain their shape; the four-command fetch/verify/extract path remains valid;
and inherited binary names remain available after extraction. Adding
`pandapi-*` entry points is an additive migration step, not an operator-approved
breaking transition.

CLI11 is the selected implementation candidate for Arc04/Arc05, but adoption
is conditional on preserving this contract through golden output tests. This
slice defines semantics only; it does not adopt dependencies or edit source,
scripts, workflows, release assets, build outputs, `vendor.env`, or `pins.env`.

## Evidence Base

Primary inputs:

- Arc01 `audit-synthesis-pandapi.md`, especially the CLI identity, stdout,
  status, version, and process-facade findings.
- Arc02 `combined-library-recommendations.md`, especially the CLI11,
  diagnostics/fmt, test, and release-packaging gates.
- Arc03 slice01 `supported-surface-classification.md` and
  `slice01-supported-surface-classification/cdc-verification.md`.
- Arc03 slice02 `status-exit-signal-taxonomy.md` and
  `slice02-status-exit-signal-taxonomy/cdc-verification.md`.
- Arc03 slice03 `stdio-event-tty-contract.md` and
  `slice03-stdio-event-tty-contract/cdc-verification.md`.
- Current release-shape inputs from `README.md`,
  `scripts/package-release.sh`, `release/release-manifest.txt`, and
  `release/SHA256SUMS`, read as evidence only.

The prior slices are load-bearing: supported surfaces stay limited to parser
normal HDDL parse, grounder normal `.htn` grounding, and engine normal search;
status names and numeric exit codes stay unchanged; stdout/stderr ownership
and tagged text status-only output stay unchanged.

## Naming Decision And Compatibility Posture

Canonical 0.3.0 command names:

| Component | Canonical command | Inherited command | 0.3.0 treatment |
|---|---|---|---|
| Parser | `pandapi-parser` | `pandaPIparser` | Compatibility alias or symlink remains executable. |
| Grounder | `pandapi-grounder` | `pandaPIgrounder` | Compatibility alias or symlink remains executable. |
| Engine | `pandapi-engine` | `pandaPIengine` | Compatibility alias or symlink remains executable. |

Policy:

- `pandapi-*` names are canonical for new documentation, help examples,
  provenance identity, tests, and Arc05 migration work.
- `pandaPI*` names remain supported compatibility entry points for 0.3.0 so
  wolong and existing release consumers do not need an immediate command rename.
- Compatibility entry points must behave the same as their canonical command
  for supported normal paths, status mapping, stdout/stderr ownership, color,
  help, version, and parse errors.
- Compatibility entry points may identify themselves in `--version` and
  provenance output as compatibility invocations, but they must not print a
  deprecation warning on ordinary execution by default.
- A future breaking removal of inherited names requires an explicit operator
  decision, release-note migration table, wolong verification, and a new design
  entry point. This slice does not approve that break.

Implementation preference for Arc05: make the canonical `pandapi-*` files the
real command targets and preserve inherited names as symlinks or tiny wrappers
that forward `argv[0]`/compatibility identity to the same implementation. If a
platform or packaging step makes symlinks unsuitable, wrappers are acceptable.
Hard errors or removal are rejected for 0.3.0.

## Release Asset And Wolong Migration Implications

0.3.0 preserves wolong's frozen fetch/install contract:

- Release asset names remain `pandapi-<tag>-<platform>.tar.gz`.
- `SHA256SUMS` remains a checksum file covering release assets.
- `release-manifest.txt` remains the aggregated provenance manifest.
- The four-command download, checksum, extract, and run path remains valid.
- `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` remain available in
  the extracted directory.

Additive 0.3.0 tarball contents may include:

- `pandapi-parser`
- `pandapi-grounder`
- `pandapi-engine`
- the inherited `pandaPI*` compatibility names
- `provenance.txt`
- `fixtures/`

Because the inherited names stay executable, this is a compatibility
transition rather than a breaking transition. Arc06 must still document the
additive command-name change in the behavior-change table and update README
examples to prefer `pandapi-*` while preserving an inherited-name compatibility
example for wolong and existing users.

If the operator later chooses a breaking rename, Arc05/Arc06 must not infer it
from this document. The breaking migration must explicitly state whether old
names are removed, whether tarball contents change, how many releases retain
aliases, and what wolong verification proves the migration.

## Supported Command Synopsis

The supported command grammar is intentionally narrow. Optional inherited
surfaces do not become supported because an option exists or because CLI11 can
parse it.

| Command | Supported surface | Canonical synopsis | Argument semantics |
|---|---|---|---|
| `pandapi-parser` | parser normal HDDL parse | `pandapi-parser [COMMON] [--output OUT.htn|-] DOMAIN.hddl PROBLEM.hddl` | `DOMAIN.hddl` and `PROBLEM.hddl` are required readable HDDL inputs. `--output` selects the parse artifact path; `-` selects stdout as the artifact sink. For compatibility, a third positional output path may remain accepted as an alias for `--output`. |
| `pandapi-grounder` | grounder normal `.htn` grounding | `pandapi-grounder [COMMON] [--output OUT.sas|-] INPUT.htn` | `INPUT.htn` is a required readable parser artifact. `--output` selects the grounded planner artifact path; `-` selects stdout. For compatibility, a second positional output path may remain accepted as an alias for `--output`. |
| `pandapi-engine` | engine normal search | `pandapi-engine [COMMON] [--output PLAN|-] INPUT.sas` | `INPUT.sas` is a required readable planner input. `--output` selects plan artifact output; if omitted, stdout is the default artifact sink for compatibility with existing shell redirection. |

Common argument rules:

- Required positional inputs are validated by CLI shape before input/model
  processing begins.
- `--output PATH` and compatibility positional output forms are mutually
  exclusive; using both is `cli_usage_error` and exit code `10`.
- `--output -` means stdout is the data artifact sink, so human diagnostics,
  progress, statistics, and status summaries must not share stdout.
- If `--status=stdout` is selected, a stdout data artifact is forbidden unless
  the artifact is redirected to a file path; conflicts are `cli_usage_error`
  and exit code `10`.

## Mode And Option Semantics

Common options are policy-level contract names for Arc05 implementation and
slice05 golden tests:

| Option | Semantics |
|---|---|
| `--help` | Print help to stdout and exit `ok`/`0`. Does not process input files. |
| `--version` | Print concise version/provenance identity to stdout and exit `ok`/`0`. Does not process input files. |
| `--provenance` | Print detailed build/source provenance to stdout and exit `ok`/`0`. Does not process input files. |
| `--output PATH|-` | Select the data artifact sink for the supported normal path. `-` means stdout. |
| `--supervised` | Select supervised/process-manager behavior: quiet human prose, no progress spinner, no unstructured statistics, no ANSI, deterministic parse-error handling. Does not by itself enable tagged status. |
| `--status` | Enable one final `PANDAPI_STATUS` record on stderr. Equivalent to `--status=stderr`. |
| `--status=stderr|stdout` | Select the tagged status stream. `stderr` is default and is compatible with stdout data artifacts. `stdout` is allowed only when stdout is not carrying a data artifact or human informational output. |
| `--quiet` | Suppress human progress, banners, option dumps, and statistics. Does not suppress fatal diagnostics or tagged status records. |
| `--verbose` | Add human diagnostics/statistics to stderr only. Does not add fields to stdout artifacts. |
| `--color=auto|always|never` | Select human stderr color policy. `auto` is default. `always` is rejected in supervised or machine-status output. `never` is equivalent to `--no-color`. |
| `--no-color` | Disable ANSI output everywhere. |
| `--no-colour` | Compatibility spelling for `--no-color`. |

Mode interactions:

- Default human CLI mode is selected when `--supervised` is absent.
- Supervised mode plus `--status` is the recommended process-manager contract.
- `--status` never changes the process exit code; it only emits the final
  tagged status record.
- Tagged status records use the slice03 `PANDAPI_STATUS` format and required
  slice02 fields. This slice does not reopen JSON Lines or `nlohmann/json`.
- Human diagnostics remain non-normative. Supervisors branch on exit code and
  tagged status fields, not diagnostic wording.

## Help, Usage, And Examples Policy

Help is a successful informational command:

- `--help` exits `ok`/`0`.
- Help writes to stdout.
- Help writes nothing to stderr unless `--status` is also enabled; in that
  case the final `PANDAPI_STATUS` record goes to the selected status stream.
- Help must not require existing input files and must not enter input/model
  processing.

Help content requirements:

- Show the canonical `pandapi-*` command in the usage line.
- Include the inherited `pandaPI*` name only in a clearly labeled
  compatibility note.
- Show only supported normal-path options in default help.
- Mention `--supervised`, `--status`, `--quiet`, `--verbose`, `--output`,
  `--color`, `--no-color`, and `--no-colour`.
- Include examples for human CLI and supervised/process-manager use.
- Avoid advertising parser verifier/output helpers, H2, cpddl/FAM,
  interactive, translation, SAT, BDD, or CUDD as supported 0.3.0 surfaces.

Recommended examples for help and docs:

```bash
pandapi-parser --output plan.htn domain.hddl problem.hddl
pandapi-grounder --output plan.sas plan.htn
pandapi-engine plan.sas > plan.raw
pandapi-engine --supervised --status --output plan.raw plan.sas
```

## Version And Provenance Policy

Version is a successful informational command:

- `--version` exits `ok`/`0`.
- Version writes concise identity to stdout.
- Version writes nothing to stderr unless `--status` is enabled.
- Version must not process input files.

`--version` output must include, in stable field form suitable for golden
tests:

- canonical command name;
- invoked compatibility name when `argv[0]` is inherited;
- component name (`parser`, `grounder`, or `engine`);
- chengdu release version;
- managed-process contract version (`0.3.0`);
- upstream pandaPI project name;
- upstream commit SHA;
- chengdu source prefix;
- chengdu build commit;
- platform;
- compiler family/version when available;
- license/NOTICE pointer.

`--provenance` is the detailed form. It should include all `--version` fields
plus the import commit, build timestamp if reproducibly available, dependency
license/provenance entries for adopted runtime dependencies, and a pointer to
`THIRD-PARTY-LICENSES`. Its fields should align with `provenance.txt` and
`release-manifest.txt` so Arc06 can reconcile binary output against release
assets.

The inherited upstream version strings such as `pandaPIparser 0.1`,
`pandaPIgrounder 0.1`, and `pandaPIengine 0.1` must not be the sole visible
identity after Arc05 migration. They may appear as upstream provenance fields,
not as the chengdu product version.

## CLI Parse-Error And Status Mapping Policy

Malformed CLI shape maps to `cli_usage_error` and exit code `10`.

Parse-error cases include:

- unknown options;
- missing required input arguments;
- too many positional inputs;
- mutually exclusive `--output` and compatibility positional output;
- malformed option values;
- invalid enum values such as an unknown `--color` or `--status` target;
- selecting `--status=stdout` while stdout is also selected for a data artifact
  or human informational output.

Parse-error behavior:

- print concise usage diagnostics to stderr;
- do not print human diagnostics to stdout;
- do not open, parse, validate, or process input/model files;
- do not create output artifacts;
- if `--status` was successfully parsed before the error is classified, emit
  a final `PANDAPI_STATUS` record carrying `status=cli_usage_error` and
  `exit_code=10`;
- if option parsing fails before status-stream selection is available, exit
  `10` with stderr diagnostics only.

Help and version are not parse errors when requested alone, and they exit
`ok`/`0`.

## Color, No-Color, No-Colour, And TTY Policy

Color policy preserves slice03:

- default `--color=auto`;
- ANSI is allowed only for human stderr on a suitable TTY;
- ANSI is forbidden in stdout artifacts, tagged status records, supervised
  output, machine output, files, and no-TTY output;
- `--no-color`, `--no-colour`, `--color=never`, and `NO_COLOR` disable ANSI
  everywhere;
- no-color/no-colour controls override `--color=auto`;
- `--color=always` is a human-only debugging affordance and is invalid with
  `--supervised`, `--status=stdout`, or any machine-parsed stream.

If both a color-enabling and color-disabling option are present, the disabling
option wins. This avoids the Arc01 parser `--no-colour` failure mode and keeps
supervised output deterministic.

## Legacy, Experimental, Unsupported, And Future Surfaces

Default help must not make non-supported surfaces look like supported 0.3.0
interfaces. If inherited options remain reachable, they must be hidden from
default help or labeled outside the supported contract.

| Surface | Disposition | CLI presentation | Status mapping |
|---|---|---|---|
| Parser verifier/output helper paths | legacy | Hidden from default help or listed under compatibility, not normal parser parse. | `legacy_surface` under supervised invocation; compatibility behavior may remain for human explicit use. |
| Grounder H2 | experimental | Hidden or labeled experimental opt-in. | `experimental_surface`; dependency failures use `dependency_failure`. |
| Grounder `cpddl`/FAM | legacy | Hidden or labeled advanced legacy behavior. | `legacy_surface` under managed invocation; dependency-boundary failures use `dependency_failure`. |
| Engine interactive | legacy | Hidden from supervised help; human-only TTY legacy note if visible. | `legacy_surface` for supervised/no-TTY invocation. |
| Engine translation | experimental | Hidden or labeled experimental opt-in. | `experimental_surface`; subprocess failures use `child_process_failure`. |
| Engine SAT | unsupported | Hidden or explicitly labeled unsupported if a build exposes it. | `unsupported_feature`. |
| Engine BDD/CUDD | future | Hidden or explicitly labeled future/unavailable if a build exposes it. | `future_surface`. |

`--help=all` or an equivalent future spelling may list hidden inherited
options only if each entry carries its disposition label and does not imply
managed-process support. Slice05 should test any visible non-supported option
that ships in default binaries.

## CLI11 And Arc02 Dependency Gates

CLI11 disposition after this slice: adopt as the selected CLI parser
implementation candidate for Arc04/Arc05, but only behind golden-output and
process-contract tests.

Required CLI11 gates:

- golden output for `--help`, `--version`, `--provenance`, parse errors,
  missing args, malformed option values, compatibility invocation names, color
  controls, `--supervised`, and `--status`;
- process fixtures proving exit code, stdout, stderr, tagged status, and no
  input/model processing after `cli_usage_error`;
- compatibility with inherited positional output forms where retained;
- no expansion of legacy, experimental, unsupported, or future surfaces through
  generated help or easier option parsing.

Other Arc02 gates remain unchanged:

- `fmt` remains behind a diagnostics/process facade.
- `nlohmann/json` remains held because 0.3.0 uses tagged text status-only
  output, not JSON.
- Catch2 and process fixtures remain slice05/Arc04-owned.
- `tl::expected` remains behind the local status/result substrate.
- reproc++ remains conditional and is not justified by CLI naming.
- Abseil and Boost.Process remain rejected as 0.3.0 foundation choices.

## Later Slice And Arc Inputs

Slice05 must turn this report into contract tests covering:

- canonical and inherited command names;
- help/version/provenance stdout and exit `0`;
- missing args, unknown options, malformed values, and option conflicts mapping
  to `cli_usage_error`/`10`;
- `--supervised` quiet behavior;
- `--status`, `--status=stderr`, and `--status=stdout` conflict behavior;
- `--color`, `--no-color`, `--no-colour`, `NO_COLOR`, TTY, and no-TTY output;
- visible legacy, experimental, unsupported, and future options if present.

Arc04 must provide the shared CLI/diagnostics substrate: CLI parser wrapper,
common option structs, status-stream selection, diagnostics routing,
TTY/color policy, version/provenance formatting, and golden-output harness
support.

Arc05 must migrate parser, grounder, and engine per binary to canonical
`pandapi-*` commands while keeping inherited `pandaPI*` compatibility commands
available for 0.3.0.

Arc06 must update release docs, `THIRD-PARTY-LICENSES`/NOTICE material for any
adopted runtime dependency, behavior-change table entries, README examples,
release notes, `release-manifest.txt` provenance reconciliation, and wolong
verification evidence.

## Final Recommendation

Ship 0.3.0 as a compatibility transition:

- canonicalize on `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`;
- preserve `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` as
  executable compatibility names;
- prefer new names in documentation and tests while avoiding default runtime
  deprecation warnings;
- expose supervised behavior with `--supervised`;
- enable tagged `PANDAPI_STATUS` output with `--status` or
  `--status=stderr|stdout`;
- keep help/version/provenance successful, stdout-owned informational commands;
- map malformed CLI shape to `cli_usage_error`/`10` before input processing;
- adopt CLI11 only after golden tests prove this contract.

This satisfies Arc03 row A5 without silently breaking wolong's frozen
fetch/install shape or letting CLI tooling decide product support.
