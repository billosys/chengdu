# Arc05 Slice04 CDC Verification

Date: 2026-08-11

## Verdict

Closed by supersession.

Slice04 landed as an adapter-era grounder contract slice before the operator's
no-compatibility policy correction. Its CC closing report is historically
accurate for that moment, but several ledger rows required inherited
`pandaPIgrounder` / `pandaPIgrounder.legacy` compatibility that is no longer
accepted 0.3.0 behavior.

CDC therefore does not claim literal reproduction of the old adapter-era
compatibility rows. Instead, Slice04 is accepted as superseded by the later
native grounder cutover work:

- project-plan v1.40 and arc-plan v1.8 changed 0.3.0 policy to no inherited
  command compatibility;
- Slice05 recorded the shim/sidecar stop condition;
- Slice07 replaced adapter-era grounder behavior with native
  `pandapi-grounder` and removed old-name grounder artifacts;
- current grounder contract evidence passes through the canonical native
  binary only.

## Current Grounder Evidence

CDC reproduced the current grounder closure state on `release/0.3.x`, macOS
arm64:

- `make test-contract-grounder-managed` passed: 256 passed, 0 failed.
- `make test-contract-parser-managed` passed: 285 passed, 0 failed.
- `make test-contract-engine-managed` passed: 299 passed, 0 failed.
- `make smoke` passed: 3 passed, 0 failed.
- `make smoke-negative` passed: 4 passed, 0 failed.
- `make test` passed.
- `make provenance-check` passed.
- `make actionlint` passed.

The current `dist/macos-arm64/` primary surface contains native canonical
`pandapi-parser`, `pandapi-grounder`, and `pandapi-engine` artifacts. The
adapter-era old-name grounder artifacts are intentionally absent.

## Superseded Rows

Rows that asserted inherited grounder compatibility are closed as superseded,
not reproduced:

- F-2, F-7, F-8, and any supporting evidence text that required
  `pandaPIgrounder` or `pandaPIgrounder.legacy` as an accepted 0.3.0 command.

Rows that describe the enduring managed contract are represented by current
canonical evidence:

- normal `.htn` grounding, file/stdout artifact ownership, CLI usage errors,
  unavailable/invalid input, unavailable output, final status fields, status
  stream legality, color policy, H2 experimental fencing, `cpddl`/FAM legacy
  fencing, parser regression, make-targeted CI routing, provenance, actionlint,
  aggregate test, and whitespace hygiene.

## Bubble-up

Arc05 may close with Slice04 counted as CDC-verified by supersession. The
current accepted grounder behavior is Slice07's native `pandapi-grounder`
contract, not Slice04's temporary adapter/inherited-name compatibility state.
Downstream docs and release work must describe only the canonical 0.3.0
surface.
