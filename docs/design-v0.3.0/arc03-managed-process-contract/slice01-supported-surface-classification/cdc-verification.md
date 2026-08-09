# CDC Verification: supported-surface-classification

Date: 2026-08-09

Branch: `release/0.3.x`

Verified commit: `7a0541d7141eb217e62a87adeb56566ab3beb9dd`
(`Classify Arc03 managed process surfaces`)

## Verdict

Accepted. Slice01 is CDC-verified.

The supported-surface classification report is complete for the slice scope,
the eight ledger rows reproduce independently, the closing report does not
drop any row, and the slice stayed inside the design-only boundary. The
accepted 0.3.0 managed-process surface is narrow:

- `supported`: parser normal HDDL parse, grounder normal `.htn` grounding,
  engine normal search;
- `legacy`: parser verifier/output helper paths, grounder `cpddl`/FAM
  integration, engine interactive mode;
- `experimental`: grounder H2, engine translation;
- `unsupported`: engine SAT;
- `future`: engine BDD/CUDD.

## Scope and Commit Check

`git show --stat --oneline --decorate 7a0541d7` showed only the expected slice
documentation changes:

- `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/ledger.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/closing-report.md`

`git show -s --format=full 7a0541d7` confirmed the required co-author
trailers:

- `Co-authored-by: Codex <noreply@openai.com>`
- `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Protected-path check:

```bash
git diff --name-only 7a0541d7^..7a0541d7 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Format check:

```bash
git diff --check 7a0541d7^..7a0541d7
```

Result: no output.

## Ledger Verification

The closing ledger contains 8 `F-*` rows:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/ledger.md
```

Result: `8`.

No open rows remain:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice01-supported-surface-classification/ledger.md
```

Result: no output, command exit 1 from no matches.

| Row | CDC result | Evidence reproduced |
|-----|------------|---------------------|
| F-1 | accepted | `test -f ... && rg -n "Arc01|Arc02|audit-synthesis-pandapi|combined-library-recommendations" ...` returned the report and Arc01/Arc02 evidence references. |
| F-2 | accepted | `rg -n "supported|legacy|experimental|unsupported|future" ...` returned the vocabulary section plus all five matrix dispositions. |
| F-3 | accepted | `rg -n "parser normal|HDDL parse|parser verifier|output helper|P-00[1-9]|P-01[0-5]|Re-entry|Behavior expectation|Rationale" ...` returned parser normal and parser verifier/output helper sections with required evidence and re-entry text. |
| F-4 | accepted | `rg -n "grounder normal|\\.htn|H2|cpddl|FAM|G-00[1-9]|G-01[0-9]|Re-entry|Behavior expectation|Rationale" ...` returned grounder normal, H2, and `cpddl`/FAM sections with required evidence and re-entry text. |
| F-5 | accepted | `rg -n "engine normal|normal search|interactive|translation|SAT|BDD|CUDD|E-00[1-9]|E-01[0-6]|Re-entry|Behavior expectation|Rationale" ...` returned engine normal, interactive, translation, SAT, and BDD/CUDD sections with required evidence and re-entry text. |
| F-6 | accepted | `rg -n "dependency availability|reproc|nlohmann/json|CUDD|H2|cpddl|parser generator|CLI11|fmt|does not decide|cannot make" ...` returned the dependency non-input section naming all required dependency families. |
| F-7 | accepted | `rg -n "slice02|status|exit|signal|slice03|stdio|event|TTY|slice04|CLI|naming|version|migration|slice05|test matrix|contract tests" ...` returned the per-surface later-slice inputs and the dedicated later Arc03 slice input sections. |
| F-8 | accepted | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output before CDC edits. |

## Bubble-Up Check

Slice01 delivered the assigned Arc03 capability: it classified every inherited
parser, grounder, and engine surface before status, stream, CLI, naming, and
test-contract decisions are designed.

No silent drops were found. The opening ledger had 8 rows, the closing ledger
has 8 rows, every row has a final `done` status, and CDC reproduced each row's
Verify command.

The closing report says no Arc03 body change is required before slice02. CDC
agrees: the existing Arc03 breakdown already has slice02 through slice05
consuming these classifications. The status surfaces should still be updated
to mark slice01 CDC-verified and to make slice02 the next unopened slice.

## What Worked

- The classification matrix kept the product contract boundary readable at a
  glance.
- The report separated optional dependency availability from support
  decisions, which prevents inherited optional paths from expanding the 0.3.0
  managed-process contract by accident.
- The later-slice routing is concrete enough for slice02 to start from status,
  exit, signal, child-process, and unsupported-feature obligations rather than
  rediscovering the supported surface.
