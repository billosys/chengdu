# pandaPIgrounder Source Audit

## 1. Executive Summary

Date: 2026-08-09

`pandaPI/pandaPIgrounder` builds successfully from the vendored tree through the
Chengdu build wrapper, and it can produce a clean output file when invoked with
`--quiet` and an explicit output path. It is not yet a high-quality
managed-process CLI. The main risks are process-contract defects: missing
positional arguments are treated as stdin/stdout execution, progress and
configuration text are written to stdout, several ordinary failures collapse to
status 1, and deep grounding/output helpers call `exit(-1)` or `_exit(0)`
instead of returning through a supervised status classifier.

The source also has inherited C/C++ risks. The grounder parser trusts counts
and indexes from the intermediate `.htn` stream, release builds compile with
`-DNDEBUG`, some validation depends on `assert()`, ownership of file streams in
`main.cpp` is raw, and the H2 mutex path contains a string-literal pointer
arithmetic bug. Integration with `cpddl` and `h2-fd-preprocessor` is
grounder-owned, but dependency internals are not fully audited in this slice.
Where dependency warnings are mentioned, they are boundary evidence, not repair
scope for this report.

No grounder source, dependency source, scripts, workflows, release assets, or
generated build outputs were intentionally changed by this audit.

## 2. Audit Context

Project root: `/Users/oubiwann/lab/billosys/chengdu`

Branch: `release/0.3.x`

Audited paths:

- `pandaPI/pandaPIgrounder/src`
- `pandaPI/pandaPIgrounder/src/options.ggo`
- `pandaPI/pandaPIgrounder/src/Makefile`
- `pandaPI/pandaPIgrounder/build.sh`
- `pandaPI/pandaPIgrounder/README.md`
- `pandaPI/pandaPIgrounder/doc`
- `pandaPI/pandaPIgrounder/cpddl`
- `pandaPI/pandaPIgrounder/h2-fd-preprocessor`
- `scripts/build-grounder.sh`
- `scripts/lib-platform.sh`

Dependency-boundary statement:

`pandaPIgrounder` is the integration owner for the in-tree `cpddl` and
`h2-fd-preprocessor` dependencies. This audit treats direct includes, copied
dependency code, build-system coupling, dependency output suppression, and
release-packaging consequences as grounder-owned integration findings. It does
not claim a complete standalone audit of `cpddl`, `h2-fd-preprocessor`, or
their nested third-party libraries. Dependency-internal warnings are recorded
only where they affect Chengdu re-entry conditions.

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
- Supporting C++ Core Guidelines rule IDs used from the local skill:
  `I.2`, `I.11`, `I.30`, `R.1`, `R.5`, `R.11`, `R.20`, `E.4`, `E.28`,
  `ES.3`, `ES.30`, `ES.65`, `SF.7`, `SL.3`, `SL.con.3`, and `SL.io.2`.

Commands run:

- `rg --files pandaPI/pandaPIgrounder | sort` - exit 0, inventoried primary
  grounder source, `cpddl`, `h2-fd-preprocessor`, nested third-party sources,
  docs, and build files.
- `find pandaPI/pandaPIgrounder/src -maxdepth 1 -type f | sort | wc -l` - exit
  0, reported 47 primary grounder source/header/generated-input files.
- `find pandaPI/pandaPIgrounder -maxdepth 2 \( -iname '*test*' -o -iname '*fixture*' \) -print` - exit 0, found no primary grounder test or fixture
  directory at that depth.
- `./scripts/build-grounder.sh` - exit 0, produced
  `dist/macos-arm64/pandaPIgrounder`; warnings included grounder-owned warnings
  in `rss.c`, `gpg.h`, `groundedGPG.cpp`, `FAMmutexes.cpp`,
  `h2mutexes.cpp`, and `output.cpp`, plus dependency-internal warnings under
  `cpddl/third-party` and `h2-fd-preprocessor`.
- Parser-to-grounder probe setup:
  `dist/macos-arm64/pandaPIparser pandaPI/pandaPIparser/tests/empty-d.hddl pandaPI/pandaPIparser/tests/empty-p.hddl "$HTN"` - parser subprocess status
  0, produced a valid `.htn` input for grounder probes.
- Grounder probe with no positional arguments - subprocess status 1; stdout
  contained the full configuration block, stderr reported stdin parsing and EOF.
- Grounder probe with a missing input file - subprocess status 1; stdout
  contained the full configuration block, stderr reported open failure.
- Grounder probe with malformed `.htn` input and an output file path -
  subprocess status 1; stdout contained the full configuration block, stderr
  reported parse failure and then output-opening messages.
- Grounder probe with valid input and default stdout output - subprocess status
  0; stdout mixed configuration/progress lines with grounded planner output.
- Grounder probe with valid input, `--quiet`, and explicit output file -
  subprocess status 0; stdout and stderr were empty, and the output file
  existed.
- `dist/macos-arm64/pandaPIgrounder --help` - exit 0; usage reports
  `./pandaPIgrounder [OPTIONS] input.htn output.sas`, with concatenated help
  text around output/SAS mode defaults.
- `dist/macos-arm64/pandaPIgrounder --version` - exit 0; output was
  `pandaPIgrounder 0.1`.
- `git status --short --branch` after build/probes - exit 0; no tracked
  grounder source, dependency source, scripts, workflows, or release assets
  were changed by the probes.

Inventory:

- Primary grounder code is under `pandaPI/pandaPIgrounder/src` with 47 files.
- CLI generation is driven by `pandaPI/pandaPIgrounder/src/options.ggo`.
- The primary source build is driven by `pandaPI/pandaPIgrounder/src/Makefile`.
- The upstream-oriented `pandaPI/pandaPIgrounder/build.sh` still clones from
  GitHub and initializes submodules.
- Chengdu's active build entry point is `scripts/build-grounder.sh`, which
  copies vendored source to `build/$PLATFORM/source` before building.
- `cpddl`, `h2-fd-preprocessor`, and nested third-party libraries are vendored
  under `pandaPI/pandaPIgrounder`.
- No primary grounder tests/fixtures were found by the maxdepth-2 inventory
  probe; dependency-internal tests exist deeper under nested dependency trees.

## 3. Findings

### Correctness and Runtime Safety

#### G-001: H2 variable names are built with string-literal pointer arithmetic

Severity: High

Location:

- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:60`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:64`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:65`

What's wrong:

The H2 mutex conversion path assigns `var.name = "var" +
internal_variables.size();`. The build warns that adding `size_type` to a
string literal does not append to the string.

Why it's wrong:

For `size() == 0`, the name is `"var"`; for small values it becomes a pointer
into the literal; for larger values it can point past the literal. That is a
runtime-safety bug in a grounder-owned H2 integration path, not a dependency
internal issue.

C++ Core Guidelines rule ID:

- `ES.65`: do not dereference or use invalid indirections.
- `SL.con.3`: avoid bounds errors.

Fix / recommendation:

Use `std::string("var") + std::to_string(internal_variables.size())`, add a
small H2-mode regression fixture, and promote this warning class to a build
failure for primary grounder code.

#### G-002: Intermediate input parsing trusts counts and indexes from the stream

Severity: High

Location:

- `pandaPI/pandaPIgrounder/src/parser.cpp:32`
- `pandaPI/pandaPIgrounder/src/parser.cpp:34`
- `pandaPI/pandaPIgrounder/src/parser.cpp:56`
- `pandaPI/pandaPIgrounder/src/parser.cpp:58`
- `pandaPI/pandaPIgrounder/src/parser.cpp:80`
- `pandaPI/pandaPIgrounder/src/parser.cpp:82`
- `pandaPI/pandaPIgrounder/src/parser.cpp:105`
- `pandaPI/pandaPIgrounder/src/parser.cpp:109`
- `pandaPI/pandaPIgrounder/src/parser.cpp:117`
- `pandaPI/pandaPIgrounder/src/parser.cpp:119`
- `pandaPI/pandaPIgrounder/src/parser.cpp:135`
- `pandaPI/pandaPIgrounder/src/parser.cpp:137`
- `pandaPI/pandaPIgrounder/src/parser.cpp:239`
- `pandaPI/pandaPIgrounder/src/parser.cpp:339`

What's wrong:

The `.htn` parser reads unbounded `size_t` counts and uses input indexes
directly to index `state.predicates`, `state.functions`, and `state.tasks`.
Some fields are explicitly validated, such as decomposition method task IDs,
but many predicate/function/task references are not. Release builds also
disable the `assert(cost_type == "var")` check.

Why it's wrong:

Malformed or adversarial intermediate input can drive excessive allocation,
out-of-range vector access, or uncaught exceptions. The probe with malformed
input returned status 1 for a simple parse failure, but the source shows other
malformation classes can escape the controlled `std::ifstream::failure` catch.

C++ Core Guidelines rule ID:

- `SL.io.2`: consider ill-formed input.
- `SL.con.3`: avoid bounds errors.
- `E.4`: design error-handling around invariants.

Fix / recommendation:

Centralize checked integer/count/index parsing for the grounder input format,
bound allocation counts, validate every cross-reference before indexing, and
route every parse defect through one typed input-error status.

#### G-003: Release build settings compile out validation asserts

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/Makefile:44`
- `pandaPI/pandaPIgrounder/src/parser.cpp:105`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:73`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:105`
- `pandaPI/pandaPIgrounder/src/postprocessing.cpp:121`
- `pandaPI/pandaPIgrounder/src/postprocessing.cpp:126`

What's wrong:

The production flags include `-DNDEBUG`, while model and input checks still use
`assert()` in code paths reached from parsed input and generated grounding
state.

Why it's wrong:

Assertions are appropriate for impossible programmer invariants, but a managed
toolchain needs deterministic handling for invalid intermediate input and
unsupported model shapes in release builds.

C++ Core Guidelines rule ID:

- `E.4`: design error-handling around invariants.
- `SL.io.2`: consider ill-formed input.

Fix / recommendation:

Classify asserts into debug-only invariants versus runtime validation. Convert
runtime validation to explicit errors that preserve release behavior.

### Error Handling and Status Behavior

#### G-004: Deep helpers terminate the whole process with `exit(-1)` or `_exit(0)`

Severity: High

Location:

- `pandaPI/pandaPIgrounder/src/postprocessing.cpp:52`
- `pandaPI/pandaPIgrounder/src/postprocessing.cpp:54`
- `pandaPI/pandaPIgrounder/src/postprocessing.cpp:164`
- `pandaPI/pandaPIgrounder/src/postprocessing.cpp:166`
- `pandaPI/pandaPIgrounder/src/output.cpp:817`
- `pandaPI/pandaPIgrounder/src/output.cpp:818`
- `pandaPI/pandaPIgrounder/src/output.cpp:824`
- `pandaPI/pandaPIgrounder/src/output.cpp:833`
- `pandaPI/pandaPIgrounder/src/output.cpp:1045`
- `pandaPI/pandaPIgrounder/src/output.cpp:1047`
- `pandaPI/pandaPIgrounder/src/output.cpp:1319`
- `pandaPI/pandaPIgrounder/src/output.cpp:1320`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:245`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:249`

What's wrong:

Unsupported conditional effects and duplicate conditional-effect groundings use
`exit(-1)`. Unreachable goals and H2 failures use `_exit(0)` in several places.
Those exits occur below `main()` and bypass a single status/output policy.

Why it's wrong:

Supervised processes need to distinguish invalid input, unsupported model
features, unsolvable/unreachable instances, dependency failures, internal
defects, and successful output. `_exit(0)` for unreachable states is especially
dangerous because it reports success to callers and can skip normal stream and
object cleanup.

C++ Core Guidelines rule ID:

- `E.4`: design error-handling around invariants.
- `E.28`: avoid error handling based on global state where possible.

Fix / recommendation:

Replace deep process exits with typed results or exceptions caught in `main()`.
Reserve status 0 for successful completion and map unreachable/unsolvable,
invalid input, unsupported feature, and internal error to documented non-zero
codes.

#### G-005: Missing positional arguments are treated as stdin/stdout execution

Severity: High

Location:

- `pandaPI/pandaPIgrounder/src/options.ggo:4`
- `pandaPI/pandaPIgrounder/src/main.cpp:84`
- `pandaPI/pandaPIgrounder/src/main.cpp:88`
- `pandaPI/pandaPIgrounder/src/main.cpp:89`
- `pandaPI/pandaPIgrounder/src/main.cpp:92`
- `pandaPI/pandaPIgrounder/src/main.cpp:96`
- `pandaPI/pandaPIgrounder/src/main.cpp:104`
- `pandaPI/pandaPIgrounder/src/main.cpp:105`
- `pandaPI/pandaPIgrounder/src/main.cpp:132`
- `pandaPI/pandaPIgrounder/src/main.cpp:133`

What's wrong:

The generated help advertises `input.htn output.sas`, but `main.cpp` defaults
missing input and output paths to `"-"`. A no-argument invocation therefore
starts reading from stdin and writing to stdout instead of failing as a usage
error.

Why it's wrong:

Under a non-interactive supervisor, accidental no-argument execution can block
on stdin, emit configuration text on stdout, and return the same status used by
other errors. The observed no-argument probe returned status 1 after EOF, not a
distinct usage classification.

C++ Core Guidelines rule ID:

- Chengdu managed-process rule; no direct C++ Core Guidelines rule.

Fix / recommendation:

Require at least the input path unless `-` is explicitly supplied, emit usage
errors on stderr, and reserve a documented status for CLI usage defects. Decide
separately whether stdout output remains a supported compatibility mode.

#### G-006: Output streams are opened after parse failure and before success is checked

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/main.cpp:128`
- `pandaPI/pandaPIgrounder/src/main.cpp:130`
- `pandaPI/pandaPIgrounder/src/main.cpp:132`
- `pandaPI/pandaPIgrounder/src/main.cpp:145`
- `pandaPI/pandaPIgrounder/src/main.cpp:156`
- `pandaPI/pandaPIgrounder/src/main.cpp:169`
- `pandaPI/pandaPIgrounder/src/main.cpp:173`
- `pandaPI/pandaPIgrounder/src/main.cpp:180`
- `pandaPI/pandaPIgrounder/src/main.cpp:183`

What's wrong:

`main.cpp` calls `readInput()`, stores `success`, opens one or two output
streams, and only then checks `success`. The malformed-input probe showed
output-open messages after the parse failure. The second-output error path also
reports `outputFilename` instead of `outputFilename2`.

Why it's wrong:

Invalid input should not create or truncate output paths. Delayed error checks
also confuse operator logs by reporting output setup after input failure.

C++ Core Guidelines rule ID:

- `E.4`: design error-handling around invariants.
- `R.1`: manage resources automatically.

Fix / recommendation:

Check `success` immediately after `readInput()` and before output setup. Fix
the second-output diagnostic and route all stream-open failures through the
same status classifier.

### Stdio, TTY, Buffering, and Color

#### G-007: Default stdout mixes configuration, progress, and machine output

Severity: High

Location:

- `pandaPI/pandaPIgrounder/src/main.cpp:74`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:13`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:17`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:45`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:75`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:252`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:212`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:256`

What's wrong:

`grounding_configuration::print_options()` writes a configuration block to
stdout before input validation, and grounding/H2 progress messages also write
to stdout. A valid default-output probe showed config/progress interleaved with
the grounded planner output.

Why it's wrong:

For a pipe-supervised tool, stdout must be reserved for the requested data
stream or a documented event stream, not mixed human diagnostics. This breaks
callers that expect to parse stdout as a grounded model.

C++ Core Guidelines rule ID:

- Chengdu managed-process rule; no direct C++ Core Guidelines rule.

Fix / recommendation:

Move human diagnostics/progress to stderr or a structured event channel. Make
`--quiet` unnecessary for producing parseable stdout, and add pipe-mode tests
that assert stdout contains only the selected output format.

#### G-008: Quiet mode suppresses H2 dependency output by globally poisoning `std::cout`

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:216`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:218`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:304`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:306`

What's wrong:

The H2 integration suppresses dependency output by setting `std::cout`'s
`failbit` and later clearing it.

Why it's wrong:

Global stream-state mutation is fragile. Any exception, early return, or
dependency behavior inside that window can leave stdout in a broken state or
hide grounder-owned output unexpectedly. It also does not provide a controlled
flush/buffer policy for pipes.

C++ Core Guidelines rule ID:

- `R.1`: manage resources and state changes with RAII.
- `E.4`: keep error paths explicit.

Fix / recommendation:

Wrap stream redirection/suppression in an RAII guard at minimum. Prefer
dependency integration that accepts an explicit output sink, or isolate H2 as a
separate subprocess with captured stdout/stderr if in-process control is not
available.

#### G-009: Debug/output-domain color uses hard-coded ANSI with no TTY or no-color control

Severity: Low

Location:

- `pandaPI/pandaPIgrounder/src/debug.cpp:8`
- `pandaPI/pandaPIgrounder/src/debug.cpp:11`
- `pandaPI/pandaPIgrounder/src/debug.cpp:20`
- `pandaPI/pandaPIgrounder/src/debug.cpp:99`
- `pandaPI/pandaPIgrounder/src/options.ggo:15`
- `pandaPI/pandaPIgrounder/src/options.ggo:18`

What's wrong:

Default probes did not show ANSI color, but debug/output-domain helpers build
ANSI escapes unconditionally. The CLI has `--debug` and `--output-domain` but
no `--no-colour`/`--no-color` option and no visible `isatty()` policy.

Why it's wrong:

When debug or inspection output is piped into logs, hard-coded ANSI can pollute
machine-readable diagnostics. This is lower risk than the default stdout
contamination because it is not observed on ordinary successful runs.

C++ Core Guidelines rule ID:

- Chengdu managed-process rule; no direct C++ Core Guidelines rule.

Fix / recommendation:

Adopt one color policy across parser, grounder, and engine: default color only
when stderr is a TTY, support `--no-colour`/`--no-color`, and provide a stable
machine-readable mode with no ANSI.

### CLI and API Surface

#### G-010: CLI identity still exposes inherited binary name and version

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/options.ggo:1`
- `pandaPI/pandaPIgrounder/src/options.ggo:2`
- `pandaPI/pandaPIgrounder/src/options.ggo:3`
- `pandaPI/pandaPIgrounder/src/options.ggo:4`
- `pandaPI/pandaPIgrounder/README.md:34`
- `pandaPI/pandaPIgrounder/README.md:36`

What's wrong:

`options.ggo` declares package `pandaPIgrounder`, version `0.1`, and usage
`./pandaPIgrounder [OPTIONS] input.htn output.sas`. The observed `--version`
output is `pandaPIgrounder 0.1`.

Why it's wrong:

Chengdu 0.3.0 needs a deliberate binary-name policy for `pandapi-*` entry
points, provenance, and compatibility. The inherited identity does not
distinguish the public fork, platform release, or managed-process contract.

C++ Core Guidelines rule ID:

- Chengdu managed-process rule; no direct C++ Core Guidelines rule.

Fix / recommendation:

Design the migration in the managed-process arc: either ship dual names for one
release or make an explicit breaking change. Tie `--version` to Chengdu
provenance, source pins/import point, and platform build metadata.

#### G-011: Generated help text has formatting and policy gaps

Severity: Low

Location:

- `pandaPI/pandaPIgrounder/src/options.ggo:42`
- `pandaPI/pandaPIgrounder/src/options.ggo:44`
- `pandaPI/pandaPIgrounder/src/options.ggo:50`
- `pandaPI/pandaPIgrounder/src/options.ggo:51`
- `pandaPI/pandaPIgrounder/src/options.ggo:56`

What's wrong:

The observed `--help` output concatenates output-mode and SAS-mode default
text. The CLI also exposes performance and output options but lacks a
machine-readable diagnostics mode, an explicit stdin/stdout policy, and a
documented status taxonomy.

Why it's wrong:

Help text is part of the public API once `wolong` or another supervisor invokes
the tool. Ambiguous help and unstated stream/status semantics make migrations
harder to test.

C++ Core Guidelines rule ID:

- `I.2`: avoid surprises in interfaces.

Fix / recommendation:

Regenerate help from a reviewed `options.ggo`, document positional argument
requirements, and reserve the managed-process behavior contract for the
namespaced `pandapi-grounder` surface.

### Build, Tests, and Dependency Coupling

#### G-012: Active build works, but upstream `build.sh` is stale for the in-tree fork

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/build.sh:3`
- `pandaPI/pandaPIgrounder/build.sh:5`
- `pandaPI/pandaPIgrounder/build.sh:6`
- `pandaPI/pandaPIgrounder/build.sh:8`
- `scripts/build-grounder.sh:18`
- `scripts/build-grounder.sh:35`
- `scripts/build-grounder.sh:39`
- `scripts/build-grounder.sh:42`

What's wrong:

The upstream `build.sh` still clones `panda-planner-dev/pandaPIgrounder`,
initializes submodules, applies a cpddl patch, and builds in place. Chengdu's
active wrapper correctly builds from the in-tree vendored source copy, but the
inherited script remains present and can mislead maintainers.

Why it's wrong:

The public fork policy is source-in-tree with history preserved. Any surviving
network/submodule build path is a documentation and reproducibility hazard,
even if it is not the path used by `scripts/build-grounder.sh`.

C++ Core Guidelines rule ID:

- `SF.7`: project structure should communicate intended use.

Fix / recommendation:

In a later non-diagnosis slice, either remove, quarantine, or rewrite the
upstream script as documentation of upstream history. Keep Chengdu's active
build path as the only maintained local build contract.

#### G-013: Grounder build compiles dependency internals directly and bypasses their native build boundary

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/Makefile:32`
- `pandaPI/pandaPIgrounder/src/Makefile:74`
- `pandaPI/pandaPIgrounder/src/Makefile:83`
- `pandaPI/pandaPIgrounder/src/Makefile:87`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:9`
- `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:15`
- `pandaPI/pandaPIgrounder/src/cpddl.h:26`
- `pandaPI/pandaPIgrounder/src/cpddl.h:60`

What's wrong:

The grounder `Makefile` compiles `h2-fd-preprocessor/src/*.cc` objects through
the grounder's flags and links `cpddl` archives directly. `h2mutexes.cpp`
includes dependency-internal headers from `../h2-fd-preprocessor/src`, and
`cpddl.h` copies private code from `cpddl/src/obj.c` and `cpddl/src/cond.c`.

Why it's wrong:

This makes grounder behavior sensitive to dependency internals, compiler flags,
and private implementation details. It also means dependency warnings and ABI
changes surface as grounder maintenance risks even though the dependencies were
not audited as standalone libraries in this slice.

C++ Core Guidelines rule ID:

- `I.30`: encapsulate rule violations and implementation details.
- `ES.3`: do not repeat yourself; duplicated dependency internals are drift
  points.

Fix / recommendation:

Create a narrow grounder-owned adapter layer for `cpddl` and H2. Prefer stable
dependency APIs or a separately built library boundary. If private copies must
remain, isolate and label them with source-version evidence and regression
tests.

#### G-014: No primary grounder test/fixture posture was found

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/Makefile:79`
- `pandaPI/pandaPIgrounder/src/Makefile:103`
- `pandaPI/pandaPIgrounder/README.md:39`
- `pandaPI/pandaPIgrounder/cpddl/Makefile:90`
- `pandaPI/pandaPIgrounder/cpddl/Makefile:99`

What's wrong:

The primary grounder makefile has `all`, `clean`, and `debug`, but no test
target. The shallow inventory found no grounder-local test or fixture directory.
`cpddl` has dependency-internal check/static-check targets, but Chengdu's
grounder wrapper does not invoke them and they are not grounder behavior tests.

Why it's wrong:

The grounder is the bridge between parser output and engine input. Without
fixtures for malformed input, no-argument invocation, stdout pipe mode,
`--quiet` output files, H2/FAM integration, and unreachable-goal behavior,
0.3.0 process-contract changes will be hard to prove.

C++ Core Guidelines rule ID:

- Chengdu verification rule; no direct C++ Core Guidelines rule.

Fix / recommendation:

Add grounder fixtures derived from parser outputs and negative `.htn` cases.
Include pipe-supervised assertions for stdout/stderr, exit status, output-file
creation, H2/FAM modes, and resource ceilings. Keep dependency checks as
separate dependency-health gates.

#### G-015: Warning policy lets primary grounder warnings through

Severity: Low

Location:

- `pandaPI/pandaPIgrounder/src/Makefile:23`
- `pandaPI/pandaPIgrounder/src/Makefile:25`
- `pandaPI/pandaPIgrounder/src/Makefile:26`
- `pandaPI/pandaPIgrounder/src/Makefile:41`
- `pandaPI/pandaPIgrounder/src/Makefile:44`

What's wrong:

The makefile enables `-Wall -Wextra -pedantic` but suppresses some warnings and
leaves `CERROR` empty. The build completed with primary grounder warnings,
including the H2 string-literal arithmetic warning called out in G-001.

Why it's wrong:

Warnings are currently evidence but not a gate. In this codebase, at least one
warning corresponds to a credible runtime defect.

C++ Core Guidelines rule ID:

- `SF.7`: keep build structure clear and enforceable.

Fix / recommendation:

Separate primary-source warnings from dependency-internal warnings. Promote
selected primary warning classes to errors first, then decide which
dependency-internal warning classes need patches, suppression, or re-entry
audits.

### Maintainability, Shared-Code Candidates, and Modernization

#### G-016: Stream ownership in `main.cpp` uses raw `new` and leaks file streams

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/main.cpp:104`
- `pandaPI/pandaPIgrounder/src/main.cpp:117`
- `pandaPI/pandaPIgrounder/src/main.cpp:124`
- `pandaPI/pandaPIgrounder/src/main.cpp:132`
- `pandaPI/pandaPIgrounder/src/main.cpp:145`
- `pandaPI/pandaPIgrounder/src/main.cpp:153`
- `pandaPI/pandaPIgrounder/src/main.cpp:156`
- `pandaPI/pandaPIgrounder/src/main.cpp:169`
- `pandaPI/pandaPIgrounder/src/main.cpp:177`

What's wrong:

Input and output files are allocated with `new std::ifstream` and
`new std::ofstream`, assigned to base stream pointers, and never deleted.

Why it's wrong:

The process is short-lived today, but this pattern makes cleanup dependent on
process exit and complicates future reuse as a library or long-lived worker.
It also interacts poorly with deep `_exit()` calls, which bypass normal
destructors.

C++ Core Guidelines rule ID:

- `R.1`: manage resources automatically.
- `R.5`: prefer scoped objects.
- `R.11`: avoid calling `new` and `delete` explicitly.
- `R.20`: use ownership handles for dynamic ownership.
- `I.11`: never transfer ownership by raw pointer.

Fix / recommendation:

Use scoped `std::ifstream`/`std::ofstream` objects and pass references to the
selected streams. Keep ownership in `main()` and guarantee flush/close behavior
before returning a status.

#### G-017: `std::hash` specializations for `std::pair` are added inside namespace `std`

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/util.h:10`
- `pandaPI/pandaPIgrounder/src/util.h:11`
- `pandaPI/pandaPIgrounder/src/util.h:12`
- `pandaPI/pandaPIgrounder/src/util.h:26`

What's wrong:

`util.h` specializes `std::hash` for `std::pair<std::unordered_set<int>, int>`
and `std::pair<int, int>` inside namespace `std`.

Why it's wrong:

Adding specializations to `std` for standard-library types is not a valid
extension point. It can collide with future standard-library implementations
and makes the header a hidden global behavior change.

C++ Core Guidelines rule ID:

- `SL.3`: do not add non-standard entities to namespace `std`.

Fix / recommendation:

Move these to explicit local hasher types and pass them to the relevant
`unordered_*` containers. Consider a shared utility header only after parser
and engine audits identify common hashing needs.

#### G-018: Grounder mutates const input through `const_cast`

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/src/grounding.cpp:65`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:74`

What's wrong:

`run_grounding()` accepts `const Domain &` and `const Problem &`, then casts
away constness to expand conditional effects into artificial tasks.

Why it's wrong:

The API advertises a read-only contract while requiring mutation. That makes
callers and future shared libraries reason about the wrong ownership and
mutation boundary.

C++ Core Guidelines rule ID:

- `I.2`: avoid surprises in interfaces.
- `I.11`: make ownership and mutation explicit.

Fix / recommendation:

Either accept mutable references, clone into an explicit working model, or
split preprocessing into a stage whose mutation is visible in the type
signature.

#### G-019: Performance/resource limits are documented externally but not enforced by the tool

Severity: Medium

Location:

- `pandaPI/pandaPIgrounder/README.md:43`
- `pandaPI/pandaPIgrounder/README.md:47`
- `pandaPI/pandaPIgrounder/README.md:48`
- `pandaPI/pandaPIgrounder/src/options.ggo:36`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:208`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:225`
- `pandaPI/pandaPIgrounder/src/grounding.cpp:235`

What's wrong:

The README documents IPC-scale failures under 8 GB RAM and 30 minutes, but the
CLI exposes only performance-tuning options. The main grounding loop repeats
until pruning reaches a fixed point without a visible operator-controlled
resource or cancellation policy.

Why it's wrong:

For a supervised build/release consumer, resource exhaustion must be
classifiable. External supervisors can impose limits, but the grounder still
needs deterministic cleanup, partial-output policy, and status mapping when a
limit is reached.

C++ Core Guidelines rule ID:

- Chengdu managed-process rule; no direct C++ Core Guidelines rule.

Fix / recommendation:

Let the managed-process design decide whether limits live in the wrapper,
binary, or both. At minimum, document expected supervisor behavior and ensure
timeout/OOM termination maps to a stable status/event contract.

## 4. Dependency Boundary Notes

Grounder-owned integration findings:

- Direct H2 integration through `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:9`
  to `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:15`.
- H2 object compilation in the grounder makefile at
  `pandaPI/pandaPIgrounder/src/Makefile:74` and
  `pandaPI/pandaPIgrounder/src/Makefile:87`.
- Copied cpddl internals in `pandaPI/pandaPIgrounder/src/cpddl.h:26` and
  `pandaPI/pandaPIgrounder/src/cpddl.h:60`.
- Chengdu wrapper build sequencing through `scripts/build-grounder.sh:35` to
  `scripts/build-grounder.sh:42`.
- Stream/output suppression around H2 via global `std::cout` state at
  `pandaPI/pandaPIgrounder/src/h2mutexes.cpp:216`.

Dependency-internal findings observed but not repaired in this slice:

- `cpddl/third-party/boruvka` emitted macOS semaphore deprecation warnings and
  GNU-extension warnings during the successful build.
- `cpddl/third-party/opts`, `bliss`, and `lpsolve` emitted dependency-internal
  compiler warnings during the successful build.
- `h2-fd-preprocessor` emitted dependency-internal warnings while being
  compiled through the grounder makefile.

Recommended re-entry conditions for deeper dependency audits:

- Audit `h2-fd-preprocessor` separately if Chengdu keeps H2 in-process or
  needs stable stdout/stderr suppression without global stream hacks.
- Audit `cpddl` separately if FAM/invariant behavior becomes part of the
  default contract or if copied internals in `cpddl.h` cannot be replaced with
  stable APIs.
- Audit nested third-party libraries if supported-platform builds require
  warning-clean gates, static linking hardening, or security/update policy.
- Prefer dependency-level work only after parser, grounder, and engine synthesis
  identifies which dependency classes are actually on the 0.3.0 critical path.

## 5. Cross-Codebase Synthesis Inputs

Synthesis inputs for slice04:

- Compare with parser: both parser and grounder contaminate stdout under
  default modes and need one shared stdout/stderr/event policy.
- Compare with parser: both have deep process termination paths that bypass a
  centralized supervised-process status classifier.
- Compare with parser: both inherit generated `options.ggo` CLI surfaces that
  need coordinated `pandapi-*` naming, version/provenance, help text, and
  status semantics.
- Compare with parser: parser has fixture coverage; grounder currently lacks
  a primary fixture/test posture and should reuse parser-generated `.htn`
  fixtures.
- Compare with engine: the engine audit should check whether unreachable,
  timeout, invalid input, and unsupported feature cases also collapse to
  success or generic failure.
- Compare with engine: all three components should share a small process
  contract library or wrapper layer only after the common requirements are
  proven by the three audits.
- Compare with engine: binary-name migration should be uniform across
  `pandapi-parser`, `pandapi-grounder`, and the engine binary, with explicit
  compatibility choices.
- Cross-codebase library candidates: checked argument parsing, stream policy,
  status taxonomy, color/TTY policy, provenance/version reporting, path/output
  validation, and fixture/probe helpers are stronger shared-code candidates
  than algorithmic internals at this stage.

## 6. Things I Looked For and Did Not Find

Clean checks:

- I did not find tracked grounder source or dependency changes after the
  successful build/probes; `git status --short --branch` stayed free of
  `pandaPI/`, `scripts/`, `.github/`, and release-asset changes from this
  audit.
- I did not find ANSI color in ordinary default successful grounder output; the
  color risk is scoped to debug/output-domain helpers.
- I did not find a need to modify Chengdu's active `scripts/build-grounder.sh`
  for this diagnosis slice; it already builds from an isolated disposable copy
  prepared by `scripts/lib-platform.sh:98`.
- I did not find a primary grounder-local test/fixture directory at maxdepth 2;
  dependency-internal tests are not a substitute for grounder behavior tests.
- I did not find a source-level claim that stdout is machine-readable by
  default; the current implementation clearly treats stdout as both human and
  data output.
- I did not find a `--no-colour`, `--no-color`, or explicit `isatty()` policy
  in `options.ggo` or debug output paths.
- I did not find a complete standalone audit mandate for `cpddl` or
  `h2-fd-preprocessor` in this slice; the right boundary is grounder-owned
  integration plus re-entry conditions.
- I did not find a grounder-side shared-library abstraction ready to extract
  today; the shared candidates are process-contract utilities and adapters that
  should be confirmed against parser and engine first.
