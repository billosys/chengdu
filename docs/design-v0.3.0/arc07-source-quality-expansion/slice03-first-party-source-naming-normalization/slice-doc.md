# Arc07 Slice03: first-party-source-naming-normalization

Status: open
Opened: 2026-08-12

## Goal

Normalize first-party maintained C/C++ file and directory names across parser,
grounder, engine, and runtime to the accepted lower snake case policy before
Arc07 quality gates begin depending on stable selector paths, compile
databases, coverage maps, static-analysis output, and warning inventories.

This slice is intentionally about source names and build references, not code
behavior. It turns the current organic mix of TitleCase, camelCase,
kebab-case, acronyms, and lower snake case into a predictable first-party
source surface.

## Background

Slice01 classified the source tree and Slice02 added Make-backed selector,
compile database, and profile/source-map surfaces. During Slice02 CDC
verification, CDC and the operator identified one more normalization step that
should happen before quality gate scaffolding: file and directory naming.

The accepted local policy is:

- first-party maintained C/C++ source and source directories use lower snake
  case;
- acronyms are normalized as words where the rename is clear, such as
  `hddl_writer`, `h2_mutexes`, or `int_data_structures`;
- generated output, vendored third-party source, dependency-internal source,
  and third-party generated code are excluded unless this slice explicitly
  accepts a path;
- conventional build filenames such as `CMakeLists.txt` are exempt;
- public binary names and managed-process behavior are unchanged.

This aligns with the C++ Core Guidelines preference for consistency and
underscore-style names, while applying it at the repository file/directory
surface where Chengdu already uses lower snake case in runtime.

## In Scope

- Add Make-backed source naming report/check targets:
  `make source-quality-naming-report` and
  `make source-quality-naming-check`.
- Document both new targets in `make help`.
- Produce a durable source naming report, expected at:
  `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`.
- Inventory first-party maintained C/C++ file and directory names under:
  `pandaPI/parser`, `pandaPI/grounder`, `pandaPI/engine`, and
  `pandaPI/runtime`.
- Rename non-conforming first-party maintained C/C++ files and directories to
  lower snake case using `git mv`, including macOS-safe handling for
  case-only changes.
- Update includes, Makefiles, CMake files, source-quality selector helpers,
  tests, fixtures, compile database/report generation, and documentation
  references required by the renamed paths.
- Preserve source-class exclusions for generated outputs, copied build
  artifacts, vendored third-party source, dependency-internal source, and
  third-party generated code.
- Re-run source-quality reports and compile database targets after renaming.

## Out Of Scope

- No algorithm, data-structure, parser, grounder, engine, or runtime behavior
  changes except include/build path updates required by renames.
- No public `pandapi-*` CLI behavior changes.
- No generated Flex/Bison/gengetopt output rewrites unless a generated file is
  already tracked and a path update is required.
- No renames under `pandaPI/grounder/cpddl`,
  `pandaPI/grounder/h2-fd-preprocessor`, or
  `pandaPI/engine/src/symbolic_search/cudd-3.0.0`.
- No README/tutorial, Arc08, Arc09, release-package, publication, or wolong
  migration edits.
- No new external dependency.
- No formatting sweep or warning/static-analysis cleanup unrelated to path
  references.
- No enforcement of coverage floors, warning fail budgets, or expanded
  parser/grounder/engine static-analysis thresholds.

## Required Target Shape

Slice03 should leave these Make targets callable and documented by
`make help`:

- `make source-quality-naming-report`
- `make source-quality-naming-check`

The report target should write a durable report under the Arc07 directory and
may also write generated helper evidence under `build/source-quality/<platform>/`.
The check target should fail when first-party maintained C/C++ file or
directory basenames violate the accepted lower snake case policy.

The target implementation should reuse Slice02 source-class selectors or the
same Make-invoked helper surface where practical. Do not add a new `scripts/`
entrypoint.

## Verification Approach

Primary verification is Make-backed:

- `make help`
- `make source-quality-naming-report`
- `make source-quality-naming-check`
- `make source-quality-surface`
- `make source-quality-profile-map`
- `make compile-db-first-party`

Component behavior/build verification should include:

- `make build-parser`
- `make build-grounder`
- `make build-engine`
- `make test-contract-parser-managed`
- `make test-contract-grounder-managed`
- `make test-contract-engine-managed`

Regression checks should include:

- `make safety-checks`
- `make actionlint`
- `make static-analysis`
- `make format-check`
- `make build`
- `make test`
- `make provenance-check`
- `git diff --check`
- `git diff --cached --check`

If `make check` is practical and does not introduce the known macOS provenance
mutation/race, run it. If a narrower set is used, the closing report must say
exactly what ran and why.

## Exit Criteria

- A written naming policy and rename/exclusion map exists in
  `source-naming-normalization.md`.
- `make source-quality-naming-report` and
  `make source-quality-naming-check` are documented and pass.
- First-party maintained parser, grounder, engine, and runtime C/C++ file and
  directory basenames match lower snake case, with conventional build-file and
  source-class exclusions documented.
- All changed includes/build references point to the renamed paths.
- Slice02 source-quality surfaces still run after the rename.
- Parser, grounder, engine, smoke, contract, static-analysis, format, safety,
  and provenance gates still pass.
- No public behavior, release, CI workflow, README/tutorial, Arc08, or Arc09
  surface expands.
- Ledger rows close with evidence, and CDC can reproduce every target.
