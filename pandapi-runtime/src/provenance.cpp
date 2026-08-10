#include "pandapi/runtime/provenance.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
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

[[nodiscard]] bool contains_ansi_escape(std::string_view value) noexcept
{
  return value.find('\x1b') != std::string_view::npos;
}

[[nodiscard]] bool is_single_line(std::string_view value) noexcept
{
  return value.find('\n') == std::string_view::npos
    && value.find('\r') == std::string_view::npos
    && value.find('\t') == std::string_view::npos;
}

[[nodiscard]] std::string lowercase_ascii(std::string_view value)
{
  std::string lowered;
  lowered.reserve(value.size());
  for (char character : value) {
    lowered.push_back(static_cast<char>(std::tolower(
      static_cast<unsigned char>(character))));
  }
  return lowered;
}

[[nodiscard]] bool contains_unknown_placeholder(std::string_view value)
{
  const auto lowered = lowercase_ascii(value);
  return lowered == "unknown" || lowered == "placeholder" || lowered == "todo"
    || lowered == "tbd" || lowered == "n/a";
}

[[nodiscard]] bool valid_provenance_value(std::string_view value)
{
  return !value.empty() && is_single_line(value) && !contains_ansi_escape(value)
    && !contains_unknown_placeholder(value);
}

[[nodiscard]] bool valid_field_name(std::string_view name) noexcept
{
  return !name.empty()
    && std::all_of(name.begin(), name.end(), [](char character) noexcept {
         return (character >= 'a' && character <= 'z')
           || (character >= '0' && character <= '9') || character == '_';
       });
}

[[nodiscard]] StatusResult<bool> add_field_if_present(
  std::vector<ProvenanceField>& fields,
  std::string name,
  std::string value,
  Component component,
  SurfaceDisposition surface_disposition)
{
  if (value.empty()) {
    return StatusResult<bool>::success(true);
  }
  if (!valid_field_name(name) || !valid_provenance_value(value)) {
    return StatusResult<bool>::failure(
      cli_usage_error_status(component, surface_disposition));
  }

  fields.push_back(ProvenanceField{std::move(name), std::move(value)});
  return StatusResult<bool>::success(true);
}

[[nodiscard]] StatusResult<std::string> render_fields(
  const std::vector<ProvenanceField>& fields,
  Component component,
  SurfaceDisposition surface_disposition)
{
  if (fields.empty()) {
    return StatusResult<std::string>::failure(
      cli_usage_error_status(component, surface_disposition));
  }

  std::ostringstream output;
  for (const auto& field : fields) {
    output << field.name << '=' << field.value << '\n';
  }

  return StatusResult<std::string>::success(output.str());
}

}  // namespace

std::string_view provenance_record_kind() noexcept
{
  return "stable field";
}

StatusResult<std::vector<ProvenanceField>> version_fields(
  const ProvenanceRecord& record,
  Component component,
  SurfaceDisposition surface_disposition)
{
  std::vector<ProvenanceField> fields;

  const auto add_canonical_command = add_field_if_present(
    fields,
    "canonical_command",
    record.canonical_command,
    component,
    surface_disposition);
  if (!add_canonical_command.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_canonical_command.status());
  }

  const auto add_invoked_command = add_field_if_present(
    fields,
    "invoked_command",
    record.invoked_command,
    component,
    surface_disposition);
  if (!add_invoked_command.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_invoked_command.status());
  }

  const auto add_component = add_field_if_present(
    fields,
    "component",
    record.component,
    component,
    surface_disposition);
  if (!add_component.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_component.status());
  }

  const auto add_chengdu_version = add_field_if_present(
    fields,
    "chengdu_version",
    record.chengdu_version,
    component,
    surface_disposition);
  if (!add_chengdu_version.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_chengdu_version.status());
  }

  const auto add_contract_version = add_field_if_present(
    fields,
    "contract_version",
    record.contract_version,
    component,
    surface_disposition);
  if (!add_contract_version.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_contract_version.status());
  }

  const auto add_upstream_project = add_field_if_present(
    fields,
    "upstream_project",
    record.upstream_project,
    component,
    surface_disposition);
  if (!add_upstream_project.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_upstream_project.status());
  }

  const auto add_upstream_commit = add_field_if_present(
    fields,
    "upstream_commit",
    record.upstream_commit,
    component,
    surface_disposition);
  if (!add_upstream_commit.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_upstream_commit.status());
  }

  const auto add_source_prefix = add_field_if_present(
    fields,
    "source_prefix",
    record.source_prefix,
    component,
    surface_disposition);
  if (!add_source_prefix.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_source_prefix.status());
  }

  const auto add_build_commit = add_field_if_present(
    fields,
    "build_commit",
    record.build_commit,
    component,
    surface_disposition);
  if (!add_build_commit.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_build_commit.status());
  }

  const auto add_platform = add_field_if_present(
    fields,
    "platform",
    record.platform,
    component,
    surface_disposition);
  if (!add_platform.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_platform.status());
  }

  const auto add_compiler = add_field_if_present(
    fields,
    "compiler",
    record.compiler,
    component,
    surface_disposition);
  if (!add_compiler.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_compiler.status());
  }

  const auto add_license = add_field_if_present(
    fields,
    "license",
    record.license,
    component,
    surface_disposition);
  if (!add_license.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_license.status());
  }

  const auto add_notice = add_field_if_present(
    fields,
    "notice",
    record.notice,
    component,
    surface_disposition);
  if (!add_notice.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_notice.status());
  }

  return StatusResult<std::vector<ProvenanceField>>::success(std::move(fields));
}

StatusResult<std::vector<ProvenanceField>> provenance_fields(
  const ProvenanceRecord& record,
  Component component,
  SurfaceDisposition surface_disposition)
{
  auto fields_result = version_fields(record, component, surface_disposition);
  if (!fields_result.has_value()) {
    return fields_result;
  }

  auto fields = fields_result.value();
  const auto add_import_commit = add_field_if_present(
    fields,
    "import_commit",
    record.import_commit,
    component,
    surface_disposition);
  if (!add_import_commit.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_import_commit.status());
  }

  const auto add_build_timestamp = add_field_if_present(
    fields,
    "build_timestamp",
    record.build_timestamp,
    component,
    surface_disposition);
  if (!add_build_timestamp.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_build_timestamp.status());
  }

  const auto add_third_party_licenses = add_field_if_present(
    fields,
    "third_party_licenses",
    record.third_party_licenses,
    component,
    surface_disposition);
  if (!add_third_party_licenses.has_value()) {
    return StatusResult<std::vector<ProvenanceField>>::failure(
      add_third_party_licenses.status());
  }

  return StatusResult<std::vector<ProvenanceField>>::success(std::move(fields));
}

StatusResult<std::string> format_version(
  const ProvenanceRecord& record,
  Component component,
  SurfaceDisposition surface_disposition)
{
  const auto fields_result = version_fields(record, component, surface_disposition);
  if (!fields_result.has_value()) {
    return StatusResult<std::string>::failure(fields_result.status());
  }

  return render_fields(fields_result.value(), component, surface_disposition);
}

StatusResult<std::string> format_provenance(
  const ProvenanceRecord& record,
  Component component,
  SurfaceDisposition surface_disposition)
{
  const auto fields_result = provenance_fields(
    record,
    component,
    surface_disposition);
  if (!fields_result.has_value()) {
    return StatusResult<std::string>::failure(fields_result.status());
  }

  return render_fields(fields_result.value(), component, surface_disposition);
}

}  // namespace pandapi::runtime
