#include "pandapi/runtime/normalization.hpp"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <sstream>
#include <string>
#include <utility>

namespace pandapi::runtime {
namespace {

[[nodiscard]] ProcessStatus input_invalid_status() noexcept
{
  return ProcessStatus::from_code(StatusCode::InputInvalid, Component::Engine,
                                  SurfaceDisposition::Supported);
}

[[nodiscard]] bool is_digit(char value) noexcept
{
  return value >= '0' && value <= '9';
}

[[nodiscard]] bool replace_all(std::string& text, std::string_view needle,
                               std::string_view replacement)
{
  if (needle.empty()) {
    return false;
  }

  bool changed = false;
  std::string::size_type position = 0;
  while ((position = text.find(needle, position)) != std::string::npos) {
    text.replace(position, needle.size(), replacement);
    position += replacement.size();
    changed = true;
  }
  return changed;
}

} // namespace

std::string_view path_token_name(PathToken token) noexcept
{
  switch (token) {
  case PathToken::FixtureRoot:
    return "FixtureRoot";
  case PathToken::BuildRoot:
    return "BuildRoot";
  case PathToken::TempRoot:
    return "TempRoot";
  case PathToken::ReleaseRoot:
    return "ReleaseRoot";
  }

  return "TempRoot";
}

std::string token_spelling(PathToken token)
{
  std::string token_text = "$";
  token_text += path_token_name(token);
  return token_text;
}

bool contains_ansi_escape(std::string_view text) noexcept
{
  return text.find('\x1b') != std::string_view::npos;
}

std::string strip_ansi_sequences(std::string_view text)
{
  std::string stripped;
  stripped.reserve(text.size());

  for (std::string::size_type index = 0; index < text.size(); ++index) {
    if (text[index] != '\x1b') {
      stripped.push_back(text[index]);
      continue;
    }

    if (index + 1 < text.size() && text[index + 1] == '[') {
      index += 2;
      while (index < text.size() &&
             (std::isdigit(static_cast<unsigned char>(text[index])) ||
              text[index] == ';')) {
        ++index;
      }
      continue;
    }
  }

  return stripped;
}

std::string normalize_line_endings_to_lf(std::string_view text)
{
  std::string normalized;
  normalized.reserve(text.size());
  for (std::string::size_type index = 0; index < text.size(); ++index) {
    const auto character = text[index];
    if (character == '\r') {
      if (index + 1 < text.size() && text[index + 1] == '\n') {
        continue;
      }
      normalized.push_back('\n');
      continue;
    }
    normalized.push_back(character);
  }
  return normalized;
}

std::string normalize_paths(std::string_view text,
                            const std::vector<PathReplacement>& replacements)
{
  std::string normalized{text};
  for (const auto& replacement : replacements) {
    static_cast<void>(
        replace_all(normalized, replacement.path, token_spelling(replacement.token)));
  }
  return normalized;
}

StatusResult<std::string> normalize_text(std::string_view text,
                                         const NormalizationRules& rules)
{
  if (rules.reject_ansi && !rules.strip_ansi && contains_ansi_escape(text)) {
    return StatusResult<std::string>::failure(input_invalid_status());
  }

  std::string normalized{text};
  if (rules.strip_ansi) {
    normalized = strip_ansi_sequences(normalized);
  }
  if (rules.normalize_line_endings) {
    normalized = normalize_line_endings_to_lf(normalized);
  }
  normalized = normalize_paths(normalized, rules.path_tokens);

  return StatusResult<std::string>::success(std::move(normalized));
}

bool duration_is_non_negative_ms(std::string_view duration) noexcept
{
  if (duration.empty()) {
    return false;
  }
  return std::all_of(duration.begin(), duration.end(), is_digit);
}

bool duration_within_bound(std::chrono::milliseconds duration,
                           std::chrono::milliseconds bound) noexcept
{
  return duration.count() >= 0 && duration <= bound;
}

bool timestamp_matches_rfc3339_like(std::string_view timestamp) noexcept
{
  if (timestamp.size() < 20) {
    return false;
  }
  return is_digit(timestamp[0]) && is_digit(timestamp[1]) && is_digit(timestamp[2]) &&
         is_digit(timestamp[3]) && timestamp[4] == '-' && is_digit(timestamp[5]) &&
         is_digit(timestamp[6]) && timestamp[7] == '-' && is_digit(timestamp[8]) &&
         is_digit(timestamp[9]) && timestamp[10] == 'T' && is_digit(timestamp[11]) &&
         is_digit(timestamp[12]) && timestamp[13] == ':' && is_digit(timestamp[14]) &&
         is_digit(timestamp[15]) && timestamp[16] == ':' && is_digit(timestamp[17]) &&
         is_digit(timestamp[18]) && timestamp.back() == 'Z';
}

bool build_metadata_present(std::string_view build_metadata) noexcept
{
  return !build_metadata.empty();
}

std::string errno_name(int errno_value)
{
  switch (errno_value) {
  case ENOENT:
    return "ENOENT";
  case EACCES:
    return "EACCES";
  case EISDIR:
    return "EISDIR";
  case ENOTDIR:
    return "ENOTDIR";
  default:
    return "ERRNO_" + std::to_string(errno_value);
  }
}

bool errno_matches_symbolic(std::string_view observed, int errno_value)
{
  return observed == errno_name(errno_value) || observed == std::to_string(errno_value);
}

std::string signal_name(int signal_number)
{
  switch (signal_number) {
  case SIGINT:
    return "SIGINT";
  case SIGTERM:
    return "SIGTERM";
  case SIGKILL:
    return "SIGKILL";
  default:
    return "platform-normalized-signal";
  }
}

int supervisor_exit_code_for_signal(int signal_number) noexcept
{
  return 128 + signal_number;
}

ComparisonResult comparison_match(std::string field)
{
  ComparisonResult result;
  result.matches = true;
  result.field = std::move(field);
  return result;
}

ComparisonResult comparison_mismatch(std::string field, std::string expected,
                                     std::string actual, std::string mismatch)
{
  ComparisonResult result;
  result.matches = false;
  result.field = std::move(field);
  result.expected = std::move(expected);
  result.actual = std::move(actual);
  result.mismatch = std::move(mismatch);
  return result;
}

ComparisonResult compare_golden(std::string field, std::string_view expected,
                                std::string_view actual,
                                const NormalizationRules& rules)
{
  auto normalized_expected = normalize_text(expected, rules);
  auto normalized_actual = normalize_text(actual, rules);
  if (!normalized_expected.has_value() || !normalized_actual.has_value()) {
    return comparison_mismatch(std::move(field), std::string{expected},
                               std::string{actual}, "normalization failed");
  }

  if (normalized_expected.value() == normalized_actual.value()) {
    return comparison_match(std::move(field));
  }

  return comparison_mismatch(std::move(field), normalized_expected.value(),
                             normalized_actual.value(), "golden mismatch");
}

ComparisonResult compare_semantic_predicate(std::string field, bool predicate_matches,
                                            std::string expected, std::string actual)
{
  if (predicate_matches) {
    return comparison_match(std::move(field));
  }

  return comparison_mismatch(std::move(field), std::move(expected), std::move(actual),
                             "semantic predicate mismatch");
}

StatusResult<StatusRecord> parse_final_status(std::string_view line)
{
  return parse_status_record(line);
}

ComparisonResult match_status_record(const StatusRecord& record,
                                     StatusCode expected_status)
{
  if (record.process_status().code() == expected_status) {
    return comparison_match("PANDAPI_STATUS");
  }

  return comparison_mismatch(
      "PANDAPI_STATUS", std::string{status_name(expected_status)},
      std::string{status_name(record.process_status())}, "final status mismatch");
}

} // namespace pandapi::runtime
