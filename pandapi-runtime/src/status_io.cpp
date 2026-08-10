#include "pandapi/runtime/status_io.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace pandapi::runtime {
namespace {

constexpr std::string_view status_tag = "PANDAPI_STATUS";

[[nodiscard]] ProcessStatus input_invalid_status() noexcept
{
  return ProcessStatus::from_code(StatusCode::InputInvalid, Component::Parser,
                                  SurfaceDisposition::Supported);
}

[[nodiscard]] bool is_lower_alpha(char value) noexcept
{
  return value >= 'a' && value <= 'z';
}

[[nodiscard]] bool is_digit(char value) noexcept
{
  return value >= '0' && value <= '9';
}

[[nodiscard]] bool is_field_key(std::string_view key) noexcept
{
  if (key.empty() || !is_lower_alpha(key.front())) {
    return false;
  }

  return std::all_of(key.begin(), key.end(), [](char value) noexcept {
    return is_lower_alpha(value) || is_digit(value) || value == '_';
  });
}

[[nodiscard]] bool is_reserved_field(std::string_view key) noexcept
{
  return key == "status" || key == "component" || key == "surface" ||
         key == "surface_disposition" || key == "exit_code" || key == "class" ||
         key == "signal_number" || key == "partial_output_policy";
}

[[nodiscard]] bool is_surface_name(std::string_view value) noexcept
{
  return is_field_key(value);
}

[[nodiscard]] std::string lowercase_ascii(std::string_view value)
{
  std::string lowered;
  lowered.reserve(value.size());
  for (char character : value) {
    lowered.push_back(
        static_cast<char>(std::tolower(static_cast<unsigned char>(character))));
  }
  return lowered;
}

[[nodiscard]] bool contains_placeholder_prose(std::string_view value)
{
  const auto lowered = lowercase_ascii(value);
  return lowered.find("placeholder") != std::string::npos ||
         lowered.find("todo") != std::string::npos ||
         lowered.find("tbd") != std::string::npos ||
         lowered.find("n/a") != std::string::npos;
}

[[nodiscard]] bool contains_ansi_escape(std::string_view value) noexcept
{
  return value.find('\x1b') != std::string_view::npos;
}

[[nodiscard]] bool is_valid_field_value(std::string_view value)
{
  return !value.empty() && !contains_ansi_escape(value) &&
         !contains_placeholder_prose(value);
}

[[nodiscard]] StatusResult<std::string> unescape_status_value(std::string_view value)
{
  if (contains_ansi_escape(value)) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }

  std::string unescaped;
  unescaped.reserve(value.size());
  bool escaping = false;

  for (char character : value) {
    if (escaping) {
      switch (character) {
      case 'n':
        unescaped.push_back('\n');
        break;
      case 'r':
        unescaped.push_back('\r');
        break;
      case 't':
        unescaped.push_back('\t');
        break;
      case '\\':
        unescaped.push_back('\\');
        break;
      default:
        return StatusResult<std::string>::failure(input_invalid_status());
      }
      escaping = false;
      continue;
    }

    if (character == '\\') {
      escaping = true;
      continue;
    }

    unescaped.push_back(character);
  }

  if (escaping || !is_valid_field_value(unescaped)) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }

  return StatusResult<std::string>::success(std::move(unescaped));
}

[[nodiscard]] bool parse_int(std::string_view value, int& parsed) noexcept
{
  if (value.empty()) {
    return false;
  }

  const auto has_only_digits =
      std::all_of(value.begin(), value.end(),
                  [](char character) noexcept { return is_digit(character); });
  if (!has_only_digits) {
    return false;
  }

  int result = 0;
  const auto parse_result =
      std::from_chars(value.data(), value.data() + value.size(), result);
  if (parse_result.ec != std::errc{} ||
      parse_result.ptr != value.data() + value.size()) {
    return false;
  }

  parsed = result;
  return true;
}

[[nodiscard]] StatusResult<StatusCode> parse_status_code(std::string_view value)
{
  if (value == "ok") {
    return StatusResult<StatusCode>::success(StatusCode::Ok);
  }
  if (value == "domain_no_plan") {
    return StatusResult<StatusCode>::success(StatusCode::DomainNoPlan);
  }
  if (value == "cli_usage_error") {
    return StatusResult<StatusCode>::success(StatusCode::CliUsageError);
  }
  if (value == "input_unavailable") {
    return StatusResult<StatusCode>::success(StatusCode::InputUnavailable);
  }
  if (value == "output_unavailable") {
    return StatusResult<StatusCode>::success(StatusCode::OutputUnavailable);
  }
  if (value == "input_invalid") {
    return StatusResult<StatusCode>::success(StatusCode::InputInvalid);
  }
  if (value == "unsupported_feature") {
    return StatusResult<StatusCode>::success(StatusCode::UnsupportedFeature);
  }
  if (value == "legacy_surface") {
    return StatusResult<StatusCode>::success(StatusCode::LegacySurface);
  }
  if (value == "experimental_surface") {
    return StatusResult<StatusCode>::success(StatusCode::ExperimentalSurface);
  }
  if (value == "future_surface") {
    return StatusResult<StatusCode>::success(StatusCode::FutureSurface);
  }
  if (value == "timeout") {
    return StatusResult<StatusCode>::success(StatusCode::Timeout);
  }
  if (value == "resource_limit") {
    return StatusResult<StatusCode>::success(StatusCode::ResourceLimit);
  }
  if (value == "interrupted") {
    return StatusResult<StatusCode>::success(StatusCode::Interrupted);
  }
  if (value == "dependency_failure") {
    return StatusResult<StatusCode>::success(StatusCode::DependencyFailure);
  }
  if (value == "child_process_failure") {
    return StatusResult<StatusCode>::success(StatusCode::ChildProcessFailure);
  }
  if (value == "internal_error") {
    return StatusResult<StatusCode>::success(StatusCode::InternalError);
  }
  if (value == "signal_terminated") {
    return StatusResult<StatusCode>::success(StatusCode::SignalTerminated);
  }

  return StatusResult<StatusCode>::failure(input_invalid_status());
}

[[nodiscard]] StatusResult<Component> parse_component(std::string_view value)
{
  if (value == "parser") {
    return StatusResult<Component>::success(Component::Parser);
  }
  if (value == "grounder") {
    return StatusResult<Component>::success(Component::Grounder);
  }
  if (value == "engine") {
    return StatusResult<Component>::success(Component::Engine);
  }

  return StatusResult<Component>::failure(input_invalid_status());
}

[[nodiscard]] StatusResult<SurfaceDisposition>
parse_surface_disposition(std::string_view value)
{
  if (value == "supported") {
    return StatusResult<SurfaceDisposition>::success(SurfaceDisposition::Supported);
  }
  if (value == "legacy") {
    return StatusResult<SurfaceDisposition>::success(SurfaceDisposition::Legacy);
  }
  if (value == "experimental") {
    return StatusResult<SurfaceDisposition>::success(SurfaceDisposition::Experimental);
  }
  if (value == "unsupported") {
    return StatusResult<SurfaceDisposition>::success(SurfaceDisposition::Unsupported);
  }
  if (value == "future") {
    return StatusResult<SurfaceDisposition>::success(SurfaceDisposition::Future);
  }

  return StatusResult<SurfaceDisposition>::failure(input_invalid_status());
}

struct ParsedField {
  std::string key;
  std::string value;
};

[[nodiscard]] StatusResult<std::vector<ParsedField>> parse_fields(std::string_view line)
{
  if (line.find('\n') != std::string_view::npos ||
      line.find('\r') != std::string_view::npos || contains_ansi_escape(line)) {
    return StatusResult<std::vector<ParsedField>>::failure(input_invalid_status());
  }

  if (line.substr(0, status_tag.size()) != status_tag) {
    return StatusResult<std::vector<ParsedField>>::failure(input_invalid_status());
  }

  std::vector<ParsedField> fields;
  auto cursor = status_tag.size();
  if (cursor == line.size()) {
    return StatusResult<std::vector<ParsedField>>::failure(input_invalid_status());
  }

  while (cursor < line.size()) {
    if (line[cursor] != '\t') {
      return StatusResult<std::vector<ParsedField>>::failure(input_invalid_status());
    }
    ++cursor;

    const auto next = line.find('\t', cursor);
    const auto token =
        line.substr(cursor, next == std::string_view::npos ? std::string_view::npos
                                                           : next - cursor);
    const auto separator = token.find('=');
    if (separator == std::string_view::npos) {
      return StatusResult<std::vector<ParsedField>>::failure(input_invalid_status());
    }

    const auto key = token.substr(0, separator);
    const auto escaped_value = token.substr(separator + 1);
    if (!is_field_key(key)) {
      return StatusResult<std::vector<ParsedField>>::failure(input_invalid_status());
    }

    const auto existing =
        std::find_if(fields.begin(), fields.end(),
                     [key](const ParsedField& field) { return field.key == key; });
    if (existing != fields.end()) {
      return StatusResult<std::vector<ParsedField>>::failure(input_invalid_status());
    }

    auto unescaped = unescape_status_value(escaped_value);
    if (!unescaped.has_value()) {
      return StatusResult<std::vector<ParsedField>>::failure(unescaped.status());
    }

    fields.push_back(ParsedField{std::string{key}, std::move(unescaped.value())});

    if (next == std::string_view::npos) {
      break;
    }
    cursor = next;
  }

  return StatusResult<std::vector<ParsedField>>::success(std::move(fields));
}

[[nodiscard]] const ParsedField* find_field(const std::vector<ParsedField>& fields,
                                            std::string_view key) noexcept
{
  const auto match = std::find_if(
      fields.begin(), fields.end(),
      [key](const ParsedField& field) noexcept { return field.key == key; });
  if (match == fields.end()) {
    return nullptr;
  }
  return &(*match);
}

[[nodiscard]] bool required_field(const std::vector<ParsedField>& fields,
                                  std::string_view key, std::string& value)
{
  const auto* field = find_field(fields, key);
  if (field == nullptr) {
    return false;
  }
  value = field->value;
  return true;
}

[[nodiscard]] bool append_field(std::ostringstream& output, std::string_view key,
                                std::string_view value)
{
  auto escaped = escape_status_value(value);
  if (!escaped.has_value()) {
    return false;
  }

  output << '\t' << key << '=' << escaped.value();
  return true;
}

} // namespace

StatusRecord::StatusRecord(ProcessStatus process_status, std::string surface)
    : process_status_{process_status}, surface_{std::move(surface)}, fields_{},
      partial_output_policy_{PartialOutputPolicy::Unknown},
      has_partial_output_policy_{false}
{
}

StatusResult<StatusRecord> StatusRecord::create(ProcessStatus process_status,
                                                std::string surface)
{
  if (!is_surface_name(surface) || !is_valid_field_value(surface)) {
    return StatusResult<StatusRecord>::failure(input_invalid_status());
  }

  return StatusResult<StatusRecord>::success(
      StatusRecord{process_status, std::move(surface)});
}

ProcessStatus StatusRecord::process_status() const noexcept { return process_status_; }

std::string_view StatusRecord::surface() const noexcept { return surface_; }

const std::vector<StatusField>& StatusRecord::fields() const noexcept
{
  return fields_;
}

bool StatusRecord::add_field(std::string key, std::string value)
{
  if (!is_field_key(key) || is_reserved_field(key) || !is_valid_field_value(value)) {
    return false;
  }

  const auto existing =
      std::find_if(fields_.begin(), fields_.end(),
                   [&key](const StatusField& field) { return field.key == key; });
  if (existing != fields_.end()) {
    return false;
  }

  fields_.push_back(StatusField{std::move(key), std::move(value)});
  return true;
}

void StatusRecord::set_partial_output_policy(PartialOutputPolicy policy) noexcept
{
  partial_output_policy_ = policy;
  has_partial_output_policy_ = true;
}

bool StatusRecord::has_partial_output_policy() const noexcept
{
  return has_partial_output_policy_;
}

PartialOutputPolicy StatusRecord::partial_output_policy() const noexcept
{
  return partial_output_policy_;
}

std::string_view status_stream_name(StatusStream stream) noexcept
{
  switch (stream) {
  case StatusStream::Stderr:
    return "stderr";
  case StatusStream::Stdout:
    return "stdout";
  }

  return "stderr";
}

bool status_stream_allowed(StatusStream stream, OutputRole stdout_role) noexcept
{
  if (stream == StatusStream::Stderr) {
    return true;
  }

  return stdout_role == OutputRole::Empty || stdout_role == OutputRole::TaggedStatus;
}

std::string_view partial_output_policy_name(PartialOutputPolicy policy) noexcept
{
  switch (policy) {
  case PartialOutputPolicy::Absent:
    return "absent";
  case PartialOutputPolicy::Retained:
    return "retained";
  case PartialOutputPolicy::Discarded:
    return "discarded";
  case PartialOutputPolicy::Complete:
    return "complete";
  case PartialOutputPolicy::Unknown:
    return "unknown";
  }

  return "unknown";
}

StatusResult<PartialOutputPolicy> parse_partial_output_policy(std::string_view value)
{
  if (value == "absent") {
    return StatusResult<PartialOutputPolicy>::success(PartialOutputPolicy::Absent);
  }
  if (value == "retained") {
    return StatusResult<PartialOutputPolicy>::success(PartialOutputPolicy::Retained);
  }
  if (value == "discarded") {
    return StatusResult<PartialOutputPolicy>::success(PartialOutputPolicy::Discarded);
  }
  if (value == "complete") {
    return StatusResult<PartialOutputPolicy>::success(PartialOutputPolicy::Complete);
  }
  if (value == "unknown") {
    return StatusResult<PartialOutputPolicy>::success(PartialOutputPolicy::Unknown);
  }

  return StatusResult<PartialOutputPolicy>::failure(input_invalid_status());
}

bool is_status_value_single_line(std::string_view value) noexcept
{
  return value.find('\n') == std::string_view::npos &&
         value.find('\r') == std::string_view::npos &&
         value.find('\t') == std::string_view::npos && !contains_ansi_escape(value);
}

StatusResult<std::string> escape_status_value(std::string_view value)
{
  if (!is_valid_field_value(value)) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }

  std::string escaped;
  escaped.reserve(value.size());

  for (char character : value) {
    switch (character) {
    case '\n':
      escaped += "\\n";
      break;
    case '\r':
      escaped += "\\r";
      break;
    case '\t':
      escaped += "\\t";
      break;
    case '\\':
      escaped += "\\\\";
      break;
    default:
      escaped.push_back(character);
      break;
    }
  }

  return StatusResult<std::string>::success(std::move(escaped));
}

StatusResult<std::string> serialize_status_record(const StatusRecord& record)
{
  std::ostringstream output;
  const auto process_status = record.process_status();

  output << status_tag;
  if (!append_field(output, "status", status_name(process_status))) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }
  if (!append_field(output, "component", component_name(process_status.component()))) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }
  if (!append_field(output, "surface", record.surface())) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }
  if (!append_field(output, "surface_disposition",
                    surface_disposition_name(process_status.surface_disposition()))) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }
  if (!append_field(output, "exit_code", std::to_string(exit_code(process_status)))) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }
  if (!append_field(output, "class", status_class_name(status_class(process_status)))) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }

  if (process_status.is_signal_terminated()) {
    if (!append_field(output, "signal_number",
                      std::to_string(process_status.signal_number()))) {
      return StatusResult<std::string>::failure(input_invalid_status());
    }
  }

  if (record.has_partial_output_policy()) {
    if (!append_field(output, "partial_output_policy",
                      partial_output_policy_name(record.partial_output_policy()))) {
      return StatusResult<std::string>::failure(input_invalid_status());
    }
  }

  for (const auto& field : record.fields()) {
    if (!append_field(output, field.key, field.value)) {
      return StatusResult<std::string>::failure(input_invalid_status());
    }
  }

  return StatusResult<std::string>::success(output.str());
}

bool write_status_record(std::ostream& output, const StatusRecord& record)
{
  auto serialized = serialize_status_record(record);
  if (!serialized.has_value()) {
    return false;
  }

  output << serialized.value() << '\n';
  output.flush();
  return output.good();
}

StatusResult<StatusRecord> parse_status_record(std::string_view line)
{
  auto parsed_fields = parse_fields(line);
  if (!parsed_fields.has_value()) {
    return StatusResult<StatusRecord>::failure(parsed_fields.status());
  }

  std::string status_value;
  std::string component_value;
  std::string surface_value;
  std::string surface_disposition_value;
  std::string exit_code_value;
  std::string class_value;

  if (!required_field(parsed_fields.value(), "status", status_value) ||
      !required_field(parsed_fields.value(), "component", component_value) ||
      !required_field(parsed_fields.value(), "surface", surface_value) ||
      !required_field(parsed_fields.value(), "surface_disposition",
                      surface_disposition_value) ||
      !required_field(parsed_fields.value(), "exit_code", exit_code_value) ||
      !required_field(parsed_fields.value(), "class", class_value)) {
    return StatusResult<StatusRecord>::failure(input_invalid_status());
  }

  auto parsed_status_code = parse_status_code(status_value);
  auto parsed_component = parse_component(component_value);
  auto parsed_disposition = parse_surface_disposition(surface_disposition_value);
  if (!parsed_status_code.has_value() || !parsed_component.has_value() ||
      !parsed_disposition.has_value()) {
    return StatusResult<StatusRecord>::failure(input_invalid_status());
  }

  int parsed_exit_code = 0;
  if (!parse_int(exit_code_value, parsed_exit_code)) {
    return StatusResult<StatusRecord>::failure(input_invalid_status());
  }

  ProcessStatus process_status = ProcessStatus::from_code(
      parsed_status_code.value(), parsed_component.value(), parsed_disposition.value());

  const auto* signal_number = find_field(parsed_fields.value(), "signal_number");
  if (parsed_status_code.value() == StatusCode::SignalTerminated) {
    int parsed_signal_number = 0;
    if (signal_number == nullptr ||
        !parse_int(signal_number->value, parsed_signal_number) ||
        parsed_signal_number <= 0) {
      return StatusResult<StatusRecord>::failure(input_invalid_status());
    }

    process_status = ProcessStatus::signal_terminated(
        parsed_signal_number, parsed_component.value(), parsed_disposition.value());
  } else if (signal_number != nullptr) {
    return StatusResult<StatusRecord>::failure(input_invalid_status());
  }

  if (parsed_exit_code != exit_code(process_status) ||
      class_value != status_class_name(status_class(process_status))) {
    return StatusResult<StatusRecord>::failure(input_invalid_status());
  }

  auto record = StatusRecord::create(process_status, std::move(surface_value));
  if (!record.has_value()) {
    return StatusResult<StatusRecord>::failure(record.status());
  }

  const auto* partial_output =
      find_field(parsed_fields.value(), "partial_output_policy");
  if (partial_output != nullptr) {
    auto policy = parse_partial_output_policy(partial_output->value);
    if (!policy.has_value()) {
      return StatusResult<StatusRecord>::failure(policy.status());
    }
    record.value().set_partial_output_policy(policy.value());
  }

  for (const auto& field : parsed_fields.value()) {
    if (!is_reserved_field(field.key)) {
      if (!record.value().add_field(field.key, field.value)) {
        return StatusResult<StatusRecord>::failure(input_invalid_status());
      }
    }
  }

  return StatusResult<StatusRecord>::success(std::move(record.value()));
}

} // namespace pandapi::runtime
