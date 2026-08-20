# Arc09 Blocker Design: stdin Contract

Date: 2026-08-20

Status: accepted design; product implementation remains out of scope for this
slice.

## Decision

Chengdu 0.3.0 must support explicit stdin input for the release-critical
managed-process pipeline:

```text
pandapi-parser -> pandapi-grounder -> pandapi-engine
```

This is release-blocking because wolong's supervised erlexec pipeline needs to
write supported inputs to stdin, receive selected artifacts from stdout, and
classify results from exit codes plus the final `PANDAPI_STATUS` on stderr.
Slice01 release-readiness-inventory remains blocked until this contract is
implemented, fixture-proven, documented in the current public managed-process
surface, and verified against wolong.

The stdin sentinel is the positional token `-`. Stdin is supported only when a
command's accepted input grammar names a positional input role and that role is
given as `-`. Omitted positional inputs do not imply stdin.

## Parser stdin Forms

`pandapi-parser` has two logical input roles: HDDL domain and HDDL problem.
The accepted forms are:

| Form | Contract |
|------|----------|
| `pandapi-parser [COMMON] [--output OUT.htn|-] - PROBLEM.hddl` | Supported: domain from stdin, problem from path. The stdin byte stream is the complete domain document. |
| `pandapi-parser [COMMON] [--output OUT.htn|-] DOMAIN.hddl -` | Supported: problem from stdin, domain from path. The stdin byte stream is the complete problem document. |
| `pandapi-parser [COMMON] [--output OUT.htn|-] - -` | Rejected: both inputs from stdin are not supported for 0.3.0 because a single stdin stream has no accepted framing for two HDDL documents. |

The `- -` form must fail before parsing as `cli_usage_error`, exit `10`, with a
final `PANDAPI_STATUS` when status output is selected. Diagnostic prose may
explain that both parser inputs from stdin require framing, but supervisors
must classify only from the exit code and status fields.

Supporting both parser inputs through stdin would require a future explicit
framing design, such as a multipart envelope or role-tagged input container.
That framing is not part of this release contract.

## Grounder stdin Form

`pandapi-grounder` has one supported normal input role: a parser-generated
`.htn` artifact.

| Form | Contract |
|------|----------|
| `pandapi-grounder [COMMON] [--output OUT.sas|-] -` | Supported: grounder stdin contains the complete parser-generated `.htn` artifact. |

Any omitted input, extra input, or additional stdin-like positional form is a
usage error. Malformed `.htn` content read from stdin is classified the same as
malformed `.htn` content read from a path: `input_invalid`, exit `22`, with no
stdout artifact.

## Engine stdin Form

`pandapi-engine` has one supported normal input role: a grounder-generated
`.sas` artifact.

| Form | Contract |
|------|----------|
| `pandapi-engine [COMMON] [--output PLAN|-] -` | Supported: engine stdin contains the complete grounder-generated `.sas` artifact. |

Any omitted input, extra input, or additional stdin-like positional form is a
usage error.

The engine stdin path must preserve the same search classification as the file
path form. If the same `.sas` content reaches search and produces no usable
plan, the result remains `domain_no_plan`, exit `2`, with
`outcome=no_plan`. It must not be downgraded to `input_invalid`,
`input_unavailable`, or a generic failure merely because the input came from
stdin.

## Stream Ownership

The existing stdout ownership rule remains unchanged:

- `--output -` assigns stdout to the selected parser, grounder, or engine
  artifact.
- `--status=stderr` assigns the final `PANDAPI_STATUS` line to stderr and is
  the supervised setting for stdout artifacts.
- `--status=stdout` conflicts with `--output -` and remains
  `cli_usage_error`, exit `10`.
- Human diagnostics, progress, statistics, and color do not belong on stdout
  when stdout carries an artifact.

For the wolong pipeline, each Chengdu process must be usable with stdin input,
`--output -`, and `--status=stderr` at the same time. The stdin stream is input
only; it does not change stdout ownership or stderr status ownership.

## Shared Runtime Ownership

The implementation must use shared runtime/helper ownership for repeated stdin
materialization and path-role behavior across parser, grounder, and engine.
Component-local one-off readers are rejected for this release blocker because
all three binaries need the same caller-visible classifications, stream
ownership checks, cleanup behavior, and status fields.

The shared helper should own:

- recognizing positional `-` after command shape validation;
- enforcing each component's accepted stdin roles;
- rejecting unsupported forms such as parser both inputs from stdin before
  domain parsing begins;
- reading stdin exactly once for an invocation;
- materializing stdin to an implementation-owned temporary file when inherited
  component internals require a path;
- reporting failures to read stdin as `input_unavailable`, exit `20`;
- reporting failures to create, write, flush, close, or clean up the temporary
  materialization path as `output_unavailable`, exit `21`;
- preserving logical path fields so supervisors see the caller-selected input
  as `-` with the appropriate `path_role`, while any temporary path remains an
  implementation detail unless explicitly exposed for diagnostics;
- reusing the same path-role vocabulary for parser domain input, parser
  problem input, grounder `.htn` input, engine `.sas` input, and selected
  output artifacts.

The follow-on `blocker-stdin-artifact-io` slice should implement this helper
before wiring component-specific stdin support. The
`blocker-stdio-contract-fixtures` slice should then prove file input parity,
stdin artifact handoff, stdout artifact ownership, stderr `PANDAPI_STATUS`,
parser `- -` rejection, and engine `domain_no_plan` / `no_plan` preservation.

## Handoff

Accepted implementation contract:

1. Parser supports exactly one of domain from stdin or problem from stdin.
2. Parser rejects both inputs from stdin because no 0.3.0 framing exists.
3. Grounder supports stdin for one complete parser-generated `.htn` artifact.
4. Engine supports stdin for one complete grounder-generated `.sas` artifact.
5. `--output -` plus `--status=stderr` is the supervised stdout/stderr shape.
6. Shared runtime/helper code owns stdin materialization, path-role behavior,
   cleanup, and common status mapping.
7. Wolong proof remains blocked until implementation and fixture slices close.
