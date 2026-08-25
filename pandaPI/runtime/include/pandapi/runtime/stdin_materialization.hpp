#pragma once

#include "pandapi/runtime/result.hpp"
#include "pandapi/runtime/status_io.hpp"

#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace pandapi::runtime {

enum class InputPathRole {
  ParserDomain,
  ParserProblem,
  GrounderHtn,
  EngineSas,
};

enum class StdinOperation {
  Read,
  Materialize,
  Cleanup,
};

struct StdinMaterializationRequest {
  Component component;
  InputPathRole path_role;
  std::string temp_prefix;
};

class MaterializedStdin {
public:
  MaterializedStdin(std::string path, InputPathRole path_role);
  MaterializedStdin(const MaterializedStdin&) = delete;
  MaterializedStdin& operator=(const MaterializedStdin&) = delete;
  MaterializedStdin(MaterializedStdin&& other) noexcept;
  MaterializedStdin& operator=(MaterializedStdin&& other) noexcept;
  ~MaterializedStdin() noexcept;

  [[nodiscard]] const std::string& path() const noexcept;
  [[nodiscard]] InputPathRole path_role() const noexcept;
  [[nodiscard]] std::string_view logical_path() const noexcept;
  [[nodiscard]] bool cleanup() noexcept;

private:
  std::string path_;
  InputPathRole path_role_;
};

[[nodiscard]] bool is_stdin_sentinel(std::string_view value) noexcept;
[[nodiscard]] std::string_view path_role_name(InputPathRole path_role) noexcept;
[[nodiscard]] std::string_view stdin_operation_name(StdinOperation operation) noexcept;
[[nodiscard]] StdinOperation stdin_failure_operation(ProcessStatus status) noexcept;
[[nodiscard]] std::vector<StatusField> stdin_status_fields(InputPathRole path_role,
                                                           StdinOperation operation);
[[nodiscard]] StatusResult<MaterializedStdin>
materialize_stdin(std::istream& input, const StdinMaterializationRequest& request);

} // namespace pandapi::runtime
