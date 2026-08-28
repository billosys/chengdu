# CC Iteration 01 Prompt: Arc04 Slice03 - diagnostics-status-io

You are CC continuing the already-implemented Arc04 Slice03 in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

CDC did not accept the first close. This is a narrow corrective iteration, not
a new slice and not a chance to broaden the runtime surface.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc04-shared-runtime-substrate/arc-plan.md`
4. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/slice-doc.md`
5. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md`
6. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/closing-report.md`
7. `pandapi-runtime/include/pandapi/runtime/status_io.hpp`
8. `pandapi-runtime/src/status_io.cpp`
9. `pandapi-runtime/tests/status_io_smoke.cpp`

Also load the C++ guideline substrate for this exact defect family:

- `cpp-guidelines` `11-anti-patterns.md`
- `10-expressions-and-statements.md`
- `06-error-handling.md`
- `13-standard-library.md`

## CDC Blocking Finding

CDC found undefined behavior in the tagged status parser's numeric field
handling. In `pandapi-runtime/src/status_io.cpp`, `parse_int` accumulates into
`int` without a range check:

```cpp
result = (result * 10) + (character - '0');
```

A malformed status record with an oversized `exit_code` is eventually rejected,
but only after signed integer overflow. CDC reproduced this with UBSan:

```text
pandapi-runtime/src/status_io.cpp:147:22: runtime error: signed integer overflow: 999999999 * 10 cannot be represented in type 'int'
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior pandapi-runtime/src/status_io.cpp:147:22
rejected
```

This blocks F-6 because malformed machine text must map cleanly to
`input_invalid`; it must not pass through undefined behavior first.

## Mission

Fix numeric parsing in the status I/O parser so malformed numeric fields are
rejected without undefined behavior.

Required coverage:

- oversized `exit_code` values;
- oversized `signal_number` values;
- non-digit numeric values should still reject as before;
- valid values should continue to parse;
- `signal_terminated` should still require a positive signal number and map to
  `128 + signal_number` through `ProcessStatus`.

Preferred implementation:

- Use `std::from_chars` from `<charconv>` for C++17 integer parsing; or
- add an explicit max-before-multiply guard before updating the accumulator.

Either approach is acceptable if it is simple, deterministic,
standard-library-only, and covered by tests.

## Hard Boundaries

- Do not edit `pandaPI/`.
- Do not adopt runtime helpers in parser, grounder, or engine.
- Do not add or vendor dependencies.
- Do not import fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json,
  Boost, Abseil, or GSL.
- Do not change public status I/O semantics beyond the overflow-safe malformed
  numeric rejection.
- Do not create `cdc-verification.md`; CDC owns that file.

## Required Code Changes

1. Repair `parse_int` or replace it with an overflow-safe C++17 parser.
2. Add regression tests in `pandapi-runtime/tests/status_io_smoke.cpp` that
   prove:
   - an oversized `exit_code` returns `StatusResult<StatusRecord>::failure`
     with `input_invalid`;
   - an oversized `signal_number` on a `signal_terminated` record returns
     `input_invalid`;
   - no signed integer overflow or sanitizer finding is needed for the tests to
     pass.
3. Update `ledger.md` evidence for the affected rows, at minimum F-6 and any
   test/build rows whose evidence changed.
4. Update `closing-report.md` with an "Iteration 01 CDC Fix" section that
   names the defect, the repair, the new regression tests, and the verification
   commands run.

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
rg -c '^\| F-' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md
! rg -n '\| open \|' docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/ledger.md
! test -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md
```

Also run an explicit sanitizer probe that would have failed before this
iteration. You may use this exact shape, or an equivalent checked command:

```bash
printf '%s\n' \
  '#include "pandapi/runtime/status_io.hpp"' \
  '#include <iostream>' \
  'int main() {' \
  '  auto parsed = pandapi::runtime::parse_status_record("PANDAPI_STATUS\tstatus=ok\tcomponent=engine\tsurface=normal_search\tsurface_disposition=supported\texit_code=999999999999999999999999999999999999\tclass=success");' \
  '  std::cout << (parsed.has_value() ? "accepted" : "rejected") << "\n";' \
  '  return parsed.has_value() ? 1 : 0;' \
  '}' |
  c++ -std=c++17 -fsanitize=signed-integer-overflow \
    -Ipandapi-runtime/include \
    pandapi-runtime/src/status.cpp \
    pandapi-runtime/src/status_io.cpp \
    -x c++ - \
    -o /private/tmp/status_io_overflow_probe &&
  /private/tmp/status_io_overflow_probe
```

Expected result: command exits 0, prints `rejected`, and produces no UBSan
runtime error.

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
- the sanitizer probe result;
- original slice gate results;
- confirmation that no `cdc-verification.md` was created.
