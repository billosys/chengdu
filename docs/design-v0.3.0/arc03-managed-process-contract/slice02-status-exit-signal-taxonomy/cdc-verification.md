# CDC Verification: status-exit-signal-taxonomy

Date: 2026-08-09

Branch: `release/0.3.x`

Verified commit: `86df4d3ef01f5236f14a6947c92a8929d5255a97`
(`Define Arc03 process status taxonomy`)

## Verdict

Accepted. Slice02 is CDC-verified.

The taxonomy report is complete for the slice scope, the nine ledger rows
reproduce independently, the closing report does not drop any row, and the
slice stayed inside the design-only boundary. The accepted status vocabulary
is:

- `ok`
- `domain_no_plan`
- `cli_usage_error`
- `input_unavailable`
- `output_unavailable`
- `input_invalid`
- `unsupported_feature`
- `legacy_surface`
- `experimental_surface`
- `future_surface`
- `timeout`
- `resource_limit`
- `interrupted`
- `dependency_failure`
- `child_process_failure`
- `internal_error`
- `signal_terminated`

Key CDC observations:

- `domain_no_plan` is separate from timeout, resource failure, malformed input,
  and internal defects.
- Exit code `1` is reserved as a compatibility fallback, not used by the new
  classified 0.3.0 outcomes.
- `interrupted` is binary-owned and mapped through the status mapper; unhandled
  signal death remains supervisor-visible as `128 + signal_number`.
- Arc02 gates are preserved: `tl::expected` waits for the taxonomy, reproc++
  remains conditional, nlohmann/json waits for slice03, CLI11 waits for
  slice04, fmt waits for slice03/Arc04, and Catch2/process fixtures wait for
  slice05/Arc04.

## Scope and Commit Check

`git show --stat --oneline --decorate 86df4d3e` showed only the expected
slice documentation changes:

- `docs/design-v0.3.0/arc03-managed-process-contract/status-exit-signal-taxonomy.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/ledger.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/closing-report.md`

`git show -s --format=full 86df4d3e` confirmed the required co-author
trailers:

- `Co-authored-by: Codex <noreply@openai.com>`
- `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Protected-path check:

```bash
git diff --name-only 86df4d3e^..86df4d3e -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Format check:

```bash
git diff --check 86df4d3e^..86df4d3e
```

Result: no output.

## Ledger Verification

The closing ledger contains 9 `F-*` rows:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/ledger.md
```

Result: `9`.

No open rows remain:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice02-status-exit-signal-taxonomy/ledger.md
```

Result: no output, command exit 1 from no matches.

| Row | CDC result | Evidence reproduced |
|-----|------------|---------------------|
| F-1 | accepted | `test -f ... && rg -n "Arc01|Arc02|slice01|audit-synthesis-pandapi|combined-library-recommendations|supported-surface-classification" ...` returned the report and required evidence references. |
| F-2 | accepted | `rg -n "success|usage|input|open|read|output|write|parse|validation|unsupported|legacy|experimental|future|unsolvable|unreachable|timeout|resource|signal|interrupted|dependency|child-process|internal" ...` returned the status vocabulary and per-surface coverage. |
| F-3 | accepted | `rg -n "exit code|numeric|table|rationale|diagnostic prose|status family|maps? to" ...` returned the numeric exit-code table, rationale, and diagnostic-prose non-reliance rule. |
| F-4 | accepted | `rg -n "parser normal|HDDL parse|grounder normal|\\.htn|engine normal|normal search|solved|success|malformed|open|read|write|validation|unsolvable|unreachable" ...` returned the three supported normal-path sections. |
| F-5 | accepted | `rg -n "parser verifier|output helper|H2|cpddl|FAM|interactive|translation|SAT|BDD|CUDD|legacy|experimental|unsupported|future" ...` returned deterministic treatment for all visible non-supported surfaces from slice01. |
| F-6 | accepted | `rg -n "timeout|resource|signal|SIGINT|SIGTERM|interrupt|interruption|supervised|internal defect|unsolvable|unreachable" ...` returned timeout/resource/signal/interruption semantics and distinctions from no-plan and internal-defect outcomes. |
| F-7 | accepted | `rg -n "tl::expected|StatusResult|reproc|nlohmann/json|JSON|CLI11|fmt|Catch2|process fixture|Abseil|Boost\\.Process|held|rejected|conditional|slice03|slice04|slice05|Arc04" ...` returned the Arc02 dependency-gating section. |
| F-8 | accepted | `rg -n "slice03|stdio|event|payload|slice04|CLI|parse error|slice05|contract test|Arc04|status/result|shared substrate|Arc05|per-binary|Arc06|release docs|behavior-change" ...` returned later slice and arc consumer sections. |
| F-9 | accepted | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output before CDC edits. |

## Bubble-Up Check

Slice02 delivered the assigned Arc03 capability: it defined the common status
vocabulary, numeric exit-code mapping, timeout/resource/signal/interruption
semantics, dependency and child-process failure semantics, and Arc02
dependency gates.

No silent drops were found. The opening ledger had 9 rows, the closing ledger
has 9 rows, every row has final `done` status, and CDC reproduced each row's
Verify command.

The closing report says no Arc03 body change is required before slice03. CDC
agrees: the existing Arc03 breakdown already has slice03 consuming status
payloads for stdio/event/TTY policy and Arc04 consuming the final taxonomy for
the shared status/result substrate. The status surfaces should still be
updated to mark slice02 CDC-verified and make slice03 the next unopened slice.

## What Worked

- The taxonomy gives supervisors a status/exit basis that does not depend on
  parsing human diagnostics.
- The report treats no-plan domain outcomes as expected classified outcomes,
  not generic process failure.
- Arc02's dependency gates survived the slice: implementation libraries remain
  carriers behind future semantic decisions, not drivers of the contract.
