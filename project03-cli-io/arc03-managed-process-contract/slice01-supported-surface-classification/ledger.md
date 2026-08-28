# Slice 01: supported-surface-classification

## Ledger

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The supported-surface classification report exists at `docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` and names Arc01 and Arc02 as evidence inputs. | `test -f docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md && rg -n "Arc01|Arc02|audit-synthesis-pandapi|combined-library-recommendations" docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` | serious | arc ledger A2 | done | `test -f ... && rg -n ...` returned the report path plus Arc01/Arc02 and source artifact references. | |
| F-2 | The report uses the agreed classification vocabulary: supported, legacy, experimental, unsupported, and future. | `rg -n "supported|legacy|experimental|unsupported|future" docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` | serious | slice-doc §3 | done | `rg -n ...` returned the vocabulary section and classification matrix entries for all five terms. | |
| F-3 | The report classifies parser normal HDDL parse path and parser verifier/output helper paths with rationale, audit evidence, behavior expectation, and re-entry condition where applicable. | `rg -n "parser normal|HDDL parse|parser verifier|output helper|P-00[1-9]|P-01[0-5]|Re-entry|Behavior expectation|Rationale" docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` | correctness-grade | Arc01 synthesis §4 | done | `rg -n ...` returned parser normal and verifier/output helper sections with `P-*` evidence, rationale, behavior expectation, and re-entry text. | |
| F-4 | The report classifies grounder normal `.htn` path, H2 path, and `cpddl`/FAM integration with rationale, audit evidence, behavior expectation, and re-entry condition where applicable. | `rg -n "grounder normal|\\.htn|H2|cpddl|FAM|G-00[1-9]|G-01[0-9]|Re-entry|Behavior expectation|Rationale" docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` | correctness-grade | Arc01 synthesis §4; Arc02 hold decisions | done | `rg -n ...` returned grounder normal, H2, and `cpddl`/FAM sections with `G-*` evidence, rationale, behavior expectation, and re-entry text. | |
| F-5 | The report classifies engine normal search, interactive mode, translation mode, SAT path, and BDD/CUDD path with rationale, audit evidence, behavior expectation, and re-entry condition where applicable. | `rg -n "engine normal|normal search|interactive|translation|SAT|BDD|CUDD|E-00[1-9]|E-01[0-6]|Re-entry|Behavior expectation|Rationale" docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` | correctness-grade | Arc01 synthesis §4; Arc02 hold decisions | done | `rg -n ...` returned engine normal, interactive, translation, SAT, and BDD/CUDD sections with `E-*` evidence, rationale, behavior expectation, and re-entry text. | |
| F-6 | The report keeps dependency availability separate from support decisions for reproc++, nlohmann/json, CUDD, H2, cpddl, parser generators, CLI11, and fmt. | `rg -n "dependency availability|reproc|nlohmann/json|CUDD|H2|cpddl|parser generator|CLI11|fmt|does not decide|cannot make" docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` | correctness-grade | Arc01/Arc02 non-inputs | done | `rg -n ...` returned the dedicated dependency non-input section naming each required dependency or dependency family. | |
| F-7 | The report identifies which later Arc03 slices consume each classification for status/exit/signal, stdio/event/TTY, CLI naming/version/migration, and contract tests. | `rg -n "slice02|status|exit|signal|slice03|stdio|event|TTY|slice04|CLI|naming|version|migration|slice05|test matrix|contract tests" docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md` | serious | arc slice sequencing | done | `rg -n ...` returned per-surface later-slice inputs and the dedicated later Arc03 slice input section. | |
| F-8 | No planner source, dependency source, scripts, workflows, release assets, build outputs, or vendored dependency files are changed by this slice. | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` | serious | diagnosis/design-only constraint | done | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output. | |

## What Worked

- Arc01's supported-surface table gave enough product evidence to accept the
  three normal paths without letting optional inherited modes leak into the
  0.3.0 contract.
- Arc02's dependency recommendations made the non-input boundary explicit:
  dependencies can implement an accepted surface, but cannot make a surface
  supported.

## Open Follow-Up

- CDC still needs to write `cdc-verification.md` and independently rerun the
  ledger evidence before this slice is verification-closed.
