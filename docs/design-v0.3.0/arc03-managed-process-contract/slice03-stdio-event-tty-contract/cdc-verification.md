# CDC Verification: stdio-event-tty-contract

Date: 2026-08-09

Branch: `release/0.3.x`

Verified commit: `e465f42736db9d443b196621d06436c6e644ee2f`
(`Close Arc03 stdio event tty contract`)

## Verdict

Not accepted yet. The stdio/event/TTY contract content reproduces against all
12 ledger rows, and the slice stayed inside its design-only boundary, but the
required CC close-set artifact is missing:

`docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/closing-report.md`

Per `PROJECT-MANAGEMENT.md`, a slice close requires both the CC
`closing-report.md` and the independent CDC `cdc-verification.md`. The
`cc-prompt.md` for this slice also explicitly required CC to write the closing
report with a row-by-row walk and Bubble-up to Arc03.

CDC should rerun verification after CC adds the missing closing report. The
technical ledger evidence below can be reused if the contract report and
ledger do not change.

## Scope and Commit Check

`git show --name-status --format=full e465f42736db9d443b196621d06436c6e644ee2f`
showed only these slice content changes:

- `docs/design-v0.3.0/arc03-managed-process-contract/stdio-event-tty-contract.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/ledger.md`

It did not include:

- `docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/closing-report.md`

`git show -s --format=full e465f42736db9d443b196621d06436c6e644ee2f`
confirmed the required co-author trailers:

- `Co-authored-by: Codex <noreply@openai.com>`
- `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Protected-path check:

```bash
git diff --name-only HEAD^..HEAD -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Format check:

```bash
git diff --check HEAD^..HEAD
```

Result: no output.

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

CDC cannot accept the slice bubble-up yet because the CC closing report is
absent. Without the closing report, there is no CC-authored row-by-row walk,
no explicit silent-drop diff, and no CC Bubble-up to Arc03 for CDC to verify.

Based on the contract artifact itself, the likely bubble-up is:

- slice03 delivered the assigned Arc03 piece for stdout/stderr/event/status
  routing, buffering/flushing, ANSI/color/TTY behavior, quiet/progress policy,
  and non-supported-surface stream fencing;
- slice04 must map semantic modes and color/status behavior to final CLI
  spelling, aliases, help, and version behavior;
- slice05 must convert the stream/status/TTY contract into process fixtures;
- Arc04 must own the diagnostics/process I/O facade, status line escaping,
  buffering/flushing helpers, and TTY/color detection.

This likely bubble-up is not accepted as a substitute for CC's required
closing report.

## Required Fix Before Acceptance

CC needs to add:

`docs/design-v0.3.0/arc03-managed-process-contract/slice03-stdio-event-tty-contract/closing-report.md`

That report should include:

- the 12-row ledger walk matching the opening ledger count;
- the same final status disposition reflected in `ledger.md`;
- the Bubble-up to Arc03;
- a silent-drop diff for scope-as-specified versus scope-as-delivered;
- the protected-path and whitespace verification evidence.

After that lands, CDC should rerun the closing-report check and either replace
or supersede this verification with an accepted one.
