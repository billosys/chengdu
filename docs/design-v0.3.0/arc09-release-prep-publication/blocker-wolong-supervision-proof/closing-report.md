# Arc09 Blocker Closing Report: wolong-supervision-proof

Date: 2026-08-25

Status: proof and routing proposed done; external Wolong blocker remains.

## Summary

This slice proves that Chengdu's accepted stdin/stdout/stderr contract is fixed
and fixture-backed, and that raw Wolong erlexec argv-list supervision can drive
a real Chengdu binary with stdin bytes, stdout capture, stderr status, EOF, and
monitoring.

It also finds that Wolong's current public `wolong-exec:run/3` API cannot yet
consume the contract because it does not expose stdin bytes or EOF. Arc09
Slice01 `release-readiness-inventory` therefore remains blocked by Wolong Arc03
`slice02-stdio-runner`.

Detailed proof report:
[`wolong-supervision-proof.md`](wolong-supervision-proof.md).

## Ledger Walk

- **F-1 - done.** `make test-contract-stdio-managed` passed with
  `SUMMARY: 187 passed, 0 failed (contract, stdio, macos-arm64)`.
- **F-2 - done.** `wolong-supervision-proof.md` maps the original
  `chengdu-stdin-contract-blocker` claims to `fixed`,
  `accepted-as-rejected`, or `still-blocked`.
- **F-3 - done.** Direct Chengdu probes passed for `pandapi-parser`,
  `pandapi-grounder`, `pandapi-engine`, parser two-stdin rejection,
  `domain_no_plan`, `no_plan`, and stderr `PANDAPI_STATUS`.
- **F-4 - done.** Raw erlexec argv-list stdin proof passed:
  `reason=normal stdout_bytes=446 stderr=PANDAPI_STATUS ... status=ok`.
  The report records the remaining `wolong-exec:run/3` stdin/EOF gap as an
  external blocker with re-entry condition.
- **F-5 - done.** Wolong workspace status stayed clean:
  `## main...origin/main`; no Wolong edits and no Wolong commit.
- **F-6 - done.** Arc09 and project plans keep Slice01
  `release-readiness-inventory` blocked by Wolong Arc03
  `slice02-stdio-runner`.
- **F-7 - done.** Existing Chengdu managed contract and smoke gates passed:
  parser `298 passed`, grounder `269 passed`, engine `312 passed`, pipeline
  `129 passed`, positive smoke `3 passed`, negative smoke `4 passed`.
- **F-8 - done.** Release-quality wayfinding passed:
  `make safety-checks && make actionlint && make static-analysis && make
  format-check && make test && make provenance-check`.
- **F-9 - done.** Staged boundary check passed with no `release/` or
  `.github/release-notes.md.in` changes.
- **F-10 - done.** No `cdc-verification.md` exists for this slice.
- **F-11 - done.** `git diff --check && git diff --cached --check` passed
  after staging.

## Bubble-Up To Arc09

Chengdu's stdin contract blockers are closed from the Chengdu side, including
design, implementation, fixtures/docs, and this proof/routing slice.

Arc09 remains active and blocked before Slice01. The next required work is a
Wolong-owned Arc03 `slice02-stdio-runner` implementation, or equivalent
runner/API slice, that proves stdin artifact delivery through Wolong's public
runner/API rather than raw erlexec calls alone.

## Silent-Drop Diff

Delivered:

- Chengdu fixture gate evidence.
- Direct Chengdu probe evidence.
- Raw Wolong erlexec argv-list proof.
- Wolong runner/API inspection.
- Arc09 and project-plan blocker-status updates.
- Ledger closure evidence.

Intentionally not delivered:

- No Chengdu product-code changes.
- No Wolong workspace edits.
- No Wolong commit.
- No release publication, tag, package output, release notes, or public
  published-version claims.
- No `cdc-verification.md`.

## Remaining Blockers Before Slice01

Arc09 Slice01 can resume only after Wolong Arc03 `slice02-stdio-runner`, or an
equivalent Wolong-owned implementation slice, exposes and verifies stdin
artifact bytes plus EOF through Wolong's public runner/API with separated
stdout/stderr capture and managed-process status/exit preservation.
