# Baseline Fixture Records

These records document current inherited behavior for the executable baseline
gate. They are intentionally not final managed-process contract conformance
records.

```yaml
- id: parser-success-minimal
  mode: baseline
  owner: Arc05 Slice02
  component: parser
  surface_disposition: supported
  command:
    argv: ["pandaPIparser", "-C", "$FixtureRoot/minimal/domain.hddl", "$FixtureRoot/minimal/problem.hddl", "$TempRoot/minimal.htn"]
  stdin:
    mode: empty
  stdout:
    role: informational
  stderr:
    role: human_diagnostics
  artifact:
    path: "$TempRoot/minimal.htn"
    state: complete
  exit:
    status: ok
    code: 0
  final_status:
    observation: absent
    future_contract: required when status output is enabled by the owning parser slice
  normalization:
    line_endings: lf
    ansi: strip_for_baseline_diagnostics
    paths: temp_and_fixture_tokens
  safety:
    cleanup: remove_temp_artifacts
    stdin_full_duplex: not_used

- id: parser-missing-file
  mode: baseline
  owner: Arc05 Slice02
  component: parser
  surface_disposition: supported
  command:
    argv: ["pandaPIparser", "-C", "$TempRoot/missing-domain.hddl", "$FixtureRoot/minimal/problem.hddl", "$TempRoot/missing.htn"]
  stdin:
    mode: empty
  stdout:
    role: human_diagnostics
  stderr:
    role: human_diagnostics
  artifact:
    path: "$TempRoot/missing.htn"
    state: absent
  exit:
    status: input_unavailable_baseline
    code: 2
  final_status:
    observation: absent
  normalization:
    ansi: strip_for_baseline_diagnostics
  safety:
    cleanup: remove_temp_artifacts
    stdin_full_duplex: not_used

- id: parser-broken-syntax
  mode: baseline
  owner: Arc05 Slice02
  component: parser
  surface_disposition: supported
  command:
    argv: ["pandaPIparser", "-C", "$FixtureRoot/broken-syntax/domain.hddl", "$FixtureRoot/broken-syntax/problem.hddl", "$TempRoot/broken-syntax.htn"]
  stdin:
    mode: empty
  stdout:
    role: empty
  stderr:
    role: human_diagnostics
  artifact:
    path: "$TempRoot/broken-syntax.htn"
    state: absent
  exit:
    status: input_invalid_baseline
    code: 255
  final_status:
    observation: absent
  normalization:
    ansi: strip_for_baseline_diagnostics
  safety:
    cleanup: remove_temp_artifacts
    stdin_full_duplex: not_used

- id: parser-broken-reference
  mode: baseline
  owner: Arc05 Slice02
  component: parser
  surface_disposition: supported
  command:
    argv: ["pandaPIparser", "-C", "$FixtureRoot/broken-reference/domain.hddl", "$FixtureRoot/broken-reference/problem.hddl", "$TempRoot/broken-reference.htn"]
  stdin:
    mode: empty
  stdout:
    role: empty
  stderr:
    role: human_diagnostics
  artifact:
    path: "$TempRoot/broken-reference.htn"
    state: retained_partial
  exit:
    status: input_invalid_baseline_distinct_semantic_error
    code: 255
  final_status:
    observation: absent
  normalization:
    ansi: strip_for_baseline_diagnostics
  safety:
    cleanup: remove_temp_artifacts
    stdin_full_duplex: not_used

- id: grounder-success-minimal
  mode: baseline
  owner: Arc05 Slice02
  component: grounder
  surface_disposition: supported
  command:
    argv: ["pandaPIgrounder", "$TempRoot/minimal.htn", "$TempRoot/minimal.sas"]
  stdin:
    mode: empty
  stdout:
    role: progress
  stderr:
    role: human_diagnostics
  artifact:
    path: "$TempRoot/minimal.sas"
    state: complete
  exit:
    status: ok
    code: 0
  final_status:
    observation: absent
  normalization:
    paths: temp_tokens
  safety:
    cleanup: remove_temp_artifacts
    stdin_full_duplex: not_used

- id: grounder-missing-input
  mode: baseline
  owner: Arc05 Slice02
  component: grounder
  surface_disposition: supported
  command:
    argv: ["pandaPIgrounder", "$TempRoot/missing.htn", "$TempRoot/missing.sas"]
  stdin:
    mode: empty
  stdout:
    role: progress
  stderr:
    role: human_diagnostics
  artifact:
    path: "$TempRoot/missing.sas"
    state: absent
  exit:
    status: input_unavailable_baseline
    code: 1
  final_status:
    observation: absent
  normalization:
    paths: temp_tokens
  safety:
    cleanup: remove_temp_artifacts
    stdin_full_duplex: not_used

- id: engine-solved-minimal
  mode: baseline
  owner: Arc05 Slice02
  component: engine
  surface_disposition: supported
  command:
    argv: ["pandaPIengine", "$TempRoot/minimal.sas"]
  stdin:
    mode: empty
  stdout:
    role: data_artifact
  stderr:
    role: empty
  artifact:
    path: "$TempRoot/engine-solved.raw"
    state: complete
  exit:
    status: ok
    code: 0
  final_status:
    observation: absent
  normalization:
    duration: predicate_only
  safety:
    cleanup: remove_temp_artifacts
    harness_timeout: ci_safe
    stdin_full_duplex: not_used

- id: engine-unsolvable
  mode: baseline
  owner: Arc05 Slice02
  component: engine
  surface_disposition: supported
  command:
    argv: ["pandaPIengine", "$TempRoot/unsolvable.sas"]
  stdin:
    mode: empty
  stdout:
    role: data_artifact
  stderr:
    role: empty
  artifact:
    path: "$TempRoot/engine-unsolvable.raw"
    state: complete
  exit:
    status: domain_no_plan_baseline
    code: 2
  final_status:
    observation: absent
  normalization:
    duration: predicate_only
  safety:
    cleanup: remove_temp_artifacts
    harness_timeout: ci_safe
    stdin_full_duplex: not_used

- id: pipeline-minimal
  mode: baseline
  owner: Arc05 Slice02
  component: pipeline
  surface_disposition: supported
  command:
    argv:
      - ["pandaPIparser", "-C", "$FixtureRoot/minimal/domain.hddl", "$FixtureRoot/minimal/problem.hddl", "$TempRoot/pipeline.htn"]
      - ["pandaPIgrounder", "$TempRoot/pipeline.htn", "$TempRoot/pipeline.sas"]
      - ["pandaPIengine", "$TempRoot/pipeline.sas"]
  stdin:
    mode: empty
  stdout:
    role: data_artifact
  stderr:
    role: human_diagnostics_per_step
  artifact:
    path: "$TempRoot/pipeline.raw"
    state: complete
  exit:
    status: ok
    code: 0
  final_status:
    observation: absent
  normalization:
    paths: fixture_and_temp_tokens
  safety:
    cleanup: remove_temp_artifacts
    harness_timeout: ci_safe
    stdin_full_duplex: not_used

- id: future-parser-contract-target
  mode: contract-target
  owner: Arc05 Slice03
  component: parser
  surface_disposition: supported
  command:
    argv: ["pandapi-parser", "--status=stderr", "--output", "$TempRoot/out.htn", "$FixtureRoot/minimal/domain.hddl", "$FixtureRoot/minimal/problem.hddl"]
  stdin:
    mode: empty
  stdout:
    role: empty_or_data_artifact_when_output_dash
  stderr:
    role: human_diagnostics_and_tagged_status
  artifact:
    path: "$TempRoot/out.htn"
    state: complete
  exit:
    status: ok
    code: 0
  final_status:
    observation: required
    fields: [status, component, surface, surface_disposition, exit_code, class]
  normalization:
    ansi: forbidden_in_machine_streams
  safety:
    cleanup: remove_temp_artifacts
    stdin_full_duplex: not_used
```
