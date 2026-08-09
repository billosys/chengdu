# chengdu arc02 / slice01 - parser-library-research - closing report

Date: 2026-08-09

## 1. Summary

The parser audit supports a small dependency portfolio, not a wholesale rewrite.
For near-term 0.3.0 planning, the strongest candidates are:

- **Standard-library modernization** as the baseline: RAII streams/files,
  `std::unique_ptr`, `std::filesystem`, `std::string_view`, checked container
  access where appropriate, and explicit status enums.
- **CLI11** as the leading replacement candidate for `gengetopt` and ad hoc
  argument validation.
- **fmt** as the leading formatting/diagnostic building block.
- **tl::expected** as a lightweight pilot for parser/status plumbing if the
  grounder and engine audits show the same result-return pressure.
- **Catch2** as the leading C++ unit/fixture test framework if chengdu adds a
  C++ test target.
- **nlohmann/json** as a hold/pilot candidate for JSON Lines machine-event mode,
  not as a parser dependency today.

The parser-generator decision should stay on hold. Bison already has C++ and
variant-based paths that could reduce raw pointer pressure without changing the
grammar technology. ANTLR, PEGTL, and lexy remain plausible later research
topics, but they are too large for a 0.3.0 parser recommendation from parser
evidence alone.

## 2. Audit Mapping

| Audit finding | Defect class | Candidate bucket |
|---------------|--------------|------------------|
| P-001 | Release-disabled validation and weak invariant/error routing | status/result model; test oracle |
| P-002 | Raw `FILE*`, unchecked include open, cwd-relative includes | `std::filesystem`; RAII file wrappers; parser adapter |
| P-003 | unchecked numeric conversion and string indexing | standard-library parsing/checks; status/result model; tests |
| P-004 | deep `exit()` calls from parser/verifier/output helpers | status/result model; CLI facade |
| P-005 | fatal CLI validation reports an error but continues | CLI parser; test oracle |
| P-006 | diagnostics/progress contaminate stdout | CLI/process facade; formatting/logging |
| P-007 | `--no-colour` bypassed by `yyerror()` | diagnostics/color facade; formatting |
| P-008 | upstream CLI identity and version only | CLI parser; version/provenance facade |
| P-009 | parser state in process-wide globals | parser context API; status/result model |
| P-010 | fixtures but no test target/oracle | C++ test framework; process fixture runner |
| P-011 | warnings not fatal | build/tooling policy; test framework |
| P-012 | raw owning pointers and heap streams | standard-library RAII; Bison C++ variant pilot |
| P-013 | headers export `using namespace std` | standard-library hygiene; no third-party needed |
| P-014 | repeated output-file and color/diagnostic code | shared process facade; formatting |
| P-015 | C APIs on input paths | standard-library wrappers; Bison C++ adapter |

## 3. Standard-library Baseline

Disposition: **adopt as default design posture**.

Parser findings P-002, P-003, P-009, P-012, P-013, and P-015 do not require new
third-party libraries before they require better use of C++17:

- Use stack `std::ifstream`/`std::ofstream` and small RAII wrappers before
  adding a file library.
- Use `std::filesystem::path` for include resolution and output paths.
- Use `std::unique_ptr` or value ownership for AST/resource ownership where
  practical.
- Use `std::string_view` for non-owning text views at API boundaries that do
  not require mutation.
- Use explicit `enum class ParserStatus` plus a small error payload before
  deciding whether a generic result library is needed everywhere.

Why this matters: C++ Core Guidelines `SL.1` says to use libraries where
possible, and `SL.2` prefers the standard library when it is enough. Here, the
standard library removes resource leaks and ownership ambiguity without adding
release-packaging risk.

## 4. Candidate Assessment

### CLI11

Disposition: **adopt or pilot for Arc03/Arc04 CLI redesign**.

Audit fit: P-005, P-006, P-008, P-014.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ command-line parser with help, validation, positional arguments, option groups, subcommands, parse-error handling, and generated exit behavior. |
| License | BSD-3-Clause. |
| Maintenance | Current GitHub page reports active repository metadata and latest release `v2.6.2` on 2026-02-26. |
| Integration | Header-only by default, single-header release available, or precompiled/static CMake target. No runtime dependency beyond C++11. |
| Build/packaging | Good fit for chengdu's Linux/macOS binary releases if vendored with NOTICE/license text or brought in through a future dependency policy. |
| Risk | Replacing `gengetopt` changes help text, parse errors, and option edge cases. This must land with golden CLI fixture tests. |

Recommendation: prefer CLI11 over continuing `gengetopt` if grounder and engine
show comparable CLI pressure. For parser alone, run a contained pilot that
preserves current options and asserts help/error/status behavior.

### fmt

Disposition: **adopt as the formatting primitive for diagnostics and structured
messages**.

Audit fit: P-006, P-007, P-014.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Type-safe formatting and direct printing alternative to C stdio and iostream formatting. |
| License | MIT. |
| Maintenance | Current GitHub page reports latest release `12.1.0` on 2025-10-29, broad use, extensive tests, fuzzing, and package-manager support. |
| Integration | Small source footprint, no external dependencies, optional header-only mode. |
| Build/packaging | Strong fit for static binary packaging if pinned and noticed. |
| Risk | Does not itself enforce stdout/stderr policy or color policy; chengdu still needs a small facade. |

Recommendation: use fmt underneath a chengdu `diagnostics`/`process_io` helper.
Do not let direct `fmt::print(stdout, ...)` calls sprawl through parser logic.

### spdlog

Disposition: **hold; pilot only if grounder/engine need real logging levels or
log sinks**.

Audit fit: P-006, P-007, P-014.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Fast C++ logging library with console color, levels, sinks, async mode, and fmt integration. |
| License | MIT. |
| Maintenance | Current project page and package listings show active ecosystem and release stream, including `v1.17.0` in early 2026 package/release metadata. |
| Integration | Header-only or compiled; available through common package managers. |
| Build/packaging | Feasible, but larger surface than parser currently needs. |
| Risk | Logging frameworks can make stdout/stderr discipline worse unless wrapped very tightly. Parser does not yet need async logging, files, rotating sinks, or runtime log configuration. |

Recommendation: do not adopt spdlog for parser first. Reconsider after all
three audits if a shared runtime needs log levels, backtraces, or multiple
sinks.

### tl::expected

Disposition: **pilot for parser result propagation; adopt only if cross-codebase
need recurs**.

Audit fit: P-001, P-003, P-004, P-009, P-015.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Single-header C++11/14/17 implementation of `std::expected` with functional extensions. |
| License | CC0-1.0/public-domain style waiver. |
| Maintenance | GitHub releases page shows latest `v1.3.1`; repository has CI and package availability through vcpkg, Conan, and build2. |
| Integration | Single header, no broad dependency footprint. |
| Build/packaging | Very low packaging cost if vendored with provenance. |
| Risk | Generic expected types can spread into public headers before the error taxonomy is stable. C++23 `std::expected` may eventually replace it. |

Recommendation: first define the chengdu error/status taxonomy. Then pilot
`tl::expected<ParsedProblem, ParserError>` or a tiny local equivalent behind a
parser facade. Avoid committing the whole project to `tl::expected` until the
grounder and engine reports show the same propagation need.

### Boost.Outcome

Disposition: **hold for now**.

Audit fit: P-004 and possible no-exception/status-heavy paths.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Result/outcome types for explicit failure reporting, including no-exception contexts and ABI-conscious use cases. |
| License | Boost Software License. |
| Maintenance | Boost documentation states Outcome joined Boost in 1.70 and standalone Outcome has ABI-stability guarantees from v2.2.3 onward. |
| Integration | Single-header and CMake options exist, but the conceptual surface is larger than parser currently needs. |
| Build/packaging | License fit is strong, but Boost association may create unnecessary perceived weight unless chengdu already adopts Boost elsewhere. |
| Risk | Overkill for a small CLI result taxonomy unless the engine needs richer error-code/status-code domains. |

Recommendation: keep as an engine/shared-runtime option if later audits show
no-exception or ABI-stable result requirements. For parser first, prefer a
small status type or `tl::expected` pilot.

### Catch2

Disposition: **adopt or pilot for C++ unit/fixture tests**.

Audit fit: P-001, P-003, P-005, P-010, P-011.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | C++ unit test framework with natural assertions, sections, and optional micro-benchmarks. |
| License | BSL-1.0. |
| Maintenance | Current GitHub page reports latest `v3.15.0` on 2026-05-12. |
| Integration | v3 is a normal compiled library rather than single-header; v2 remains the single-header legacy line. |
| Build/packaging | Good for tests, not runtime. Requires a test build target and probably a modest build-system improvement. |
| Risk | Does not by itself solve process-level CLI fixture testing; chengdu still needs a wrapper that runs binaries and checks status/stdout/stderr/files. |

Recommendation: adopt as the C++ test framework if Arc03/Arc04 add parser-local
library seams. Pair it with a shell/Python process fixture runner for CLI
contract tests.

### nlohmann/json

Disposition: **hold; pilot only for machine-readable event/status mode**.

Audit fit: future event mode implied by P-006, P-008, and Arc03 process-contract
work.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | JSON for Modern C++, including single-header integration and JSON Lines support in documentation. |
| License | MIT, with bundled third-party notices documented by the project. |
| Maintenance | Current docs show version `v3.12.0`; GitHub release page lists `v3.12.0` released 2025-04-11. |
| Integration | Single required `json.hpp`, C++11, package-manager and CMake integration available. |
| Build/packaging | Runtime dependency surface is acceptable if machine JSON output becomes a committed feature. |
| Risk | Pulling JSON into parser before the event/status contract exists would be premature. |

Recommendation: do not adopt for parser cleanup alone. Revisit in Arc03 if
JSON Lines becomes the selected supervised-process event format.

### Bison C++ skeleton and variants

Disposition: **pilot later, not a third-party dependency adoption**.

Audit fit: P-002, P-004, P-009, P-012, P-015.

Assessment:

| Axis | Evidence |
|------|----------|
| Role | Existing parser-generator family already used by pandaPIparser, with C++ parser class support and variant semantic values. |
| License/build | Uses existing Bison toolchain, so this is a generator-mode migration rather than a new runtime dependency. |
| Maintenance | GNU Bison manual documents C++ parser interface and variant semantic values. |
| Build/packaging | Build-time toolchain impact only, but generated source and scanner interface will change. |
| Risk | Grammar migration can be semantic work. It should not be bundled with CLI/status cleanup. |

Recommendation: keep current Flex/Bison for 0.3.0 unless source changes touch
the grammar anyway. Consider a later parser-modernization slice that moves from
C `%union` raw pointers toward Bison C++ variants after status handling and
tests exist.

### ANTLR, PEGTL, and lexy

Disposition: **hold/reject for 0.3.0 parser work; retain as later parser rewrite
research candidates**.

Audit fit: P-009, P-012, P-015 only if we accept a larger parser rewrite.

Assessment:

| Candidate | Evidence | Parser recommendation |
|-----------|----------|-----------------------|
| ANTLR4 | BSD license, C++ target among 10 supported runtimes, mature grammar ecosystem, but release policy notes minor versions may include breaking changes and parser regeneration is expected. | Hold. Existing `doc/antlrHDDL*.g4` makes it interesting, but runtime and grammar migration are too large for this slice. |
| PEGTL | Header-only parsing expression grammar library; Homebrew formula shows BSL-1.0 license and current stable `4.0.1`. | Hold. Good for a fresh hand-authored parser, but would replace the parser architecture. |
| lexy | Modern C++ parsing DSL with recent releases visible, but a larger paradigm shift. | Hold. Worth a later spike only if Bison modernization is rejected. |

Recommendation: do not replace the parser generator during 0.3.0 infrastructure
cleanup. First create the parser status boundary and test oracle so any later
grammar rewrite has behavior to preserve.

## 5. Recommendation Portfolio

### Adopt

- Standard-library RAII/ownership/path modernization as the default baseline.
- fmt as the formatting primitive inside a chengdu diagnostics/process facade.

### Adopt or Pilot

- CLI11 for the future CLI redesign, gated by golden help/status/stdout/stderr
  tests and comparison with grounder/engine option surfaces.
- Catch2 for C++ unit tests if implementation arcs introduce parser library
  seams or shared runtime helpers.

### Pilot

- `tl::expected` for parser result propagation after the status taxonomy is
  defined.

### Hold

- spdlog until there is evidence all three tools need log levels/sinks beyond a
  small diagnostics facade.
- nlohmann/json until Arc03 chooses a machine-readable event/status format.
- Boost.Outcome until engine/shared-runtime evidence justifies richer status
  domains or no-exception propagation.
- Bison C++ skeleton/variants until tests and status boundaries exist.
- ANTLR, PEGTL, and lexy until a dedicated parser rewrite project is justified.

### Reject for 0.3.0 Parser Cleanup

- A full parser-generator replacement as part of the managed-process cleanup.
  It solves too little of the immediate process-contract problem and carries too
  much semantic migration risk before fixture oracles exist.

## 6. Bubble-up to the Arc

This slice delivered its assigned piece: it created the reusable workflow in
`../arc-plan.md` and applied it to the parser audit.

What the arc plan should preserve for later slices:

- Grounder and engine reports should use the same candidate categories and
  disposition names.
- The final slice should not merely union all adopted candidates. It must choose
  a minimal portfolio that composes across all three binaries.
- CLI11 and fmt are the early leaders, but their final disposition remains
  conditional on grounder/engine evidence.
- Parser-generator replacement is explicitly held, not forgotten.

Silent-drop diff: no requested parser research category was dropped. Final
cross-codebase recommendation is intentionally deferred to slice04 because the
grounder and engine audits are not yet available.

## 7. Ledger Walk

| Row | Final status | Evidence |
|-----|--------------|----------|
| F-1 | done | `../arc-plan.md` section 2 records the reusable workflow. |
| F-2 | done | Section 2 maps parser findings P-001 through P-015 to candidate buckets. |
| F-3 | done | Section 4 records license, maintenance/release evidence, integration, build/packaging, risk, and disposition for adopt/pilot candidates. |
| F-4 | done | Section 5 separates adopt, adopt/pilot, pilot, hold, and reject-for-0.3.0 decisions. |
| F-5 | done | Section 8 records upstream source links used for candidate assessment. |
| F-6 | done | This slice changed planning/research docs only; verification command recorded in `ledger.md`. |

## 8. Sources

- CLI11: <https://github.com/CLIUtils/CLI11>
- fmt: <https://github.com/fmtlib/fmt>
- spdlog: <https://github.com/gabime/spdlog>
- tl::expected: <https://github.com/TartanLlama/expected>
- Boost.Outcome: <https://www.boost.org/doc/libs/latest/libs/outcome/doc/html/index.html>
- Catch2: <https://github.com/catchorg/Catch2>
- nlohmann/json: <https://github.com/nlohmann/json>
- GNU Bison C++ interface: <https://www.gnu.org/software/bison/manual/html_node/C_002b_002b-Bison-Interface.html>
- GNU Bison C++ variants: <https://www.gnu.org/software/bison/manual/html_node/C_002b_002b-Variants.html>
- ANTLR4: <https://github.com/antlr/antlr4>
- ANTLR license: <https://www.antlr.org/license.html>
- PEGTL Homebrew formula: <https://formulae.brew.sh/formula/pegtl>
- lexy release evidence: <https://newreleases.io/project/github/foonathan/lexy/release/v2025.05.0>
