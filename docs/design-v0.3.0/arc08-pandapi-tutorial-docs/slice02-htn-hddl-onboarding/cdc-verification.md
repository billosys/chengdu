# CDC Verification: Arc08 Slice02 htn-hddl-onboarding

Status: closed and CDC-verified.
Verdict: PASS after path correction.
Date: 2026-08-13.
Reviewed commit: `841d5e7a` (`docs: add Arc08 HTN onboarding`).
Correction: this CDC verification change moves the public page to the accepted
`docs/tutorial/` home and updates the dependent evidence.

## Scope Reviewed

CDC reviewed the Slice02 public onboarding page, Slice02 ledger, closing
report, Arc08 plan, project plan, and the operator correction that public
project documentation belongs under `docs/` with tutorial pages under
`docs/tutorial/`.

The delivered page was moved from the initial
`docs/pandapi-0.3.0/tutorial/htn-hddl-onboarding.md` location to:

- `docs/tutorial/htn-hddl-onboarding.md`

The correction preserves the public page content and updates the Arc08
blueprint, Slice02 evidence commands, Arc08 plan, and project plan so future
docs work uses the accepted docs home.

## Ledger Verification

CDC reproduced the Slice02 F-1 through F-16 ledger checks against the corrected
path:

- F-1 through F-7: the page exists under `docs/tutorial/`, introduces the
  beginner tutorial, explains HTN/HDDL/PDDL concepts, walks
  `fixtures/minimal`, explains the pipeline, describes solved/no-plan
  outcomes, and records that the minimal fixture is enough.
- F-8: the documented canonical parser command was verified locally. It built
  `pandapi-parser`, produced a non-empty `.htn` artifact from
  `fixtures/minimal/domain.hddl` and `fixtures/minimal/problem.hddl`, and left
  the generated artifact outside the repository.
- F-9: `make test-contract-parser-managed` passed.
- F-10: the public page uses canonical `pandapi-*` names and does not mention
  inherited command names as supported compatibility surfaces.
- F-11: the public page contains no Arc08/CC/CDC/ledger planning vocabulary.
- F-12 and F-13: Arc08 and project plans record the Slice02 evidence and keep
  Arc09 release-publication scope separate.
- F-14: protected-surface boundary check passed for README, release, `.github`,
  Make, tests, tools, pandaPI source, and licenses.
- F-15: `git diff --check` and `git diff --cached --check` passed.
- F-16: the closing report walks F-1 through F-16 and bubbles up to Arc08.

Aggregate `make check` was not run. This correction changes documentation
paths and planning evidence only; the reproduced ledger checks, parser command,
managed parser contract gate, boundary check, and whitespace checks are the
appropriate evidence for the changed surface.

## Findings

Slice02 delivers Arc08 A2 after the path correction. The onboarding page
teaches HTN/PDDL/HDDL concepts, the domain/problem split, the
`fixtures/minimal` example, the `pandapi-parser` -> `pandapi-grounder` ->
`pandapi-engine` artifact pipeline, and beginner solved/no-plan outcomes.

The minimal fixture is enough for beginner onboarding. No new fixture is
needed before Slice03.

The only correction required was path ownership: public project docs are rooted
at `docs/`, tutorials are rooted at `docs/tutorial/`, and
`docs/design-v0.3.0/` remains planning/design/implementation-tracking evidence.

## Bubble-up to Arc08

Slice02 closes A2. Arc08 remains active, and Slice03
`first-project-workflow` should use:

- `docs/tutorial/first-project-workflow.md`

Slice03 should continue from the same minimal fixture for the happy-path
workflow, then introduce `fixtures/unsolvable` only after the parse -> ground
-> solve path is clear.
