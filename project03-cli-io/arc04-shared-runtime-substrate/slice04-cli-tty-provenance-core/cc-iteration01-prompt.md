# CC Iteration 01 Prompt: Arc04 Slice04 - cli-tty-provenance-core

You are CC continuing the already-implemented Arc04 Slice04 in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

CDC did not accept the first close. This is a narrow corrective iteration, not
a new slice and not a chance to broaden the runtime surface.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
4. `docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
6. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/slice-doc.md`
7. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/ledger.md`
8. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/closing-report.md`
9. `pandapi-runtime/include/pandapi/runtime/cli_policy.hpp`
10. `pandapi-runtime/include/pandapi/runtime/tty.hpp`
11. `pandapi-runtime/include/pandapi/runtime/provenance.hpp`
12. `pandapi-runtime/src/cli_policy.cpp`
13. `pandapi-runtime/src/tty.cpp`
14. `pandapi-runtime/src/provenance.cpp`
15. `pandapi-runtime/tests/cli_tty_provenance_smoke.cpp`

Also load the C++ guideline substrate for this exact defect family:

- `cpp-guidelines` `02-api-design.md`
- `03-functions.md`
- `06-error-handling.md`
- `11-anti-patterns.md`
- `13-standard-library.md`

## CDC Blocking Findings

### S04-CDC-001: Required version/provenance fields are not enforced

CDC found that `format_version` accepts a `ProvenanceRecord` with only
`canonical_command` populated:

```bash
printf '%s\n' \
  '#include "pandapi/runtime/provenance.hpp"' \
  '#include <iostream>' \
  'int main() {' \
  '  pandapi::runtime::ProvenanceRecord record;' \
  '  record.canonical_command = "pandapi-engine";' \
  '  auto result = pandapi::runtime::format_version(record, pandapi::runtime::Component::Engine);' \
  '  std::cout << (result.has_value() ? "accepted" : "rejected") << "\n";' \
  '  if (result.has_value()) { std::cout << result.value(); }' \
  '  return result.has_value() ? 1 : 0;' \
  '}' |
  c++ -std=c++17 \
    -Ipandapi-runtime/include \
    pandapi-runtime/src/status.cpp \
    pandapi-runtime/src/provenance.cpp \
    -x c++ - \
    -o /private/tmp/provenance_required_probe &&
  /private/tmp/provenance_required_probe
```

Observed output:

```text
accepted
canonical_command=pandapi-engine
```

This conflicts with the Arc03 version/provenance policy and the slice04 scope.
`--version` output must include stable fields suitable for golden tests:
canonical command, component, chengdu version, managed-process contract
version, upstream project, upstream commit, source prefix, build commit,
platform, compiler, license pointer, and NOTICE pointer. `invoked_command`
is required only when the invocation uses an inherited compatibility name.

Required behavior:

- `format_version` must reject missing required version fields with
  `StatusCode::CliUsageError` / exit `10`.
- `version_fields` must not silently produce a partial required version
  record.
- Optional detailed provenance-only fields may still be omitted when absent,
  including import commit, build timestamp, and third-party license pointer.
- Placeholder prose such as `unknown`, `placeholder`, `todo`, `tbd`, and
  `n/a` must still reject as before.

### S04-CDC-002: `--color=always` with tagged status on stderr is accepted

CDC found that common CLI validation accepts `--color=always` when tagged
status output is selected on stderr:

```bash
printf '%s\n' \
  '#include "pandapi/runtime/cli_policy.hpp"' \
  '#include <iostream>' \
  'int main() {' \
  '  pandapi::runtime::CommonCliOptions options;' \
  '  options.status_target = pandapi::runtime::StatusTarget::Stderr;' \
  '  options.color_controls.color_mode = pandapi::runtime::ColorMode::Always;' \
  '  auto result = pandapi::runtime::validate_common_cli_options(options, pandapi::runtime::Component::Engine);' \
  '  std::cout << (result.has_value() ? "accepted" : "rejected") << "\n";' \
  '  return result.has_value() ? 1 : 0;' \
  '}' |
  c++ -std=c++17 \
    -Ipandapi-runtime/include \
    pandapi-runtime/src/status.cpp \
    pandapi-runtime/src/status_io.cpp \
    pandapi-runtime/src/tty.cpp \
    pandapi-runtime/src/cli_policy.cpp \
    -x c++ - \
    -o /private/tmp/color_status_stderr_probe &&
  /private/tmp/color_status_stderr_probe
```

Observed output:

```text
accepted
```

This conflicts with Arc03: `--color=always` is invalid with `--supervised`,
`--status=stdout`, or any machine-parsed stream. `--status=stderr` is still a
machine-parsed tagged status stream even though stderr is also the human
diagnostic channel.

Required behavior:

- `validate_common_cli_options` must reject `ColorMode::Always` whenever
  `status_target != StatusTarget::None`, unless disabling controls reduce the
  effective color mode to `Never`.
- The existing `--color=always` rejection for supervised and machine-output
  cases must remain intact.
- The existing disabling-wins behavior for `--color=never`, `--no-color`,
  `--no-colour`, and `NO_COLOR` must remain intact.

## Mission

Repair the two contract misses without expanding slice04.

Required coverage:

- missing required version field rejects as `cli_usage_error`;
- a record with every required version field succeeds;
- optional detailed provenance-only fields may be omitted;
- placeholder values still reject;
- `--color=always --status=stderr` rejects as `cli_usage_error`;
- `--color=always --status=stdout` still rejects;
- `--color=always --supervised` still rejects;
- a disabling control plus status selection is accepted because effective color
  is `Never`.

## Hard Boundaries

- Do not edit `pandaPI/`.
- Do not adopt runtime helpers in parser, grounder, or engine.
- Do not add `pandapi-*` entry points, wrappers, symlinks, copied binaries, or
  renamed binaries.
- Do not add or vendor dependencies.
- Do not import fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json,
  Boost, Abseil, or GSL.
- Do not change public status I/O semantics outside the validation/provenance
  repairs above.
- Do not create `cdc-verification.md`; CDC owns that file.

## Required Code Changes

1. Repair provenance/version validation so required `--version` fields cannot
   be silently omitted.
2. Repair common CLI color validation so tagged status on stderr is treated as
   a machine-parsed stream for `--color=always`.
3. Add regressions in
   `pandapi-runtime/tests/cli_tty_provenance_smoke.cpp`.
4. Update `ledger.md` evidence for affected rows, at minimum F-5, F-10,
   F-13, F-14, F-19, and any test/build rows whose evidence changed.
5. Update `closing-report.md` with an "Iteration 01 CDC Fix" section that
   names both findings, the repairs, the new regression tests, and the
   verification commands run.

## Required Verification

Run the original slice gates:

```bash
./scripts/build-runtime.sh
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

Also re-run the two CDC probes above. Expected result for both probes after
the fix: command exits `0` and prints `rejected`.

## Close Requirements

Commit the corrective change and close-report update together. Include the
required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```

Report back with:

- commit SHA;
- files changed;
- disposition of S04-CDC-001 and S04-CDC-002;
- the two CDC probe results;
- original slice gate results;
- confirmation that no `cdc-verification.md` was created.
