# CC Prompt - chengdu arc02 / slice03 - engine-library-research

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

Use:

- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/slice-doc.md`
- `docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/ledger.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-engine.md`
- the `cpp-guidelines` skill, especially anti-patterns, standard library,
  resource management, error handling, project/tooling, and performance guides.

Assignment:

1. Read the engine audit as the problem statement. Extract finding IDs and
   defect classes before naming libraries.
2. Apply the reusable Arc02 workflow from `arc-plan.md`.
3. Assess candidate libraries against engine-specific findings, especially:
   CLI parsing/help/version, status/result propagation, diagnostics/formatting,
   stdout/stderr/event output, contract/unit tests, translation child-process
   containment, timeout/signal/resource behavior, model parsing checks,
   optional SAT/BDD/CUDD supported-surface decisions, and performance
   measurement.
4. Prefer C++17 standard-library modernization where it retires defect classes
   without a new dependency.
5. Record license, maintenance/release evidence, integration mode,
   build/packaging impact, migration risk, and disposition for adopted or
   piloted candidates.
6. Write the findings in `closing-report.md`, close the ledger rows, and update
   the Arc02 plan status if the slice changes the arc's next-step picture.

Do not:

- Change engine source, dependency source, scripts, workflows, release assets,
  or build outputs.
- Recommend algorithmic replacement, SAT/BDD enablement, CUDD upgrade, or
  performance-container adoption without a measured re-entry condition.
- Treat translation, SAT, BDD, or interactive mode as part of the 0.3.0
  managed-process contract merely because a library exists.

Before closing, run the ledger verification commands and `git diff --check` for
the Arc02 docs you edited.
