# Arc08 Slice02 Closing Report: htn-hddl-onboarding

Status: CC proposed done, awaiting CDC verification.

## Scope

Slice02 created the beginner onboarding page, and the post-report path
correction moved it to the accepted public tutorial path:
`docs/tutorial/htn-hddl-onboarding.md`. The slice also updated Arc08/project
planning status. It did not touch README, release/package/publish surfaces,
`.github/`, Makefile or `mk/`, tests, tools, parser/grounder/engine/runtime
source, managed-process guide, CLI reference, migration page, architecture
page, docs index, licenses, or wolong integration.

## Fixture Decision

The minimal fixture is enough for beginner onboarding. It has one object
(`ccdp-v03`), one compound task (`produce`), one method (`m-produce`), two
primitive actions (`draft`, `review`), ordered subtasks, an empty initial
state, and a top-level HTN task network. That is enough to teach the domain
versus problem split and the parser artifact relationship without adding a
new checked-in fixture.

## Ledger Walk

| Row | Result | Evidence |
|-----|--------|----------|
| F-1 | done | Public page exists and introduces pandaPI 0.3.0, beginner tutorial, HTN, HDDL, and planning terms. |
| F-2 | done | Page explains Hierarchical Task Network, task, method, action, decompose/decomposition, primitive work, and ordered subtasks. |
| F-3 | done | Page explains HDDL/PDDL, domain and problem roles, objects, initial state, `:init`, `:htn`, task network, `domain.hddl`, and `problem.hddl`. |
| F-4 | done | Page uses `fixtures/minimal` and walks `project-work`, `ship-the-spec`, `produce`, `draft`, `review`, and `ccdp-v03`. |
| F-5 | done | Page explains the `pandapi-parser` -> `pandapi-grounder` -> `pandapi-engine` pipeline and `.htn`, `.sas`, plan artifact, and artifact roles without becoming the full workflow tutorial. |
| F-6 | done | Page explains solved, solution, plan found, no-plan, no plan, unsolvable, and outcome language in beginner terms. |
| F-7 | done | Public page and this closing report state the minimal fixture is enough for onboarding; no new fixture was added. |
| F-8 | done | Ledger parser command passed: `make build-parser` succeeded and `dist/macos-arm64/pandapi-parser --status=stderr --output "$tmp/minimal.htn" fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl` produced a non-empty `.htn` artifact. |
| F-9 | done | `make test-contract-parser-managed` passed after the command wording was settled. |
| F-10 | done | Public page uses canonical `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`; inherited command-name grep returned no matches. |
| F-11 | done | Internal planning vocabulary grep returned no matches in the public page. |
| F-12 | done | Arc08 `arc-plan.md` marks Slice02 CC proposed done, points to the Slice02 evidence set, and preserves A1-A10. |
| F-13 | done | `project-plan.md` records Arc08 Slice02 as CC proposed done and keeps Arc09 release preparation/publication separate. |
| F-14 | done | Staged protected-boundary check passed; changed paths stayed in public docs and Arc08/project design-planning surfaces. |
| F-15 | done | `git diff --check` and `git diff --cached --check` passed. |
| F-16 | done | This report walks F-1 through F-16 and includes Bubble-up to Arc08 with the Slice03 fixture/workflow adjustment. |

Post-report correction: the page path was corrected from the initial
`docs/pandapi-0.3.0/tutorial/htn-hddl-onboarding.md` placement to
`docs/tutorial/htn-hddl-onboarding.md` by operator decision. The verification
commands below use the corrected path.

## Verification

Commands run:

```sh
test -f docs/tutorial/htn-hddl-onboarding.md && rg -n -- "pandaPI 0\\.3\\.0|beginner|tutorial|HTN|HDDL|planning" docs/tutorial/htn-hddl-onboarding.md
rg -n -- "Hierarchical Task Network|HTN|task|method|action|decompose|decomposition|primitive|ordered subtask|ordered-subtasks" docs/tutorial/htn-hddl-onboarding.md
rg -n -- "HDDL|PDDL|domain|problem|objects|initial state|:init|:htn|task network|domain\\.hddl|problem\\.hddl" docs/tutorial/htn-hddl-onboarding.md
rg -n -- "fixtures/minimal|domain\\.hddl|problem\\.hddl|project-work|ship-the-spec|produce|draft|review|ccdp-v03" docs/tutorial/htn-hddl-onboarding.md
rg -n -- "pandapi-parser|pandapi-grounder|pandapi-engine|parser|grounder|engine|\\.htn|\\.sas|plan artifact|pipeline|artifact" docs/tutorial/htn-hddl-onboarding.md
rg -n -- "solved|solution|plan found|no-plan|no plan|unsolvable|domain_no_plan|outcome" docs/tutorial/htn-hddl-onboarding.md
rg -n -- "minimal fixture|beginner fixture|small enough|enough for onboarding|new fixture|fixture decision|checked-in" docs/tutorial/htn-hddl-onboarding.md docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice02-htn-hddl-onboarding/closing-report.md
set -e; rg -n -- "pandapi-parser|--status=stderr|--output|fixtures/minimal/domain\\.hddl|fixtures/minimal/problem\\.hddl|\\.htn" docs/tutorial/htn-hddl-onboarding.md; platform=unsupported; case "$(uname -s)-$(uname -m)" in Darwin-arm64) platform=macos-arm64 ;; Linux-x86_64) platform=linux-x86_64 ;; esac; test "$platform" != unsupported; make build-parser; tmp="$(mktemp -d)"; "dist/$platform/pandapi-parser" --status=stderr --output "$tmp/minimal.htn" fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl; test -s "$tmp/minimal.htn"
make test-contract-parser-managed
set -e; rg -n -- "pandapi-parser|pandapi-grounder|pandapi-engine" docs/tutorial/htn-hddl-onboarding.md; if rg -n -- "pandaPIparser|pandaPIgrounder|pandaPIengine|pandaPI\\*" docs/tutorial/htn-hddl-onboarding.md; then exit 1; fi
set -e; if rg -n -- "Arc08|Slice02|CDC|CC|ledger|closing report|project-plan|arc-plan" docs/tutorial/htn-hddl-onboarding.md; then exit 1; fi
rg -n -- "slice02|htn-hddl-onboarding|slice-doc.md|ledger.md|cc-prompt.md|A1|A2|A3|A4|A5|A6|A7|A8|A9|A10" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md
rg -n -- "arc08 - active|Slice02|htn-hddl-onboarding|pandapi-tutorial-docs|Arc09|release preparation|publication" docs/design-v0.3.0/project-plan.md
set -e; if git diff --cached --name-only -- README.md release .github Makefile mk tests tools pandaPI licenses | rg .; then exit 1; fi
git diff --check && git diff --cached --check
rg -n -- "F-1|F-2|F-3|F-4|F-5|F-6|F-7|F-8|F-9|F-10|F-11|F-12|F-13|F-14|F-15|F-16|Bubble-up to Arc08|Slice03|fixture|workflow" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice02-htn-hddl-onboarding/closing-report.md
```

Aggregate `make check` was intentionally not run. This was a docs-writing
slice that changed no code, Make, CI, tests, tools, release/package, or other
behavior-bearing surfaces. The narrower gates were the F-1 through F-16 ledger
checks, the verified parser command, `make test-contract-parser-managed`, the
staged protected-boundary check, and whitespace checks.

## Bubble-up to Arc08

Slice02 delivers A2 at CC proposed-done strength: the public page explains
HTN/PDDL/HDDL, the pandaPI three-stage pipeline, artifacts, solved/no-plan
outcomes, and where `fixtures/minimal` fits.

The minimal fixture was enough; no new beginner fixture is needed.

Slice03 does not need a fixture change before opening. It should use the same
minimal fixture for the happy-path workflow, then introduce `fixtures/unsolvable`
only when teaching no-plan behavior after parse -> ground -> solve is clear.
