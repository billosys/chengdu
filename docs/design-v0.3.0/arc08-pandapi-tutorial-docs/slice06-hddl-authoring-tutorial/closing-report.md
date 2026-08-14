# Arc08 Slice06 Closing Report: hddl-authoring-tutorial

Status: implementation proposed done, awaiting CDC verification.

## Scope

Slice06 created the public HDDL authoring tutorial at
`docs/tutorial/04-write-hddl-files.md` and updated Arc08/project planning
status. It did not touch README, release/package/publish surfaces, `.github/`,
Makefile or `mk/`, tests, tools, fixtures, parser/grounder/engine/runtime
source, CLI reference, migration page, architecture page, docs index,
licenses, or wolong integration.

## Ledger Walk

| Row | Result | Evidence |
|-----|--------|----------|
| F-1 | done | `docs/tutorial/04-write-hddl-files.md` exists and introduces HDDL authoring. |
| F-2 | done | The page reuses the `status-summary` feature-shipping model from Slice05. |
| F-3 | done | The page includes and explains `:types`, `:predicates`, `:task`, `:method`, and `:action`. |
| F-4 | done | The page includes and explains `:domain`, `:objects`, `:htn`, and `:init`. |
| F-5 | done | The page explains methods, actions, predicates, preconditions, effects, and ordered subtasks. |
| F-6 | done | The page creates `domain.hddl` and `problem.hddl` in a temporary workspace. |
| F-7 | done | The page includes parse, ground, and solve commands using `./bin/pandapi-*`. |
| F-8 | done | Happy-path authoring workflow passed locally: parser, grounder, and engine emitted `status=ok`; `.htn`, `.sas`, and plan artifacts were non-empty. |
| F-9 | done | Generated plan contained `implement-feature`, `test-feature`, `document-feature`, `review-feature`, and `release-feature` for `status-summary`. |
| F-10 | done | The page explains a valid no-plan mistake caused by missing `(requested status-summary)`. |
| F-11 | done | Missing-initial-fact workflow passed locally: parser and grounder succeeded, engine exited `2`, and stderr matched `domain_no_plan` / `exit_code=2`. |
| F-12 | done | The page explains a broken-syntax mistake as invalid input. |
| F-13 | done | Broken-syntax parser check passed locally: parser exited `22` and stderr matched `input_invalid` / `exit_code=22`. |
| F-14 | done | The page includes an authoring checklist. |
| F-15 | done | The page records that no checked-in fixture is needed because files are authored in a temporary workspace. |
| F-16 | done | `make test-contract-pipeline-managed` passed with `129 passed, 0 failed`. |
| F-17 | done | Canonical `pandapi-*` names are present and inherited command-name grep returned no matches. |
| F-18 | done | Internal planning vocabulary grep returned no matches in the public page. |
| F-19 | done | Arc08 `arc-plan.md`, `documentation-blueprint.md`, and project plan reflect Slice06 proposed-done status and keep Slice07 as the CLI/migration follow-up. |
| F-20 | done | Staged protected-boundary check passed; changed paths stayed in public docs and Arc08/project design-planning surfaces. |
| F-21 | done | `git diff --check` and `git diff --cached --check` passed. |
| F-22 | done | This report walks F-1 through F-22 and includes Bubble-up to Arc08 with Slice07, README, and Arc09 handoff notes. |

## Verification

The static ledger greps passed for F-1 through F-7, F-10, F-12, F-14, F-15,
F-17, F-18, F-19, and F-22.

Happy-path command reproduced:

```sh
set -e
make build
tmp="$(mktemp -d)"
# domain.hddl and problem.hddl were created with the tutorial heredocs.
./bin/pandapi-parser --status=stderr --output "$tmp/status-summary.htn" "$tmp/domain.hddl" "$tmp/problem.hddl"
test -s "$tmp/status-summary.htn"
./bin/pandapi-grounder --status=stderr --output "$tmp/status-summary.sas" "$tmp/status-summary.htn"
test -s "$tmp/status-summary.sas"
./bin/pandapi-engine --status=stderr --output "$tmp/status-summary.plan" "$tmp/status-summary.sas"
test -s "$tmp/status-summary.plan"
rg -q -- "implement-feature\\[status-summary\\]" "$tmp/status-summary.plan"
rg -q -- "test-feature\\[status-summary\\]" "$tmp/status-summary.plan"
rg -q -- "document-feature\\[status-summary\\]" "$tmp/status-summary.plan"
rg -q -- "review-feature\\[status-summary\\]" "$tmp/status-summary.plan"
rg -q -- "release-feature\\[status-summary\\]" "$tmp/status-summary.plan"
```

The generated plan contained the expected five primitive actions. The engine
reported solved status.

No-plan command reproduced:

```sh
set -e
# problem-no-request.hddl was created with the tutorial heredoc.
./bin/pandapi-parser --status=stderr --output "$tmp/no-request.htn" "$tmp/domain.hddl" "$tmp/problem-no-request.hddl"
./bin/pandapi-grounder --status=stderr --output "$tmp/no-request.sas" "$tmp/no-request.htn"
set +e
./bin/pandapi-engine --status=stderr --output "$tmp/no-request.plan" "$tmp/no-request.sas" >"$tmp/no-request.stdout" 2>"$tmp/no-request.stderr"
code="$?"
set -e
test "$code" -eq 2
rg -q -- "status=domain_no_plan|exit_code=2" "$tmp/no-request.stderr"
```

Broken-syntax command reproduced:

```sh
set -e
# broken-domain.hddl was created with the tutorial heredoc.
set +e
./bin/pandapi-parser --status=stderr --output "$tmp/broken.htn" "$tmp/broken-domain.hddl" "$tmp/problem.hddl" >"$tmp/broken.stdout" 2>"$tmp/broken.stderr"
code="$?"
set -e
test "$code" -eq 22
rg -q -- "status=input_invalid|exit_code=22" "$tmp/broken.stderr"
```

Make-backed workflow regression:

```sh
make test-contract-pipeline-managed
```

Result: `129 passed, 0 failed`.

Aggregate `make check` was intentionally not run. This was a docs-writing
slice that changed no code, Make, CI, tests, tools, release/package, fixtures,
or other behavior-bearing surfaces. The narrower gates were the F-1 through
F-22 ledger checks, the local happy-path/no-plan/broken-syntax authoring
commands, the managed pipeline contract gate, the staged protected-boundary
check, and whitespace checks.

## Bubble-up to Arc08

Slice06 delivers A12 at implementation-proposed-done strength. The public
chapter now turns the Slice05 model into concrete HDDL files, verifies the
files through parse -> ground -> solve, and teaches the difference between a
valid no-plan modelling mistake and broken syntax.

No checked-in fixture is needed for this slice. The tutorial deliberately
creates files in a temporary workspace so readers practice authoring the HDDL
themselves while the commands remain runnable from the repository root.

Slice07 should now write the CLI reference and migration docs using the
numbered tutorial sequence as the beginner path. README should later point to
the complete tutorial sequence after stable reference and architecture pages
exist. Arc09 has no release-asset change from this slice; its handoff remains
public docs links and migration/release wording after the later CLI/reference
and README slices land.
