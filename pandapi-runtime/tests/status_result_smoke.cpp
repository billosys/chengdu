#include "pandapi/runtime/result.hpp"
#include "pandapi/runtime/status.hpp"

#include <array>
#include <string_view>

namespace {

struct StatusCase {
  pandapi::runtime::StatusCode code;
  std::string_view name;
  int exit_code;
  pandapi::runtime::StatusClass status_class;
};

bool check_status_case(const StatusCase& test_case)
{
  const auto status = pandapi::runtime::ProcessStatus::from_code(
    test_case.code,
    pandapi::runtime::Component::Engine,
    pandapi::runtime::SurfaceDisposition::Supported);

  return pandapi::runtime::status_name(status) == test_case.name
    && pandapi::runtime::exit_code(status) == test_case.exit_code
    && pandapi::runtime::status_class(status) == test_case.status_class;
}

bool check_status_result()
{
  const auto ok = pandapi::runtime::StatusResult<int>::success(42);
  if (!ok.has_value() || ok.value() != 42) {
    return false;
  }

  const auto failed = pandapi::runtime::StatusResult<int>::failure(
    pandapi::runtime::ProcessStatus::from_code(
      pandapi::runtime::StatusCode::InputInvalid,
      pandapi::runtime::Component::Parser,
      pandapi::runtime::SurfaceDisposition::Supported));

  return !failed.has_value()
    && failed.status().code() == pandapi::runtime::StatusCode::InputInvalid
    && pandapi::runtime::exit_code(failed.status()) == 22;
}

}  // namespace

int main()
{
  using pandapi::runtime::StatusClass;
  using pandapi::runtime::StatusCode;

  const std::array<StatusCase, 16> cases{{
    {StatusCode::Ok, "ok", 0, StatusClass::Success},
    {StatusCode::DomainNoPlan, "domain_no_plan", 2, StatusClass::ExpectedDomainOutcome},
    {StatusCode::CliUsageError, "cli_usage_error", 10, StatusClass::CallerError},
    {StatusCode::InputUnavailable, "input_unavailable", 20, StatusClass::CallerError},
    {StatusCode::OutputUnavailable, "output_unavailable", 21, StatusClass::CallerError},
    {StatusCode::InputInvalid, "input_invalid", 22, StatusClass::InputModelError},
    {StatusCode::UnsupportedFeature, "unsupported_feature", 30, StatusClass::PolicySurfaceFailure},
    {StatusCode::LegacySurface, "legacy_surface", 31, StatusClass::PolicySurfaceFailure},
    {StatusCode::ExperimentalSurface, "experimental_surface", 32, StatusClass::PolicySurfaceFailure},
    {StatusCode::FutureSurface, "future_surface", 33, StatusClass::PolicySurfaceFailure},
    {StatusCode::Timeout, "timeout", 40, StatusClass::TimeoutResourceInterruptionFailure},
    {StatusCode::ResourceLimit, "resource_limit", 41, StatusClass::TimeoutResourceInterruptionFailure},
    {StatusCode::Interrupted, "interrupted", 42, StatusClass::TimeoutResourceInterruptionFailure},
    {StatusCode::DependencyFailure, "dependency_failure", 50, StatusClass::DependencyChildProcessFailure},
    {StatusCode::ChildProcessFailure, "child_process_failure", 51, StatusClass::DependencyChildProcessFailure},
    {StatusCode::InternalError, "internal_error", 60, StatusClass::InternalDefect},
  }};

  for (const auto& test_case : cases) {
    if (!check_status_case(test_case)) {
      return 1;
    }
  }

  const auto signal_status = pandapi::runtime::ProcessStatus::signal_terminated(
    15,
    pandapi::runtime::Component::Engine,
    pandapi::runtime::SurfaceDisposition::Supported);
  if (!signal_status.is_signal_terminated()) {
    return 2;
  }
  if (pandapi::runtime::status_name(signal_status) != "signal_terminated") {
    return 3;
  }
  if (pandapi::runtime::exit_code(signal_status) != 128 + signal_status.signal_number()) {
    return 4;
  }
  if (pandapi::runtime::exit_code(signal_status) != 143) {
    return 5;
  }
  if (pandapi::runtime::status_class(signal_status)
      != StatusClass::SupervisorOwnedSignalTermination) {
    return 6;
  }

  if (pandapi::runtime::component_name(pandapi::runtime::Component::Parser) != "parser"
      || pandapi::runtime::component_name(pandapi::runtime::Component::Grounder) != "grounder"
      || pandapi::runtime::component_name(pandapi::runtime::Component::Engine) != "engine") {
    return 7;
  }

  if (pandapi::runtime::surface_disposition_name(
        pandapi::runtime::SurfaceDisposition::Experimental)
      != "experimental") {
    return 8;
  }

  if (!check_status_result()) {
    return 9;
  }

  return 0;
}
