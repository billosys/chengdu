#pragma once

#include "pandapi/runtime/result.hpp"
#include "pandapi/runtime/status.hpp"

#include <string_view>

namespace pandapi::runtime {

enum class ColorMode {
  Auto,
  Always,
  Never,
};

enum class TerminalKind {
  Tty,
  NotTty,
  Unknown,
};

enum class AnsiStream {
  Stderr,
  Stdout,
  File,
  Pipe,
};

struct ColorControls {
  ColorMode color_mode = ColorMode::Auto;
  bool no_color = false;
  bool no_colour = false;
  bool no_color_environment = false;
};

struct AnsiPolicy {
  ColorControls color_controls;
  TerminalKind stderr_terminal = TerminalKind::Unknown;
  AnsiStream stream = AnsiStream::Stderr;
  OutputRole output_role = OutputRole::Empty;
  bool supervised = false;
  bool machine_output = false;
};

[[nodiscard]] std::string_view color_mode_name(ColorMode mode) noexcept;
[[nodiscard]] std::string_view terminal_kind_name(TerminalKind terminal) noexcept;
[[nodiscard]] std::string_view ansi_stream_name(AnsiStream stream) noexcept;
[[nodiscard]] StatusResult<ColorMode> parse_color_mode(
  std::string_view value,
  Component component,
  SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] TerminalKind detect_terminal(bool observed_is_tty) noexcept;
[[nodiscard]] bool terminal_allows_color(TerminalKind terminal) noexcept;
[[nodiscard]] bool color_disabling_control_present(
  const ColorControls& controls) noexcept;
[[nodiscard]] ColorMode effective_color_mode(
  const ColorControls& controls) noexcept;
[[nodiscard]] bool allows_color(const AnsiPolicy& policy) noexcept;

}  // namespace pandapi::runtime
