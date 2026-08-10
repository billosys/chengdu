#pragma once

#include "pandapi/runtime/result.hpp"
#include "pandapi/runtime/status.hpp"
#include "pandapi/runtime/status_io.hpp"
#include "pandapi/runtime/tty.hpp"

#include <string>
#include <string_view>

namespace pandapi::runtime {

enum class InvocationMode {
  HumanCli,
  Supervised,
};

enum class StatusTarget {
  None,
  Stderr,
  Stdout,
};

enum class OutputTarget {
  Absent,
  File,
  Stdout,
};

enum class OutputConflict {
  None,
  StatusStdoutWithDataArtifact,
  StatusStdoutWithInformational,
  OutputOptionWithPositionalAlias,
  ColorAlwaysWithSupervised,
  ColorAlwaysWithMachineOutput,
};

enum class InformationalCommand {
  None,
  Help,
  Version,
  Provenance,
};

class CommandIdentity {
public:
  [[nodiscard]] std::string_view canonical_name() const noexcept;
  [[nodiscard]] std::string_view invoked_name() const noexcept;
  [[nodiscard]] bool is_compatibility_invocation() const noexcept;

private:
  friend StatusResult<CommandIdentity>
  command_identity_for(Component component, std::string invoked_name,
                       SurfaceDisposition surface_disposition);

  CommandIdentity(std::string canonical_name, std::string invoked_name,
                  bool compatibility_invocation);

  std::string canonical_name_;
  std::string invoked_name_;
  bool compatibility_invocation_;
};

struct CommonCliOptions {
  InvocationMode invocation_mode = InvocationMode::HumanCli;
  StatusTarget status_target = StatusTarget::None;
  OutputTarget output_target = OutputTarget::Absent;
  ColorControls color_controls;
  InformationalCommand informational_command = InformationalCommand::None;
  bool machine_output = false;
  bool output_option = false;
  bool positional_output_alias = false;
};

[[nodiscard]] std::string_view invocation_mode_name(InvocationMode mode) noexcept;
[[nodiscard]] std::string_view status_target_name(StatusTarget target) noexcept;
[[nodiscard]] std::string_view output_target_name(OutputTarget target) noexcept;
[[nodiscard]] std::string_view output_conflict_name(OutputConflict conflict) noexcept;
[[nodiscard]] std::string_view
informational_command_name(InformationalCommand command) noexcept;
[[nodiscard]] std::string_view canonical_command_name(Component component) noexcept;
[[nodiscard]] std::string_view inherited_command_name(Component component) noexcept;
[[nodiscard]] bool is_canonical_command_name(std::string_view name) noexcept;
[[nodiscard]] bool is_inherited_command_name(std::string_view name) noexcept;
[[nodiscard]] StatusResult<CommandIdentity> command_identity_for(
    Component component, std::string invoked_name,
    SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] StatusResult<StatusTarget> parse_status_target(
    std::string_view value, Component component,
    SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] StatusStream status_stream_for_target(StatusTarget target) noexcept;
[[nodiscard]] OutputRole
stdout_role_for_options(const CommonCliOptions& options) noexcept;
[[nodiscard]] OutputConflict
output_conflict_for_options(const CommonCliOptions& options) noexcept;
[[nodiscard]] StatusResult<CommonCliOptions> validate_common_cli_options(
    const CommonCliOptions& options, Component component,
    SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] StatusResult<InformationalCommand> informational_command_from_flags(
    bool help, bool version, bool provenance, Component component,
    SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] ProcessStatus informational_command_status(
    InformationalCommand command, Component component,
    SurfaceDisposition surface_disposition = SurfaceDisposition::Supported) noexcept;
[[nodiscard]] OutputRole
informational_command_output_role(InformationalCommand command) noexcept;

} // namespace pandapi::runtime
