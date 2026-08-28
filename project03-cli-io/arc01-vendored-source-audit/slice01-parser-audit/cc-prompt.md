# CC assignment - chengdu arc01 / slice01 - parser-audit

You are CC, the implementing context for one slice, working in the `chengdu`
repo (`~/lab/billosys/chengdu`) on `release/0.3.x`. This file is your
assignment; read it fully before writing anything.

## Read order (before any work)

1. `AGENTS.md` - standing project contracts, branch/release policy, commit
   footer convention, and wolong fetch-spec constraint.
2. `docs/design-v0.2.0/project-plan.md` and
   `docs/design-v0.2.0/closing-report.md` - the closed fork/release baseline.
3. `docs/design-v0.3.0/project-plan.md` - 0.3.0 DoD, arc roadmap, and project
   ledger.
4. `docs/design-v0.3.0/notes.md` - absorbed pre-planning notes and managed
   process rubric.
5. `docs/design-v0.3.0/arc01-vendored-source-audit/arc-plan.md` - arc
   capability, audit rubric, report paths, and arc ledger.
6. `/Users/oubiwann/lab/billosys/ai-engineering/knowledge/cpp/SKILL.md` -
   C++ audit substrate. Follow its routing: load
   `guides/11-anti-patterns.md` first, then at minimum
   `guides/02-api-design.md`, `guides/03-functions.md`,
   `guides/05-resource-management.md`, `guides/06-error-handling.md`,
   `guides/10-expressions-and-statements.md`,
   `guides/12-project-structure-and-tooling.md`,
   `guides/13-standard-library.md`, and
   `guides/14-c-style-and-modernization.md` for this parser audit.
7. `docs/design-v0.3.0/arc01-vendored-source-audit/slice01-parser-audit/slice-doc.md`
   - goal, scope, constraints, exit criteria.
8. `docs/design-v0.3.0/arc01-vendored-source-audit/slice01-parser-audit/ledger.md`
   - the specification of done. Twelve rows; every one reaches final status
   before this slice advances. Follow the CC protocol in the collaboration
   framework's `LEDGER-DISCIPLINE.md`.

## The work

Produce a complete parser audit report at:

`docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`

Audit `pandaPI/pandaPIparser` as inherited C/C++ source that must become a
maintainable component in a high-quality managed-process toolchain. Cover:

- source/build/test inventory;
- correctness and runtime safety;
- error handling and status/exit behavior;
- stdout/stderr, buffering/flushing, ANSI/color, and TTY/pipe behavior;
- CLI/API surface, including `options.ggo`, help/version behavior, and
  implications for `pandapi-*` binary names;
- build/test coverage and fixture posture;
- maintainability, duplication, global state, coupling, comments/docs, and
  shared-code/header/library candidates;
- modernization candidates, including third-party C/C++ libraries only where
  they solve a real defect class or maintenance burden.

Use the report structure:

1. Executive summary.
2. Audit context: date, branch, project root, audited paths, commands run, and
   the C++ knowledge-pack guides loaded.
3. Findings grouped by category. Each finding has Severity, Location, What's
   wrong, Why it's wrong, C++ Core Guidelines rule ID where applicable, and
   Fix / recommendation.
4. Cross-codebase synthesis inputs.
5. Things I looked for and did not find - at least five clean checks.

## Constraints

- Do not change parser source, scripts, workflows, release assets, or build
  outputs. This is diagnosis only.
- Do not produce generic advice. Every finding cites an exact
  `pandaPI/pandaPIparser/...:<line>` location.
- Map source-quality findings to C++ Core Guidelines rule IDs where
  applicable. Keep chengdu-specific managed-process findings, such as
  stdout/stderr or pipe behavior, clearly labeled when no Core Guidelines rule
  directly applies.
- Do not claim current upstream/GitHub state unless you actually refresh it;
  this audit can proceed from the in-tree fork baseline without network.
- If you run builds/tests, record exact commands and exit status. If sandbox
  or missing dependencies block a useful command, record the blocker.
- Update the ledger Evidence column as the report lands.

## Protocol

- Work against the ledger from the start.
- At close: write `closing-report.md` in this slice directory - per-row walk
  for all 12 rows, then a **Bubble-up to the arc** section answering:
  delivered-as-assigned, what the arc-plan did not anticipate, and the
  silent-drop diff.
- Do not write `cdc-verification.md`; that is CDC's independent file.

## Definition of done

All 12 ledger rows reach final status; the parser audit report exists at the
planned arc-local path; findings are concrete, file/line-cited, and mapped to
C++ Core Guidelines rule IDs where applicable; at least five clean checks are
recorded; synthesis inputs are explicit; and no planner source/build/release
files changed.
