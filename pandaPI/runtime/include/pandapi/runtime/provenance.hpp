#pragma once

#include "pandapi/runtime/result.hpp"
#include "pandapi/runtime/status.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace pandapi::runtime {

struct ProvenanceField {
  std::string name;
  std::string value;
};

struct ProvenanceRecord {
  std::string canonical_command;
  std::string invoked_command;
  std::string component;
  std::string chengdu_version;
  std::string contract_version;
  std::string upstream_project;
  std::string upstream_commit;
  std::string source_prefix;
  std::string build_commit;
  std::string platform;
  std::string compiler;
  std::string license;
  std::string notice;
  std::string import_commit;
  std::string build_timestamp;
  std::string third_party_licenses;
};

[[nodiscard]] std::string_view provenance_record_kind() noexcept;
[[nodiscard]] StatusResult<std::vector<ProvenanceField>>
version_fields(const ProvenanceRecord& record, Component component,
               SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] StatusResult<std::vector<ProvenanceField>> provenance_fields(
    const ProvenanceRecord& record, Component component,
    SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] StatusResult<std::string>
format_version(const ProvenanceRecord& record, Component component,
               SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);
[[nodiscard]] StatusResult<std::string> format_provenance(
    const ProvenanceRecord& record, Component component,
    SurfaceDisposition surface_disposition = SurfaceDisposition::Supported);

} // namespace pandapi::runtime
