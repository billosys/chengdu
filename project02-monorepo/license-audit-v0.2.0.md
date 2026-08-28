# chengdu - license and linkage audit - v0.2.0

> Written by CC for arc01 slice04 (`provenance-notice-gate`), 2026-08-07.
> This revises `docs/license-audit-v0.1.0.md` for the in-tree fork baseline.
> It is an engineering compliance audit, not legal advice.

## 1. Scope and method

This audit covers the release binaries chengdu builds from the in-tree
`pandaPI/` source at v0.2.0: `pandaPIparser`, `pandaPIgrounder`, and
`pandaPIengine` for `linux-x86_64` and `macos-arm64`.

The v0.1.0 audit established the binary linkage surface from real build logs
and binary inspection. Slice04 does not change build inputs or link commands;
it changes the public identity model. The current build source is the chengdu
Git commit. `vendor.env` records identity import metadata: each subtree's
`source_prefix`, original upstream URL, upstream SHA, and chengdu import
commit. `pins.env` remains historical 0.1.0/import-point evidence only.

Three classes of change are separated below:

- identity import: upstream source imported into chengdu history with no
  behavior claim beyond provenance.
- patch-dissolution: former build-time compatibility patches are now ordinary
  in-tree source history from slice03.
- arc02: future upstream issue/PR absorption, not part of this audit.

## 2. Shipped binary linkage

| Shipped binary | Linked third-party/source surfaces | License posture |
|---|---|---|
| `pandaPIparser` | parser own code only | BSD 3-Clause |
| `pandaPIgrounder` | grounder own code, `h2-fd-preprocessor`, `cpddl`, `boruvka` | BSD 3-Clause plus conservative GPL-3.0 treatment for h2 |
| `pandaPIengine` | engine own code only in the shipped build configuration | BSD 3-Clause |

The v0.1.0 evidence remains the linkage base: `h2-fd-preprocessor`,
`cpddl`, and `boruvka` are linked into `pandaPIgrounder`; `bliss`,
`lpsolve`, and `opts` are built or present in the cpddl third-party tree but
not linked into the shipped binaries.

## 3. Component license table

| Source surface | Source prefix | Import identity | License | Shipped obligation |
|---|---|---|---|---|
| `pandaPIparser` | `pandaPI/pandaPIparser` | upstream `88c0995c72c5ce2685e9546ec153a3545977ed81`, import `ac8e385d` | BSD 3-Clause from `LICENSE` | license text shipped |
| `pandaPIgrounder` | `pandaPI/pandaPIgrounder` | upstream `b04ff7bcca710ae80a1ce73b8c027f0224565afc`, import `2b0a6093` | BSD 3-Clause from `LICENSE` | license text shipped |
| `pandaPIengine` | `pandaPI/pandaPIengine` | upstream `810f04388667db5e3e4f114e960a4efbb43b1ac0`, import `f734b692` | BSD 3-Clause from `LICENSE` | license text shipped |
| `cpddl` | `pandaPI/pandaPIgrounder/cpddl` | upstream `8c142bab211200213746c75be71810526e834d4a`, import `25a278a5` | BSD 3-Clause from `BSD-LICENSE` | license text shipped |
| `h2-fd-preprocessor` | `pandaPI/pandaPIgrounder/h2-fd-preprocessor` | upstream `e335433bfc53171a1c215bd49ba39cc5fb1d485b`, import `b27e102c` | treated conservatively as GPL-3.0-derived from Fast Downward | GPL-3.0 text shipped; source availability through chengdu commit |
| `boruvka` | `pandaPI/pandaPIgrounder/cpddl/third-party/boruvka` | upstream `e320cc7b02714e4096a686965fab343735d82bec`, import `1f0ac094` | BSD 3-Clause from `BSD-LICENSE` | license text shipped |
| `opts` | `pandaPI/pandaPIgrounder/cpddl/third-party/opts` | upstream `819d90e8664f9deda56f0dd8e7b62ea3ab9b2f6d`, import `68072cb3` | BSD 3-Clause from `BSD-LICENSE` | not linked; no release license text required |
| `bliss` | `pandaPI/pandaPIgrounder/cpddl/third-party/bliss` | derived from cpddl's former vendored bliss 0.73 archive | LGPL-3.0 from `COPYING.LESSER` | not linked; no release license text required |
| `lpsolve` | `pandaPI/pandaPIgrounder/cpddl/third-party/lpsolve` | derived from cpddl's third-party source tree | LGPL per lp_solve headers, including `lp_lib.h` | not linked; no release license text required |

## 4. Patch-dissolution boundary

Slice03 retired the build-time patch bridge. The compatibility deltas that
used to be represented as patch files are now normal source history under
`pandaPI/`. Release provenance therefore reports `patches=none`: no build
script applies source patches during the v0.2.0 build.

This does not mean chengdu source is byte-identical to upstream at the import
SHA. It means the current source is chengdu's tree at the release commit, with
the import identities preserved in `vendor.env` and `provenance.txt`.

## 5. NOTICE and source availability

Each vendored source surface has a `NOTICE` file naming origin, license,
import or derivation evidence, divergence policy, and Arc01 modifications:

- `pandaPI/pandaPIparser/NOTICE`
- `pandaPI/pandaPIgrounder/NOTICE`
- `pandaPI/pandaPIengine/NOTICE`
- `pandaPI/pandaPIgrounder/cpddl/NOTICE`
- `pandaPI/pandaPIgrounder/h2-fd-preprocessor/NOTICE`
- `pandaPI/pandaPIgrounder/cpddl/third-party/boruvka/NOTICE`
- `pandaPI/pandaPIgrounder/cpddl/third-party/opts/NOTICE`
- `pandaPI/pandaPIgrounder/cpddl/third-party/bliss/NOTICE`
- `pandaPI/pandaPIgrounder/cpddl/third-party/lpsolve/NOTICE`

For the GPL-3.0-treated `h2-fd-preprocessor` code linked into
`pandaPIgrounder`, source availability is satisfied by the public chengdu
repository at the release commit. The release manifest and rendered notes
record the chengdu commit plus per-component `source_prefix`,
`upstream_sha`, and `import_commit`.

## 6. Release asset obligations

The v0.2.0 release asset shape remains unchanged: per-platform tarballs,
`SHA256SUMS`, `release-manifest.txt`, and `THIRD-PARTY-LICENSES`. The
`THIRD-PARTY-LICENSES` asset contains the license texts for the components
whose object code is compiled into shipped binaries:

- `pandaPIparser` - BSD 3-Clause
- `pandaPIgrounder` - BSD 3-Clause
- `pandaPIengine` - BSD 3-Clause
- `cpddl` - BSD 3-Clause
- `boruvka` - BSD 3-Clause
- `h2-fd-preprocessor` - GPL-3.0 text

`bliss`, `lpsolve`, and `opts` are explicitly audited but not included in
`THIRD-PARTY-LICENSES` because they are not linked into shipped binaries.

## 7. Arc02 deferral

This audit does not absorb upstream PRs, file upstream issues, change
dependency selection, or redesign build behavior. Those belong to arc02. If
arc02 changes any linked dependency, build flag, source import, or license
surface, this audit and `licenses/` must be revised before publication.
