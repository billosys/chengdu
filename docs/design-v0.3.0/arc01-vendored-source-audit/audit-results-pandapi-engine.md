# pandaPIengine Source Audit

## 1. Executive Summary

Date: 2026-08-09

`pandaPI/pandaPIengine` builds successfully from the vendored tree through the
Chengdu build wrapper, and the 0.2.0 exit-status absorption has fixed the
specific "proven unsolvable exits success" defect for the default progression
path. It is still not yet a high-quality managed-process CLI. The main risks
are process-contract defects: stdout carries metadata, progress, diagnostics,
statistics, status, and plan output; timeouts are not assigned a distinct exit
class; many non-search failures collapse to exit 1; ANSI escapes are emitted
without a TTY/color policy; and several non-`main` paths still terminate the
process directly.

The source also has inherited C++ maintainability risks. Model/search state is
mostly raw pointer owned, important input/model validations rely on
release-disabled `assert()`, headers export `using namespace std`, CMake
generates files into the source directory of the disposable build copy, and
there is no engine-local test target or fixture oracle for status/stream
contracts. The translation and optional SAT/BDD paths widen the managed-process
surface with shell execution, fixed output filenames, process-level signals,
and direct `exit()` calls.

No engine source, scripts, workflows, README, release assets, or tracked build
outputs were changed by this audit.

## 2. Audit Context

Project root: `/Users/oubiwann/lab/billosys/chengdu`

Branch: `release/0.3.x`

Audited source state: `0c22e86c`

Audited paths:

- `pandaPI/pandaPIengine/src`
- `pandaPI/pandaPIengine/src/options.ggo`
- `pandaPI/pandaPIengine/src/CMakeLists.txt`
- `pandaPI/pandaPIengine/problemSolver.sh`
- `pandaPI/pandaPIengine/README.md`
- `scripts/build-engine.sh`
- `scripts/lib-platform.sh`
- `scripts/smoke-test.sh`
- root `fixtures/`

C++ knowledge-pack guides loaded:

- `/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/SKILL.md`
- `guides/11-anti-patterns.md`
- `guides/02-api-design.md`
- `guides/03-functions.md`
- `guides/05-resource-management.md`
- `guides/06-error-handling.md`
- `guides/08-concurrency.md`
- `guides/09-performance.md`
- `guides/10-expressions-and-statements.md`
- `guides/12-project-structure-and-tooling.md`
- `guides/13-standard-library.md`
- `guides/14-c-style-and-modernization.md`
- Supporting local `cpp-guidelines` skill for rule IDs including `I.2`,
  `I.11`, `I.30`, `R.1`, `R.5`, `R.11`, `R.20`, `E.4`, `E.6`, `ES.20`,
  `ES.22`, `ES.65`, `SF.7`, `SL.con.3`, and `SL.io.50`.

Commands run:

- `git status --short --branch` - exit 0; reported `## release/0.3.x`.
- `rg --files pandaPI/pandaPIengine` - exit 0; inventoried engine source,
  CMake inputs, bundled CUDD source, docs, and wrapper script.
- `find pandaPI/pandaPIengine -maxdepth 3 -type d | sort` - exit 0; top-level
  engine directories are `src`, `heuristics`, `intDataStructures`, `sat`,
  `search`, `symbolic_search`, and `translation`.
- `find pandaPI/pandaPIengine -maxdepth 3 -type f ... | sort` - exit 0;
  inventoried 100 shallow engine files before the bundled CUDD subtree expands
  the full file set.
- `find pandaPI/pandaPIengine -maxdepth 3 -type d \( -iname '*test*' -o
  -iname '*fixture*' \) -print | sort` - exit 0; no engine-local test/fixture
  directories found.
- `./scripts/build-engine.sh` - exit 0; produced
  `dist/macos-arm64/pandaPIengine`; warnings included repeated `-lpthread`
  compile-option misuse, `Model` class/struct mismatches, unused fields/vars,
  integer-to-pointer cast, and missing `override`.
- CLI probe script using `dist/macos-arm64/pandaPIengine` - exit 0 for the
  probe wrapper; observed `--help` status 0 with 132 stdout lines and 0 stderr
  lines; missing file status 1 with 3 stdout lines (`Random seed`, `Time
  limit`, `Reading input`) and 1 stderr line (`Unable to open input file ...`).
- `./scripts/smoke-test.sh --negative` - exit 0; reported 4 passed, 0 failed,
  including unsolvable engine exit 2 with `Status: Proven unsolvable`.
- `git diff --name-only -- pandaPI scripts .github README.md
  release-manifest.txt vendor.env pins.env` - exit 0; no output.

Inventory:

- Engine public CLI options are generated from
  `pandaPI/pandaPIengine/src/options.ggo`.
- The main executable is built from `pandaPI/pandaPIengine/src/CMakeLists.txt`;
  CMake generates `cmdline.c` and `cmdline.h` in the source directory of the
  disposable build copy.
- Default build disables SAT, BDD, and ILP/CPLEX support unless CMake options
  or `CPLEX_SOURCE_DIR` are supplied.
- Chengdu builds through `scripts/build-engine.sh`, which copies vendored
  source to `build/<platform>/source/pandaPIengine` before CMake runs.
- Root smoke fixtures exist under `fixtures/`, but the engine tree itself has
  no test/fixture directory or engine-local test target.

## 3. Findings

### Correctness and Runtime Safety

#### E-001: Model parsing trusts malformed input and release-disabled asserts

Severity: High

Location:

- `pandaPI/pandaPIengine/src/Model.cpp:1546`
- `pandaPI/pandaPIengine/src/Model.cpp:1548`
- `pandaPI/pandaPIengine/src/Model.cpp:1568`
- `pandaPI/pandaPIengine/src/Model.cpp:1570`
- `pandaPI/pandaPIengine/src/Model.cpp:1572`
- `pandaPI/pandaPIengine/src/Model.cpp:1658`
- `pandaPI/pandaPIengine/src/Model.cpp:1661`
- `pandaPI/pandaPIengine/src/Model.cpp:1664`
- `pandaPI/pandaPIengine/src/Model.cpp:1766`
- `pandaPI/pandaPIengine/src/Model.cpp:1769`
- `pandaPI/pandaPIengine/src/Model.cpp:1841`
- `pandaPI/pandaPIengine/src/Model.cpp:1873`

What's wrong:

The SAS reader uses `getline()` plus stream extraction into counts and indexes
without checking extraction success, then relies on `assert()` for bounds and
model-shape checks. The default Chengdu engine build is Release, so these
assertions are compiled out.

Why it's wrong:

Malformed or truncated input can become default-initialized counts, unchecked
array indexes, or later undefined behavior instead of a classified parse/input
error. A managed process needs input failure to be bounded and status-coded.

C++ Core Guidelines rule ID:

- `E.4`: design error handling around invariants.
- `E.6`: use RAII to prevent leaks around failures.
- `SL.con.3`: avoid bounds errors.
- Chengdu-specific managed-process finding: malformed input must map to a
  documented input/parse status.

Fix / recommendation:

Introduce a checked SAS reader boundary that validates each section header,
count, index, and full-token consumption before allocation or indexing. Route
reader failures to a single engine result type and keep `assert()` only for
debug-only programmer invariants.

#### E-002: Input/model errors can terminate from deep helpers

Severity: High

Location:

- `pandaPI/pandaPIengine/src/SearchEngine.cpp:90`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:91`
- `pandaPI/pandaPIengine/src/Model.cpp:1788`
- `pandaPI/pandaPIengine/src/Model.cpp:1789`
- `pandaPI/pandaPIengine/src/Model.cpp:1829`
- `pandaPI/pandaPIengine/src/Model.cpp:1830`
- `pandaPI/pandaPIengine/src/Model.cpp:1866`
- `pandaPI/pandaPIengine/src/Model.cpp:1867`
- `pandaPI/pandaPIengine/src/Model.cpp:1880`
- `pandaPI/pandaPIengine/src/Model.cpp:1883`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:742`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:744`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:903`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:907`
- `pandaPI/pandaPIengine/src/symbolic_search/automaton.cpp:1730`
- `pandaPI/pandaPIengine/src/symbolic_search/automaton.cpp:1731`
- `pandaPI/pandaPIengine/src/interactivePlanner.cpp:24`

What's wrong:

Several non-`main` helpers call `exit()`, including `exit(1)`, `exit(-1)`,
`exit(0)`, and `exit(2)`. Some are default path reachable, such as heuristic
argument parsing and model read failures; others are optional planner paths.

Why it's wrong:

Deep process termination bypasses cleanup, collapses error ownership, and makes
the final process contract depend on where an error happened. `exit(-1)` also
normalizes to shell status 255, which is not a deliberate taxonomy.

C++ Core Guidelines rule ID:

- `E.4`: design error handling around invariants.
- `I.30`: encapsulate rule violations behind a narrow interface.
- Chengdu-specific managed-process finding: only `main` should map classified
  outcomes to process exit codes.

Fix / recommendation:

Replace deep exits with return-status or exception boundaries appropriate to
the current no-exception code style. Let `main` own status mapping for usage,
input/parse, unsupported option/build, timeout, unsolvable, solved, and
internal error.

### Error Handling and Exit Behavior

#### E-003: Exit taxonomy remains incomplete after the unsolvable-status fix

Severity: High

Location:

- `pandaPI/pandaPIengine/src/SearchEngine.cpp:172`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:196`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:197`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:213`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:214`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:404`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:405`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:408`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:409`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:247`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:284`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:301`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:302`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:304`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:498`

What's wrong:

The default progression path now returns `0` for solved and `2` for timeout or
proven unsolvable, while command-line parse errors, too many input files,
missing input files, unsupported compiled-out planner choices, and unknown
heuristics all return `1`. Timeout and unsolvable share `2`.

Why it's wrong:

The 0.3.0 managed-process target requires callers to classify solved,
unsolvable, input/parse error, resource/timeout failure, and internal error
without scraping human text. Current exit codes still require `Status:` parsing
for timeout versus unsolvable and collapse many failure causes into generic
exit 1.

C++ Core Guidelines rule ID:

- Chengdu-specific managed-process finding: no direct Core Guidelines rule
  defines this process taxonomy.

Fix / recommendation:

Define an engine status enum and documented exit mapping. Keep the 0.2.0
unsolvable behavior as an input, but split timeout/resource from unsolvable and
split usage/configuration from input parse and internal defects.

### Stdio, TTY, ANSI, Buffering, and Pipe Behavior

#### E-004: stdout mixes diagnostics, progress, statistics, status, and plan data

Severity: High

Location:

- `pandaPI/pandaPIengine/src/SearchEngine.cpp:182`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:186`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:205`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:208`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:242`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:279`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:282`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:424`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:232`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:259`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:291`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:294`
- `pandaPI/pandaPIengine/src/Model.cpp:2175`
- `pandaPI/pandaPIengine/src/Model.cpp:2187`
- `pandaPI/pandaPIengine/README.md:42`

What's wrong:

Human metadata, progress lines, search statistics, status lines, and the plan
all use stdout. The missing-file probe showed three stdout lines before the
stderr diagnostic, and the README documents piping the whole stdout stream to
`plan.original`.

Why it's wrong:

For a supervised process, stdout should be the data stream or a documented
machine-event stream, not a mixed human transcript. A caller cannot safely
capture the plan without also filtering metadata, status, and progress.

C++ Core Guidelines rule ID:

- Chengdu-specific managed-process finding: no direct Core Guidelines rule
  applies.
- `SL.io.50` is relevant to incidental flushing from `endl`, but the main
  defect is stream ownership rather than iostream style.

Fix / recommendation:

Move diagnostics/progress/statistics to stderr by default. Reserve stdout for
plan data in plain CLI mode or define a machine-readable event mode where every
line is typed and versioned. Add stream assertions to smoke tests.

#### E-005: ANSI color has no disable flag or TTY policy

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/Util.h:24`
- `pandaPI/pandaPIengine/src/Util.h:27`
- `pandaPI/pandaPIengine/src/Util.cpp:19`
- `pandaPI/pandaPIengine/src/Util.cpp:22`
- `pandaPI/pandaPIengine/src/Util.cpp:24`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:722`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:747`
- `pandaPI/pandaPIengine/src/symbolic_search/automaton.cpp:1730`
- `pandaPI/pandaPIengine/src/symbolic_search/automaton.cpp:1736`

What's wrong:

The `color()` helper always wraps text in SGR escapes and `options.ggo` exposes
no `--no-colour`/`--no-color` or TTY-aware policy.

Why it's wrong:

Supervisors, log collectors, and machine parsers need plain diagnostics by
default or a reliable way to disable control sequences. Parser already has a
similar color-policy defect; engine should not grow a separate one-off fix.

C++ Core Guidelines rule ID:

- `I.30`: encapsulate special behavior.
- Chengdu-specific managed-process finding: ANSI/TTY behavior is part of the
  process contract.

Fix / recommendation:

Introduce one shared color/diagnostic policy across parser, grounder, and
engine: default color only for TTY human mode, explicit `--color=auto|always|never`
or compatible naming, and forced no-color in machine mode.

### CLI and API Surface

#### E-006: Public CLI identity is still upstream `pandaPIengine` version `0.1`

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/options.ggo:1`
- `pandaPI/pandaPIengine/src/options.ggo:2`
- `pandaPI/pandaPIengine/src/options.ggo:4`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:19`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:78`
- `scripts/build-engine.sh:24`
- `scripts/build-engine.sh:30`
- `pandaPI/pandaPIengine/README.md:27`

What's wrong:

The generated CLI package is `pandaPIengine` version `0.1`, usage is
`./pandaPIengine`, and the Chengdu wrapper publishes `dist/<platform>/pandaPIengine`.
There is no Chengdu release/provenance/version surface in the binary help.

Why it's wrong:

0.3.0 is explicitly considering `pandapi-*` namespacing and managed-process
contracts. The current binary identity cannot communicate command namespace,
Chengdu release, upstream lineage, or status/event contracts to humans or
supervisors.

C++ Core Guidelines rule ID:

- Chengdu-specific CLI/API finding: no direct Core Guidelines rule applies.

Fix / recommendation:

In Arc02, decide whether the binary itself becomes `pandapi-engine` or the
release ships compatibility wrappers. Add `--version` output with Chengdu
version, commit, upstream import SHA, and process-contract version.

#### E-007: Interactive mode is reachable from the public CLI

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/options.ggo:29`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:271`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:275`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:277`
- `pandaPI/pandaPIengine/src/interactivePlanner.cpp:18`
- `pandaPI/pandaPIengine/src/interactivePlanner.cpp:22`
- `pandaPI/pandaPIengine/src/interactivePlanner.cpp:24`
- `pandaPI/pandaPIengine/src/interactivePlanner.cpp:37`

What's wrong:

`--interactive` selects an interactive planner that prompts on stdout, reads
from `cin`, and can call `exit(0)` from inside the loop.

Why it's wrong:

Interactive behavior is valid for a human CLI mode only if it is explicit and
well isolated. For supervised execution, it is a deadlock risk and bypasses the
normal status path.

C++ Core Guidelines rule ID:

- Chengdu-specific managed-process finding: no direct Core Guidelines rule
  applies.

Fix / recommendation:

Keep interactive mode only as an explicit TTY-required mode. Refuse it under
machine/supervised mode or when stdin is not a TTY, and route termination
through `main`.

### Resource, Timeout, Signal, and Lifecycle Behavior

#### E-008: Timeout and signal behavior is split between internal polling and an unsafe wrapper

Severity: High

Location:

- `pandaPI/pandaPIengine/src/options.ggo:17`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:185`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:452`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:43`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:226`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:247`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:249`
- `pandaPI/pandaPIengine/problemSolver.sh:12`
- `pandaPI/pandaPIengine/problemSolver.sh:14`
- `pandaPI/pandaPIengine/problemSolver.sh:16`
- `pandaPI/pandaPIengine/problemSolver.sh:19`
- `pandaPI/pandaPIengine/problemSolver.sh:57`

What's wrong:

The engine's default timeout is enforced only when the search loop reaches its
periodic `CHECKAFTER` block. The wrapper script has a separate two-minute alarm
that calls `kill 0`, which kills the whole process group. There is no
documented SIGTERM/SIGINT handling path for flushing status or classifying
supervisor termination.

Why it's wrong:

Managed processes need resource outcomes to be classified by the binary itself
where possible. Polling can overshoot under expensive node expansion, and
process-group kill is too coarse for a supervised tree.

C++ Core Guidelines rule ID:

- Chengdu-specific managed-process finding: no direct Core Guidelines rule
  applies.

Fix / recommendation:

Define engine-owned timeout/resource status distinct from unsolvable. Add
signal handling for graceful termination and flush/status emission. Retire
`problemSolver.sh` as a process-contract reference or mark it as a legacy demo.

#### E-009: Optional SAT path uses process-level signals, threads, and direct exits

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:862`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:864`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:871`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:873`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:903`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:907`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:945`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:958`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:973`
- `pandaPI/pandaPIengine/src/sat/sat_planner.cpp:1014`

What's wrong:

The optional SAT time-interleaving code uses global state, numeric real-time
signals derived from `signalBase = 40`, `pthread_kill`, and direct `exit()` from
worker threads.

Why it's wrong:

Signals are process-global, interact poorly with supervisors, and need careful
ownership. Direct worker-thread process exits bypass the caller and cleanup.
This path is optional today, but it is part of the source that future Arc02/Arc04
contracts must either exclude or govern.

C++ Core Guidelines rule ID:

- `CP.2`: avoid data races.
- `I.2`: avoid non-`const` global variables.
- Chengdu-specific managed-process finding: process signal ownership must be
  explicit.

Fix / recommendation:

Decide whether SAT is in or out of the supported 0.3.0 binary contract. If in,
replace process-level signal scheduling with a controlled cancellation/lifecycle
mechanism and return statuses to `main`.

### Build and Tests

#### E-010: Engine build has no local test target or fixture oracle

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/CMakeLists.txt:58`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:63`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:78`
- `pandaPI/pandaPIengine/README.md:14`
- `pandaPI/pandaPIengine/README.md:20`
- `scripts/smoke-test.sh:194`
- `scripts/smoke-test.sh:248`

What's wrong:

The engine CMake tree defines libraries and the executable but no `enable_testing`,
`add_test`, or engine-local fixture target. Verification lives in Chengdu's
top-level smoke script, which proves some release behavior but is not an
engine-local status/stream oracle.

Why it's wrong:

0.3.0 will change exactly the behavior most likely to regress: exit codes,
stdout/stderr separation, timeout classification, and help/version surfaces.
Those need focused tests, not just successful build plus smoke coverage.

C++ Core Guidelines rule ID:

- Chengdu-specific build/test finding: no direct Core Guidelines rule applies.

Fix / recommendation:

Add engine-local tests or a Chengdu contract-test runner that asserts status,
stdout, stderr, generated files, timeout, signal, `--help`, `--version`, and
malformed input behavior.

#### E-011: Build warnings are visible but non-fatal

Severity: Low

Location:

- `pandaPI/pandaPIengine/src/CMakeLists.txt:25`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:31`
- `pandaPI/pandaPIengine/src/ProgressionNetwork.h:30`
- `pandaPI/pandaPIengine/src/Model.h:31`
- `pandaPI/pandaPIengine/src/VisitedList.cpp:551`
- `pandaPI/pandaPIengine/src/heuristics/rcHeuristics/hhRC2.h:107`

What's wrong:

The build enables warnings but suppresses some categories and does not fail on
warnings. The observed build completed with class/struct tag mismatch warnings,
unused variables/fields, integer-to-pointer cast, `-lpthread` passed as a
compile input, duplicate libraries at link time, and missing `override`.

Why it's wrong:

Not every inherited warning is a release blocker, but these warnings overlap
with portability, ABI, and resource-safety concerns relevant to a maintained
fork.

C++ Core Guidelines rule ID:

- `ES.48`: avoid casts; relevant to the integer-to-pointer payload cast.
- `12-project-structure-and-tooling` guidance: use tool warnings as an
  enforcement surface.

Fix / recommendation:

Burn down warnings in hand-written engine code before process-contract changes.
Keep generated or third-party warning exceptions isolated and documented.

### Maintainability, Duplication, and Shared-Code Candidates

#### E-012: Engine ownership is raw-pointer heavy across model, search, and heuristics

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/SearchEngine.cpp:210`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:231`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:288`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:301`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:336`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:402`
- `pandaPI/pandaPIengine/src/Model.h:52`
- `pandaPI/pandaPIengine/src/Model.h:82`
- `pandaPI/pandaPIengine/src/Model.h:143`
- `pandaPI/pandaPIengine/src/Model.cpp:113`
- `pandaPI/pandaPIengine/src/Model.cpp:235`
- `pandaPI/pandaPIengine/src/ProgressionNetwork.h:80`
- `pandaPI/pandaPIengine/src/ProgressionNetwork.cpp:79`
- `pandaPI/pandaPIengine/src/heuristics/rcHeuristics/hhRC2.h:41`
- `pandaPI/pandaPIengine/src/heuristics/rcHeuristics/hhRC2.h:103`

What's wrong:

The main path allocates input streams, model objects, heuristic arrays, and
heuristic implementations with `new`. `Model` owns many arrays manually.
`hhRC2` allocates `cuts` and `sasH` but its destructor deletes only `factory`.
Several early returns bypass normal cleanup.

Why it's wrong:

Short-lived process leaks may be tolerable for inherited CLI use, but they are
a poor substrate for tests, library extraction, repeated runs, and graceful
supervised termination.

C++ Core Guidelines rule ID:

- `R.1`: manage resources automatically with RAII.
- `R.5`: prefer scoped objects; do not heap-allocate unnecessarily.
- `R.11`: avoid calling `new` and `delete` explicitly.
- `R.20`: use smart pointers to represent ownership.
- `I.11`: never transfer ownership by raw pointer or reference.

Fix / recommendation:

Start at the outer process boundary: scoped `ifstream`, `std::unique_ptr<Model>`,
`std::vector<std::unique_ptr<Heuristic>>`, and explicit ownership of duplicated
heuristics. Later, isolate legacy array storage behind value/RAII containers or
small owner types.

#### E-013: Public headers leak namespace policy and specializations

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/Model.h:26`
- `pandaPI/pandaPIengine/src/ProgressionNetwork.h:24`
- `pandaPI/pandaPIengine/src/ProgressionNetwork.h:151`
- `pandaPI/pandaPIengine/src/ProgressionNetwork.h:153`
- `pandaPI/pandaPIengine/src/ProgressionNetwork.h:155`
- `pandaPI/pandaPIengine/src/VisitedList.h:15`
- `pandaPI/pandaPIengine/src/heuristics/rcHeuristics/hsAddFF-2.h:19`

What's wrong:

Several headers put `using namespace std;` or `using namespace progression;` at
global scope. `ProgressionNetwork.h` also opens `namespace std` for a hash
specialization in the same public header.

Why it's wrong:

Header-level namespace pollution leaks into every includer and makes a future
shared engine library boundary harder to maintain.

C++ Core Guidelines rule ID:

- `SF.7`: do not write `using namespace` at global scope in a header file.

Fix / recommendation:

Remove global using directives from headers, qualify standard-library names,
and keep any required specializations narrowly documented near the owning type.

#### E-014: Process-policy duplication is already visible across parser and engine

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/SearchEngine.cpp:172`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:213`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:404`
- `pandaPI/pandaPIengine/src/SearchEngine.cpp:408`
- `pandaPI/pandaPIengine/src/Util.cpp:19`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:291`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:302`
- `pandaPI/pandaPIengine/src/search/PriorityQueueSearch.h:304`

What's wrong:

Engine has its own ad hoc status mapping, color helper, diagnostics, and status
text. The accepted parser audit found analogous defects in parser status,
stdout/stderr, color, and output-file policy.

Why it's wrong:

The same process concerns are being solved independently in each component.
0.3.0 needs a common process contract, not three parallel local conventions.

C++ Core Guidelines rule ID:

- `I.30`: encapsulate rule violations and special behavior.
- Chengdu-specific managed-process finding: shared process policy should be a
  product-level contract.

Fix / recommendation:

Compare with the grounder audit and extract a small Chengdu runtime/process
facade for exit taxonomy, stream routing, color policy, version/provenance, and
machine-event writing.

### Modernization Candidates

#### E-015: Translation mode shells out with fixed filenames and weak cleanup

Severity: Medium

Location:

- `pandaPI/pandaPIengine/src/options.ggo:66`
- `pandaPI/pandaPIengine/src/options.ggo:67`
- `pandaPI/pandaPIengine/src/options.ggo:70`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:78`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:80`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:90`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:93`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:95`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:111`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:112`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:262`
- `pandaPI/pandaPIengine/src/translation/translationController.cpp:264`

What's wrong:

Translation mode builds a shell command string from user-provided solver/config
values, redirects input with `<`, writes a default `output.sas`, deletes
pre-existing plan files, emits the command to stdout, and has an explicit
comment that cleanup is not correct.

Why it's wrong:

This path has cwd/temp-file assumptions and shell semantics that do not fit a
managed-process contract. It may be acceptable as a legacy experimental mode,
but it cannot be silently included in a high-quality supervised process surface.

C++ Core Guidelines rule ID:

- `R.1`: manage resources automatically.
- Chengdu-specific managed-process finding: cwd/temp and child-process
  ownership must be explicit.

Fix / recommendation:

Either exclude translation mode from the 0.3.0 managed-process contract or
wrap it in explicit output-directory, child-process, and status semantics. Avoid
`system()` for supervised paths.

#### E-016: Optional bundled CUDD/BDD build needs a separate dependency decision

Severity: Low

Location:

- `pandaPI/pandaPIengine/src/CMakeLists.txt:9`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:10`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:40`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:42`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:89`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:91`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:94`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:101`
- `pandaPI/pandaPIengine/src/CMakeLists.txt:102`

What's wrong:

The default build disables BDD, but the source includes a bundled CUDD 3.0.0
tree and an `ExternalProject_add` path that runs `autoreconf`, `configure`, and
`make` when enabled.

Why it's wrong:

This is not a current release blocker because the default shipped build does
not link CUDD. It is a future dependency and build-matrix risk if symbolic
search becomes part of the supported product surface.

C++ Core Guidelines rule ID:

- Chengdu-specific dependency/build finding: no direct Core Guidelines rule
  applies.

Fix / recommendation:

For 0.3.0, explicitly decide whether BDD/symbolic search is supported,
unsupported, hidden, or split. If supported, audit CUDD license/build/security
and prove both Linux and macOS builds in CI.

## 4. Cross-Codebase Synthesis Inputs

Compare with parser:

- Parser and engine both mix diagnostics/progress/status with machine-relevant
  stdout output.
- Parser and engine both have ad hoc color helpers without a complete process
  policy.
- Parser and engine both call `exit()` below `main`.
- Parser and engine both rely on raw ownership and release-disabled assertions
  around input/model invariants.
- Parser and engine both retain upstream CLI names and version strings.

Compare with grounder:

- Does grounder also collapse timeout/resource and unsolvable statuses, or does
  it expose distinct classification?
- Does grounder have cwd/temp filename assumptions like engine translation mode
  and `problemSolver.sh`?
- Does grounder route child-process/dependency failures into structured status,
  or only human logs?
- Does grounder have its own color/diagnostic helper that should be unified
  with parser and engine?
- Does grounder use header-level namespace pollution and raw ownership at the
  same density as engine?

Shared/library candidates:

- A Chengdu process-status enum and exit-code mapper shared by all three tools.
- A stream router for stdout data, stderr diagnostics, progress verbosity, and
  optional machine-readable event mode.
- A color/TTY policy helper with `auto|always|never` behavior and no-color
  machine mode.
- A version/provenance helper exposing Chengdu version, commit, upstream import
  SHA, and process-contract version.
- RAII wrappers for optional input/output files and temporary workspace paths.
- A contract-test harness that asserts exit status, stdout, stderr, generated
  files, timeout/signal behavior, and help/version text across parser, grounder,
  and engine.

Third-party/library candidates:

- CLI parsing can remain gengetopt during the first contract pass if wrapper
  behavior is enough; a later migration to CLI11 or `cxxopts` should be justified
  only if gengetopt blocks version/subcommand/mode design.
- `fmt` or C++20 `std::format` could simplify diagnostics, but it should follow
  the stream/status design rather than lead it.
- `std::filesystem` is a likely fit for output/temp/path handling if the
  supported compiler floor remains C++17.
- A process-spawn library is relevant only if translation mode remains in scope;
  otherwise avoid adding one just for legacy shell-out support.
- CUDD remains an optional bundled dependency risk, not a default shared-runtime
  candidate.

## 5. Things I Looked For and Did Not Find

Clean checks:

1. I did not find tracked source mutation from the current build wrapper:
   `scripts/build-engine.sh:12` calls `prepare_build_source_copy`, and
   `scripts/lib-platform.sh:98` through `scripts/lib-platform.sh:101` refresh
   an ignored build copy before CMake generates `cmdline.c`/`cmdline.h`.
2. I did not find a missing central CLI specification:
   `pandaPI/pandaPIengine/src/options.ggo` contains the public option surface
   in one auditable file.
3. I did not find a current regression of the 0.2.0 unsolvable-status fix:
   `./scripts/smoke-test.sh --negative` exited 0 and reported unsolvable
   engine exit 2 with `Status: Proven unsolvable`.
4. I did not find default shipped CUDD/BDD linkage in the normal build:
   `pandaPI/pandaPIengine/src/CMakeLists.txt:10` defines `BDD` default `OFF`,
   and lines 89 through 103 only build/link CUDD under `if (BDD)`.
5. I did not find default shipped SAT linkage in the normal build:
   `pandaPI/pandaPIengine/src/CMakeLists.txt:9` defines `SAT` default `OFF`,
   and lines 110 through 114 only add/link `sat` under `if (SAT)`.
6. I did not find missing vendored provenance markers: `pandaPI/pandaPIengine`
   contains `LICENSE` and `NOTICE`, and `scripts/build-engine.sh:33` appends
   engine provenance to the platform `dist` directory.
7. I did not find a failed current macOS engine build:
   `./scripts/build-engine.sh` exited 0 and produced
   `dist/macos-arm64/pandaPIengine`.
8. I did not find tracked planner/source/script/release edits from this audit:
   `git diff --name-only -- pandaPI scripts .github README.md
   release-manifest.txt vendor.env pins.env` produced no output.
