# CDC Verification: managed-process-contract-synthesis

Date: 2026-08-09

Branch: `release/0.3.x`

Verified commit: `e87d4881d0c50002b50223d2bc4b997265671c91`
(`docs: synthesize arc03 managed process contract`)

## Verdict

Accepted. Slice06 is CDC-verified.

The final managed-process contract is complete for the slice scope, all 17
ledger rows reproduce independently, the closing report walks every row, and
the slice stayed inside its design-only boundary.

The accepted contract decisions are:

- `managed-process-contract.md` is the normative Arc03 output for Arc04,
  Arc05, and Arc06.
- The supported surface remains narrow: parser normal HDDL parse, grounder
  normal `.htn` grounding, and engine normal search.
- Inherited parser helpers, grounder `cpddl`/FAM, engine interactive mode,
  H2, translation, SAT, BDD, and CUDD are fenced as legacy, experimental,
  unsupported, or future work with explicit re-entry conditions.
- Canonical commands are `pandapi-parser`, `pandapi-grounder`, and
  `pandapi-engine`; inherited `pandaPI*` names remain executable in 0.3.0.
- The status vocabulary, numeric exit map, stream ownership, final
  `PANDAPI_STATUS` tagged-text record, color/TTY policy, help/version/
  provenance behavior, parse-error behavior, and contract-test obligations are
  accepted as the Arc03 contract.
- Arc02 dependency gates are preserved: standard-library modernization is the
  baseline; fmt, CLI11, and Catch2 enter only through their owning gates;
  `tl::expected` and reproc++ remain pilots; `nlohmann/json` and optional
  expansion libraries remain held; Abseil and Boost.Process remain rejected as
  0.3.0 foundation choices.
- Implementation conformance, release packaging, and wolong runtime proof are
  not claimed by Arc03; they are routed to Arc04, Arc05, and Arc06.

## Scope and Commit Check

Slice commit:

```bash
git show --name-status --format=full e87d4881
```

Result: the commit included only:

- `docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/closing-report.md`
- `docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/ledger.md`

`git show --name-status --format=full e87d4881` confirmed the required
co-author trailers:

- `Co-authored-by: Codex <noreply@openai.com>`
- `Co-authored-by: Billo AI <ai-engineering@billo.systems>`

Protected-path check against the slice commit:

```bash
git diff --name-only e87d4881^..e87d4881 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build
```

Result: no output.

Format check against the slice commit:

```bash
git diff --check e87d4881^..e87d4881
```

Result: no output.

## Close-Set Verification

The required CC closing report exists:

```bash
test -f docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/closing-report.md
```

Result: command exit 0.

The closing report includes the expected close sections:

```bash
rg -n "Ledger Walk|Silent-Drop Diff|Bubble-Up To Arc03|CDC Handoff|17" docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/closing-report.md
```

Result: matched row count, ledger walk, silent-drop diff, Bubble-up to Arc03,
and CDC handoff sections.

The closing report's silent-drop diff is complete and honest. It explicitly
routes planner source changes, dependency adoption, build-policy changes,
shared helpers, tests, wrappers, status writers, CLI parsers, diagnostics
facades, per-binary migrations, release documentation, and wolong verification
to Arc04, Arc05, or Arc06.

## Ledger Verification

The closing ledger contains 17 `F-*` rows:

```bash
rg -c '^\| F-' docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/ledger.md
```

Result: `17`.

No open rows remain:

```bash
rg -n '\| open \|' docs/design-v0.3.0/arc03-managed-process-contract/slice06-managed-process-contract-synthesis/ledger.md
```

Result: no output, command exit 1 from no matches.

| Row | CDC result | Evidence reproduced |
|-----|------------|---------------------|
| F-1 | accepted | The contract exists and names Arc01, Arc02, Arc03 slices 01-05, the audit synthesis, combined library recommendations, and all five accepted Arc03 input reports. |
| F-2 | accepted | The normative status section defines `MUST`, `SHOULD`, and `MAY`, and separates informative evidence, rationale, examples, implementation notes, and later-arc guidance. |
| F-3 | accepted | The surface table preserves parser, grounder, and engine classifications, including legacy, experimental, unsupported, and future surfaces. |
| F-4 | accepted | The command-name and release-shape section defines `pandapi-*`, inherited `pandaPI*`, wolong compatibility, release assets, checksums, manifest, and future breaking-removal requirements. |
| F-5 | accepted | Invocation modes and common options cover human CLI, supervised/process-manager, machine-status, `--supervised`, `--status`, stdout conflicts, quiet, verbose, progress, statistics, diagnostics, and machine behavior. |
| F-6 | accepted | The full status vocabulary and numeric exit-code table reproduce, including `128 + signal_number`. |
| F-7 | accepted | stdout/stderr ownership, final tagged-text `PANDAPI_STATUS`, required fields, flushing, partial-output policy, and the non-use of JSON Lines reproduce. |
| F-8 | accepted | Help, usage, examples, version, provenance, parse-error behavior, exit `10`, no input/model processing, and stable provenance fields reproduce. |
| F-9 | accepted | Color, no-color/no-colour, `NO_COLOR`, TTY/no-TTY, ANSI restrictions, supervised/machine restrictions, and legacy interactive TTY fencing reproduce. |
| F-10 | accepted | The contract-test section includes fixture schema, golden-output policy, normalization, positive and negative probes, timeout/resource/signal/color/non-supported probes, and black-box versus Catch2 ownership. |
| F-11 | accepted | Arc02 dependency gates reproduce for standard-library modernization, fmt, CLI11, Catch2, `tl::expected`, reproc++, `nlohmann/json`, Abseil, Boost.Process, and held optional dependencies. |
| F-12 | accepted | Arc04 shared-runtime substrate routing reproduces for status/result, diagnostics/process I/O, CLI parser, status writer/parser, TTY/color policy, output finalization, provenance, fixture harness, and seam tests. |
| F-13 | accepted | Arc05 per-binary routing reproduces for parser, grounder, engine, command names, compatibility names, positive paths, negative statuses, stream ownership, status emission, color/TTY, and non-supported surface fencing. |
| F-14 | accepted | Arc06 release-hardening routing reproduces for behavior changes, README/release notes, wolong verification, asset shape, manifest/checksum/provenance, license/NOTICE, THIRD-PARTY-LICENSES, CI, and test-only dependency exclusion. |
| F-15 | accepted | Deferrals and re-entry conditions reproduce for legacy, experimental, unsupported, future, optional dependency, JSON/event, SAT, BDD/CUDD, H2/cpddl, translation, and interactive surfaces. |
| F-16 | accepted | The contract states Arc03 close readiness and the remaining design-only caveat. |
| F-17 | accepted | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output. |

## Bubble-Up Check

Slice06 delivers the assigned final Arc03 artifact:
`docs/design-v0.3.0/arc03-managed-process-contract/managed-process-contract.md`.

No silent drops were found. The opening ledger had 17 rows, the closing ledger
has 17 rows, the closing report walks all 17 rows, and every out-of-scope item
is explicitly routed to Arc04, Arc05, Arc06, or a later re-entry decision.

CDC agrees with the slice06 bubble-up: Arc03 is ready for arc-level close,
subject to the normal arc composition check. The remaining caveat is not a
blocker because Arc03 is design-only; implementation conformance, release
packaging, and wolong runtime behavior remain owned by Arc04, Arc05, and Arc06.

## What Worked

- The synthesis kept the narrow supported surface intact instead of expanding
  behavior around inherited flags or available libraries.
- The final contract promotes Arc02 recommendations into gates without letting
  dependency choices decide product semantics.
- The later-arc handoff sections are concrete enough to plan Arc04-Arc06
  without re-reading every prior slice to rediscover obligations.
