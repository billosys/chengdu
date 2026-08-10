# CC Prompt: Arc05 Slice07 - grounder-native-contract-cutover

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`
4. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice05-canonical-binary-cutover/closing-report.md`
5. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice05-canonical-binary-cutover/cdc-verification.md`
6. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/closing-report.md`
7. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice06-parser-native-contract-cutover/cdc-verification.md`
8. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/slice-doc.md`
9. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice07-grounder-native-contract-cutover/ledger.md`
10. `scripts/pandapi-grounder-adapter.sh`
11. `scripts/install-grounder-adapter.sh`
12. `pandaPI/pandaPIgrounder/`
13. `pandapi-runtime/`
14. `fixtures/contract/README.md`
15. `fixtures/contract/grounder-contract-records.md`
16. `fixtures/contract/baseline-records.md`
17. `tests/contract/run`
18. `tests/smoke/run`
19. `mk/build.mk`, `mk/tests.mk`, `mk/checks.mk`, `mk/release.mk`,
    `mk/help.mk`, `mk/config.mk`, and `Makefile`
20. `tools/provenance/check`, `tools/release/package`, and
    `tools/shared/platform`
21. `README.md`

Also load the C++ guideline substrate for touched C++ or build-facing code:

- `cpp-guidelines` `12-project-structure-and-tooling.md`
- `03-resource-management.md` if you touch file/output ownership or cleanup
- `10-expressions-and-statements.md` if you touch option parsing, status/exit
  mapping, or validation flow
- `13-standard-library.md` if you add helper code or standard-library
  boundaries

## Mission

Implement Arc05 Slice07: grounder-native-contract-cutover.

Slice05 proved that the current canonical grounder was a Bash adapter over a
raw inherited sidecar, and the raw sidecar rejected managed-process options.
Slice06 has already made `pandapi-parser` native and deleted parser old-name
artifacts. Your mission is to make `pandapi-grounder` the real grounder
artifact for 0.3.0: native/non-shell, no grounder old-name artifact, no
grounder `.legacy` sidecar, and still passing the accepted grounder
managed-process contract.

This is grounder-only. Parser native behavior must stay green. Engine remains
inherited until engine adoption.

## Hard Boundaries

- Do not change parser behavior except for necessary regression-test fallout.
- Do not restore `pandaPIparser` or `pandaPIparser.legacy`.
- Do not migrate `pandaPIengine` or introduce `pandapi-engine`.
- Do not preserve `pandaPIgrounder` as an active 0.3.0 compatibility command.
- Do not leave `pandaPIgrounder.legacy` in `dist/<platform>/`.
- Do not keep `scripts/pandapi-grounder-adapter.sh` or
  `scripts/install-grounder-adapter.sh` after grounder native contract
  fixtures pass.
- Do not claim full Arc05 binary adoption; engine remains pending.
- Do not claim Arc08 release acceptance, publish a release, update checksums,
  or create final release manifests.
- Do not broaden H2 or `cpddl`/FAM into supported normal grounder behavior.
- Do not add new direct project-script invocations to GitHub Actions; route
  CI behavior through make targets.
- Do not add network fetches, package-manager bootstraps, `curl`,
  `git clone`, `FetchContent`, or `ExternalProject` to default build/test
  paths.
- Do not create Arc05 `closing-report.md`.
- Do not create `cdc-verification.md`; CDC owns that file.

If grounder managed behavior still requires the Bash adapter or a grounder
`.legacy` sidecar, stop and ask for an amendment. Do not mark the no-shim rows
done.

## Implementation Guidance

Recommended sequence:

1. Port grounder adapter behavior into native code.
   - Preserve accepted grounder contract semantics from the fixture records.
   - Use the existing chengdu runtime substrate where it fits.
   - Keep process-contract policy separated from grounder algorithm code.

2. Update grounder build/install behavior.
   - `make build-grounder` should leave `dist/<platform>/pandapi-grounder`.
   - It should not leave `pandaPIgrounder` or `pandaPIgrounder.legacy`.
   - `pandapi-grounder` must not be a shell script.

3. Remove grounder transition shim dependency.
   - Delete `scripts/pandapi-grounder-adapter.sh`.
   - Delete `scripts/install-grounder-adapter.sh`.
   - Remove all active build/test/tooling references to those scripts.
   - Do not restore parser shims.

4. Update grounder contract records and tooling.
   - Update `fixtures/contract/grounder-contract-records.md`.
   - Update grounder-relevant parts of `fixtures/contract/README.md`,
     `tests/contract/run`, `tests/smoke/run`, `mk/*`,
     `tools/provenance/check`, and `tools/release/package`.
   - Avoid broad README cleanup that would imply engine adoption.

5. Close the ledger.
   - Update every ledger row with evidence.
   - Write `closing-report.md` with capability verdict, implementation
     decisions, verification, ledger walk, silent-drop check, and Bubble-up to
     Arc05.

## Required Verification

Run the ledger commands. At minimum:

```bash
make build-grounder
make test-contract-grounder-managed
make test-contract-parser-managed
make smoke
make smoke-negative
make test
git diff --check
git diff --cached --check
```

Also prove:

- `dist/<platform>/pandapi-grounder` exists and is executable.
- `dist/<platform>/pandapi-grounder` is not a shell script.
- `dist/<platform>/pandaPIgrounder` does not exist.
- `dist/<platform>/pandaPIgrounder.legacy` does not exist.
- `scripts/pandapi-grounder-adapter.sh` is deleted.
- `scripts/install-grounder-adapter.sh` is deleted.
- Parser native artifacts and contract behavior remain intact.

## Commit Scope

Expected touched areas include:

- `pandaPI/pandaPIgrounder/` and/or chengdu-owned grounder entrypoint/build
  files
- `pandapi-runtime/` only if grounder contract support needs accepted runtime
  helpers
- `mk/`
- `tests/contract/run`
- `tests/smoke/run`
- `tools/provenance/check`
- `tools/release/package`
- `fixtures/contract/grounder-contract-records.md`
- grounder-specific active docs, including `fixtures/contract/README.md` and
  narrowly scoped `README.md` edits when they do not claim engine completion
- deletion of `scripts/pandapi-grounder-adapter.sh`
- deletion of `scripts/install-grounder-adapter.sh`
- this slice's `ledger.md` and `closing-report.md`

Do not touch engine source, release publication outputs, checksums, or final
release manifests.

Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
