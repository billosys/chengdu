# Arc03 Slice01 - Supported Surface Classification

## 1. Executive Summary

This report classifies the inherited pandaPI parser, grounder, and engine
surfaces for the 0.3.0 managed-process contract. It consumes Arc01 evidence
from `audit-synthesis-pandapi.md` and Arc02 evidence from
`combined-library-recommendations.md`.

The supported 0.3.0 surface is intentionally narrow:

- parser normal HDDL parse path;
- grounder normal parser-output `.htn` to planner-output path;
- engine normal search path.

Everything else is retained only as legacy behavior, marked experimental,
rejected from the 0.3.0 contract, or deferred as future work. This prevents
dependency availability from deciding product semantics before Arc03 defines
status, stream, CLI, and contract-test behavior.

## 2. Evidence Inputs

Primary Arc01 input:

- `docs/design-v0.3.0/arc01-vendored-source-audit/audit-synthesis-pandapi.md`
  section 4, which names the initial supported-surface candidates and
  non-inputs.
- Parser findings `P-001` through `P-015`, grounder findings `G-001` through
  `G-019`, and engine findings `E-001` through `E-016` as summarized in the
  Arc01 synthesis.

Primary Arc02 input:

- `docs/design-v0.3.0/arc02-cpp-library-research/combined-library-recommendations.md`,
  especially the final recommendation matrix and the optional SAT, BDD, CUDD,
  H2, and cpddl decisions.

The Arc01/Arc02 combined rule is: support is a product contract decision first.
Libraries such as reproc++, nlohmann/json, CLI11, fmt, parser generators, and
CUDD can implement or improve a chosen contract, but they do not decide what
the product supports.

## 3. Classification Vocabulary

This report uses the slice vocabulary exactly:

- `supported`: part of the 0.3.0 managed-process contract.
- `legacy`: retained for compatibility or historical CLI behavior but not part
  of the supervised-process contract unless explicitly invoked.
- `experimental`: visible only behind explicit flags or docs warnings; no
  stability promise.
- `unsupported`: rejected from the 0.3.0 contract.
- `future`: potentially valuable, but deferred to a later explicit slice or
  project.

## 4. Surface Classification Matrix

| Component | Surface | Disposition | 0.3.0 managed-process consequence |
|-----------|---------|-------------|-----------------------------------|
| Parser | parser normal HDDL parse path | `supported` | Primary parser contract surface. |
| Parser | parser verifier/output helper paths | `legacy` | Retained only as inherited explicit behavior, not a supervised contract. |
| Grounder | grounder normal `.htn` to planner-output path | `supported` | Primary grounder contract surface. |
| Grounder | grounder H2 path | `experimental` | Explicit opt-in path only; not default supervised behavior. |
| Grounder | grounder `cpddl`/FAM integration | `legacy` | Retained as inherited advanced behavior; dependency internals are not support commitments. |
| Engine | engine normal search path | `supported` | Primary engine contract surface. |
| Engine | engine interactive mode | `legacy` | TTY-only inherited mode; not a managed-process surface. |
| Engine | engine translation mode | `experimental` | Explicit opt-in path only; no stability promise in 0.3.0. |
| Engine | engine SAT path | `unsupported` | Rejected from the default 0.3.0 managed-process contract. |
| Engine | engine BDD/CUDD path | `future` | Deferred to later optional-capability and dependency-audit work. |

## 5. Per-Surface Classification

### 5.1 Parser Normal HDDL Parse Path

Disposition: `supported`.

Rationale: Arc01 already names the parser normal HDDL parse path as a
"Supported 0.3.0 path". Findings `P-001` through `P-008` affect that primary
path, but they are contract-repair inputs rather than blockers to accepting the
surface. The path is load-bearing for wolong compatibility and for the
parser-to-grounder pipeline.

Audit evidence: `P-001` release-disabled validation asserts; `P-002` raw
`FILE*` include handling and cwd-relative include behavior; `P-004` deep
parser exits; `P-005` argument validation that reports an error but continues;
`P-006` stdout contamination; `P-007` ANSI color escaping despite
`--no-colour`; `P-008` inherited upstream version identity.

Behavior expectation: For 0.3.0, normal HDDL parse must be invokable both by a
human CLI user and by a supervising process. The exact exit/status taxonomy,
stdout/stderr policy, color policy, help/version strings, and fixture oracles
are assigned to later Arc03 slices, but this surface is in scope for all of
them.

Managed-process effect: Supervisors may rely on the normal parser path as a
supported process boundary once later Arc03 slices define status, stream, CLI,
and tests. Deep exits and stdout mixing are defects to be converted into the
contract, not reasons to hide the surface.

Re-entry: No re-entry condition; this surface is accepted now. Defect repair
enters later slices through `P-001` through `P-008` and test coverage through
`P-010`.

Later Arc03 inputs: slice02 must classify parser parse/open/validation/internal
statuses and exit codes; slice03 must separate parse output from diagnostics,
events, color, and TTY behavior; slice04 must settle `pandapi-parser`
naming/help/version/provenance; slice05 must include normal HDDL parse
contract tests.

### 5.2 Parser Verifier and Output Helper Paths

Disposition: `legacy`.

Rationale: Arc01 says parser verifier/output helper paths are optional or
supported only after Arc03 classification. `P-004` shows verifier/helper code
can terminate the process directly, and `P-009` says there is no reusable
parser API boundary yet. These are inherited helper surfaces, not the primary
0.3.0 supervised contract.

Audit evidence: `P-004` verifier/helper exits; `P-009` missing parser API
boundary; `P-014` shared diagnostic/color/output-file/status helper
candidates.

Behavior expectation: Existing explicit helper or verifier behavior may remain
available for compatibility, but 0.3.0 should not document it as a managed
process contract. If a user invokes it, behavior is legacy until a later slice
accepts and tests it.

Managed-process effect: Supervisors should not depend on parser verifier or
output helper modes for deterministic status, stdout, stderr, or event
semantics in 0.3.0. Later status work should provide a clear legacy or
unsupported-feature result if supervised invocation attempts to enter a
non-contract helper mode.

Re-entry: Re-enter only with a dedicated parser helper/verifier slice that
defines the API boundary, removes deep exits, names statuses, documents output
shape, and adds fixture tests.

Later Arc03 inputs: slice02 needs a status for legacy or unsupported helper
invocation; slice03 must avoid accidentally granting stdout/event semantics to
helper output; slice04 should label these paths honestly in help text if still
visible; slice05 may add negative or compatibility probes.

### 5.3 Grounder Normal `.htn` to Planner-Output Path

Disposition: `supported`.

Rationale: Arc01 names the grounder normal parser-output `.htn` to planner
output path as a "Supported 0.3.0 path". Findings `G-002`, `G-004`, `G-005`,
`G-007`, and `G-014` are primary-path defects that must feed Arc03 and Arc05,
not reasons to reject the surface.

Audit evidence: `G-002` trusted counts and indexes from `.htn` input; `G-004`
deep helper exits; `G-005` missing arguments treated as stdin/stdout execution;
`G-006` output streams opened after parse failure; `G-007` stdout mixing;
`G-014` missing primary grounder test/fixture posture.

Behavior expectation: For 0.3.0, the grounder normal path supports
parser-generated `.htn` input and emits planner-consumable output through the
contracted file or stream destination. Exact status, output, diagnostics,
progress, and test details are later Arc03 obligations.

Managed-process effect: Supervisors may rely on this as the supported grounder
boundary after later Arc03 slices define the contract. It must not inherit
ambiguous stdin/stdout defaults, progress leakage, or deep process exits.

Re-entry: No re-entry condition; this surface is accepted now. Its defects
must feed later status, stream, and test slices.

Later Arc03 inputs: slice02 must classify malformed `.htn`, open/write,
validation, and internal statuses; slice03 must separate planner output from
diagnostics/progress/events; slice04 must settle naming/help/version; slice05
must include normal grounder fixture tests driven by parser output.

### 5.4 Grounder H2 Path

Disposition: `experimental`.

Rationale: Arc01 calls the grounder H2 path optional/experimental until
classified. H2 has H2-specific bugs and process-boundary risks: `G-001`
identifies a string-literal pointer arithmetic bug, `G-008` finds quiet mode
mutating global stream state, and `G-013` finds H2/cpddl dependency internals
compiled or copied into the grounder. Arc02 says H2 support must not be decided
by reproc++ or adapter availability.

Audit evidence: `G-001`, `G-008`, `G-013`, and `G-019`; Arc02 "H2/cpddl"
decision and `reproc++` conditional pilot guidance.

Behavior expectation: H2 remains explicit opt-in and experimental for 0.3.0.
It should not be part of the default supervised contract. If the path remains
visible, docs and help should avoid implying stability.

Managed-process effect: Supervisors should not use H2 as a default path. Later
slices must decide whether a supervised H2 invocation returns an
experimental/unsupported status or runs best-effort behind an explicit opt-in
contract. Quiet-mode stream mutation must not leak into supported output.

Re-entry: Re-enter as supported only after fixing the H2-specific correctness
issue, defining the dependency boundary, choosing in-process versus child
process containment, isolating streams, and adding H2 fixtures. reproc++ is
eligible only after this support decision, not before it.

Later Arc03 inputs: slice02 needs status/exit behavior for experimental H2,
dependency failure, and internal H2 errors; slice03 must protect stdout,
events, quiet mode, and TTY behavior from H2 stream mutation; slice04 should
mark any H2 CLI option as experimental; slice05 should add opt-in and
non-default H2 probes if the option remains visible.

### 5.5 Grounder `cpddl`/FAM Integration

Disposition: `legacy`.

Rationale: Arc01 says grounder `cpddl`/FAM integration is supported only to
the extent current default behavior requires it and that dependency-internal
audit remains separate. The visible FAM/invariant behavior is inherited
advanced behavior, and `G-013` makes the boundary risk explicit. Arc02 holds
H2/cpddl dependency-internal work behind this supported-surface decision.

Audit evidence: `G-013` H2/cpddl dependency internals; Arc01 section 4
distinguishing grounder-owned integration from dependency-internal findings;
Arc02 "H2/cpddl" hold decision.

Behavior expectation: The default supported grounder path must not promise
FAM-specific semantics beyond what the normal grounder path already requires.
If explicit `cpddl`/FAM behavior remains user-visible, it is a legacy advanced
path, not a 0.3.0 managed-process guarantee.

Managed-process effect: Supervisors should treat FAM-specific behavior as
outside the default contract. Build or link availability of cpddl internals is
an implementation constraint and cannot by itself create a supported product
surface.

Re-entry: Re-enter only after a cpddl/FAM boundary audit names which behavior
is product-owned, which code is dependency-internal, what license/build/security
work is required, and what fixtures prove deterministic status and output.

Later Arc03 inputs: slice02 needs a status distinction for legacy/advanced
grounder features and dependency-boundary failures; slice03 must avoid stream
leakage from dependency internals; slice04 must label FAM/cpddl-facing CLI
options carefully; slice05 should include default-path tests that prove FAM is
not silently required unless a later decision makes it supported.

### 5.6 Engine Normal Search Path

Disposition: `supported`.

Rationale: Arc01 names engine normal search as a "Supported 0.3.0 path".
Findings `E-003`, `E-004`, `E-006`, `E-008`, `E-010`, and `E-012` affect
primary command behavior and must feed the contract.

Audit evidence: `E-003` incomplete exit taxonomy; `E-004` stdout mixing of
diagnostics, progress, statistics, status, and plan data; `E-006` inherited
upstream version identity; `E-008` timeout/signal behavior split between
internal polling and wrapper behavior; `E-010` missing local test target or
fixture oracle; `E-012` raw-pointer-heavy model/search/heuristic ownership.

Behavior expectation: For 0.3.0, normal engine search is the supported planner
execution path for CLI users and managed-process supervisors. Plan output,
unsolvable outcomes, validation errors, timeouts, and internal errors must be
classified by later contract slices.

Managed-process effect: Supervisors may rely on normal engine search as a
supported process boundary after Arc03 defines status, signal, stream, CLI,
and tests. The contract must fence off interactive, translation, SAT, and
BDD/CUDD behavior so they cannot silently alter normal search semantics.

Re-entry: No re-entry condition; this surface is accepted now. Primary defects
move into later Arc03/Arc05 repair work.

Later Arc03 inputs: slice02 must define search outcome, unsolvable, malformed
input, timeout, signal, usage, and internal statuses; slice03 must separate
plan data from diagnostics/progress/statistics/events; slice04 must settle
`pandapi-engine` naming/help/version/provenance; slice05 must include normal
search, unsolvable, timeout, and malformed-input contract tests.

### 5.7 Engine Interactive Mode

Disposition: `legacy`.

Rationale: Arc01 says engine interactive mode is legacy or hidden unless Arc03
accepts it. `E-007` finds interactive mode reachable from the public CLI. A
TTY-driven interactive shell is incompatible with the first 0.3.0
managed-process contract unless it receives its own TTY contract, tests, and
docs.

Audit evidence: `E-007`; Arc02 interactive-mode guidance says no library
adoption and requires Arc03 classification before managed-process behavior is
defined around it.

Behavior expectation: Interactive mode may remain available only as explicit
human CLI legacy behavior. It should not be the default and should not be
advertised as a supervised-process interface.

Managed-process effect: Supervisors should not invoke interactive mode. Later
slices should either reject it under managed invocation or require explicit TTY
preconditions and legacy labeling.

Re-entry: Re-enter only with a TTY-specific slice that defines prompts,
buffering, cancellation, status-on-exit, no-stdin behavior, docs, and fixture
or pseudo-terminal tests.

Later Arc03 inputs: slice02 needs status for interactive invocation without a
TTY or under managed mode; slice03 owns TTY, buffering, and stdin/stdout
policy; slice04 must mark the CLI surface as legacy or hidden; slice05 should
include no-TTY negative probes if the option remains visible.

### 5.8 Engine Translation Mode

Disposition: `experimental`.

Rationale: Arc01 says engine translation mode is optional/legacy unless
explicitly accepted. `E-015` finds shell-out behavior with fixed filenames and
weak cleanup. Arc02 says reproc++ or any process library must not be adopted
solely to preserve translation mode before Arc03 classifies it.

Audit evidence: `E-015`; Arc02 `reproc++` conditional pilot guidance and
translation-mode hold decision.

Behavior expectation: Translation mode may remain visible only as an explicit
experimental path with no 0.3.0 stability promise. It is not part of the
default engine managed-process contract.

Managed-process effect: Supervisors should not depend on translation mode. If
kept callable, later slices must define whether managed invocation rejects it
or runs it behind explicit experimental status, isolated temp files, cleanup,
and child-process containment.

Re-entry: Re-enter only after defining the translation product value, replacing
fixed filenames with managed temp/output paths, guaranteeing cleanup, defining
child-process behavior, and adding contract tests. reproc++ can be reconsidered
only after this path is accepted.

Later Arc03 inputs: slice02 needs translation-specific unsupported or
experimental statuses and child-process failure mapping; slice03 needs output,
event, and stderr behavior for subprocess diagnostics; slice04 must mark
translation naming/help/version honestly; slice05 should include disabled,
negative, or opt-in translation probes.

### 5.9 Engine SAT Path

Disposition: `unsupported`.

Rationale: Arc01 initially called SAT experimental unless separately planned,
but `E-009` identifies process-level signals, threads, and direct exits in
optional SAT behavior. Arc02 says not to add a threading, signal, or SAT
support library for 0.3.0 from the current evidence. The default 0.3.0
managed-process contract should reject this path rather than imply partial
support.

Audit evidence: `E-009`; Arc02 SAT hold/reject-default decision.

Behavior expectation: SAT behavior is not supported in 0.3.0. If a custom
build exposes it, that build is outside the default managed-process contract
unless a later project changes the classification.

Managed-process effect: Supervisors must not rely on SAT mode. Later CLI and
status work should avoid presenting SAT as accepted default behavior and should
map invocation to unsupported behavior if the option is visible in the shipped
binary.

Re-entry: Re-enter only through a dedicated SAT capability slice that removes
process-global signal hazards, defines thread/cancellation/status behavior,
audits any SAT dependencies, and adds representative fixtures.

Later Arc03 inputs: slice02 needs an unsupported-feature status if SAT
invocation is visible; slice03 should not design event/stdout semantics around
SAT; slice04 should hide or label SAT options; slice05 should include negative
tests proving unsupported SAT behavior is deterministic if the option remains
reachable.

### 5.10 Engine BDD/CUDD Path

Disposition: `future`.

Rationale: Arc01 says BDD/CUDD is an experimental optional dependency out of
the default 0.3.0 contract. Arc02 says not to enable, upgrade, or replace CUDD
in the default engine build and requires a dedicated optional-dependency
license/build/security audit before support expansion. This is potentially
valuable, but not a 0.3.0 default contract item.

Audit evidence: `E-016`; Arc02 BDD/CUDD hold decision and final matrix
classification of SAT/BDD/CUDD as hold/reject default.

Behavior expectation: BDD/CUDD behavior is not part of the default 0.3.0
managed-process contract. No help text, release notes, or supervised interface
should imply support unless a later optional-capability project accepts it.

Managed-process effect: Supervisors should not see BDD/CUDD as a supported
surface. If source or build flags remain in tree, they are dormant future work
unless a later slice explicitly activates and tests them.

Re-entry: Re-enter through a dedicated BDD/CUDD optional-dependency project
that audits license, build, security, maintained source lineage, runtime
behavior, status taxonomy, and contract fixtures.

Later Arc03 inputs: slice02 needs unsupported/future-feature handling only if a
visible option can enter BDD/CUDD; slice03 should not design event/stdout
semantics around it; slice04 should hide or label any visible option; slice05
should include negative contract tests only if the default binary exposes the
surface.

## 6. Dependency Availability Does Not Decide Support

Dependency availability is deliberately a non-input to support classification:

- `reproc++` can implement a child-process adapter only after H2 or translation
  is classified as supported. It cannot make either path supported.
- `nlohmann/json` is held until Arc03 chooses an event format. JSON library
  availability cannot make managed-process events supported or required.
- `CUDD` source availability cannot make BDD/CUDD supported; the BDD/CUDD path
  is `future` until a dedicated optional-dependency audit and contract project
  accepts it.
- H2 and `cpddl` being present in the vendored tree cannot make H2 or FAM
  supported. H2 is `experimental`; `cpddl`/FAM is `legacy` as a user-facing
  advanced path.
- Parser generator options are held because parser semantics and the parser
  process contract must be defined before generator migration.
- `CLI11` can implement accepted CLI naming/help/version behavior after
  slice04 and golden tests; it cannot decide which inherited modes are
  supported.
- `fmt` can implement diagnostics under a shared facade after slice03; it
  cannot decide what stdout/stderr/events mean.

This is the Arc03 ordering constraint in practical form: support surface first,
contract second, dependencies third.

## 7. Later Arc03 Slice Inputs

### slice02 - status, exit, and signal taxonomy

slice02 must include supported statuses for parser normal HDDL parse, grounder
normal `.htn` grounding, and engine normal search. It must also define
deterministic handling for legacy parser helpers, legacy grounder FAM/cpddl,
legacy engine interactive mode, experimental H2, experimental translation,
unsupported SAT, and future BDD/CUDD if any visible option can invoke them.
Signals, timeouts, child-process failures, and dependency-boundary failures
must not collapse into generic failure prose.

### slice03 - stdio, events, color, and TTY

slice03 must treat stdout as owned for the three supported paths. Parser parse
output, grounder planner output, and engine plan output must be separated from
diagnostics, progress, statistics, color, and any machine events. H2 stream
mutation, interactive TTY behavior, translation subprocess output, and
dependency-internal diagnostics must be fenced away from the supported default
contract.

### slice04 - CLI naming, version, and migration

slice04 must make the three supported binaries clear and consistent while
preserving wolong compatibility. Help/version/provenance text should identify
supported surfaces and avoid advertising legacy, experimental, unsupported, or
future behavior as stable. CLI11 remains an implementation candidate only
after the accepted surface and golden compatibility expectations are named.

### slice05 - contract test matrix

slice05 must build the contract-test matrix around the three supported
surfaces, then add explicit negative or opt-in probes for legacy,
experimental, unsupported, and future paths that remain reachable in the
default binaries. Tests should prove that optional surfaces cannot silently
change stdout, stderr, status, signal, TTY, or version behavior for supported
normal paths.

## 8. Final Recommendation

Arc03 should proceed with a narrow 0.3.0 managed-process contract:

- supported: parser normal HDDL parse, grounder normal `.htn` grounding, engine
  normal search;
- legacy: parser verifier/output helper paths, grounder `cpddl`/FAM
  integration, engine interactive mode;
- experimental: grounder H2, engine translation;
- unsupported: engine SAT;
- future: engine BDD/CUDD.

This classification gives the next Arc03 slices a stable product boundary.
It also keeps optional capability work from leaking into 0.3.0 through library
availability, build flags, or inherited upstream command surfaces.
