# Arc08 Slice09 Closing Report: readme-entrypoint-refresh

Status: implementation proposed done
Date: 2026-08-14

## Scope Delivered

Slice09 refreshed the top-level `README.md` as the public entry point into the
0.3.0 documentation suite. The README now:

- describes chengdu as the public fork and release home for the pandaPI HTN
  planning toolchain;
- names the in-tree `pandaPI/` source roots and canonical `pandapi-parser`,
  `pandapi-grounder`, and `pandapi-engine` workflow;
- links the tutorial sequence, managed-process guide, CLI reference,
  migration guide, and architecture/source-quality page;
- keeps published install examples on v0.2.0 while stating that 0.3.0 release
  assets, checksums, manifest, license/NOTICE bundles, and wolong proof are not
  published yet;
- preserves the `make readme-verbatim` source-build path and names the smoke
  and test gates.

`docs/index.md` was not created. The README remains concise at 169 lines,
down from 187 lines before this slice, and links the public docs suite
directly. A separate docs landing page is deferred to Slice10 docs-synthesis
if final composition review shows that wayfinding still needs it.

No code, Make, CI, release assets, tests, tools, fixtures, implementation
source, `pandaPI/`, or dependency-vendoring surfaces were changed.

## Verification

- `rg -n -- "chengdu|pandaPI|HTN|HDDL|pandaPI/|pandapi-parser|pandapi-grounder|pandapi-engine|managed-process|Make" README.md`
- `rg -n -- "v0\\.2\\.0|0\\.3\\.0|release preparation|published|not.*published|Arc09|checksums|manifest|wolong|when.*released|until.*release" README.md`
- `rg -n -- "make readme-verbatim|\\.\\/bin\\/pandapi-parser|\\.\\/bin\\/pandapi-grounder|\\.\\/bin\\/pandapi-engine|make smoke|make smoke-negative|make test" README.md`
- `rg -n -- "docs/tutorial/01-htn-hddl-onboarding\\.md|docs/tutorial/02-first-project-workflow\\.md|docs/tutorial/03-model-a-feature-as-htn\\.md|docs/tutorial/04-write-hddl-files\\.md" README.md`
- `rg -n -- "docs/managed-process\\.md|docs/reference/cli\\.md|docs/migration\\.md|docs/architecture\\.md" README.md`
- README local-link check passed after removing a stale `docs/license-audit-v0.2.0.md` link.
- Public vocabulary checks passed: README does not mention internal arc/slice,
  CC/CDC, ledger, closing-report, project-plan, or arc-plan vocabulary.
- Inherited command-name check passed: README does not teach
  `pandaPIparser`, `pandaPIgrounder`, or `pandaPIengine`.
- Unpublished 0.3.0 release-asset check passed.
- `wc -l README.md` reported 169 lines.
- `make readme-verbatim` passed.
- `make smoke && make smoke-negative` passed.
- `make help && rg -n -- "readme-verbatim|smoke|smoke-negative|test|enable-dev-strict|dev-strict-status" mk Makefile` passed.
- Post-staging protected-boundary and whitespace checks passed.

Aggregate `make check` was not run. This was a documentation-entrypoint slice
with no code, Make, CI, test, fixture, release, package, or tool behavior
changes. The narrower gates above cover the changed public README surface and
the README-named build/smoke path.

## Ledger Walk

- **F-1 - done.** README now gives a current chengdu/pandaPI overview with
  HTN/HDDL workflow, in-tree `pandaPI/` roots, canonical `pandapi-*`
  commands, managed-process behavior, and Make-backed local/CI behavior.
- **F-2 - done.** README separates published v0.2.0 install examples from
  0.3.0 release-preparation proof that remains unpublished.
- **F-3 - done.** README documents `make readme-verbatim`,
  `./bin/pandapi-parser`, `./bin/pandapi-grounder`, `./bin/pandapi-engine`,
  `make smoke`, `make smoke-negative`, and `make test`.
- **F-4 - done.** README links all four tutorial pages.
- **F-5 - done.** README links managed-process, CLI reference, migration, and
  architecture docs.
- **F-6 - done.** `docs/index.md` is explicitly deferred because README links
  the docs suite directly and remains concise; Slice10 can revisit the need.
- **F-7 - done.** README local links resolve; the stale local v0.2.0 license
  audit link was removed.
- **F-8 - done.** `make readme-verbatim` passed.
- **F-9 - done.** `make smoke && make smoke-negative` passed.
- **F-10 - done.** `make help` and target grep passed for README-named Make
  entrypoints.
- **F-11 - done.** Public README prose avoids internal project-management
  vocabulary.
- **F-12 - done.** README does not teach inherited `pandaPI*` command names as
  supported 0.3.0 commands.
- **F-13 - done.** README does not claim unpublished 0.3.0 assets, checksums,
  manifest, or wolong proof.
- **F-14 - done.** README is 169 lines and delegates details to public docs.
- **F-15 - done.** Arc08 plan and project plan mark Slice09 implementation
  proposed done, reference README/closing evidence, record the `docs/index.md`
  deferral, and keep Slice10 docs-synthesis next.
- **F-16 - done.** Staged protected-boundary check passed; the slice did not
  touch release, CI, Make, tests, tools, fixtures, implementation, `pandaPI/`,
  or dependency-vendoring scope.
- **F-17 - done.** `git diff --check && git diff --cached --check` passed
  after staging.
- **F-18 - done.** This closing report exists and walks F-1 through F-18.

## Bubble-Up

Slice10 docs-synthesis should verify final suite composition: all public docs
links, examples, README references, status/exit wording, and Arc09 handoff
language should compose without duplication or contradiction.

Arc09 remains responsible for real release assets, checksums,
manifest/provenance, license/NOTICE release proof, test-only dependency
exclusion, release notes, wolong fetch/install/migration verification, and
publication of v0.3.0.
