# CC assignment - chengdu arc02 / slice03 - parser-makefile-pr

You are CC, the implementing context for one source-changing slice, working in
the `chengdu` repo (`~/lab/billosys/chengdu`) on `release/0.2.x`. This file is
your assignment; read it fully before writing anything.

## Read order (before any work)

1. `/AGENTS.md` - standing project contracts, release policy, commit footer
   convention, and wolong fetch-spec constraint.
2. `docs/design-v0.2.0/project-plan.md` - 0.2.0 DoD, Arc02 boundary, and
   project ledger P3/P5.
3. `docs/design-v0.2.0/arc02-upstream-absorption/arc-plan.md` - current
   absorption queue and Arc02 ledger.
4. `docs/design-v0.2.0/arc02-upstream-absorption/slice02-engine-exit-status/closing-report.md`
   and `docs/design-v0.2.0/arc02-upstream-absorption/slice02-engine-exit-status/cdc-verification.md`
   - the verified prior source absorption.
5. `docs/design-v0.2.0/arc02-upstream-absorption/slice03-parser-makefile-pr/slice-doc.md`
   - goal, scope, constraints, upstream snapshot, and exit criteria.
6. `docs/design-v0.2.0/arc02-upstream-absorption/slice03-parser-makefile-pr/ledger.md`
   - the specification of done. Thirteen rows; every one reaches final status
   before this slice advances. Follow the CC protocol in the collaboration
   framework's `LEDGER-DISCIPLINE.md` Section A.

## The work

Absorb pandaPIparser PR
[#21](https://github.com/panda-planner-dev/pandaPIparser/pull/21) as a bounded
makefile/install-target change:

- Refresh upstream PR #21 at the start of implementation. Record state, head
  SHA, author/commit metadata, changed-file summary, and applicability result
  in the ledger/closing report. Do not rely only on the slice-open snapshot if
  GitHub is reachable.
- If PR #21 still matches the slice-open shape, use the project-preferred
  native absorption path, for example fetching the PR ref and running
  `git cherry-pick -Xsubtree=pandaPI/pandaPIparser <commit>`, so Robert P.
  Goldman's author identity is preserved.
- Add the upstream PR URL to the implementation commit:
  `Upstream-PR: https://github.com/panda-planner-dev/pandaPIparser/pull/21`.
- If a clean native cherry-pick unexpectedly fails, do a minimal manual port,
  credit Robert P. Goldman `<rpgoldman@sift.net>` explicitly, and disclose why.
- Keep the code diff bounded to `pandaPI/pandaPIparser/makefile` unless a local
  verification issue requires a disclosed adaptation.
- Verify the normal chengdu build path still works through
  `./scripts/build-parser.sh`.
- Verify the new install target in the ignored build-source copy, not in the
  tracked vendored tree. A suitable smoke is:

```bash
./scripts/build-parser.sh
tmp="$(mktemp -d)"
mkdir -p "$tmp/bin"
PLATFORM="$(bash -c '. scripts/lib-platform.sh; detect_platform')"
make -C "build/$PLATFORM/source/pandaPIparser" DESTDIR="$tmp" install
"$tmp/bin/pandaPIparser" --help
git status --short -- pandaPI
```

The point is: disposable destination, disposable build-source copy, no tracked
`pandaPI/` dirt.
- Run the full local no-fetch gate and static checks listed below.
- Close the ledger and write `closing-report.md` with a row-by-row walk and a
  Bubble-up to the arc/project.

## Constraints

- No release publication, tags, GitHub Release, release announcement issues, or
  wolong fetch-spec change.
- No engine PR #15 wrapper script, engine PR #9 SAT verifier, grounder PR #7
  Docker work, grounder issue #8, or parser #15/#29 resource/normalization
  fixes.
- No broad makefile cleanup, install-layout redesign, README expansion, or
  package-manager semantics beyond PR #21 unless needed to keep the upstream
  behavior working locally.
- Do not build in tracked `pandaPI/`; use ignored `build/` copies.
- Keep shell script changes bash-3.2 compatible. None are expected.
- Do not write `cdc-verification.md`; CDC writes that after independent
  verification.

## Credit and commits

Expected implementation commit:

- Native author metadata: Robert P. Goldman `<rpgoldman@sift.net>`.
- Upstream PR trailer:
  `Upstream-PR: https://github.com/panda-planner-dev/pandaPIparser/pull/21`.
- Required assistant co-author trailers:
  - `Co-authored-by: Codex <noreply@openai.com>`
  - `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

A clean split is preferred: one PR absorption implementation commit, then one
docs-close commit.

## Verification commands

Run the strongest practical local gate and record exact results:

```bash
./scripts/build-parser.sh
# parser install-target smoke in build/<platform>/source/pandaPIparser
./scripts/build-grounder.sh
./scripts/build-engine.sh
./scripts/check-provenance.sh
./scripts/smoke-test.sh
./scripts/smoke-test.sh --negative
./scripts/package-release.sh v0.2.0-slice03-smoke
bash -n scripts/*.sh
/bin/bash -n scripts/*.sh
shellcheck scripts/*.sh
actionlint .github/workflows/*.yml
git diff --check
git status --short --branch
```

Because provenance records the current Git commit, be explicit in the close
report about which commit the build/provenance/package gate was run against.
If the docs-close commit follows the gate, say so; CDC will rebuild at the
committed close HEAD.

If a local tool is unavailable, record the exact missing tool/version issue and
which later CDC/CI gate must cover it. Do not mark a row done silently.

## Definition of done

All 13 ledger rows at final status with concrete evidence; parser PR #21 is
absorbed with upstream credit; `make install` works from the disposable
build-source copy; parser/grounder/engine builds, provenance, positive smoke,
negative smoke, package dry-run, and static checks pass; non-selected upstream
work remains untouched; worktree is clean; `closing-report.md` exists; no
`cdc-verification.md` is written by CC.
