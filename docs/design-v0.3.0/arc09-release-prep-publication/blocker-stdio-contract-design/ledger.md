# Arc09 Blocker Ledger: stdio-contract-design

| ID | Criterion | Verify | Significance | Origin | Status | Evidence | Notes |
|----|-----------|--------|--------------|--------|--------|----------|-------|
| F-1 | The blocker slice is explicitly marked as blocking Slice01 release-readiness-inventory. | `rg -n -- "blocks Slice01 release-readiness-inventory" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design/slice-doc.md docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | wolong blocker | closed | Passed 2026-08-20; `slice-doc.md` status and `arc-plan.md` current-status text retain the blocking phrase. | |
| F-2 | The design records parser stdin semantics for domain-from-stdin, problem-from-stdin, and both-input forms. | `rg -n -- "domain from stdin|problem from stdin|both.*stdin|framing|rejected" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | correctness-grade | wolong parser need | closed | Passed 2026-08-20; `stdin-contract-design.md` accepts domain-from-stdin and problem-from-stdin, rejects parser `- -`, and records the missing 0.3.0 framing decision. | |
| F-3 | The design records grounder stdin semantics for parser-generated `.htn` artifacts. | `rg -n -- "grounder.*stdin|\\.htn|parser-generated" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | correctness-grade | wolong grounder need | closed | Passed 2026-08-20; `stdin-contract-design.md` accepts one complete parser-generated `.htn` artifact on grounder stdin. | |
| F-4 | The design records engine stdin semantics for grounder-generated `.sas` artifacts and no-plan classification preservation. | `rg -n -- "engine.*stdin|\\.sas|domain_no_plan|no_plan" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | correctness-grade | wolong engine need | closed | Passed 2026-08-20; `stdin-contract-design.md` accepts one complete grounder-generated `.sas` artifact on engine stdin and preserves `domain_no_plan` / `no_plan`. | |
| F-5 | The design preserves stdout and stderr ownership rules for `--output -` plus `--status=stderr`. | `rg -n -- "--output -|--status=stderr|stdout ownership|stderr|PANDAPI_STATUS" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | managed-process contract | closed | Passed 2026-08-20; `stdin-contract-design.md` preserves stdout artifact ownership, stderr status ownership, `PANDAPI_STATUS`, and the `--status=stdout --output -` conflict. | |
| F-6 | The design chooses shared runtime/helper ownership for repeated stdin materialization or gives a concrete no-share rationale. | `rg -n -- "shared runtime|stdin materialization|helper|reuse|no-share rationale" docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design docs/design-v0.3.0/arc09-release-prep-publication/arc-plan.md` | serious | good engineering | closed | Passed 2026-08-20; `stdin-contract-design.md` chooses shared runtime/helper ownership for stdin materialization, path-role behavior, cleanup, and common status mapping. | |
| F-7 | The slice stays out of product code, public docs, packaging, publication, and wolong workspace changes. | `set -e; if git diff --cached --name-only -- pandaPI README.md docs/index.md docs/tutorial docs/managed-process.md docs/reference docs/migration.md docs/architecture.md Makefile mk tools .github/workflows release | rg .; then exit 1; fi` | serious | boundary | closed | Passed 2026-08-20 after staging; staged files are limited to Arc09 design/planning/ledger files and the project-plan status update. | Wolong is a separate workspace; this chengdu slice did not edit it. |
| F-8 | No CDC verification file is created by CC. | `test ! -f docs/design-v0.3.0/arc09-release-prep-publication/blocker-stdio-contract-design/cdc-verification.md` | serious | independence | closed | Passed 2026-08-20; no `cdc-verification.md` exists for this blocker. | |
| F-9 | Whitespace checks pass after staging. | `git diff --check && git diff --cached --check` | serious | no regression | closed | Passed 2026-08-20 after staging. | |

## What Worked

Arc09 now has a durable design-only stdin contract decision for the
release-blocking wolong pipeline. The design explicitly accepts parser
single-role stdin, grounder `.htn` stdin, and engine `.sas` stdin; rejects
parser both-input stdin without a framing design; preserves stdout/stderr
ownership; and hands repeated stdin materialization/path-role behavior to
shared runtime/helper implementation.

## Closure

Closed by ledger evidence only. No product code, public docs, packaging,
publication, wolong workspace edits, or CC-owned `cdc-verification.md` were
created.
