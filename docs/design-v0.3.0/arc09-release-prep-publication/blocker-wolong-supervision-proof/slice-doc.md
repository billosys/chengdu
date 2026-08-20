# Arc09 Blocker Slice: wolong-supervision-proof

Status: open; blocks Slice01 release-readiness-inventory
Opened: 2026-08-20

## Goal

Prove that wolong can resume Arc03 from a supported Chengdu stdin/stdout/stderr
managed-process contract, without relying on shell command strings or
unsupported temporary-file workarounds.

## Scope

In scope:

- Verify the accepted stdin contract from Chengdu against wolong's Arc03
  paused blocker.
- Run chengdu's Make-backed stdin/pipeline contract gate.
- Run the relevant wolong runner/probe gate when wolong has a slice ready to
  consume the contract.
- Record whether wolong can resume `slice02-stdio-runner`.
- Update Arc09 Slice01 release-readiness-inventory blocker status and slice
  ordering.

Out of scope:

- No Chengdu release publication.
- No tag creation.
- No package/public install wording unless Slice01 is resumed and explicitly
  routes that work.
- No broad wolong feature implementation beyond acceptance/probe evidence.
- No `cdc-verification.md`.

## Exit Criteria

- Chengdu's stdin contract gates pass.
- Wolong's re-entry condition is satisfied or the remaining blocker is
  documented with owner and re-entry condition.
- Arc09 Slice01 can either resume or remain explicitly blocked with no
  ambiguity.
