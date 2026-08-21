# Arc09 Blocker Closing Report: stdio-contract-design

Date: 2026-08-20

Status: closed by ledger evidence; design-only.

## Summary

Arc09 `blocker-stdio-contract-design` closed the design question raised by
wolong's supervised stdin/stdout/stderr pipeline blocker. The accepted contract
is recorded in [`stdin-contract-design.md`](stdin-contract-design.md), with no
product implementation in this slice.

The decision is:

- Parser supports exactly one stdin input role at a time: domain from stdin or
  problem from stdin.
- Parser rejects both inputs from stdin because Chengdu 0.3.0 has no accepted
  framing for two HDDL documents on one byte stream.
- Grounder supports stdin for one parser-generated `.htn` artifact.
- Engine supports stdin for one grounder-generated `.sas` artifact and must
  preserve `domain_no_plan` / `no_plan` classification for valid no-plan
  input.
- `--output -` plus `--status=stderr` remains the supervised
  stdout/stderr/PANDAPI_STATUS shape.
- Shared runtime/helper code owns repeated stdin materialization, path-role
  behavior, cleanup, and common status mapping.

## Per-Row Closure Evidence

| Row | Closure evidence |
|-----|------------------|
| F-1 | `slice-doc.md` and `arc-plan.md` both retain the literal `blocks Slice01 release-readiness-inventory` marker while clarifying that the design blocker is closed and the follow-on blockers still gate Slice01. |
| F-2 | `stdin-contract-design.md` records parser domain from stdin, problem from stdin, both inputs from stdin, rejected `- -`, and missing 0.3.0 framing. |
| F-3 | `stdin-contract-design.md` records grounder stdin for one complete parser-generated `.htn` artifact. |
| F-4 | `stdin-contract-design.md` records engine stdin for one complete grounder-generated `.sas` artifact and preserves `domain_no_plan` / `no_plan`. |
| F-5 | `stdin-contract-design.md` preserves `--output -`, `--status=stderr`, stdout ownership, stderr ownership, and final `PANDAPI_STATUS`; it keeps `--status=stdout --output -` as `cli_usage_error`. |
| F-6 | `stdin-contract-design.md` chooses shared runtime/helper ownership for stdin materialization, path-role behavior, cleanup, and common status mapping. |
| F-7 | The staged boundary check for the design slice passed with only Arc09 planning/evidence docs and `project-plan.md` status updates staged. No product code, public docs, packaging, release tooling, or wolong workspace files changed. |
| F-8 | No `cdc-verification.md` exists for this blocker; CDC remains the independent verification owner. |
| F-9 | `git diff --check && git diff --cached --check` passed for the design-slice close. |

## Bubble-Up To Arc09

Arc09 now has an accepted stdin contract design and no longer needs to
rediscover parser role semantics, grounder/engine artifact input semantics, or
shared helper ownership before implementation.

The bubble-up to `arc-plan.md` is:

- `blocker-stdio-contract-design` is closed.
- `blocker-stdin-artifact-io` is ready to implement the accepted stdin design.
- `blocker-stdio-contract-fixtures` remains blocked by implementation.
- `blocker-wolong-supervision-proof` remains blocked by fixture evidence.
- `slice01-release-readiness-inventory` remains blocked until the accepted
  stdin contract is implemented, fixture-proven, documented, and verified
  against wolong's paused Arc03 re-entry condition.

## Silent-Drop Diff

The close intentionally did not change:

- parser, grounder, engine, or shared runtime product code;
- contract fixture runner behavior;
- public managed-process, CLI, tutorial, README, migration, architecture, or
  release docs;
- Make, `mk/`, release tooling, CI workflows, or package outputs;
- the wolong workspace;
- publication, tagging, or release assets;
- CDC-owned verification artifacts.

The accepted design also silently drops any implicit stdin semantics:

- omitted positional operands do not imply stdin;
- parser `- -` is not accepted without a future framing design;
- human diagnostic prose is not contract evidence for supervisors.

## Remaining Blockers Before Slice01 Can Resume

Slice01 release-readiness-inventory must not resume until the remaining wolong
stdin blocker chain closes:

1. `blocker-stdin-artifact-io`: implement accepted stdin forms through shared
   runtime/helper ownership while preserving existing file-input behavior,
   stdout artifacts, stderr status, and no-plan classification.
2. `blocker-stdio-contract-fixtures`: add Make-backed component and supervised
   pipeline fixtures, including stdin artifact handoff, parser `- -`
   rejection, stdout/stderr ownership, and engine no-plan preservation.
3. `blocker-wolong-supervision-proof`: verify wolong can drive the supported
   Chengdu stdin/stdout/stderr contract, or record a concrete remaining
   external blocker before Slice01 resumes.

No `cdc-verification.md` was created.
