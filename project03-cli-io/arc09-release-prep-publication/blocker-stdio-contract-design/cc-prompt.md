# CC Prompt: Arc09 blocker-stdio-contract-design

You are implementing the Arc09 unnumbered blocker slice
`blocker-stdio-contract-design` in Chengdu.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md`
4. `docs/design-v0.3.0/arc09-release-prep-publication/wolong-stdin-contract-audit.md`
5. `/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/chengdu-stdin-contract-blocker.md`
6. `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
7. `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
8. `docs/managed-process.md`
9. `docs/reference/cli.md`

Task:

- Decide and document the supported stdin input contract for
  `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`.
- Treat wolong's supervised stdin/stdout/stderr pipeline as release-blocking.
- Prefer a shared runtime/helper design for repeated stdin materialization and
  path-role handling.
- Keep the parser two-input role semantics explicit.
- Close the blocker ledger only with concrete evidence.

Do not:

- Implement product code in this slice.
- Update public release install wording.
- Edit wolong.
- Create `cdc-verification.md`.
- Publish a release or create a tag.

Expected verification:

- Every F-row command in `ledger.md`.
- `make safety-checks && make actionlint`
- `git diff --check`
- `git diff --cached --check`
- A staged boundary check showing only Arc09 planning/evidence docs and
  project-plan/arc-plan status updates.
