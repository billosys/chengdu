# Model a Software Feature as an HTN

The first two tutorials used prepared HDDL fixtures. This chapter takes one
step back and shows how to turn a real-world planning problem into an HTN
model before writing `domain.hddl` and `problem.hddl`.

The example is a small software feature. The same modelling moves apply to
other planning problems: choose the planner's question, name the objects,
describe facts that matter, split large work into compound tasks, define
primitive actions, and decide what solved or no-plan should mean.

## Start With a Planning Question

Real-world requests arrive as prose:

> Add a machine-readable status summary to a command-line tool. The feature
> should be implemented, tested, documented, reviewed, and released.

That request is still too broad for a planner. A planner should answer a
specific question:

> Given a requested feature, what ordered work can ship it?

This question keeps the model focused on work sequencing. It does not ask the
planner to choose the product strategy, design the user interface, write code,
or judge whether the feature is a good idea.

## Choose the Domain and Problem Split

HDDL separates reusable planning rules from one concrete instance.

For this example:

- The domain is the reusable feature-shipping workflow.
- The problem is one feature request named `status-summary`.

The domain should contain the general ideas: features can be implemented,
tested, documented, reviewed, and released. The problem should contain the
specific object and the top-level task to perform.

## Name the Objects

Objects are the things the plan talks about. Keep them concrete and few.

Conceptual model:

```text
Object:
  status-summary : feature
```

This example needs only one object because the interesting question is the
ordered work for one feature. A larger model might add engineers, branches,
test suites, documents, environments, or release channels, but each new object
should earn its place by changing which plans are possible.

## Describe the Facts

Facts are true or false statements about the world. Use facts for conditions
that affect whether work can happen.

Conceptual model:

```text
requested(status-summary)
implemented(status-summary)
tested(status-summary)
documented(status-summary)
reviewed(status-summary)
released(status-summary)
```

The problem starts with `requested(status-summary)` true. The other facts are
not true yet. Actions will make them true as the plan progresses.

Avoid facts that are too vague to control. `good(status-summary)` is not a
useful planning fact unless the model says exactly which action can establish
it and which later work depends on it.

## Identify Compound Tasks

Compound tasks are work that must be decomposed. They are not executed
directly.

Conceptual model:

```text
ship_feature(feature)
prepare_feature(feature)
```

`ship_feature` is the top-level task. It is too large to execute directly, so
a method will decompose it into smaller work.

`prepare_feature` groups implementation, testing, and documentation. Grouping
keeps the top-level method readable while still letting the model express
ordered subtasks.

## Identify Primitive Actions

Primitive actions are executable steps. In a planning model, an action has
preconditions and effects.

Conceptual model:

```text
implement_feature(feature)
  precondition: requested(feature)
  effect: implemented(feature)

test_feature(feature)
  precondition: implemented(feature)
  effect: tested(feature)

document_feature(feature)
  precondition: implemented(feature)
  effect: documented(feature)

review_feature(feature)
  precondition: tested(feature), documented(feature)
  effect: reviewed(feature)

release_feature(feature)
  precondition: reviewed(feature)
  effect: released(feature)
```

This is the point where the model becomes useful. The preconditions explain
why review cannot happen before testing and documentation, and why release
cannot happen before review.

## Write the Methods

Methods explain how compound tasks decompose into subtasks.

Conceptual model:

```text
Method m-ship-feature:
  task: ship_feature(feature)
  subtasks:
    1. prepare_feature(feature)
    2. review_feature(feature)
    3. release_feature(feature)

Method m-prepare-feature:
  task: prepare_feature(feature)
  subtasks:
    1. implement_feature(feature)
    2. test_feature(feature)
    3. document_feature(feature)
```

These methods make the workflow intentionally conservative. Testing and
documentation both wait for implementation. Review waits until both are done.
Release waits for review.

If your real process allows tests and documentation to happen in either order,
you can model that later with weaker ordering constraints or multiple
methods. Start with the simplest truthful model first.

## Define Solved and No-Plan Boundaries

A solved outcome means the planner found primitive actions that satisfy the
top-level task network. For this model, a solved plan should be equivalent to:

Conceptual expected plan:

```text
implement_feature(status-summary)
test_feature(status-summary)
document_feature(status-summary)
review_feature(status-summary)
release_feature(status-summary)
```

A no-plan outcome means the model is valid enough to process, but no action
sequence can satisfy the task network. Useful no-plan examples include:

- `review_feature` requires `tested` and `documented`, but the model has no
  action that can create `documented`.
- `release_feature` requires `reviewed`, but the method omits
  `review_feature`.
- The problem asks to ship an object that is not marked as `requested`.

No-plan examples are not failures to avoid at all costs. They are part of how
you test whether the model says what you think it says.

## Check the Model Before Writing HDDL

Use this checklist before moving to syntax:

| Question | Example answer |
|----------|----------------|
| What is the planning question? | What ordered work can ship one requested feature? |
| What belongs in the domain? | Reusable feature-shipping tasks, methods, actions, predicates, and ordering. |
| What belongs in the problem? | One `status-summary` feature and a top-level `ship_feature` task. |
| Which facts affect planning? | Requested, implemented, tested, documented, reviewed, released. |
| Which tasks are compound? | `ship_feature`, `prepare_feature`. |
| Which actions are primitive? | Implement, test, document, review, release. |
| What order matters? | Implement before test/docs; test and docs before review; review before release. |
| What is solved? | A primitive plan that releases the requested feature. |
| What is no-plan? | A valid model where required facts cannot be established. |

## Fixture Decision

This chapter does not add a checked-in fixture. Its examples are conceptual
and are not intended to be copied directly into the terminal.

The next tutorial turns this same model into real `domain.hddl` and
`problem.hddl` files, then runs them through `pandapi-parser`,
`pandapi-grounder`, and `pandapi-engine`.
