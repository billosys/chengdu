# First Project Workflow for pandaPI 0.3.0

This tutorial runs the first complete pandaPI 0.3.0 workflow: parse an HDDL
project, ground it, solve it, inspect the generated artifacts, and compare the
happy path with a valid no-plan case.

It builds on the HTN and HDDL onboarding page. The happy path uses
`fixtures/minimal`, the `project-work` domain, the `ship-the-spec` problem,
and the `produce` task that decomposes into `draft` and `review`.

## Prepare Local Binaries

Run commands from the repository root. This setup builds the three tools into
`./bin/` and creates a temporary workspace for generated artifacts.

```sh
make build
tmp="$(mktemp -d)"
echo "$tmp"
```

Keep using the same shell so `tmp` stays set.

## Parse HDDL Into an `.htn` Artifact

The parser reads the domain and problem files and writes a parser artifact.

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl
test -s "$tmp/minimal.htn"
head -20 "$tmp/minimal.htn"
```

The generated `minimal.htn` file is the parser artifact. It is the normalized
input for the next stage, not the final plan.

## Ground the Parser Artifact Into `.sas`

The grounder reads `minimal.htn` and writes a grounded planner artifact.

```sh
./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/minimal.sas" \
  "$tmp/minimal.htn"
test -s "$tmp/minimal.sas"
head -20 "$tmp/minimal.sas"
```

The generated `minimal.sas` file is the grounder artifact. It contains the
grounded search input that the engine can solve.

## Solve the Grounded Artifact

The engine reads `minimal.sas` and writes a plan artifact when a plan is
found.

```sh
./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/minimal.plan" \
  "$tmp/minimal.sas"
test -s "$tmp/minimal.plan"
cat "$tmp/minimal.plan"
```

For this fixture, a solution should draft the artifact before reviewing it.
That is the concrete plan found from the `produce ccdp-v03` task network.

## Inspect and Rerun Safely

Generated artifacts belong in the temporary workspace. Do not edit the
checked-in fixtures while experimenting with this workflow.

Useful checks:

```sh
ls -lh "$tmp"
test -s "$tmp/minimal.htn"
test -s "$tmp/minimal.sas"
test -s "$tmp/minimal.plan"
```

To rerun from a clean state, delete or replace generated artifact files in
`$tmp`, then run the same parser, grounder, and engine commands again.

```sh
rm -f "$tmp/minimal.htn" "$tmp/minimal.sas" "$tmp/minimal.plan"
```

The source files under `fixtures/minimal` are checked-in fixtures. Keep them
stable and rerun by changing only files in the temporary workspace.

## Compare With a Valid No-Plan Case

`fixtures/unsolvable` is the same small project shape with one important
change: the method for `produce` asks for `review` without first doing
`draft`, so the precondition for review can never be established. The input is
valid enough to parse and ground, but the engine reports a no-plan outcome.

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/unsolvable.htn" \
  fixtures/unsolvable/domain.hddl \
  fixtures/unsolvable/problem.hddl

./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/unsolvable.sas" \
  "$tmp/unsolvable.htn"

set +e
./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/unsolvable.plan" \
  "$tmp/unsolvable.sas" \
  >"$tmp/unsolvable.stdout" \
  2>"$tmp/unsolvable.stderr"
code="$?"
set -e

test "$code" -eq 2
grep -E "status=domain_no_plan|exit_code=2" "$tmp/unsolvable.stderr"
```

An exit code of `2` with `domain_no_plan` means the planner processed valid
input and found no usable plan. It is not the same thing as a missing file,
broken syntax, or invalid model.

## Fixture Decision

No intermediate fixture beyond `fixtures/unsolvable` is needed for this first
workflow. `fixtures/minimal` teaches the happy path, and `fixtures/unsolvable`
is enough for workflow-level no-plan comparison. Later docs can add a new
checked-in fixture only if they need to teach a different behavior.
