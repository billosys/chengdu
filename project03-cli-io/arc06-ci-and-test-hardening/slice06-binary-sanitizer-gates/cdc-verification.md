# CDC Verification - Arc06 Slice06 - binary-sanitizer-gates

Verifier: CDC
Verified on: 2026-08-12
Implementation commit reviewed: `95f98412`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc06 Slice06 is CDC-verified.

The slice delivered Make-backed ASan/UBSan binary sanitizer gates for the
canonical `pandapi-*` binaries. The gate builds an isolated sanitizer
distribution under `build/binary-sanitize/<platform>/dist`, points the contract
fixture runner at that distribution with `CHENGDU_CONTRACT_DIST_DIR`, and runs
the managed parser, grounder, engine, and pipeline fixture workload.

macOS LSan is correctly documented as a SKIP, not overclaimed. On this
platform the gate runs ASan/UBSan with `detect_leaks=0`; Linux or another
LeakSanitizer-capable toolchain with `detect_leaks=1` remains the release
re-entry path.

## Source and Scope Checks

- `git show --stat --oneline --name-status 95f98412` confirmed the commit
  scope is limited to Arc06 docs, Make sanitizer wiring, the contract-runner
  dist override, and two narrow engine sanitizer fixes.
- `git show --format=fuller --no-patch 95f98412` confirmed both required
  co-author trailers are present.
- `mk/build.mk` adds `sanitize-binaries`; `mk/tests.mk` adds
  `test-binary-sanitize`; `mk/help.mk` lists both targets.
- `tests/contract/run` documents and applies `CHENGDU_CONTRACT_DIST_DIR` so
  isolated gates can avoid mutating or depending on `dist/<platform>`.
- Engine source changes are limited to sanitizer-promoted behavior fixes:
  zero-task model preflight to managed `domain_no_plan`, and a nodes/sec
  divide-by-zero guard.
- No GitHub Actions, release, Arc07, Arc08, or README files were changed.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | `make help | rg -n "sanitize-binaries|test-binary-sanitize|sanitize-runtime|test-runtime-sanitize"` lists the runtime and binary sanitizer targets. |
| F-2 | reproduced | `make test-runtime-sanitize` passed the runtime ASan/UBSan CTest gate: 5 tests, 0 failures. |
| F-3 | reproduced | `make test-binary-sanitize` built sanitizer `pandapi-parser` and ran managed parser fixtures through the isolated sanitizer dist. |
| F-4 | reproduced | `make test-binary-sanitize` built sanitizer `pandapi-grounder` and ran managed grounder fixtures; dependency-internal warnings remain warning-budget items, not sanitizer findings. |
| F-5 | reproduced | `make test-binary-sanitize` built sanitizer `pandapi-engine` and ran managed engine fixtures; the known `VisitedList.cpp` payload-cast warning did not produce a runtime sanitizer finding in this workload. |
| F-6 | reproduced | `make test-binary-sanitize` ran all managed contract fixtures against `build/binary-sanitize/macos-arm64/dist`; summary: 1008 passed, 0 failed. |
| F-7 | reproduced | `mk/config.mk` records ASan/UBSan options and macOS LSan SKIP with Linux/toolchain re-entry. |
| F-8 | reproduced | The closing report triages findings by Slice05 ownership tiers; no blanket sanitizer suppressions were added. |
| F-9 | reproduced | `make safety-checks && make actionlint` passed; no workflow files changed. |
| F-10 | reproduced | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` passed. |
| F-11 | reproduced | Boundary review confirmed no README, release, workflow, Arc07, Arc08, or unsupported optional-surface scope changes. |

## Commands Reproduced

```text
make help | rg -n "sanitize-binaries|test-binary-sanitize|sanitize-runtime|test-runtime-sanitize"
make test-runtime-sanitize
make test-binary-sanitize
find build/binary-sanitize/macos-arm64/dist -maxdepth 1 -type f -perm +111 -print
rg -n "CHENGDU_CONTRACT_DIST_DIR|BINARY_SANITIZER_LSAN_STATUS|BINARY_SANITIZER_ASAN_OPTIONS|sanitize-binaries|test-binary-sanitize" tests/contract/run mk docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates
make safety-checks && make actionlint
make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check
git show --stat --oneline --name-status 95f98412
git show --format=fuller --no-patch 95f98412
```

Observed results:

- Runtime sanitizer: 5/0.
- Binary sanitizer managed fixtures: 1008/0.
- Isolated sanitizer executables:
  `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` under
  `build/binary-sanitize/macos-arm64/dist`.
- Normal runtime CTest during `make test`: 5/0.
- Baseline contract fixtures: 38/0.
- Managed fixtures during `make test`: parser 298/0, grounder 269/0, engine
  312/0, pipeline 129/0.
- Positive smoke: 3/0. Negative smoke: 4/0.
- Provenance check passed for all three normal dist components on
  `macos-arm64`.

## Review Notes

No blocking findings remain.

CDC agrees with the `make check` disposition in the closing report. The slice
prompt required the narrower sanitizer, policy, regression, provenance, and
diff gates, while `make check` overlaps those gates and adds macOS provenance
mutation outside the sanitizer slice's release/publication boundary.

The sanitizer gate surfaced real value: it promoted two engine issues and both
were fixed. The remaining `VisitedList.cpp` payload-cast warning is still a
warning-budget item because the reproduced managed sanitizer workload did not
turn it into a runtime sanitizer finding.

## Bubble-up to Arc06

Slice06 closes Arc06 row A8 for the macOS ASan/UBSan surface. Runtime and
canonical binary sanitizer gates now run through Make, exercise representative
managed parser/grounder/engine/pipeline fixtures, and preserve ownership
triage for generated and nested dependency noise.

Slice07 should open as `tsan-and-ci-synthesis`. It must decide whether TSan has
a representative race-detection workload or should be explicitly deferred, then
compose final Arc06 local/CI evidence and handoff requirements for Arc07
documentation and Arc08 release preparation.
