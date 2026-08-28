# chengdu arc05 / slice05 - canonical-binary-cutover - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Cut parser and grounder over from transition shims to canonical `pandapi-*`
build and release artifacts.

This slice implements the operator's 2026-08-10 policy correction: `v0.2.0`
is the transition release, and `v0.3.0` is not required to preserve inherited
`pandaPIparser` or `pandaPIgrounder` command compatibility. Once parser and
grounder can be built and contract-tested under their new names, the old-name
outputs, `.legacy` outputs, shim scripts, tests, fixture records, Make targets,
package metadata, provenance checks, and README/docs references must move to
the canonical names.

This is a parser/grounder cutover slice. Engine remains on the inherited
`pandaPIengine` name until the engine adoption slice.

## 2. Scope

**In:**

- Make `make build-parser` install `dist/<platform>/pandapi-parser` as the
  parser artifact used by tests and tooling.
- Make `make build-grounder` install `dist/<platform>/pandapi-grounder` as the
  grounder artifact used by tests and tooling.
- Stop installing `dist/<platform>/pandaPIparser`,
  `dist/<platform>/pandaPIparser.legacy`, `dist/<platform>/pandaPIgrounder`,
  and `dist/<platform>/pandaPIgrounder.legacy`.
- Delete transition shim scripts once they are no longer needed:
  `scripts/pandapi-parser-adapter.sh`,
  `scripts/pandapi-grounder-adapter.sh`, and
  `scripts/install-grounder-adapter.sh`.
- Update `mk/*`, `Makefile` help text, contract fixtures, smoke tests,
  provenance checks, macOS minimum-OS recording, and package tooling so parser
  and grounder use `pandapi-parser` and `pandapi-grounder`.
- Update `README.md` and current fixture documentation so the supported
  parser/grounder command examples use `pandapi-*`, not inherited `pandaPI*`.
- Remove parser/grounder contract assertions that inherited `pandaPIparser` or
  `pandaPIgrounder` remains executable, emits inherited provenance, or has a
  raw `.legacy` passthrough.
- Preserve the accepted parser and grounder managed-process behavior for
  supported normal surfaces through the canonical commands.
- Preserve engine baseline behavior through `pandaPIengine` until the engine
  slice owns `pandapi-engine`.
- Keep CI and local operations routed through make targets.
- Close the slice ledger and write the CC closing report.

**Out:**

- No engine contract adoption and no `pandapi-engine` cutover.
- No release publication, checksums, final release manifest, or GitHub release
  publishing behavior. Package helper metadata may be updated only so local
  dry-run tooling no longer teaches parser/grounder old-name artifacts.
- No attempt to preserve 100% inherited parser/grounder CLI compatibility.
- No default inherited-name alias, symlink, copied binary, wrapper, or
  deprecation-warning path for parser or grounder.
- No broad parser, grounder, H2, `cpddl`, FAM, SAT, BDD, CUDD, or engine
  optional-surface expansion.
- No new dependency import, network fetch, package-manager bootstrap,
  `FetchContent`, `ExternalProject`, `git clone`, or `curl` in default build
  and test paths.
- No direct project-script invocation from new CI/GitHub Actions jobs.
- No Arc05 close report.
- No `cdc-verification.md`; CDC owns that file.

## 3. Required Decisions

CC must decide and record:

- whether parser and grounder contract behavior is now source-native, a
  build-produced canonical executable, or another non-shim mechanism;
- how the build avoids installing old-name parser/grounder artifacts while
  still using inherited source directories internally as needed;
- which current tests remain baseline-only because engine is still inherited,
  and which must become canonical parser/grounder contract tests;
- how provenance and macOS minimum-OS evidence name parser/grounder after the
  cutover;
- whether release-package helper changes are limited to local dry-run metadata
  and do not claim Arc08 release acceptance;
- whether any stale `pandaPIparser`, `pandaPIgrounder`, or `.legacy` reference
  is historical-only and should remain, or active user/tooling surface and
  must be updated.

If native parser or grounder contract behavior cannot be achieved without the
transition shell shims, stop and ask for an amendment. Do not keep a shim and
call the row done.

## 4. Constraints

- Treat the operator's 2026-08-10 no-compatibility decision in `AGENTS.md`,
  `project-plan.md`, and `arc-plan.md` as current policy.
- Use the top-level `Makefile` for build, test, check, CI, and release helper
  operations.
- New CI behavior must use make targets.
- Keep parser and grounder cutover narrow: supported normal parser/grounder
  managed-process behavior only.
- Do not bulk-format inherited `pandaPI/` source.
- Do not change release publication shape beyond local helper updates needed
  to stop naming parser/grounder old artifacts.
- Required assistant-authored commits must include both co-author trailers from
  `AGENTS.md`.

## 5. Verification Approach

CC verifies by rebuilding parser and grounder, proving only canonical
parser/grounder artifacts exist in `dist/<platform>/`, rerunning contract and
smoke fixtures through make targets, checking provenance/package/min-OS helper
paths, and grep-checking active docs/tooling for stale old-name parser/grounder
surface.

CDC will rerun the ledger commands, inspect the diff for accidental engine or
release-publication expansion, confirm the shim files are deleted, confirm
contract fixtures no longer require inherited parser/grounder commands, and
verify that `make check` passes with parser and grounder under canonical names.

## 6. Exit Criteria

The ledger reaches final status. `dist/<platform>/pandapi-parser` and
`dist/<platform>/pandapi-grounder` are the only parser and grounder artifacts
produced by the build. Parser and grounder contract fixtures pass through the
canonical names, smoke/pipeline tests use canonical parser/grounder plus the
still-inherited engine, and no active README, make, test, fixture, provenance,
package, or script path presents `pandaPIparser`, `pandaPIgrounder`, or
`.legacy` as supported 0.3.0 parser/grounder surface.
