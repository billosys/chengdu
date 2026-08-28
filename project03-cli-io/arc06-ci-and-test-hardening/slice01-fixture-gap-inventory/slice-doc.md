# Arc06 Slice01: fixture-gap-inventory

## Goal

Create the Arc06 inventory that makes later hardening work precise: current
fixture coverage, Make/CI gate coverage, missing Arc03 matrix obligations, and
ownership routing for every gap. Add narrow Make entry points for fixture
listing if the current Make surface cannot list the managed fixture set without
calling harnesses directly.

This slice should produce no parser, grounder, or engine behavior change. It
is a map, a small Make-target repair if needed, and a gate inventory.

## Scope

In scope:

- Inventory current baseline and managed contract fixtures for parser,
  grounder, engine, and pipeline.
- Map current fixture cases to Arc03 contract-test matrix categories.
- Identify missing, deferred, duplicate, or superseded fixture obligations.
- Route every gap to Arc06 Slice02-Slice06, Arc07, Arc08, or no-op with
  rationale.
- Map current Make targets and GitHub Actions jobs to proof classes.
- Add Make fixture-list targets if needed so active workflows and handoffs do
  not teach direct `tests/contract/run` invocation.
- Use current source paths: `pandaPI/parser`, `pandaPI/grounder`,
  `pandaPI/engine`, and `pandaPI/runtime`.

Out of scope:

- No new parser, grounder, engine, or pipeline behavior.
- No fixture expansion beyond listing/inventory support.
- No coverage, sanitizer, TSan, `clang-tidy`, or workflow gate expansion yet.
- No release asset, checksum, manifest, package, publish, or wolong proof.
- No public README/tutorial/migration prose except narrow handoff references
  inside Arc06 planning artifacts.

## Verification Approach

Verification is primarily grep- and Make-backed:

- Make fixture-list targets expose baseline and managed fixture inventory.
- The inventory report lists current fixture counts and maps fixture IDs to
  Arc03 matrix categories.
- The gap table routes every missing/deferred item to a later slice or later
  arc.
- Existing non-mutating gates still pass.
- Active Arc06 docs and Make changes use current runtime/source paths.

## Exit Criteria

- Arc06 has a durable fixture/gate inventory report in the arc directory.
- CC can hand Slice02 a concrete first fixture-expansion batch rather than a
  vague "add more tests" instruction.
- No active workflow calls project scripts directly for new behavior.
- Existing build/test/provenance/format/static/actionlint gates still pass.

## Handoff

At close, the slice closing report must include a **Bubble-up to the arc**
section that names the recommended Slice02 fixture-expansion batch and any
Arc06 sequencing changes discovered by the inventory.
