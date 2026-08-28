# CDC Verification: cli-naming-version-migration

Date: 2026-08-09

Branch: `release/0.3.x`

Verified commit: `09b15bd25e070adbe9997ec93fbc5cc5c3f20e97`
(`Close Arc03 CLI naming migration contract`)

## Verdict

Accepted. Slice04 is CDC-verified.

The CLI naming, version, provenance, help, compatibility, and migration report
is complete for the slice scope, all 12 ledger rows reproduce independently,
the closing report does not drop any row, and the slice stayed inside its
design-only boundary.

The accepted design decisions are:

- canonical command names are `pandapi-parser`, `pandapi-grounder`, and
  `pandapi-engine`;
- 0.3.0 is a compatibility transition, not a breaking rename;
- inherited `pandaPIparser`, `pandaPIgrounder`, and `pandaPIengine` remain
  executable in release assets and installed trees;
- inherited compatibility commands do not emit default runtime deprecation
  warnings;
- release asset names, `SHA256SUMS`, `release-manifest.txt`, and the
  four-command fetch/verify/extract/run path remain compatible with wolong;
- `--supervised` selects quiet process-manager behavior;
- `--status` enables one final `PANDAPI_STATUS` record and is equivalent to
  `--status=stderr`;
- `--status=stdout` is allowed only when stdout is not carrying a data
  artifact or human informational output;
- `--no-color` is canonical and `--no-colour` remains a compatibility spelling;
- help, version, and provenance are successful stdout-owned informational
  commands and exit `ok`/`0`;
- malformed CLI shape maps to `cli_usage_error`/`10` before input/model
  processing;
- CLI11 is the selected CLI parser implementation candidate only behind
  golden-output and process-contract tests.

## Scope and Commit Check

Slice commit:

```bash
git show --name-status --format=full 09b15bd25e070adbe9997ec93fbc5cc5c3f20e97
```

Result: the commit included only:

- `docs/design-v0.3.0/arc03-managed-process-contract/cli-naming-version-migration.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/closing-report.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/ledger.md`

`git show --name-status --format=full` confirmed the required co-author
trailers:

- `Co-authored-by: Codex <noreply@openai.com>`
- `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Protected-path check against the slice commit:

```bash
git diff --name-only 09b15bd2^..09b15bd2 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Format check against the slice commit:

```bash
git diff --check 09b15bd2^..09b15bd2
```

Result: no output.

## Close-Set Verification

The required CC closing report exists:

```bash
test -f docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/closing-report.md
```

Result: command exit 0.

The closing report includes the expected close sections:

```bash
rg -n "Ledger Walk|Silent-Drop Diff|Bubble-Up To Arc03|CDC Handoff|12" docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/closing-report.md
```

Result: matched row count, ledger walk, silent-drop diff, Bubble-up to Arc03,
and CDC handoff sections.

The closing report's silent-drop diff is complete and honest: it defers final
contract tests to slice05, implementation of aliases/wrappers/symlinks/binary
renaming/CLI11/shared CLI helpers to Arc04 and Arc05, release documentation and
wolong verification to Arc06, and treats any future breaking rename as requiring
explicit operator approval.

## Ledger Verification

The closing ledger contains 12 `F-*` rows:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/ledger.md
```

Result: `12`.

No open rows remain:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice04-cli-naming-version-migration/ledger.md
```

Result: no output, command exit 1 from no matches.

| Row | CDC result | Evidence reproduced |
|-----|------------|---------------------|
| F-1 | accepted | `test -f ... && rg -n "Arc01|Arc02|slice01|slice02|slice03|audit-synthesis-pandapi|combined-library-recommendations|supported-surface-classification|status-exit-signal-taxonomy|stdio-event-tty-contract" ...` returned the report and required evidence references. |
| F-2 | accepted | `rg -n "pandapi-parser|pandapi-grounder|pandapi-engine|pandaPIparser|pandaPIgrounder|pandaPIengine|compatibility|alias|wrapper|symlink|warning|deprecation|breaking|migration|rename" ...` returned canonical names and inherited-name compatibility policy. |
| F-3 | accepted | `rg -n "release asset|tarball|manifest|SHA256SUMS|install|path|wolong|frozen fetch|fetch/install|compatibility|breaking transition|operator-approved|migration" ...` returned release asset, tarball, manifest, SHA256SUMS, install, wolong, and breaking-transition policy. |
| F-4 | accepted | `rg -n "synopsis|usage|argument|required|optional|parser normal|HDDL parse|grounder normal|\\.htn|engine normal|normal search|supported surface|legacy|experimental|unsupported|future|expand" ...` returned the supported command synopsis and non-expansion language. |
| F-5 | accepted | `rg -n "human CLI|supervised|process-manager|machine-status|tagged status|PANDAPI_STATUS|status-only|option|flag|stdout|stderr|artifact|owned|ownership" ...` returned `--supervised`, `--status`, tagged status, stdout/stderr, and ownership semantics. |
| F-6 | accepted | `rg -n "help|usage|example|parse error|parser error|cli_usage_error|exit 10|exit code 10|ok|exit 0|malformed|invalid option|missing|required|continue|input processing|model processing" ...` returned help, examples, `ok`/`0`, and `cli_usage_error`/`10` parse-error behavior. |
| F-7 | accepted | `rg -n "version|provenance|chengdu|upstream|pandaPI|commit|source|build|platform|compiler|license|NOTICE|ok|exit 0" ...` returned version/provenance fields and exit `ok`/`0` policy. |
| F-8 | accepted | `rg -n "color|colour|no-color|no-colour|NO_COLOR|ANSI|TTY|stderr|stdout|auto|human|supervised|machine|escape|compatibility" ...` returned color/no-color/no-colour/NO_COLOR and slice03 TTY policy. |
| F-9 | accepted | `rg -n "legacy_surface|experimental_surface|unsupported_feature|future_surface|legacy|experimental|unsupported|future|hidden|help|diagnostic|status|surface|parser verifier|output helper|H2|cpddl|FAM|interactive|translation|SAT|BDD|CUDD" ...` returned non-supported surface presentation and status mapping. |
| F-10 | accepted | `rg -n "CLI11|golden-output|golden output|fmt|diagnostics/process|facade|nlohmann/json|held|JSON|Catch2|process fixture|slice05|Arc04|reproc|conditional|Abseil|Boost\\.Process|rejected" ...` returned CLI11 golden-output gating and preserved Arc02 dependency dispositions. |
| F-11 | accepted | `rg -n "slice05|contract test|golden|fixture|Arc04|CLI|diagnostics|shared substrate|Arc05|per-binary|migration|Arc06|release docs|license|NOTICE|behavior-change|wolong|verification" ...` returned later consumer routing for slice05 and Arc04-Arc06. |
| F-12 | accepted | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output before CDC edits. |

## Bubble-Up Check

Slice04 delivered the assigned Arc03 row A5 input: canonical `pandapi-*`
binary naming, inherited-name compatibility, help/version/provenance output,
CLI parse-error behavior, status-option spelling, color/no-color/no-colour
policy, and wolong release-shape implications.

No silent drops were found. The opening ledger had 12 rows, the closing ledger
has 12 rows, the closing report walks all 12 rows, and every requested
out-of-scope item is explicitly deferred to the owning later slice or arc.

CDC agrees that no Arc03 slice breakdown change is required before slice05.
The status surfaces should still be updated to mark slice04 CDC-verified and
make slice05 the next planned slice to open.

## What Worked

- The compatibility-transition decision introduces namespaced `pandapi-*`
  commands without breaking wolong's inherited command paths.
- The report preserves the accepted stream/status contract while adding CLI
  spelling and help/version/provenance semantics.
- CLI11 is correctly treated as an implementation mechanism gated by golden
  tests, not as the source of product behavior.
