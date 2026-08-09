# CDC Verification: parser-audit

Date: 2026-08-09

Branch: `release/0.3.x`

CC commits:

- `948c54d3` - `docs: open vendored source audit slice`
- `afe18cd4` - `docs: close parser audit slice`

Verdict: **accepted**. Slice01 is CDC-verified.

## Row Count

Opening ledger row count at `948c54d3`: 12.

Closing ledger row count at `afe18cd4`: 12.

No ledger rows were dropped.

## Commit and Scope Check

`afe18cd4` changes only:

- `docs/design-v0.3.0/arc01-vendored-source-audit/slice01-parser-audit/closing-report.md`
- `docs/design-v0.3.0/arc01-vendored-source-audit/slice01-parser-audit/ledger.md`
- `workbench/2026.08.09-audit-results-pandapi-parser.md`

Both `948c54d3` and `afe18cd4` include the required assistant co-author
trailers.

Protected-path check:

```bash
git diff --name-only 948c54d3..afe18cd4 -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env
```

Result: no output. No parser source, scripts, workflows, README, release
manifest, `vendor.env`, or `pins.env` changed in the close commit.

Current worktree after CDC rebuild/probes:

```bash
git status --short --branch
```

Result:

```text
## release/0.3.x
```

## Ledger Verification

CDC reran every ledger Verify command. All rows reproduce.

| Row | CDC disposition | Evidence |
|-----|-----------------|----------|
| F-1 | accepted | Report exists and `rg` finds project root, branch, audited paths, C++ knowledge-pack guides, Core Guidelines references, and `11-anti-patterns`. |
| F-2 | accepted | Report inventories `pandaPI/pandaPIparser/src`, `options.ggo`, `makefile`, `tests`, source file count, and fixture count. |
| F-3 | accepted | Report contains the required finding groups: correctness/runtime safety, error handling, stdio/TTY, CLI/API, build/tests, maintainability, and modernization. |
| F-4 | accepted | Findings P-001 through P-015 include severity and exact parser file/line citations. |
| F-5 | accepted | Report covers status/exit behavior, including deep `exit()` paths and observed statuses 1, 2, 255, and 0. |
| F-6 | accepted | Report covers stdout/stderr contamination, buffering/pipe implications, ANSI/color, `--no-colour`, and TTY policy. |
| F-7 | accepted | Report covers `options.ggo`, inherited package/version/help identity, build wrapper publication, and `pandapi-*` migration implications. |
| F-8 | accepted | Report identifies duplicate output-file, diagnostic, color, status, and shared helper candidates. |
| F-9 | accepted | Report lists eight parser-scope clean checks. |
| F-10 | accepted | Report lists grounder, engine, and shared/library synthesis inputs. |
| F-11 | accepted | Report maps source-quality findings to C++ Core Guidelines rule IDs where applicable and labels Chengdu-specific managed-process findings separately. |
| F-12 | accepted | Protected-path diff check produced no output. |

Additional format check:

```bash
git diff --check
```

Result: no output.

## Behavioral Reproduction

CDC rebuilt the parser:

```bash
./scripts/build-parser.sh
```

Result: exit 0; produced `dist/macos-arm64/pandaPIparser`. The warning profile
matched the report's characterization: generated Flex helper warnings,
unused/set-but-unused warning, range-loop copy warning, and sign-compare
warnings.

CDC then ran subprocess probes against `dist/macos-arm64/pandaPIparser`.
Observed results:

| Probe | Status | Reproduced behavior |
|-------|--------|---------------------|
| missing args | 1 | Diagnostic appears on stdout; configuration text appears on stderr. |
| missing file | 2 | Missing-file diagnostic appears on stdout; configuration text appears on stderr. |
| malformed input | 255 | Parse progress appears on stdout; stderr contains ANSI escapes. |
| malformed input with `--no-colour` | 255 | Stderr still contains ANSI escapes, reproducing the report's `--no-colour` defect. |
| valid empty fixture parse | 0 | Parse progress for domain/problem appears on stdout. |

## Source Sampling

CDC sampled the report's high-severity source citations:

- `pandaPI/pandaPIparser/makefile:5` sets `-DNDEBUG`; `output.cpp:180-250`
  relies on `assert()` for model/output invariants.
- `hddl.y:596-608` assigns raw `FILE*` to `yyin`, recurses through
  `fopen(file, "r")`, and does not check the include open result.
- `hddl.y:619-626` hard-codes ANSI escapes in `yyerror()` and exits with
  `exit(-1)`.
- `main.cpp:188-229` prints missing-argument and missing-file diagnostics to
  `cout`; `main.cpp:231-240` prints an ordinary-output arity error but
  continues into parsing.
- `verify.cpp:27-80` and `verify.cpp:1318-1321` call `exit()` from helper code,
  including `exit(0)` for an empty plan.
- `util.hpp:3` exports `using namespace std;`; `options.ggo:1-4` preserves the
  inherited `pandaPIparser` package/version/usage identity.

The sampled citations support the report's severities and recommendations.

## Bubble-Up Check

Delivered-as-assigned: yes. Slice01 delivered the parser audit assigned by
`arc-plan.md`: a diagnosis-only report with exact source citations, C++ Core
Guidelines rule mapping where applicable, managed-process findings, clean
checks, and synthesis inputs.

Silent-drop diff: complete. The slice did not refresh upstream state, but the
assignment explicitly allowed the in-tree fork baseline. The slice did not fix
source defects, as required by the diagnosis-only boundary.

Arc-plan impact: no slice breakdown change is required. The parser audit
confirmed the planned sequencing: grounder audit next, engine audit after that,
then synthesis. The only status updates required are to mark slice01
CDC-verified and make slice02 the next unopened slice.

## What Worked

- The C++ knowledge pack gave the report a concrete rule-ID substrate without
  displacing Chengdu's managed-process rubric.
- The report cleanly separated C++ Core Guidelines findings from
  Chengdu-specific process-contract findings.
- The parser probes gave stronger evidence than static inspection alone for
  stdout/stderr, ANSI, and exit-status findings.
