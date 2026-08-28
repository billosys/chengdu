# Arc09 Wolong Supervision Proof

Date: 2026-08-25

Status: proof slice closed; external Wolong runner/API blocker remains.

## Summary

Chengdu's accepted stdin/stdout/stderr contract is now implemented and
fixture-proven for the supported `pandapi-*` forms. Direct Chengdu probes also
match the accepted contract: parser accepts exactly one stdin role, grounder
accepts one `.htn` artifact on stdin, engine accepts one `.sas` artifact on
stdin, solved and no-plan pipelines can pass artifacts over stdout/stdin, and
final `PANDAPI_STATUS` remains on stderr.

The Wolong substrate can supervise a Chengdu binary through erlexec argv-list
execution with explicit stdin, stdout, stderr, EOF, and monitor handling. That
raw proof passes against `pandapi-grounder`.

Wolong's current public runner API does not yet expose stdin artifact bytes:
`wolong-exec:run/3` builds an argv list and captures stdout/stderr, but it does
not enable erlexec `stdin`, send bytes, or send EOF. Therefore Arc09 Slice01
`release-readiness-inventory` remains blocked on a Wolong-owned external
blocker: Arc03 `slice02-stdio-runner` must add a runner/API contract for
stdin bytes and separated stdout/stderr capture before Chengdu release
readiness can resume.

## Original Claim Disposition

Source claim set: `chengdu-stdin-contract-blocker.md` in the Wolong workspace,
also summarized by Chengdu's `wolong-stdin-contract-audit.md`.

| Claim | Disposition | Evidence |
|-------|-------------|----------|
| `pandapi-parser` rejects domain-from-stdin with `cli_usage_error`. | fixed | Direct Chengdu probe passed: `pandapi-parser --supervised --status=stderr --output "$tmp/domain-from-stdin.htn" - fixtures/minimal/problem.hddl < fixtures/minimal/domain.hddl` produced a non-empty `.htn` artifact and `PANDAPI_STATUS status=ok component=parser`. |
| `pandapi-parser` rejects problem-from-stdin with `cli_usage_error`. | fixed | Direct Chengdu probe passed: `pandapi-parser --supervised --status=stderr --output "$tmp/problem-from-stdin.htn" fixtures/minimal/domain.hddl - < fixtures/minimal/problem.hddl` produced a non-empty `.htn` artifact and `PANDAPI_STATUS status=ok component=parser`. |
| Parser two-input stdin semantics were undefined. | accepted-as-rejected | The accepted Chengdu contract rejects `pandapi-parser - -` because no `v0.3.0` framing exists for two logical artifacts on one byte stream. Direct probe preserved exit `10` and `status=cli_usage_error`. |
| `pandapi-grounder` rejects `.htn` stdin with `cli_usage_error`. | fixed | Direct Chengdu probe passed: parser output was fed to `pandapi-grounder --supervised --status=stderr --output - -`, producing a non-empty `.sas` artifact on stdout and `PANDAPI_STATUS status=ok component=grounder` on stderr. |
| `pandapi-engine` rejects `.sas` stdin with `cli_usage_error`. | fixed | Direct Chengdu probe passed: grounder output was fed to `pandapi-engine --supervised --status=stderr --output - -`, producing solved plan bytes on stdout and `PANDAPI_STATUS status=ok component=engine outcome=solved` on stderr. |
| Engine no-plan stdin behavior was unavailable. | fixed | Direct Chengdu probe passed with exit `2`, `status=domain_no_plan`, `outcome=no_plan`, and status on stderr for an unsolvable `.sas` artifact read from stdin. |
| Full solved stdin/stdout/stderr supervised chain was unavailable. | fixed | Direct Chengdu probe passed: parser stdout fed grounder stdin; grounder stdout fed engine stdin; final engine plan stdout was non-empty and stderr contained `PANDAPI_STATUS status=ok outcome=solved`. |
| Full no-plan stdin/stdout/stderr supervised chain was unavailable. | fixed | Direct Chengdu probe passed: parser stdout fed grounder stdin; grounder stdout fed engine stdin; final engine exit was `2` with stderr `PANDAPI_STATUS status=domain_no_plan outcome=no_plan`. |
| Wolong could not resume a stdin/stdout/stderr supervised pipeline. | still-blocked | Raw erlexec argv-list stdin proof passes, but `wolong-exec:run/3` has no stdin-bytes API or EOF path. Wolong Arc03 `slice02-stdio-runner` owns the re-entry implementation. |

## Chengdu Fixture Gate

Command:

```bash
make test-contract-stdio-managed
```

Result:

```text
SUMMARY: 187 passed, 0 failed (contract, stdio, macos-arm64)
```

This is the Make-backed fixture proof for the supported stdin contract. It
covers parser domain stdin, parser problem stdin, parser both-stdin rejection,
grounder `.htn` stdin to stdout `.sas`, engine `.sas` stdin solved, engine
`.sas` stdin no-plan, solved supervised stdio pipeline, and no-plan supervised
stdio pipeline behavior.

## Direct Chengdu Probe Evidence

Command family:

```bash
./bin/pandapi-parser --supervised --status=stderr --output "$tmp/domain-from-stdin.htn" - fixtures/minimal/problem.hddl < fixtures/minimal/domain.hddl
./bin/pandapi-parser --supervised --status=stderr --output "$tmp/problem-from-stdin.htn" fixtures/minimal/domain.hddl - < fixtures/minimal/problem.hddl
./bin/pandapi-parser --supervised --status=stderr --output "$tmp/both-stdin.htn" - - < fixtures/minimal/domain.hddl
./bin/pandapi-grounder --supervised --status=stderr --output - - < fixtures/grounder/minimal.htn
./bin/pandapi-engine --supervised --status=stderr --output - - < fixtures/engine/minimal.sas
./bin/pandapi-engine --supervised --status=stderr --output - - < fixtures/engine/unsolvable.sas
```

Observed result:

```text
parser domain-from-stdin: fixed
parser problem-from-stdin: fixed
parser both-stdin: accepted-as-rejected
grounder htn stdin to stdout sas: fixed
engine sas stdin solved to stdout: fixed
engine sas stdin no-plan: fixed, exit 2, status=domain_no_plan, outcome=no_plan
full solved stdin/stdout/stderr chain: fixed
full no-plan stdin/stdout/stderr chain: fixed, exit 2, status=domain_no_plan, outcome=no_plan
```

These shell probes are diagnostic baseline evidence. They are not treated as
Wolong supervision proof by themselves.

## Wolong Erlexec Substrate Proof

Wolong raw erlexec proof command, run from `/Users/oubiwann/lab/billosys/wolong`:

```bash
erl -pa _build/test/lib/*/ebin -noshell -eval 'application:ensure_all_started(wolong), {ok, Pid, OsPid} = exec:run(["../chengdu/bin/pandapi-grounder", "--supervised", "--status=stderr", "--output", "-", "-"], [stdin, stdout, stderr, monitor]), {ok, Bytes} = file:read_file("../chengdu/fixtures/grounder/minimal.htn"), ok = exec:send(Pid, Bytes), ok = exec:send(Pid, eof), Collect = fun F(Out, Err) -> receive {stdout, OsPid, D} -> F(<<Out/binary, D/binary>>, Err); {stderr, OsPid, D} -> F(Out, <<Err/binary, D/binary>>); {DOWN, OsPid, process, Pid, Reason} -> io:format("erlexec argv-list stdin proof: reason=~p stdout_bytes=~p stderr=~s", [Reason, byte_size(Out), Err]), case {Reason, byte_size(Out) > 0, binary:match(Err, <<"PANDAPI_STATUS">>)} of {normal, true, {_, _}} -> halt(0); _ -> halt(1) end after 5000 -> io:format("timeout~n"), halt(1) end end, Collect(<<>>, <<>>).'
```

Result:

```text
erlexec argv-list stdin proof: reason=normal stdout_bytes=446 stderr=PANDAPI_STATUS	status=ok	component=grounder	surface=normal_grounding	surface_disposition=supported	exit_code=0	class=success	partial_output_policy=complete	artifact=stdout	path_role=htn	path=-	operation=read
```

This proves the lower-level erlexec substrate can run a supported Chengdu
binary with argv-list execution, stdin bytes, EOF, separated stdout/stderr, and
process monitoring.

## Wolong Runner/API Inspection

Files inspected:

- `/Users/oubiwann/lab/billosys/wolong/src/wolong-exec.lfe`
- `/Users/oubiwann/lab/billosys/wolong/test/wolong_exec_SUITE.lfe`
- `/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/arc-plan.md`
- `/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/slice01-stdio-contract-investigation/closing-report.md`
- `/Users/oubiwann/lab/billosys/wolong/docs/design-v0.1.0/arc03-stdio-pipeline/slice01-stdio-contract-investigation/cdc-verification.md`

Inspection result:

- `wolong-exec.lfe` exports `(run 3)`.
- `run-valid` builds `argv` as `(cons command args)`.
- The erlexec options include `monitor`, `stdout`, `stderr`, `kill_group`,
  `group`, and `kill_timeout`.
- The runner does not enable erlexec `stdin`.
- The runner does not call `exec:send/2`.
- The runner has no EOF path for child stdin.
- Existing Common Test coverage exercises stdout/stderr capture and process
  status, but not supervised stdin artifact delivery.

Wolong workspace status:

```text
## main...origin/main
```

No Wolong edits were made. No Wolong commit was created. The required follow-on
is a Wolong slice, not a Chengdu code change.

## Wolong Gate Evidence

Command, run from `/Users/oubiwann/lab/billosys/wolong`:

```bash
rebar3 compile && rebar3 as test eunit && rebar3 as test ct && rebar3 xref && rebar3 dialyzer && rebar3 lfe format --check
```

Result:

```text
EUnit: 9 tests, 0 failures
Common Test: All 62 tests passed.
xref passed
dialyzer success typing completed
All 13 file(s) are formatted
```

This keeps the Wolong workspace clean while documenting that current tests pass
without stdin runner/API coverage.

## External Blocker

Owner: Wolong.

Affected slice: Wolong Arc03 `slice02-stdio-runner`.

Blocker: Wolong must add a runner/API contract that can pass stdin artifact
bytes to a supervised child process through erlexec argv-list execution, send
EOF, collect separated stdout and stderr, preserve status/exit classification,
and expose that behavior through tests or fixtures.

Re-entry condition: Arc09 Slice01 `release-readiness-inventory` can resume only
after Wolong proves its public runner/API can execute at least the supported
Chengdu stdin/stdout/stderr chain without shell command strings or temporary
file workarounds. Raw erlexec argv-list proof alone is not enough because it
does not exercise `wolong-exec:run/3` or its eventual stdin API.

## Chengdu Regression Gates

Existing managed contract and smoke gate:

```bash
make test-contract-parser-managed && make test-contract-grounder-managed && make test-contract-engine-managed && make test-contract-pipeline-managed && make smoke && make smoke-negative
```

Result:

```text
parser contract: 298 passed, 0 failed
grounder contract: 269 passed, 0 failed
engine contract: 312 passed, 0 failed
pipeline contract: 129 passed, 0 failed
positive smoke: 3 passed, 0 failed
negative smoke: 4 passed, 0 failed
```

Release-quality wayfinding gate:

```bash
make safety-checks && make actionlint && make static-analysis && make format-check && make test && make provenance-check
```

Result:

```text
safety-checks passed
actionlint passed
static-analysis passed
format-check passed
make test passed
provenance-check passed
```

## Bubble-Up

The Chengdu stdin contract blockers are closed from Chengdu's side, and this
proof slice records the remaining consumer gap without hiding it in release
readiness.

Arc09 remains active and blocked before Slice01. The next work is not Arc09
`release-readiness-inventory`; it is Wolong Arc03 `slice02-stdio-runner` or an
equivalent Wolong-owned stdin runner/API implementation slice. After that
implementation is CDC-verified in Wolong, Arc09 can re-enter with a new proof
run and then resume Slice01.

## Silent-Drop Diff

Delivered:

- Re-read the Wolong blocker report and mapped each claim to `fixed`,
  `accepted-as-rejected`, or `still-blocked`.
- Proved Chengdu's accepted stdin contract through `make
  test-contract-stdio-managed`.
- Ran direct Chengdu probes for parser, grounder, engine, solved pipeline, and
  no-plan pipeline stdin behavior.
- Proved raw erlexec argv-list stdin handling against a real Chengdu binary.
- Inspected Wolong runner/API/test state.
- Recorded the Wolong-owned external blocker and re-entry condition.

Intentionally not delivered:

- No Chengdu product-code changes.
- No Wolong workspace edits.
- No Wolong commit.
- No release publication, tag, package output, release notes, or public
  published-version claims.
- No `cdc-verification.md`.
