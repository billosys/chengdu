# CC Prompt: Arc04 Slice04 - cli-tty-provenance-core

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
4. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/slice-doc.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md`
6. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md`
7. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
8. `docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`
9. `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
10. `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

Also load the C++ guideline substrate for the touched rule families:

- `cpp-guidelines` `11-anti-patterns.md`
- `02-api-design.md`
- `03-functions.md`
- `06-error-handling.md`
- `10-expressions-and-statements.md`
- `12-project-structure-and-tooling.md`
- `13-standard-library.md`

## Mission

Implement Arc04 Slice04: a tested `pandapi::runtime` CLI/TTY/provenance core.

The deliverable is a C++17, standard-library-first runtime facility that turns
Arc03's accepted command naming, option-policy, help/version/provenance, and
TTY/color rules into code:

- common CLI policy value types;
- command identity for canonical `pandapi-*` and inherited `pandaPI*` names;
- validation helpers for common option conflicts that map to
  `cli_usage_error`;
- informational-command helpers for `--help`, `--version`, and
  `--provenance`;
- TTY/color policy helpers for `--color`, `--no-color`, `--no-colour`, and
  `NO_COLOR`;
- ANSI legality helpers that keep machine/artifact/status streams clean;
- provenance/version field assembly from caller-supplied metadata;
- seam tests proving the policy and formatting behavior;
- README/CMake updates.

Keep this slice inert with respect to the inherited binaries. Do not replace
parser, grounder, or engine CLI parsing yet.

## Hard Boundaries

- No edits under `pandaPI/`.
- No `pandapi-*` wrapper, symlink, copied binary, release packaging, or
  inherited-name migration work.
- No replacement of `gengetopt` or per-binary CLI parsing.
- No process fixture harness or golden-output runner; slice05 owns that test
  substrate.
- No CLI11 import, vendoring, fetching, `find_package`, or direct include.
  This slice may define the local policy facade that a later CLI11 adapter must
  target.
- No fmt, Catch2, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, or
  GSL import.
- Do not expose a third-party type from a public runtime header.
- Do not touch workflow, top-level README, release asset, manifest,
  `vendor.env`, `pins.env`, `dist`, `build`, or `release` paths.
- Do not create `cdc-verification.md`; CDC owns that file.

If you discover CLI11 must enter in this slice, stop and record that as an
amendment request or bubble-up rather than importing it silently.

## Implementation Guidance

Prefer small, strongly typed C++17 APIs:

- `enum class InvocationMode { HumanCli, Supervised }` or equivalent;
- `enum class StatusTarget { None, Stderr, Stdout }` or equivalent;
- `enum class OutputTarget { None, File, Stdout }` or equivalent;
- `enum class ColorMode { Auto, Always, Never }` or equivalent;
- `enum class TerminalKind { Tty, NotTty, Unknown }` or equivalent;
- a `CommandIdentity` value that records canonical name, invoked name, and
  whether invocation used an inherited compatibility name;
- a `CommonCliOptions` value for already-parsed common policy;
- validation functions returning `StatusResult<...>` so bad CLI shape maps to
  `cli_usage_error`;
- provenance/version functions that assemble stable field-oriented output from
  explicit caller-supplied values;
- no global mutable process state;
- `std::string_view` for static names and `std::string` for owned values;
- `noexcept` on pure mapping and legality checks.

For TTY detection, keep tests deterministic. Prefer a policy function that
accepts an observed `TerminalKind` and, if you add OS-level detection, keep it
small, isolated, and separately testable without relying on the current shell
being interactive.

## Required Verification

Run the ledger commands and record the commit SHA plus outputs in
`ledger.md`. At minimum, run:

```bash
./scripts/build-runtime.sh
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
```

Also verify:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md
```

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all 24 rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc04, including whether slice05 planning should change.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
