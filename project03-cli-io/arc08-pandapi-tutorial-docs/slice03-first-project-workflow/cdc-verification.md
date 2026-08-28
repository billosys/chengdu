# Arc08 Slice03 CDC Verification: first-project-workflow

Status: CDC verified and closed.

Verified: 2026-08-13 on `release/0.3.x`.

## Review Basis

CDC reviewed the Slice03 ledger, closing report, public tutorial page, Arc08
plan, and project plan. The opening ledger has 20 rows, and the closing report
walks F-1 through F-20 without silent drops.

## Reproduced Evidence

CDC reproduced the static public-doc checks for:

- accepted tutorial path and workflow introduction;
- HTN/HDDL concept continuity from Slice02;
- `make build` plus `./bin/` local binary setup;
- canonical parser, grounder, and engine workflow commands;
- artifact inspection and rerun hygiene;
- `fixtures/unsolvable` no-plan comparison;
- fixture decision;
- canonical command-name policy;
- absence of internal planning vocabulary from the public page;
- Arc08/project plan status and Arc09 release-publication boundary.

CDC reproduced the runnable workflow gates:

```sh
set -e
make build
tmp="$(mktemp -d)"
./bin/pandapi-parser --status=stderr --output "$tmp/minimal.htn" fixtures/minimal/domain.hddl fixtures/minimal/problem.hddl
test -s "$tmp/minimal.htn"
./bin/pandapi-grounder --status=stderr --output "$tmp/minimal.sas" "$tmp/minimal.htn"
test -s "$tmp/minimal.sas"
./bin/pandapi-engine --status=stderr --output "$tmp/minimal.plan" "$tmp/minimal.sas"
test -s "$tmp/minimal.plan"
```

Result: parser, grounder, and engine emitted final `PANDAPI_STATUS
status=ok` records, and `.htn`, `.sas`, and plan artifacts were non-empty.

CDC reproduced the no-plan workflow:

```sh
set -e
make build
tmp="$(mktemp -d)"
./bin/pandapi-parser --status=stderr --output "$tmp/unsolvable.htn" fixtures/unsolvable/domain.hddl fixtures/unsolvable/problem.hddl
./bin/pandapi-grounder --status=stderr --output "$tmp/unsolvable.sas" "$tmp/unsolvable.htn"
set +e
./bin/pandapi-engine --status=stderr --output "$tmp/unsolvable.plan" "$tmp/unsolvable.sas" >"$tmp/unsolvable.stdout" 2>"$tmp/unsolvable.stderr"
code="$?"
set -e
test "$code" -eq 2
rg -q -- "status=domain_no_plan|exit_code=2" "$tmp/unsolvable.stderr"
```

Result: parser and grounder succeeded, engine exited `2`, and stderr matched
`domain_no_plan` / `exit_code=2`.

CDC reproduced the Make-backed gates:

```sh
make test-contract-pipeline-managed
make smoke
make smoke-negative
```

Results:

- managed pipeline contract: `129 passed, 0 failed`;
- positive smoke: `3 passed, 0 failed`;
- negative smoke: `4 passed, 0 failed`.

## Boundary And Whitespace

CDC reran the protected-path and whitespace checks during the combined
Slices03-05 verification commit:

```sh
git diff --check
git diff --cached --check
```

The Slice03 implementation stayed within public docs and Arc08/project
planning scope. No `README.md`, release, `.github/`, Make, `mk/`, tests,
tools, `pandaPI/`, or license surfaces were part of the Slice03 diff.

## Result

All F-1 through F-20 rows reproduced. Slice03 is CDC verified and closed.

## Bubble-up to Arc08

Slice03 satisfies Arc08 A3 and A8 for the first runnable project workflow.
The page proves `parse -> ground -> solve` with `fixtures/minimal`, compares
that happy path with `fixtures/unsolvable`, and resolves that no additional
intermediate fixture is needed before later tutorials.
