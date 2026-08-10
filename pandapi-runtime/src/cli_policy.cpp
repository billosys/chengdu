#include "pandapi/runtime/cli_policy.hpp"

#include <utility>

namespace pandapi::runtime {
namespace {

[[nodiscard]] ProcessStatus cli_usage_error_status(
  Component component,
  SurfaceDisposition surface_disposition) noexcept
{
  return ProcessStatus::from_code(
    StatusCode::CliUsageError,
    component,
    surface_disposition);
}

[[nodiscard]] bool component_accepts_invoked_name(
  Component component,
  std::string_view invoked_name) noexcept
{
  return invoked_name == canonical_command_name(component)
    || invoked_name == inherited_command_name(component);
}

}  // namespace

CommandIdentity::CommandIdentity(
  std::string canonical_name,
  std::string invoked_name,
  bool compatibility_invocation)
  : canonical_name_{std::move(canonical_name)},
    invoked_name_{std::move(invoked_name)},
    compatibility_invocation_{compatibility_invocation}
{
}

std::string_view CommandIdentity::canonical_name() const noexcept
{
  return canonical_name_;
}

std::string_view CommandIdentity::invoked_name() const noexcept
{
  return invoked_name_;
}

bool CommandIdentity::is_compatibility_invocation() const noexcept
{
  return compatibility_invocation_;
}

std::string_view invocation_mode_name(InvocationMode mode) noexcept
{
  switch (mode) {
    case InvocationMode::HumanCli:
      return "human_cli";
    case InvocationMode::Supervised:
      return "supervised";
  }

  return "supervised";
}

std::string_view status_target_name(StatusTarget target) noexcept
{
  switch (target) {
    case StatusTarget::None:
      return "none";
    case StatusTarget::Stderr:
      return "stderr";
    case StatusTarget::Stdout:
      return "stdout";
  }

  return "none";
}

std::string_view output_target_name(OutputTarget target) noexcept
{
  switch (target) {
    case OutputTarget::Absent:
      return "absent";
    case OutputTarget::File:
      return "file";
    case OutputTarget::Stdout:
      return "stdout";
  }

  return "absent";
}

std::string_view output_conflict_name(OutputConflict conflict) noexcept
{
  switch (conflict) {
    case OutputConflict::None:
      return "none";
    case OutputConflict::StatusStdoutWithDataArtifact:
      return "--status=stdout conflicts with stdout DataArtifact";
    case OutputConflict::StatusStdoutWithInformational:
      return "--status=stdout conflicts with human Informational stdout";
    case OutputConflict::OutputOptionWithPositionalAlias:
      return "--output and positional output alias are mutually exclusive";
    case OutputConflict::ColorAlwaysWithSupervised:
      return "color=always conflicts with supervised output";
    case OutputConflict::ColorAlwaysWithMachineOutput:
      return "color=always conflicts with machine output";
  }

  return "none";
}

std::string_view informational_command_name(InformationalCommand command) noexcept
{
  switch (command) {
    case InformationalCommand::None:
      return "none";
    case InformationalCommand::Help:
      return "--help";
    case InformationalCommand::Version:
      return "--version";
    case InformationalCommand::Provenance:
      return "--provenance";
  }

  return "none";
}

std::string_view canonical_command_name(Component component) noexcept
{
  switch (component) {
    case Component::Parser:
      return "pandapi-parser";
    case Component::Grounder:
      return "pandapi-grounder";
    case Component::Engine:
      return "pandapi-engine";
  }

  return "pandapi-engine";
}

std::string_view inherited_command_name(Component component) noexcept
{
  switch (component) {
    case Component::Parser:
      return "pandaPIparser";
    case Component::Grounder:
      return "pandaPIgrounder";
    case Component::Engine:
      return "pandaPIengine";
  }

  return "pandaPIengine";
}

bool is_canonical_command_name(std::string_view name) noexcept
{
  return name == "pandapi-parser" || name == "pandapi-grounder"
    || name == "pandapi-engine";
}

bool is_inherited_command_name(std::string_view name) noexcept
{
  return name == "pandaPIparser" || name == "pandaPIgrounder"
    || name == "pandaPIengine";
}

StatusResult<CommandIdentity> command_identity_for(
  Component component,
  std::string invoked_name,
  SurfaceDisposition surface_disposition)
{
  if (!component_accepts_invoked_name(component, invoked_name)) {
    return StatusResult<CommandIdentity>::failure(
      cli_usage_error_status(component, surface_disposition));
  }

  return StatusResult<CommandIdentity>::success(CommandIdentity{
    std::string{canonical_command_name(component)},
    std::move(invoked_name),
    is_inherited_command_name(invoked_name)});
}

StatusResult<StatusTarget> parse_status_target(
  std::string_view value,
  Component component,
  SurfaceDisposition surface_disposition)
{
  if (value.empty() || value == "stderr") {
    return StatusResult<StatusTarget>::success(StatusTarget::Stderr);
  }
  if (value == "stdout") {
    return StatusResult<StatusTarget>::success(StatusTarget::Stdout);
  }
  if (value == "none") {
    return StatusResult<StatusTarget>::success(StatusTarget::None);
  }

  return StatusResult<StatusTarget>::failure(
    cli_usage_error_status(component, surface_disposition));
}

StatusStream status_stream_for_target(StatusTarget target) noexcept
{
  if (target == StatusTarget::Stdout) {
    return StatusStream::Stdout;
  }

  return StatusStream::Stderr;
}

OutputRole stdout_role_for_options(const CommonCliOptions& options) noexcept
{
  if (options.informational_command != InformationalCommand::None) {
    return OutputRole::Informational;
  }
  if (options.status_target == StatusTarget::Stdout
    && options.output_target == OutputTarget::Absent) {
    return OutputRole::TaggedStatus;
  }
  if (options.output_target == OutputTarget::Stdout) {
    return OutputRole::DataArtifact;
  }

  return OutputRole::Empty;
}

OutputConflict output_conflict_for_options(
  const CommonCliOptions& options) noexcept
{
  if (options.output_option && options.positional_output_alias) {
    return OutputConflict::OutputOptionWithPositionalAlias;
  }

  const auto stdout_role = stdout_role_for_options(options);
  if (options.status_target == StatusTarget::Stdout
    && !status_stream_allowed(StatusStream::Stdout, stdout_role)) {
    if (stdout_role == OutputRole::Informational) {
      return OutputConflict::StatusStdoutWithInformational;
    }
    return OutputConflict::StatusStdoutWithDataArtifact;
  }

  if (effective_color_mode(options.color_controls) == ColorMode::Always) {
    if (options.invocation_mode == InvocationMode::Supervised) {
      return OutputConflict::ColorAlwaysWithSupervised;
    }
    if (options.status_target != StatusTarget::None || options.machine_output) {
      return OutputConflict::ColorAlwaysWithMachineOutput;
    }
  }

  return OutputConflict::None;
}

StatusResult<CommonCliOptions> validate_common_cli_options(
  const CommonCliOptions& options,
  Component component,
  SurfaceDisposition surface_disposition)
{
  const auto conflict = output_conflict_for_options(options);
  if (conflict != OutputConflict::None) {
    return StatusResult<CommonCliOptions>::failure(
      cli_usage_error_status(component, surface_disposition));
  }

  return StatusResult<CommonCliOptions>::success(options);
}

StatusResult<InformationalCommand> informational_command_from_flags(
  bool help,
  bool version,
  bool provenance,
  Component component,
  SurfaceDisposition surface_disposition)
{
  const int enabled_count = (help ? 1 : 0) + (version ? 1 : 0)
    + (provenance ? 1 : 0);
  if (enabled_count > 1) {
    return StatusResult<InformationalCommand>::failure(
      cli_usage_error_status(component, surface_disposition));
  }
  if (help) {
    return StatusResult<InformationalCommand>::success(InformationalCommand::Help);
  }
  if (version) {
    return StatusResult<InformationalCommand>::success(
      InformationalCommand::Version);
  }
  if (provenance) {
    return StatusResult<InformationalCommand>::success(
      InformationalCommand::Provenance);
  }

  return StatusResult<InformationalCommand>::success(InformationalCommand::None);
}

ProcessStatus informational_command_status(
  InformationalCommand command,
  Component component,
  SurfaceDisposition surface_disposition) noexcept
{
  if (command == InformationalCommand::None) {
    return cli_usage_error_status(component, surface_disposition);
  }

  return ProcessStatus::from_code(
    StatusCode::Ok,
    component,
    surface_disposition);
}

OutputRole informational_command_output_role(
  InformationalCommand command) noexcept
{
  if (command == InformationalCommand::None) {
    return OutputRole::Empty;
  }

  return OutputRole::Informational;
}

}  // namespace pandapi::runtime
