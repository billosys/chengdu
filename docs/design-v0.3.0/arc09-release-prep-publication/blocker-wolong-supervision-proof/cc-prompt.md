# CC Prompt: Arc09 blocker-wolong-supervision-proof

You are implementing the Arc09 unnumbered blocker slice
`blocker-wolong-supervision-proof` in Chengdu.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md`
4. `docs/design-v0.3.0/arc09-release-prep-publication/wolong-stdin-contract-audit.md`
5. Closing reports from the preceding stdin blocker slices
6. `/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/chengdu-stdin-contract-blocker.md`
7. The relevant wolong Arc03 docs and runner/probe files

Task:

- Verify Chengdu's fixed stdin contract with the Make-backed stdin fixture
  gate.
- Verify wolong can consume the contract, or record the exact remaining
  external blocker.
- Update Arc09 status so Slice01 release-readiness-inventory can resume only
  when the wolong blocker is actually cleared.

Do not:

- Publish a release or create a tag.
- Convert release docs from preparation wording to published v0.3.0 claims.
- Create `cdc-verification.md`.

Run the ledger gates and include exact wolong/chengdu evidence in the closing
report.
