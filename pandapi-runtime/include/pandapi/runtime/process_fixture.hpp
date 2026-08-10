#pragma once

#include "pandapi/runtime/fixture.hpp"
#include "pandapi/runtime/result.hpp"

#include <chrono>
#include <string>

namespace pandapi::runtime {

enum class TimeoutDisposition {
  None,
  HarnessTimeout,
  BinaryTimeout,
};

enum class CleanupState {
  NotRequired,
  Completed,
  Failed,
};

struct WaitStatus {
  WaitStatusKind kind = WaitStatusKind::Exited;
  int exit_code = 0;
  int signal_number = 0;
};

struct ProcessObservation {
  std::string stdout_text;
  std::string stderr_text;
  int exit_code = 0;
  WaitStatus wait_status;
  bool timed_out = false;
  TimeoutDisposition timeout_disposition = TimeoutDisposition::None;
  bool cleanup_completed = true;
  CleanupState cleanup_state = CleanupState::NotRequired;
};

struct RunOptions {
  std::chrono::milliseconds timeout{0};
};

[[nodiscard]] std::string_view
timeout_disposition_name(TimeoutDisposition disposition) noexcept;
[[nodiscard]] std::string_view cleanup_state_name(CleanupState state) noexcept;
[[nodiscard]] WaitStatus exited_wait_status(int exit_code) noexcept;
[[nodiscard]] WaitStatus signaled_wait_status(int signal_number) noexcept;
[[nodiscard]] ProcessStatus
signal_terminated_status(const WaitStatus& wait_status, Component component,
                         SurfaceDisposition surface_disposition) noexcept;
[[nodiscard]] StatusResult<ProcessObservation>
run_process_fixture(const CommandSpec& command, RunOptions options);

} // namespace pandapi::runtime
