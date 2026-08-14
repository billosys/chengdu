# Write HDDL Files for a Small HTN Project

The previous tutorial modelled a software feature as an HTN. This chapter
turns that model into runnable `domain.hddl` and `problem.hddl` files, then
runs them through the pandaPI pipeline.

The example keeps using the `status-summary` feature:

- `domain.hddl` contains the reusable feature-shipping workflow.
- `problem.hddl` contains one feature request and one top-level task.

Run commands from the repository root.

## Prepare a Workspace

Build the local binaries and create a temporary authoring workspace:

```sh
make build
tmp="$(mktemp -d)"
echo "$tmp"
```

Keep using the same shell so `$tmp` stays set.

## Write `domain.hddl`

Create the reusable domain:

```sh
cat > "$tmp/domain.hddl" <<'HDDL'
(define (domain feature-shipping)
  (:types feature)
  (:predicates
    (requested ?f - feature)
    (implemented ?f - feature)
    (tested ?f - feature)
    (documented ?f - feature)
    (reviewed ?f - feature)
    (released ?f - feature))

  (:task ship-feature :parameters (?f - feature))
  (:task prepare-feature :parameters (?f - feature))

  (:method m-ship-feature
    :parameters (?f - feature)
    :task (ship-feature ?f)
    :ordered-subtasks (and
      (prepare-feature ?f)
      (review-feature ?f)
      (release-feature ?f)))

  (:method m-prepare-feature
    :parameters (?f - feature)
    :task (prepare-feature ?f)
    :ordered-subtasks (and
      (implement-feature ?f)
      (test-feature ?f)
      (document-feature ?f)))

  (:action implement-feature
    :parameters (?f - feature)
    :precondition (requested ?f)
    :effect (implemented ?f))

  (:action test-feature
    :parameters (?f - feature)
    :precondition (implemented ?f)
    :effect (tested ?f))

  (:action document-feature
    :parameters (?f - feature)
    :precondition (implemented ?f)
    :effect (documented ?f))

  (:action review-feature
    :parameters (?f - feature)
    :precondition (and (tested ?f) (documented ?f))
    :effect (reviewed ?f))

  (:action release-feature
    :parameters (?f - feature)
    :precondition (reviewed ?f)
    :effect (released ?f)))
HDDL
```

Read it as four sections:

- `:types` says this domain talks about `feature` objects.
- `:predicates` names the facts that actions can require or create.
- `:task` declares compound tasks that methods can decompose.
- `:method` and `:action` define the actual HTN workflow.

`ship-feature` is the top-level compound task. Its method decomposes into
`prepare-feature`, `review-feature`, and `release-feature`. `prepare-feature`
then decomposes into implementation, testing, and documentation.

The primitive actions are where preconditions and effects matter. For example,
`review-feature` requires both `tested` and `documented`, and
`release-feature` requires `reviewed`.

## Write `problem.hddl`

Create one concrete problem:

```sh
cat > "$tmp/problem.hddl" <<'HDDL'
(define (problem ship-status-summary)
  (:domain feature-shipping)
  (:objects status-summary - feature)
  (:htn :ordered-subtasks (ship-feature status-summary))
  (:init (requested status-summary)))
HDDL
```

The problem file supplies the instance data:

- `:domain` must match the domain name.
- `:objects` creates one feature object.
- `:htn` asks pandaPI to ship that feature.
- `:init` says the feature has been requested.

Without `(requested status-summary)` in `:init`, the first primitive action
cannot run, because `implement-feature` requires `requested`.

## Parse, Ground, and Solve

Parse the HDDL files into an `.htn` artifact:

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/status-summary.htn" \
  "$tmp/domain.hddl" \
  "$tmp/problem.hddl"
test -s "$tmp/status-summary.htn"
```

Ground the parser artifact into `.sas`:

```sh
./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/status-summary.sas" \
  "$tmp/status-summary.htn"
test -s "$tmp/status-summary.sas"
```

Solve the grounded artifact:

```sh
./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/status-summary.plan" \
  "$tmp/status-summary.sas"
test -s "$tmp/status-summary.plan"
```

Check that the plan contains the expected primitive work:

```sh
grep -E "implement-feature\\[status-summary\\]" "$tmp/status-summary.plan"
grep -E "test-feature\\[status-summary\\]" "$tmp/status-summary.plan"
grep -E "document-feature\\[status-summary\\]" "$tmp/status-summary.plan"
grep -E "review-feature\\[status-summary\\]" "$tmp/status-summary.plan"
grep -E "release-feature\\[status-summary\\]" "$tmp/status-summary.plan"
```

The generated plan should implement, test, document, review, and release the
feature. The plan output also includes engine diagnostics; for this tutorial,
the important part is that the primitive action sequence exists and the final
engine status is solved.

## Mistake: Missing Initial Fact

This problem is syntactically valid, but it omits `requested` from `:init`:

```sh
cat > "$tmp/problem-no-request.hddl" <<'HDDL'
(define (problem ship-status-summary-no-request)
  (:domain feature-shipping)
  (:objects status-summary - feature)
  (:htn :ordered-subtasks (ship-feature status-summary))
  (:init))
HDDL
```

Parse and ground still succeed:

```sh
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/no-request.htn" \
  "$tmp/domain.hddl" \
  "$tmp/problem-no-request.hddl"

./bin/pandapi-grounder \
  --status=stderr \
  --output "$tmp/no-request.sas" \
  "$tmp/no-request.htn"
```

The engine reports a valid no-plan outcome:

```sh
set +e
./bin/pandapi-engine \
  --status=stderr \
  --output "$tmp/no-request.plan" \
  "$tmp/no-request.sas" \
  >"$tmp/no-request.stdout" \
  2>"$tmp/no-request.stderr"
code="$?"
set -e

test "$code" -eq 2
grep -E "status=domain_no_plan|exit_code=2" "$tmp/no-request.stderr"
```

That is different from a syntax error. The model is valid enough to process,
but no primitive action can establish the missing `requested` fact.

## Mistake: Broken HDDL Syntax

This domain is intentionally incomplete:

```sh
cat > "$tmp/broken-domain.hddl" <<'HDDL'
(define (domain broken-feature-shipping)
  (:types feature)
  (:predicates (requested ?f - feature)
HDDL
```

The parser rejects it as invalid input:

```sh
set +e
./bin/pandapi-parser \
  --status=stderr \
  --output "$tmp/broken.htn" \
  "$tmp/broken-domain.hddl" \
  "$tmp/problem.hddl" \
  >"$tmp/broken.stdout" \
  2>"$tmp/broken.stderr"
code="$?"
set -e

test "$code" -eq 22
grep -E "status=input_invalid|exit_code=22" "$tmp/broken.stderr"
```

Use the distinction when debugging:

- `input_invalid` means the parser or model checker could not accept the
  files.
- `domain_no_plan` means the files were valid enough to process, but the
  model did not allow a plan.

## Authoring Checklist

Before moving on, check the shape of your own HDDL project:

| Check | Why it matters |
|-------|----------------|
| Domain and problem names match | The problem must point at the domain it uses. |
| Objects have useful types | Types make task/action parameters meaningful. |
| Every top-level task has a method | Compound tasks must decompose before primitive work can run. |
| Primitive preconditions can become true | Otherwise the engine may report no-plan. |
| Primitive effects create facts later work needs | A missing effect can make a valid model unsolvable. |
| Negative cases are intentional | Use no-plan tests to prove the model says what you think it says. |

## Fixture Decision

This tutorial does not add a checked-in fixture. The commands create the HDDL
files in a temporary workspace so readers practice authoring the files
themselves, while the tutorial remains runnable from the repository root.
