#include "pandapi/runtime/fixture.hpp"
#include "pandapi/runtime/normalization.hpp"
#include "pandapi/runtime/process_fixture.hpp"
#include "pandapi/runtime/status_io.hpp"

#include <cassert>
#include <chrono>
#include <string>

namespace {

using pandapi::runtime::ArtifactState;
using pandapi::runtime::CleanupPolicy;
using pandapi::runtime::CommandSpec;
using pandapi::runtime::ComparisonPolicy;
using pandapi::runtime::Component;
using pandapi::runtime::EnvironmentVariable;
using pandapi::runtime::ExpectedExit;
using pandapi::runtime::FinalStatusObservation;
using pandapi::runtime::FixtureSpec;
using pandapi::runtime::NormalizationRules;
using pandapi::runtime::OutputArtifactExpectation;
using pandapi::runtime::PathReplacement;
using pandapi::runtime::PathToken;
using pandapi::runtime::ProbeKind;
using pandapi::runtime::ProbePolicy;
using pandapi::runtime::ProbeSafety;
using pandapi::runtime::RunOptions;
using pandapi::runtime::StatusCode;
using pandapi::runtime::StreamExpectation;
using pandapi::runtime::StreamRole;
using pandapi::runtime::SurfaceDisposition;

[[nodiscard]] FixtureSpec sample_fixture_record()
{
  FixtureSpec fixture;
  fixture.id = "fixture record normalization status matching comparison process "
               "observation timeout";
  fixture.owner = "Arc04";
  fixture.kind = "black-box-process";
  fixture.component = Component::Engine;
  fixture.surface_disposition = SurfaceDisposition::Supported;
  fixture.command.executable_path = "/bin/sh";
  fixture.command.argv = {"/bin/sh", "-c",
                          "printf fixture-stdout; printf fixture-stderr >&2"};
  fixture.command.environment.variables.push_back(
      EnvironmentVariable{"PANDAPI_TEST_ROOT", "/tmp/pandapi-fixture"});
  fixture.stdout_expectation = StreamExpectation{
      StreamRole::DataArtifact, ComparisonPolicy::Golden, "fixture-stdout", false};
  fixture.stderr_expectation = StreamExpectation{
      StreamRole::HumanDiagnostics, ComparisonPolicy::Semantic, "fixture-stderr", true};
  fixture.output_artifacts.push_back(
      OutputArtifactExpectation{"$TMPDIR/plan.txt", "plan", ArtifactState::Complete});
  fixture.expected_exit = ExpectedExit{StatusCode::Ok, 0};
  fixture.final_status.observation = FinalStatusObservation::Required;
  fixture.final_status.required_fields = {
      "status", "component", "surface", "surface_disposition", "exit_code", "class"};
  fixture.normalization.path_tokens.push_back(
      PathReplacement{PathToken::TempRoot, "/tmp"});
  fixture.safety.cleanup = CleanupPolicy::RemoveTempArtifacts;
  fixture.safety.probes.push_back(ProbePolicy{
      ProbeKind::HarnessTimeout, ProbeSafety::CiSafe, std::chrono::milliseconds{50}});
  fixture.safety.probes.push_back(ProbePolicy{
      ProbeKind::BinaryTimeout, ProbeSafety::Quarantine, std::chrono::milliseconds{0}});
  fixture.safety.probes.push_back(ProbePolicy{
      ProbeKind::ResourceLimit, ProbeSafety::Manual, std::chrono::milliseconds{0}});
  return fixture;
}

void fixture_records_are_typed()
{
  const auto fixture = sample_fixture_record();
  assert(fixture.id.find("fixture record") != std::string::npos);
  assert(pandapi::runtime::stream_role_name(fixture.stdout_expectation.role) ==
         "data_artifact");
  assert(pandapi::runtime::artifact_state_name(
             fixture.output_artifacts.front().expected_state) == "complete");
  assert(pandapi::runtime::probe_kind_name(ProbeKind::HarnessTimeout) ==
         "HarnessTimeout");
  assert(pandapi::runtime::probe_safety_name(ProbeSafety::Quarantine) == "quarantine");
  assert(pandapi::runtime::probe_safety_name(ProbeSafety::Manual) == "manual");
  assert(pandapi::runtime::probe_safety_name(ProbeSafety::CiSafe) == "CI-safe");
  assert(!pandapi::runtime::fixture_is_ci_safe(fixture));
  assert(pandapi::runtime::stream_allows_ansi(fixture.stderr_expectation));
  assert(!pandapi::runtime::stream_allows_ansi(fixture.stdout_expectation));
  assert(pandapi::runtime::output_role_for_stream(StreamRole::TaggedStatus) ==
         pandapi::runtime::OutputRole::TaggedStatus);
}

void normalization_and_comparison_helpers_work()
{
  NormalizationRules rules;
  rules.path_tokens.push_back(PathReplacement{PathToken::FixtureRoot, "/fixtures"});
  rules.strip_ansi = true;
  rules.reject_ansi = false;

  const auto normalized =
      pandapi::runtime::normalize_text("\x1b[31m/fixtures/domain.hddl\r\n", rules);
  assert(normalized.has_value());
  assert(normalized.value() == "$FixtureRoot/domain.hddl\n");
  assert(pandapi::runtime::duration_is_non_negative_ms("37"));
  assert(pandapi::runtime::duration_within_bound(std::chrono::milliseconds{37},
                                                 std::chrono::milliseconds{50}));
  assert(pandapi::runtime::timestamp_matches_rfc3339_like("2026-08-09T12:00:00Z"));
  assert(pandapi::runtime::build_metadata_present("clang-17"));
  assert(pandapi::runtime::errno_matches_symbolic("ENOENT", 2));
  assert(pandapi::runtime::signal_name(2) == "SIGINT");
  assert(pandapi::runtime::signal_name(15) == "SIGTERM");
  assert(pandapi::runtime::signal_name(9) == "SIGKILL");
  assert(pandapi::runtime::supervisor_exit_code_for_signal(15) == 143);

  const auto golden = pandapi::runtime::compare_golden(
      "stdout", "$FixtureRoot/domain.hddl\n", "/fixtures/domain.hddl\r\n", rules);
  assert(golden.matches);

  const auto mismatch = pandapi::runtime::compare_golden("stdout", "expected", "actual",
                                                         NormalizationRules{});
  assert(!mismatch.matches);
  assert(mismatch.mismatch == "golden mismatch");

  const auto semantic = pandapi::runtime::compare_semantic_predicate(
      "duration", true, "non-negative duration", "37");
  assert(semantic.matches);
}

void final_status_matching_uses_status_parser()
{
  const auto parsed = pandapi::runtime::parse_final_status(
      "PANDAPI_STATUS\tstatus=ok\tcomponent=engine\tsurface=normal_search"
      "\tsurface_disposition=supported\texit_code=0\tclass=success");
  assert(parsed.has_value());

  const auto matched =
      pandapi::runtime::match_status_record(parsed.value(), StatusCode::Ok);
  assert(matched.matches);

  const auto mismatch =
      pandapi::runtime::match_status_record(parsed.value(), StatusCode::InputInvalid);
  assert(!mismatch.matches);
}

void process_observation_captures_success_and_failure()
{
  CommandSpec command;
  command.executable_path = "/bin/sh";
  command.argv = {"/bin/sh", "-c", "printf fixture-stdout; printf fixture-stderr >&2"};

  const auto observed = pandapi::runtime::run_process_fixture(
      command, RunOptions{std::chrono::milliseconds{500}});
  assert(observed.has_value());
  assert(observed.value().stdout_text == "fixture-stdout");
  assert(observed.value().stderr_text == "fixture-stderr");
  assert(observed.value().exit_code == 0);
  assert(observed.value().wait_status.kind == pandapi::runtime::WaitStatusKind::Exited);
  assert(!observed.value().timed_out);

  CommandSpec failure = command;
  failure.argv = {"/bin/sh", "-c", "exit 7"};
  const auto failed = pandapi::runtime::run_process_fixture(
      failure, RunOptions{std::chrono::milliseconds{500}});
  assert(failed.has_value());
  assert(failed.value().exit_code == 7);
}

void harness_timeout_is_ci_safe_and_bounded()
{
  CommandSpec command;
  command.executable_path = "/bin/sh";
  command.argv = {"/bin/sh", "-c", "sleep 1"};

  const auto observed = pandapi::runtime::run_process_fixture(
      command, RunOptions{std::chrono::milliseconds{50}});
  assert(observed.has_value());
  assert(observed.value().timed_out);
  assert(observed.value().timeout_disposition ==
         pandapi::runtime::TimeoutDisposition::HarnessTimeout);
  assert(observed.value().cleanup_completed);
  assert(observed.value().cleanup_state == pandapi::runtime::CleanupState::Completed);
  assert(observed.value().wait_status.kind ==
         pandapi::runtime::WaitStatusKind::Signaled);
  assert(observed.value().wait_status.signal_number > 0);

  const auto status = pandapi::runtime::signal_terminated_status(
      observed.value().wait_status, Component::Engine, SurfaceDisposition::Supported);
  assert(status.code() == StatusCode::SignalTerminated);
}

} // namespace

int main()
{
  fixture_records_are_typed();
  normalization_and_comparison_helpers_work();
  final_status_matching_uses_status_parser();
  process_observation_captures_success_and_failure();
  harness_timeout_is_ci_safe_and_bounded();
  return 0;
}
