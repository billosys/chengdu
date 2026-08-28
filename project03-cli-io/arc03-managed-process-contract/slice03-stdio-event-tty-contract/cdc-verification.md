# CDC Verification: stdio-event-tty-contract

Date: 2026-08-09

Branch: `release/0.3.x`

Verified commits:

- `e465f42736db9d443b196621d06436c6e644ee2f`
  (`Close Arc03 stdio event tty contract`)
- `7f5940426138409b0b12b3a677480dbc408d67c0`
  (`docs: add stdio event tty closing report`)

Prior CDC blocker:

- `34e0e6ce204bbe421c16f2fc2f43b04c0232657f`
  (`docs: record stdio event tty cdc blocker`)

## Verdict

Accepted. Slice03 is CDC-verified.

The stdio/event/TTY contract report is complete for the slice scope, all 12
ledger rows reproduce independently, the supplemental CC closing report fixes
the previously missing close-set artifact, and the slice stayed inside its
design-only boundary.

The accepted design decisions are:

- stdout is a single selected artifact sink, event/status sink, or empty;
- stderr is the human diagnostics, warnings, progress, statistics, and summary
  channel;
- supervisors classify outcomes from exit codes and tagged status records, not
  diagnostic prose;
- 0.3.0 uses tagged text status-only machine-readable output, not JSON Lines;
- `nlohmann/json` remains held because the accepted event/status format is not
  JSON;
- `fmt` remains behind a future diagnostics/process I/O facade;
- legacy, experimental, unsupported, and future surfaces are fenced before
  they can contaminate supported stream semantics.

## Scope and Commit Check

Primary slice commit:

```bash
git show --name-status --format=full e465f42736db9d443b196621d06436c6e644ee2f
```

Result: the commit included only:

- `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md`

Supplemental close-set commit:

```bash
git show --name-status --format=full 7f5940426138409b0b12b3a677480dbc408d67c0
```

Result: the commit included only:

- `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/closing-report.md`

`git show -s --format=full` for both commits confirmed the required co-author
trailers:

- `Co-authored-by: Codex <noreply@openai.com>`
- `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Protected-path check:

```bash
git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Format check for the current verification state:

```bash
git diff --check -- docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md
```

Result: no output.

## Close-Set Verification

The required CC closing report now exists:

```bash
test -f docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/closing-report.md
```

Result: command exit 0.

The closing report includes the expected close sections:

```bash
rg -n "Ledger Walk|Silent-Drop Diff|Bubble-Up To Arc03|CDC Handoff|12" docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/closing-report.md
```

Result: matched row count, ledger walk, silent-drop diff, Bubble-up to Arc03,
and CDC handoff sections.

The closing report's silent-drop diff is complete and honest: it defers final
CLI spelling/help/version and CLI11 migration to slice04, contract tests and
fixtures to slice05/Arc04, implementation substrate work to Arc04, per-binary
migration to Arc05, release documentation/license/NOTICE/behavior-change work
to Arc06, and keeps JSON Lines/`nlohmann/json` held after selecting tagged
text status-only output.

## Ledger Verification

The closing ledger contains 12 `F-*` rows:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md
```

Result: `12`.

No open rows remain:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md
```

Result: no output, command exit 1 from no matches.

| Row | CDC result | Evidence reproduced |
|-----|------------|---------------------|
| F-1 | accepted | `test -f ... && rg -n "Arc01|Arc02|slice01|slice02|audit-synthesis-pandapi|combined-library-recommendations|supported-surface-classification|status-exit-signal-taxonomy" ...` returned the report and required evidence references. |
| F-2 | accepted | `rg -n "human CLI|supervised|process-manager|machine-event|event mode|mode|CLI option|slice04|option spelling|flag spelling" ...` returned the three-mode table and slice04 ownership language. |
| F-3 | accepted | `rg -n "stdout|parser normal|HDDL parse|grounder normal|\\.htn|engine normal|normal search|success|domain_no_plan|diagnostic|progress|statistics|mixing|owned" ...` returned the stdout ownership table and no-mixing rules. |
| F-4 | accepted | `rg -n "stderr|diagnostic|summary|warning|progress|statistics|fatal|internal_error|diagnostic prose|non-normative|supervisor" ...` returned stderr ownership and non-normative prose language. |
| F-5 | accepted | `rg -n "machine-readable|event|status payload|payload fields|JSON Lines|tagged text|status-only|ordering|flush|nlohmann/json|JSON|held|re-enter" ...` returned the tagged text status-only decision, payload routing, ordering/flush rules, and JSON hold. |
| F-6 | accepted | `rg -n "buffer|buffering|flush|flushing|partial-output|partial_output_policy|pipe|TTY|file|timeout|resource_limit|interrupted|dependency_failure|child_process_failure|internal_error" ...` returned buffering/flushing and outcome disposition policy. |
| F-7 | accepted | `rg -n "ANSI|color|colour|TTY|NO_COLOR|no-color|no-colour|auto|supervised|machine|interactive|no-TTY|escape" ...` returned ANSI/color, NO_COLOR, supervised/machine, and no-TTY interactive fencing. |
| F-8 | accepted | `rg -n "quiet|verbosity|verbose|progress|statistics|human|supervised|H2|stream mutation|std::cout|contain|contained|G-008" ...` returned quiet/verbosity policy and H2 stream mutation containment. |
| F-9 | accepted | `rg -n "parser verifier|output helper|H2|cpddl|FAM|interactive|translation|SAT|BDD|CUDD|legacy|experimental|unsupported|future|stream|event|TTY|fence" ...` returned the non-supported surface fencing table. |
| F-10 | accepted | `rg -n "fmt|diagnostics/process|process I/O|facade|nlohmann/json|event-format|CLI11|slice04|Catch2|process fixture|slice05|Arc04|reproc|conditional|Abseil|Boost\\.Process|rejected" ...` returned the Arc02 dependency gates. |
| F-11 | accepted | `rg -n "slice04|CLI|option|slice05|contract test|Arc04|diagnostics|process I/O|shared substrate|Arc05|per-binary|Arc06|release docs|license|NOTICE|behavior-change" ...` returned later slice and arc consumer routing. |
| F-12 | accepted | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output before CDC edits. |

## Bubble-Up Check

Slice03 delivered the assigned Arc03 capability from the arc-plan slice
breakdown: stdout/stderr ownership, event-output mode, buffering/flushing,
diagnostics/progress routing, quiet modes, and ANSI/color/TTY policy.

No silent drops were found. The opening ledger had 12 rows, the closing ledger
has 12 rows, the closing report walks all 12 rows, and every requested
out-of-scope item is explicitly deferred to the owning later slice or arc.

CDC agrees that the slice findings require a small Arc03 plan update before
slice04 opens:

- mark slice03 closed and CDC-verified;
- mark the event-format question resolved as tagged text status-only output;
- route CLI spelling, aliases, help/version, no-color/no-colour controls, and
  machine-status enablement to slice04;
- keep `nlohmann/json` held unless a later accepted event-format decision
  reopens JSON.

## What Worked

- The contract gives supervisors parseable status observation without making
  diagnostic prose normative.
- Tagged text status-only output gives 0.3.0 a machine-readable path while
  avoiding premature JSON dependency adoption.
- The slice cleanly converts Arc02 library findings into design gates rather
  than letting libraries define product semantics.
