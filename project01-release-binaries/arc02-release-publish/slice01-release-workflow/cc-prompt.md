# CC assignment — chengdu arc02 / slice01 — release-workflow

You are CC, the implementing context for one slice, in `chengdu`
(`~/lab/billosys/chengdu`, remote `billosys/chengdu`, public,
direct-to-main). Read fully before writing anything.

## Read order (before any code)

1. `/AGENTS.md`.
2. `docs/design-v0.1.0/arc02-release-publish/arc-plan.md` (v1.0) —
   especially decisions D1–D3 (versioning, compat floors) and RQ1
   (publish model: direct, with the draft toggle documented).
3. `…/slice01-release-workflow/slice-doc.md` — goal, scope, constraints.
4. `…/slice01-release-workflow/ledger.md` — the specification of done:
   8 rows. `LEDGER-DISCIPLINE.md` Section A protocol.
5. `…/arc01-build-matrix/closing-report.md` — what you inherit and must
   not regress (the 8-job matrix is the arc01 capability; G-7 pins it).

## The work

Reusable-workflow refactor; `release.yml` (tag `v*` → reused build →
package → publish, publish structurally unreachable past a failure);
`scripts/package-release.sh` (+ `check-provenance.sh` reuse);
release-notes template with provenance + compat floors; the
refuse-to-publish demonstration; CDC-5 action bumps; happy-path
pre-release test tag.

## Constraints that bite

- **The refactor moves callers, not logic.** Scripts stay the single
  implementation; if the reusable workflow needs a branch, it goes in a
  script. G-1 and G-7 together mean: same jobs, same gates, zero
  behavior drift — prove it by run-to-run job-list comparison.
- **Tags are cheap, releases are public.** Use clearly-marked
  pre-release/test tags for all rehearsals; clean up test tags; never
  cut `v0.1.0` in this slice (it is slice02's closing act, gated on the
  license audit).
- **The refusal demonstration is mandatory evidence,** not an optional
  flourish — a red run plus a *provably absent* release. Restore state
  afterward and link the restoration commit.
- **Idempotency:** document what re-running a tag's workflow does
  (skip, fail, or update) in the workflow header — whichever behavior
  you implement, it must be stated and true.
- Token/permissions: use the workflow's own `GITHUB_TOKEN` with
  least-privilege `permissions:` (contents: write for releases);
  actionlint will not catch permission errors — expect an iteration
  there and it counts as runner-environment discovery, not against the
  budget, unless a local read of the docs would have caught it.

## Protocol

As before: evidence per row as it lands (attested, links included);
amendment requests over silent deviation; five-iteration budget with
the CI nuance above; at close, `closing-report.md` with the 8-row walk
plus the Part IV bubble-up (this arc has two slices — your bubble-up
shapes slice02's open set, especially anything the packaging revealed
about asset naming/formats, which wolong will consume frozen).
CDC writes `cdc-verification.md`.

## Definition of done

All 8 rows at final status with attested evidence: one green
post-refactor push run, one pre-release test release with full assets,
one red refusal run with absence evidence, zero Node20 warnings.
