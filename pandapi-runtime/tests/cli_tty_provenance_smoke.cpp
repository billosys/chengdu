#include "pandapi/runtime/cli_policy.hpp"
#include "pandapi/runtime/provenance.hpp"
#include "pandapi/runtime/tty.hpp"

#include <cassert>
#include <string>

namespace {

using pandapi::runtime::AnsiPolicy;
using pandapi::runtime::AnsiStream;
using pandapi::runtime::ColorControls;
using pandapi::runtime::ColorMode;
using pandapi::runtime::CommandIdentity;
using pandapi::runtime::CommonCliOptions;
using pandapi::runtime::Component;
using pandapi::runtime::InformationalCommand;
using pandapi::runtime::InvocationMode;
using pandapi::runtime::OutputRole;
using pandapi::runtime::OutputTarget;
using pandapi::runtime::ProvenanceRecord;
using pandapi::runtime::StatusCode;
using pandapi::runtime::StatusTarget;
using pandapi::runtime::SurfaceDisposition;
using pandapi::runtime::TerminalKind;

[[nodiscard]] CommonCliOptions base_options()
{
  CommonCliOptions options;
  options.status_target = StatusTarget::Stderr;
  options.output_target = OutputTarget::File;
  options.output_option = true;
  return options;
}

[[nodiscard]] ProvenanceRecord sample_provenance_record()
{
  ProvenanceRecord record;
  record.canonical_command = "pandapi-engine";
  record.invoked_command = "pandaPIengine";
  record.component = "engine";
  record.chengdu_version = "0.3.0";
  record.contract_version = "0.3.0";
  record.upstream_project = "pandaPIengine";
  record.upstream_commit = "0123456789abcdef";
  record.source_prefix = "pandaPI/engine";
  record.build_commit = "fedcba9876543210";
  record.platform = "macos-arm64";
  record.compiler = "clang-17";
  record.license = "LICENSE";
  record.notice = "NOTICE";
  record.import_commit = "1111111111111111";
  record.third_party_licenses = "THIRD-PARTY-LICENSES";
  return record;
}

void command_identity_covers_canonical_and_compatibility_names()
{
  const auto parser = pandapi::runtime::command_identity_for(
    Component::Parser,
    "pandapi-parser");
  assert(parser.has_value());
  assert(parser.value().canonical_name() == "pandapi-parser");
  assert(!parser.value().is_compatibility_invocation());

  const auto grounder = pandapi::runtime::command_identity_for(
    Component::Grounder,
    "pandaPIgrounder");
  assert(grounder.has_value());
  assert(grounder.value().canonical_name() == "pandapi-grounder");
  assert(grounder.value().invoked_name() == "pandaPIgrounder");
  assert(grounder.value().is_compatibility_invocation());

  assert(pandapi::runtime::is_canonical_command_name("pandapi-engine"));
  assert(pandapi::runtime::is_inherited_command_name("pandaPIparser"));

  const auto mismatch = pandapi::runtime::command_identity_for(
    Component::Engine,
    "pandaPIparser");
  assert(!mismatch.has_value());
  assert(mismatch.status().code() == StatusCode::CliUsageError);
}

void cli_policy_conflicts_are_cli_usage_error()
{
  auto data_stdout_conflict = base_options();
  data_stdout_conflict.status_target = StatusTarget::Stdout;
  data_stdout_conflict.output_target = OutputTarget::Stdout;
  const auto data_result = pandapi::runtime::validate_common_cli_options(
    data_stdout_conflict,
    Component::Parser);
  assert(!data_result.has_value());
  assert(data_result.status().code() == StatusCode::CliUsageError);
  assert(pandapi::runtime::exit_code(data_result.status()) == 10);

  auto informational_conflict = base_options();
  informational_conflict.status_target = StatusTarget::Stdout;
  informational_conflict.output_target = OutputTarget::Absent;
  informational_conflict.informational_command = InformationalCommand::Help;
  const auto informational_result = pandapi::runtime::validate_common_cli_options(
    informational_conflict,
    Component::Grounder);
  assert(!informational_result.has_value());
  assert(informational_result.status().code() == StatusCode::CliUsageError);

  // --output and the inherited positional output alias are mutually exclusive;
  // this fails before input/model processing.
  auto alias_conflict = base_options();
  alias_conflict.positional_output_alias = true;
  const auto alias_result = pandapi::runtime::validate_common_cli_options(
    alias_conflict,
    Component::Parser);
  assert(!alias_result.has_value());
  assert(alias_result.status().code() == StatusCode::CliUsageError);

  auto color_conflict = base_options();
  color_conflict.invocation_mode = InvocationMode::Supervised;
  color_conflict.color_controls.color_mode = ColorMode::Always;
  const auto color_result = pandapi::runtime::validate_common_cli_options(
    color_conflict,
    Component::Engine);
  assert(!color_result.has_value());
  assert(color_result.status().code() == StatusCode::CliUsageError);

  auto disabled_color = color_conflict;
  disabled_color.color_controls.no_color = true;
  const auto disabled_color_result = pandapi::runtime::validate_common_cli_options(
    disabled_color,
    Component::Engine);
  assert(disabled_color_result.has_value());
}

void informational_commands_are_stdout_owned_successes()
{
  const auto command = pandapi::runtime::informational_command_from_flags(
    false,
    true,
    false,
    Component::Engine);
  assert(command.has_value());
  assert(command.value() == InformationalCommand::Version);
  assert(pandapi::runtime::informational_command_output_role(command.value())
    == OutputRole::Informational);

  const auto status = pandapi::runtime::informational_command_status(
    command.value(),
    Component::Engine);
  assert(status.code() == StatusCode::Ok);
  assert(pandapi::runtime::status_name(status) == "ok");
  assert(pandapi::runtime::exit_code(status) == 0);

  const auto multiple = pandapi::runtime::informational_command_from_flags(
    true,
    true,
    false,
    Component::Engine);
  assert(!multiple.has_value());
  assert(multiple.status().code() == StatusCode::CliUsageError);
}

void color_policy_is_injected_and_keeps_machine_streams_clean()
{
  const ColorControls enabling{ColorMode::Always, false, false, false};
  const AnsiPolicy human_stderr{
    enabling,
    TerminalKind::Tty,
    AnsiStream::Stderr,
    OutputRole::Empty,
    false,
    false};
  assert(pandapi::runtime::allows_color(human_stderr));

  // disabling controls win: --no-color, --no-colour, NO_COLOR, and never
  // disables ANSI even when another option requests color; the last wins check
  // is expressed through effective_color_mode.
  const ColorControls disabled_by_no_color{ColorMode::Always, true, false, false};
  assert(pandapi::runtime::effective_color_mode(disabled_by_no_color)
    == ColorMode::Never);
  assert(pandapi::runtime::color_disabling_control_present(
    ColorControls{ColorMode::Auto, false, true, false}));
  assert(pandapi::runtime::color_disabling_control_present(
    ColorControls{ColorMode::Auto, false, false, true}));

  assert(pandapi::runtime::detect_terminal(true) == TerminalKind::Tty);
  assert(pandapi::runtime::detect_terminal(false) == TerminalKind::NotTty);

  const AnsiPolicy stdout_artifact{
    enabling,
    TerminalKind::Tty,
    AnsiStream::Stdout,
    OutputRole::DataArtifact,
    false,
    false};
  assert(!pandapi::runtime::allows_color(stdout_artifact));

  const AnsiPolicy tagged_status{
    enabling,
    TerminalKind::Tty,
    AnsiStream::Stderr,
    OutputRole::TaggedStatus,
    false,
    false};
  assert(!pandapi::runtime::allows_color(tagged_status));

  const AnsiPolicy supervised_pipe{
    enabling,
    TerminalKind::NotTty,
    AnsiStream::Pipe,
    OutputRole::Empty,
    true,
    true};
  assert(!pandapi::runtime::allows_color(supervised_pipe));

  const AnsiPolicy file_output{
    enabling,
    TerminalKind::Tty,
    AnsiStream::File,
    OutputRole::Empty,
    false,
    false};
  assert(!pandapi::runtime::allows_color(file_output));
}

void provenance_output_is_stable_and_caller_supplied()
{
  const auto record = sample_provenance_record();
  const auto version = pandapi::runtime::format_version(record, Component::Engine);
  assert(version.has_value());
  assert(version.value().find("canonical_command=pandapi-engine\n")
    != std::string::npos);
  assert(version.value().find("invoked_command=pandaPIengine\n")
    != std::string::npos);
  assert(version.value().find("contract_version=0.3.0\n") != std::string::npos);
  assert(version.value().find("notice=NOTICE\n") != std::string::npos);

  const auto provenance = pandapi::runtime::format_provenance(
    record,
    Component::Engine);
  assert(provenance.has_value());
  assert(provenance.value().find("import_commit=1111111111111111\n")
    != std::string::npos);
  assert(provenance.value().find("third_party_licenses=THIRD-PARTY-LICENSES\n")
    != std::string::npos);

  auto missing_optional = record;
  missing_optional.import_commit.clear();
  missing_optional.build_timestamp.clear();
  const auto omitted = pandapi::runtime::format_provenance(
    missing_optional,
    Component::Engine);
  assert(omitted.has_value());
  assert(omitted.value().find("import_commit=") == std::string::npos);
  assert(omitted.value().find("build_timestamp=") == std::string::npos);

  auto placeholder = record;
  placeholder.platform = "unknown";
  const auto rejected = pandapi::runtime::format_version(
    placeholder,
    Component::Engine);
  assert(!rejected.has_value());
  assert(rejected.status().code() == StatusCode::CliUsageError);
}

void malformed_enum_values_are_rejected()
{
  const auto color = pandapi::runtime::parse_color_mode(
    "rainbow",
    Component::Engine);
  assert(!color.has_value());
  assert(color.status().code() == StatusCode::CliUsageError);

  const auto status_target = pandapi::runtime::parse_status_target(
    "both",
    Component::Engine);
  assert(!status_target.has_value());
  assert(status_target.status().code() == StatusCode::CliUsageError);
}

}  // namespace

int main()
{
  command_identity_covers_canonical_and_compatibility_names();
  cli_policy_conflicts_are_cli_usage_error();
  informational_commands_are_stdout_owned_successes();
  color_policy_is_injected_and_keeps_machine_streams_clean();
  provenance_output_is_stable_and_caller_supplied();
  malformed_enum_values_are_rejected();
  return 0;
}
