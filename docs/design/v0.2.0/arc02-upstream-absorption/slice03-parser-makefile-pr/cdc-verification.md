# CDC verification - Slice 03: parser-makefile-pr

Verified by CDC on 2026-08-08 for `release/0.2.x`.

## Verdict

Accepted. CC's Slice03 closure is reproduced at close HEAD
`8810395c6c77e28c85895d6289a78821f7fad585`.

Scope checked: opening commit `e861c1fe` through docs-close commit
`8810395c`. The implementation commit
`49c02f7f0cb7a68c9d30df66992c172e111b3641` preserves Robert P. Goldman
`<rpgoldman@sift.net>` as author, includes
`Upstream-PR: https://github.com/panda-planner-dev/pandaPIparser/pull/21`,
and has the required assistant co-author trailers.

## Upstream Recheck

CDC refreshed PR #21 through the GitHub API on 2026-08-08:

| Claim | CDC result |
|-------|------------|
| PR state | open, non-draft |
| Title | `Add "make install" target` |
| Head | `rpgoldman/pandaPIparser:make-install` at `9735763ffd694d95c287766a447deba980d7deaa` |
| Base | `master` at `69016f1046ce221c14d82b7102de49a03e12c192` |
| Mergeability | `mergeable=true`, `mergeable_state=clean`, `rebaseable=true` |
| Size | 1 commit, 1 changed file, +6/-2 |
| Commit author | Robert P. Goldman `<rpgoldman@sift.net>` |

This matches the slice-open and CC close snapshots.

## Diff and Credit Checks

| Check | Result |
|-------|--------|
| `git show --stat --format=fuller 49c02f7f` | pass; author is Robert P. Goldman; one file changed, `pandaPI/pandaPIparser/makefile`, +6/-2 |
| `git diff e861c1fe..49c02f7f -- pandaPI/pandaPIparser/makefile` | pass; bounded to PR #21 shape |
| `rg -n '^all: pandaPIparser$|^pandaPIparser:|^install: pandaPIparser|install pandaPIparser' pandaPI/pandaPIparser/makefile` | pass; lines 26, 28, 65, and 66 match |
| `git diff --name-only e861c1fe..49c02f7f` | pass; only `pandaPI/pandaPIparser/makefile` |
| `git diff --name-only e861c1fe..HEAD -- pandaPI/pandaPIengine pandaPI/pandaPIgrounder` | pass; no output |

## Reproduced Gates

CDC first reproduced the generated-output caveat: after ordinary rebuilds,
`./scripts/check-provenance.sh` failed because the first provenance blocks in
`dist/macos-arm64/provenance.txt` still named implementation commit
`49c02f7f...` while `HEAD` was `8810395c...`.

CDC removed the stale ignored generated provenance file and rebuilt all three
artifacts at `8810395c`. The close-HEAD gate then passed:

| Gate | Result |
|------|--------|
| `./scripts/build-parser.sh` | pass; produced `dist/macos-arm64/pandaPIparser` |
| parser install smoke from `build/macos-arm64/source/pandaPIparser` | pass; `make DESTDIR=<tmp> install`, executable check, `--help`, and `git status --short -- pandaPI` all succeeded |
| `./scripts/build-grounder.sh` | pass; produced `dist/macos-arm64/pandaPIgrounder` |
| `./scripts/build-engine.sh` | pass; produced `dist/macos-arm64/pandaPIengine` |
| `./scripts/check-provenance.sh` | pass after stale generated provenance was removed and all three artifacts rebuilt |
| `./scripts/smoke-test.sh` | pass: 5 passed, 0 failed |
| `./scripts/smoke-test.sh --negative` | pass: 4 passed, 0 failed |
| `./scripts/package-release.sh v0.2.0-slice03-smoke` | pass; provenance checked, tarball, licenses, manifest, checksums, and notes rendered for `macos-arm64` |
| `bash -n scripts/*.sh` | pass |
| `/bin/bash -n scripts/*.sh` | pass |
| `shellcheck scripts/*.sh` | pass |
| `actionlint .github/workflows/*.yml` | pass |
| `git diff --check` | pass |

Existing compiler warnings remain warning-only and are not new Slice03 signal.

## Ledger Verification

Rows verified: 13. Done: 13. Deferred: 0. No-op: 0.

| Row | CDC disposition |
|-----|-----------------|
| F-1 | Reproduced. Current branch is `release/0.2.x`; Slice02 close and CDC verification exist; worktree was clean before CDC wrote this file. |
| F-2 | Reproduced. GitHub API refresh matches CC's PR #21 state, head SHA, author identity, mergeability, and one-file +6/-2 summary. |
| F-3 | Reproduced. `49c02f7f` is a native author-preserving implementation commit for `pandaPI/pandaPIparser/makefile`. |
| F-4 | Reproduced. The makefile contains `all: pandaPIparser`, a named `pandaPIparser:` target with object dependencies, and `install: pandaPIparser` to `${DESTDIR}/bin`. |
| F-5 | Reproduced. Parser build passed from the ignored source copy and produced the expected `dist` binary. |
| F-6 | Reproduced. Install smoke passed in a disposable `DESTDIR`; tracked `pandaPI/` stayed clean. |
| F-7 | Reproduced with caveat discharged. The stale-provenance failure occurred as predicted, then passed after removing generated provenance and rebuilding all three artifacts at close HEAD. |
| F-8 | Reproduced. Package dry-run preserved the local release artifact shape and passed checksum/manifest generation. |
| F-9 | Reproduced. Bash syntax checks, shellcheck, actionlint, and diff hygiene passed. |
| F-10 | Reproduced. Source implementation diff is limited to the parser makefile; non-selected upstream work remains untouched. |
| F-11 | Reproduced. Implementation commit has upstream author metadata, upstream PR URL, and required assistant trailers. |
| F-12 | Reproduced. No nested `.git`, no mode `160000` gitlinks under `pandaPI/`, one worktree, expected `origin git@github.com:billosys/chengdu.git`, and no tracked `pandaPI/` dirt. |
| F-13 | Reproduced. Closing report exists, walks all 13 rows, and includes Bubble-up; CC did not write this CDC file. |

## Bubble-up Check

Delivered-as-assigned: yes. Slice03 absorbed the second and final selected
upstream PR into the parser makefile with credit preserved and no unrelated
upstream work.

Silent-drop diff: complete. No PR content was dropped, and no deferred engine,
grounder, parser resource/normalization, publication, or wolong asset-shape
work was smuggled into this slice.

Arc/project plan update: not required before planning Slice04. The next and
last planned Arc02 slice remains release publication. The only operational
learning to carry forward is provenance hygiene: rebuild from a clean generated
provenance file at the final commit whose artifacts are being checked.

## What Worked

- Native `cherry-pick -Xsubtree` gave the desired upstream credit shape with a
  very small source diff.
- The install smoke exercised the new target in the ignored build-source copy,
  which kept the tracked vendored tree clean.
- The stale provenance failure was easy to reproduce and discharge because the
  close report named it plainly.
