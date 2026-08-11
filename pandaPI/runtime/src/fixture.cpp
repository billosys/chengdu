#include "pandapi/runtime/fixture.hpp"

namespace pandapi::runtime {

std::string_view environment_mode_name(EnvironmentMode mode) noexcept
{
  switch (mode) {
  case EnvironmentMode::InheritParent:
    return "inherit_parent";
  case EnvironmentMode::Exact:
    return "exact";
  }

  return "inherit_parent";
}

std::string_view stdin_mode_name(StdinMode mode) noexcept
{
  switch (mode) {
  case StdinMode::Empty:
    return "empty";
  case StdinMode::Bytes:
    return "bytes";
  }

  return "empty";
}

std::string_view stream_role_name(StreamRole role) noexcept
{
  switch (role) {
  case StreamRole::Empty:
    return "empty";
  case StreamRole::DataArtifact:
    return "data_artifact";
  case StreamRole::Informational:
    return "informational";
  case StreamRole::TaggedStatus:
    return "tagged_status";
  case StreamRole::HumanDiagnostics:
    return "human_diagnostics";
  case StreamRole::Progress:
    return "progress";
  case StreamRole::Warning:
    return "warning";
  }

  return "empty";
}

std::string_view artifact_state_name(ArtifactState state) noexcept
{
  switch (state) {
  case ArtifactState::Absent:
    return "absent";
  case ArtifactState::Complete:
    return "complete";
  case ArtifactState::RetainedPartial:
    return "retained-partial";
  case ArtifactState::DiscardedPartial:
    return "discarded-partial";
  }

  return "absent";
}

std::string_view wait_status_kind_name(WaitStatusKind kind) noexcept
{
  switch (kind) {
  case WaitStatusKind::Exited:
    return "exited";
  case WaitStatusKind::Signaled:
    return "signaled";
  }

  return "exited";
}

std::string_view
final_status_observation_name(FinalStatusObservation observation) noexcept
{
  switch (observation) {
  case FinalStatusObservation::Absent:
    return "absent";
  case FinalStatusObservation::Required:
    return "required";
  case FinalStatusObservation::Forbidden:
    return "forbidden";
  }

  return "absent";
}

std::string_view probe_kind_name(ProbeKind kind) noexcept
{
  switch (kind) {
  case ProbeKind::HarnessTimeout:
    return "HarnessTimeout";
  case ProbeKind::BinaryTimeout:
    return "BinaryTimeout";
  case ProbeKind::ResourceLimit:
    return "ResourceLimit";
  case ProbeKind::Interrupt:
    return "interrupt";
  case ProbeKind::SignalTermination:
    return "signal_terminated";
  }

  return "HarnessTimeout";
}

std::string_view probe_safety_name(ProbeSafety safety) noexcept
{
  switch (safety) {
  case ProbeSafety::CiSafe:
    return "CI-safe";
  case ProbeSafety::Quarantine:
    return "quarantine";
  case ProbeSafety::Manual:
    return "manual";
  }

  return "manual";
}

std::string_view cleanup_policy_name(CleanupPolicy policy) noexcept
{
  switch (policy) {
  case CleanupPolicy::RemoveTempArtifacts:
    return "remove-temp-artifacts";
  case CleanupPolicy::PreserveForDebug:
    return "preserve-for-debug";
  }

  return "remove-temp-artifacts";
}

bool probe_is_ci_safe(ProbeSafety safety) noexcept
{
  return safety == ProbeSafety::CiSafe;
}

bool fixture_is_ci_safe(const FixtureSpec& fixture) noexcept
{
  if (!probe_is_ci_safe(fixture.safety.default_safety)) {
    return false;
  }

  for (const auto& probe : fixture.safety.probes) {
    if (!probe_is_ci_safe(probe.safety)) {
      return false;
    }
  }
  return true;
}

bool stream_allows_ansi(const StreamExpectation& expectation) noexcept
{
  return expectation.ansi_allowed && expectation.role == StreamRole::HumanDiagnostics;
}

OutputRole output_role_for_stream(StreamRole role) noexcept
{
  switch (role) {
  case StreamRole::Empty:
    return OutputRole::Empty;
  case StreamRole::DataArtifact:
    return OutputRole::DataArtifact;
  case StreamRole::Informational:
    return OutputRole::Informational;
  case StreamRole::TaggedStatus:
    return OutputRole::TaggedStatus;
  case StreamRole::HumanDiagnostics:
  case StreamRole::Progress:
  case StreamRole::Warning:
    return OutputRole::Empty;
  }

  return OutputRole::Empty;
}

} // namespace pandapi::runtime
