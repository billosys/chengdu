#include "pandapi/runtime/stdin_materialization.hpp"

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <unistd.h>

namespace {

[[nodiscard]] bool file_exists(const std::string& path)
{
  return access(path.c_str(), F_OK) == 0;
}

[[nodiscard]] std::string read_file(const std::string& path)
{
  std::ifstream input{path, std::ios::binary};
  std::ostringstream output;
  output << input.rdbuf();
  return output.str();
}

void materializes_stdin_and_cleans_up()
{
  std::istringstream input{"parser artifact\nline 2\n"};
  auto result = pandapi::runtime::materialize_stdin(
      input,
      pandapi::runtime::StdinMaterializationRequest{
          pandapi::runtime::Component::Grounder,
          pandapi::runtime::InputPathRole::GrounderHtn, "pandapi-runtime-stdin-smoke"});

  assert(result.has_value());
  auto materialized = std::move(result.value());
  const auto path = materialized.path();

  assert(pandapi::runtime::is_stdin_sentinel(materialized.logical_path()));
  assert(materialized.path_role() == pandapi::runtime::InputPathRole::GrounderHtn);
  assert(pandapi::runtime::path_role_name(materialized.path_role()) == "htn");
  assert(file_exists(path));
  assert(read_file(path) == "parser artifact\nline 2\n");
  assert(materialized.cleanup());
  assert(!file_exists(path));
}

void materialized_stdin_cleans_up_on_destruction()
{
  std::string path;
  {
    std::istringstream input{"engine artifact\n"};
    auto result = pandapi::runtime::materialize_stdin(
        input,
        pandapi::runtime::StdinMaterializationRequest{
            pandapi::runtime::Component::Engine,
            pandapi::runtime::InputPathRole::EngineSas, "pandapi-runtime-stdin-smoke"});

    assert(result.has_value());
    auto materialized = std::move(result.value());
    path = materialized.path();
    assert(file_exists(path));
  }

  assert(!file_exists(path));
}

void records_status_fields_for_stdin_path_roles()
{
  const auto fields = pandapi::runtime::stdin_status_fields(
      pandapi::runtime::InputPathRole::ParserDomain,
      pandapi::runtime::StdinOperation::Read);

  assert(fields.size() == 3);
  assert(fields[0].key == "path_role");
  assert(fields[0].value == "domain");
  assert(fields[1].key == "path");
  assert(fields[1].value == "-");
  assert(fields[2].key == "operation");
  assert(fields[2].value == "read");

  const auto status = pandapi::runtime::ProcessStatus::from_code(
      pandapi::runtime::StatusCode::OutputUnavailable,
      pandapi::runtime::Component::Parser,
      pandapi::runtime::SurfaceDisposition::Supported);
  assert(pandapi::runtime::stdin_failure_operation(status) ==
         pandapi::runtime::StdinOperation::Materialize);
  assert(pandapi::runtime::stdin_operation_name(
             pandapi::runtime::StdinOperation::Cleanup) == "cleanup");
}

} // namespace

int main()
{
  materializes_stdin_and_cleans_up();
  materialized_stdin_cleans_up_on_destruction();
  records_status_fields_for_stdin_path_roles();
  return 0;
}
