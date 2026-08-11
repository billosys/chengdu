# CDC Verification - Arc05 Slice07: grounder-native-contract-cutover

Verifier: CDC
Date: 2026-08-10
Branch: `release/0.3.x`

Reviewed commits:

- CDC/open-set commit: `e2e6914b`
- CC implementation: `43151b5f`

## Verdict

GO. Arc05 Slice07 is independently CDC-verified.

The reviewed tree delivers the grounder half of the Slice05 remediation:
`pandapi-grounder` is now the canonical native/non-shell grounder artifact,
grounder old-name artifacts are no longer installed, and the grounder managed
contract passes without `scripts/pandapi-grounder-adapter.sh`,
`scripts/install-grounder-adapter.sh`, or a `pandaPIgrounder.legacy` sidecar.

This verification does not claim engine adoption or release-shape acceptance.
Engine remains inherited as `pandaPIengine` and is routed to Slice08.

## Reproduced Evidence

Commands reproduced locally on macos-arm64:

- `git status --short --branch`
  - observed: `## release/0.3.x`
- `make build-grounder`
  - observed: `Built .../dist/macos-arm64/pandapi-grounder`
- Artifact checks after `make build-grounder`
  - `dist/macos-arm64/pandapi-grounder` is executable
  - `file dist/macos-arm64/pandapi-grounder` reports
    `Mach-O 64-bit executable arm64`
  - no shebang is present
  - `dist/macos-arm64/pandaPIgrounder` is absent
  - `dist/macos-arm64/pandaPIgrounder.legacy` is absent
- `make test-contract-grounder-managed`
  - observed: `256 passed, 0 failed`
- `make test-contract-parser-managed`
  - observed: `285 passed, 0 failed`
- `make smoke`
  - observed: `3 passed, 0 failed`
- `make smoke-negative`
  - observed: `4 passed, 0 failed`
- `make test`
  - observed: grounder contract `256 passed, 0 failed`, positive smoke
    `3 passed, 0 failed`, negative smoke `4 passed, 0 failed`, and final
    `Test suite passed`
- `make actionlint`
  - observed: `actionlint passed`
- `git diff --check`
- `git diff --cached --check`

Additional boundary checks reproduced:

- Grounder adapter scripts are absent:
  `scripts/pandapi-grounder-adapter.sh` and
  `scripts/install-grounder-adapter.sh`.
- Parser adapter remains absent: `scripts/pandapi-parser-adapter.sh`.
- Active grounder build/test/tooling surfaces have no matches for
  `pandapi-grounder-adapter`, `install-grounder-adapter`,
  `pandaPIgrounder.legacy`, copied `pandaPIgrounder`, or
  `legacy_binary=pandaPIgrounder`.
- Grounder fixture records and fixture README no longer assert inherited
  grounder command compatibility, grounder `.legacy` sidecars, or grounder
  adapter provenance.
- Grounder provenance contains `canonical_command=pandapi-grounder`,
  `component=grounder`, `managed_process_contract=0.3.0`,
  `dependency_surface_h2=experimental_surface`, and
  `dependency_surface_cpddl_fam=legacy_surface`, with no grounder
  `legacy_binary` or inherited invoked-command sidecar field.
- Parser native behavior remains preserved: parser contract passes and parser
  old-name artifacts remain absent.
- Protected-path checks found no engine source, release-publication, checksum,
  final manifest, Arc05 close, or Slice07 CDC-file drift in the CC
  implementation.
- Dependency/network-fetch checks found no `FetchContent`, `ExternalProject`,
  `curl`, `git clone`, rejected dependency import, or third-party API leakage
  in the reviewed diff.
- `.github/workflows/build-reusable.yml` continues to route repository checks
  through make targets (`make install-actionlint`, `make actionlint`,
  `make ci-linux`, `make ci-macos`, and `make readme-verbatim`).

## Ledger Review

| ID | CDC result | Evidence |
|----|------------|----------|
| F-1 | reproduced | Slice open set records grounder-only native cutover, Slice05/Slice06 context, parser-native preservation, engine deferral, no Bash adapter, no grounder old-name artifacts, make verification, and release boundary. |
| F-2 | reproduced | `make build-grounder` leaves executable `dist/<platform>/pandapi-grounder` and no `pandaPIgrounder` or `pandaPIgrounder.legacy`. |
| F-3 | reproduced | `pandapi-grounder` is a Mach-O executable, not a shell/text adapter, and has no shebang. |
| F-4 | reproduced | Grounder managed options pass through the full managed contract, including info, status-stream, and final-status cases. |
| F-5 | reproduced | H2 is fenced as `experimental_surface` / 32 and `cpddl`/FAM is fenced as `legacy_surface` / 31. |
| F-6 | reproduced | Active grounder build/test/tooling surfaces no longer invoke or install grounder adapter scripts or old-name artifacts. |
| F-7 | reproduced | Grounder transition scripts are deleted and the parser transition script remains absent. |
| F-8 | reproduced | Grounder fixture records no longer assert inherited grounder command compatibility, sidecars, or adapter provenance. |
| F-9 | reproduced | Grounder provenance names canonical `pandapi-grounder`, records H2 and `cpddl`/FAM disposition, and does not advertise a grounder `legacy_binary`. |
| F-10 | reproduced | Parser managed contract still passes and parser old-name artifacts remain absent. |
| F-11 | reproduced | Grounder contract, positive smoke, and negative smoke pass through make. |
| F-12 | reproduced | `make test` passes with parser and grounder native and engine still inherited. |
| F-13 | reproduced | Protected engine, release-publication, release asset, Arc05 close, and CDC paths are untouched by the implementation. |
| F-14 | reproduced | Dependency/network-fetch gate passes for touched grounder/runtime/fixture/tooling surfaces. |
| F-15 | reproduced | Staged and unstaged whitespace checks pass. |
| F-16 | reproduced | Closing report exists with capability verdict, implementation decisions, verification, ledger walk, silent-drop check, and Arc05 bubble-up. |

## Review Notes

- No blocking issue found for Slice07.
- The accepted behavior change is grounder-native only.
- Engine remains the last inherited-name primary binary and is the explicit
  target of Slice08.
