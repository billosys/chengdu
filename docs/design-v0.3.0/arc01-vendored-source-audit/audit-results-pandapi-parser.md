# pandaPIparser Source Audit

## 1. Executive Summary

Date: 2026-08-09

`pandaPI/pandaPIparser` builds successfully from the vendored tree through the
Chengdu build wrapper, but it is not yet a high-quality managed-process CLI.
The main risks are process-contract defects: diagnostics and progress messages
are written to stdout, `--no-colour` does not suppress all ANSI escapes, parse
errors exit through `exit(-1)` instead of a controlled status path, and several
helpers terminate the process from deep inside parser/verifier code.

The source also has inherited C++ maintainability risks. Parser state is held in
process-wide globals, AST ownership is represented by raw pointers and explicit
`new`, public headers use `using namespace std`, and release builds compile with
`-DNDEBUG` even though several model-validation paths depend on `assert()`.
These are credible risks for the 0.3.0 goal of supervised execution, stable
status classification, and eventual shared parser/grounder/engine substrate.

No parser source, build scripts, workflows, release assets, or generated build
outputs were changed by this audit.

## 2. Audit Context

Project root: `/Users/oubiwann/lab/billosys/chengdu`

Branch: `release/0.3.x`

Audited paths:

- `pandaPI/pandaPIparser/src`
- `pandaPI/pandaPIparser/src/options.ggo`
- `pandaPI/pandaPIparser/makefile`
- `pandaPI/pandaPIparser/tests`
- `pandaPI/pandaPIparser/doc`
- `scripts/build-parser.sh`
- `scripts/lib-platform.sh`

C++ knowledge-pack guides loaded:

- `/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/SKILL.md`
- `guides/11-anti-patterns.md`
- `guides/02-api-design.md`
- `guides/03-functions.md`
- `guides/05-resource-management.md`
- `guides/06-error-handling.md`
- `guides/10-expressions-and-statements.md`
- `guides/12-project-structure-and-tooling.md`
- `guides/13-standard-library.md`
- `guides/14-c-style-and-modernization.md`
- Supporting Core Guidelines excerpts from the local `cpp-guidelines` skill for
  rule IDs: `I.2`, `I.11`, `I.30`, `R.1`, `R.11`, `R.20`, `E.4`, `ES.65`,
  `SF.7`, `SL.con.3`, and `SL.io.2`.

Commands run:

- `git diff --cached --check` - exit 0 before committing staged planning files.
- `git commit -m "docs: open vendored source audit slice" ...` - exit 0,
  produced commit `948c54d3`.
- `git status --short --branch` - exit 0, clean on `release/0.3.x` after the
  planning commit.
- `rg --files pandaPI/pandaPIparser` - exit 0, inventoried parser source,
  documentation, makefile, and fixtures.
- `find pandaPI/pandaPIparser/tests -maxdepth 1 -type f | sort | wc -l` - exit
  0, reported 30 fixture files.
- `find pandaPI/pandaPIparser/src -maxdepth 1 -type f | sort | wc -l` - exit 0,
  reported 39 source/header/generated-input files.
- `./scripts/build-parser.sh` - exit 0, produced
  `dist/macos-arm64/pandaPIparser`; warnings were limited to unused variable,
  sign-compare, range-loop copy, and generated Flex helper warnings.
- CLI probe script using `dist/macos-arm64/pandaPIparser` - probe command exit
  0; observed parser subprocess statuses: missing args status 1, missing file
  status 2, malformed input status 255, malformed input with `--no-colour`
  status 255, valid fixture parse status 0.
- `git status --short --branch` after build/probes - exit 0, clean on
  `release/0.3.x`.

Inventory:

- Parser source is isolated under `pandaPI/pandaPIparser/src` with 39 files.
- CLI generation is driven by `pandaPI/pandaPIparser/src/options.ggo`.
- Build is driven by `pandaPI/pandaPIparser/makefile`.
- Parser fixtures are present under `pandaPI/pandaPIparser/tests` with 30 files.
- Build wrapper copies vendored source into an ignored platform build directory
  before running `make`.

## 3. Findings

### Correctness and Runtime Safety

#### P-001: Release builds compile out validation asserts that protect output invariants

Severity: High

Location:

- `pandaPI/pandaPIparser/makefile:5`
- `pandaPI/pandaPIparser/src/output.cpp:180`
- `pandaPI/pandaPIparser/src/output.cpp:186`
- `pandaPI/pandaPIparser/src/output.cpp:190`
- `pandaPI/pandaPIparser/src/output.cpp:194`
- `pandaPI/pandaPIparser/src/output.cpp:217`
- `pandaPI/pandaPIparser/src/output.cpp:235`
- `pandaPI/pandaPIparser/src/output.cpp:246`
- `pandaPI/pandaPIparser/src/output.cpp:250`

What's wrong:

The makefile compiles release objects with `-DNDEBUG`, but `output.cpp` uses
`assert()` for duplicate task and sort-membership invariants while continuing to
emit output. In a release build, those checks disappear. For example, duplicate
primitive/abstract task checks print to stderr and then rely on `assert()` for
enforcement, and sort IDs are accessed with `sort_id[s]` after an assertion that
is disabled in release mode.

Why it's wrong:

A managed process must reliably classify invalid input and internal model
inconsistency. With `-DNDEBUG`, invalid models can become silent output
corruption instead of a deterministic error status. This is especially risky
because `operator[]` on a `map` can insert a default value when the key is
missing.

C++ Core Guidelines rule ID:

- `E.4`: design error-handling around invariants.
- `SL.io.2`: consider ill-formed input when reading.

Fix / recommendation:

Replace validation `assert()` calls on user/model data with explicit checked
errors that return through a single parser status path. Keep `assert()` only for
debug-only programmer invariants that are impossible to trigger from input.

#### P-002: Include handling can recurse into a null `FILE*`

Severity: High

Location:

- `pandaPI/pandaPIparser/src/hddl.y:118`
- `pandaPI/pandaPIparser/src/hddl.y:119`
- `pandaPI/pandaPIparser/src/hddl.y:120`
- `pandaPI/pandaPIparser/src/hddl.y:596`
- `pandaPI/pandaPIparser/src/hddl.y:599`
- `pandaPI/pandaPIparser/src/hddl.y:600`
- `pandaPI/pandaPIparser/src/hddl.y:607`
- `pandaPI/pandaPIparser/src/hddl.y:608`

What's wrong:

The `:include` grammar stores `NAME + ".hddl"` with `strdup()`, then
`run_parser_on_file()` recursively calls `run_parser_on_file(fopen(file, "r"),
file)` without checking whether `fopen()` succeeded. The parser then assigns the
possibly null handle to `yyin` and calls `yyparse()`.

Why it's wrong:

Missing includes become parser-internal undefined behavior or parser-library
dependent failure instead of a clear "include not found" error. The code also
resolves includes relative to the current working directory, not necessarily the
including file.

C++ Core Guidelines rule ID:

- `ES.65`: do not dereference or use invalid indirections.
- `R.1`: manage `fopen`/`fclose` style resource pairs with RAII handles.

Fix / recommendation:

Resolve includes relative to the including file, check open failures before
calling the parser, close file handles through RAII wrappers, and return a
structured parse error rather than recursing through a raw `FILE*`.

#### P-003: Plan converter/verifier parsing accepts malformed tokens and assumes non-empty names

Severity: Medium

Location:

- `pandaPI/pandaPIparser/src/plan.cpp:160`
- `pandaPI/pandaPIparser/src/plan.cpp:166`
- `pandaPI/pandaPIparser/src/plan.cpp:222`
- `pandaPI/pandaPIparser/src/plan.cpp:557`
- `pandaPI/pandaPIparser/src/plan.cpp:558`
- `pandaPI/pandaPIparser/src/plan.cpp:572`
- `pandaPI/pandaPIparser/src/plan.cpp:573`
- `pandaPI/pandaPIparser/src/plan.cpp:577`
- `pandaPI/pandaPIparser/src/plan.cpp:593`
- `pandaPI/pandaPIparser/src/plan.cpp:594`
- `pandaPI/pandaPIparser/src/plan.cpp:609`
- `pandaPI/pandaPIparser/src/plan.cpp:610`
- `pandaPI/pandaPIparser/src/plan.cpp:629`
- `pandaPI/pandaPIparser/src/plan.cpp:630`

What's wrong:

Plan parsing uses `atoi()` for an ID token and `stoi()` without a local
try/catch or validation path for later IDs. The converter also indexes
`method.second[0]`, `method.second[1]`, `task.second.name[0]`, and
`task.second.name[1]` without checking string length.

Why it's wrong:

Malformed plan input can be converted to `0`, throw an uncaught exception, or
read past a string boundary. That prevents the caller from distinguishing
invalid input from an internal parser/verifier defect.

C++ Core Guidelines rule ID:

- `SL.io.2`: validate ill-formed input as soon as possible.
- `SL.con.3`: avoid bounds errors.

Fix / recommendation:

Parse numeric IDs with a checked conversion routine, validate full-token
consumption, reject empty/short names before indexing, and route all parse
errors through the same status classifier used by HDDL parse failures.

### Error Handling and Exit Behavior

#### P-004: Deep parser/verifier helpers terminate the process directly

Severity: High

Location:

- `pandaPI/pandaPIparser/src/hddl.y:619`
- `pandaPI/pandaPIparser/src/hddl.y:626`
- `pandaPI/pandaPIparser/src/verify.cpp:27`
- `pandaPI/pandaPIparser/src/verify.cpp:28`
- `pandaPI/pandaPIparser/src/verify.cpp:29`
- `pandaPI/pandaPIparser/src/verify.cpp:61`
- `pandaPI/pandaPIparser/src/verify.cpp:62`
- `pandaPI/pandaPIparser/src/verify.cpp:63`
- `pandaPI/pandaPIparser/src/verify.cpp:73`
- `pandaPI/pandaPIparser/src/verify.cpp:74`
- `pandaPI/pandaPIparser/src/verify.cpp:75`
- `pandaPI/pandaPIparser/src/verify.cpp:78`
- `pandaPI/pandaPIparser/src/verify.cpp:79`
- `pandaPI/pandaPIparser/src/verify.cpp:80`
- `pandaPI/pandaPIparser/src/verify.cpp:1318`
- `pandaPI/pandaPIparser/src/verify.cpp:1319`
- `pandaPI/pandaPIparser/src/verify.cpp:1320`
- `pandaPI/pandaPIparser/src/verify.cpp:1321`
- `pandaPI/pandaPIparser/src/output.cpp:155`
- `pandaPI/pandaPIparser/src/output.cpp:156`
- `pandaPI/pandaPIparser/src/output.cpp:157`

What's wrong:

Parser, verifier, and output helpers call `exit()` directly, including
`exit(-1)`, `exit(2)`, `exit(1)`, and `exit(0)` from non-`main` code. The
malformed-input CLI probe observed shell status 255 from `exit(-1)`. The
verifier reports "No plan provided" and then exits success from inside
`verify_plan()`.

Why it's wrong:

Supervised execution needs a stable taxonomy: usage error, unreadable input,
parse error, validation failure, unsupported feature, verifier false, internal
defect, and success. Deep `exit()` calls bypass `main`, prevent cleanup, and
make status ownership unclear.

C++ Core Guidelines rule ID:

- `E.4`: design error-handling around invariants.
- Chengdu-specific managed-process finding: no Core Guidelines rule directly
  defines this CLI status taxonomy.

Fix / recommendation:

Introduce a small result/status type for parser and verifier operations. Let
deep helpers return errors upward; let `main` be the only place that maps
outcomes to exit codes and stdout/stderr content.

#### P-005: Some argument validation reports an error but continues

Severity: High

Location:

- `pandaPI/pandaPIparser/src/main.cpp:231`
- `pandaPI/pandaPIparser/src/main.cpp:232`
- `pandaPI/pandaPIparser/src/main.cpp:233`
- `pandaPI/pandaPIparser/src/main.cpp:238`
- `pandaPI/pandaPIparser/src/main.cpp:239`
- `pandaPI/pandaPIparser/src/main.cpp:240`

What's wrong:

When ordinary pandaPI output receives two output files, `main.cpp` prints "you
may only specify one output file" but does not return. It proceeds to parse the
domain and problem.

Why it's wrong:

An invalid CLI invocation should not continue into parsing and output
generation. For a supervised process, the first classified error should be the
final process result.

C++ Core Guidelines rule ID:

- `E.4`: preserve invariants after errors.
- Chengdu-specific managed-process finding: CLI argument classification must be
  terminal and status-coded.

Fix / recommendation:

Return a usage/configuration status immediately after this diagnostic. Include a
regression test that confirms the process does not parse or write output after a
fatal CLI validation error.

### Stdio, TTY, ANSI, Buffering, and Pipe Behavior

#### P-006: Diagnostics and progress contaminate stdout

Severity: High

Location:

- `pandaPI/pandaPIparser/src/main.cpp:188`
- `pandaPI/pandaPIparser/src/main.cpp:190`
- `pandaPI/pandaPIparser/src/main.cpp:192`
- `pandaPI/pandaPIparser/src/main.cpp:214`
- `pandaPI/pandaPIparser/src/main.cpp:215`
- `pandaPI/pandaPIparser/src/main.cpp:223`
- `pandaPI/pandaPIparser/src/main.cpp:224`
- `pandaPI/pandaPIparser/src/main.cpp:227`
- `pandaPI/pandaPIparser/src/main.cpp:228`
- `pandaPI/pandaPIparser/src/hddl.y:596`
- `pandaPI/pandaPIparser/src/hddl.y:597`

What's wrong:

Missing-argument and missing-file diagnostics are printed to `cout`. Parse
progress is also printed to `cout` for every parsed file. The CLI probes
confirmed:

- missing args: status 1, stdout contained "You need to provide a domain and
  problem file as input."
- missing file: status 2, stdout contained "I can't open does-not-exist.hddl!"
- valid fixture parse with an output file: status 0, stdout still contained
  `Parsing pandaPI/pandaPIparser/tests/empty-d.hddl` and
  `Parsing pandaPI/pandaPIparser/tests/empty-p.hddl`.

Why it's wrong:

For a high-quality CLI and managed process, stdout should be reserved for the
primary data product unless the user explicitly asks for human progress output.
When stdout is contaminated, callers cannot reliably pipe or parse output.

C++ Core Guidelines rule ID:

- Chengdu-specific managed-process finding: no direct Core Guidelines rule
  applies.

Fix / recommendation:

Move errors and progress to stderr, and suppress progress by default when
writing a machine data artifact. Add a quiet/machine mode if stdout must become
structured process output.

#### P-007: `--no-colour` does not suppress parser syntax-error ANSI escapes

Severity: High

Location:

- `pandaPI/pandaPIparser/src/options.ggo:15`
- `pandaPI/pandaPIparser/src/options.ggo:16`
- `pandaPI/pandaPIparser/src/util.cpp:3`
- `pandaPI/pandaPIparser/src/util.cpp:5`
- `pandaPI/pandaPIparser/src/util.cpp:6`
- `pandaPI/pandaPIparser/src/hddl.y:619`
- `pandaPI/pandaPIparser/src/hddl.y:620`

What's wrong:

The CLI exposes `--no-colour`, and most colorized strings can route through the
`color()` helper that respects `no_colors_in_output`. `yyerror()`, however,
hard-codes escape sequences around "Parse error" and the line number. The CLI
probe confirmed raw `\033` sequences in stderr even with `--no-colour`.

Why it's wrong:

Supervisors and log collectors must be able to disable terminal control
sequences. A color flag that is only partially honored is worse than no flag
because callers will trust it and still receive non-plain diagnostics.

C++ Core Guidelines rule ID:

- `I.30`: encapsulate necessary unsafe or special behavior rather than leaking
  it through interfaces.
- Chengdu-specific managed-process finding: ANSI/TTY behavior is part of the
  process contract.

Fix / recommendation:

Route `yyerror()` through the same `color()` helper, and make the default color
policy explicit: no color unless stdout/stderr is a TTY, or no color whenever
machine/quiet mode is active.

### CLI and API Surface

#### P-008: Public CLI identity is still upstream `pandaPIparser` version `0.1`

Severity: Medium

Location:

- `pandaPI/pandaPIparser/src/options.ggo:1`
- `pandaPI/pandaPIparser/src/options.ggo:2`
- `pandaPI/pandaPIparser/src/options.ggo:3`
- `pandaPI/pandaPIparser/src/options.ggo:4`
- `scripts/build-parser.sh:18`
- `scripts/build-parser.sh:27`
- `scripts/build-parser.sh:30`

What's wrong:

The generated CLI package is `pandaPIparser`, version `0.1`, with usage
`./pandaPIparser`. The Chengdu build wrapper also publishes the binary as
`dist/<platform>/pandaPIparser`.

Why it's wrong:

The 0.3.0 project is evaluating a managed `pandapi-*` command surface. As long
as parser identity remains only upstream-oriented, downstream help/version
output cannot communicate the Chengdu release, provenance, binary namespace, or
status contract.

C++ Core Guidelines rule ID:

- Chengdu-specific CLI/API finding: no direct Core Guidelines rule applies.

Fix / recommendation:

For the future implementation slice, define whether the source binary is renamed
or wrapped. Either way, help/version output should include the Chengdu-facing
command name, upstream source identity, release/provenance metadata, and a
stable statement of output/status modes.

#### P-009: Parser is not factored as a reusable API boundary

Severity: Medium

Location:

- `pandaPI/pandaPIparser/src/main.cpp:37`
- `pandaPI/pandaPIparser/src/main.cpp:40`
- `pandaPI/pandaPIparser/src/main.cpp:44`
- `pandaPI/pandaPIparser/src/main.cpp:49`
- `pandaPI/pandaPIparser/src/main.cpp:50`
- `pandaPI/pandaPIparser/src/parsetree.hpp:113`
- `pandaPI/pandaPIparser/src/parsetree.hpp:114`
- `pandaPI/pandaPIparser/src/parsetree.hpp:116`
- `pandaPI/pandaPIparser/src/parsetree.hpp:118`
- `pandaPI/pandaPIparser/src/parsetree.hpp:120`
- `pandaPI/pandaPIparser/src/domain.hpp:83`
- `pandaPI/pandaPIparser/src/domain.hpp:84`
- `pandaPI/pandaPIparser/src/domain.hpp:85`
- `pandaPI/pandaPIparser/src/domain.hpp:86`
- `pandaPI/pandaPIparser/src/domain.hpp:87`
- `pandaPI/pandaPIparser/src/domain.hpp:88`

What's wrong:

The parser writes parsed domain/problem data into process-wide global
containers declared in headers and defined in `main.cpp`. The parse entry point
does not return a parse result object.

Why it's wrong:

This makes parser execution non-reentrant, hard to test in isolation, and hard
to reuse from a library or long-lived supervisor. It also makes later
cross-codebase sharing with grounder/engine harder because dependencies are
implicit.

C++ Core Guidelines rule ID:

- `I.2`: avoid non-`const` global variables.
- `I.30`: encapsulate rule violations.

Fix / recommendation:

Introduce a `ParserContext`/`ParsedProblem` boundary that owns parser state and
returns either a parsed model or a classified error. Keep any unavoidable Bison
global state inside a small adapter.

### Build and Tests

#### P-010: Fixtures exist, but the parser makefile has no test target or expected-output oracle

Severity: Medium

Location:

- `pandaPI/pandaPIparser/makefile:24`
- `pandaPI/pandaPIparser/makefile:26`
- `pandaPI/pandaPIparser/makefile:28`
- `pandaPI/pandaPIparser/makefile:59`
- `pandaPI/pandaPIparser/makefile:65`

What's wrong:

The parser tree has 30 fixture files under `tests/`, but the makefile exposes
only `all`, `debug`, `clean`, and `install` behavior. There is no parser-local
test target, no fixture runner, and no expected-output/status oracle.

Why it's wrong:

The managed-process work will change exactly the areas most likely to regress:
stdout/stderr, color, status codes, and malformed-input handling. Without a
fixture-driven oracle, these changes are easy to break silently.

C++ Core Guidelines rule ID:

- Chengdu-specific build/test finding: no direct Core Guidelines rule applies.

Fix / recommendation:

Add a parser smoke/fixture target in a future implementation slice. At minimum,
cover success, missing args, missing file, malformed HDDL, `--no-colour`,
ordinary output-file mode, and verifier empty-plan behavior with expected exit
codes and stdout/stderr assertions.

#### P-011: Build uses good warning flags but leaves warnings non-fatal

Severity: Low

Location:

- `pandaPI/pandaPIparser/makefile:2`
- `pandaPI/pandaPIparser/makefile:3`
- `pandaPI/pandaPIparser/makefile:5`

What's wrong:

The makefile enables `-Wall -Wextra -pedantic -std=c++17`, but `CERROR` is empty
and `CWARN` suppresses unused-parameter warnings. The observed build completed
with warnings in `hpdlWriter.cpp`, `plan.cpp`, `verify.cpp`, and generated Flex
helpers.

Why it's wrong:

Warnings are not currently blocking release artifacts. Some observed warnings
are harmless in inherited source, but sign-compare and range-loop copy warnings
can hide real correctness or performance bugs over time.

C++ Core Guidelines rule ID:

- Chengdu-specific build-quality finding: warning policy is a project release
  contract rather than a single Core Guidelines rule.

Fix / recommendation:

Keep inherited generated-code warnings non-fatal if needed, but add a future
`strict` target or CI step for hand-written parser code and burn warnings down
before making process-contract changes.

### Maintainability, Duplication, and Shared-Code Candidates

#### P-012: Raw owning pointers and explicit `new` dominate parser AST and streams

Severity: Medium

Location:

- `pandaPI/pandaPIparser/src/parsetree.hpp:48`
- `pandaPI/pandaPIparser/src/parsetree.hpp:51`
- `pandaPI/pandaPIparser/src/parsetree.hpp:55`
- `pandaPI/pandaPIparser/src/parsetree.hpp:83`
- `pandaPI/pandaPIparser/src/parsetree.hpp:85`
- `pandaPI/pandaPIparser/src/parsetree.hpp:86`
- `pandaPI/pandaPIparser/src/parsetree.hpp:87`
- `pandaPI/pandaPIparser/src/parsetree.hpp:97`
- `pandaPI/pandaPIparser/src/parsetree.hpp:98`
- `pandaPI/pandaPIparser/src/parsetree.hpp:99`
- `pandaPI/pandaPIparser/src/parsetree.hpp:100`
- `pandaPI/pandaPIparser/src/main.cpp:197`
- `pandaPI/pandaPIparser/src/main.cpp:198`
- `pandaPI/pandaPIparser/src/main.cpp:201`
- `pandaPI/pandaPIparser/src/main.cpp:252`
- `pandaPI/pandaPIparser/src/main.cpp:260`
- `pandaPI/pandaPIparser/src/main.cpp:285`
- `pandaPI/pandaPIparser/src/main.cpp:316`
- `pandaPI/pandaPIparser/src/main.cpp:324`
- `pandaPI/pandaPIparser/src/main.cpp:353`
- `pandaPI/pandaPIparser/src/main.cpp:361`
- `pandaPI/pandaPIparser/src/main.cpp:372`

What's wrong:

AST structures store owning raw pointers, Bison actions allocate many nodes, and
`main.cpp` allocates `ifstream`/`ofstream` objects with `new` without visible
matching `delete`.

Why it's wrong:

Short-lived CLI process leaks may be tolerable in the inherited binary, but raw
ownership becomes a real defect class for tests, library extraction, repeated
in-process parsing, and long-lived managed execution.

C++ Core Guidelines rule ID:

- `R.1`: manage resources automatically with RAII.
- `R.11`: avoid calling `new` and `delete` explicitly.
- `R.20`: use smart pointers to represent ownership.
- `I.11`: never transfer ownership by raw pointer or reference.

Fix / recommendation:

Start with streams: replace heap-allocated `ifstream`/`ofstream` with local
objects. For the AST, define ownership explicitly with values or
`std::unique_ptr`, and isolate Bison `%union` raw pointers behind a construction
layer if a full grammar rewrite is out of scope.

#### P-013: Headers export `using namespace std`

Severity: Medium

Location:

- `pandaPI/pandaPIparser/src/util.hpp:3`
- `pandaPI/pandaPIparser/src/domain.hpp:10`
- `pandaPI/pandaPIparser/src/parsetree.hpp:10`
- `pandaPI/pandaPIparser/src/output.hpp:6`
- `pandaPI/pandaPIparser/src/orderingDecomposition.hpp:9`
- `pandaPI/pandaPIparser/src/cwa.hpp:8`
- `pandaPI/pandaPIparser/src/verification_encoding.hpp:6`

What's wrong:

Multiple public project headers put `using namespace std;` at global scope.

Why it's wrong:

This leaks namespace choices into every translation unit that includes these
headers and makes later shared-library boundaries more fragile.

C++ Core Guidelines rule ID:

- `SF.7`: do not write `using namespace` at global scope in a header file.

Fix / recommendation:

Remove header-level `using namespace std;` during a modernization slice, qualify
standard-library types in headers, and keep using declarations local to `.cpp`
files where appropriate.

#### P-014: Parser-side shared-code candidates are clear but not yet extracted

Severity: Medium

Location:

- `pandaPI/pandaPIparser/src/main.cpp:197`
- `pandaPI/pandaPIparser/src/main.cpp:247`
- `pandaPI/pandaPIparser/src/main.cpp:311`
- `pandaPI/pandaPIparser/src/main.cpp:348`
- `pandaPI/pandaPIparser/src/main.cpp:369`
- `pandaPI/pandaPIparser/src/util.cpp:5`
- `pandaPI/pandaPIparser/src/util.hpp:8`
- `pandaPI/pandaPIparser/src/hddl.y:619`
- `pandaPI/pandaPIparser/src/hddl.y:620`

What's wrong:

Output-file opening and diagnostic/color behavior are repeated in several
branches of `main.cpp`, while parser errors bypass the central `color()` helper.

Why it's wrong:

The same duplicated areas are exactly where 0.3.0 needs stronger semantics:
status mapping, stream discipline, and color/TTY policy. If grounder and engine
have similar CLI concerns, this should become a shared Chengdu helper contract
rather than three separate one-off rewrites.

C++ Core Guidelines rule ID:

- `I.30`: encapsulate rule violations and special behavior.
- `R.1`: use RAII for repeated file resource handling.

Fix / recommendation:

Compare this report with the grounder and engine audits. If similar defects are
present, extract a small common CLI/process layer for opening outputs,
diagnostics, color policy, and exit-status mapping.

### Modernization Candidates

#### P-015: C library APIs remain on hot input paths

Severity: Low

Location:

- `pandaPI/pandaPIparser/src/main.cpp:219`
- `pandaPI/pandaPIparser/src/main.cpp:220`
- `pandaPI/pandaPIparser/src/main.cpp:221`
- `pandaPI/pandaPIparser/src/hddl-token.l:72`
- `pandaPI/pandaPIparser/src/hddl-token.l:73`
- `pandaPI/pandaPIparser/src/hddl-token.l:74`
- `pandaPI/pandaPIparser/src/hddl-token.l:75`
- `pandaPI/pandaPIparser/src/hddl-token.l:76`

What's wrong:

The parser opens files with `fopen()`, stores lexer strings with `strdup()`, and
parses numeric tokens with `atoi()`/`atof()` in the lexer.

Why it's wrong:

These APIs are common in Flex/Bison code, so this is not automatically a release
blocker. They become a maintenance burden when errors must be surfaced through a
managed-process API because ownership and conversion failures are not explicit.

C++ Core Guidelines rule ID:

- `R.1`: manage resources automatically.
- `R.11`: avoid explicit allocation paths where RAII handles are available.
- `SL.io.2`: validate ill-formed input.

Fix / recommendation:

Do not rewrite the grammar just for style. First wrap file ownership and checked
numeric conversion. Later, consider Bison C++ parser APIs or a parser adapter
only if the grounder/engine synthesis shows repeated ownership/status defects.

## 4. Cross-Codebase Synthesis Inputs

Compare with grounder:

- Does grounder write progress/errors to stdout when stdout may be a machine
  data artifact?
- Does grounder expose a color/TTY option, and is it consistently applied?
- Does grounder return all errors through `main`, or call `exit()` from helpers?
- Does grounder use release-disabled `assert()` for user/model validation?
- Does grounder publish upstream binary names/version strings that conflict
  with the planned `pandapi-*` namespace?

Compare with engine:

- Does engine have the same stdout/stderr/status taxonomy defects under
  supervised execution?
- Does engine rely on process-wide global state that blocks in-process reuse or
  repeated tests?
- Does engine use raw ownership for long-lived planner structures in ways that
  would affect a managed worker process?
- Does engine have a fixture/test oracle for statuses and streams, not only
  algorithmic success?
- Are there shared diagnostic, color, output-file, and exit-status helpers that
  should be implemented once across parser, grounder, and engine?

Shared/library candidates:

- A Chengdu CLI/process facade for usage errors, input open errors, parse/model
  validation errors, unsupported-feature errors, and internal defects.
- A stream policy helper for stdout data, stderr diagnostics, quiet/progress
  modes, and color/TTY detection.
- A small RAII file/output helper for optional output files.
- A fixture runner that asserts exit status, stdout, stderr, and produced files
  consistently across parser, grounder, and engine.

## 5. Things I Looked For and Did Not Find

Clean checks:

1. I did not find build-time source mutation against the tracked vendored tree:
   `scripts/build-parser.sh:12` calls `prepare_build_source_copy`, and
   `scripts/lib-platform.sh:98` through `scripts/lib-platform.sh:101` copy
   source under `build/<platform>/source` before generation and compilation.
2. I did not find a missing parser fixture directory: `pandaPI/pandaPIparser/tests`
   exists and contains 30 files.
3. I did not find a missing central CLI definition: options are generated from
   `pandaPI/pandaPIparser/src/options.ggo`, making the public option surface
   auditable in one file.
4. I did not find an absence of basic compiler warnings: the makefile uses
   `-Wall -Wextra -pedantic -std=c++17` at
   `pandaPI/pandaPIparser/makefile:5`.
5. I did not find parser syntax errors without file/line context:
   `pandaPI/pandaPIparser/src/hddl-token.l:6` and
   `pandaPI/pandaPIparser/src/hddl-token.l:11` enable location tracking, and
   `pandaPI/pandaPIparser/src/hddl.y:620` prints file and line.
6. I did not find color handling completely scattered: `util.cpp` provides a
   central `color()` helper at `pandaPI/pandaPIparser/src/util.cpp:5`, and the
   defect is specifically that `yyerror()` bypasses it.
7. I did not find a failed current macOS parser build: `./scripts/build-parser.sh`
   exited 0 and produced `dist/macos-arm64/pandaPIparser`.
8. I did not find tracked parser/build/release edits from this audit: the
   post-build/probe `git status --short --branch` remained clean before report
   artifacts were written.
