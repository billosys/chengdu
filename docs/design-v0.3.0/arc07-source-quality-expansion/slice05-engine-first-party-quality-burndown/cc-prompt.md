# CC Prompt: Arc07 Slice05 - engine-first-party-quality-burndown

You are implementing Arc07 Slice05 on `release/0.3.x`.

## Required Context

Read these first:

1. `AGENTS.md`
2. `docs/design-v0.3.0/project-plan.md`
3. `docs/design-v0.3.0/arc07-source-quality-expansion/arc-plan.md`
4. `docs/design-v0.3.0/arc07-source-quality-expansion/source-classification-inventory.md`
5. `docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md`
6. `docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md`
7. `docs/design-v0.3.0/arc07-source-quality-expansion/slice04-first-party-quality-gate-scaffold/cdc-verification.md`
8. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice05-compiler-warning-burndown/warning-inventory.md`
9. `docs/design-v0.3.0/arc06-ci-and-test-hardening/slice06-binary-sanitizer-gates/closing-report.md`
10. `docs/design-v0.3.0/arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/slice-doc.md`
11. `docs/design-v0.3.0/arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/ledger.md`

Also inspect current Make/tool/source surfaces before editing:

- `Makefile`
- `mk/config.mk`
- `mk/help.mk`
- `mk/source-quality.mk`
- `mk/checks.mk`
- `mk/format.mk`
- `mk/coverage.mk`
- `mk/tests.mk`
- `mk/build.mk`
- `tools/source-quality/surface`
- `pandaPI/engine/src/CMakeLists.txt`
- `pandaPI/engine/src/visited_list.cpp`
- `pandaPI/engine/src/visited_list.h`
- nearby engine data-structure/search files only as needed for the
  `visited_list` seam.

## Policy

This is the first component source-quality burndown slice. It is engine-only.

Convert the Slice04 engine report-backed rows into honest Make-backed engine
targets. Prefer real executable gates. If a target cannot be made honest
inside this slice, stop and request an amendment rather than adding an empty
green target.

The key warning to burn down is the engine primary first-party
`visited_list.cpp` integer-to-pointer payload-cast warning. Do not silence it
with compiler pragmas or blanket suppressions. Add the smallest useful
unit/seam evidence around the payload representation or nearest safe
data-structure seam, then fix the representation if the evidence supports it.

Duplicate link-library warning noise is separate from first-party source
correctness debt. Fix it if the link graph can be cleaned safely; otherwise
classify it durably with a concrete re-entry condition.

Keep source classes intact:

- first-party maintained engine source is in scope;
- generated `cmdline.c` and `cmdline.h` are out of first-party maintained
  engine gates;
- vendored CUDD is out of first-party obligations;
- copied build trees are evidence/source-map material, not source;
- build output remains evidence only.

Do not broaden public engine behavior. Managed process-contract fixtures must
continue to pass.

## Task

Implement Slice05:

1. Add Make targets and help text:
   - `make format-check-engine`
   - `make static-analysis-engine`
   - `make coverage-engine`
   - `make warning-inventory-engine`
   - `make test-unit-engine`
   - `make sanitize-engine`
2. Reuse `compile-db-engine`, `source-quality-profile-map`,
   `source-quality-naming-check`, existing source-class selectors, and the
   Slice04 report machinery.
3. Make engine first-party selection explicit and exclude generated cmdline
   output, vendored CUDD, copied build trees, and build output.
4. Add the smallest engine unit/seam test surface needed to support the
   `visited_list` payload representation change.
5. Fix the `visited_list.cpp` integer-to-pointer payload-cast warning if the
   seam evidence makes the change safe. Stop for amendment if it is not safe
   or expands beyond this slice.
6. Fix or durably classify duplicate engine link-library warning noise.
7. Update durable Arc07 source-quality reports with engine component target
   status, warning disposition, coverage baseline/report placement, sanitizer
   mapping, and re-entry conditions.
8. Preserve existing managed engine behavior and all broader regression gates.

## Scope Constraints

Allowed surfaces:

- `Makefile`
- `mk/`
- `tools/source-quality/`
- engine build/test files under `pandaPI/engine/`
- focused engine source edits needed for the visited-list warning fix and seam
  test
- Arc07 source-quality reports
- Arc07 Slice05 docs/ledger/closing report

Do not change:

- parser or grounder source/build/test behavior;
- public `pandapi-*` CLI behavior;
- CI workflows unless a ledger row is amended to accept one;
- public README/tutorial docs;
- Arc08 or Arc09 docs;
- release/package/publication tooling;
- wolong migration guidance;
- dependency pins/fetch behavior;
- global `-Werror`, global warning fail policy, coverage floors, or
  static-analysis thresholds;
- vendored CUDD obligations or optional/fenced SAT/BDD/translation support.

Do not add a new `scripts/` entrypoint. All new automation must be reachable
through Make.

## Required Verification

Engine component evidence:

```text
make help
make compile-db-engine
make format-check-engine
make static-analysis-engine
make coverage-engine
make warning-inventory-engine
make test-unit-engine
make sanitize-engine
make build-engine
make test-contract-engine-managed
```

Cross-surface regression evidence:

```text
make source-quality-surface
make source-quality-profile-map
make source-quality-naming-check
make compile-db-first-party
make warning-inventory-first-party
make safety-checks
make actionlint
make static-analysis
make format-check
make test
make provenance-check
git diff --check
git diff --cached --check
```

Run `make check` if practical. If you do not run it, say exactly why and list
the narrower Make-backed gates that substitute for this slice.

Also run the ledger boundary checks:

- no parser/grounder changes;
- no `.github`, README, release, `tools/release`, Arc08, or Arc09 changes;
- no dependency pinning or fetch behavior;
- no global coverage floor, global warning failure policy, or global
  static-analysis threshold;
- no optional/fenced engine support promotion.

## Stop Conditions

Stop and request an amendment if:

- `visited_list.cpp` warning cleanup needs a broad engine representation
  rewrite rather than a focused seam-backed fix;
- engine component static analysis cannot be made honest without blanket
  suppressions;
- engine coverage cannot produce a meaningful baseline/report without mixing
  generated, CUDD, copied-build, or build-output paths as first-party source;
- `test-unit-engine` would be a process-contract fixture wrapper rather than
  an internal unit/seam test;
- sanitizer evidence cannot map findings back to source class;
- any parser/grounder, CI, public docs, release, dependency, or global
  threshold change appears necessary;
- optional/fenced SAT, BDD/CUDD, translation, or interactive behavior would be
  promoted as supported public behavior.

## Closing Requirements

Before committing:

- Close every ledger row with evidence.
- Write
  `docs/design-v0.3.0/arc07-source-quality-expansion/slice05-engine-first-party-quality-burndown/closing-report.md`.
- Include `Bubble-up to Arc07`, explicitly stating whether the next slice can
  proceed to parser generated-warning/root-cause work or whether engine
  remediation remains.
- Do not create `cdc-verification.md`; CDC writes it after independent review.
- Commit with both required trailers:

```text
Co-authored-by: Codex <noreply@openai.com>
Co-authored-by: Billo AI <ai-engineering@billo.systems>
```
