#include "pandapi/runtime/process_fixture.hpp"

#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

namespace pandapi::runtime {
namespace {

class UniqueFd {
public:
  UniqueFd() noexcept = default;
  explicit UniqueFd(int fd) noexcept : fd_{fd} {}
  ~UniqueFd() noexcept { close(); }

  UniqueFd(const UniqueFd&) = delete;
  UniqueFd& operator=(const UniqueFd&) = delete;

  UniqueFd(UniqueFd&& other) noexcept : fd_{other.fd_} { other.fd_ = -1; }

  UniqueFd& operator=(UniqueFd&& other) noexcept
  {
    if (this != &other) {
      close();
      fd_ = other.fd_;
      other.fd_ = -1;
    }
    return *this;
  }

  [[nodiscard]] int get() const noexcept { return fd_; }

  [[nodiscard]] bool valid() const noexcept { return fd_ >= 0; }

  [[nodiscard]] int release() noexcept
  {
    const auto released = fd_;
    fd_ = -1;
    return released;
  }

  void close() noexcept
  {
    if (fd_ >= 0) {
      ::close(fd_);
      fd_ = -1;
    }
  }

private:
  int fd_ = -1;
};

struct PipePair {
  UniqueFd read_end;
  UniqueFd write_end;
};

[[nodiscard]] StatusResult<PipePair> make_pipe()
{
  int fds[2] = {-1, -1};
  if (::pipe(fds) != 0) {
    return StatusResult<PipePair>::failure(
        ProcessStatus::from_code(StatusCode::ChildProcessFailure, Component::Engine,
                                 SurfaceDisposition::Supported));
  }

  return StatusResult<PipePair>::success(PipePair{UniqueFd{fds[0]}, UniqueFd{fds[1]}});
}

void make_nonblocking(const UniqueFd& fd) noexcept
{
  const auto flags = ::fcntl(fd.get(), F_GETFL, 0);
  if (flags >= 0) {
    static_cast<void>(::fcntl(fd.get(), F_SETFL, flags | O_NONBLOCK));
  }
}

void append_available_output(UniqueFd& fd, std::string& output)
{
  char buffer[4096];
  while (fd.valid()) {
    const auto count = ::read(fd.get(), buffer, sizeof(buffer));
    if (count > 0) {
      output.append(buffer, static_cast<std::string::size_type>(count));
      continue;
    }
    if (count == 0) {
      fd.close();
      return;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
      return;
    }
    fd.close();
    return;
  }
}

void write_stdin_and_close(UniqueFd& fd, const StdinSpec& stdin_spec)
{
  if (!fd.valid()) {
    return;
  }

  if (stdin_spec.mode == StdinMode::Bytes) {
    std::string::size_type written = 0;
    while (written < stdin_spec.bytes.size()) {
      const auto count = ::write(fd.get(), stdin_spec.bytes.data() + written,
                                 stdin_spec.bytes.size() - written);
      if (count > 0) {
        written += static_cast<std::string::size_type>(count);
        continue;
      }
      if (count < 0 && errno == EINTR) {
        continue;
      }
      break;
    }
  }

  fd.close();
}

[[nodiscard]] std::vector<std::string> argv_strings(const CommandSpec& command)
{
  if (!command.argv.empty()) {
    return command.argv;
  }
  return std::vector<std::string>{command.executable_path};
}

[[nodiscard]] std::vector<std::string>
environment_strings(const EnvironmentSpec& environment)
{
  std::vector<std::string> values;
  values.reserve(environment.variables.size());
  for (const auto& variable : environment.variables) {
    values.push_back(variable.name + "=" + variable.value);
  }
  return values;
}

[[nodiscard]] std::vector<char*> mutable_pointers(std::vector<std::string>& values)
{
  std::vector<char*> pointers;
  pointers.reserve(values.size() + 1);
  for (auto& value : values) {
    pointers.push_back(value.data());
  }
  pointers.push_back(nullptr);
  return pointers;
}

[[nodiscard]] ProcessStatus child_process_failure_status() noexcept
{
  return ProcessStatus::from_code(StatusCode::ChildProcessFailure, Component::Engine,
                                  SurfaceDisposition::Supported);
}

} // namespace

std::string_view timeout_disposition_name(TimeoutDisposition disposition) noexcept
{
  switch (disposition) {
  case TimeoutDisposition::None:
    return "none";
  case TimeoutDisposition::HarnessTimeout:
    return "harness-timeout";
  case TimeoutDisposition::BinaryTimeout:
    return "binary-timeout";
  }

  return "none";
}

std::string_view cleanup_state_name(CleanupState state) noexcept
{
  switch (state) {
  case CleanupState::NotRequired:
    return "not_required";
  case CleanupState::Completed:
    return "cleanup_completed";
  case CleanupState::Failed:
    return "cleanup_failed";
  }

  return "cleanup_failed";
}

WaitStatus exited_wait_status(int exit_code) noexcept
{
  return WaitStatus{WaitStatusKind::Exited, exit_code, 0};
}

WaitStatus signaled_wait_status(int signal_number) noexcept
{
  return WaitStatus{WaitStatusKind::Signaled,
                    supervisor_exit_code_for_signal(signal_number), signal_number};
}

ProcessStatus signal_terminated_status(const WaitStatus& wait_status,
                                       Component component,
                                       SurfaceDisposition surface_disposition) noexcept
{
  return ProcessStatus::signal_terminated(wait_status.signal_number, component,
                                          surface_disposition);
}

StatusResult<ProcessObservation> run_process_fixture(const CommandSpec& command,
                                                     RunOptions options)
{
  auto stdin_pipe = make_pipe();
  auto stdout_pipe = make_pipe();
  auto stderr_pipe = make_pipe();
  if (!stdin_pipe.has_value() || !stdout_pipe.has_value() || !stderr_pipe.has_value()) {
    return StatusResult<ProcessObservation>::failure(child_process_failure_status());
  }

  auto argv_storage = argv_strings(command);
  auto argv = mutable_pointers(argv_storage);
  auto env_storage = environment_strings(command.environment);
  auto envp = mutable_pointers(env_storage);

  const auto pid = ::fork();
  if (pid < 0) {
    return StatusResult<ProcessObservation>::failure(child_process_failure_status());
  }

  if (pid == 0) {
    static_cast<void>(::dup2(stdin_pipe.value().read_end.get(), STDIN_FILENO));
    static_cast<void>(::dup2(stdout_pipe.value().write_end.get(), STDOUT_FILENO));
    static_cast<void>(::dup2(stderr_pipe.value().write_end.get(), STDERR_FILENO));

    stdin_pipe.value().read_end.close();
    stdin_pipe.value().write_end.close();
    stdout_pipe.value().read_end.close();
    stdout_pipe.value().write_end.close();
    stderr_pipe.value().read_end.close();
    stderr_pipe.value().write_end.close();

    if (!command.working_directory.empty()) {
      static_cast<void>(::chdir(command.working_directory.c_str()));
    }

    if (command.environment.mode == EnvironmentMode::Exact) {
      ::execve(command.executable_path.c_str(), argv.data(), envp.data());
    } else {
      ::execv(command.executable_path.c_str(), argv.data());
    }
    _exit(127);
  }

  stdin_pipe.value().read_end.close();
  stdout_pipe.value().write_end.close();
  stderr_pipe.value().write_end.close();
  make_nonblocking(stdout_pipe.value().read_end);
  make_nonblocking(stderr_pipe.value().read_end);
  write_stdin_and_close(stdin_pipe.value().write_end, command.stdin_spec);

  ProcessObservation observation;
  bool process_done = false;
  bool killed_for_timeout = false;
  int status = 0;
  const auto start = std::chrono::steady_clock::now();

  while (!process_done || stdout_pipe.value().read_end.valid() ||
         stderr_pipe.value().read_end.valid()) {
    append_available_output(stdout_pipe.value().read_end, observation.stdout_text);
    append_available_output(stderr_pipe.value().read_end, observation.stderr_text);

    if (!process_done) {
      const auto wait_result = ::waitpid(pid, &status, WNOHANG);
      if (wait_result == pid) {
        process_done = true;
      } else if (wait_result < 0 && errno != EINTR) {
        process_done = true;
        observation.cleanup_completed = false;
        observation.cleanup_state = CleanupState::Failed;
      } else if (!killed_for_timeout && options.timeout.count() > 0) {
        const auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed >= options.timeout) {
          static_cast<void>(::kill(pid, SIGKILL));
          killed_for_timeout = true;
          observation.timed_out = true;
          observation.timeout_disposition = TimeoutDisposition::HarnessTimeout;
        }
      }
    }

    if (!process_done || stdout_pipe.value().read_end.valid() ||
        stderr_pipe.value().read_end.valid()) {
      std::this_thread::sleep_for(std::chrono::milliseconds{5});
    }
  }

  if (WIFEXITED(status)) {
    observation.wait_status = exited_wait_status(WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    observation.wait_status = signaled_wait_status(WTERMSIG(status));
  } else {
    observation.wait_status = exited_wait_status(60);
  }
  observation.exit_code = observation.wait_status.exit_code;
  if (observation.cleanup_state == CleanupState::NotRequired) {
    observation.cleanup_state =
        killed_for_timeout ? CleanupState::Completed : CleanupState::NotRequired;
  }

  return StatusResult<ProcessObservation>::success(std::move(observation));
}

} // namespace pandapi::runtime
