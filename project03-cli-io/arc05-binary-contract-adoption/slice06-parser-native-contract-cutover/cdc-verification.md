# CDC Verification - Arc05 Slice06: parser-native-contract-cutover

Verifier: CDC
Date: 2026-08-10
Branch: `release/0.3.x`

Reviewed commit:

- CC implementation: `b70b08256b53595d308f3747c557e87810d0ca7b`

## Verdict

GO. Arc05 Slice06 is independently CDC-verified.

The reviewed tree delivers the parser half of the Slice05 remediation:
`pandapi-parser` is now the canonical native/non-shell parser artifact,
parser old-name artifacts are no longer installed, and the parser managed
contract passes without `scripts/pandapi-parser-adapter.sh` or a
`pandaPIparser.legacy` sidecar.

This verification does not claim grounder native cutover, engine adoption, or
release-shape acceptance. Grounder remains transitional and is routed to the
next slice.

## Reproduced Evidence

Commands reproduced locally on macos-arm64:

- `git status --short --branch`
  - observed: `## release/0.3.x`
- `make build-parser`
  - observed: parser artifacts built in `dist/macos-arm64`
- `make test-contract-parser-managed`
  - observed: `285 passed, 0 failed`
- Focused parser informational aliases:
  - `./tests/contract/run --contract --component parser --case parser_help`
  - `./tests/contract/run --contract --component parser --case parser_version`
  - `./tests/contract/run --contract --component parser --case parser_provenance`
  - observed: each alias reproduced `parser-info-commands` with
    `15 passed, 0 failed`
- Focused parser legacy-surface alias:
  - `./tests/contract/run --contract --component parser --case parser_legacy_surface`
  - observed: reproduced `parser-legacy-surface-fencing` with
    `18 passed, 0 failed`
- `make smoke`
  - observed: `3 passed, 0 failed`
- `make smoke-negative`
  - observed: `4 passed, 0 failed`
- `make test`
  - observed: parser contract, grounder contract, positive smoke, negative
    smoke, and final `Test suite passed`
- `git diff --check`
- `git diff --cached --check`

Additional boundary checks reproduced:

- `dist/<platform>/pandapi-parser` is executable.
- `dist/<platform>/pandaPIparser` is absent.
- `dist/<platform>/pandaPIparser.legacy` is absent.
- `dist/<platform>/pandapi-parser` is not classified by `file` as a shell or
  text script, and its first bytes do not contain a shebang.
- `scripts/pandapi-parser-adapter.sh` is absent.
- Grounder transition shims remain present:
  `scripts/pandapi-grounder-adapter.sh` and
  `scripts/install-grounder-adapter.sh`.
- Parser provenance contains `canonical_command=pandapi-parser`,
  `component=parser`, and `managed_process_contract=0.3.0`, with no parser
  `legacy_binary` or inherited invoked-command sidecar field.
- Active parser build/test/tooling surfaces have no matches for
  `pandapi-parser-adapter`, `pandaPIparser.legacy`, copied
  `pandaPIparser`, or `legacy_binary=pandaPIparser`.
- Protected-path checks found no grounder source, engine source,
  release-publication, checksum, final manifest, Arc05 close, or CDC-file
  drift in the CC implementation.
- Dependency/network-fetch checks found no `FetchContent`, `ExternalProject`,
  `curl`, `git clone`, rejected dependency import, or third-party API leakage
  in the reviewed diff.

## Ledger Review

| ID | CDC result | Evidence |
|----|------------|----------|
| F-1 | reproduced | Slice open set records parser-only native cutover, Slice05 context, grounder/engine deferral, no Bash adapter, no parser old-name artifacts, make verification, and release boundary. |
| F-2 | reproduced | `make build-parser` leaves executable `dist/<platform>/pandapi-parser` and no `pandaPIparser` or `pandaPIparser.legacy`. |
| F-3 | reproduced | `pandapi-parser` is not a shell/text adapter and has no shebang. |
| F-4 | reproduced | Parser managed options pass through the full managed contract plus help/version/provenance aliases. |
| F-5 | reproduced | Parser legacy/helper surfaces remain fenced by managed status. |
| F-6 | reproduced | Active parser build/test/tooling surfaces no longer invoke or install parser adapter or old-name artifacts. |
| F-7 | reproduced | Parser adapter script is deleted; grounder transition scripts remain. |
| F-8 | reproduced | Parser fixture records no longer assert inherited parser command compatibility, parser sidecars, or adapter provenance. |
| F-9 | reproduced | Parser provenance names canonical `pandapi-parser` and does not advertise a parser `legacy_binary`. |
| F-10 | reproduced | Parser contract, positive smoke, and negative smoke pass through make. |
| F-11 | reproduced | `make test` passes with parser native and grounder still transitional. |
| F-12 | reproduced | Protected grounder, engine, release-publication, release asset, Arc05 close, and CDC paths are untouched by the implementation. |
| F-13 | reproduced | Dependency/network-fetch gate passes for touched parser/runtime/fixture/tooling surfaces. |
| F-14 | reproduced | Staged and unstaged whitespace checks pass. |
| F-15 | reproduced | Closing report exists with capability verdict, implementation decisions, verification, ledger walk, silent-drop check, and Arc05 bubble-up. |

## Review Notes

- No blocking issue found for Slice06.
- The accepted behavior change is parser-native only.
- Grounder still installs transitional artifacts during aggregate builds; that
  is expected and is the explicit target of Slice07.
