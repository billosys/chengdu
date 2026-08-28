# CC Prompt: Arc09 blocker-stdio-contract-fixtures

You are implementing the Arc09 unnumbered blocker slice
`blocker-stdio-contract-fixtures` in Chengdu.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md`
4. `docs/design-v0.3.0/arc09-release-prep-publication/wolong-stdin-contract-audit.md`
5. The closing reports from `blocker-stdio-contract-design` and
   `blocker-stdin-artifact-io`
6. `tests/contract/run`
7. `fixtures/contract/README.md`
8. `docs/managed-process.md`
9. `docs/reference/cli.md`

Task:

- Add Make-backed managed stdin contract fixtures.
- Add end-to-end solved and no-plan supervised pipeline probes.
- Update current public managed-process docs to match the implemented stdin
  input contract.
- Keep CI entries Make-backed.
- Close every ledger row with exact evidence.

Do not:

- Call test harnesses directly from CI.
- Edit wolong.
- Publish a release or create a tag.
- Create `cdc-verification.md`.

Run the ledger gates and explain any use of narrower gates instead of
`make check`.
