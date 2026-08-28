# CDC Verification - Arc06 Slice01 - fixture-gap-inventory

Verifier: CDC
Verified on: 2026-08-11
Implementation commit reviewed: `91cc50b6`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc06 Slice01 is CDC-verified.

The slice delivered the intended inventory and gate map without changing
parser, grounder, engine, release, README, Arc07, or Arc08 surfaces. The new
`make test-contract-list-managed` target exposes the managed fixture inventory
through the Make public interface, and the Arc06 inventory report gives Slice02
a concrete first expansion batch rather than a vague "add more tests" target.

## Source and Scope Checks

- `git show --stat --oneline --name-status 91cc50b6` confirmed the commit is
  limited to `mk/help.mk`, `mk/tests.mk`, `tests/contract/run`, the Arc06
  inventory report, the Slice01 ledger, and the Slice01 closing report.
- `git show --format=fuller 91cc50b6` confirmed both required co-author
  trailers are present.
- Commit-file boundary checks confirmed no touched paths under
  `pandaPI/parser`, `pandaPI/grounder`, or `pandaPI/engine`.
- Commit-file boundary checks confirmed no touched release package/publish,
  checksum, manifest, wolong, README, Arc07, or Arc08 surfaces.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | `make help` lists `test-contract-list` and `test-contract-list-managed`; both list targets passed. Baseline listed 9 fixtures; managed listed 40 fixtures. |
| F-2 | reproduced | `fixture-gap-inventory.md` exists and records baseline and managed fixture counts by parser, grounder, engine, pipeline, and total. |
| F-3 | reproduced | Inventory maps current fixture IDs to Arc03 matrix categories including CLI usage, input unavailable, input invalid, output unavailable, status stream, color, and surface fencing. |
| F-4 | reproduced | Missing/deferred/superseded obligations are routed to Arc06 Slice02-Slice06, Arc07, Arc08, or no-op with rationale and re-entry conditions. |
| F-5 | reproduced | Inventory maps Make and GitHub Actions gates to proof classes and records local, CI, release, and deferred status. |
| F-6 | reproduced | Active Arc06 docs use `pandaPI/runtime` and do not introduce active references to the retired runtime source path. |
| F-7 | reproduced | `HEAD` commit-file boundary check found no parser, grounder, or engine product-source edits. |
| F-8 | reproduced | `HEAD` commit-file boundary check found no release package/publish, release workflow, README, Arc07, or Arc08 edits. |
| F-9 | reproduced | `make actionlint`, `make static-analysis`, `make format-check`, `make safety-checks`, `git diff --check`, and `git diff --cached --check` passed. |
| F-10 | reproduced | `make test` and `make provenance-check` passed. |

## Commands Reproduced

```text
set -e; make help | rg -n "test-contract-list"; make test-contract-list; make test-contract-list-managed
inventory grep checks for rows F-2 through F-6
commit-file boundary checks for rows F-7 and F-8
make actionlint
make static-analysis
make format-check
make safety-checks
git diff --check
git diff --cached --check
make test
make provenance-check
```

Observed results:

- Baseline fixture list: 9 fixtures.
- Managed fixture list: 40 fixtures.
- `make test`: passed, including runtime CTest, baseline contract fixtures,
  managed parser/grounder/engine fixtures, positive smoke, and negative smoke.
- `make provenance-check`: passed with all three components verified against
  `vendor.env` and Git state on `macos-arm64`.
- Quality/static/workflow/whitespace gates passed.

## Code Review Notes

No blocking findings remain.

The inventory is appropriately scoped: it distinguishes baseline, managed
component, smoke, optional corpus, and absent managed pipeline coverage. It
also routes original Arc03 inherited-name compatibility probes to no-op under
the later Arc05 no-compatibility policy, which prevents old matrix text from
silently re-opening retired behavior.

CDC adapted two staged-diff ledger checks for post-commit review: F-7 and F-8
were verified against `HEAD`'s committed file list rather than an empty
post-commit index. The substantive boundary claim reproduced.

## Bubble-up to Arc06

Slice02 should open as `process-fixture-expansion` using the inventory's first
batch:

1. Managed pipeline fixture for a file-backed parse-ground-solve chain with
   `--status=stderr` on each component.
2. Pairwise artifact-composition fixtures for parser stdout captured into
   grounder input and grounder stdout captured into engine input.
3. Positive `--supervised` fixtures for parser, grounder, and engine.
4. Deterministic output-finalization failure fixtures where the harness can
   prove them portably, otherwise an explicit deferral.
5. TTY/color positive fixtures only if the harness can make TTY/no-TTY
   observations stable.

No Arc06 sequencing change is required. The inventory confirms the planned
order: expand representative process fixtures before coverage, static-analysis,
binary sanitizer, and TSan decisions.
