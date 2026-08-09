# chengdu arc01 - vendored-source-audit - arc plan

> Plan-of-record for arc01, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../project-plan.md`](../project-plan.md). This arc begins from the closed
> `v0.2.0` in-tree fork baseline.

## 1. Capability statement

Roadmap line: *Complete per-repo audits for parser, grounder, and engine, then
synthesize cross-codebase recommendations for cleanup, shared code, process
contracts, and dependency choices.*

Expanded: this arc produces the evidence base for the 0.3.0 redesign before
any source cleanup or API/process-contract implementation starts. Each of the
three primary vendored repos receives its own complete audit report:
`pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine`. Those reports use
`ai-engineering/knowledge/cpp/SKILL.md` and its C++ Core Guidelines-derived
guide set as the C++ audit substrate, while retaining chengdu's
managed-process rubric for process behavior. They inspect source quality,
correctness risks, error handling, stdio/TTY behavior, CLI surface,
build/test posture, resource behavior, process-manager suitability,
duplication, and candidates for shared C/C++ substrate. The final synthesis
turns the three reports into prioritized inputs for Arc02's C++ library
research, Arc03's managed-process design, and the later implementation arcs.

This arc is diagnosis only. It may create audit reports and planning docs; it
does not modify planner source, scripts, workflows, release assets, or
packaging.

## 2. Slice breakdown

| Slice | Slug | Scope (one line) | Load-bearing for |
|-------|------|------------------|------------------|
| slice01 | `parser-audit` | Audit `pandaPI/pandaPIparser` and produce a complete report with findings, clean checks, and recommendations. | slice04 synthesis; Arc02 library research; Arc03 CLI/process contract |
| slice02 | `grounder-audit` | Audit `pandaPI/pandaPIgrounder`, including grounder-owned integration with `cpddl` and `h2-fd-preprocessor`, and produce a complete report. | slice04 synthesis; Arc02 library research; Arc03/Arc04 integration design |
| slice03 | `engine-audit` | Audit `pandaPI/pandaPIengine` and produce a complete report focused on managed-process behavior, resource safety, status semantics, and source quality. | slice04 synthesis; Arc02 library research; Arc03/Arc05 engine adoption |
| slice04 | `audit-synthesis` | Synthesize the three reports into a prioritized cross-codebase recommendation document covering cleanup, shared code, third-party libraries, binary naming, and implementation sequencing. | Arc02 recommendation synthesis; Arc03 detailed planning |

Slice01 (`parser-audit`) and slice02 (`grounder-audit`) are closed and
CDC-verified. Slice03 (`engine-audit`) is also closed and CDC-verified. The
accepted reports live at the arc root as durable design evidence:
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
and
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
and
`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`.
Slice04 (`audit-synthesis`) is closed and CDC-verified. All Arc01 slices are
closed and the arc-level closing report has accepted the composition:
`docs/design-v0.3.0/arc01-vendored-source-audit/closing-report.md`.

## 3. Dependencies

**Consumes:** closed 0.2.0 fork baseline; `docs/design-v0.2.0/project-plan.md`;
`docs/design-v0.2.0/closing-report.md`; `docs/design-v0.3.0/notes.md`;
`/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/SKILL.md` and
topic guides under `knowledge/cpp/guides/`; root `README.md`; `AGENTS.md`;
source under `pandaPI/`; build scripts under `scripts/`; CI/release workflows
as needed for build/test/report findings.

**Leaves for arc02/arc03:** three concrete audit reports, one synthesis report,
a clear set of dependency/design inputs and non-inputs, and a recommended
ordering for library-research and managed-process contract decisions.

## 4. Audit rubric

Every per-repo audit must start from the C++ knowledge pack:
`SKILL.md`, `guides/11-anti-patterns.md`, then the component-relevant topic
guides. Findings should cite C++ Core Guidelines rule IDs where applicable
and should still name chengdu-specific process-contract findings when the
guidelines do not cover the concern.

Every per-repo audit must cover at least:

- **Correctness and runtime safety:** crashes, unchecked assumptions,
  undefined behavior risk, resource leaks, path/cwd assumptions, malformed
  input behavior, and memory/runtime blow-up risks visible from code or tests.
- **Error handling:** return-code policy, exception/use of `exit`, swallowed
  errors, inconsistent status propagation, user-facing diagnostic quality, and
  library-vs-main boundary discipline.
- **stdio and TTY behavior:** stdout data vs diagnostics, stderr usage,
  buffering/flushing under pipes, ANSI/color detection and suppression, log
  verbosity, and interleaving risks.
- **CLI/API surface:** `options.ggo`, help/version behavior, flag semantics,
  positional arguments, non-interactive reachability, and machine-readable
  mode implications.
- **Build and tests:** makefiles, generated code assumptions, gengetopt/flex/
  bison interactions, local/CI gate coverage, fixture quality, negative tests,
  and install/packaging impact.
- **Maintainability:** duplicate code, global state, cross-component patterns,
  naming/layout drift, comments/docs, coupling, public/private boundaries, and
  likely shared-library/header candidates.
- **Modernization candidates:** C++ standard level, candidate libraries,
  dependency risk, license/build consequences, and whether each candidate
  replaces real complexity rather than adding fashion.

Each report must include things checked and not found, so negative evidence is
visible and the report does not become only a list of defects.

## 5. Report paths

Arc01 uses arc-local reports as durable diagnostic artifacts:

| Report | Planned path |
|--------|--------------|
| Parser audit | `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md` |
| Grounder audit | `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md` |
| Engine audit | `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md` |
| Synthesis | `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md` |

The slice close reports link to these files and walk their ledgers. Accepted
recommendations become inputs in Arc02 library research and Arc03 design;
unaccepted or deferred recommendations remain visible in the synthesis with
re-entry conditions.

## 6. Open questions and risks

- **OQ1 - C/C++ audit substrate resolved.** Arc01 uses
  `/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/SKILL.md`,
  starting with `guides/11-anti-patterns.md` and then topic guides relevant
  to each component. The main remaining risk is over-applying modern C++ rules
  without respecting current build/toolchain constraints; each report must
  inspect local compiler flags, makefiles, CI, and legacy constraints before
  recommending modernization.
- **OQ2 - audit scope creep.** Grounder includes former submodule directories.
  Slice02 must distinguish grounder-owned integration findings from deep
  dependency-internal findings, and recommend any separate dependency audit
  explicitly.
- **OQ3 - no implementation during audit.** Fixes discovered by the reports
  belong to later arcs unless the operator explicitly opens a separate repair
  slice. Audit credibility depends on not mixing diagnosis and cleanup.
- **OQ4 - report comparability.** The three reports must use the same category
  vocabulary and severity scale so the synthesis can prioritize across repos.

## 7. Arc ledger

Composition rows verifying the capability; open here and close per-row in
this arc's `closing-report.md`.

| Row | Criterion | Target strength |
|-----|-----------|-----------------|
| A1 | Parser, grounder, and engine each have a complete audit report at the planned arc-local path, with exact file/line citations for findings, C++ Core Guidelines rule IDs where applicable, and at least five clean checks per report. | reproduced |
| A2 | Every per-repo report covers the required audit rubric categories or explicitly marks a category no-op with evidence-backed rationale. | reproduced |
| A3 | The grounder audit distinguishes grounder-owned source/integration findings from dependency-internal findings in `cpddl`/`h2-fd-preprocessor`, with any deeper dependency audit routed as a recommendation. | reproduced |
| A4 | The synthesis report compares all three audits and produces prioritized recommendations for error handling, stdio/process behavior, CLI/binary naming, duplicate-code removal, shared code/header/library candidates, third-party dependencies, and implementation sequencing. | reproduced |
| A5 | Arc02 and Arc03 can be planned from the synthesis without silent drops: every High/Blocker finding is either accepted into a later arc/slice candidate, deferred with re-entry condition, or rejected/no-op with rationale. | reproduced |
| A6 | No planner source, build scripts, workflows, or release assets are changed by this audit arc except for explicitly planned docs/report artifacts. | reproduced |

## 8. Version history

- **v1.9 - 2026-08-09.** Closed Arc01 after the arc-level composition check
  accepted the parser, grounder, engine, and synthesis reports. Surfaced by:
  operator request after Arc02 close. Why: Arc03 now has verified audit
  synthesis and dependency-recommendation inputs.
- **v1.8 - 2026-08-09.** Marked slice04 audit-synthesis CDC-verified and
  surfaced that Arc01 is ready for arc-level close. Surfaced by: slice04 CDC
  verification. Why: all four Arc01 slices are now verified, and the synthesis
  report provides the composition evidence for the arc close.
- **v1.7 - 2026-08-09.** Opened slice04 audit-synthesis with explicit
  architecture-option and supported-surface criteria. Surfaced by: operator
  guidance after Arc01 audits and Arc02 component research landed. Why: the
  synthesis must be willing to recommend a larger shared architecture if that
  is the best path, while preserving sequencing and verification discipline.
- **v1.6 - 2026-08-09.** Marked slice03 engine audit CDC-verified and surfaced
  that slice04 audit synthesis is now the remaining Arc01 work. Surfaced by:
  slice03 CDC verification. Why: parser, grounder, and engine reports are all
  accepted design evidence for synthesis, Arc02 library research, and Arc03
  managed-process design.
- **v1.5 - 2026-08-09.** Marked slice02 grounder audit CDC-verified and
  surfaced its Arc02/Arc03 inputs. Surfaced by: slice02 CDC verification. Why:
  the grounder audit delivered its assigned report, dependency-boundary notes,
  and synthesis inputs without changing planner source.
- **v1.4 - 2026-08-09.** Updated downstream arc references after insertion of
  `arc02-cpp-library-research` in the project roadmap. Surfaced by: operator
  direction to create a dedicated library-research arc. Why: Arc01 audit
  outputs now feed both Arc02 dependency research and Arc03 managed-process
  design, rather than a single former Arc02.
- **v1.3 - 2026-08-09.** Promoted the parser audit report into the arc
  directory, changed all remaining Arc01 report homes to arc-local paths, and
  opened slice02 grounder-audit plus slice03 engine-audit. Surfaced by:
  operator direction after parser CDC verification. Why: the per-repo reports
  are durable design evidence for synthesis and Arc02, not transient workbench
  scratch artifacts.
- **v1.2 - 2026-08-09.** Marked slice01 parser audit CDC-verified and made
  slice02 grounder audit the next unopened slice. No arc breakdown change.
  Surfaced by: slice01 CDC verification. Why: the parser audit delivered its
  assigned report and confirmed the planned audit sequencing.
- **v1.1 - 2026-08-09.** Added the new `ai-engineering/knowledge/cpp` Core
  Guidelines skill as mandatory audit substrate for all Arc01 reports, while
  preserving chengdu's managed-process rubric for process-contract findings.
  Surfaced by: operator research and new C++ knowledge-pack creation. Why:
  Arc01 audits should use a durable C++ rule corpus and cite guideline IDs
  where applicable before source diagnosis begins.
- **v1.0 - 2026-08-09.** Initial Arc01 plan and slice01 opening. Source:
  `../project-plan.md` v1.0 and operator direction that 0.3.0 starts with
  full per-repo audits plus synthesis before redesign.
