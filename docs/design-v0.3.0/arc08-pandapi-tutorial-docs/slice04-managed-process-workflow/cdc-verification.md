# Arc08 Slice04 CDC Verification: managed-process-workflow

Status: CDC verified and closed.

Verified: 2026-08-13 on `release/0.3.x`.

## Review Basis

CDC reviewed the Slice04 ledger, closing report, public managed-process page,
Arc08 plan, and project plan. The opening ledger has 25 rows, and the closing
report walks F-1 through F-25 without silent drops.

## Reproduced Evidence

CDC reproduced the static public-doc checks for:

- accepted public docs path and managed-process introduction;
- link back to the first project workflow;
- `--supervised`, `--status`, `--status=stderr`, and `--status=stdout`;
- stdout/stderr ownership and the no-prose-scraping rule;
- final `PANDAPI_STATUS` records and required fields;
- common status/exit classes;
- ANSI/color/TTY behavior;
- successful parser, supervised pipeline, no-plan, invalid-input, and stdout
  conflict examples;
- canonical command-name policy;
- absence of internal planning vocabulary from the public page;
- Arc08/project plan status and Arc09 release-publication boundary.

CDC reproduced the documented supervised examples with one fresh temporary
workspace after `make build`:

- supervised parser success: non-empty `.htn` artifact, empty stdout, no ANSI
  leakage, and parser `status=ok` on stderr;
- supervised parser -> grounder -> engine pipeline: non-empty `.htn`, `.sas`,
  and plan artifacts with parser, grounder, and engine `status=ok`;
- valid no-plan: parser and grounder succeeded, engine exited `2`, and stderr
  matched `domain_no_plan` / `exit_code=2`;
- invalid input: parser exited `22`, stdout stayed empty, and stderr matched
  `input_invalid` / `exit_code=22`;
- stdout status/artifact conflict: parser exited `10`, and stdout carried
  `cli_usage_error` / `exit_code=10`.

CDC reproduced the managed contract gates:

```sh
make test-contract-parser-managed
make test-contract-grounder-managed
make test-contract-engine-managed
make test-contract-pipeline-managed
```

Results:

- managed parser contract: `298 passed, 0 failed`;
- managed grounder contract: `269 passed, 0 failed`;
- managed engine contract: `312 passed, 0 failed`;
- managed pipeline contract: `129 passed, 0 failed`.

The first combined run produced a very large transcript; CDC reran
`make test-contract-grounder-managed` separately and captured the summary tail
to pin the exact grounder evidence.

## Boundary And Whitespace

CDC reran the protected-path and whitespace checks during the combined
Slices03-05 verification commit:

```sh
git diff --check
git diff --cached --check
```

The Slice04 implementation stayed within public docs and Arc08/project
planning scope. No `README.md`, release, `.github/`, Make, `mk/`, tests,
tools, fixtures, `pandaPI/`, or license surfaces were part of the Slice04
diff.

## Result

All F-1 through F-25 rows reproduced. Slice04 is CDC verified and closed.

## Bubble-up to Arc08

Slice04 satisfies Arc08 A4 and contributes to A8. The public guide now gives
process-manager integrators a verified supervised-process path based on exit
codes and final `PANDAPI_STATUS` records rather than diagnostic prose.
Slice07 can link this guide from CLI reference and migration docs, and Arc09
can use it as the wolong managed-process documentation handoff while still
owning release-asset installation proof.
