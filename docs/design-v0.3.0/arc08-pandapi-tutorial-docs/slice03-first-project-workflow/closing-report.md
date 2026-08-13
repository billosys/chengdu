# Arc08 Slice03 Closing Report: first-project-workflow

Status: CC proposed done, awaiting CDC verification.

## Scope

Slice03 created the first runnable public workflow tutorial at
`docs/tutorial/first-project-workflow.md` and updated Arc08/project planning
status. It did not touch README, release/package/publish surfaces, `.github/`,
Makefile or `mk/`, tests, tools, parser/grounder/engine/runtime source,
managed-process guide, CLI reference, migration page, architecture page, docs
index, licenses, fixtures, or wolong integration.

## Fixture Decision

No intermediate fixture beyond `fixtures/unsolvable` is needed before later
docs. `fixtures/minimal` proves the happy path, and `fixtures/unsolvable`
proves a valid no-plan comparison using the same small project shape. That is
enough for workflow-level teaching; later docs can add a new checked-in
fixture only if they need to teach a different behavior.

## Ledger Walk

| Row | Result | Evidence |
|-----|--------|----------|
| F-1 | done | `docs/tutorial/first-project-workflow.md` exists and introduces a first project workflow for pandaPI 0.3.0. |
| F-2 | done | The page builds on HTN/HDDL concepts and uses `fixtures/minimal`, `project-work`, `ship-the-spec`, `produce`, `draft`, and `review`. |
| F-3 | done | The page includes a local binary setup that builds canonical commands into `./bin/` with `make build`. |
| F-4 | done | The parser section uses `pandapi-parser`, `--output`, `minimal.htn`, `.htn`, parser artifact, and the minimal domain/problem files. |
| F-5 | done | The grounder section uses `pandapi-grounder`, `--output`, `minimal.sas`, `.sas`, grounded/grounder artifact, and `minimal.htn`. |
| F-6 | done | The engine section uses `pandapi-engine`, `--output`, `minimal.plan`, plan artifact, plan found/solution language, and `minimal.sas`. |
| F-7 | done | Happy-path workflow verification passed locally and produced non-empty `.htn`, `.sas`, and plan artifacts. |
| F-8 | done | Page teaches artifact inspection and rerun hygiene with `cat`, `head`, `test -s`, `mktemp`, delete/replace guidance, generated artifacts, and checked-in fixtures. |
| F-9 | done | Page compares with `fixtures/unsolvable` and explains no-plan, `domain_no_plan`, valid input, outcome, and exit code `2`. |
| F-10 | done | No-plan workflow verification passed locally: parser and grounder succeeded, engine exited `2`, and stderr included `status=domain_no_plan` / `exit_code=2`. |
| F-11 | done | Page and this report resolve the fixture decision: no new fixture is needed beyond `fixtures/unsolvable`. |
| F-12 | done | `make test-contract-pipeline-managed` passed after workflow wording was settled. |
| F-13 | done | `make smoke && make smoke-negative` passed after workflow wording was settled. |
| F-14 | done | Public page uses canonical `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`; inherited command-name grep returned no matches. |
| F-15 | done | Internal planning vocabulary grep returned no matches in the public page. |
| F-16 | done | Arc08 `arc-plan.md` marks Slice03 CC proposed done, points to the Slice03 evidence set, and preserves A1-A10. |
| F-17 | done | `project-plan.md` records Arc08 Slice03 as CC proposed done and keeps Arc09 release preparation/publication separate. |
| F-18 | done | Staged protected-boundary check passed; changed paths stayed in public docs and Arc08/project design-planning surfaces. |
| F-19 | done | `git diff --check` and `git diff --cached --check` passed. |
| F-20 | done | This report walks F-1 through F-20 and includes Bubble-up to Arc08 with Slice04, Slice05, README, and Arc09 handoff notes. |

## Verification

Commands run:

```sh
test -f docs/tutorial/first-project-workflow.md && rg -n -- "first project|workflow|parse|ground|solve|pandaPI 0\\.3\\.0|tutorial" docs/tutorial/first-project-workflow.md
rg -n -- "HTN|HDDL|domain\\.hddl|problem\\.hddl|fixtures/minimal|project-work|ship-the-spec|produce|draft|review" docs/tutorial/first-project-workflow.md
rg -n -- "local binaries|\\./bin|make build" docs/tutorial/first-project-workflow.md
rg -n -- "pandapi-parser|--output|minimal\\.htn|\\.htn|parser artifact|fixtures/minimal/domain\\.hddl|fixtures/minimal/problem\\.hddl" docs/tutorial/first-project-workflow.md
rg -n -- "pandapi-grounder|--output|minimal\\.sas|\\.sas|grounded|grounder artifact|minimal\\.htn" docs/tutorial/first-project-workflow.md
rg -n -- "pandapi-engine|--output|minimal\\.plan|plan artifact|plan found|solution|minimal\\.sas" docs/tutorial/first-project-workflow.md
set -e; make build; tmp="$(mktemp -d)"; ./bin/pandapi-parser --status=stderr --output "$tmp/minimal.htn" fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl; test -s "$tmp/minimal.htn"; ./bin/pandapi-grounder --status=stderr --output "$tmp/minimal.sas" "$tmp/minimal.htn"; test -s "$tmp/minimal.sas"; ./bin/pandapi-engine --status=stderr --output "$tmp/minimal.plan" "$tmp/minimal.sas"; test -s "$tmp/minimal.plan"
rg -n -- "inspect|cat|head|test -s|temporary|mktemp|rerun|delete|replace|generated artifact|do not edit|checked-in fixtures" docs/tutorial/first-project-workflow.md
rg -n -- "fixtures/unsolvable|unsolvable|no-plan|no plan|domain_no_plan|valid input|outcome|exit code|2" docs/tutorial/first-project-workflow.md
set -e; make build; tmp="$(mktemp -d)"; ./bin/pandapi-parser --status=stderr --output "$tmp/unsolvable.htn" fixtures/unsolvable/domain.hddl fixtures/unsolvable/problem.hddl; ./bin/pandapi-grounder --status=stderr --output "$tmp/unsolvable.sas" "$tmp/unsolvable.htn"; set +e; ./bin/pandapi-engine --status=stderr --output "$tmp/unsolvable.plan" "$tmp/unsolvable.sas" >"$tmp/unsolvable.stdout" 2>"$tmp/unsolvable.stderr"; code="$?"; set -e; test "$code" -eq 2; rg -q -- "status=domain_no_plan|exit_code=2" "$tmp/unsolvable.stderr"
rg -n -- "intermediate fixture|fixtures/unsolvable|enough for workflow|no new fixture|new fixture|fixture decision|checked-in" docs/tutorial/first-project-workflow.md docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice03-first-project-workflow/closing-report.md
make test-contract-pipeline-managed
make smoke && make smoke-negative
set -e; rg -n -- "pandapi-parser|pandapi-grounder|pandapi-engine" docs/tutorial/first-project-workflow.md; if rg -n -- "pandaPIparser|pandaPIgrounder|pandaPIengine|pandaPI\\*" docs/tutorial/first-project-workflow.md; then exit 1; fi
set -e; if rg -n -- "Arc08|Slice03|CDC|CC|ledger|closing report|project-plan|arc-plan" docs/tutorial/first-project-workflow.md; then exit 1; fi
rg -n -- "slice03|first-project-workflow|slice-doc.md|ledger.md|cc-prompt.md|A1|A2|A3|A4|A5|A6|A7|A8|A9|A10" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md
rg -n -- "arc08 - active|Slice03|first-project-workflow|pandapi-tutorial-docs|Arc09|release preparation|publication" docs/design-v0.3.0/project-plan.md
set -e; if git diff --cached --name-only -- README.md release .github Makefile mk tests tools pandaPI licenses | rg .; then exit 1; fi
git diff --check && git diff --cached --check
rg -n -- "F-1|F-2|F-3|F-4|F-5|F-6|F-7|F-8|F-9|F-10|F-11|F-12|F-13|F-14|F-15|F-16|F-17|F-18|F-19|F-20|Bubble-up to Arc08|Slice04|Slice05|README|Arc09|handoff" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice03-first-project-workflow/closing-report.md
```

Aggregate `make check` was intentionally not run. This was a docs-writing
slice that changed no code, Make, CI, tests, tools, release/package, fixtures,
or other behavior-bearing surfaces. The narrower gates were the F-1 through
F-20 ledger checks, the happy-path and no-plan workflow verifications,
`make test-contract-pipeline-managed`, `make smoke`, `make smoke-negative`,
the staged protected-boundary check, and whitespace checks.

## Bubble-up to Arc08

Slice03 delivers A3 at CC proposed-done strength: the public page teaches a
runnable parse -> ground -> solve workflow using canonical `pandapi-*`
commands and checked-in fixtures.

No intermediate fixture beyond `fixtures/unsolvable` was needed.

Slice04 should reuse the same happy-path and no-plan workflow shape when it
teaches supervised/process-manager details. Slice05 can point its CLI
reference and behavior-change table at this workflow as the basic command
sequence. README should later link to this page once the public docs entry
point is refreshed. Arc09 should treat this workflow as tutorial evidence, not
release-publication proof; release assets and wolong installation still need
their own verification.
