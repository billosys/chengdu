# Arc09 Blocker Slice: stdio-contract-design

Status: closed; blocks Slice01 release-readiness-inventory until the follow-on
stdin implementation, fixture, and wolong proof blockers close
Opened: 2026-08-20
Closed: 2026-08-20

## Goal

Amend the Chengdu 0.3.0 managed-process contract so stdin input is either
explicitly supported or explicitly rejected for each canonical binary, with
wolong's supervised erlexec pipeline as the acceptance anchor.

The expected direction is to support stdin input for the release-critical
pipeline:

```text
pandapi-parser -> pandapi-grounder -> pandapi-engine
```

This slice decides the contract before product code changes.

Accepted design:
[`stdin-contract-design.md`](stdin-contract-design.md).

## Scope

In scope:

- Define parser stdin semantics for the two HDDL input roles.
- Define grounder stdin semantics for parser-generated `.htn` artifacts.
- Define engine stdin semantics for grounder-generated `.sas` artifacts.
- Define which stdin forms are supported, which are rejected, and which final
  `PANDAPI_STATUS` and exit codes apply to each form.
- Preserve stdout ownership rules for `--output -`.
- Preserve status ownership rules for `--status=stderr`.
- Decide whether stdin materialization is part of the shared runtime substrate
  or a component-local concern; prefer shared runtime when two or more
  components need the same behavior.
- Update the Arc09 blocker plan with the accepted contract and implementation
  handoff.

Out of scope:

- No product code implementation.
- No fixture-runner changes.
- No public release install wording.
- No wolong code changes.
- No release packaging or publication work.
- No `cdc-verification.md`.

## Design Notes

The design must not assume shell command strings. Wolong uses argv-list
erlexec process management; shell commands may appear only as reproduction
probes.

The parser is the only component with two logical input roles. The design must
make these cases explicit:

- domain from stdin, problem from path;
- domain from path, problem from stdin;
- both domain and problem from stdin.

If both parser inputs through stdin are supported, the design must define
framing. If that form is rejected, the rejection must be a stable contract
result rather than an accidental parser option failure.

## Exit Criteria

- A durable contract amendment or release-blocker decision exists in Arc09.
- The amendment names every supported and rejected stdin form.
- The amendment identifies the shared runtime/helper design to reuse during
  implementation.
- The implementation blocker slice has enough detail for CC to implement
  without rediscovering the design.

## Closure

The design decision is closed. Parser supports exactly one stdin role at a
time, either domain from stdin or problem from stdin; parser `- -` is rejected
because no 0.3.0 framing exists. Grounder accepts one parser-generated `.htn`
artifact from stdin. Engine accepts one grounder-generated `.sas` artifact from
stdin and must preserve `domain_no_plan` / `no_plan` classification for valid
no-plan input. Repeated stdin materialization and path-role behavior belong to
shared runtime/helper code.

No product code, public docs, packaging, publication, wolong workspace, or
`cdc-verification.md` changes are part of this slice.
