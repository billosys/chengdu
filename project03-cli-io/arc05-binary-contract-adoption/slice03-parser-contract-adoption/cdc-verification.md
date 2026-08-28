# CDC Verification - Arc05 Slice03: parser-contract-adoption

Verifier: CDC
Date: 2026-08-10
Branch: `release/0.3.x`

Reviewed commits:

- CC implementation: `5c807016b3fe9c6c2e99c3d72b4c77f1f38a4905`
- Make-target follow-up: `1d91a1aec2cee955ba1c82f2a28687903369cb99`
- CDC evidence repair: `fe4efbfaca597a3434b3c7d5f71c161338b19555`

## Verdict

GO. Arc05 Slice03 is independently CDC-verified.

The final tree, including the make-target follow-up, delivers managed parser
contract adoption for the supported normal HDDL parser surface. Canonical
`pandapi-parser` and inherited `pandaPIparser` entry points are executable,
the raw inherited parser remains available as `pandaPIparser.legacy`, and
grounder, engine, and pipeline behavior remain baseline-only.

The make-target follow-up is accepted as part of the reviewed final state. It
routes the reusable Linux/macOS CI legs through `make ci-linux` and
`make ci-macos`; the `test` aggregate reaches `test-contract-parser-managed`,
which installs the parser adapter and runs the parser contract fixtures. The
same follow-up also routes existing release workflow package/publish script
calls through release make targets. CDC treats that as CI entrypoint routing,
not as release-shape proof or Arc08 release acceptance.

## Reproduced Evidence

Commands reproduced locally on macos-arm64:

- `./scripts/build-parser.sh`
- `./scripts/run-contract-fixtures.sh --contract --component parser`
  - observed: `300 passed, 0 failed`
- Focused parser contract cases, including:
  - `parser-input-unavailable`
  - `parser-input-invalid`
  - `parser-output-unavailable`
  - `parser-final-status-fields`
  - `parser-status-stream-legality`
  - `parser-color-policy`
  - `parser-legacy-surface-fencing`
- `./scripts/run-contract-fixtures.sh --baseline`
  - observed: `38 passed, 0 failed`
- `make test`
  - observed: runtime 5/5, parser contract 300/0, positive smoke 5/0,
    negative smoke 4/0, final `Test suite passed`
- `make test-runtime-sanitize`
  - observed: sanitizer runtime CTest 5/5
- `make format-check`
  - observed: format check passed for 25 files
- `make provenance-check`
  - observed: all 3 components verified against `vendor.env` and Git state
- `make actionlint`
  - observed: passed with no diagnostics
- `git diff --check`
- `git diff --cached --check`

Additional boundary checks reproduced:

- `dist/<platform>/pandapi-parser`, `dist/<platform>/pandaPIparser`, and
  `dist/<platform>/pandaPIparser.legacy` are executable after parser build.
- Slice03 touched no protected grounder or engine source paths, no tracked
  release assets/checksums/manifests, and no `scripts/package-release.sh` or
  `scripts/publish-release.sh` changes.
- The CC implementation commit added no `FetchContent`, `ExternalProject`,
  `curl`, `git clone`, rejected dependency import, or third-party API leakage
  into the parser/runtime/fixture surface.
- Workflow parser-contract execution is make-targeted in the final tree:
  `.github/workflows/build-reusable.yml` runs `make ci-linux` and
  `make ci-macos`; `mk/tests.mk` wires `test` to
  `test-contract-parser-managed`.

## Ledger Review

| ID | CDC result | Evidence |
|----|------------|----------|
| F-1 | reproduced | Slice scope still names parser-only adoption, canonical/inherited commands, status/artifact/stream behavior, CI, and grounder handoff. |
| F-2 | reproduced | `./scripts/build-parser.sh` installed executable `pandapi-parser`, `pandaPIparser`, and `pandaPIparser.legacy`. |
| F-3 | reproduced | Parser contract records cover canonical/inherited commands, file/stdout artifacts, positive/negative outcomes, status fields, normalization, and safety. |
| F-4 | reproduced | Runner supports baseline, contract parser selection, `--case`, and list modes. |
| F-5 | reproduced | Canonical file-backed success covered by parser contract suite. |
| F-6 | reproduced | Canonical stdout-backed success covered by parser contract suite with no stdout status/prose contamination. |
| F-7 | reproduced | Inherited managed `pandaPIparser` compatibility and legacy passthrough covered by parser contract suite. |
| F-8 | reproduced | Help, version, and provenance informational behavior covered by parser contract suite. |
| F-9 | reproduced | Parser CLI usage errors classify as `cli_usage_error` / 10. |
| F-10 | reproduced | Missing domain, problem, and include inputs classify as `input_unavailable` / 20; focused case passed. |
| F-11 | reproduced | Malformed and semantically invalid HDDL classify as `input_invalid` / 22. |
| F-12 | reproduced | Unavailable output paths classify as `output_unavailable` / 21. |
| F-13 | reproduced | Final `PANDAPI_STATUS` field set and single-emission behavior covered by parser contract suite. |
| F-14 | reproduced | Status stream legality covered by parser contract suite. |
| F-15 | reproduced | ANSI/color policy covered by parser contract suite. |
| F-16 | reproduced | Parser verifier/output helper surfaces are fenced as `legacy_surface` / 31. |
| F-17 | reproduced | Final CI wiring runs through make targets; CDC repaired stale direct-script evidence before verification. |
| F-18 | reproduced | Baseline fixtures passed: 38 passed, 0 failed; grounder, engine, and pipeline remain baseline-only. |
| F-19 | reproduced | `make test` and `make test-runtime-sanitize` passed after parser adoption. |
| F-20 | reproduced | `make actionlint` passed with no diagnostics. |
| F-21 | reproduced | Dependency gate passed for the implementation commit; the make follow-up only moved existing workflow helper behavior behind make targets. |
| F-22 | reproduced | Protected grounder/engine/release-shape boundary checks passed. |
| F-23 | reproduced | Staged and unstaged whitespace checks passed. |
| F-24 | reproduced | Closing report exists with capability verdict, implementation decisions, verification, ledger walk, silent-drop check, and Arc05 bubble-up. |

## Review Notes

- No blocking issue found.
- CDC explicitly reviewed the final post-CC state because workflow entrypoints
  were routed through make targets after CC's implementation commit.
- The accepted behavior change is parser-only. Slice04 should extend the
  existing baseline/contract fixture split for grounder without changing the
  parser contract or release packaging.
