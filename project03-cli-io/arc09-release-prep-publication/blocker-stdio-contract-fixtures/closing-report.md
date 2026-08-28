# Arc09 Blocker Closing Report: stdio-contract-fixtures

Date: 2026-08-25

Status: implementation proposed done.

## Summary

This blocker turns the accepted and implemented stdin artifact IO contract into
durable Make-backed process fixtures and current public documentation.

The delivered behavior is:

- `make test-contract-stdio-managed` runs a dedicated managed stdio contract
  component.
- Parser fixtures cover domain from stdin, problem from stdin, and rejected
  both-inputs-from-stdin.
- Grounder fixtures cover `.htn` input from stdin while writing `.sas` to
  stdout and status to stderr.
- Engine fixtures cover `.sas` input from stdin for solved and no-plan
  outcomes.
- Full supervised stdio pipeline fixtures cover solved and no-plan chains
  using stdout artifacts as the byte source for the next process's stdin.
- Public managed-process and CLI docs describe supported stdin forms,
  unsupported parser `- -`, stdout artifact ownership, stderr status, and
  logical stdin status fields.
- Existing CI jobs inherit the gate through `make test`, which is already used
  by `make ci-linux` and `make ci-macos`; no workflow direct harness calls were
  added.

## Ledger Walk

- **F-1 - done.** `tests/contract/run` and
  `fixtures/contract/stdio-contract-records.md` contain
  `stdio-parser-domain-stdin`, `stdio-parser-problem-stdin`, and
  `stdio-parser-both-stdin-unsupported`. The exact grep Verify passed.
- **F-2 - done.** `stdio-grounder-stdin-stdout-status` covers grounder stdin
  `.htn` input with `--output -`, stdout artifact ownership, stderr final
  status, and logical stdin fields. The exact grep Verify passed.
- **F-3 - done.** `stdio-engine-stdin-solved`,
  `stdio-engine-stdin-no-plan`, `stdio-pipeline-supervised-solved`, and
  `stdio-pipeline-supervised-no-plan` cover engine stdin `.sas` solved and
  no-plan behavior. The exact grep Verify passed.
- **F-4 - done.** `make test-contract-stdio-managed` passed with 187 passed,
  0 failed on macos-arm64.
- **F-5 - done.** `make actionlint && rg -n --
  "test-contract-stdio-managed|make " .github/workflows` passed. No workflow
  file was edited; the CI target path inherits the stdio gate through
  `make test`.
- **F-6 - done.** `docs/managed-process.md` and `docs/reference/cli.md`
  document supported stdin forms, parser `- -` rejection, stdout artifact
  ownership, `--status=stderr`, and logical stdin status fields. The exact
  grep Verify passed.
- **F-7 - done.** The exact existing contract/smoke chain completed
  successfully:
  `make test-contract-parser-managed && make test-contract-grounder-managed &&
  make test-contract-engine-managed && make test-contract-pipeline-managed &&
  make smoke && make smoke-negative`.
- **F-8 - done.** The exact release-quality wayfinding chain passed after a
  shellcheck-discovered local-assignment fix in the new runner cases:
  `make safety-checks && make actionlint && make static-analysis &&
  make format-check && make test && make provenance-check`.
- **F-9 - done.** Staged boundary check passed; no `release/` state was
  changed and no wolong workspace files were edited.
- **F-10 - done.** No `cdc-verification.md` exists for this implementation
  close.
- **F-11 - done.** `git diff --check && git diff --cached --check` passed
  after staging.

## Bubble-Up To Arc09

Arc09 now has fixture-proven Chengdu behavior for the stdin/stdout/stderr
contract that blocked wolong's supervised process pipeline.

Slice01 release-readiness-inventory remains blocked only by
`blocker-wolong-supervision-proof`, which should verify wolong against the
same supported contract or record a concrete remaining external blocker. The
Slice01 inventory ledger and prompt now require `make test-contract-stdio-managed`
as part of the release-quality gate inventory.

## Silent-Drop Diff

Scope as specified versus scope as delivered:

- Delivered accepted parser stdin forms and rejected parser both-stdin.
- Delivered grounder stdin `.htn` fixture coverage with stdout artifact and
  stderr status separation.
- Delivered engine stdin `.sas` solved and no-plan fixture coverage.
- Delivered full solved and no-plan supervised stdio pipeline fixtures.
- Delivered Make target and CI inheritance through `make test`.
- Delivered public managed-process and CLI doc updates.
- Delivered Arc09 and project-plan status updates.

Intentionally not delivered:

- No wolong workspace edits.
- No release publication, tags, package output, checksums, or manifest edits.
- No product behavior changes beyond the shell harness fixture implementation.
- No `cdc-verification.md`.
