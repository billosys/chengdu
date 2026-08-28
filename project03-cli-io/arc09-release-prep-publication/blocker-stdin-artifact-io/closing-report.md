# Arc09 Blocker Closing Report: stdin-artifact-io

Date: 2026-08-24

Status: implementation proposed done after CDC repair of staged cleanup issue.

## Summary

This blocker implements the accepted stdin artifact IO contract from
`blocker-stdio-contract-design/stdin-contract-design.md` for the three
canonical `pandapi-*` binaries.

The delivered behavior is:

- `pandapi-parser` accepts exactly one stdin input role: domain from stdin or
  problem from stdin.
- `pandapi-parser - -` fails before parsing with `cli_usage_error`, exit `10`.
- `pandapi-grounder` accepts one parser-generated `.htn` artifact on stdin.
- `pandapi-engine` accepts one grounder-generated `.sas` artifact on stdin.
- Engine stdin preserves solved and `domain_no_plan` / `outcome=no_plan`
  classification.
- `--output -` and `--status=stderr` remain the supervised stdout/stderr
  ownership shape.
- Shared runtime code owns stdin materialization, path-role vocabulary,
  cleanup, and materialization failure status fields.

CDC review found one serious staged implementation defect before commit: the
wrapper `finish()` helpers used `std::exit`, which bypasses destructors for
automatic C++ objects and therefore could leak materialized stdin paths. The
final implementation replaces that termination path with a status-emitting
`ProcessExit` exception caught by `main`, adds destructor cleanup coverage, and
explicitly cleans materialized stdin before final status classification so a
cleanup failure can be reported as `output_unavailable` with
`operation=cleanup`.

## Ledger Walk

- **F-1 - done.** The slice consumes the accepted stdin contract from
  `blocker-stdio-contract-design`; the slice and Arc09 docs preserve the
  dependency and follow-on blocker chain.
- **F-2 - done.** Shared runtime files
  `pandaPI/runtime/include/pandapi/runtime/stdin_materialization.hpp`,
  `pandaPI/runtime/src/stdin_materialization.cpp`, and
  `pandaPI/runtime/tests/stdin_materialization_smoke.cpp` own materialization,
  path roles, cleanup helpers, status fields, and cleanup coverage. The
  parser, grounder, and engine wrappers call the shared helper instead of
  open-coding stdin readers.
- **F-3 - done.** Parser stdin domain and stdin problem forms produce non-empty
  `.htn` artifacts with `status=ok component=parser`; parser both-stdin
  rejects with exit `10` and `status=cli_usage_error component=parser`.
- **F-4 - done.** Grounder consumes parser-produced `.htn` content from stdin,
  writes the selected `.sas` artifact, keeps stdout empty when output is a
  file, and emits `status=ok component=grounder` on stderr.
- **F-5 - done.** Engine consumes grounder-produced `.sas` content from stdin,
  solves the positive case with `status=ok outcome=solved`, and preserves
  unsolvable stdin input as exit `2`, `status=domain_no_plan`, and
  `outcome=no_plan`.
- **F-6 - done.** Managed parser, grounder, and engine contract gates remain
  part of closure evidence.
- **F-7 - done.** Managed pipeline, positive smoke, and negative smoke gates
  remain part of closure evidence.
- **F-8 - done.** Source-quality and safety gates remain part of closure
  evidence for the touched code path.
- **F-9 - done.** The slice does not edit wolong, release publication state, or
  public install claims.
- **F-10 - done.** No `cdc-verification.md` is created by the implementing
  slice.
- **F-11 - done.** Staged and unstaged whitespace checks remain required before
  commit.

## Bubble-Up To Arc09

Arc09 now has an implemented stdin artifact IO path for the accepted
parser/grounder/engine contract. Slice01 release-readiness-inventory remains
blocked until:

1. `blocker-stdio-contract-fixtures` adds Make-backed stdin fixtures, CI
   coverage, and public managed-process/CLI documentation for this behavior.
2. `blocker-wolong-supervision-proof` verifies wolong against the supported
   stdin/stdout/stderr contract or records a concrete remaining external
   blocker.

The follow-on fixture slice should include regression coverage for stdin
materialization cleanup or process-level temp-file absence where practical.

## Silent-Drop Diff

The close intentionally does not change:

- wolong workspace files;
- release publication, package contents, checksums, or manifests;
- public install claims;
- inherited `pandaPI*` compatibility surfaces;
- JSON, packet, or multipart stdin framing;
- parser both-inputs-from-stdin support.

No `cdc-verification.md` was created by this implementation close.
