#include "pandapi/runtime/status.hpp"

namespace pandapi::runtime {

ProcessStatus::ProcessStatus(StatusCode code, Component component,
                             SurfaceDisposition surface_disposition,
                             int signal_number) noexcept
    : code_{code},
      component_{component},
      surface_disposition_{surface_disposition},
      signal_number_{signal_number}
{
}

ProcessStatus ProcessStatus::from_code(StatusCode code, Component component,
                                       SurfaceDisposition surface_disposition) noexcept
{
  return ProcessStatus{code, component, surface_disposition, 0};
}

ProcessStatus
ProcessStatus::signal_terminated(int signal_number, Component component,
                                 SurfaceDisposition surface_disposition) noexcept
{
  return ProcessStatus{StatusCode::SignalTerminated, component, surface_disposition,
                       signal_number};
}

StatusCode ProcessStatus::code() const noexcept { return code_; }

Component ProcessStatus::component() const noexcept { return component_; }

SurfaceDisposition ProcessStatus::surface_disposition() const noexcept
{
  return surface_disposition_;
}

int ProcessStatus::signal_number() const noexcept { return signal_number_; }

bool ProcessStatus::is_signal_terminated() const noexcept
{
  return code_ == StatusCode::SignalTerminated;
}

std::string_view status_name(StatusCode code) noexcept
{
  switch (code) {
  case StatusCode::Ok:
    return "ok";
  case StatusCode::DomainNoPlan:
    return "domain_no_plan";
  case StatusCode::CliUsageError:
    return "cli_usage_error";
  case StatusCode::InputUnavailable:
    return "input_unavailable";
  case StatusCode::OutputUnavailable:
    return "output_unavailable";
  case StatusCode::InputInvalid:
    return "input_invalid";
  case StatusCode::UnsupportedFeature:
    return "unsupported_feature";
  case StatusCode::LegacySurface:
    return "legacy_surface";
  case StatusCode::ExperimentalSurface:
    return "experimental_surface";
  case StatusCode::FutureSurface:
    return "future_surface";
  case StatusCode::Timeout:
    return "timeout";
  case StatusCode::ResourceLimit:
    return "resource_limit";
  case StatusCode::Interrupted:
    return "interrupted";
  case StatusCode::DependencyFailure:
    return "dependency_failure";
  case StatusCode::ChildProcessFailure:
    return "child_process_failure";
  case StatusCode::InternalError:
    return "internal_error";
  case StatusCode::SignalTerminated:
    return "signal_terminated";
  }

  return "internal_error";
}

std::string_view status_name(ProcessStatus status) noexcept
{
  return status_name(status.code());
}

StatusClass status_class(StatusCode code) noexcept
{
  switch (code) {
  case StatusCode::Ok:
    return StatusClass::Success;
  case StatusCode::DomainNoPlan:
    return StatusClass::ExpectedDomainOutcome;
  case StatusCode::CliUsageError:
  case StatusCode::InputUnavailable:
  case StatusCode::OutputUnavailable:
    return StatusClass::CallerError;
  case StatusCode::InputInvalid:
    return StatusClass::InputModelError;
  case StatusCode::UnsupportedFeature:
  case StatusCode::LegacySurface:
  case StatusCode::ExperimentalSurface:
  case StatusCode::FutureSurface:
    return StatusClass::PolicySurfaceFailure;
  case StatusCode::Timeout:
  case StatusCode::ResourceLimit:
  case StatusCode::Interrupted:
    return StatusClass::TimeoutResourceInterruptionFailure;
  case StatusCode::DependencyFailure:
  case StatusCode::ChildProcessFailure:
    return StatusClass::DependencyChildProcessFailure;
  case StatusCode::InternalError:
    return StatusClass::InternalDefect;
  case StatusCode::SignalTerminated:
    return StatusClass::SupervisorOwnedSignalTermination;
  }

  return StatusClass::InternalDefect;
}

StatusClass status_class(ProcessStatus status) noexcept
{
  return status_class(status.code());
}

std::string_view status_class_name(StatusClass status_class_value) noexcept
{
  switch (status_class_value) {
  case StatusClass::Success:
    return "success";
  case StatusClass::ExpectedDomainOutcome:
    return "expected_domain_outcome";
  case StatusClass::CallerError:
    return "caller_error";
  case StatusClass::InputModelError:
    return "input_model_error";
  case StatusClass::PolicySurfaceFailure:
    return "policy_surface_failure";
  case StatusClass::TimeoutResourceInterruptionFailure:
    return "timeout_resource_interruption_failure";
  case StatusClass::DependencyChildProcessFailure:
    return "dependency_child_process_failure";
  case StatusClass::InternalDefect:
    return "internal_defect";
  case StatusClass::SupervisorOwnedSignalTermination:
    return "supervisor_owned_signal_termination";
  }

  return "internal_defect";
}

std::string_view component_name(Component component) noexcept
{
  switch (component) {
  case Component::Parser:
    return "parser";
  case Component::Grounder:
    return "grounder";
  case Component::Engine:
    return "engine";
  }

  return "engine";
}

std::string_view
surface_disposition_name(SurfaceDisposition surface_disposition) noexcept
{
  switch (surface_disposition) {
  case SurfaceDisposition::Supported:
    return "supported";
  case SurfaceDisposition::Legacy:
    return "legacy";
  case SurfaceDisposition::Experimental:
    return "experimental";
  case SurfaceDisposition::Unsupported:
    return "unsupported";
  case SurfaceDisposition::Future:
    return "future";
  }

  return "unsupported";
}

int exit_code(ProcessStatus status) noexcept
{
  switch (status.code()) {
  case StatusCode::Ok:
    return 0;
  case StatusCode::DomainNoPlan:
    return 2;
  case StatusCode::CliUsageError:
    return 10;
  case StatusCode::InputUnavailable:
    return 20;
  case StatusCode::OutputUnavailable:
    return 21;
  case StatusCode::InputInvalid:
    return 22;
  case StatusCode::UnsupportedFeature:
    return 30;
  case StatusCode::LegacySurface:
    return 31;
  case StatusCode::ExperimentalSurface:
    return 32;
  case StatusCode::FutureSurface:
    return 33;
  case StatusCode::Timeout:
    return 40;
  case StatusCode::ResourceLimit:
    return 41;
  case StatusCode::Interrupted:
    return 42;
  case StatusCode::DependencyFailure:
    return 50;
  case StatusCode::ChildProcessFailure:
    return 51;
  case StatusCode::InternalError:
    return 60;
  case StatusCode::SignalTerminated:
    return 128 + status.signal_number();
  }

  return 60;
}

} // namespace pandapi::runtime
