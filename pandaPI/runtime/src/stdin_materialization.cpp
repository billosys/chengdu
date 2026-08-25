#include "pandapi/runtime/stdin_materialization.hpp"

#include <cstdio>
#include <array>
#include <fstream>
#include <istream>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

namespace pandapi::runtime {
namespace {

[[nodiscard]] ProcessStatus failure_status(Component component,
                                           StatusCode code) noexcept
{
  return ProcessStatus::from_code(code, component, SurfaceDisposition::Supported);
}

[[nodiscard]] StatusResult<MaterializedStdin> output_failure(Component component,
                                                             const std::string& path)
{
  if (!path.empty()) {
    std::remove(path.c_str());
  }
  return StatusResult<MaterializedStdin>::failure(
      failure_status(component, StatusCode::OutputUnavailable));
}

[[nodiscard]] StatusResult<MaterializedStdin> input_failure(Component component,
                                                            const std::string& path)
{
  if (!path.empty()) {
    std::remove(path.c_str());
  }
  return StatusResult<MaterializedStdin>::failure(
      failure_status(component, StatusCode::InputUnavailable));
}

[[nodiscard]] std::string make_template_path(std::string_view prefix)
{
  return "/tmp/" + std::string{prefix} + ".XXXXXX";
}

} // namespace

MaterializedStdin::MaterializedStdin(std::string path, InputPathRole path_role)
    : path_{std::move(path)},
      path_role_{path_role}
{
}

MaterializedStdin::MaterializedStdin(MaterializedStdin&& other) noexcept
    : path_{std::move(other.path_)},
      path_role_{other.path_role_}
{
  other.path_.clear();
}

MaterializedStdin& MaterializedStdin::operator=(MaterializedStdin&& other) noexcept
{
  if (this != &other) {
    (void)cleanup();
    path_ = std::move(other.path_);
    path_role_ = other.path_role_;
    other.path_.clear();
  }
  return *this;
}

MaterializedStdin::~MaterializedStdin() noexcept { (void)cleanup(); }

const std::string& MaterializedStdin::path() const noexcept { return path_; }

InputPathRole MaterializedStdin::path_role() const noexcept { return path_role_; }

std::string_view MaterializedStdin::logical_path() const noexcept { return "-"; }

bool MaterializedStdin::cleanup() noexcept
{
  if (path_.empty()) {
    return true;
  }

  const auto removed = std::remove(path_.c_str()) == 0;
  path_.clear();
  return removed;
}

bool is_stdin_sentinel(std::string_view value) noexcept { return value == "-"; }

std::string_view path_role_name(InputPathRole path_role) noexcept
{
  switch (path_role) {
  case InputPathRole::ParserDomain:
    return "domain";
  case InputPathRole::ParserProblem:
    return "problem";
  case InputPathRole::GrounderHtn:
    return "htn";
  case InputPathRole::EngineSas:
    return "engine_input";
  }

  return "input";
}

std::string_view stdin_operation_name(StdinOperation operation) noexcept
{
  switch (operation) {
  case StdinOperation::Read:
    return "read";
  case StdinOperation::Materialize:
    return "materialize";
  case StdinOperation::Cleanup:
    return "cleanup";
  }

  return "materialize";
}

StdinOperation stdin_failure_operation(ProcessStatus status) noexcept
{
  if (status.code() == StatusCode::InputUnavailable) {
    return StdinOperation::Read;
  }
  return StdinOperation::Materialize;
}

std::vector<StatusField> stdin_status_fields(InputPathRole path_role,
                                             StdinOperation operation)
{
  return {{"path_role", std::string{path_role_name(path_role)}},
          {"path", "-"},
          {"operation", std::string{stdin_operation_name(operation)}}};
}

StatusResult<MaterializedStdin>
materialize_stdin(std::istream& input, const StdinMaterializationRequest& request)
{
  auto template_path = make_template_path(request.temp_prefix);
  std::vector<char> mutable_path{template_path.begin(), template_path.end()};
  mutable_path.push_back('\0');

  const int fd = mkstemp(mutable_path.data());
  if (fd < 0) {
    return output_failure(request.component, "");
  }

  const std::string path{mutable_path.data()};
  if (close(fd) != 0) {
    return output_failure(request.component, path);
  }

  std::ofstream output{path, std::ios::binary | std::ios::trunc};
  if (!output.good()) {
    return output_failure(request.component, path);
  }

  std::array<char, 8192> buffer{};
  while (input) {
    input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    const auto bytes_read = input.gcount();
    if (bytes_read > 0) {
      output.write(buffer.data(), bytes_read);
      if (!output.good()) {
        return output_failure(request.component, path);
      }
    }
  }
  if (input.bad()) {
    return input_failure(request.component, path);
  }
  if (!output.good()) {
    return output_failure(request.component, path);
  }
  output.close();
  if (!output.good()) {
    return output_failure(request.component, path);
  }

  return StatusResult<MaterializedStdin>::success(
      MaterializedStdin{path, request.path_role});
}

} // namespace pandapi::runtime
