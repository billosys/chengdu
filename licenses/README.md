# licenses/

Exact upstream license texts for every third-party component whose
object code is compiled into a shipped chengdu binary, per
[`docs/license-audit-v0.2.0.md`](../docs/license-audit-v0.2.0.md).
`scripts/package-release.sh` assembles these into each release's
`THIRD-PARTY-LICENSES` asset; it fails loudly if any file listed below
is missing.

| File | Component | License | Source path @ import identity |
|---|---|---|---|
| `pandaPIparser-BSD-3-Clause.txt` | pandaPIparser | BSD 3-Clause | `LICENSE` @ `88c0995` |
| `pandaPIgrounder-BSD-3-Clause.txt` | pandaPIgrounder | BSD 3-Clause | `LICENSE` @ `b04ff7b` |
| `pandaPIengine-BSD-3-Clause.txt` | pandaPIengine | BSD 3-Clause | `LICENSE` @ `810f043` |
| `cpddl-BSD-3-Clause.txt` | cpddl | BSD 3-Clause | `BSD-LICENSE` @ `8c142ba` |
| `boruvka-BSD-3-Clause.txt` | boruvka | BSD 3-Clause | `BSD-LICENSE` @ `e320cc7` |
| `h2-fd-preprocessor-GPL-3.0.txt` | h2-fd-preprocessor | GPL-3.0 | canonical FSF text (h2-fd-preprocessor's own vendored copy carries no LICENSE file; text sourced from `cpddl/third-party/bliss/COPYING`, itself the unmodified FSF GPL-3.0 text — see the audit's §2.2.1) |

`bliss` (LGPL-3.0) and `lpsolve` (LGPL) are built but confirmed **not**
linked into any shipped binary (audit §2.2.2) and are intentionally
absent here — no distribution obligation, no license text shipped.

Update this directory (and re-run the audit) whenever `vendor.env`
changes, a new third-party dependency is introduced, or an existing
one's linkage status changes.
