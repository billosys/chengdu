# chengdu arc02 / slice03 - parser-makefile-pr - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).

## 1. Goal

Absorb upstream pandaPIparser PR
[#21](https://github.com/panda-planner-dev/pandaPIparser/pull/21) as a small
build-system fix in chengdu's in-tree parser fork. At slice close, the parser
makefile has the upstream `pandaPIparser` target split from `all`, the new
`install` target works in a disposable build-source copy, the normal chengdu
build/smoke/package gate still passes, and the upstream author/PR credit is
preserved.

This is the second and final selected upstream PR absorption before the
release-publication slice. It must not reopen deferred parser resource-work or
other non-selected upstream PRs.

## 2. Upstream snapshot at slice open

CDC refreshed primary-source state on 2026-08-08 before opening this slice:

| Item | Current state |
|------|---------------|
| PR | [`panda-planner-dev/pandaPIparser#21`](https://github.com/panda-planner-dev/pandaPIparser/pull/21), "Add \"make install\" target" |
| State | open, non-draft |
| Author / head repo | `rpgoldman` / `rpgoldman/pandaPIparser:make-install` |
| Head SHA | `9735763ffd694d95c287766a447deba980d7deaa` |
| Base | `panda-planner-dev/pandaPIparser:master` at `69016f1046ce221c14d82b7102de49a03e12c192` |
| Merge state | clean (`mergeable=true`, `mergeable_state=clean`, `rebaseable=true`) |
| Size | 1 commit, 1 file, +6/-2 |
| PR commit | `9735763` "Fix install dependency." |
| Commit author | Robert P. Goldman `<rpgoldman@sift.net>` |
| Local applicability | `git apply --check --directory=pandaPI/pandaPIparser /tmp/chengdu-parser-pr21.diff` exited `0` |

The PR changes only `makefile`: `all` now depends on the `pandaPIparser`
binary target, the binary target carries the object-file dependency list, and
`install: pandaPIparser` installs the binary to `${DESTDIR}/bin`.

## 3. Scope

**In:**

- Start from the Slice02 CDC-verified Arc02 state and refresh parser PR #21
  metadata at implementation time. If the PR state/head/author/file set has
  materially changed from the slice-open snapshot, stop and record the change
  before absorbing it.
- Absorb PR #21 into `pandaPI/pandaPIparser/makefile`, preferably by native
  `git cherry-pick -Xsubtree=pandaPI/pandaPIparser ...` so Robert P. Goldman's
  author identity is preserved.
- Add an upstream PR URL trailer to the implementation commit. If native
  cherry-pick is not possible despite the clean slice-open check, explicitly
  credit Robert P. Goldman in the commit body/trailers.
- Verify normal chengdu parser behavior still builds from in-tree source into
  ignored `build/<platform>/source/` and publishes `dist/<platform>/pandaPIparser`.
- Verify the new parser `install` target in a disposable build-source copy:
  create a temporary `DESTDIR`, create its `bin/` directory, run
  `make -C build/<platform>/source/pandaPIparser DESTDIR=<tmp> install`, and
  confirm `<tmp>/bin/pandaPIparser --help` exits `0`.
- Run and record the full local no-fetch gate: parser build, grounder build,
  engine build, provenance check, positive smoke, negative smoke, package
  dry-run, maintained shell/static checks, and diff hygiene.
- Confirm non-selected Arc02 upstream PRs remain outside this diff: engine
  #15, engine #9, and grounder #7 stay no-op/deferred as triaged; parser
  #15/#29 resource work stays deferred.

**Out:**

- No release publication, release notes finalization, tags, GitHub Release, or
  upstream courtesy announcement filing; those belong to slice04.
- No parser normalization/resource fixes for parser issues #15/#29, no parser
  diagnostic work for #23/#24/#25/#28, and no parser license-policy change.
- No engine PR #15 wrapper script, engine PR #9 SAT verifier, grounder PR #7
  Docker work, or grounder issue #8 exit-status policy.
- No wolong fetch-spec or release asset-shape change.

## 4. Constraints

- Do not build in or dirty the tracked `pandaPI/pandaPIparser/` source tree.
  Use chengdu's disposable ignored build-source copy for build/install tests.
- Do not broaden PR #21. Avoid extra makefile style cleanup, install-dir
  redesign, README expansion, or package-manager semantics unless required to
  preserve the upstream behavior locally; disclose any such adaptation.
- Keep wolong's frozen release asset shape untouched: names, `SHA256SUMS`,
  `release-manifest.txt`, tarball contents, and 4-command install path.
- Keep shell changes bash-3.2 compatible. Shell/script changes are not expected
  in this slice unless verification exposes a real local issue.
- Assistant-authored commits use the current project footer convention:
  `Co-authored-by: Codex <noreply@openai.com>` and
  `Co-authored-by: Billo AI <ai-engineering@billo.systems>`.

## 5. Verification approach

CC verifies by recording the refreshed upstream snapshot, the absorption method
and author-credit evidence, the exact parser makefile diff, a parser install
target smoke in a disposable destination, the full local build/smoke/package
gate, static checks, scoped-diff checks, and a clean final worktree. CDC
verifies by re-running practical gates at the docs-close HEAD, inspecting the
implementation commit for native/upstream credit, and confirming this slice did
not absorb unrelated parser/engine/grounder work.

Because `provenance.txt` records the current chengdu commit, CC should be
explicit about whether build/provenance/package gates were run before or after
the docs-close commit. CDC will rebuild at the committed close HEAD either way.

## 6. Exit criteria

All ledger rows reach final status. PR #21 is absorbed with upstream credit.
`make install` works from the disposable parser build-source copy. Normal
parser build, full smoke, provenance, package dry-run, and static checks pass.
The source diff is limited to the parser makefile unless a disclosed local
adaptation is required. The closing report walks every row and bubbles up that
release publication is the next remaining Arc02 slice.
