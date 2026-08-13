# HTN and HDDL Onboarding for pandaPI 0.3.0

This beginner tutorial introduces the planning ideas behind pandaPI 0.3.0
before you run the full toolchain. You will see how a small HDDL example
describes work, how pandaPI reads it, and what kinds of artifacts the three
commands produce.

## The Shape of HTN Planning

HTN means Hierarchical Task Network. Instead of asking a planner to choose
from a flat list of actions, an HTN model starts with tasks and explains how
larger tasks can be decomposed into smaller work.

The core pieces are:

- A task names work that should be achieved.
- A method explains how to decompose a compound task into subtasks.
- An action is primitive work the planner can execute directly.
- Ordered subtasks say that one subtask must happen before another.

In the minimal example, the compound task is `produce`. The method
`m-produce` decomposes that task into two ordered subtasks: first `draft`,
then `review`. The actions `draft` and `review` are primitive actions.

## Domain and Problem Files

HDDL is a hierarchical planning language in the PDDL family. For this
onboarding page, the important split is simple:

- `domain.hddl` contains reusable rules: types, predicates, tasks, methods,
  and actions.
- `problem.hddl` contains one instance: the object names, initial state, and
  top-level HTN task network to solve.

The checked-in beginner example lives in `fixtures/minimal`.

`fixtures/minimal/domain.hddl` defines the reusable `project-work` domain:

- `artifact` is the object type.
- `drafted` and `reviewed` describe facts about an artifact.
- `produce` is the compound task.
- `m-produce` says producing an artifact means doing `draft` and then
  `review`.
- `draft` creates the `drafted` fact.
- `review` requires `drafted` and creates the `reviewed` fact.

`fixtures/minimal/problem.hddl` defines the `ship-the-spec` problem:

- It uses the `project-work` domain.
- It creates one object, `ccdp-v03`, as an `artifact`.
- Its top-level `:htn` task network asks pandaPI to `produce ccdp-v03`.
- Its `:init` state is empty, so the plan must establish every needed fact
  through actions.

The example is intentionally small enough for beginner onboarding. It has one
object, one compound task, one method, and two primitive actions, but still
shows the key HTN idea: a high-level task becomes ordered executable work.

## The pandaPI Pipeline

pandaPI uses three stages:

1. `pandapi-parser` reads HDDL domain and problem files and writes a parser
   artifact, usually a `.htn` file.
2. `pandapi-grounder` reads the `.htn` artifact and writes a grounded planner
   artifact, usually a `.sas` file.
3. `pandapi-engine` searches the grounded artifact and writes a plan artifact
   when a plan is found, or reports a no-plan outcome when the model is valid
   but no usable plan exists.

This page verifies only the first stage so the concept stays focused. The
complete parse, ground, and solve workflow comes next.

## Produce the First Artifact

Run this command from the repository root. It selects the local binary
directory for macOS arm64 or Linux x86_64, builds the parser, writes a `.htn`
artifact into a temporary directory, and checks that the artifact is non-empty.

```sh
platform=unsupported
case "$(uname -s)-$(uname -m)" in
  Darwin-arm64) platform=macos-arm64 ;;
  Linux-x86_64) platform=linux-x86_64 ;;
esac
test "$platform" != unsupported

make build-parser
tmp="$(mktemp -d)"
"dist/$platform/pandapi-parser" \
  --status=stderr \
  --output "$tmp/minimal.htn" \
  fixtures/minimal/domain.hddl \
  fixtures/minimal/problem.hddl
test -s "$tmp/minimal.htn"
```

The generated `.htn` file is the parser artifact. It is not the final plan;
it is the normalized planning input that the grounder will consume in the next
workflow.

## Solved and No-Plan Outcomes

A solved outcome means the engine found executable steps that satisfy the
task network. For the minimal example, that plan should include drafting the
artifact before reviewing it.

A no-plan outcome is different from a syntax error. It means the input was
valid enough to process, but the planner could not find a usable plan. For
example, a model can ask for a goal that no available method or action can
reach. Later tutorials use no-plan examples after the happy path is clear.
