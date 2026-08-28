# Slice 03 closing report - parser-makefile-pr

Closed by CC on 2026-08-08 for `release/0.2.x`.

## Summary

Delivered as assigned. Upstream pandaPIparser PR #21 was refreshed, found still
open and unchanged from the slice-open shape, and absorbed by native subtree
cherry-pick in implementation commit
`49c02f7f0cb7a68c9d30df66992c172e111b3641`.

The parser makefile now splits the `pandaPIparser` binary target from `all` and
adds the upstream `install: pandaPIparser` target that installs the binary to
`${DESTDIR}/bin`. No parser resource/normalization work, engine wrapper/SAT
verifier work, grounder Docker work, release publication, or wolong release
shape change was introduced.

## Upstream Refresh

Implementation-time refresh on 2026-08-08:

| Item | Evidence |
|------|----------|
| PR | `panda-planner-dev/pandaPIparser#21`, `Add "make install" target` |
| State | open, non-draft |
| Head | `9735763ffd694d95c287766a447deba980d7deaa` |
| Base | `69016f1046ce221c14d82b7102de49a03e12c192` on `master` |
| Merge state | `mergeable=true`, `mergeable_state=clean`, `rebaseable=true` |
| Size | 1 commit, 1 file, +6/-2 |
| Author | Robert P. Goldman `<rpgoldman@sift.net>` |
| Applicability | `git apply --check --directory=pandaPI/pandaPIparser /tmp/chengdu-parser-pr21.patch` exited 0 |

The PR patch was saved to `/tmp/chengdu-parser-pr21.patch`. Its local shape
matched the slice-open snapshot: one makefile changed, `all` depends on the
binary target, object dependencies move to `pandaPIparser:`, and `install`
copies the binary to `${DESTDIR}/bin`.

## Implementation

Commit:
`49c02f7f0cb7a68c9d30df66992c172e111b3641` -
`Fix install dependency.`

Absorption path:

```text
git fetch https://github.com/panda-planner-dev/pandaPIparser.git refs/pull/21/head
git cherry-pick -Xsubtree=pandaPI/pandaPIparser FETCH_HEAD
```

The cherry-pick applied cleanly. The commit message was amended only to add
the required trailers. Author metadata remains:

```text
Author: Robert P. Goldman <rpgoldman@sift.net>
```

Implementation commit trailers include:

```text
Upstream-PR: https://github.com/panda-planner-dev/pandaPIparser/pull/21
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```

Functional diff:

| Path | Change |
|------|--------|
| `pandaPI/pandaPIparser/makefile` | Adds `all: pandaPIparser`, moves object dependencies to `pandaPIparser:`, and adds `install: pandaPIparser` with `install pandaPIparser ${DESTDIR}/bin`. |

## Verification

Local gates were run on `macos-arm64` at implementation commit
`49c02f7f0cb7a68c9d30df66992c172e111b3641`.

| Gate | Result |
|------|--------|
| `./scripts/build-parser.sh` | pass; produced `dist/macos-arm64/pandaPIparser`; existing compiler warnings only |
| parser install smoke | pass; `make -C build/macos-arm64/source/pandaPIparser DESTDIR=<tmp> install`, `test -x <tmp>/bin/pandaPIparser`, and `<tmp>/bin/pandaPIparser --help` all succeeded |
| `git status --short -- pandaPI` after install smoke | pass; no output |
| `./scripts/build-grounder.sh` | pass; existing compiler warnings only |
| `./scripts/build-engine.sh` | pass; existing compiler warnings only |
| `./scripts/check-provenance.sh` | pass after clearing stale ignored `dist/macos-arm64/provenance.txt` and rebuilding parser, grounder, and engine |
| `./scripts/smoke-test.sh` | pass: 5 passed, 0 failed |
| `./scripts/smoke-test.sh --negative` | pass: 4 passed, 0 failed |
| `./scripts/package-release.sh v0.2.0-slice03-smoke` | pass; built macos-arm64 dry-run tarball, licenses, manifest, checksums, and notes |
| `bash -n scripts/*.sh` | pass |
| `/bin/bash -n scripts/*.sh` | pass |
| `shellcheck scripts/*.sh` | pass |
| `actionlint .github/workflows/*.yml` | pass |
| `git diff --check` | pass |

The first provenance check saw stale ignored generated output from commit
`1ea6038706c32e941f665d95c3f53ccf65e495a9`. Removing
`dist/macos-arm64/provenance.txt` and rebuilding all three artifacts generated
fresh provenance for `49c02f7f0cb7a68c9d30df66992c172e111b3641`.

Because this docs-close commit follows the build/provenance/package gate, CDC
should rebuild generated artifacts at the committed close HEAD before writing
`cdc-verification.md`.

## Ledger Walk

| Row | Close |
|-----|-------|
| F-1 | Done. Slice started on `release/0.2.x`; Slice02 closing and CDC verification files were present; the worktree was clean except for the expected slice-open docs commit ahead of origin. |
| F-2 | Done. PR #21 was refreshed with state, head, base, mergeability, author identity, changed-file summary, and patch applicability. |
| F-3 | Done. Native subtree cherry-pick succeeded and preserved Robert P. Goldman's author metadata. |
| F-4 | Done. The makefile diff is bounded to PR #21 behavior: `all: pandaPIparser`, `pandaPIparser:` with object dependencies, and `install: pandaPIparser` to `${DESTDIR}/bin`. |
| F-5 | Done. Normal parser build passed and produced `dist/macos-arm64/pandaPIparser`. |
| F-6 | Done. The install target worked from the ignored build-source copy into a disposable destination, and `pandaPI/` stayed clean. |
| F-7 | Done. Parser, grounder, engine, provenance, positive smoke, and negative smoke passed locally at the implementation commit after clearing stale ignored provenance. |
| F-8 | Done. Package dry-run passed for `v0.2.0-slice03-smoke` without publication. |
| F-9 | Done. Bash syntax checks, shellcheck, actionlint, and diff hygiene passed. |
| F-10 | Done. The only source diff is `pandaPI/pandaPIparser/makefile`; non-selected upstream work stayed deferred. |
| F-11 | Done. Implementation commit preserves upstream author metadata and includes the upstream PR URL plus required co-author trailers. |
| F-12 | Done. No nested `.git`, gitlink, extra worktree, temporary remote, or tracked generated vendor output was introduced. |
| F-13 | Done. This report and the ledger provide row-by-row closure and Bubble-up; no `cdc-verification.md` was written by CC. |

## Bubble-up

Delivered-as-assigned: yes.

Install-target caveat: PR #21 installs to `${DESTDIR}/bin` and assumes that
directory exists. The smoke created `<tmp>/bin` before `make install`, matching
the slice instruction and preserving upstream behavior without redesign.

Arc/project docs changed: no parent arc or project-plan update was needed. This
slice closes the second and final selected upstream PR absorption before release
publication.

Silent-drop diff: no manual PR content was dropped because the native
cherry-pick applied cleanly. Non-selected upstream work remains out of scope:
engine PR #15, engine PR #9, grounder PR #7, grounder issue #8, and parser
#15/#29 resource/normalization work.

CDC handoff: rebuild generated artifacts from the close HEAD, rerun practical
gates, inspect implementation commit credit, and write `cdc-verification.md`.
