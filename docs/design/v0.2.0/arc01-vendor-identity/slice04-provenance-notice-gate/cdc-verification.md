# CDC verification - chengdu arc01 / slice04 - provenance-notice-gate

Verified by CDC on 2026-08-08, against `257901fc` on `release/0.2.x`.
Implementation commits: `acaf183f2d9ac90ad497e9da27575c3d713231ba`
and `258311a16e40529d9411fd2fc39f95eccc3cd090`.
Role: independent ledger reproduction and bubble-up check, per
`LEDGER-DISCIPLINE.md` Section A and `PROJECT-MANAGEMENT.md` Part IV.

## Verdict

**Accepted.** Rows F-1 through F-14 reproduce on `macos-arm64` or are
reconciled against the current GitHub Actions run. Slice04 delivered its
assigned arc01 piece: provenance is anchored on chengdu source plus vendored
import identities, active release docs describe source availability through
chengdu, NOTICE/license delineation is present, `license-audit-v0.2.0.md` is
landed, local behavior/package gates pass, and the current-head two-platform
CI gate is green without planner-source fetches.

CDC note on F-6: the raw broad grep over `docs/design-v0.2.0` now finds the
Slice04 ledger/closing report discussing the retired helper. Active paths
(`README.md`, `.github/`, `scripts/`, and `.github/release-notes.md.in`) have
no such hits. I treat this as an over-broad evidence wording in the close docs,
not a failure of the active no-fetch criterion.

## Row Reproduction

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | Current branch is `release/0.2.x`; `git status -sb` was clean against `origin/release/0.2.x`; slice03 CDC verification file exists. |
| F-2 | reproduced | `vendor.env` exists and defines prefix, URL, upstream SHA, and import commit for parser, grounder, engine, cpddl, h2-fd-preprocessor, boruvka, and opts. `pins.env` now says historical only, and active build/check scripts no longer source it. |
| F-3 | reproduced | From clean generated output, local `dist/macos-arm64/provenance.txt` records `chengdu_commit`, `source_prefix`, `upstream_url`, `upstream_sha`, `import_commit`, `patches=none`, and `compiler` for all three components; `rg '^sha=' dist/macos-arm64/provenance.txt` returned no matches. |
| F-4 | reproduced | `./scripts/check-provenance.sh` passed against `vendor.env` and Git state. A tampered copy at `/tmp/chengdu-provenance-tamper-cdc.txt` failed with `pandaPIparser: import_commit mismatch`. |
| F-5 | reproduced | `rg 'pins\.env|PARSER_SHA|GROUNDER_SHA|ENGINE_SHA' scripts/build-parser.sh scripts/build-grounder.sh scripts/build-engine.sh scripts/check-provenance.sh` returned no matches. `append_provenance` writes the new fields from `vendor.env`. |
| F-6 | reproduced with caveat | `scripts/fetch-upstream.sh` is absent. The no-fetch grep over active paths (`README.md`, `.github`, `scripts`, `.github/release-notes.md.in`) returned no matches. The broader design-doc grep finds only the Slice04 ledger/closing report discussing the retired helper as evidence. |
| F-7 | reproduced | All nine expected NOTICE files exist under `pandaPI/`. `find pandaPI -name NOTICE -print0 | xargs -0 rg 'Origin:|License:|Import|Derived|Divergence|Arc01'` found the required fields across the NOTICE set. |
| F-8 | reproduced | `docs/license-audit-v0.2.0.md` exists and the required-term grep covers identity import, patch-dissolution, arc02, `bliss`, `boruvka`, `lpsolve`, `opts`, `h2-fd-preprocessor`, GPL-3.0, BSD 3-Clause, and LGPL. |
| F-9 | reproduced | `./scripts/package-release.sh v0.2.0-slice04-smoke` passed. `release/THIRD-PARTY-LICENSES`, `licenses/README.md`, and `scripts/package-release.sh` reference `docs/license-audit-v0.2.0.md`; stale `license-audit-v0.1.0` references are absent from active release docs/scripts. |
| F-10 | reproduced | README and release-note template positive grep found `vendor.env`, `license-audit-v0.2.0`, `chengdu commit`, `source_prefix`, `upstream_sha`, and `import_commit`. The stale wording grep for pinned-upstream/patch/v0.1 audit phrasing returned no matches. |
| F-11 | reproduced | From clean `build/ dist/ release/`: parser, grounder, engine, `check-provenance`, positive smoke, negative smoke, and package dry-run all passed. Positive smoke reported `5 passed, 0 failed`; negative smoke reported `4 passed, 0 failed`. |
| F-12 | reconciled | `gh run view 31239214748 --json conclusion,headSha,event,url` reports success at head `257901fcb72b97424df88fd94ad434b19cfb5abb`. Jobs for ubuntu-22.04, ubuntu-24.04, macos-15, README-verbatim, and cross-compat passed. `gh run view 31239214748 --log | rg 'panda-planner-dev|gitlab.com/danfis|galvusdamor/h2-fd-preprocessor|fetch-upstream'` returned no matches. |
| F-13 | reproduced | Package dry-run generated `release-manifest.txt`, `notes.md`, `SHA256SUMS`, `THIRD-PARTY-LICENSES`, and `pandapi-v0.2.0-slice04-smoke-macos-arm64.tar.gz`. `shasum -a 256 -c SHA256SUMS --ignore-missing` passed. Tarball contents remain `fixtures/`, `pandaPIparser`, `pandaPIgrounder`, `pandaPIengine`, and `provenance.txt`. |
| F-14 | reproduced | `bash -n scripts/*.sh`, `shellcheck scripts/*.sh`, `actionlint .github/workflows/*.yml`, and `git diff --check` all passed. `git status --short -- pandaPI` and the generated-output `find` under `pandaPI/` both returned empty. |

## Additional Checks

- CC's Slice04 commits include the required co-author trailers for Codex and
  Billo AI.
- `.github/workflows/build-reusable.yml` and `.github/workflows/release.yml`
  use `fetch-depth: 0` for chengdu checkout, matching the provenance checker's
  need for import-commit ancestry while still avoiding planner-source fetches.
- The current package dry-run renders release notes with the new provenance
  table and source availability through the chengdu release commit.
- Upstream compiler warnings remain present during local builds, but all build,
  smoke, package, lint, and static checks exit 0.

## Bubble-up Check

**Delivered as assigned:** yes. Slice04 closes the provenance/NOTICE/license
surface that arc01 assigned it, and provides current-head CI evidence for the
two-platform no-fetch gate.

**Arc-plan change required before arc close:** no. The only implementation
finding was the need for full chengdu history in CI checkout so import-ancestry
checks can run. That is now implemented in workflow checkout configuration and
does not change arc scope or sequencing.

**Silent-drop diff:** complete. No Slice04 ledger row is deferred or no-op.
The remaining work, real `v0.2.0` publication and upstream PR/issue absorption,
is still correctly assigned to arc02. Arc01 is ready for its formal arc-level
closing report and composition check.
