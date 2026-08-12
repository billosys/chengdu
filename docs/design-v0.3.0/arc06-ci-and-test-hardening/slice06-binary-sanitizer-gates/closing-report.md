# Arc06 Slice06: binary-sanitizer-gates Closing Report

Status: CC proposed done
Implementation commit: this commit

## Ledger Walk

| ID | Status | Evidence |
|----|--------|----------|
| F-1 | done | `make help` lists `make sanitize-binaries` and `make test-binary-sanitize`; `set -e; make help | rg -n "sanitize|sanitizer|asan|ubsan|lsan" >/dev/null` passed. |
| F-2 | done | `make test-runtime-sanitize` passed the scoped runtime ASan/UBSan CTest gate: 5 tests, 0 failures. |
| F-3 | done | `make test-binary-sanitize` built sanitizer `pandapi-parser` with AppleClang ASan/UBSan and ran managed parser fixtures through the isolated sanitizer dist. |
| F-4 | done | `make test-binary-sanitize` built sanitizer `pandapi-grounder`, including cpddl and nested dependencies with sanitizer flags where their Makefiles accepted them, and ran managed grounder fixtures. |
| F-5 | done | `make test-binary-sanitize` built sanitizer `pandapi-engine` and ran managed engine fixtures. Sanitizer evidence promoted two issues, both fixed: zero-task engine input now returns managed `domain_no_plan` before inherited search, and search diagnostics no longer divide by zero for nodes/sec. `VisitedList.cpp` still emits the known payload-cast warning but did not produce a runtime sanitizer finding in this workload. |
| F-6 | done | `make test-binary-sanitize` ran `tests/contract/run --contract` with `CHENGDU_CONTRACT_DIST_DIR=build/binary-sanitize/macos-arm64/dist`; summary: 1008 passed, 0 failed across parser, grounder, engine, and pipeline managed fixtures. |
| F-7 | done | ASan/UBSan run on macos-arm64 through AppleClang. LSan is `SKIP` on macOS because this gate uses `detect_leaks=0`; Re-entry is Linux or another LeakSanitizer-capable toolchain with `detect_leaks=1`. |
| F-8 | done | Findings and skips were triaged by Slice05 ownership tiers: managed adapter code was fixed, primary engine diagnostic UB was fixed, generated parser and dependency-internal grounder warnings remain warning-budget items, and no blanket sanitizer suppressions were added. |
| F-9 | done | No GitHub Actions files changed. `make safety-checks` and `make actionlint` passed. |
| F-10 | done | `make static-analysis`, `make format-check`, `make test`, `make provenance-check`, `git diff --check`, and `git diff --cached --check` passed. `make check` was not used as the closing gate because the slice prompt requires the narrower reproduced gates and `make check` overlaps them while adding macOS provenance mutation. |
| F-11 | done | Boundary check passed: no README, release, `.github/workflows/release.yml`, Arc07, or Arc08 files were staged. |

## Bubble-up to Arc06

ASan/UBSan evidence is strong enough for Arc08 release preparation as a
pre-release gate over canonical managed binaries. It now exercises runtime,
parser, grounder, engine, and pipeline process fixtures against an isolated
sanitizer distribution rather than only runtime CTest paths.

LSan evidence is not strong enough on macOS. Treat macOS LSan as a documented
SKIP with Linux/toolchain re-entry. Arc08 should not claim leak-clean release
evidence until a Linux or other LSan-capable run passes with `detect_leaks=1`.

Recommended sanitizer placement:

- Always-on CI: keep `make test-runtime-sanitize` if runtime cost stays low.
- Scheduled or pre-release CI: run `make test-binary-sanitize` because it
  rebuilds parser, grounder, engine, and dependencies and is noisier.
- Local pre-merge: run `make test-binary-sanitize` for changes touching
  managed adapters, inherited engine search, or sanitizer/build plumbing.
- Release prep: require runtime sanitizer plus binary sanitizer on macOS, and
  add Linux LSan once Slice07/CI synthesis selects the job shape.

Slice05 warning budget status:

- `VisitedList.cpp` payload cast remains a warning-budget item; sanitizer
  evidence did not promote it in the managed workload.
- Generated Flex helper warnings and grounder dependency-internal warnings
  remain classified warning budget, not sanitizer findings.
- The promoted sanitizer items were separate: adapter-level zero-task no-plan
  handling and engine search diagnostics division by zero.

Slice07 TSan/CI synthesis should decide whether `make test-binary-sanitize`
is scheduled, pre-release-only, or split by component in CI. It should also
decide whether Linux LSan runs as part of the sanitizer job or as a separate
release-prep leg.
