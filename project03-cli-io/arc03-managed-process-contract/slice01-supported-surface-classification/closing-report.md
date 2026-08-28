# Slice 01 Closing Report - Supported Surface Classification

## 1. Outcome

Slice01 delivered the Arc03 supported-surface classification report at:

`docs/design-v0.3.0/arc03-managed-process-contract/supported-surface-classification.md`

The report classifies all required parser, grounder, and engine surfaces,
cites Arc01/Arc02 evidence, keeps dependency availability separate from product
support, and names the later Arc03 slices that consume each classification.
No planner source, dependency source, scripts, workflows, release assets, build
outputs, `vendor.env`, or `pins.env` were changed.

## 2. Ledger Row Walk

| ID | Final status | Evidence |
|----|--------------|----------|
| F-1 | done | The report exists and `test -f ... && rg -n "Arc01|Arc02|audit-synthesis-pandapi|combined-library-recommendations" ...` returned Arc01/Arc02 evidence references. |
| F-2 | done | `rg -n "supported|legacy|experimental|unsupported|future" ...` returned the vocabulary section and classification matrix entries. |
| F-3 | done | `rg -n "parser normal|HDDL parse|parser verifier|output helper|P-00[1-9]|P-01[0-5]|Re-entry|Behavior expectation|Rationale" ...` returned parser normal and parser verifier/output helper coverage. |
| F-4 | done | `rg -n "grounder normal|\\.htn|H2|cpddl|FAM|G-00[1-9]|G-01[0-9]|Re-entry|Behavior expectation|Rationale" ...` returned grounder normal, H2, and `cpddl`/FAM coverage. |
| F-5 | done | `rg -n "engine normal|normal search|interactive|translation|SAT|BDD|CUDD|E-00[1-9]|E-01[0-6]|Re-entry|Behavior expectation|Rationale" ...` returned engine normal, interactive, translation, SAT, and BDD/CUDD coverage. |
| F-6 | done | `rg -n "dependency availability|reproc|nlohmann/json|CUDD|H2|cpddl|parser generator|CLI11|fmt|does not decide|cannot make" ...` returned the dependency non-input section. |
| F-7 | done | `rg -n "slice02|status|exit|signal|slice03|stdio|event|TTY|slice04|CLI|naming|version|migration|slice05|test matrix|contract tests" ...` returned later Arc03 slice inputs. |
| F-8 | done | `git diff --name-only -- pandaPI scripts .github README.md release-manifest.txt vendor.env pins.env dist build` produced no output. |

No ledger rows were dropped, deferred, or marked no-op.

## 3. Delivered Classification

| Surface | Disposition |
|---------|-------------|
| parser normal HDDL parse path | `supported` |
| parser verifier/output helper paths | `legacy` |
| grounder normal `.htn` to planner-output path | `supported` |
| grounder H2 path | `experimental` |
| grounder `cpddl`/FAM integration | `legacy` |
| engine normal search path | `supported` |
| engine interactive mode | `legacy` |
| engine translation mode | `experimental` |
| engine SAT path | `unsupported` |
| engine BDD/CUDD path | `future` |

The supported 0.3.0 contract surface is therefore limited to the normal parser,
normal grounder, and normal engine search paths.

## 4. Bubble-up to the Arc

### Assigned Arc Capability

Arc03 assigned slice01 to classify supported, legacy, experimental,
unsupported, and future process surfaces before the arc defines status,
stdio/event/TTY, CLI naming/version, and contract tests. This slice delivered
that piece of the arc capability.

### New Arc Findings

The slice sharpened several later-slice inputs:

- H2 is `experimental`, not supported by dependency availability or by the
  presence of reproc++ as a candidate.
- `cpddl`/FAM is `legacy` as a user-facing advanced surface; dependency
  internals remain outside support decisions.
- Engine translation is `experimental`, so child-process containment is not a
  default 0.3.0 requirement.
- Engine SAT is `unsupported` for 0.3.0, while BDD/CUDD is `future`.

These findings fit the existing Arc03 slice breakdown. They do not require an
`arc-plan.md` body change before slice02, because slice02 through slice05 were
already planned to consume exactly these classifications for status, stream,
CLI, and test matrix design.

### Silent-Drop Diff

Scope as specified:

- Create the supported-surface classification report.
- Cover every required parser, grounder, and engine surface.
- Cite Arc01 and Arc02 evidence.
- Use the agreed five-word vocabulary.
- Separate dependency availability from support decisions.
- Identify later Arc03 slice consumers.
- Leave source/build/release/protected paths untouched.

Scope as delivered:

- All specified items were delivered.
- No source, script, workflow, release, build, `vendor.env`, or `pins.env`
  path was changed.
- CDC verification remains intentionally unfilled for the independent reviewer.

No silent drops identified.
