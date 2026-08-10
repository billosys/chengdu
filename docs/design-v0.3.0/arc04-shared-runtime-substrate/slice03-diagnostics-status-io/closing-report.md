# Slice 03 Closing Report: diagnostics-status-io

Date: 2026-08-09
Branch: `release/0.3.x`
Role: CC

## Capability Verdict

Delivered. Arc04 now has a tested, standard-library-only
`pandapi::runtime` Diagnostics/status I/O facade for the Arc03 tagged status
contract.

The slice adds a typed `StatusRecord`, stdout/stderr status-stream legality
helpers, a single-line `PANDAPI_STATUS` writer, a tagged status parser returning
`StatusResult<StatusRecord>`, final-status flushing, single-line value escaping
and rejection, and `PartialOutputPolicy` vocabulary. Parser, grounder, and
engine behavior is not changed; no binary adoption was performed.

## Implementation Decisions

- Header/source split: public declarations live in
  `pandapi-runtime/include/pandapi/runtime/status_io.hpp`; implementation lives
  in `pandapi-runtime/src/status_io.cpp`.
- Main value type: `StatusRecord`, because the facade models one final status
  record rather than a generic line.
- Optional fields: stored as validated `StatusField` values. Required fields
  are generated from `ProcessStatus`; reserved field names cannot be injected
  as optional fields. Unknown/absent values are omitted instead of represented
  with placeholder prose.
- Malformed tagged text: `parse_status_record` returns
  `StatusResult<StatusRecord>::failure(...)` with `input_invalid`, matching the
  Arc03 machine-input failure class for this local parser seam.
- Surface names: validated as lowercase token names such as `normal_search`.
  Per-binary surface adoption still belongs to Arc05.
- Final flush testing: `status_io_smoke.cpp` uses a custom stream buffer that
  counts `sync()` calls, proving `write_status_record` flushes before return.
- Dependency posture: no fmt, CLI11, Catch2, `tl::expected`, reproc++,
  nlohmann/json, Boost, Abseil, or GSL import. Formatting, parsing, and stream
  handling use C++17 standard-library types.

## Verification

Commands run by CC:

```text
./scripts/build-runtime.sh
./scripts/build-parser.sh
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/smoke-test.sh --negative
git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release
```

Results:

- `./scripts/build-runtime.sh`: passed on `macos-arm64`; CTest reported 3/3
  runtime tests passed.
- `./scripts/build-parser.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/build-grounder.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/build-engine.sh`: passed on `macos-arm64`; inherited warnings
  only.
- `./scripts/smoke-test.sh --negative`: passed, 4 passed and 0 failed.
- Protected path diff check: no output.

## Iteration 01 CDC Fix

CDC blocked the first close because `parse_int` accumulated decimal digits into
an `int` without a range check. An oversized numeric field was rejected only
after signed integer overflow, so malformed machine text did not cleanly map to
`input_invalid`.

Repair:

- replaced handwritten multiply/add parsing with digit validation plus
  `std::from_chars`;
- kept the parser standard-library-only and deterministic;
- preserved public status I/O semantics.

New regression coverage in `status_io_smoke.cpp` proves:

- oversized `exit_code` rejects as `input_invalid`;
- oversized `signal_number` on a `signal_terminated` record rejects as
  `input_invalid`;
- non-digit numeric values still reject;
- valid `signal_terminated` still requires a positive signal number and maps
  signal 15 to exit code 143 through `ProcessStatus`.

Iteration verification:

```text
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
printf ... | c++ -std=c++17 -fsanitize=signed-integer-overflow ... && /private/tmp/status_io_overflow_probe
```

Results:

- runtime build passed on `macos-arm64`; CTest reported 3/3 tests passed;
- parser, grounder, and engine builds passed on `macos-arm64`;
- negative smoke passed, 4 passed and 0 failed;
- whitespace, protected path, ledger row count, no-open-row, and no-CDC-file
  checks passed;
- sanitizer probe exited 0, printed `rejected`, and produced no UBSan runtime
  error.

## Ledger Walk

| Row | Result | Evidence |
|-----|--------|----------|
| F-1 | done | Slice open set retained diagnostics/status I/O scope, `PANDAPI_STATUS`, stdout/stderr, flushing, partial-output, no-edit, fmt, Core Guidelines, and no-behavior-change terms. |
| F-2 | done | `pandapi-runtime/README.md` now documents Diagnostics/status I/O and no longer says the helpers do not exist. |
| F-3 | done | `status_io.hpp` defines `StatusRecord`, `StatusStream`, `PartialOutputPolicy`, and uses Slice02 `ProcessStatus`/`StatusResult`. |
| F-4 | done | `status_stream_allowed` encodes stderr-always and stdout-only-when-empty-or-tagged-status ownership. |
| F-5 | done | Writer emits `PANDAPI_STATUS` with required `status`, `component`, `surface`, `surface_disposition`, `exit_code`, and `class` fields. |
| F-6 | done | Parser accepts tagged records and returns `StatusResult<StatusRecord>`; invalid machine text maps to `input_invalid`. Iteration01 makes oversized numeric fields reject without signed overflow. |
| F-7 | done | Seam test round-trips `ok` and `domain_no_plan`. |
| F-8 | done | Values escape newline and tab, reject ANSI escape and placeholder prose, and keep emitted records single-line. |
| F-9 | done | `write_status_record` flushes the selected stream; seam test verifies flush count. |
| F-10 | done | `PartialOutputPolicy` covers absent, retained, discarded, complete, and unknown. |
| F-11 | done | Runtime status I/O remains standard-library-only with no forbidden dependency imports or exposure. |
| F-12 | done | Public headers avoid global `using namespace`, mutable namespace-scope state, object definitions, raw ownership transfer, and explicit allocation/deallocation. |
| F-13 | done | Non-failing helpers/accessors use `noexcept`; tests exercise the public `status_io.hpp` header. |
| F-14 | done | CMake builds `src/status_io.cpp` and registers `pandapi_status_io_smoke`. |
| F-15 | done | Runtime build/test gate passed, 3/3 tests, including iteration01 oversized numeric field regressions. |
| F-16 | done | Parser, grounder, and engine builds passed after the runtime-only change and were rerun after iteration01. |
| F-17 | done | Negative smoke behavior still passed, 4/4, after iteration01. |
| F-18 | done | Protected path diff check produced no output. |
| F-19 | done | This closing report contains the required sections. |
| F-20 | done | CC did not create `cdc-verification.md`. |

## Silent-Drop Check

No prompt requirement was dropped:

- status record value type: delivered as `StatusRecord`;
- status stream selection helpers: delivered as `StatusStream`,
  `status_stream_name`, and `status_stream_allowed`;
- tagged writer/parser: delivered as `serialize_status_record`,
  `write_status_record`, and `parse_status_record`;
- single-line value handling: delivered through escaping/rejection helpers and
  seam tests for newline, tab, ANSI escape, malformed input, and placeholder
  prose;
- final-status flushing: delivered and tested;
- partial-output vocabulary: delivered and tested;
- README/CMake updates: delivered;
- no-binary-adoption boundary: preserved;
- no CDC verification file: preserved.

## Bubble-up to Arc04

Arc04 can proceed to slice04 without changing the slice04 plan. The new inputs
available to later slices are:

- typed stdout/stderr status-stream legality helpers;
- final `PANDAPI_STATUS` serialization/parsing;
- single-line field escaping/rejection;
- final-status flush behavior;
- partial-output policy vocabulary.

Slice04 should build CLI/TTY/provenance policy on these helpers, but it should
not reinterpret their stream legality rules. Arc05 remains the owner of
per-binary adoption and any user-visible `--status` wiring.
