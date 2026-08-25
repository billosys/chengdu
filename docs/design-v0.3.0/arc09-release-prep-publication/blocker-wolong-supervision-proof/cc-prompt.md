# CC Prompt: Arc09 blocker-wolong-supervision-proof

You are implementing the Arc09 unnumbered blocker slice
`blocker-wolong-supervision-proof` in Chengdu.

This is a proof and routing slice. Do the work carefully and resist the
temptation to satisfy Wolong proof with shell pipelines alone. Shell probes are
useful diagnostics; Wolong release proof requires erlexec/argv-list evidence or
a clear external blocker.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md`
4. `docs/design-v0.3.0/arc09-release-prep-publication/wolong-stdin-contract-audit.md`
5. Closing reports and CDC verification docs from the preceding stdin blocker
   slices
6. `/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/chengdu-stdin-contract-blocker.md`
7. The relevant wolong Arc03 docs and runner/probe files

Task:

- Verify Chengdu's fixed stdin contract with the Make-backed stdin fixture
  gate.
- Write
  `docs/design-v0.3.0/arc09-release-prep-publication/blocker-wolong-supervision-proof/wolong-supervision-proof.md`.
- In that report, map every original Wolong blocker claim to one of:
  `fixed`, `accepted-as-rejected`, or `still-blocked`.
- Run direct local Chengdu probes from `./bin/pandapi-*` for:
  - parser domain-from-stdin;
  - parser problem-from-stdin;
  - parser both-stdin rejected;
  - grounder `.htn` stdin to stdout `.sas`;
  - engine `.sas` stdin solved to stdout;
  - engine `.sas` stdin no-plan, preserving exit `2`,
    `status=domain_no_plan`, and `outcome=no_plan`;
  - full solved stdin/stdout/stderr chain;
  - full no-plan stdin/stdout/stderr chain.
- Inspect Wolong's current erlexec runner/API/test state. Determine whether
  Wolong can already consume stdin artifact bytes through argv-list erlexec
  execution with separated stdout/stderr capture.
- If a narrow Wolong proof fixture/test is sufficient, add it in the Wolong
  workspace and keep any Wolong commit separate from the Chengdu commit. Record
  exact Wolong commands and commit/status in the Chengdu proof report.
- If Wolong requires a real follow-on runner/API implementation slice, do not
  hide that inside this Chengdu release slice. Record it as an external blocker
  with owner, affected Wolong slice, evidence, and re-entry condition.
- Update Arc09 status so Slice01 release-readiness-inventory can resume only
  when the wolong blocker is actually cleared. If the blocker moves to Wolong,
  keep Arc09 blocked and say exactly why.

Do not:

- Publish a release or create a tag.
- Convert release docs from preparation wording to published v0.3.0 claims.
- Treat shell command strings or shell pipelines as Wolong supervision proof.
- Add broad Wolong feature work under a Chengdu commit.
- Create `cdc-verification.md`.

Run the ledger gates and include exact wolong/chengdu evidence in the closing
report.

Required Chengdu gates:

```bash
make test-contract-stdio-managed
make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed && make smoke && make smoke-negative
make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check
git diff --check && git diff --cached --check
```

Also run the relevant Wolong gates for whatever Wolong proof path is used, and
record the commands exactly.
