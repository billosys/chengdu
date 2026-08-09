# Closing Report: cli-naming-version-migration

Date: 2026-08-09

Branch: `release/0.3.x`

## Result

Slice 04 is CC-complete. It produced the Arc03 CLI naming, version,
provenance, help, compatibility, and migration contract report; closed all 12
ledger rows; preserved the design-only boundary; and left
`cdc-verification.md` for CDC.

The main artifact is:

`docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`

## Delivered Scope

The slice report decides:

- canonical command names: `pandapi-parser`, `pandapi-grounder`,
  `pandapi-engine`;
- 0.3.0 compatibility transition: inherited `pandaPIparser`,
  `pandaPIgrounder`, and `pandaPIengine` remain executable;
- release asset/install-shape implications for wolong's frozen fetch/install
  contract;
- command synopsis and argument semantics for the three supported normal
  paths;
- `--supervised`, `--status`, `--status=stderr|stdout`, `--quiet`,
  `--verbose`, `--output`, `--color`, `--no-color`, and `--no-colour`
  semantics;
- help, usage, examples, parse-error, version, and provenance behavior;
- legacy, experimental, unsupported, and future surface presentation;
- CLI11 adoption as a conditional Arc04/Arc05 implementation candidate behind
  golden-output tests.

## Ledger Walk

The opening ledger contained 12 rows. All 12 are closed in
`slice04-cli-naming-version-migration/ledger.md`.

| Row | Status | CC evidence |
|-----|--------|-------------|
| F-1 | closed | The report exists and names Arc01, Arc02, slice01, slice02, slice03, `audit-synthesis-pandapi`, `combined-library-recommendations`, `supported-surface-classification`, `status-exit-signal-taxonomy`, and `stdio-event-tty-contract` as inputs. |
| F-2 | closed | The report defines `pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` as canonical names and maps `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` to compatibility alias/symlink/wrapper policy. |
| F-3 | closed | The report states 0.3.0 is a compatibility transition, preserves asset names, `SHA256SUMS`, `release-manifest.txt`, the four-command install path, and inherited executable names for wolong. |
| F-4 | closed | The report defines supported parser normal HDDL parse, grounder normal `.htn` grounding, and engine normal search synopsis and argument semantics without expanding optional or legacy surfaces. |
| F-5 | closed | The report defines human CLI defaults, `--supervised`, `--status`, `--status=stderr|stdout`, tagged `PANDAPI_STATUS`, and stdout/stderr ownership preservation. |
| F-6 | closed | The report defines help, usage, examples, parse-error behavior, help `ok`/`0`, malformed CLI `cli_usage_error`/`10`, and no continuation into input/model processing after CLI errors. |
| F-7 | closed | The report defines `--version` and `--provenance` fields including component, chengdu version, upstream pandaPI provenance, source/build/platform/compiler, license/NOTICE, and exit `ok`/`0`. |
| F-8 | closed | The report defines `--color`, `--no-color`, `--no-colour`, `NO_COLOR`, ANSI, TTY, stderr/stdout, human/supervised/machine, and compatibility behavior. |
| F-9 | closed | The report maps parser verifier/output helpers, H2, cpddl/FAM, interactive, translation, SAT, and BDD/CUDD to legacy/experimental/unsupported/future help, hidden-option, diagnostic, and status treatment. |
| F-10 | closed | The report preserves CLI11 golden-output gating, fmt behind diagnostics/process, `nlohmann/json` held, Catch2/process fixtures slice05/Arc04-owned, reproc++ conditional, and Abseil/Boost.Process rejected. |
| F-11 | closed | The report routes slice05 contract tests and golden fixtures, Arc04 shared CLI/diagnostics substrate, Arc05 per-binary migration, and Arc06 release docs/license/NOTICE/behavior-change/wolong verification. |
| F-12 | closed | The protected-path check returned no paths, confirming no planner source, dependency source, scripts, workflows, release assets, build outputs, or vendored dependency files changed. |

## Verification Evidence

Row count:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/ledger.md
```

Result: `12`.

Open-row check:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/ledger.md
```

Result: no output.

Protected-path check:

```bash
git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Whitespace check:

```bash
git diff --check
```

Result: no output.

## Silent-Drop Diff

Scope requested and delivered:

- canonical `pandapi-*` names;
- inherited `pandaPI*` compatibility policy;
- compatibility-transition decision for release assets and wolong;
- command synopsis and argument semantics for supported normal paths;
- final policy-level option spelling for supervised and tagged status behavior;
- help/version/provenance and CLI parse-error behavior;
- color/no-color/no-colour/NO_COLOR policy;
- non-supported surface presentation;
- CLI11 disposition after semantic contract definition.

Scope intentionally not delivered:

- planner source changes, dependency source changes, scripts, workflows,
  release assets, build outputs, `vendor.env`, and `pins.env`: out of scope
  for Arc03 design-only work;
- final contract test matrix: routed to slice05;
- implementation of aliases, wrappers, symlinks, binary renaming, CLI11, or
  shared CLI helpers: routed to Arc04/Arc05;
- release documentation, behavior-change table, license/NOTICE updates, and
  wolong verification: routed to Arc06;
- any breaking rename: not accepted by this report and requires explicit
  operator approval.

No requested in-scope ledger row remains open.

## Bubble-Up To Arc03

Slice04 delivers the Arc03 A5 input: the final CLI naming, compatibility,
help/version/provenance, parse-error, color, status-option, and wolong
migration policy.

Arc03 can now plan slice05 from stable command semantics:

- tests must cover both canonical `pandapi-*` and inherited `pandaPI*`
  compatibility entry points;
- `--status` is the final machine-status enablement spelling;
- `--supervised` is the final supervised/process-manager spelling;
- `--no-color` is canonical and `--no-colour` remains a compatibility spelling;
- help, version, and provenance are successful informational commands;
- malformed CLI shape is `cli_usage_error`/`10` and must stop before input
  processing.

No Arc03 slice breakdown change is required before slice05. The existing
arc-plan already routes contract tests to slice05, shared CLI/diagnostics
substrate to Arc04, per-binary migration to Arc05, and release docs plus
wolong verification to Arc06.

## CDC Handoff

CDC should verify the row count, reproduce all 12 ledger Verify commands,
inspect that only the report, ledger, and this closing report changed, and
confirm that the bubble-up gives slice05 enough CLI semantics to write golden
process fixtures.
