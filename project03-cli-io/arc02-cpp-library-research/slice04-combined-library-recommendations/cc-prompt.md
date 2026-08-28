# CC assignment - chengdu arc02 / slice04 - combined-library-recommendations

You are working in `/Users/oubiwann/lab/billosys/chengdu` on `release/0.3.x`.

## Read order

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc02-cpp-library-research/arc-plan.md`
4. `docs/design-v0.3.0/arc02-cpp-library-research/slice04-combined-library-recommendations/slice-doc.md`
5. `docs/design-v0.3.0/arc02-cpp-library-research/slice04-combined-library-recommendations/ledger.md`
6. Arc02 component reports:
   - `docs/design-v0.3.0/arc02-cpp-library-research/slice01-parser-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/slice02-grounder-library-research/closing-report.md`
   - `docs/design-v0.3.0/arc02-cpp-library-research/slice03-engine-library-research/closing-report.md`
7. Arc01 accepted audit reports and CDC verifications.
8. The `cpp-guidelines` skill, especially standard-library, resource
   management, error handling, project/tooling, and performance guidance.

## The work

Produce:

`docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`

This is the final Arc02 library portfolio. It must compare the three component
research reports and make a cross-codebase recommendation for:

- standard-library modernization baseline;
- CLI11;
- fmt;
- tl::expected;
- Catch2;
- nlohmann/json;
- reproc++ and Boost.Process;
- Microsoft GSL;
- Abseil;
- performance containers and google/benchmark;
- parser-generator alternatives;
- optional SAT/BDD/CUDD/H2/cpddl related decisions.

For each adopt or pilot recommendation, refresh current public metadata and
record source links, license, maintenance/release signal, integration mode,
build/packaging impact, migration risk, audit findings addressed, and
disposition.

Include an explicit architecture-impact section. Say how each adopted or
piloted choice affects Arc03 managed-process design, Arc04 shared runtime
substrate, Arc05 per-binary adoption, NOTICE/license work, release packaging,
and rollback/defer options. If the best architecture requires a large change,
recommend it clearly and sequence it carefully.

## Constraints

- Do not vendor dependencies, change source, change scripts/workflows, or alter
  release assets.
- Do not let a library recommendation decide product support for optional
  paths. Arc03 must classify translation, interactive, SAT, BDD, CUDD, H2, and
  cpddl before optional-path dependencies become commitments.
- Do not overfit to performance libraries without representative measurements.
- Do not turn broad foundation libraries into defaults unless the cross-codebase
  evidence beats a smaller focused portfolio.

## Closing

Update `ledger.md`, write `closing-report.md` with a row-by-row walk and
Bubble-up to Arc02, and leave `cdc-verification.md` for CDC.
