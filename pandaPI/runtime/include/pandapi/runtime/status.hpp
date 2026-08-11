#pragma once

#include <string_view>

namespace pandapi::runtime {

enum class StatusCode {
  Ok,
  DomainNoPlan,
  CliUsageError,
  InputUnavailable,
  OutputUnavailable,
  InputInvalid,
  UnsupportedFeature,
  LegacySurface,
  ExperimentalSurface,
  FutureSurface,
  Timeout,
  ResourceLimit,
  Interrupted,
  DependencyFailure,
  ChildProcessFailure,
  InternalError,
  SignalTerminated,
};

enum class StatusClass {
  Success,
  ExpectedDomainOutcome,
  CallerError,
  InputModelError,
  PolicySurfaceFailure,
  TimeoutResourceInterruptionFailure,
  DependencyChildProcessFailure,
  InternalDefect,
  SupervisorOwnedSignalTermination,
};

enum class Component {
  Parser,
  Grounder,
  Engine,
};

enum class SurfaceDisposition {
  Supported,
  Legacy,
  Experimental,
  Unsupported,
  Future,
};

enum class OutputRole {
  Empty,
  DataArtifact,
  Informational,
  TaggedStatus,
};

struct ProcessStatus {
  [[nodiscard]] static ProcessStatus
  from_code(StatusCode code, Component component,
            SurfaceDisposition surface_disposition) noexcept;

  [[nodiscard]] static ProcessStatus
  signal_terminated(int signal_number, Component component,
                    SurfaceDisposition surface_disposition) noexcept;

  [[nodiscard]] StatusCode code() const noexcept;
  [[nodiscard]] Component component() const noexcept;
  [[nodiscard]] SurfaceDisposition surface_disposition() const noexcept;
  [[nodiscard]] int signal_number() const noexcept;
  [[nodiscard]] bool is_signal_terminated() const noexcept;

private:
  ProcessStatus(StatusCode code, Component component,
                SurfaceDisposition surface_disposition, int signal_number) noexcept;

  StatusCode code_;
  Component component_;
  SurfaceDisposition surface_disposition_;
  int signal_number_;
};

[[nodiscard]] std::string_view status_name(StatusCode code) noexcept;
[[nodiscard]] std::string_view status_name(ProcessStatus status) noexcept;
[[nodiscard]] StatusClass status_class(StatusCode code) noexcept;
[[nodiscard]] StatusClass status_class(ProcessStatus status) noexcept;
[[nodiscard]] std::string_view status_class_name(StatusClass status_class) noexcept;
[[nodiscard]] std::string_view component_name(Component component) noexcept;
[[nodiscard]] std::string_view
surface_disposition_name(SurfaceDisposition surface_disposition) noexcept;
[[nodiscard]] int exit_code(ProcessStatus status) noexcept;

} // namespace pandapi::runtime
