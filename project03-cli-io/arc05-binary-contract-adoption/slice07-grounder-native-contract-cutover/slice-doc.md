# chengdu arc05 / slice07 - grounder-native-contract-cutover - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Replace the grounder transition Bash adapter with a native canonical
`pandapi-grounder` artifact that satisfies the accepted managed-process
contract without installing grounder old-name compatibility artifacts.

Slice05 proved that the full parser/grounder cutover could not be achieved by
deleting adapters because the raw inherited sidecars rejected contract options.
Slice06 completed the parser half by making `pandapi-parser` native and
deleting parser old-name artifacts. This slice completes the grounder half.
Engine remains inherited until the engine adoption slice.

## 2. Scope

**In:**

- Build/install `dist/<platform>/pandapi-grounder` as the grounder artifact
  used by tests and tooling.
- Make `pandapi-grounder` a native executable or native chengdu-owned grounder
  entrypoint, not a Bash adapter copied into `dist/`.
- Stop installing grounder artifacts named `pandaPIgrounder` or
  `pandaPIgrounder.legacy` in `dist/<platform>/`.
- Delete `scripts/pandapi-grounder-adapter.sh` and
  `scripts/install-grounder-adapter.sh` after grounder contract fixtures pass
  natively.
- Remove active grounder build/test/provenance/package-helper call sites that
  depend on the grounder adapter scripts, `pandaPIgrounder`, or
  `pandaPIgrounder.legacy`.
- Preserve accepted grounder managed-process behavior through canonical
  `pandapi-grounder`, including normal `.htn` grounding, file and stdout
  artifacts, status output, diagnostics, help/version/provenance, TTY/color
  policy, and H2 / `cpddl` / FAM surface fencing.
- Update grounder contract fixture records and grounder-specific active docs
  so they no longer treat inherited grounder commands or `.legacy` sidecars as
  accepted grounder behavior.
- Preserve Slice06 parser-native behavior and ensure parser old-name artifacts
  remain absent.
- Keep build/test/check/CI entrypoints routed through make targets.
- Close this slice ledger and write the CC closing report.

**Out:**

- No parser behavior change except regression-test/fixture fallout required to
  keep accepted Slice06 parser-native evidence green.
- No engine contract adoption and no `pandapi-engine` cutover.
- No full README/package/release cleanup that requires engine adoption; update
  grounder-specific references only when they can avoid claiming engine
  completion.
- No release publication, final release manifest, checksums, or GitHub release
  publishing behavior.
- No H2 product expansion, `cpddl`/FAM product expansion, standalone
  dependency audit, optional dependency enablement, SAT/BDD/CUDD work, engine
  interactive/translation work, or full-duplex stdin adoption.
- No new dependency import, network fetch, `FetchContent`, `ExternalProject`,
  `git clone`, or `curl` in default build/test paths.
- No direct project-script invocation from new CI/GitHub Actions jobs.
- No Arc05 close report.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- whether the native grounder contract is implemented by renaming/refactoring
  the inherited grounder entrypoint, adding a chengdu-owned compiled
  entrypoint, or another non-shell mechanism;
- how grounder managed-process option parsing maps to existing grounder
  behavior without making H2 or `cpddl`/FAM supported 0.3.0 normal surfaces;
- how grounder status, diagnostics, artifact disposition, and output-file
  errors are produced through `pandapi::runtime` or another accepted
  chengdu-owned runtime boundary;
- how grounder provenance avoids claiming a `legacy_binary` sidecar while
  still preserving source/upstream credit and dependency-surface visibility;
- what grounder references can be safely updated now, and what remains
  deferred until engine adoption or Arc07/Arc08 documentation and release
  work.

If grounder managed behavior still requires a Bash adapter or grounder
`.legacy` sidecar, stop and ask for an amendment. Do not mark grounder no-shim
rows done.

## 4. Constraints

- Treat the operator's no-compatibility decision as current policy:
  `v0.3.0` should not ship inherited grounder command compatibility.
- Keep this slice grounder-only.
- Do not bulk-format inherited `pandaPI/` source.
- Use make targets for build, test, check, CI, and release-helper operations.
- New CI behavior must use make targets.
- Preserve Arc02 dependency gates and Arc03 managed-process semantics.
- Preserve Slice06 parser-native behavior and artifact cleanup.
- Required assistant-authored commits must include both co-author trailers from
  `AGENTS.md`.

## 5. Verification Approach

CC verifies by rebuilding the grounder, proving
`dist/<platform>/pandapi-grounder` is executable and not a shell adapter,
proving no grounder old-name or `.legacy` artifact exists in
`dist/<platform>/`, rerunning grounder managed contract fixtures through make,
and grep-checking active grounder surfaces for stale shim or sidecar
references.

CDC will rerun the ledger commands, inspect the diff for accidental parser,
engine, release-publication, or dependency expansion, and confirm that grounder
managed behavior no longer depends on `scripts/pandapi-grounder-adapter.sh` or
`scripts/install-grounder-adapter.sh`.

## 6. Exit Criteria

`make build-grounder` leaves only `dist/<platform>/pandapi-grounder` for
grounder. The artifact is native/non-shell and passes grounder managed
contract fixtures through `make test-contract-grounder-managed`. Active
grounder build/test/tooling surfaces no longer invoke or install the grounder
transition adapter, inherited grounder command, or grounder `.legacy` sidecar.
Parser native behavior remains green, parser old-name artifacts remain absent,
and engine behavior remains inherited until Slice08.
