# CC assignment - chengdu arc01 / slice04 - audit-synthesis

You are CC, the implementing context for one slice, working in the `chengdu`
repo (`~/lab/billosys/chengdu`) on `release/0.3.x`. This file is your
assignment; read it fully before writing anything.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc01-vendored-source-audit/arc-plan.md`
4. `docs/design-v0.3.0/arc01-vendored-source-audit/slice04-audit-synthesis/slice-doc.md`
5. `docs/design-v0.3.0/arc01-vendored-source-audit/slice04-audit-synthesis/ledger.md`
6. The three accepted audit reports:
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
   - `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
7. The three CDC verification reports under slice01, slice02, and slice03.
8. `docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md` and the three
   Arc02 component closing reports as context, not as final library decisions.
9. The `cpp-guidelines` skill, especially anti-patterns, standard-library,
   resource-management, error-handling, project/tooling, and performance
   guides.

## The work

Produce:

`docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`

This is not a summary. It is the cross-codebase diagnosis that turns three
audits into architecture inputs. Compare parser, grounder, and engine across:

- error/status propagation and deep process exits;
- stdout/stderr/TTY/color/buffering behavior;
- CLI identity, help/version behavior, and `pandapi-*` naming;
- resource, timeout, signal, and child-process behavior;
- ownership/lifetime, raw pointers, globals, generated-code risks, and header
  hygiene;
- build/test/fixture gaps and warning policy;
- duplicate code and shared runtime/header/library candidates;
- optional or legacy supported-surface questions.

Include an architecture decision matrix with at least three paths:

1. Conservative wrapper/contract repair.
2. Moderate shared runtime substrate plus per-binary adoption.
3. Big-change architecture, if warranted by the evidence.

For each path, name benefits, costs, risks, prerequisites, implementation
sequence, and re-entry conditions. Be brave about the recommendation, but make
the plan boringly checkable.

## Constraints

- Diagnosis and synthesis only. Do not change planner source, scripts,
  workflows, release assets, or build outputs.
- Cross-codebase claims cite audit finding IDs and report sections.
- Do not make final library choices owned by Arc02 slice04, and do not write
  the final managed-process contract owned by Arc03.
- Preserve wolong's frozen fetch/install shape unless routing an explicit
  future operator decision.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc01, and leave `cdc-verification.md` for CDC.
