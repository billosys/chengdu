# CDC Verification - Arc06 Slice05 - compiler-warning-burndown

Verifier: CDC
Verified on: 2026-08-12
Implementation commit reviewed: `fc8b671d`
Evidence commit reviewed: `85a396ff`
Current branch during verification: `release/0.3.x`

## Verdict

GO. Arc06 Slice05 is CDC-verified.

The slice delivered a reproducible Make-backed compiler warning inventory and
burned down the low-risk primary hand-written warning classes observed in
parser, grounder, and engine source. The remaining warning stream is classified
by ownership tier instead of hidden behind a generic inherited-warning label.

The warning state is clean enough for Slice06 binary sanitizer gates to produce
useful release-readiness signal. Runtime remains zero-warning, parser and
grounder primary warnings observed in this slice are gone, and the remaining
engine primary warning is budgeted with a concrete sanitizer/release re-entry
condition.

## Source and Scope Checks

- `git show --stat --oneline --name-status fc8b671d` confirmed the
  implementation scope is limited to Make warning-inventory wiring, Arc06
  documentation, runtime README quality notes, and narrow parser, grounder, and
  engine warning fixes.
- `git show --stat --oneline --name-status 85a396ff` confirmed the follow-up
  evidence commit only records Slice05 ledger and closing-report evidence.
- `git show --format=fuller --no-patch fc8b671d 85a396ff` confirmed both
  required co-author trailers are present on both commits.
- `mk/checks.mk`, `mk/config.mk`, and `mk/help.mk` expose
  `make warning-inventory` plus the compatibility aliases
  `make compiler-warning-inventory` and `make warning-burndown`; generated
  warning reports are written under ignored `build/warnings/<platform>/`.
- The source edits do not change release publication, Arc07, Arc08, or
  unsupported optional-surface behavior.

## Independent Verification Table

| Row | CDC result | Evidence |
|-----|------------|----------|
| F-1 | reproduced | `make help | rg -n "warning-inventory|compiler-warning|warning-burndown"` lists `make warning-inventory`; `make warning-inventory` passed with `Warning lines: 56`. |
| F-2 | reproduced | Parser warnings are classified in `warning-inventory.md`; the remaining parser warning class is generated Flex helper code, and primary parser warnings in `hpdlWriter.cpp`, `plan.cpp`, and `verify.cpp` were fixed. |
| F-3 | reproduced | Grounder warnings are classified in `warning-inventory.md`; primary grounder warnings were fixed or marked no-op/debug-only, while remaining cpddl/boruvka/opts/lpsolve/RSS/H2 warnings are dependency-internal. |
| F-4 | reproduced | Engine warnings are classified in `warning-inventory.md`; low-risk primary/link-flag warnings were fixed, while `VisitedList.cpp` payload cast and duplicate link-library noise remain budgeted with re-entry conditions. |
| F-5 | reproduced | Runtime is documented as the zero-warning owned surface, and `make warning-inventory` produced no runtime warning lines. |
| F-6 | reproduced | `warning-inventory.md` records the fixed primary-source classes and the remaining primary engine budget with a visited-list/sanitizer/release-gate re-entry condition. |
| F-7 | reproduced | Generated and nested dependency warnings are isolated by class and source; no blanket suppressions or global `-Werror` policy was introduced. |
| F-8 | reproduced | `make safety-checks && make actionlint` passed; no new workflow behavior bypasses Make. |
| F-9 | reproduced | `make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check` passed. |
| F-10 | reproduced | Boundary review found no README, release package, Arc07, Arc08, or unsupported optional-surface edits in the Slice05 implementation scope. |

## Commands Reproduced

```text
make help | rg -n "warning-inventory|compiler-warning|warning-burndown"
make warning-inventory
ledger greps for warning inventory, P-011, G-015, E-011, runtime zero-warning, primary budgets, generated/dependency isolation, and suppression policy
make safety-checks && make actionlint
make static-analysis && make format-check && make test && make provenance-check && git diff --check && git diff --cached --check
git show --stat --oneline --name-status fc8b671d
git show --stat --oneline --name-status 85a396ff
git show --format=fuller --no-patch fc8b671d 85a396ff
```

Observed results:

- `make warning-inventory`: passed and reported `Warning lines: 56`.
- Warning report path:
  `build/warnings/macos-arm64/warning-inventory.txt`.
- Full build log path: `build/warnings/macos-arm64/build.log`.
- Runtime CTest during `make test`: 5/0.
- Baseline contract fixtures: 38/0.
- Managed fixtures during `make test`: parser 298/0, grounder 269/0, engine
  312/0, pipeline 129/0.
- Positive smoke: 3/0. Negative smoke: 4/0.
- Provenance check passed for all three components on `macos-arm64`.

## Review Notes

No blocking findings remain.

The remaining warning budget is specific enough for follow-on work:

- Parser: generated Flex scanner helpers remain; primary parser warning debt
  observed here was burned down.
- Grounder: dependency-internal warnings remain in cpddl/boruvka/opts/lpsolve,
  RSS, and the H2 preprocessor.
- Engine: `VisitedList.cpp` still carries an integer-to-pointer payload cast;
  duplicate static-library link noise remains.
- Runtime: zero warning lines remain the owned-runtime policy.

The engine `VisitedList.cpp` warning is the most release-relevant budget item.
CDC agrees with the documented decision not to rewrite that representation
inside Slice05 without targeted visited-list fixture or sanitizer evidence.

## Bubble-up to Arc06

Slice05 closes Arc06 row A7: compiler warnings are now inventoried by ownership
tier, low-risk primary hand-written warnings are fixed, and generated or nested
dependency warning debt is isolated rather than normalized away.

Slice06 should open as `binary-sanitizer-gates`. It should reuse the expanded
managed fixture workload from Slice02, the owned-runtime sanitizer baseline
already exposed by `make test-runtime-sanitize`/`make sanitize-runtime`, and
the Slice05 warning ownership budget so ASan/UBSan/LSan findings can be triaged
by source ownership rather than by build-noise volume.
