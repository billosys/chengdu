# Arc 01 closing report: vendor-identity

Closed by: CDC
Date: 2026-08-08
Arc plan: [`arc-plan.md`](arc-plan.md)
Verified base head before arc-close documentation:
`03340003322473bcdec32c9e010fb4caf5a0a4c2`
Verdict: accepted / closed

Slices: 4. Delivered: 4. Deferred: 0. Dropped: 0.
Arc ledger rows: 6. Done: 6. Deferred: 0. No-op: 0.

## Capability verdict

Arc01 delivered its capability: pandaPI and the grounder's former submodules
now live in-tree under `pandaPI/`, with the full-history subtree identities
recorded and checked against the 0.1.0 pins/import points. The build and
release scripts consume the in-tree source through disposable build copies,
not planner-source fetches. The former patch bridge is dissolved into normal
in-tree source history. Provenance, NOTICE, license-audit, release-manifest,
smoke, packaging, and CI gates now describe and verify chengdu's public fork
identity rather than an upstream-at-build-time harness.

The arc intentionally stops before upstream PR/issue absorption. Arc02 starts
from a CI-gated in-tree baseline whose source identity, licensing surface, and
wolong-compatible release shape are explicit.

## Slice walk

| Slice | Outcome | Evidence |
|-------|---------|----------|
| slice01 `subtree-import` | Delivered. Imported parser, grounder, engine, cpddl, h2-fd-preprocessor, and the discovered nested cpddl dependencies `boruvka` and `opts`; removed gitlinks and verified import-tree identity. | [`slice01-subtree-import/cdc-verification.md`](slice01-subtree-import/cdc-verification.md); arc plan v1.1 records the `boruvka`/`opts` bubble-up. |
| slice02 `in-tree-build` | Delivered. Local scripts and GitHub workflows build from in-tree `pandaPI/` source copies while preserving the temporary compatibility bridge. | [`slice02-in-tree-build/cdc-verification.md`](slice02-in-tree-build/cdc-verification.md). |
| slice03 `patch-dissolution` | Delivered. The active patch bridge is retired; durable bliss source and former compatibility fixes are represented as ordinary in-tree source history; provenance reports `patches=none`. | [`slice03-patch-dissolution/cdc-verification.md`](slice03-patch-dissolution/cdc-verification.md). |
| slice04 `provenance-notice-gate` | Delivered. `vendor.env`, new provenance/checking, NOTICE files, `license-audit-v0.2.0.md`, release docs, and no-fetch CI evidence landed. | [`slice04-provenance-notice-gate/cdc-verification.md`](slice04-provenance-notice-gate/cdc-verification.md); GitHub Actions run `31239214748` passed at Slice04 implementation head `257901fcb72b97424df88fd94ad434b19cfb5abb`. |

## Arc ledger walk

| Row | Result | Evidence |
|-----|--------|----------|
| A1 | Done / attested. | All four slice close reports and CDC verification files exist. No slice ledger row was deferred or no-op. Slice04's CDC caveat is documentation-scan wording only: active paths are clean of planner-source fetch behavior. |
| A2 | Done / reproduced. | Slice01 CDC reproduced ancestor and normalized tree-identity checks for the seven full-history subtree imports: parser, grounder, engine, cpddl, h2-fd-preprocessor, boruvka, and opts. `vendor.env` records prefix, origin URL, upstream SHA, and import commit for each. |
| A3 | Done / reconciled. | GitHub Actions run `31239214748` passed for Ubuntu, macOS, README-verbatim, and cross-compat jobs at Slice04 implementation head `257901fcb72b97424df88fd94ad434b19cfb5abb`. Its log scan found no active clone/fetch of `panda-planner-dev`, `gitlab.com/danfis`, `galvusdamor/h2-fd-preprocessor`, or `fetch-upstream`. The remaining checkout full-history fetch is chengdu history for provenance ancestry checks. |
| A4 | Done / reproduced. | Slice04 CDC reproduced local parser, grounder, and engine builds; `check-provenance`; positive smoke (`5 passed, 0 failed`); negative smoke (`4 passed, 0 failed`); package dry-run; checksum verification; and the frozen tarball contents: fixtures, three binaries, and `provenance.txt`. |
| A5 | Done / reproduced. | Slice03 CDC verified `git ls-files patches 'pandaPI/pandaPIgrounder/*.patch'` empty, active scripts/docs/workflows free of patch-bridge references, and commit `0958e760` carrying `Dissolves-Patch` trailers for the retired bridge patches. Slice04 provenance continues to report `patches=none`. |
| A6 | Done / reproduced. | Slice04 CDC reproduced the nine `NOTICE` files under `pandaPI/`, `docs/license-audit-v0.2.0.md`, current release packaging with regenerated `THIRD-PARTY-LICENSES`, and `release-manifest.txt` naming `chengdu_commit`, `source_prefix`, `upstream_sha`, and `import_commit`. |

## Composition check

The slices compose in the intended order:

1. Slice01 moved source identity into chengdu history without source edits.
2. Slice02 repointed the build to that in-tree source without changing the
   planner baseline.
3. Slice03 converted compatibility changes from build-time patch inputs into
   normal fork history.
4. Slice04 re-anchored public provenance, NOTICE, licensing, and CI evidence on
   the resulting in-tree fork.

No arc-level contradiction remains. The only broad-scan caveat is that design
docs mention the retired `fetch-upstream` helper as historical evidence; active
build, workflow, README, and release-note paths no longer use it. No generated
build outputs are tracked under `pandaPI/`, and generated local artifacts remain
ignored under `build/`, `dist/`, and `release/`.

## Accumulated arc-plan changes

- v1.1: Slice01 surfaced cpddl nested gitlinks at `third-party/boruvka` and
  `third-party/opts`; arc layout, identity table, OQ1, and OQ4 were updated.
- v1.2: Slice02 resolved scratch-build handling by using ignored disposable
  source copies under `build/`.
- v1.3: Slice03 resolved durable bliss representation under cpddl's
  `third-party/bliss/`.
- v1.4: Slice04 fixed the license-audit boundary around identity imports,
  patch-dissolution deltas, nested third-party source surfaces, and public
  source availability through chengdu.

All arc-plan changes are incorporated in [`arc-plan.md`](arc-plan.md). No
additional arc-plan update is required after close.

## Bubble-up to project

Project plan status is updated to mark arc01 closed and arc02 ready for
detailed planning. No project roadmap scope changes are needed.

Project ledger disposition:

| Project row | Arc01 disposition |
|-------------|-------------------|
| P1 | Substantially satisfied for all arc01 imported components. The final project close should rerun or cite the subtree identity checks at the release head after arc02 lands. |
| P2 | Satisfied for the arc01 baseline by the current full CI no-fetch gate. The final project close should repeat this against the actual `v0.2.0` release head. |
| P3 | Not owned by arc01; remains assigned to arc02 upstream absorption. |
| P4 | Satisfied for the arc01 baseline by NOTICE files, `license-audit-v0.2.0.md`, and current release packaging. Recheck if arc02 changes any source or linkage surface. |
| P5 | Not owned by arc01; remains assigned to arc02 release publication and wolong fetch-spec verification. |

Arc01 is closed. The next work is detailed planning for arc02
`upstream-absorption`.
