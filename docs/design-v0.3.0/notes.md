# chengdu 0.3.0 — pre-planning notes (placeholder)

> **This is not a plan-of-record.** No project has been convened at this
> design version; there is no DoD, no arc roadmap, no ledger. These are
> the tentative intentions and accumulated notes for 0.3.0, parked
> in-repo at the correct contextual level so they survive session
> boundaries. When 0.3.0 is convened, write a real `project-plan.md`
> per `PROJECT-MANAGEMENT.md` (research → definition → plan) and retire
> or absorb this file — tracked, not silently.
>
> Recorded 2026-08-06, from the fork-decision working session
> (operator + Cowork session). Prerequisite: chengdu 0.2.0 closed
> (pandaPI in-tree, upstream absorption done).

## Tentative intent

Make the three pandaPI binaries *excellent managed processes* — the
behavioral overhaul that the 0.2.0 fork exists to enable. Three bodies
of work, likely arcs:

1. **CLI/API audit** — full survey of what each binary can do and how
   accessible it is via flags (all three already use gengetopt /
   `options.ggo`, so the scaffolding exists); recommendations for
   changes/overhaul.
2. **stdio/error-handling audit** — exit codes, stream discipline,
   buffering, crash behavior; recommendations for changes/overhaul.
3. **Design doc: the managed-process ideal** — the target state for
   each tool when run under a supervisor that owns its stdio (wolong's
   erlexec trees), then implementation of the accepted
   recommendations.

## The erlexec-critical rubric (draft audit checklist)

Known defects marked ✗ (field-tested at 0.1.0 or source-verified
2026-08-06); unknowns marked ?.

- **Exit-code contract** — distinct, documented codes for
  solved / unsolvable / parse-error / internal-error.
  ✗ engine exits 0 on "Proven unsolvable"; parser mixes
  255/2/1/3; upstream PR #14 (absorbed at 0.2.0 if vetted) is the
  starting point, not the end state.
- **Stream separation** — stdout carries data (plans, results), stderr
  carries diagnostics. ✗ plan interleaved with progress logs on
  stdout; 0.1.0 pipeline captures `2>&1` and greps the plan back out.
- **Buffering** — explicit line-buffering or flush discipline under
  pipes (erlexec gives pipes, not ttys). ✗ no setvbuf/flush policy
  anywhere; `endl` flushes are incidental, not designed.
- **ANSI/TTY discipline** — color only when `isatty`. ✗ parser leaks
  SGR codes past `-C/--no-colour` on some error paths.
- **Fail-fast on malformed input** — bounded memory, prompt error exit.
  ✗ engine observed spinning unbounded (12+ GB RSS) on malformed
  `.sas`; 0.1.0 wraps it in an external watchdog.
- **Signal handling** — graceful SIGTERM (flush, report, exit with
  distinct code); SIGKILL as supervisor's last resort, not first. ?
- **Machine-readable output mode** — stable, versioned event stream
  (NDJSON is the working idea) for progress/solutions/status, so the
  wolong statem consumes events rather than regex archaeology. Absent
  today.
- **`--version` with provenance** — version + chengdu commit + import
  lineage, machine-parseable. Absent today (gengetopt stubs exist).
- **No cwd/temp assumptions** — explicit paths for all outputs, no
  interactive prompts on any non-interactive path. ? (engine has an
  `interactivePlanner` mode — audit its reachability.)

## Consumer context (why these, in this order)

wolong's API contract is `#(ok plan)` / `#(unsolvable ...)` /
`#(error #(gate reason detail))` — no unverified plan crosses the API.
Every rubric line above maps to a wolong failure mode: exit-code
ambiguity breaks the return type; stream mixing breaks plan capture;
buffering breaks liveness detection; unbounded spins break supervision
budgets. The 0.3.0 acceptance question is roughly: *can wolong's
erlexec layer be written with no grep, no watchdog hack, and no
special-casing per binary?*

## Tentatively out of scope

- A true Erlang port-protocol mode (length-prefixed packets). Judged
  unnecessary in the 2026-08-06 session: erlexec is fine with
  well-behaved stdio; a stable line protocol is the target. Revisit
  only if the NDJSON mode proves insufficient.
- Solver-algorithm work of any kind. 0.3.0 is interface and behavior,
  not planning research.

## Release note (tentative)

0.3.0 would be the first release where the binaries' *behavior*
intentionally diverges from upstream pandaPI. The release notes should
carry a behavior-change table (old → new exit codes, stream layout,
new flags) since wolong and any other consumer must migrate
deliberately.
