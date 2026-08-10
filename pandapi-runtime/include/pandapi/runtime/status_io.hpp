#pragma once

#include "pandapi/runtime/result.hpp"
#include "pandapi/runtime/status.hpp"

#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace pandapi::runtime {

enum class StatusStream {
  Stderr,
  Stdout,
};

enum class PartialOutputPolicy {
  Absent,
  Retained,
  Discarded,
  Complete,
  Unknown,
};

struct StatusField {
  std::string key;
  std::string value;
};

class StatusRecord {
public:
  [[nodiscard]] static StatusResult<StatusRecord> create(
    ProcessStatus process_status,
    std::string surface);

  [[nodiscard]] ProcessStatus process_status() const noexcept;
  [[nodiscard]] std::string_view surface() const noexcept;
  [[nodiscard]] const std::vector<StatusField>& fields() const noexcept;

  [[nodiscard]] bool add_field(std::string key, std::string value);
  void set_partial_output_policy(PartialOutputPolicy policy) noexcept;
  [[nodiscard]] bool has_partial_output_policy() const noexcept;
  [[nodiscard]] PartialOutputPolicy partial_output_policy() const noexcept;

private:
  StatusRecord(ProcessStatus process_status, std::string surface);

  ProcessStatus process_status_;
  std::string surface_;
  std::vector<StatusField> fields_;
  PartialOutputPolicy partial_output_policy_;
  bool has_partial_output_policy_;
};

[[nodiscard]] std::string_view status_stream_name(StatusStream stream) noexcept;
[[nodiscard]] bool status_stream_allowed(
  StatusStream stream,
  OutputRole stdout_role) noexcept;
[[nodiscard]] std::string_view partial_output_policy_name(
  PartialOutputPolicy policy) noexcept;
[[nodiscard]] StatusResult<PartialOutputPolicy> parse_partial_output_policy(
  std::string_view value);

[[nodiscard]] bool is_status_value_single_line(std::string_view value) noexcept;
[[nodiscard]] StatusResult<std::string> escape_status_value(std::string_view value);
[[nodiscard]] StatusResult<std::string> serialize_status_record(
  const StatusRecord& record);
[[nodiscard]] bool write_status_record(std::ostream& output, const StatusRecord& record);
[[nodiscard]] StatusResult<StatusRecord> parse_status_record(std::string_view line);

}  // namespace pandapi::runtime
