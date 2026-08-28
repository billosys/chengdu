# Arc06 Slice04: static-analysis-gate

## Goal

Add or graduate C++ static-analysis gates for owned runtime/process-policy code
where compile databases are reliable, with Make-backed entrypoints and a
written ownership/suppression policy.

This slice starts from Slice03's runtime coverage build evidence and the normal
runtime `compile_commands.json`. Its purpose is to turn C++ static analysis
from a future intention into a concrete, reproducible local gate without
pretending inherited planner, generated, or third-party source has the same
ownership profile as chengdu-owned runtime code.

## In Scope

- Add Make target(s) for C++ static analysis, listed in `make help`.
- Prefer `clang-tidy` for the first gate over owned runtime source and tests
  under `pandaPI/runtime/`, using a reliable runtime compile database.
- Add Clang Static Analyzer support only if it can run reproducibly in this
  slice without duplicating large inherited build volume or creating a
  platform-only illusion.
- Define which paths are included, excluded, or deferred:
  `pandaPI/runtime/` is the required first scope; `pandapi_*_native.cpp`
  adoption seams may be included only if their compile commands are reliable;
  inherited planner, generated, and nested third-party code must be excluded or
  separately classified.
- Document tool resolution, missing-tool behavior, suppression policy, and
  warning ownership.
- Keep new CI behavior Make-backed. If a workflow is changed, it must call the
  Make target rather than `clang-tidy` or analyzer commands directly.

## Out of Scope

- No compiler-warning burndown; that is Slice05.
- No sanitizer, LSan, TSan, release package, README migration prose, Arc07
  tutorial work, or Arc08 publication work.
- No product behavior changes in parser, grounder, or engine.
- No blanket inherited-source cleanup, bulk formatting, or third-party
  suppression churn.
- No hard "zero findings forever" promise without an explicit owned-code
  baseline and suppression policy.

## Verification Approach

- `make help` must expose the C++ static-analysis entrypoint(s).
- The gate must either pass locally or fail/skip explicitly with a documented
  missing-tool/platform reason and re-entry condition.
- Path policy must be grep-verifiable and must distinguish owned runtime code
  from inherited/generated/third-party code.
- If suppressions are added, they must be narrow, justified, and documented.
- Existing Make quality and behavior gates must still pass.

## Active Static-Analysis Scope

`make static-analysis-cpp` is the active public C++ static-analysis entrypoint.
It configures a Debug runtime CMake build with
`CMAKE_EXPORT_COMPILE_COMMANDS=ON` under
`build/static-analysis/runtime/<platform>/build/`, reads that
`compile_commands.json`, and runs `clang-tidy` over the runtime source and test
translation units that are present in the compile database.

The first blocking check family is `clang-analyzer-*`, invoked through
`clang-tidy` with a header filter for `pandaPI/runtime/include`,
`pandaPI/runtime/src`, and `pandaPI/runtime/tests`. This gives a reproducible
Clang Static Analyzer signal for owned runtime/process-policy code without
running a separate `scan-build` pass over duplicate build volume. Standalone
`scan-build` remains deferred; Re-entry: add a separate analyzer target only if
Slice05 warning ownership or Slice06 sanitizer triage needs a path-sensitive
report distinct from the `clang-tidy` analyzer checks.

Tool resolution is explicit. The target uses `CLANG_TIDY` if set, then PATH,
then `xcrun`, then common Homebrew LLVM locations. If `clang-tidy` is still
unavailable, the target prints a `SKIP` line naming the missing tool and the
re-entry condition; silent success is not acceptable.

Included paths:

- `pandaPI/runtime/src/*.cpp`
- `pandaPI/runtime/tests/*.cpp` when each file appears in the compile database
- headers under `pandaPI/runtime/include/` reached through those translation
  units

Excluded paths:

- inherited planner source under `pandaPI/parser`, `pandaPI/grounder`, and
  `pandaPI/engine`
- generated parser/lexer output
- nested third-party source such as cpddl, CUDD, bliss, and lpsolve
- generated `build`, `dist`, release, upstream, and workbench output

Deferred paths:

- adoption seams `pandapi_parser_native.cpp`,
  `pandapi_grounder_native.cpp`, and `pandapi_engine_native.cpp`

Those adoption seams are deferred because their current compile commands are
produced through component-specific inherited build trees and copied source
directories. Re-entry: include them after canonical `pandapi-*` build
instrumentation can expose reliable compile commands for the owned native
files while inherited/generated and third-party paths remain excluded or
separately classified.

No suppressions or `NOLINT` annotations are added in this slice. Future
suppressions must be narrow, colocated with the finding where practical, name
the check being suppressed, and carry a short rationale. Broad unowned
suppressions for inherited/generated/third-party code are not part of this
owned runtime gate.

## Exit Criteria

- A contributor can discover and run the static-analysis target from Make.
- Owned runtime C++ source is analyzed through a reliable compile database or a
  missing-tool/platform deferral is explicitly recorded.
- Any adoption-seam inclusion or deferral is documented with a re-entry
  condition.
- CI wiring, if added, calls Make targets only.
- Slice ledger rows close with evidence, and CDC can reproduce the target or
  the explicit deferral plus the existing regression gates.

## Bubble-up Expectation

The closing report must say whether the static-analysis signal is strong enough
to inform Slice05 compiler-warning burndown and Slice06 sanitizer ownership. It
must also route any analyzer/toolchain gaps so Arc06 can close without
overclaiming static-analysis coverage.
