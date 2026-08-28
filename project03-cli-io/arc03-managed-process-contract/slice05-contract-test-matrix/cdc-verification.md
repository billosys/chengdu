# CDC Verification: contract-test-matrix

Date: 2026-08-09

Branch: `release/0.3.x`

Verified commit: `e4a277f805811328b8254b43656176df57fe2e38`
(`docs: add arc03 contract test matrix`)

## Verdict

Accepted. Slice05 is CDC-verified.

The contract test matrix report is complete for the slice scope, all 16 ledger
rows reproduce independently, the closing report does not drop any row, and
the slice stayed inside its design-only boundary.

CDC made one narrow documentation correction before closing: the report's
evidence-input section cited four arc-local reports under slice directories
where they do not live, and described Arc02 as upstream absorption rather than
C++ library research. The corrected paths now point to the accepted arc-local
reports and slice CDC verification files. This was citation repair only; it did
not change the test-matrix decisions or ledger scope.

The accepted test-matrix decisions are:

- process fixtures use a structured schema covering command, argv,
  environment, stdin/stdout/stderr, artifacts, exits, wait status, timeout,
  resource, TTY/color, and final `PANDAPI_STATUS` observation;
- golden comparisons freeze stable contract fields, while non-normative human
  diagnostic prose remains semantic/predicate-based;
- canonical `pandapi-*` and inherited `pandaPI*` commands both require probes;
- help, version, and provenance are successful stdout-owned informational
  commands, with `--status=stdout` conflicts classified as `cli_usage_error`;
- parser, grounder, engine positive probes include file-backed and stdout/pipe
  artifact paths plus pipeline composition;
- input/output/invalid-input failures, engine `domain_no_plan`, timeout,
  resource, interrupt, signal termination, color/TTY, and non-supported
  surfaces are all covered by explicit probe obligations;
- black-box process fixtures cannot be replaced by Catch2 seam tests;
- Catch2 remains test-only, CLI11 remains conditional on golden/process tests,
  fmt remains behind a diagnostics/process facade, `nlohmann/json` remains
  held, `tl::expected` remains behind a status/result substrate, reproc++
  remains conditional, and Abseil/Boost.Process remain rejected.

## Scope and Commit Check

Slice commit:

```bash
git show --name-status --format=full e4a277f8
```

Result: the commit included only:

- `docs/design-v0.3.0/arc03-managed-process-contract/contract-test-matrix.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/closing-report.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/ledger.md`

`git show --name-status --format=full e4a277f8` confirmed the required
co-author trailers:

- `Co-authored-by: Codex <noreply@openai.com>`
- `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Protected-path check against the slice commit:

```bash
git diff --name-only e4a277f8^..e4a277f8 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Format check against the slice commit:

```bash
git diff --check e4a277f8^..e4a277f8
```

Result: no output.

## Close-Set Verification

The required CC closing report exists:

```bash
test -f docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/closing-report.md
```

Result: command exit 0.

The closing report includes the expected close sections:

```bash
rg -n "Ledger Walk|Silent-Drop Diff|Bubble-Up To Arc03|CDC Handoff|16" docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/closing-report.md
```

Result: matched row count, ledger walk, silent-drop diff, Bubble-up to Arc03,
and CDC handoff sections.

The closing report's silent-drop diff is complete and honest: it defers
generated fixture data files and process harness implementation to Arc04/Arc05,
dependency adoption and build-policy changes to later implementation planning,
final `managed-process-contract.md` to slice06, and release documentation plus
wolong verification to Arc06.

## Ledger Verification

The closing ledger contains 16 `F-*` rows:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/ledger.md
```

Result: `16`.

No open rows remain:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice05-contract-test-matrix/ledger.md
```

Result: no output, command exit 1 from no matches.

| Row | CDC result | Evidence reproduced |
|-----|------------|---------------------|
| F-1 | accepted | `test -f ... && rg -n "Arc01|Arc02|slice01|slice02|slice03|slice04|audit-synthesis-pandapi|combined-library-recommendations|supported-surface-classification|status-exit-signal-taxonomy|stdio-event-tty-contract|cli-naming-version-migration" ...` returned the report and required evidence references. CDC corrected stale evidence paths before close. |
| F-2 | accepted | `rg -n "fixture schema|probe schema|command|argv|environment|stdin|stdout|stderr|artifact|exit code|wait status|signal|timeout|resource|TTY|color|PANDAPI_STATUS|observation" ...` returned the fixture schema and required observation fields. |
| F-3 | accepted | `rg -n "golden|stable field|semantic predicate|normalize|normalization|path|duration|build metadata|timestamp|errno|signal|diagnostic prose|non-normative|regex" ...` returned golden-output policy, semantic predicates, normalization, and diagnostic-prose limits. |
| F-4 | accepted | `rg -n "pandapi-parser|pandapi-grounder|pandapi-engine|pandaPIparser|pandaPIgrounder|pandaPIengine|compatibility|inherited|deprecation warning|warning|argv\\[0\\]" ...` returned canonical and inherited command probes and the no-default-warning rule. |
| F-5 | accepted | `rg -n "help|version|provenance|stdout|stderr|ok|exit 0|input/model processing|PANDAPI_STATUS|canonical command|compatibility name|chengdu version|upstream commit|build commit|platform|compiler|license|NOTICE" ...` returned informational command probes and provenance fields. |
| F-6 | accepted | `rg -n "cli_usage_error|exit 10|missing required|unknown option|malformed option|too many positional|--output|positional output|--status=stdout|conflict|input/model processing|output artifact|no artifact" ...` returned all required usage-error probe classes. |
| F-7 | accepted | `rg -n "parser positive|grounder positive|engine positive|valid|file-backed|stdout|pipe|artifact|stderr ownership|PANDAPI_STATUS|pipeline|parser-to-grounder|grounder-to-engine|composition" ...` returned parser, grounder, engine, stdout/file artifact, and pipeline-composition probes. |
| F-8 | accepted | `rg -n "input_unavailable|output_unavailable|input_invalid|missing input|unreadable|missing include|unavailable output|write|flush|finalize|malformed HDDL|malformed \\.htn|malformed engine|invalid input|status mapping" ...` returned unavailable and invalid input/output probes. |
| F-9 | accepted | `rg -n "domain_no_plan|no plan|unsolvable|unreachable|malformed input|timeout|resource_limit|dependency_failure|internal_error|distinct|engine" ...` returned engine solved/no-plan distinction and adjacent failure classes. |
| F-10 | accepted | `rg -n "timeout|resource_limit|resource limit|interrupt|interrupted|SIGINT|SIGTERM|signal_terminated|128 \\+ signal_number|safe CI|cleanup|partial_output_policy|final status|wait status|supervisor-visible" ...` returned timeout/resource/interrupt/signal probes and CI-safety constraints. |
| F-11 | accepted | `rg -n "color=auto|color=always|color=never|no-color|no-colour|NO_COLOR|TTY|no-TTY|ANSI|stderr|stdout artifact|tagged status|supervised|machine output|file|pipe|escape" ...` returned the color and TTY matrix. |
| F-12 | accepted | `rg -n "legacy_surface|experimental_surface|unsupported_feature|future_surface|parser verifier|output helper|H2|cpddl|FAM|interactive|translation|SAT|BDD|CUDD|visible|hidden|supported surface|not supported|no expansion" ...` returned all non-supported surface probes and no-expansion language. |
| F-13 | accepted | `rg -n "black-box|process fixture|Catch2|seam test|Arc04|substrate test|Arc05|per-binary|adoption gate|Arc06|release|CI|wolong|verification" ...` returned black-box/Catch2 split and ownership routing. |
| F-14 | accepted | `rg -n "CLI11|golden-output|golden output|process test|fmt|diagnostics/process|facade|nlohmann/json|held|JSON|Catch2|test-only|tl::expected|status/result|reproc|conditional|Abseil|Boost\\.Process|rejected" ...` returned preserved Arc02 dependency gates. |
| F-15 | accepted | `rg -n "slice06|managed-process-contract|Arc04|Arc05|Arc06|consumer|route|fixture obligation|probe obligation|deferred|optional surface|compatibility decision|dependency gate|behavior-change|release obligation|silent drop" ...` returned slice06 and later-arc consumer routing. |
| F-16 | accepted | `git diff --name-only e4a277f8^..e4a277f8 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output. |

## Bubble-Up Check

Slice05 delivered the assigned Arc03 row A6 input: the contract test matrix for
human CLI and supervised-process behavior across parser, grounder, and engine,
including positive/negative probes, fixture schema, comparison policy, and the
black-box process fixture versus Catch2 seam-test split.

No silent drops were found. The opening ledger had 16 rows, the closing ledger
has 16 rows, the closing report walks all 16 rows, and every requested
out-of-scope item is explicitly deferred to the owning later slice or arc.

CDC agrees that no Arc03 slice breakdown change is required before slice06.
The status surfaces should still be updated to mark slice05 CDC-verified and
make slice06 the next planned slice to open.

## What Worked

- The matrix converts the prior design slices into executable obligations
  without prematurely implementing a harness.
- The golden-output policy preserves machine stability while avoiding brittle
  human-prose tests.
- The black-box/Catch2 split keeps release/process behavior observable at the
  executable boundary while still giving Arc04 room for shared seam tests.
