# Arc09 Blocker Slice: stdin-artifact-io

Status: open; blocks Slice01 release-readiness-inventory
Opened: 2026-08-20

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
