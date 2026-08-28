# Arc09 Blocker Slice: wolong-supervision-proof

Status: closed; proof and routing complete; external Wolong runner/API blocker
still blocks Slice01 release-readiness-inventory
Opened: 2026-08-20
Activated: 2026-08-25
Closed: 2026-08-25

## Goal

Prove that wolong can resume Arc03 from a supported Chengdu stdin/stdout/stderr
managed-process contract, without relying on shell command strings or
unsupported temporary-file workarounds.

This slice is a proof and routing slice. It must distinguish:

- Chengdu binary behavior, which is now fixture-proven by
  `make test-contract-stdio-managed`;
- direct local command probes, which are useful baseline evidence but are not
  Wolong supervision proof by themselves;
- Wolong erlexec argv-list supervision evidence, which is the release-blocking
  consumer proof; and
- any remaining Wolong-owned implementation gap, which must be recorded as an
  external blocker with owner and re-entry condition rather than hidden inside
  Chengdu release readiness.

## Scope

In scope:

- Verify the accepted stdin contract from Chengdu against wolong's Arc03
  paused blocker.
- Run chengdu's Make-backed stdin/pipeline contract gate.
- Re-read wolong's original blocker report and map every original Chengdu
  failure claim to fixed, accepted-as-rejected, or still blocked.
- Run direct Chengdu probes for parser, grounder, engine, solved pipeline, and
  no-plan pipeline stdin behavior from `./bin/pandapi-*`.
- Inspect wolong's current erlexec runner/API/test state and determine whether
  Wolong can already consume the contract.
- Run the relevant wolong erlexec/argv-list probe gate if it exists or can be
  added in the Wolong workspace as a narrowly scoped verification fixture.
- Record whether wolong can resume `slice02-stdio-runner`, or whether Wolong
  now owns a follow-on implementation blocker.
- Update Arc09 Slice01 release-readiness-inventory blocker status and slice
  ordering.
- Add a durable Chengdu-side proof report:
  `wolong-supervision-proof.md`.

Out of scope:

- No Chengdu release publication.
- No tag creation.
- No package/public install wording unless Slice01 is resumed and explicitly
  routes that work.
- No broad wolong feature implementation beyond acceptance/probe evidence. If
  Wolong needs a real runner/API slice to add stdin support, document that
  blocker instead of implementing it inside this Chengdu slice.
- No committed Chengdu product-code changes unless direct proof exposes a new
  Chengdu release blocker and the operator accepts a scope amendment.
- No shell-command-string workaround as release proof. Shell probes may be
  baseline diagnostics only.
- No `cdc-verification.md`.

## Exit Criteria

- Chengdu's stdin contract gates pass.
- Each original Wolong blocker report claim has a written disposition.
- Direct local Chengdu stdin probes match the accepted contract.
- Wolong erlexec/argv-list supervision proof passes, or the remaining external
  blocker is documented with owner, affected Wolong slice, and re-entry
  condition.
- Arc09 Slice01 can either resume or remain explicitly blocked with no
  ambiguity.
- The closing report bubbles up whether the next work is Arc09 Slice01,
  a Wolong Arc03 slice, or another explicit Chengdu blocker.

## Closure

This proof slice is closed as a Chengdu-side routing artifact. Chengdu's
supported stdin/stdout/stderr contract is fixture-proven and direct-probe
verified, and raw Wolong erlexec argv-list stdin handling works against a real
`pandapi-grounder` child process.

Arc09 Slice01 remains blocked because Wolong's current `wolong-exec:run/3`
runner/API does not expose stdin artifact bytes, does not enable erlexec
`stdin`, and does not send EOF. The next required work is a Wolong-owned Arc03
`slice02-stdio-runner` implementation or equivalent runner/API slice, followed
by a renewed Arc09 proof run.

Close artifacts:
[`wolong-supervision-proof.md`](wolong-supervision-proof.md) and
[`closing-report.md`](closing-report.md).
