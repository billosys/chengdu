# CC Prompt: Arc05 Slice08 - engine-contract-adoption

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`
4. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/closing-report.md`
5. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/cdc-verification.md`
6. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/closing-report.md`
7. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/cdc-verification.md`
8. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice08-engine-contract-adoption/slice-doc.md`
9. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice08-engine-contract-adoption/ledger.md`
10. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
11. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
12. `pandaPI/pandaPIengine/`
13. `pandapi-runtime/`
14. `fixtures/contract/README.md`
15. `fixtures/contract/baseline-records.md`
16. `tests/contract/run`
17. `tests/smoke/run`
18. `mk/build.mk`, `mk/tests.mk`, `mk/checks.mk`, `mk/release.mk`,
    `mk/help.mk`, `mk/config.mk`, and `Makefile`
19. `tools/provenance/check`, `tools/release/package`, and
    `tools/shared/platform`
20. `README.md`

Also load the C++ guideline substrate for touched C++ or build-facing code:

- `cpp-guidelines` `12-project-structure-and-tooling.md`
- `03-resource-management.md` if you touch file/output ownership or cleanup
- `10-expressions-and-statements.md` if you touch option parsing, status/exit
  mapping, or validation flow
- `13-standard-library.md` if you add helper code or standard-library
  boundaries

## Mission

Implement Arc05 Slice08: engine-contract-adoption.

Parser and grounder are now native canonical binaries. Your mission is to make
`pandapi-engine` the real engine artifact for 0.3.0: native/non-shell, no
active inherited `pandaPIengine` artifact, and passing the accepted engine
managed-process contract for normal search.

This is engine adoption only. Parser and grounder native behavior must stay
green. Arc05 synthesis remains the next slice.

## Hard Boundaries

- Do not change parser or grounder behavior except for necessary regression
  fallout and pipeline command updates.
- Do not restore `pandaPIparser`, `pandaPIparser.legacy`,
  `pandaPIgrounder`, or `pandaPIgrounder.legacy`.
- Do not preserve `pandaPIengine` as an active 0.3.0 compatibility command.
- Do not leave `pandaPIengine` in `dist/<platform>/` after `make build-engine`.
- Do not claim Arc05 is fully closed; Slice09 owns synthesis.
- Do not claim Arc08 release acceptance, publish a release, update checksums,
  or create final release manifests.
- Do not broaden interactive, translation, SAT, BDD, or CUDD into supported
  normal engine behavior.
- Do not add new direct project-script invocations to GitHub Actions; route
  CI behavior through make targets.
- Do not add network fetches, package-manager bootstraps, `curl`,
  `git clone`, `FetchContent`, or `ExternalProject` to default build/test
  paths.
- Do not create Arc05 `closing-report.md`.
- Do not create `cdc-verification.md`; CDC owns that file.

If engine managed behavior still requires active `pandaPIengine`
compatibility, a shell adapter, or broad optional-surface work, stop and ask
for an amendment. Do not mark the canonical/no-compat rows done.

## Implementation Guidance

Recommended sequence:

1. Port engine normal-search behavior into a native managed entrypoint.
   - Preserve accepted engine normal-search semantics from Arc03.
   - Use the existing chengdu runtime substrate where it fits.
   - Keep process-contract policy separated from engine search algorithm code.

2. Update engine build/install behavior.
   - `make build-engine` should leave `dist/<platform>/pandapi-engine`.
   - It should not leave `dist/<platform>/pandaPIengine`.
   - `pandapi-engine` must not be a shell script.

3. Add engine contract fixture support.
   - Add `fixtures/contract/engine-contract-records.md`.
   - Extend `tests/contract/run` for `--contract --component engine`.
   - Add or rename the make gate as `make test-contract-engine-managed`
     unless a better local name is agreed and reflected in the ledger.

4. Fence non-normal engine surfaces.
   - Interactive is legacy.
   - Translation is experimental.
   - SAT is unsupported.
   - BDD/CUDD are future.
   - If a surface is not visible through the new CLI, record that with a
     re-entry condition instead of inventing a synthetic fixture.

5. Update active command surfaces.
   - Update `tests/smoke/run`, baseline/pipeline records, make targets,
     provenance checks, package helpers, and active README references from
     `pandaPIengine` to `pandapi-engine` where they describe the 0.3.0
     supported command.
   - Avoid release-publication overclaim; package helper updates may prepare
     local packaging but do not prove Arc08 release acceptance.

6. Close the ledger.
   - Update every ledger row with evidence.
   - Write `closing-report.md` with capability verdict, implementation
     decisions, verification, ledger walk, silent-drop check, and Bubble-up to
     Arc05.

## Required Verification

Run the ledger commands. At minimum:

```bash
make build-engine
make test-contract-engine-managed
make test-contract-parser-managed
make test-contract-grounder-managed
make smoke
make smoke-negative
make test
make actionlint
git diff --check
git diff --cached --check
```

Also prove:

- `dist/<platform>/pandapi-engine` exists and is executable.
- `dist/<platform>/pandapi-engine` is not a shell script.
- `dist/<platform>/pandaPIengine` does not exist.
- Parser and grounder native artifacts and contract behavior remain intact.
- Active package/provenance/smoke/fixture surfaces use canonical
  `pandapi-engine`.

## Commit Scope

Expected touched areas include:

- `pandaPI/pandaPIengine/` and/or chengdu-owned engine entrypoint/build files
- `pandapi-runtime/` only if engine contract support needs accepted runtime
  helpers
- `mk/`
- `tests/contract/run`
- `tests/smoke/run`
- `tools/provenance/check`
- `tools/release/package`
- `fixtures/contract/engine-contract-records.md`
- `fixtures/contract/baseline-records.md`
- engine-specific active docs, including `fixtures/contract/README.md` and
  narrowly scoped `README.md` edits
- this slice's `ledger.md` and `closing-report.md`

Do not touch release publication outputs, checksums, or final release
manifests.

Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
