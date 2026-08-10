#include "pandapi/runtime/tty.hpp"

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

}  // namespace

std::string_view color_mode_name(ColorMode mode) noexcept
{
  switch (mode) {
    case ColorMode::Auto:
      return "auto";
    case ColorMode::Always:
      return "always";
    case ColorMode::Never:
      return "never";
  }

  return "never";
}

std::string_view terminal_kind_name(TerminalKind terminal) noexcept
{
  switch (terminal) {
    case TerminalKind::Tty:
      return "tty";
    case TerminalKind::NotTty:
      return "not_tty";
    case TerminalKind::Unknown:
      return "unknown";
  }

  return "unknown";
}

std::string_view ansi_stream_name(AnsiStream stream) noexcept
{
  switch (stream) {
    case AnsiStream::Stderr:
      return "stderr";
    case AnsiStream::Stdout:
      return "stdout";
    case AnsiStream::File:
      return "file";
    case AnsiStream::Pipe:
      return "pipe";
  }

  return "pipe";
}

StatusResult<ColorMode> parse_color_mode(
  std::string_view value,
  Component component,
  SurfaceDisposition surface_disposition)
{
  if (value == "auto") {
    return StatusResult<ColorMode>::success(ColorMode::Auto);
  }
  if (value == "always") {
    return StatusResult<ColorMode>::success(ColorMode::Always);
  }
  if (value == "never") {
    return StatusResult<ColorMode>::success(ColorMode::Never);
  }

  return StatusResult<ColorMode>::failure(
    cli_usage_error_status(component, surface_disposition));
}

TerminalKind detect_terminal(bool observed_is_tty) noexcept
{
  return observed_is_tty ? TerminalKind::Tty : TerminalKind::NotTty;
}

bool terminal_allows_color(TerminalKind terminal) noexcept
{
  return terminal == TerminalKind::Tty;
}

bool color_disabling_control_present(const ColorControls& controls) noexcept
{
  return controls.color_mode == ColorMode::Never || controls.no_color
    || controls.no_colour || controls.no_color_environment;
}

ColorMode effective_color_mode(const ColorControls& controls) noexcept
{
  if (color_disabling_control_present(controls)) {
    return ColorMode::Never;
  }

  return controls.color_mode;
}

bool allows_color(const AnsiPolicy& policy) noexcept
{
  if (effective_color_mode(policy.color_controls) == ColorMode::Never) {
    return false;
  }
  if (policy.supervised || policy.machine_output) {
    return false;
  }
  if (policy.stream != AnsiStream::Stderr) {
    return false;
  }
  if (policy.output_role == OutputRole::DataArtifact
    || policy.output_role == OutputRole::TaggedStatus) {
    return false;
  }

  return terminal_allows_color(policy.stderr_terminal);
}

}  // namespace pandapi::runtime
