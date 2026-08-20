# CC Prompt: Arc09 blocker-stdin-artifact-io

You are implementing the Arc09 unnumbered blocker slice
`blocker-stdin-artifact-io` in Chengdu.

Read first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md`
4. `docs/design-v0.3.0/arc09-release-prep-publication/wolong-stdin-contract-audit.md`
5. `docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design/closing-report.md`
6. The accepted contract amendment produced by `blocker-stdio-contract-design`
7. `pandaPI/parser/src/pandapi_parser_native.cpp`
8. `pandaPI/grounder/src/pandapi_grounder_native.cpp`
9. `pandaPI/engine/src/pandapi_engine_native.cpp`
10. `pandaPI/runtime/`

Task:

- Implement the accepted stdin input forms for parser, grounder, and engine.
- Refactor repeated behavior into shared runtime/helper code.
- Preserve existing file-input, stdout artifact, stderr status, no-plan, and
  conflict behavior.
- Add focused helper tests or seam tests where the shared code can be tested
  without relying only on black-box process probes.
- Close the ledger with exact evidence.

Do not:

- Patch each binary independently when shared code would be clearer.
- Change wolong.
- Publish a release or create a tag.
- Create `cdc-verification.md`.

Run the ledger gates and explain any use of narrower gates instead of
`make check`.
