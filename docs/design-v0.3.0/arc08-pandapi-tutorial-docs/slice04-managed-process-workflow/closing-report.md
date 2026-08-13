# Arc08 Slice04 Closing Report: managed-process-workflow

Status: CC proposed done, awaiting CDC verification.

## Scope

Slice04 created the public managed-process guide at
`docs/managed-process.md` and updated Arc08/project planning status. It did
not touch README, release/package/publish surfaces, `.github/`, Makefile or
`mk/`, tests, tools, fixtures, parser/grounder/engine/runtime source, CLI
reference, migration page, architecture page, docs index, licenses, or wolong
integration.

## Ledger Walk

| Row | Result | Evidence |
|-----|--------|----------|
| F-1 | done | `docs/managed-process.md` exists and introduces supervised/process-manager integration for `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine`. |
| F-2 | done | The page points readers back to `tutorial/02-first-project-workflow.md` and names the `parse -> ground -> solve` path. |
| F-3 | done | The page teaches `--supervised`, `--status`, `--status=stderr`, `--status=stdout`, status streams, status lines, and machine status. |
| F-4 | done | The page documents stdout/stderr ownership and says not to scrape human diagnostic prose for classification. |
| F-5 | done | The page documents final `PANDAPI_STATUS` records, required fields, `outcome=`, and `partial_output_policy=`. |
| F-6 | done | The page lists common status names and exit-code classes used by supervisors. |
| F-7 | done | The page documents ANSI/color behavior, `--no-color`, `--no-colour`, `NO_COLOR`, TTY-aware defaults, machine-owned streams, supervised output, and escape-sequence expectations. |
| F-8 | done | The page includes a successful supervised parser example with `./bin/pandapi-parser`, `--supervised`, `--status=stderr`, `minimal.htn`, and `fixtures/minimal`. |
| F-9 | done | Supervised parser verification passed locally: artifact non-empty, stdout empty, no ANSI leakage, and final parser `status=ok` on stderr. |
| F-10 | done | The page includes a supervised parse -> ground -> solve pipeline with `./bin/pandapi-parser`, `./bin/pandapi-grounder`, and `./bin/pandapi-engine`. |
| F-11 | done | Supervised pipeline verification passed locally: parser, grounder, and engine artifacts were non-empty, and all three final statuses were `ok`. |
| F-12 | done | The page explains valid no-plan behavior with `fixtures/unsolvable`, `domain_no_plan`, no-plan language, exit 2, and valid input. |
| F-13 | done | No-plan verification passed locally: parser and grounder succeeded, engine exited `2`, and stderr matched `domain_no_plan` / `exit_code=2`. |
| F-14 | done | The page includes a `broken-syntax` negative example, classifies `input_invalid` from status fields, and distinguishes `output_unavailable` exit 21 from `input_invalid` exit 22. |
| F-15 | done | Negative input verification passed locally: parser exited `22`, stdout was empty, and stderr matched `input_invalid` / `exit_code=22`. |
| F-16 | done | The page explains a `--status=stdout` conflict where stdout cannot own both artifact and status, classified as `cli_usage_error`, exit 10. |
| F-17 | done | Stdout conflict verification passed locally: parser exited `10` and stdout carried the usage-error status record. |
| F-18 | done | Managed contract gates passed: parser `298 passed, 0 failed`; grounder `269 passed, 0 failed`; engine `312 passed, 0 failed`; pipeline `129 passed, 0 failed`. |
| F-19 | done | Public page uses canonical `pandapi-*` command names; inherited command-name grep returned no matches. |
| F-20 | done | Internal planning vocabulary grep returned no matches in the public page. |
| F-21 | done | Arc08 `arc-plan.md` marks Slice04 CC proposed done, points to the Slice04 evidence set, and preserves A1-A10. |
| F-22 | done | `project-plan.md` records Arc08 Slice04 as CC proposed done and keeps Arc09 release preparation/publication separate. |
| F-23 | done | Staged protected-boundary check passed; changed paths stayed in public docs and Arc08/project design-planning surfaces. |
| F-24 | done | `git diff --check` and `git diff --cached --check` passed. |
| F-25 | done | This report walks F-1 through F-25 and includes Bubble-up to Arc08 with Slice05, README, and Arc09 handoff notes. |

## Verification

Commands run:

```sh
test -f docs/managed-process.md && rg -n -- "managed process|supervised|process-manager|integrat|pandapi-parser|pandapi-grounder|pandapi-engine" docs/managed-process.md
rg -n -- "02-first-project-workflow|first project workflow|tutorial/02|parse -> ground -> solve|parse .* ground .* solve" docs/managed-process.md
rg -n -- "--supervised|--status|--status=stderr|--status=stdout|status stream|status line|machine status" docs/managed-process.md
rg -n -- "stdout|stderr|ownership|artifact|diagnostic|progress|statistics|human prose|diagnostic prose|do not scrape|scrap" docs/managed-process.md
rg -n -- "PANDAPI_STATUS|required fields|status=|component=|surface=|surface_disposition=|exit_code=|class=|outcome=|partial_output_policy=" docs/managed-process.md
rg -n -- "ok|domain_no_plan|cli_usage_error|input_unavailable|output_unavailable|input_invalid|unsupported_feature|legacy_surface|experimental_surface|future_surface|timeout|resource_limit|interrupted|dependency_failure|child_process_failure|internal_error|signal_terminated|exit code" docs/managed-process.md
rg -n -- "ANSI|color|colour|--no-color|--no-colour|NO_COLOR|TTY|machine-owned|supervised output|escape" docs/managed-process.md
rg -n -- "\./bin/pandapi-parser|--supervised|--status=stderr|minimal\.htn|fixtures/minimal/domain\.hddl|fixtures/minimal/problem\.hddl" docs/managed-process.md
set -e; make build; tmp="$(mktemp -d)"; ./bin/pandapi-parser --supervised --status=stderr --output "$tmp/minimal.htn" fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl >"$tmp/parser.stdout" 2>"$tmp/parser.stderr"; test -s "$tmp/minimal.htn"; test ! -s "$tmp/parser.stdout"; perl -ne 'exit 1 if /\e\[/' "$tmp/parser.stdout" "$tmp/parser.stderr"; rg -q -- "^PANDAPI_STATUS\tstatus=ok\tcomponent=parser" "$tmp/parser.stderr"
rg -n -- "\./bin/pandapi-parser|\./bin/pandapi-grounder|\./bin/pandapi-engine|--supervised|minimal\.htn|minimal\.sas|minimal\.plan|pipeline" docs/managed-process.md
set -e; make build; tmp="$(mktemp -d)"; ./bin/pandapi-parser --supervised --status=stderr --output "$tmp/minimal.htn" fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl >"$tmp/parser.stdout" 2>"$tmp/parser.stderr"; ./bin/pandapi-grounder --supervised --status=stderr --output "$tmp/minimal.sas" "$tmp/minimal.htn" >"$tmp/grounder.stdout" 2>"$tmp/grounder.stderr"; ./bin/pandapi-engine --supervised --status=stderr --output "$tmp/minimal.plan" "$tmp/minimal.sas" >"$tmp/engine.stdout" 2>"$tmp/engine.stderr"; test -s "$tmp/minimal.htn"; test -s "$tmp/minimal.sas"; test -s "$tmp/minimal.plan"; rg -q -- "^PANDAPI_STATUS\tstatus=ok\tcomponent=parser" "$tmp/parser.stderr"; rg -q -- "^PANDAPI_STATUS\tstatus=ok\tcomponent=grounder" "$tmp/grounder.stderr"; rg -q -- "^PANDAPI_STATUS\tstatus=ok\tcomponent=engine" "$tmp/engine.stderr"
rg -n -- "fixtures/unsolvable|unsolvable|domain_no_plan|no-plan|no plan|exit 2|valid input" docs/managed-process.md
set -e; make build; tmp="$(mktemp -d)"; ./bin/pandapi-parser --supervised --status=stderr --output "$tmp/unsolvable.htn" fixtures/unsolvable/domain.hddl fixtures/unsolvable/problem.hddl >"$tmp/parser.stdout" 2>"$tmp/parser.stderr"; ./bin/pandapi-grounder --supervised --status=stderr --output "$tmp/unsolvable.sas" "$tmp/unsolvable.htn" >"$tmp/grounder.stdout" 2>"$tmp/grounder.stderr"; set +e; ./bin/pandapi-engine --supervised --status=stderr --output "$tmp/unsolvable.plan" "$tmp/unsolvable.sas" >"$tmp/engine.stdout" 2>"$tmp/engine.stderr"; code="$?"; set -e; test "$code" -eq 2; rg -q -- "status=domain_no_plan|exit_code=2" "$tmp/engine.stderr"
rg -n -- "broken-syntax|broken-reference|input_invalid|output_unavailable|negative|status fields|exit 21|exit 22" docs/managed-process.md
set -e; make build; tmp="$(mktemp -d)"; set +e; ./bin/pandapi-parser --supervised --status=stderr --output "$tmp/broken.htn" fixtures/broken-syntax/domain.hddl fixtures/broken-syntax/problem.hddl >"$tmp/parser.stdout" 2>"$tmp/parser.stderr"; code="$?"; set -e; test "$code" -eq 22; test ! -s "$tmp/parser.stdout"; rg -q -- "status=input_invalid|exit_code=22" "$tmp/parser.stderr"
rg -n -- "--status=stdout|stdout.*artifact|artifact.*stdout|conflict|cli_usage_error|exit 10" docs/managed-process.md
set -e; make build; tmp="$(mktemp -d)"; set +e; ./bin/pandapi-parser --supervised --status=stdout --output - fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl >"$tmp/parser.stdout" 2>"$tmp/parser.stderr"; code="$?"; set -e; test "$code" -eq 10; rg -q -- "status=cli_usage_error|exit_code=10" "$tmp/parser.stdout"
make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed
set -e; rg -n -- "pandapi-parser|pandapi-grounder|pandapi-engine" docs/managed-process.md; if rg -n -- "pandaPIparser|pandaPIgrounder|pandaPIengine|pandaPI\*" docs/managed-process.md; then exit 1; fi
set -e; if rg -n -- "Arc08|Slice04|CDC|CC|ledger|closing report|project-plan|arc-plan" docs/managed-process.md; then exit 1; fi
rg -n -- "slice04|managed-process-workflow|slice-doc.md|ledger.md|cc-prompt.md|A1|A2|A3|A4|A5|A6|A7|A8|A9|A10" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/arc-plan.md
rg -n -- "arc08 - active|Slice04|managed-process-workflow|pandapi-tutorial-docs|Arc09|release preparation|publication" docs/design-v0.3.0/project-plan.md
set -e; if git diff --cached --name-only -- README.md release .github Makefile mk tests tools pandaPI fixtures licenses | rg .; then exit 1; fi
git diff --check && git diff --cached --check
rg -n -- "F-1|F-2|F-3|F-4|F-5|F-6|F-7|F-8|F-9|F-10|F-11|F-12|F-13|F-14|F-15|F-16|F-17|F-18|F-19|F-20|F-21|F-22|F-23|F-24|F-25|Bubble-up to Arc08|Slice05|README|Arc09|handoff" docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice04-managed-process-workflow/closing-report.md
```

Aggregate `make check` was intentionally not run. This was a docs-writing
slice that changed no code, Make, CI, tests, tools, release/package, fixtures,
or other behavior-bearing surfaces. The narrower gates were the F-1 through
F-25 ledger checks, the supervised parser, pipeline, no-plan, negative-input,
and stdout-conflict verifications, the managed parser/grounder/engine/pipeline
contract gates, the staged protected-boundary check, and whitespace checks.

## Bubble-up to Arc08

Slice04 delivers A4 at CC proposed-done strength: the public guide teaches
`--supervised`, status stream selection, stdout/stderr ownership, final
`PANDAPI_STATUS`, exit/status taxonomy, ANSI/color behavior, and negative
outcomes without asking integrators to scrape human diagnostic prose.

No fixture or verification gap needs a later docs/testing slice for the basic
managed-process workflow. Existing fixtures cover the happy path,
`domain_no_plan`, invalid input, and stdout status/artifact conflict cases.

Slice05 should link to this page from the CLI reference and migration table
when it documents the canonical `pandapi-*` options and behavior changes.
README should later include this page as the process-manager entry point after
the docs index and README refresh slice. Arc09 should use this guide as the
wolong managed-process documentation handoff, while still verifying release
assets, installation, and wolong migration against published package contents.
