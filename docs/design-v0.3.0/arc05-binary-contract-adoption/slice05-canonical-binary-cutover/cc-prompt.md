# CC Prompt: Arc05 Slice05 - canonical-binary-cutover

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`
4. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice05-canonical-binary-cutover/slice-doc.md`
5. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice05-canonical-binary-cutover/ledger.md`
6. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/closing-report.md`
7. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/cdc-verification.md`
8. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice04-grounder-contract-adoption/closing-report.md`
9. `fixtures/contract/README.md`
10. `fixtures/contract/baseline-records.md`
11. `fixtures/contract/parser-contract-records.md`
12. `fixtures/contract/grounder-contract-records.md`
13. `tests/contract/run`
14. `tests/smoke/run`
15. `mk/build.mk`, `mk/tests.mk`, `mk/checks.mk`, `mk/release.mk`,
    `mk/help.mk`, `mk/ci.mk`, and `Makefile`
16. `tools/provenance/check`, `tools/release/package`, and
    `tools/shared/platform`
17. `README.md`

Also load the C++ guideline substrate for any touched C++ or build-facing
code:

- `cpp-guidelines` `12-project-structure-and-tooling.md`
- `03-resource-management.md` if you touch file/output ownership or cleanup
- `10-expressions-and-statements.md` if you touch status/exit mapping,
  option parsing, or numeric handling
- `13-standard-library.md` if you add helper code or discuss library
  boundaries

## Mission

Implement Arc05 Slice05: canonical-binary-cutover.

Cut parser and grounder over from transition shims and inherited-name release
surface to canonical `pandapi-parser` and `pandapi-grounder` artifacts. The
operator has explicitly decided that `v0.3.0` is not an inherited
`pandaPI*` compatibility release. `v0.2.0` is the transition release; `v0.3.0`
may break old command names and old process behavior.

This slice must remove parser/grounder old-name artifacts and shims only after
the canonical artifacts pass the accepted parser and grounder contract tests.
Engine remains inherited as `pandaPIengine` until the next binary adoption
slice.

## Hard Boundaries

- Do not migrate `pandaPIengine` or introduce `pandapi-engine`.
- Do not preserve `pandaPIparser` or `pandaPIgrounder` as active 0.3.0
  compatibility commands.
- Do not leave `pandaPIparser.legacy` or `pandaPIgrounder.legacy` sidecars in
  `dist/<platform>/`.
- Do not keep the parser/grounder transition shell shims after canonical
  artifacts satisfy tests.
- Do not claim Arc08 release acceptance, publish a release, update checksums,
  or create final release manifests.
- Do not add new direct project-script invocations to GitHub Actions; route
  all CI behavior through make targets.
- Do not broaden optional inherited surfaces such as H2, `cpddl`/FAM, engine
  interactive, translation, SAT, BDD, or CUDD.
- Do not add network fetches, package-manager bootstraps, `curl`, `git clone`,
  `FetchContent`, or `ExternalProject` to default build/test paths.
- Do not create Arc05 `closing-report.md`.
- Do not create `cdc-verification.md`; CDC owns that file.

If parser or grounder cannot satisfy the accepted managed-process contract
without the transition shims, stop and ask for an amendment. Do not keep a shim
and mark the no-shim rows done.

## Implementation Guidance

Recommended sequence:

1. Build/install canonical parser and grounder artifacts.
   - `make build-parser` should leave `dist/<platform>/pandapi-parser`.
   - `make build-grounder` should leave `dist/<platform>/pandapi-grounder`.
   - Do not leave `pandaPIparser`, `pandaPIgrounder`, or `.legacy` parser /
     grounder sidecars in `dist/<platform>/`.

2. Remove transition shim files.
   - Delete `scripts/pandapi-parser-adapter.sh`.
   - Delete `scripts/pandapi-grounder-adapter.sh`.
   - Delete `scripts/install-grounder-adapter.sh`.
   - Remove all build/test invocations of those scripts.

3. Move active tests and records to canonical names.
   - Update `tests/contract/run` and `tests/smoke/run`.
   - Update `fixtures/contract/*.md`.
   - Remove inherited parser/grounder contract cases, provenance assertions,
     "remains executable" assertions, and `.legacy` passthrough assertions.
   - Keep engine references as `pandaPIengine` until the engine slice.

4. Move Make/tooling surfaces to canonical names.
   - Update `mk/build.mk`, `mk/tests.mk`, `mk/checks.mk`, `mk/release.mk`,
     `mk/help.mk`, and any other make include that names parser/grounder
     artifacts.
   - Update `tools/provenance/check`, `tools/release/package`, and macOS
     minimum-OS recording logic.
   - Keep package helper changes local/dry-run oriented; do not publish.

5. Move active docs to canonical names.
   - Update `README.md` examples and artifact lists.
   - Update current fixture docs if they still teach old parser/grounder
     names as active 0.3.0 surface.
   - Do not rewrite historical closed reports unless a line is actively used
     as current instruction.

6. Close the ledger.
   - Update every ledger row with evidence.
   - Write `closing-report.md` with capability verdict, implementation
     decisions, verification, ledger walk, silent-drop check, and Bubble-up to
     Arc05.

## Required Verification

Run the ledger commands. Prefer make targets for local gates that correspond
to CI. At minimum, run:

```bash
make build-parser
make build-grounder
make test-contract-parser-managed
make test-contract-grounder-managed
make test
make check
git diff --check
git diff --cached --check
```

Also run the old-name absence checks from the ledger against:

- `dist/<platform>/`
- `scripts/`
- `mk/`, `tests/`, `tools/`, `fixtures/contract/`
- `README.md`
- `AGENTS.md`
- `docs/design-v0.3.0/project-plan.md`
- `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`

If `make package-release TAG=v0.3.0-slice05-dryrun` cannot run locally, record
the exact blocker and verify the package helper diff directly. Do not publish.

## Commit Scope

Expected touched areas include:

- `mk/`
- `tests/contract/run`
- `tests/smoke/run`
- `tools/provenance/check`
- `tools/release/package`
- `README.md`
- `fixtures/contract/`
- parser/grounder source or build files needed to make canonical artifacts
  pass without shims
- deletion of parser/grounder shim scripts under `scripts/`
- this slice's `ledger.md` and `closing-report.md`

Do not touch engine source except for unavoidable build/test naming context
that keeps `pandaPIengine` baseline behavior intact. Do not touch release
publication outputs, checksums, or final release manifests.

Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
