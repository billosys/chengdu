# CDC Verification: diagnostics-status-io

Date: 2026-08-09
Branch: `release/0.3.x`
Role: CDC
Verified commit: `ece82e0e311ab2e63c922a6b679bb3ae647fdbec`
Initial implementation commit: `182410e3ce5e94e3d6cd84d598a23339e78c56be`

## Verdict

Accepted after Iteration 01.

Arc04 slice03 now has a CDC-verified, standard-library-only
`pandapi::runtime` diagnostics/status I/O facade. The accepted implementation
adds typed status records, stdout/stderr status-stream legality helpers,
single-line `PANDAPI_STATUS` serialization and parsing, final-status flushing,
single-line field escaping/rejection, and partial-output vocabulary without
binary adoption or external dependency import.

The first close was blocked because numeric parsing accumulated decimal digits
into `int` using unchecked multiply/add. CC repaired that defect with digit
validation plus `std::from_chars`, added oversized numeric regressions, and
reran the original gates. CDC reproduced the overflow-focused probe and found
no remaining blocking issue.

## Source and Scope Checks

| Check | Result | Evidence |
|-------|--------|----------|
| Worktree clean before CDC docs | accepted | `git status --short --branch` showed only `## release/0.3.x`. |
| Iteration commit scope | accepted | `git show --stat --oneline --summary ece82e0e...` showed edits to `status_io.cpp`, `status_io_smoke.cpp`, the slice ledger, and CC closing report only. |
| No preexisting CDC verification | accepted | `test ! -e docs/design-v0.3.0/arc04-shared-runtime-substrate/slice03-diagnostics-status-io/cdc-verification.md` passed before this file was created. |
| Protected source/release paths | accepted | `git diff --name-only -- pandaPI .github README.md release-manifest.txt vendor.env pins.env dist build release` produced no output during CDC verification. |

## Independent Verification

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | accepted | Slice open set still documents diagnostics/status I/O scope, Arc03 tagged status requirements, no-binary-adoption boundary, fmt gate, and Core Guidelines pressure. |
| F-2 | accepted | `pandapi-runtime/README.md` documents implemented diagnostics/status I/O and no stale skeleton wording remains. |
| F-3 | accepted | `status_io.hpp` defines `StatusRecord`, `StatusStream`, and `PartialOutputPolicy` and uses `ProcessStatus`/`StatusResult`. |
| F-4 | accepted | Status-stream helpers encode stderr status legality and stdout restrictions against data/informational ownership. |
| F-5 | accepted | Writer emits required `PANDAPI_STATUS` fields: `status`, `component`, `surface`, `surface_disposition`, `exit_code`, and `class`. |
| F-6 | accepted | Parser returns `StatusResult<StatusRecord>` and malformed machine text maps to `input_invalid`; `std::from_chars` is used for numeric fields. |
| F-7 | accepted | Tests round-trip success and non-success statuses. |
| F-8 | accepted | Tests cover newline/tab escaping, ANSI rejection, malformed input, placeholder rejection, and single-line output. |
| F-9 | accepted | `write_status_record` calls `flush()` and the custom stream-buffer test verifies flush count. |
| F-10 | accepted | `PartialOutputPolicy` covers absent, retained, discarded, complete, and unknown. |
| F-11 | accepted | Dependency-gate search found no fmt, CLI11, Catch2, `tl::expected`, reproc++, nlohmann/json, Boost, Abseil, or GSL import/exposure. |
| F-12 | accepted | Public header hazard search found no global `using namespace`, mutable namespace-scope state, raw ownership transfer, or explicit allocation/deallocation patterns. |
| F-13 | accepted | Non-failing helpers include `noexcept`; tests include the public `status_io.hpp` header. |
| F-14 | accepted | CMake builds `src/status_io.cpp` and registers the `pandapi_status_io_smoke` test. |
| F-15 | accepted | `./scripts/build-runtime.sh` passed on macos-arm64 with 3/3 CTest tests. |
| F-16 | accepted | `./scripts/build-parser.sh`, `./scripts/build-grounder.sh`, and `./scripts/build-engine.sh` passed on macos-arm64. |
| F-17 | accepted | `./scripts/smoke-test.sh --negative` passed: 4 passed, 0 failed. |
| F-18 | accepted | Protected-path diff check produced no output. |
| F-19 | accepted | CC closing report contains capability verdict, implementation decisions, verification, ledger walk, silent-drop check, and bubble-up sections. |
| F-20 | accepted | CC did not create `cdc-verification.md`. |

## Overflow Probe

CDC reproduced the Iteration 01 defect-class probe with
`-fsanitize=signed-integer-overflow`:

```text
printf ... | c++ -std=c++17 -fsanitize=signed-integer-overflow \
  -Ipandapi-runtime/include \
  pandapi-runtime/src/status.cpp pandapi-runtime/src/status_io.cpp \
  -x c++ - -o /private/tmp/status_io_overflow_probe &&
  /private/tmp/status_io_overflow_probe
```

Result:

```text
rejected
```

The probe exited 0 and produced no UBSan signed-overflow report. The oversized
`exit_code` record was rejected before numeric overflow. CDC also confirmed
test coverage for oversized `exit_code`, oversized `signal_number`,
non-digit numeric input, and valid `signal_terminated` mapping.

## Code Review Notes

No blocking findings remain.

The prior blocker is resolved: status numeric parsing now validates decimal
input and uses `std::from_chars` for bounded conversion. The parser remains
standard-library-only, deterministic, and behind the local runtime facade.

Arc04 still has the intended no-binary-adoption boundary. This verification
does not assert managed-process conformance at executable boundaries; that
proof remains routed to Arc05 and Arc06.

## Bubble-up to Arc04

Arc04 can proceed to slice04 from these accepted inputs:

- typed stdout/stderr status-stream legality helpers;
- final `PANDAPI_STATUS` serialization and parser behavior;
- escaped/rejected field-value policy for single-line machine status;
- overflow-safe parsing for `exit_code` and `signal_number`;
- final-status stream flush behavior;
- partial-output policy vocabulary.

Slice04 should consume the accepted stream legality helpers rather than
reinterpreting stdout/stderr policy. fmt remains behind its dependency gate,
and CLI11 remains gated by CLI/name/help/version compatibility plus golden
output/process tests. Per-binary `--status` wiring and managed-process
conformance remain Arc05 work.
