# Arc08 Slice09 Ledger: readme-entrypoint-refresh

| Row | Criterion | Verify command | Strength | Parent | Status | Evidence | Notes |
|-----|-----------|----------------|----------|--------|--------|----------|-------|
| F-1 | The README presents a current public overview of chengdu/pandaPI 0.3.0 without stale three-repo framing. | `rg -n -- "chengdu|pandaPI|HTN|HDDL|pandaPI/|pandapi-parser|pandapi-grounder|pandapi-engine|managed-process|Make" README.md` | correctness-grade | A7/A10 | open | | Product name `pandaPI` is allowed; inherited command names are not. |
| F-2 | The README clearly distinguishes currently published release-install examples from 0.3.0 release-preparation proof owned by Arc09. | `rg -n -- "v0\\.2\\.0|0\\.3\\.0|release preparation|published|not.*published|Arc09|checksums|manifest|wolong|when.*released|until.*release" README.md` | correctness-grade | A7/A9 | open | | Public prose may say "release preparation"; avoid depending on arc names in README if a reader-facing phrase is clearer. |
| F-3 | The source-build path documents `make readme-verbatim`, `./bin/pandapi-parser`, `./bin/pandapi-grounder`, and `./bin/pandapi-engine`. | `rg -n -- "make readme-verbatim|\\.\\/bin\\/pandapi-parser|\\.\\/bin\\/pandapi-grounder|\\.\\/bin\\/pandapi-engine|make smoke|make smoke-negative|make test" README.md` | serious | A3/A7/A8 | open | | |
| F-4 | The README links to the complete public tutorial sequence. | `rg -n -- "docs/tutorial/01-htn-hddl-onboarding\\.md|docs/tutorial/02-first-project-workflow\\.md|docs/tutorial/03-model-a-feature-as-htn\\.md|docs/tutorial/04-write-hddl-files\\.md" README.md` | correctness-grade | A1/A2/A3/A11/A12 | open | | |
| F-5 | The README links to public integration, reference, migration, and architecture docs. | `rg -n -- "docs/managed-process\\.md|docs/reference/cli\\.md|docs/migration\\.md|docs/architecture\\.md" README.md` | correctness-grade | A4/A5/A6/A7 | open | | |
| F-6 | Optional `docs/index.md` is either created with verified wayfinding links or explicitly deferred in the closing report. | `test -f docs/index.md && rg -n -- "Tutorial|Managed Process|CLI|Migration|Architecture|docs/tutorial/01-htn-hddl-onboarding\\.md|docs/reference/cli\\.md" docs/index.md || rg -n -- "docs/index\\.md|defer|not created|not needed|Slice10" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice09-readme-entrypoint-refresh/closing-report.md` | serious | A1/A7/A10 | open | | This row is intentionally a decision row. |
| F-7 | Public README/docs links point to existing files. | `set -e; rg -o -- "\\]\\(([^)]+)\\)" README.md docs/index.md 2>/dev/null | sed -E 's/.*\\(([^)]+)\\).*/\\1/' | while read -r path; do case "$path" in http*|"#"*|"mailto:"*) continue ;; esac; target="${path%%#*}"; [ -z "$target" ] && continue; test -e "$target" || test -e "docs/${target#./}"; done` | serious | A10 | open | | If `docs/index.md` does not exist, the command still checks README. |
| F-8 | `make readme-verbatim` passes after the README refresh. | `make readme-verbatim` | reproduced | A7/A8 | open | | This is the load-bearing README drift gate. |
| F-9 | Smoke gates named by the README pass. | `make smoke && make smoke-negative` | serious | A3/A7/A8 | open | | |
| F-10 | `make help` confirms README-named Make targets are current entrypoints. | `make help && rg -n -- "readme-verbatim|smoke|smoke-negative|test|enable-dev-strict|dev-strict-status" mk Makefile` | serious | A7/A8 | open | | |
| F-11 | Public README/docs prose avoids internal project-management vocabulary. | `set -e; paths="README.md"; test ! -f docs/index.md || paths="$paths docs/index.md"; if rg -n -- "Arc[0-9]|Slice[0-9]|CDC|CC|ledger|closing report|project-plan|arc-plan" $paths; then exit 1; fi` | polish | A1/A7 | open | | Public prose should not mention Arc09 by name; prefer release-preparation wording. |
| F-12 | Public README/docs prose does not teach inherited command names as supported 0.3.0 commands. | `set -e; paths="README.md"; test ! -f docs/index.md || paths="$paths docs/index.md"; if rg -n -- "pandaPIparser|pandaPIgrounder|pandaPIengine" $paths; then exit 1; fi` | correctness-grade | A5/A7 | open | | |
| F-13 | Public README/docs prose does not claim unpublished 0.3.0 release assets or wolong proof. | `set -e; paths="README.md"; test ! -f docs/index.md || paths="$paths docs/index.md"; if rg -n -- "v0\\.3\\.0/.*pandapi-v0\\.3\\.0|release assets are published|0\\.3\\.0.*SHA256SUMS.*published|wolong.*verified.*0\\.3\\.0" $paths; then exit 1; fi` | correctness-grade | A7/A9 | open | | |
| F-14 | The README remains concise and delegates detailed tutorial/reference/architecture content to public docs. | `wc -l README.md && rg -n -- "Tutorial|Reference|Migration|Architecture|Managed Process|More" README.md` | polish | A7/A10 | open | | Closing report should note if the line count grew materially. |
| F-15 | Arc08 `arc-plan.md` and project plan mark Slice09 open/proposed done at close and preserve Slice10 docs-synthesis as next. | `rg -n -- "Slice09|readme-entrypoint-refresh|README\\.md|docs/index\\.md|Slice10|docs-synthesis|A7" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md docs/design-v0.3.0/project-plan.md` | correctness-grade | project management | open | | |
| F-16 | The slice stays out of release, CI, Make, tests, tools, fixtures, implementation, and dependency-vendoring scope unless the ledger is amended. | `set -e; if git diff --cached --name-only -- release .github Makefile mk tests tools pandaPI fixtures licenses | rg .; then exit 1; fi` | serious | boundary | open | | README, `docs/index.md`, and Arc08/project planning docs are allowed. |
| F-17 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | open | | |
| F-18 | The closing report exists and walks every ledger row F-1 through F-18. | `test -f docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice09-readme-entrypoint-refresh/closing-report.md && rg -n -- "F-1|F-2|F-3|F-4|F-5|F-6|F-7|F-8|F-9|F-10|F-11|F-12|F-13|F-14|F-15|F-16|F-17|F-18" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice09-readme-entrypoint-refresh/closing-report.md` | correctness-grade | project management | open | | |

## Ledger Notes

- This is the open ledger for Slice09; do not mark rows done until the
  implementation exists and the commands have been run.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Public prose should use reader-facing release-preparation language, not
  internal arc/slice vocabulary.
- If the README refresh requires changing Make, CI, release packaging, tests,
  tools, fixtures, or implementation source, stop and request a scope
  amendment before editing those surfaces.
- If aggregate `make check` is not run, the closing report must name the
  narrower gates and the docs-slice rationale.
