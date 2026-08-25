# Arc09 Blocker Slice: stdin-artifact-io

Status: closed; Slice01 remains blocked by follow-on stdin fixtures and wolong proof
Opened: 2026-08-20
Closed: 2026-08-20

## Goal

Implement the accepted stdin input contract for `pandapi-parser`,
`pandapi-grounder`, and `pandapi-engine` through shared, tested I/O helpers
rather than component-local quick fixes.

## Scope

In scope:

- Add shared runtime helpers for stdin materialization, input-role labeling,
  temporary file ownership, cleanup, and input-unavailable/input-invalid
  status mapping.
- Teach canonical parser, grounder, and engine entrypoints to accept the
  supported `-` stdin input forms from the design slice.
- Preserve existing file-input behavior, `--output -` behavior,
  `--status=stderr` behavior, and stdout/status conflict handling.
- Keep unsupported stdin forms stable and explicit.
- Add focused runtime/unit or seam coverage for the new shared helpers.
- Run per-component managed contract gates after implementation.

Out of scope:

- No release packaging or publication.
- No public install wording.
- No wolong workspace changes.
- No shell-only API design.
- No `cdc-verification.md`.

## Design Expectations

Implementation should route repeated behavior through shared runtime code. The
native wrappers should not grow three independent copies of stdin reading,
temporary-file creation, cleanup, or status classification.

The inherited parser, grounder, and engine internals may still require file
paths. If so, the supported implementation may materialize stdin to temporary
files before invoking the inherited code path, provided ownership, cleanup,
and failure status are tested.

## Exit Criteria

- Supported stdin forms pass for all three canonical binaries.
- Existing file-backed behavior remains unchanged.
- Existing managed contract, smoke, sanitizer, static-analysis, and
  source-quality gates remain green or have explicit release-blocker evidence.
- Unsupported stdin forms return stable documented status and exit code.

## Closure Evidence

The implementation closes the accepted stdin contract from
`blocker-stdio-contract-design/stdin-contract-design.md` for the product
entrypoints:

- `pandapi-parser` accepts exactly one stdin role: domain from stdin or
  problem from stdin. Parser `- -` is rejected with `cli_usage_error`.
- `pandapi-grounder` accepts one parser-generated `.htn` artifact on stdin.
- `pandapi-engine` accepts one grounder-generated `.sas` artifact on stdin and
  preserves `domain_no_plan` / `outcome=no_plan` for unsolvable stdin input.

Shared runtime code owns stdin materialization, path-role status fields,
temporary-file cleanup, and materialization failure status mapping:

- `pandaPI/runtime/include/pandapi/runtime/stdin_materialization.hpp`
- `pandaPI/runtime/src/stdin_materialization.cpp`
- `pandaPI/runtime/tests/stdin_materialization_smoke.cpp`

Exact closure commands and results are recorded in
[`ledger.md`](ledger.md). The next blocker slice must turn these implemented
stdin behaviors into Make-backed managed fixtures, CI coverage, and current
public process documentation before wolong supervision proof can resume.
