#pragma once

#include "pandapi/runtime/normalization.hpp"
#include "pandapi/runtime/status.hpp"
#include "pandapi/runtime/status_io.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace pandapi::runtime {

enum class EnvironmentMode {
  InheritParent,
  Exact,
};

enum class StdinMode {
  Empty,
  Bytes,
};

enum class StreamRole {
  Empty,
  DataArtifact,
  Informational,
  TaggedStatus,
  HumanDiagnostics,
  Progress,
  Warning,
};

enum class ArtifactState {
  Absent,
  Complete,
  RetainedPartial,
  DiscardedPartial,
};

enum class WaitStatusKind {
  Exited,
  Signaled,
};

enum class FinalStatusObservation {
  Absent,
  Required,
  Forbidden,
};

enum class ProbeKind {
  HarnessTimeout,
  BinaryTimeout,
  ResourceLimit,
  Interrupt,
  SignalTermination,
};

enum class ProbeSafety {
  CiSafe,
  Quarantine,
  Manual,
};

enum class CleanupPolicy {
  RemoveTempArtifacts,
  PreserveForDebug,
};

struct EnvironmentVariable {
  std::string name;
  std::string value;
};

struct EnvironmentSpec {
  EnvironmentMode mode = EnvironmentMode::InheritParent;
  std::vector<EnvironmentVariable> variables;
};

struct StdinSpec {
  StdinMode mode = StdinMode::Empty;
  std::string bytes;
};

struct CommandSpec {
  std::string executable_path;
  std::vector<std::string> argv;
  EnvironmentSpec environment;
  std::string working_directory;
  StdinSpec stdin_spec;
};

struct StreamExpectation {
  StreamRole role = StreamRole::Empty;
  ComparisonPolicy comparison = ComparisonPolicy::Empty;
  std::string expected_text;
  bool ansi_allowed = false;
};

struct OutputArtifactExpectation {
  std::string path;
  std::string role;
  ArtifactState expected_state = ArtifactState::Absent;
};

struct ExpectedExit {
  StatusCode status = StatusCode::Ok;
  int exit_code = 0;
};

struct WaitStatusExpectation {
  WaitStatusKind kind = WaitStatusKind::Exited;
  int signal_number = 0;
};

struct FinalStatusExpectation {
  FinalStatusObservation observation = FinalStatusObservation::Absent;
  StatusStream stream = StatusStream::Stderr;
  StatusCode status = StatusCode::Ok;
  std::vector<std::string> required_fields;
  std::vector<std::string> forbidden_fields;
};

struct ProbePolicy {
  ProbeKind kind = ProbeKind::HarnessTimeout;
  ProbeSafety safety = ProbeSafety::CiSafe;
  std::chrono::milliseconds bound{0};
};

struct SafetyPolicy {
  ProbeSafety default_safety = ProbeSafety::CiSafe;
  CleanupPolicy cleanup = CleanupPolicy::RemoveTempArtifacts;
  std::vector<ProbePolicy> probes;
};

struct FixtureSpec {
  std::string id;
  std::string owner;
  std::string kind;
  Component component = Component::Engine;
  SurfaceDisposition surface_disposition = SurfaceDisposition::Supported;
  CommandSpec command;
  StreamExpectation stdout_expectation;
  StreamExpectation stderr_expectation;
  std::vector<OutputArtifactExpectation> output_artifacts;
  ExpectedExit expected_exit;
  WaitStatusExpectation wait_status;
  FinalStatusExpectation final_status;
  NormalizationRules normalization;
  SafetyPolicy safety;
};

[[nodiscard]] std::string_view environment_mode_name(EnvironmentMode mode) noexcept;
[[nodiscard]] std::string_view stdin_mode_name(StdinMode mode) noexcept;
[[nodiscard]] std::string_view stream_role_name(StreamRole role) noexcept;
[[nodiscard]] std::string_view artifact_state_name(ArtifactState state) noexcept;
[[nodiscard]] std::string_view wait_status_kind_name(WaitStatusKind kind) noexcept;
[[nodiscard]] std::string_view final_status_observation_name(
  FinalStatusObservation observation) noexcept;
[[nodiscard]] std::string_view probe_kind_name(ProbeKind kind) noexcept;
[[nodiscard]] std::string_view probe_safety_name(ProbeSafety safety) noexcept;
[[nodiscard]] std::string_view cleanup_policy_name(CleanupPolicy policy) noexcept;
[[nodiscard]] bool probe_is_ci_safe(ProbeSafety safety) noexcept;
[[nodiscard]] bool fixture_is_ci_safe(const FixtureSpec& fixture) noexcept;
[[nodiscard]] bool stream_allows_ansi(const StreamExpectation& expectation) noexcept;
[[nodiscard]] OutputRole output_role_for_stream(StreamRole role) noexcept;

}  // namespace pandapi::runtime
