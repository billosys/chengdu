# chengdu arc05 / slice06 - parser-native-contract-cutover - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Replace the parser transition Bash adapter with a native canonical
`pandapi-parser` artifact that satisfies the accepted managed-process contract
without installing parser old-name compatibility artifacts.

Slice05 proved that the full parser/grounder cutover cannot be achieved by
deleting shims: the raw inherited parser sidecar rejects managed-process
options such as `--status=stderr`. This slice handles the parser half first.
Grounder remains transitional until its own native cutover slice. Engine remains
inherited until the engine adoption slice.

## 2. Scope

**In:**

- Build/install `dist/<platform>/pandapi-parser` as the parser artifact used
  by tests and tooling.
- Make `pandapi-parser` a native executable or native chengdu-owned parser
  entrypoint, not a Bash adapter copied into `dist/`.
- Stop installing parser artifacts named `pandaPIparser` or
  `pandaPIparser.legacy` in `dist/<platform>/`.
- Delete `scripts/pandapi-parser-adapter.sh` after parser contract fixtures
  pass natively.
- Remove active parser build/test/provenance/package-helper call sites that
  depend on `scripts/pandapi-parser-adapter.sh`, `pandaPIparser`, or
  `pandaPIparser.legacy`.
- Preserve accepted parser managed-process behavior through canonical
  `pandapi-parser`, including normal HDDL parse, supported common options,
  status output, diagnostics, help/version/provenance, TTY/color policy, and
  legacy-surface fences.
- Update parser contract fixture records and parser-specific active docs so
  they no longer treat inherited parser commands or `.legacy` sidecars as
  accepted parser behavior.
- Keep build/test/check/CI entrypoints routed through make targets.
- Close this slice ledger and write the CC closing report.

**Out:**

- No grounder native cutover and no deletion of grounder shims.
- No engine contract adoption and no `pandapi-engine` cutover.
- No full README/package/release cleanup that requires grounder to be native
  first; update parser-specific references only when they can avoid claiming
  grounder completion.
- No release publication, final release manifest, checksums, or GitHub release
  publishing behavior.
- No broad parser grammar rewrite, algorithm work, or optional inherited
  helper-surface expansion.
- No new dependency import, network fetch, `FetchContent`, `ExternalProject`,
  `git clone`, or `curl` in default build/test paths.
- No direct project-script invocation from new CI/GitHub Actions jobs.
- No Arc05 close report.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- whether the native parser contract is implemented by renaming/refactoring the
  inherited parser entrypoint, adding a chengdu-owned compiled entrypoint, or
  another non-shell mechanism;
- how parser managed-process option parsing maps to existing parser behavior
  without making inherited helper modes supported 0.3.0 surfaces;
- how parser status, diagnostics, and output-file errors are produced through
  `pandapi::runtime` or another accepted chengdu-owned runtime boundary;
- how parser provenance avoids claiming a `legacy_binary` sidecar while still
  preserving source/upstream credit;
- what parser references can be safely updated now, and what remains deferred
  until grounder native cutover.

If parser managed behavior still requires a Bash adapter or parser `.legacy`
sidecar, stop and ask for an amendment. Do not mark parser no-shim rows done.

## 4. Constraints

- Treat the operator's no-compatibility decision as current policy:
  `v0.3.0` should not ship inherited parser command compatibility.
- Keep this slice parser-only.
- Do not bulk-format inherited `pandaPI/` source.
- Use make targets for build, test, check, CI, and release-helper operations.
- New CI behavior must use make targets.
- Preserve Arc02 dependency gates and Arc03 managed-process semantics.
- Required assistant-authored commits must include both co-author trailers from
  `AGENTS.md`.

## 5. Verification Approach

CC verifies by rebuilding the parser, proving `dist/<platform>/pandapi-parser`
is executable and not a shell adapter, proving no parser old-name or `.legacy`
artifact exists in `dist/<platform>/`, rerunning parser managed contract
fixtures through make, and grep-checking active parser surfaces for stale shim
or sidecar references.

CDC will rerun the ledger commands, inspect the diff for accidental grounder,
engine, release-publication, or dependency expansion, and confirm that parser
managed behavior no longer depends on `scripts/pandapi-parser-adapter.sh`.

## 6. Exit Criteria

`make build-parser` leaves only `dist/<platform>/pandapi-parser` for parser.
The artifact is native/non-shell and passes parser managed contract fixtures
through `make test-contract-parser-managed`. Active parser build/test/tooling
surfaces no longer invoke or install the parser transition adapter, inherited
parser command, or parser `.legacy` sidecar. Grounder transition artifacts may
remain only because the grounder native cutover is explicitly out of scope.
