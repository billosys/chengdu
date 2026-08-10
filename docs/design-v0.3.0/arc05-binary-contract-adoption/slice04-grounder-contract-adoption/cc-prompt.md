# CC Prompt: Arc05 Slice04 - grounder-contract-adoption

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`
4. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice04-grounder-contract-adoption/slice-doc.md`
5. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice04-grounder-contract-adoption/ledger.md`
6. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/cdc-verification.md`
7. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/closing-report.md`
8. `fixtures/contract/README.md`
9. `fixtures/contract/baseline-records.md`
10. `fixtures/contract/parser-contract-records.md`
11. `scripts/run-contract-fixtures.sh`
12. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
13. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
14. `docs/design-v0.3.0/arc04-shared-runtime-substrate/substrate-integration-readiness.md`
15. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md`
16. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md`
17. `docs/design-v0.3.0/arc01-vendored-source-audit/audit-results-pandapi-grounder.md`
18. `scripts/build-grounder.sh`
19. `scripts/build-all.sh`
20. `mk/tests.mk`, `mk/build.mk`, `mk/ci.mk`, `mk/help.mk`, and `Makefile`
21. `.github/workflows/build-reusable.yml`

Also load the C++ guideline substrate for any touched C++ or build-facing
code:

- `cpp-guidelines` `11-anti-patterns.md`
- `12-project-structure-and-tooling.md`
- `03-resource-management.md` if you touch file/output ownership or cleanup
- `10-expressions-and-statements.md` if you touch status/exit mapping,
  option parsing, or numeric handling
- `13-standard-library.md` if you add helper code or discuss library
  boundaries

## Mission

Implement Arc05 Slice04: grounder-contract-adoption.

Make supported normal `.htn` grounding conform to the accepted
managed-process contract through `pandapi-grounder`, while preserving inherited
`pandaPIgrounder` compatibility for 0.3.0. Extend the Slice02/Slice03
black-box fixture scaffold so the grounder contract is executable evidence and
the parser contract remains accepted.

This is grounder adoption only. Engine remains baseline-only until Slice05.

## Hard Boundaries

- Do not migrate `pandaPIengine`.
- Do not edit `pandaPI/pandaPIparser` or `pandaPI/pandaPIengine`.
- Do not change accepted parser contract behavior except for test harness
  fallout required to keep Slice03 gates green.
- Do not change release-package shape, release assets, checksums,
  `release-manifest.txt`, wolong fetch/install/migration behavior, package, or
  publish scripts.
- Do not remove inherited `pandaPIgrounder`.
- Do not emit a default deprecation warning for inherited `pandaPIgrounder`.
- Do not make H2 supported product behavior. Fence visible H2 paths as
  `experimental_surface`, or document that they are not visible through the new
  supported grounder CLI.
- Do not make `cpddl`/FAM supported product behavior. Fence visible paths as
  `legacy_surface`, or document that they are not visible through the new
  supported grounder CLI.
- Do not add network fetches, package-manager bootstraps, `curl`, `git clone`,
  `FetchContent`, or `ExternalProject` to the default build/test path.
- Do not add new direct project-script invocations to GitHub Actions. New CI /
  workflow behavior must invoke make targets.
- Do not create Arc05 `closing-report.md`; arc close waits for all Arc05
  slices.
- Do not create `cdc-verification.md`; CDC owns that file.

If correct grounder adoption requires broad `cpddl` or H2 audits, replacing
the entire grounder CLI, importing CLI11, changing release packaging, or
widening supported surfaces, stop and ask for an amendment. CLI11 is not
forbidden in principle, but this slice may use it only if the accepted Arc02
gate is fully satisfied inside the slice: local facade, golden/process tests,
and no leakage of third-party APIs into planner internals.

## Implementation Guidance

Recommended shape:

1. Decide the grounder command mechanism.
   - Produce `dist/<platform>/pandapi-grounder` and keep
     `dist/<platform>/pandaPIgrounder`.
   - Prefer the narrowest mechanism that can meet the contract and stay
     auditable: a chengdu-owned adapter/wrapper, a small grounder-source entry
     point, copied executable behavior, or another documented choice.
   - If you wrap the inherited binary, consider preserving raw inherited
     behavior as `pandaPIgrounder.legacy`, analogous to Slice03 parser, but do
     not make raw legacy behavior the supported managed contract.

2. Implement grounder contract behavior.
   - Support `--output PATH|-`.
   - Support `--status`, `--status=stderr`, and legal `--status=stdout`.
   - Support `--supervised`, `--quiet`, `--verbose`, color/no-color controls,
     `--help`, `--version`, and `--provenance` to the extent required by the
     grounder slice ledger.
   - Emit exactly one final `PANDAPI_STATUS` record when status is enabled.
   - Map grounder outcomes to accepted statuses and exit codes.
   - Keep stdout artifact output free of progress, statistics, ANSI, status
     records, and human prose.

3. Fence non-supported grounder surfaces.
   - Treat normal `.htn` grounding as the only supported grounder surface.
   - H2 remains experimental. Visible H2 invocation maps to
     `experimental_surface` / 32, or the close report documents that H2 is not
     visible through the new supported CLI.
   - `cpddl`/FAM remains legacy advanced behavior. Visible invocation maps to
     `legacy_surface` / 31, or the close report documents that it is not
     visible through the new supported CLI.
   - Do not let default help or provenance imply that those surfaces are
     supported normal behavior.

4. Extend `fixtures/contract/`.
   - Add `fixtures/contract/grounder-contract-records.md`.
   - Include canonical and inherited commands, file-backed and stdout-backed
     success, missing input, invalid input, unavailable output, usage errors,
     status stream legality, color policy, H2 fencing, and `cpddl`/FAM
     fencing.
   - Assess `domain_no_plan`: add a fixture if reachable through accepted
     grounder semantics, or document why it is not reachable in this slice and
     what input/evidence would re-enter it.

5. Extend `scripts/run-contract-fixtures.sh`.
   - Preserve `--baseline`.
   - Preserve `--contract --component parser`.
   - Add grounder contract selection, expected as
     `--contract --component grounder`.
   - Support the ledger's grounder case IDs or amend the ledger before
     closing.
   - Parse/validate `PANDAPI_STATUS` fields directly; do not classify by
     diagnostic prose.

6. Extend make targets and CI reachability.
   - Add a managed grounder contract target, expected as
     `test-contract-grounder-managed`, or amend the ledger with the chosen
     target name before closing.
   - Wire the managed grounder contract into `make test` so
     `make ci-linux` and `make ci-macos` reach it through the existing CI
     entrypoints.
   - If workflow changes are necessary, workflow `run:` steps must invoke make
     targets, not project scripts.

## Required Verification

Run the ledger commands and record outputs in `ledger.md`. Prefer make targets
for local gates that correspond to CI. At minimum, run:

```bash
make build-grounder
./scripts/run-contract-fixtures.sh --contract --component grounder
make test-contract-parser-managed
./scripts/run-contract-fixtures.sh --baseline
make format-check
make test-runtime-sanitize
make test
make provenance-check
make actionlint
git diff --check
git diff --cached --check
```

Also verify protected boundaries:

```bash
git diff --name-only -- pandaPI/pandaPIparser pandaPI/pandaPIengine
git diff --name-only -- release release-manifest.txt SHA256SUMS scripts/package-release.sh scripts/publish-release.sh
git diff --cached --name-only -- pandaPI/pandaPIparser pandaPI/pandaPIengine
git diff --cached --name-only -- release release-manifest.txt SHA256SUMS scripts/package-release.sh scripts/publish-release.sh
! test -e docs/design-v0.3.0/arc05-binary-contract-adoption/slice04-grounder-contract-adoption/cdc-verification.md
```

If local `actionlint` is unavailable, record that the existing make/CI
actionlint gate remains the workflow syntax gate.

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc05-binary-contract-adoption/slice04-grounder-contract-adoption/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc05, including whether Slice05 engine-contract-adoption is
  ready to open after independent CDC verification.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
