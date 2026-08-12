# Arc06 Slice05 Warning Inventory

Date: 2026-08-11
Platform: `macos-arm64`
Inventory command: `make warning-inventory`
Generated evidence: `build/warnings/macos-arm64/build.log` and
`build/warnings/macos-arm64/warning-inventory.txt`

`make warning-inventory` runs `make build`, captures the complete build log,
and writes an indexed warnings-only report. The generated reports stay under
ignored `build/` output; this file records the durable ownership
classification, warning budget, and release re-entry policy.

## Current Result

The post-burndown inventory passed and reported 56 `warning:` lines.

| Component | Ownership tier | Current warning classes | Disposition |
|-----------|----------------|-------------------------|-------------|
| runtime | chengdu-owned runtime | zero compiler warnings observed in `make warning-inventory` | Done. Runtime remains the zero-warning surface. |
| parser | generated source | `src/hddl-token.cpp` generated Flex helpers: `yyunput` unused and `yyinput` unneeded internal declaration | Budgeted generated-code warning. Re-entry: regenerate or patch scanner generation if parser-generated code becomes a strict warning gate. |
| parser | primary hand-written parser | none remaining from the current inventory | `P-011` primary parser warning debt dispositioned by fixing `hpdlWriter.cpp`, `plan.cpp`, and `verify.cpp` low-risk warnings. |
| grounder | nested third-party/dependency-internal source | cpddl boruvka GNU null-pointer arithmetic, macOS semaphore deprecation, opts strict-prototypes, lpsolve `-O9`, lpsolve parentheses, and RSS strict-prototypes warnings | Budgeted dependency-internal warning set. Re-entry: supported release policy requires warning-clean dependency builds, or cpddl/lpsolve/boruvka/RSS behavior becomes a supported product surface. |
| grounder | dependency-internal H2 source | `../h2-fd-preprocessor/src/h2_mutexes.cc` unused counters | Budgeted dependency-internal H2 warning. Re-entry: H2 becomes a supported default surface or a separate H2 dependency audit is accepted. |
| grounder | primary hand-written grounder | none remaining from the current inventory | `G-015` dispositioned by fixing the H2 string construction bug, no-op primary unused warnings, and FAM error initialization. |
| engine | primary hand-written engine | `VisitedList.cpp` integer-to-pointer payload cast | Budgeted primary engine warning. This overlaps storage representation in the compressed-sequence payload and is not safe to rewrite without a targeted engine visited-list fixture. Re-entry: Slice06 sanitizer evidence flags this path, Arc08 promotes warning-clean engine primary source to a release gate, or a visited-list unit/fixture seam is added. |
| engine | copied build-tree artifact path for primary source | warning report paths point into `build/macos-arm64/source/engine.*`, copied from `pandaPI/engine` for build isolation | Ownership remains engine primary source; copied build paths are not a separate source owner. |
| engine | platform/toolchain-only or build-system noise | duplicate link-library warning for `heuristics/libheuristics.a` and `heuristics/rcHeuristics/libhrc.a` | Budgeted linker warning. Re-entry: link order or duplicate archive inclusion affects supported platforms, CI starts treating linker warnings as release blockers, or Arc08 release dry-run detects duplicate-symbol/package risk. |

## Fixed Warning Classes

Parser primary hand-written fixes:

- `hpdlWriter.cpp`: marked the debug-only parentless-sort invariant as
  intentionally `maybe_unused` in Release builds.
- `plan.cpp`: made existing int/vector-size comparisons explicit at the
  inherited int boundary.
- `verify.cpp`: used `size_t` for vector indexing and `const string &` for
  range iteration.

Grounder primary hand-written fixes:

- `h2mutexes.cpp`: changed `"var" + internal_variables.size()` to string
  construction with `std::to_string`, fixing the Arc01 `G-001` string-pointer
  arithmetic warning surfaced through `G-015`.
- `gpg.h`, `groundedGPG.cpp`, `output.cpp`, and `h2mutexes.cpp`: removed or
  marked no-op debug/dead counters and loop variables without changing emitted
  artifacts.
- `FAMmutexes.cpp`: initialized `bor_err_t` through `borErrInit(&err)`.

Engine primary hand-written fixes:

- `CMakeLists.txt`: removed `-lpthread` from compiler flags while leaving
  linker flags intact.
- `ProgressionNetwork.h`: changed the `Model` forward declaration from
  `struct` to `class`.
- `hhRC2.h`: marked `getDescription()` as `override`.
- Debug-only or intentionally retained fields/counters were marked
  `maybe_unused` where the value is only used by assertions, debug macros, or
  retained private layout.

## Warning Policy

No blanket `-Werror` or global suppression was added. The current long-term
policy is:

- runtime stays zero-warning because it is chengdu-owned and already has
  reliable CMake/CTest gates;
- primary parser and grounder warnings observed in this slice are fixed;
- the remaining parser generated warning is budgeted as generated source;
- grounder cpddl/boruvka/opts/lpsolve/RSS and H2 warnings are
  dependency-internal and require a separate dependency audit or accepted
  support-surface change before they block release;
- engine primary warning debt is narrow but not erased without targeted
  visited-list evidence;
- linker duplicate-library noise remains budgeted until CI/release packaging
  turns it into a supported-platform risk.

Slice06 sanitizer gates may proceed with meaningful signal: runtime is clean,
parser/grounder primary warning debt is burned down, and the remaining
warnings are isolated enough that sanitizer failures can be triaged by
ownership tier rather than hidden in a generic inherited-warning bucket.
