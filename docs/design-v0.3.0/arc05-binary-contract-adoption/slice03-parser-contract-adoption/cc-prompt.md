# CC Prompt: Arc05 Slice03 - parser-contract-adoption

You are CC implementing one ledgered slice in
`/Users/oubiwann/lab/billosys/chengdu` on branch `release/0.3.x`.

## Read First

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc05-binary-contract-adoption/arc-plan.md`
4. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/slice-doc.md`
5. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/ledger.md`
6. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/cdc-verification.md`
7. `docs/design-v0.3.0/arc05-binary-contract-adoption/slice02-contract-fixture-scaffold/closing-report.md`
8. `fixtures/contract/README.md`
9. `fixtures/contract/baseline-records.md`
10. `scripts/run-contract-fixtures.sh`
11. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
12. `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
13. `docs/design-v0.3.0/arc04-shared-runtime-substrate/substrate-integration-readiness.md`
14. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice04-cli-tty-provenance-core/cdc-verification.md`
15. `docs/design-v0.3.0/arc04-shared-runtime-substrate/slice05-fixture-and-seam-test-substrate/cdc-verification.md`
16. `scripts/build-parser.sh`
17. `scripts/build-all.sh`
18. `scripts/smoke-test.sh`
19. `.github/workflows/build-reusable.yml`
20. `Makefile` and `mk/*.mk` if you route verification through make targets.

Also load the C++ guideline substrate for any touched C++ or build-facing code:

- `cpp-guidelines` `11-anti-patterns.md`
- `12-project-structure-and-tooling.md`
- `03-resource-management.md` if you touch file/output ownership or cleanup
- `10-expressions-and-statements.md` if you touch status/exit mapping,
  option parsing, or numeric handling
- `13-standard-library.md` if you add helper code or discuss library boundaries

## Mission

Implement Arc05 Slice03: parser-contract-adoption.

Make supported normal HDDL parsing conform to the accepted managed-process
contract through `pandapi-parser`, while preserving inherited `pandaPIparser`
compatibility for 0.3.0. Extend the Slice02 black-box fixture scaffold so the
parser contract is executable evidence, not just a future target.

This is parser adoption only. Grounder and engine remain baseline-only until
their owning slices.

## Hard Boundaries

- Do not migrate `pandaPIgrounder` or `pandaPIengine`.
- Do not edit `pandaPI/pandaPIgrounder` or `pandaPI/pandaPIengine`.
- Do not change release-package shape, release assets, checksums,
  `release-manifest.txt`, wolong fetch/install/migration behavior, package, or
  publish scripts.
- Do not remove inherited `pandaPIparser`.
- Do not emit a default deprecation warning for inherited `pandaPIparser`.
- Do not make parser verifier/output-helper paths supported product behavior.
  Fence visible helper paths as `legacy_surface`, or document that they are not
  visible through the new supported parser CLI.
- Do not add network fetches, package-manager bootstraps, `curl`, `git clone`,
  `FetchContent`, or `ExternalProject` to the default build/test path.
- Do not create Arc05 `closing-report.md`; arc close waits for all Arc05
  slices.
- Do not create `cdc-verification.md`; CDC owns that file.

If correct parser adoption requires replacing the entire parser CLI, importing
CLI11, changing release packaging, or widening supported surfaces, stop and
ask for an amendment. CLI11 is not forbidden in principle, but this slice may
use it only if the accepted Arc02 gate is fully satisfied inside the slice:
local facade, golden/process tests, and no leakage of third-party APIs into
planner internals.

## Implementation Guidance

Recommended shape:

1. Decide the parser command mechanism.
   - Produce `dist/<platform>/pandapi-parser` and keep
     `dist/<platform>/pandaPIparser`.
   - Prefer the narrowest mechanism that can meet the contract and stay
     auditable: a chengdu-owned adapter/wrapper, a small parser-source entry
     point, copied executable behavior, or another documented choice.

2. Implement parser contract behavior.
   - Support `--output PATH|-`.
   - Support `--status`, `--status=stderr`, and legal `--status=stdout`.
   - Support `--supervised`, `--quiet`, `--verbose`, color/no-color controls,
     `--help`, `--version`, and `--provenance` to the extent required by the
     parser slice ledger.
   - Emit exactly one final `PANDAPI_STATUS` record when status is enabled.
   - Map parser outcomes to accepted statuses and exit codes.
   - Keep stdout artifact output free of human prose and status records.

3. Extend `fixtures/contract/`.
   - Turn the existing future parser target into executable parser contract
     records or add parser-specific contract records.
   - Include canonical and inherited commands, file-backed and stdout-backed
     success, missing input, invalid input, unavailable output, usage errors,
     status stream legality, color policy, and legacy-surface fencing.

4. Extend `scripts/run-contract-fixtures.sh`.
   - Preserve `--baseline`.
   - Add parser contract selection, expected as
     `--contract --component parser`.
   - Support the ledger's parser case IDs or amend the ledger before closing.
   - Parse/validate `PANDAPI_STATUS` fields directly; do not classify by
     diagnostic prose.

5. Add CI coverage if reliable.
   - Prefer routing through the existing make targets if that is where CI now
     lives.
   - If CI cannot run parser contract fixtures reliably, document the blocker
     and re-entry condition precisely in the closing report.

## Required Verification

Run the ledger commands and record outputs in `ledger.md`. At minimum, run:

```bash
./scripts/build-parser.sh
./scripts/run-contract-fixtures.sh --contract --component parser
./scripts/run-contract-fixtures.sh --baseline
./scripts/check-format-owned.sh
./scripts/build-runtime.sh
./scripts/sanitize-runtime.sh
./scripts/build-all.sh
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
git diff --check
git diff --cached --check
```

Also verify protected boundaries:

```bash
git diff --name-only -- pandaPI/pandaPIgrounder pandaPI/pandaPIengine
git diff --name-only -- release release-manifest.txt SHA256SUMS scripts/package-release.sh scripts/publish-release.sh
git diff --cached --name-only -- pandaPI/pandaPIgrounder pandaPI/pandaPIengine
git diff --cached --name-only -- release release-manifest.txt SHA256SUMS scripts/package-release.sh scripts/publish-release.sh
! test -e docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/cdc-verification.md
```

If local `actionlint` is available, run it. If it is unavailable, record that
the existing CI actionlint job remains the workflow syntax gate.

## Close Requirements

Before committing, add:

- `docs/design-v0.3.0/arc05-binary-contract-adoption/slice03-parser-contract-adoption/closing-report.md`

The closing report must include:

- capability verdict;
- implementation decisions;
- verification commands and results;
- per-row ledger walk for all rows;
- silent-drop check comparing scope-as-specified to scope-as-delivered;
- Bubble-up to Arc05, including whether Slice04 grounder-contract-adoption is
  ready to open after independent CDC verification.

Commit the implementation and close set together. Include the required
trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
