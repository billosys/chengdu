#include "pandapi/runtime/status_io.hpp"

#include <ostream>
#include <sstream>
#include <string>

namespace {

class FlushCountingBuffer : public std::stringbuf {
public:
  [[nodiscard]] int flush_count() const noexcept
  {
    return flush_count_;
  }

private:
  int sync() override
  {
    ++flush_count_;
    return std::stringbuf::sync();
  }

  int flush_count_ = 0;
};

[[nodiscard]] pandapi::runtime::ProcessStatus status(
  pandapi::runtime::StatusCode code)
{
  return pandapi::runtime::ProcessStatus::from_code(
    code,
    pandapi::runtime::Component::Engine,
    pandapi::runtime::SurfaceDisposition::Supported);
}

[[nodiscard]] bool check_round_trip_success()
{
  auto record = pandapi::runtime::StatusRecord::create(
    status(pandapi::runtime::StatusCode::Ok),
    "normal_search");
  if (!record.has_value()) {
    return false;
  }

  record.value().set_partial_output_policy(
    pandapi::runtime::PartialOutputPolicy::Complete);
  if (!record.value().add_field("duration_ms", "37")) {
    return false;
  }

  auto serialized = pandapi::runtime::serialize_status_record(record.value());
  if (!serialized.has_value()) {
    return false;
  }

  const std::string expected =
    "PANDAPI_STATUS\tstatus=ok\tcomponent=engine\tsurface=normal_search"
    "\tsurface_disposition=supported\texit_code=0\tclass=success"
    "\tpartial_output_policy=complete\tduration_ms=37";
  if (serialized.value() != expected) {
    return false;
  }

  auto parsed = pandapi::runtime::parse_status_record(serialized.value());
  return parsed.has_value()
    && pandapi::runtime::status_name(parsed.value().process_status()) == "ok"
    && parsed.value().surface() == "normal_search"
    && parsed.value().has_partial_output_policy()
    && parsed.value().partial_output_policy()
      == pandapi::runtime::PartialOutputPolicy::Complete
    && parsed.value().fields().size() == 1
    && parsed.value().fields().front().key == "duration_ms";
}

[[nodiscard]] bool check_round_trip_domain_no_plan()
{
  auto record = pandapi::runtime::StatusRecord::create(
    status(pandapi::runtime::StatusCode::DomainNoPlan),
    "normal_search");
  if (!record.has_value()) {
    return false;
  }

  record.value().set_partial_output_policy(
    pandapi::runtime::PartialOutputPolicy::Absent);
  if (!record.value().add_field("outcome_kind", "unsolvable")) {
    return false;
  }

  auto serialized = pandapi::runtime::serialize_status_record(record.value());
  if (!serialized.has_value()
      || serialized.value().find("domain_no_plan") == std::string::npos) {
    return false;
  }

  auto parsed = pandapi::runtime::parse_status_record(serialized.value());
  return parsed.has_value()
    && parsed.value().process_status().code()
      == pandapi::runtime::StatusCode::DomainNoPlan
    && pandapi::runtime::exit_code(parsed.value().process_status()) == 2;
}

[[nodiscard]] bool check_stream_legality()
{
  return pandapi::runtime::status_stream_name(
      pandapi::runtime::StatusStream::Stderr) == "stderr"
    && pandapi::runtime::status_stream_name(
      pandapi::runtime::StatusStream::Stdout) == "stdout"
    && pandapi::runtime::status_stream_allowed(
      pandapi::runtime::StatusStream::Stderr,
      pandapi::runtime::OutputRole::DataArtifact)
    && pandapi::runtime::status_stream_allowed(
      pandapi::runtime::StatusStream::Stdout,
      pandapi::runtime::OutputRole::Empty)
    && pandapi::runtime::status_stream_allowed(
      pandapi::runtime::StatusStream::Stdout,
      pandapi::runtime::OutputRole::TaggedStatus)
    && !pandapi::runtime::status_stream_allowed(
      pandapi::runtime::StatusStream::Stdout,
      pandapi::runtime::OutputRole::DataArtifact)
    && !pandapi::runtime::status_stream_allowed(
      pandapi::runtime::StatusStream::Stdout,
      pandapi::runtime::OutputRole::Informational);
}

[[nodiscard]] bool check_single_line_escape_and_reject()
{
  auto escaped = pandapi::runtime::escape_status_value("line\nwith\ttab");
  if (!escaped.has_value() || escaped.value() != "line\\nwith\\ttab") {
    return false;
  }
  if (!pandapi::runtime::is_status_value_single_line(escaped.value())) {
    return false;
  }

  auto record = pandapi::runtime::StatusRecord::create(
    status(pandapi::runtime::StatusCode::Ok),
    "normal_search");
  if (!record.has_value()) {
    return false;
  }
  if (!record.value().add_field("note", "line\nwith\ttab")) {
    return false;
  }
  if (record.value().add_field("ansi", "\x1b[31mred")) {
    return false;
  }
  if (record.value().add_field("finding_refs", "placeholder prose")) {
    return false;
  }

  auto serialized = pandapi::runtime::serialize_status_record(record.value());
  if (!serialized.has_value()) {
    return false;
  }

  const auto newline = serialized.value().find('\n');
  const auto escaped_newline = serialized.value().find("\\n");
  const auto escaped_tab = serialized.value().find("\\t");
  return newline == std::string::npos
    && escaped_newline != std::string::npos
    && escaped_tab != std::string::npos;
}

[[nodiscard]] bool check_invalid_input()
{
  auto missing_tag = pandapi::runtime::parse_status_record(
    "status=ok\tcomponent=engine");
  if (missing_tag.has_value()
      || missing_tag.status().code() != pandapi::runtime::StatusCode::InputInvalid) {
    return false;
  }

  auto raw_newline = pandapi::runtime::parse_status_record(
    "PANDAPI_STATUS\tstatus=ok\ncomponent=engine");
  if (raw_newline.has_value()
      || raw_newline.status().code() != pandapi::runtime::StatusCode::InputInvalid) {
    return false;
  }

  auto bad_exit = pandapi::runtime::parse_status_record(
    "PANDAPI_STATUS\tstatus=ok\tcomponent=engine\tsurface=normal_search"
    "\tsurface_disposition=supported\texit_code=1\tclass=success");
  if (bad_exit.has_value()
      || bad_exit.status().code() != pandapi::runtime::StatusCode::InputInvalid) {
    return false;
  }

  auto non_digit_exit = pandapi::runtime::parse_status_record(
    "PANDAPI_STATUS\tstatus=ok\tcomponent=engine\tsurface=normal_search"
    "\tsurface_disposition=supported\texit_code=not_a_number\tclass=success");
  if (non_digit_exit.has_value()
      || non_digit_exit.status().code()
        != pandapi::runtime::StatusCode::InputInvalid) {
    return false;
  }

  auto oversized_exit = pandapi::runtime::parse_status_record(
    "PANDAPI_STATUS\tstatus=ok\tcomponent=engine\tsurface=normal_search"
    "\tsurface_disposition=supported"
    "\texit_code=999999999999999999999999999999999999\tclass=success");
  if (oversized_exit.has_value()
      || oversized_exit.status().code()
        != pandapi::runtime::StatusCode::InputInvalid) {
    return false;
  }

  auto valid_signal = pandapi::runtime::parse_status_record(
    "PANDAPI_STATUS\tstatus=signal_terminated\tcomponent=engine"
    "\tsurface=normal_search\tsurface_disposition=supported\texit_code=143"
    "\tclass=supervisor_owned_signal_termination\tsignal_number=15");
  if (!valid_signal.has_value()
      || valid_signal.value().process_status().signal_number() != 15
      || pandapi::runtime::exit_code(valid_signal.value().process_status()) != 143) {
    return false;
  }

  auto oversized_signal = pandapi::runtime::parse_status_record(
    "PANDAPI_STATUS\tstatus=signal_terminated\tcomponent=engine"
    "\tsurface=normal_search\tsurface_disposition=supported\texit_code=143"
    "\tclass=supervisor_owned_signal_termination"
    "\tsignal_number=999999999999999999999999999999999999");
  return !oversized_signal.has_value()
    && oversized_signal.status().code()
      == pandapi::runtime::StatusCode::InputInvalid;
}

[[nodiscard]] bool check_flushing()
{
  auto record = pandapi::runtime::StatusRecord::create(
    status(pandapi::runtime::StatusCode::Ok),
    "normal_search");
  if (!record.has_value()) {
    return false;
  }

  FlushCountingBuffer buffer;
  std::ostream output{&buffer};
  if (!pandapi::runtime::write_status_record(output, record.value())) {
    return false;
  }

  return buffer.flush_count() == 1
    && buffer.str().find("PANDAPI_STATUS\tstatus=ok") == 0
    && !buffer.str().empty()
    && buffer.str().back() == '\n';
}

[[nodiscard]] bool check_partial_output_policy()
{
  return pandapi::runtime::partial_output_policy_name(
      pandapi::runtime::PartialOutputPolicy::Absent) == "absent"
    && pandapi::runtime::partial_output_policy_name(
      pandapi::runtime::PartialOutputPolicy::Retained) == "retained"
    && pandapi::runtime::partial_output_policy_name(
      pandapi::runtime::PartialOutputPolicy::Discarded) == "discarded"
    && pandapi::runtime::partial_output_policy_name(
      pandapi::runtime::PartialOutputPolicy::Complete) == "complete"
    && pandapi::runtime::partial_output_policy_name(
      pandapi::runtime::PartialOutputPolicy::Unknown) == "unknown"
    && pandapi::runtime::parse_partial_output_policy("retained").has_value()
    && !pandapi::runtime::parse_partial_output_policy("malformed").has_value();
}

}  // namespace

int main()
{
  if (!check_round_trip_success()) {
    return 1;
  }
  if (!check_round_trip_domain_no_plan()) {
    return 2;
  }
  if (!check_stream_legality()) {
    return 3;
  }
  if (!check_single_line_escape_and_reject()) {
    return 4;
  }
  if (!check_invalid_input()) {
    return 5;
  }
  if (!check_flushing()) {
    return 6;
  }
  if (!check_partial_output_policy()) {
    return 7;
  }

  return 0;
}
