# Arc07 Slice08: source-quality-synthesis

Status: open
Opened: 2026-08-13

## Goal

Compose Arc07's source-quality evidence into one release-facing synthesis:
which source classes are first-party obligations, which gates are strict
today, which gates are measured baselines, which items are valid post-0.3.0
budgets, and which items must block release until a later slice or arc closes
them.

This is a synthesis and arc-composition slice. It should not quietly implement
new parser, grounder, engine, dependency, CLI, CI, or release behavior. If the
synthesis finds an Arc07 promise that is not honestly covered by the completed
slices, it must say so and bubble up the next required slice instead of
closing the arc by optimism.

## Background

Arc07 corrected the earlier runtime-only quality framing. All in-tree pandaPI
product source is Chengdu-owned in the fork/product sense, while quality
obligations are controlled by source class:

- first-party maintained source is in scope for formatting, static analysis,
  coverage, unit/seam tests, warning policy, and sanitizer triage;
- Chengdu-owned generators/templates are in scope, and generated defects are
  fixed at the generator/template when the finding belongs there;
- generated output, copied build roots, build output, vendored third-party
  source, dependency-internal source, and third-party generated output are
  excluded from first-party obligations unless a later audit accepts the
  boundary.

Slices01-07 established the current evidence:

- Slice01 classified source-quality gaps and recommended selector/build
  normalization before stricter gates.
- Slice02 added source-class selectors, profile mapping, and compile database
  surfaces.
- Slice03 normalized first-party maintained source names.
- Slice04 added the first-party quality gate scaffold and re-entry map.
- Slice05 added engine component targets and the first engine seam.
- Slice06 root-caused and fixed parser Flex generated warnings.
- Slice07 added grounder component targets, resolved `rss.c`, and preserved
  dependency boundaries.

## In Scope

- Create
  `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-synthesis.md`.
- Reconcile all Arc07 child slices and CDC verifications against the arc
  ledger A1-A9.
- Produce a final source-class policy summary for Arc08 public docs and Arc09
  release prep.
- Produce a component/gate matrix for parser, grounder, engine, and runtime
  across format, static analysis, coverage, unit/seam tests, warning policy,
  sanitizer, generated-warning triage, and process-contract fixtures.
- Classify every remaining quality item as one of:
  - release blocker before `v0.3.0`;
  - accepted measured baseline;
  - accepted deferred/post-0.3.0 budget with reason and re-entry condition;
  - out of first-party scope unless a dependency audit accepts the boundary.
- Explicitly decide whether Arc07 can close after this slice. If yes, write
  `docs/design-v0.3.0/arc07-source-quality-expansion/closing-report.md` as
  CC proposed done. If no, do not write an arc closing report; instead update
  `arc-plan.md` and `project-plan.md` with the required next slice.
- Update `arc-plan.md` and `project-plan.md` with the slice outcome and
  bubble-up.

## Out Of Scope

- Implementing new parser, grounder, or engine source-quality targets.
- Cleaning parser maintained-source debt unless the slice stops for an
  amendment.
- CLI11 migration or gengetopt replacement.
- Dependency audits for cpddl, H2, boruvka, opts, lpsolve, RSS, or CUDD.
- Editing vendored third-party or dependency-internal source.
- Adding new CI/GitHub Actions jobs.
- Public README, tutorial, wolong, release packaging, or publication changes.
- Adding global coverage floors, global warning failure policy, global
  `-Werror`, or global static-analysis thresholds without explicit synthesis
  evidence and release posture.

## Verification Approach

This slice should rely on current Make-backed evidence and durable Arc07
reports. The synthesis must reproduce enough evidence to avoid merely
summarizing old close reports:

- source-quality selectors/profile/naming/compile database reports;
- aggregate warning inventory;
- existing runtime gates;
- engine component gates from Slice05;
- parser generated-warning triage from Slice06;
- grounder component gates from Slice07;
- managed parser, grounder, engine, and pipeline behavior through `make test`;
- safety, actionlint, static analysis, formatting, provenance, and whitespace
  regression checks.

`make check` should run if practical. If it is not run, the closing report must
name the narrower Make targets used and explain the macOS provenance/noise
reason or any other concrete reason.

## Exit Criteria

- `source-quality-synthesis.md` exists and is specific enough for Arc08 and
  Arc09 to consume without re-reading every slice packet.
- The synthesis includes release-blocker, baseline, deferred, post-release,
  and out-of-first-party-scope classifications with concrete re-entry
  conditions.
- Every Arc07 ledger row A1-A9 is either honestly satisfied or identified as a
  blocker that prevents arc close.
- If Arc07 is proposed closed, the arc-level `closing-report.md` walks A1-A9
  and bubbles up to the project.
- The slice ledger is closed row by row, with no silent drops.
- The worktree is clean after commit.
