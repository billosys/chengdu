# CC Prompt: arc02 / slice01 - parser-library-research

You are working in `/Users/oubiwann/lab/billosys/chengdu` on
`release/0.3.x`.

Read, in order:

1. `AGENTS.md`
2. `/Users/oubiwann/.agents/skills/collaboration-framework/SKILL.md`
3. `/Users/oubiwann/.agents/skills/collaboration-framework/docs/PROJECT-MANAGEMENT.md`
4. `/Users/oubiwann/.agents/skills/collaboration-framework/templates/LEDGER-DISCIPLINE.md`
5. `/Users/oubiwann/.agents/skills/cpp-guidelines/SKILL.md`
6. `/Users/oubiwann/.agents/skills/cpp-guidelines/guides/11-anti-patterns.md`
7. `/Users/oubiwann/.agents/skills/cpp-guidelines/guides/12-project-structure-and-tooling.md`
8. `/Users/oubiwann/.agents/skills/cpp-guidelines/guides/13-standard-library.md`
9. `docs/design-v0.3.0/project-plan.md`
10. `docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md`
11. `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/slice-doc.md`
12. `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/ledger.md`
13. `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-parser.md`

Task:

Use the arc workflow to assess open source C++ library candidates for
`pandaPIparser`. Start from the parser audit findings, map those findings to
candidate capability buckets, research current candidate status from upstream
sources, and write the results into this slice's `closing-report.md`.

Rules:

- Do not modify `pandaPI/`, `scripts/`, workflows, release assets, build
  outputs, or vendored dependency source.
- Do not recommend a library unless it maps to a concrete parser audit defect
  class or a clearly reusable cross-component concern.
- Prefer C++17 standard-library facilities before adding dependencies.
- Record rejected and held candidates with rationale.
- Keep cross-component decisions provisional until grounder and engine slices
  complete.
