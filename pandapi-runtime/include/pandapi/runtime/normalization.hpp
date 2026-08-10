#pragma once

#include "pandapi/runtime/result.hpp"
#include "pandapi/runtime/status.hpp"
#include "pandapi/runtime/status_io.hpp"

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

namespace pandapi::runtime {

enum class PathToken {
  FixtureRoot,
  BuildRoot,
  TempRoot,
  ReleaseRoot,
};

enum class ComparisonPolicy {
  Golden,
  Semantic,
  Predicate,
  Empty,
};

struct PathReplacement {
  PathToken token = PathToken::FixtureRoot;
  std::string path;
};

struct NormalizationRules {
  std::vector<PathReplacement> path_tokens;
  bool normalize_line_endings = true;
  bool reject_ansi = true;
  bool strip_ansi = false;
  bool duration_predicate_only = true;
  bool timestamp_predicate_only = true;
  bool build_metadata_predicate_only = true;
  bool errno_symbolic_preferred = true;
  bool signal_platform_normalized = true;
};

struct ComparisonResult {
  bool matches = false;
  std::string field;
  std::string expected;
  std::string actual;
  std::string mismatch;
};

[[nodiscard]] std::string_view path_token_name(PathToken token) noexcept;
[[nodiscard]] std::string token_spelling(PathToken token);
[[nodiscard]] bool contains_ansi_escape(std::string_view text) noexcept;
[[nodiscard]] std::string strip_ansi_sequences(std::string_view text);
[[nodiscard]] std::string normalize_line_endings_to_lf(std::string_view text);
[[nodiscard]] std::string normalize_paths(
  std::string_view text,
  const std::vector<PathReplacement>& replacements);
[[nodiscard]] StatusResult<std::string> normalize_text(
  std::string_view text,
  const NormalizationRules& rules);
[[nodiscard]] bool duration_is_non_negative_ms(std::string_view duration) noexcept;
[[nodiscard]] bool duration_within_bound(
  std::chrono::milliseconds duration,
  std::chrono::milliseconds bound) noexcept;
[[nodiscard]] bool timestamp_matches_rfc3339_like(std::string_view timestamp) noexcept;
[[nodiscard]] bool build_metadata_present(std::string_view build_metadata) noexcept;
[[nodiscard]] std::string errno_name(int errno_value);
[[nodiscard]] bool errno_matches_symbolic(
  std::string_view observed,
  int errno_value);
[[nodiscard]] std::string signal_name(int signal_number);
[[nodiscard]] int supervisor_exit_code_for_signal(int signal_number) noexcept;
[[nodiscard]] ComparisonResult comparison_match(std::string field);
[[nodiscard]] ComparisonResult comparison_mismatch(
  std::string field,
  std::string expected,
  std::string actual,
  std::string mismatch);
[[nodiscard]] ComparisonResult compare_golden(
  std::string field,
  std::string_view expected,
  std::string_view actual,
  const NormalizationRules& rules);
[[nodiscard]] ComparisonResult compare_semantic_predicate(
  std::string field,
  bool predicate_matches,
  std::string expected,
  std::string actual);
[[nodiscard]] StatusResult<StatusRecord> parse_final_status(
  std::string_view line);
[[nodiscard]] ComparisonResult match_status_record(
  const StatusRecord& record,
  StatusCode expected_status);

}  // namespace pandapi::runtime
