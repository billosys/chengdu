# CDC Verification - Arc06 Slice07 - tsan-and-ci-synthesis

Verifier: CDC
Verified on: 2026-08-12
Implementation commit reviewed: `a4e52a4f`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc06 Slice07 is CDC-verified.

CDC agrees with the Slice07 ThreadSanitizer disposition. No TSan target should
be added yet because the accepted managed-process workload does not exercise a
representative multithreaded chengdu-owned race-detection surface. The current
supported observations are process, stream, status, artifact, timeout-harness,
and supervised-mode behaviors; the only clear in-tree thread primitives are in
the engine SAT path, which the managed contract fences as
`unsupported_feature`.

Arc06 can close. The remaining dynamic-analysis work is explicitly routed to
future re-entry conditions instead of represented as a symbolic CI badge.

## Source and Scope Checks

- `git show --stat --oneline --name-status a4e52a4f` confirmed the commit
  scope is limited to Arc06 planning, ledger, inventory, and closing-report
  documents.
- `git show --format=fuller --no-patch a4e52a4f` confirmed both required
  co-author trailers are present.
- `make help | rg -n "tsan|ThreadSanitizer|race"` produced no match, matching
  the documented no-target decision.
- Boundary review of `a4e52a4f` found no README, release publication, release
  workflow, Arc07, or Arc08 path changes.
- CDC made one follow-up documentation cleanup during closure: the Arc06
  fixture/gate inventory no longer lists `make sanitize-runtime` twice.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Workload probe found `fork`, `pipe`, `waitpid`, and `sleep_for` in `pandaPI/runtime/src/process_fixture.cpp`, shell harness timeouts in `tests/contract/run`, and `--supervised` fixture cases for parser, grounder, engine, and pipeline. |
| F-2 | reproduced | The TSan deferral is documented with concrete reason, owner, and Re-entry in the Slice07 closing report and Arc06 inventory. |
| F-3 | reproduced | No TSan target appears in `make help`; the no-target decision is documented in the Slice07 closing report. |
| F-4 | reproduced | The final gate map separates always-on CI, local/pre-commit, scheduled/pre-release, targeted local, and deferred gates. |
| F-5 | reproduced | The fixture/gate inventory records `make coverage`, `make static-analysis-cpp`, `make warning-inventory`, `make test-binary-sanitize`, runtime sanitizer, Linux LSan re-entry, and TSan deferral. |
| F-6 | reproduced | `make test-runtime-sanitize` passed 5/0 and `make test-binary-sanitize` passed 1008/0. |
| F-7 | reproduced | `make static-analysis`, `make format-check`, `make test`, `make provenance-check`, `git diff --check`, and `git diff --cached --check` passed. |
| F-8 | reproduced | Arc07 handoff inputs are named in Slice07 and Arc06 closing reports; no Arc07 files were edited. |
| F-9 | reproduced | Arc08 release-prep gates are named in Slice07 and Arc06 closing reports; no release package/publication surfaces were edited. |
| F-10 | reproduced | Commit boundary review confirmed no release, README, release workflow, Arc07, or Arc08 changes. |
| F-11 | reproduced | Arc06 closing report exists, has no NO-GO items, and is updated by CDC closure to mark Arc06 closed and CDC-verified. |

## Commands Reproduced

```text
git status --short --branch
git show --stat --oneline --name-status a4e52a4f
git show --format=fuller --no-patch a4e52a4f
rg -n -- "std::thread|pthread_create|pthread_mutex|fork\(|pipe\(|waitpid|sleep_for|--supervised|timeout" pandaPI/runtime/src pandaPI/runtime/tests tests/contract/run pandaPI/engine/src
make help
rg -n -- "no TSan target|TSan.*deferred" docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis
rg -n -- "TSan|ThreadSanitizer|deferred|Re-entry|make coverage|make static-analysis-cpp|make warning-inventory|make test-binary-sanitize|make sanitize-runtime|make test-runtime-sanitize" docs/design-v0.3.0/arc06-ci-and-test-hardening/fixture-gap-inventory.md docs/design-v0.3.0/arc06-ci-and-test-hardening/slice07-tsan-and-ci-synthesis docs/design-v0.3.0/arc06-ci-and-test-hardening/closing-report.md
git show --name-only --format= a4e52a4f -- README.md release tools/release .github/workflows/release.yml docs/design-v0.3.0/arc07-pandapi-tutorial-docs docs/design-v0.3.0/arc08-release-prep-publication
make coverage
make warning-inventory
make test-runtime-sanitize
make test-binary-sanitize
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

Observed results:

- Coverage: runtime CTest 5/0; included runtime line coverage 74.09%.
- Warning inventory: 56 warning lines.
- Runtime sanitizer: 5/0.
- Binary sanitizer managed fixtures: 1008/0.
- `make test`: runtime 5/0, baseline contract 38/0, managed parser 298/0,
  managed grounder 269/0, managed engine 312/0, managed pipeline 129/0,
  positive smoke 3/0, negative smoke 4/0.
- Provenance check passed for all three normal dist components on
  `macos-arm64`.

## Review Notes

No blocking findings remain.

CDC accepts the `make check` disposition. The Slice07 change is docs/synthesis
only; CDC reproduced the underlying behavior, safety, sanitizer, coverage,
warning, provenance, and diff gates directly. Avoiding full `make check` also
avoids adding the known macOS provenance mutation surface to a docs-only close.

The TSan deferral is valid because it is tied to observable workload shape:
add TSan only after accepted supported behavior includes multithreaded
chengdu-owned process observation, a concurrent stream-draining child-process
adapter, or promoted supported planner concurrency. TSan must remain separate
from ASan/UBSan when it re-enters.

## Bubble-up to Arc06

Slice07 closes Arc06 rows A9 and A12. It also provides the final gate placement
and handoff synthesis required to close Arc06 without claiming release
publication, wolong migration, public tutorial/docs, Linux LSan, adoption-seam
coverage/static analysis, output-finalization fixtures, or binary-owned
timeout/resource fixtures.
