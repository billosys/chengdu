# chengdu arc05 / slice02 - contract-fixture-scaffold - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Turn the accepted Arc03 contract-test matrix and Arc04 fixture substrate into
a repo-level black-box process fixture scaffold that later parser, grounder,
and engine adoption slices can extend.

This slice creates the fixture home, runner shape, baseline probe categories,
and CI-ready local gate needed before any executable behavior changes. It
should capture current inherited-binary behavior as a baseline and make the
future contract expectations legible, but it must not make `pandaPIparser`,
`pandaPIgrounder`, or `pandaPIengine` conform to the new managed-process
contract yet.

## 2. Scope

**In:**

- Add a repo-level contract fixture structure, expected under
  `fixtures/contract/`, with documentation for fixture record shape, ownership,
  comparison policy, safety policy, and baseline-versus-contract semantics.
- Add or update a fixture runner, expected as
  `scripts/run-contract-fixtures.sh`, that can run baseline black-box probes
  against the currently built inherited binaries in `dist/<platform>/`.
- Make the runner produce clear labeled PASS/FAIL output and a nonzero exit
  on any unexpected observation.
- Capture stdout, stderr, exit status, output artifact state, and cleanup
  separately enough that future contract-adoption slices can assert stream
  ownership without regex archaeology.
- Add baseline probes covering current parser, grounder, and engine behavior
  using existing repo fixtures: `fixtures/minimal`, `fixtures/broken-syntax`,
  `fixtures/broken-reference`, and `fixtures/unsolvable`.
- Include at least one pipeline baseline probe that composes parser, grounder,
  and engine using generated temporary artifacts.
- Represent accepted future contract expectations without requiring them to
  pass before the owning binary slice migrates behavior.
- Keep machine-contract checks grounded in the Arc03 matrix: command/argv,
  stdout/stderr role, output artifact, exit/status, final status policy,
  normalization, color/ANSI expectations where applicable, and safety.
- Respect the Arc04 stdin/full-duplex caveat: zero or small stdin probes may
  exist, but stdin-heavy fixtures must remain deferred until runner hardening.
- Add CI coverage for the baseline fixture scaffold if reliable, or record a
  concrete blocker and follow-up in the closing report.
- Keep the fixture scaffold compatible with the existing C++17 runtime and
  CMake/CTest substrate without importing new production dependencies.
- Close the slice ledger and write the CC closing report.

**Out:**

- No parser, grounder, or engine adoption of `pandapi::runtime`.
- No behavior change under `pandaPI/`.
- No canonical `pandapi-parser`, `pandapi-grounder`, or `pandapi-engine`
  wrapper, symlink, copied binary, renamed binary, or release entry point.
- No inherited-name migration, deprecation warning, or compatibility policy
  change.
- No CLI11 import, CLI parser migration, `gengetopt` replacement, or
  per-binary option parser change.
- No final `PANDAPI_STATUS` emission by inherited binaries.
- No attempt to make current inherited stdout/stderr behavior match the future
  managed-process contract.
- No coverage threshold, `llvm-cov` gate, TSan gate, release-package dry-run,
  checksum, manifest, license/NOTICE, test-only dependency exclusion, wolong
  fetch/install/migration, package, publish, or release workflow change.
- No `FetchContent`, `ExternalProject`, package-manager bootstrap, `curl`,
  `git clone`, or network download in the default build/test path.
- No Arc05 closing report.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- the exact fixture directory layout under `fixtures/contract/`;
- whether fixture records are represented as shell-safe metadata, YAML-like
  documentation records, generated C++ fixture objects, or another local form;
- the runner interface and default mode: for example `--baseline`, `--list`,
  `--component`, or explicit fixture IDs;
- which observations are executable in this slice and which are documented
  contract targets for later binary slices;
- how temporary files and generated artifacts are isolated and cleaned up;
- how stdout, stderr, exit status, and artifact state are captured separately;
- whether the runner invokes existing `build-*` scripts or requires callers/CI
  to build first;
- whether CI should run the new baseline fixture gate immediately;
- how the scaffold preserves the Arc03 black-box/Catch2 distinction;
- how the Arc04 stdin/full-duplex caveat is encoded in fixture safety policy.

If a useful fixture requires changing binary behavior, adding canonical
`pandapi-*` commands, changing release shape, or broadening supported surfaces,
stop and record an amendment request instead of widening this slice.

## 4. Constraints

- The slice follows the existing `docs/design-v0.3.0/` layout.
- Fixture work must stay black-box at the executable boundary.
- Catch2 seam tests may be referenced or extended only as helper proof; they
  cannot replace black-box fixture evidence.
- Baseline probes are allowed to assert current inherited behavior, including
  known pre-contract exit codes or mixed streams, but must label that behavior
  as baseline and not as final contract conformance.
- Generated intermediate artifacts must live under `build/`, `dist/`, a temp
  directory, or another ignored generated location.
- The default gate must not require network access or untracked third-party
  tooling.
- Protected release-package and wolong-facing surfaces remain untouched.

## 5. Verification Approach

CC verifies the slice by running the new fixture runner, existing runtime and
binary build gates, existing smoke tests, owned-source format checks, and repo
hygiene checks. The fixture runner should be executable locally after
`./scripts/build-all.sh`; if it self-builds, that behavior must be explicit and
must not hide build failures.

CDC will rerun the ledger commands, inspect the scaffold and runner, verify
that baseline observations are not overclaimed as managed-process conformance,
and confirm that parser, grounder, engine, release packaging, package/publish
scripts, and wolong-facing surfaces did not change.

## 6. Exit Criteria

The ledger reaches final status. The repo has an accepted fixture scaffold and
baseline black-box gate that future Arc05 binary adoption slices can extend
for parser, grounder, and engine contract proof. The scaffold distinguishes
current inherited behavior from future accepted contract expectations, keeps
fixture evidence at the process boundary, and does not change binary behavior
or release shape.
