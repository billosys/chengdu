# Arc08 Slice09 CDC Verification: readme-entrypoint-refresh

Status: closed and CDC-verified
Date: 2026-08-14
Reviewed commit: `735da684 docs: refresh Arc08 README entrypoint`

## Scope Reviewed

CDC reviewed the Slice09 implementation commit and confirmed the intended
surface:

- `README.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice09-readme-entrypoint-refresh/ledger.md`
- `docs/design-v0.3.0/arc08-pandapi-tutorial-docs/slice09-readme-entrypoint-refresh/closing-report.md`
- Arc08 and project plan status updates

The README now acts as a concise public entry point into the 0.3.0 docs suite.
It describes the in-tree `pandaPI/` source roots, canonical `pandapi-*`
workflow, managed-process behavior, Make-backed local/CI entrypoints, public
tutorial/reference/architecture links, and the distinction between published
v0.2.0 install examples and still-unpublished 0.3.0 release assets.

## Verification Evidence

CDC reproduced the Slice09 evidence independently:

- Static README checks for F-1 through F-5 passed.
- The `docs/index.md` decision row passed: the closing report explicitly
  defers a separate docs landing page because the README remains concise and
  links the public docs suite directly.
- README local links resolve to existing files.
- Public prose checks passed:
  - no internal arc/slice, CC/CDC, ledger, project-plan, or arc-plan
    vocabulary appears in README;
  - no inherited `pandaPIparser`, `pandaPIgrounder`, or `pandaPIengine`
    command names are taught as supported 0.3.0 commands;
  - README does not claim unpublished 0.3.0 release assets, checksums,
    manifests, or wolong proof.
- `wc -l README.md` reported `169 README.md`.
- `make readme-verbatim` passed:
  - runtime, parser, grounder, and engine built for `macos-arm64`;
  - local binaries were populated under `bin/`;
  - positive smoke passed `3 passed, 0 failed`.
- `make smoke && make smoke-negative` passed:
  - positive smoke passed `3 passed, 0 failed`;
  - negative smoke passed `4 passed, 0 failed`.
- `make help` passed and showed the README-named Make targets as current
  entrypoints.
- Make target grep over `Makefile` and `mk/` confirmed the README-named
  targets are present.
- `git diff --check` passed before CDC edits.
- The worktree remained clean after the build/smoke verification runs.

Aggregate `make check` was not run for CDC verification because Slice09 is a
documentation-entrypoint slice and does not change code, Make, CI, tests,
tools, fixtures, release packaging, dependency vendoring, or runtime behavior.
The reproduced gates cover the slice's risk: README wayfinding, documented
source-build/smoke behavior, public vocabulary, release-publication honesty,
protected scope, and whitespace.

## Findings

No CDC blockers were found.

The README is materially better aligned with the current project state than
the prior version. It no longer presents the old three-repo fork shape as the
main story, it points readers into the complete public docs suite, and it uses
`./bin/pandapi-*` for the current source-build path.

The release wording is appropriately conservative. Published install examples
remain v0.2.0-specific, while 0.3.0 asset, checksum, manifest, license/NOTICE,
wolong, and publication proof remain release-preparation work.

The `docs/index.md` deferral is acceptable for this slice. The README is short
enough to serve as the public entry point directly, and Slice10 can still add a
docs landing page if final composition review shows a real wayfinding need.

## Bubble-up

Arc08 A7 is closed at CDC-verified strength for the README entrypoint refresh
and `make readme-verbatim` preservation.

Required bubble-up changes:

- Mark Slice09 closed and CDC-verified in the Arc08 plan.
- Mark Arc08 Slice09 closed and CDC-verified in the project plan.
- Preserve Slice10 docs-synthesis as the next planned Arc08 slice.
