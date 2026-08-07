# chengdu arc01 / slice04 - provenance-notice-gate - slice doc

> Plan-of-record for this slice, per `PROJECT-MANAGEMENT.md` (v2.1). Parent:
> [`../arc-plan.md`](../arc-plan.md). Ledger: [`./ledger.md`](./ledger.md).
> Assignment: [`./cc-prompt.md`](./cc-prompt.md).
>
> This is arc01's final slice. Its close should leave CDC able to verify the
> arc ledger and assemble the arc closing report without discovering new
> provenance or licensing work.

## 1. Goal

Close the public identity contract for the in-tree pandaPI fork. At slice
close, provenance no longer claims that binaries were built directly from
upstream pins plus patches; it records the current chengdu source commit and
the import identities that connect each vendored subtree back to upstream.
Every vendored source surface has explicit NOTICE/license delineation, the
license audit is revised for the in-tree baseline, release notes and manifests
describe the new source-availability story, and the full two-platform
no-fetch build/release gate has evidence.

## 2. Scope

**In:**

- Introduce a committed `vendor.env` as the machine-readable source of truth
  for vendored source identities: prefix, upstream URL, upstream SHA, and
  subtree import commit for each history-imported source tree (`pandaPIparser`,
  `pandaPIgrounder`, `pandaPIengine`, `cpddl`, `h2-fd-preprocessor`,
  `boruvka`, `opts`). `pins.env` remains only the historical 0.1.0/import-point
  record and should no longer be the active provenance checker's source of
  truth.
- Rework `dist/<platform>/provenance.txt`,
  `scripts/check-provenance.sh`, `scripts/package-release.sh`, and
  `.github/release-notes.md.in` around the new provenance model:
  chengdu commit + per-component current source prefix + import identity +
  `patches=none`.
- Retire `scripts/fetch-upstream.sh` from active tooling. If CC keeps a
  historical source-inspection path, it must not live under `scripts/` as an
  active build/release helper and must not be referenced by README or workflow
  source-build paths.
- Add NOTICE files for every vendored source surface that needs public
  provenance/delineation: `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`,
  `cpddl`, `h2-fd-preprocessor`, `boruvka`, `opts`, durable `bliss`, and
  `lpsolve`. Each NOTICE names origin, import or derivation evidence, license,
  chengdu divergence policy, and whether arc01 has modified the source.
- Write `docs/license-audit-v0.2.0.md` as a revision of the 0.1.0 audit for
  the in-tree fork baseline. It must distinguish identity imports, slice03
  patch-dissolution source changes, and future arc02 changes; it must include
  the nested source surfaces surfaced by slice01/slice03.
- Update `licenses/`, `licenses/README.md`, and generated
  `THIRD-PARTY-LICENSES` behavior as needed so release assets satisfy the
  v0.2.0 audit and no text points at the v0.1.0 audit as the current release
  authority.
- Update README maintenance/release/source-build wording so it describes the
  in-tree fork baseline and source availability through chengdu's public repo,
  not upstream repos plus `patches/`.
- Demonstrate the full no-fetch gate: local gate on the current platform, and
  GitHub Actions evidence for Linux and macOS builds/release packaging whose
  logs show no planner-source clone/fetch.
- Leave arc02 with a clean baseline for upstream PR/issue absorption: no
  hidden provenance rewrite, license audit, NOTICE, or patch-bridge cleanup.

**Out (disclosed, not dropped):**

- Absorbing upstream PRs or fixing upstream issues - arc02.
- Publishing the real `v0.2.0` release - arc02. This slice may run packaging
  dry-runs and CI gates, but it does not cut the final tag.
- Behavioral redesigns beyond preserving the 0.1.0 gate behavior - 0.3.0.
- Reworking wolong's fetch/install code - wolong's project work, against the
  frozen chengdu asset shape.

## 3. Constraints

- Preserve the 0.1.0 release asset shape: per-platform
  `pandapi-<tag>-<platform>.tar.gz`, `SHA256SUMS`, `release-manifest.txt`,
  `THIRD-PARTY-LICENSES`, and rendered release notes. Do not add or remove
  public release assets without an amendment.
- No active build or CI path may fetch planner source from
  `panda-planner-dev`, `gitlab.com/danfis`, `galvusdamor`, or archived source
  URLs. Tool/action installation is allowed; planner source fetch is not.
- Provenance must not soften into "current source is upstream SHA." The current
  build source is chengdu's tree at a chengdu commit; upstream SHAs are import
  identities and historical anchors.
- Keep bash 3.2 compatibility in maintained scripts.
- Keep disposable build copies; do not compile directly inside `pandaPI/`.
- Do not make GPL/legal claims stronger than the evidence supports. The audit
  should state evidence and conservative compliance judgment, not pretend to be
  external legal advice.

## 4. Verification approach

CC verifies locally by starting from clean `build/ dist/ release/`, running
the three build scripts, the reworked provenance checker, both smokes,
packaging dry-run, and maintained static checks. CC also runs tamper checks
against the new provenance schema, inspects generated release artifacts for
the new wording, and records GitHub Actions run URLs/log evidence for the
two-platform no-fetch gate. CDC reproduces the local rows where possible,
checks the CI/log evidence, re-walks the NOTICE/audit/license surface, and
then can close arc01's composition ledger.

## 5. Exit criteria

The ledger's 14 rows reach final status. Summary shape: `vendor.env` records
import identities; active provenance and manifests are anchored on chengdu
commit + import identities, not `pins.env`; NOTICE and license-audit v0.2.0
cover every vendored source surface; release notes/source availability no
longer point at `patches/`; local and CI no-fetch gates pass; and arc01 is
ready for its arc-level close.

## 6. Implementation notes

Expected history-imported identities:

| Component | Prefix | Upstream URL | Upstream SHA | Import commit |
|-----------|--------|--------------|--------------|---------------|
| pandaPIparser | `pandaPI/pandaPIparser` | `https://github.com/panda-planner-dev/pandaPIparser.git` | `88c0995c72c5ce2685e9546ec153a3545977ed81` | `ac8e385d` |
| pandaPIgrounder | `pandaPI/pandaPIgrounder` | `https://github.com/panda-planner-dev/pandaPIgrounder.git` | `b04ff7bcca710ae80a1ce73b8c027f0224565afc` | `2b0a6093` |
| pandaPIengine | `pandaPI/pandaPIengine` | `https://github.com/panda-planner-dev/pandaPIengine.git` | `810f04388667db5e3e4f114e960a4efbb43b1ac0` | `f734b692` |
| cpddl | `pandaPI/pandaPIgrounder/cpddl` | `https://gitlab.com/danfis/cpddl.git` | `8c142bab211200213746c75be71810526e834d4a` | `25a278a5` |
| h2-fd-preprocessor | `pandaPI/pandaPIgrounder/h2-fd-preprocessor` | `https://github.com/galvusdamor/h2-fd-preprocessor.git` | `e335433bfc53171a1c215bd49ba39cc5fb1d485b` | `b27e102c` |
| boruvka | `pandaPI/pandaPIgrounder/cpddl/third-party/boruvka` | `https://gitlab.com/danfis/boruvka.git` | `e320cc7b02714e4096a686965fab343735d82bec` | `1f0ac094` |
| opts | `pandaPI/pandaPIgrounder/cpddl/third-party/opts` | `https://github.com/danfis/opts.git` | `819d90e8664f9deda56f0dd8e7b62ea3ab9b2f6d` | `68072cb3` |

`bliss` is not a subtree import. Its NOTICE/audit entry should say it is
durable source derived from cpddl's vendored `bliss-0.73.zip` during slice03,
with cpddl's former memleak patch and chengdu's former C++11 string-literal
patch represented as normal in-tree source history. `lpsolve` remains a
vendored cpddl third-party source directory that is built but not linked into
the shipped grounder binary per the audit evidence.
