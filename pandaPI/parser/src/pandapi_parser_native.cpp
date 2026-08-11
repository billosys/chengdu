#include "pandapi/runtime/status.hpp"
#include "pandapi/runtime/status_io.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

int pandaPIparser_legacy_main(int argc, char** argv);

namespace {

using pandapi::runtime::Component;
using pandapi::runtime::PartialOutputPolicy;
using pandapi::runtime::ProcessStatus;
using pandapi::runtime::StatusCode;
using pandapi::runtime::StatusRecord;
using pandapi::runtime::StatusStream;
using pandapi::runtime::SurfaceDisposition;

constexpr std::string_view hidden_driver = "--pandapi-parser-legacy-driver";

enum class StatusTarget {
  None,
  Stderr,
  Stdout,
};

struct Options {
  StatusTarget status_target{StatusTarget::None};
  std::string output_path;
  bool output_given{false};
  bool supervised{false};
  bool quiet{false};
  bool verbose{false};
  std::string color_mode{"auto"};
  bool color_disabled{false};
  std::string info_command{"none"};
  bool positional_output_alias{false};
  std::vector<std::string> operands;
};

struct ChildResult {
  int exit_code{60};
  std::string stdout_path;
  std::string stderr_path;
};

class TemporaryDirectory {
public:
  TemporaryDirectory()
  {
    char pattern[] = "/tmp/pandapi-parser.XXXXXX";
    char* path = mkdtemp(pattern);
    if (path != nullptr) {
      path_ = path;
    }
  }

  TemporaryDirectory(const TemporaryDirectory&) = delete;
  TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;

  ~TemporaryDirectory()
  {
    if (!path_.empty()) {
      std::remove(file("parser.htn").c_str());
      std::remove(file("stdout.txt").c_str());
      std::remove(file("stderr.txt").c_str());
      rmdir(path_.c_str());
    }
  }

  [[nodiscard]] bool valid() const noexcept { return !path_.empty(); }
  [[nodiscard]] std::string path() const { return path_; }
  [[nodiscard]] std::string file(std::string_view name) const
  {
    return path_ + "/" + std::string{name};
  }

private:
  std::string path_;
};

[[nodiscard]] bool is_readable_file(const std::string& path)
{
  return access(path.c_str(), R_OK) == 0;
}

[[nodiscard]] bool is_directory(const std::string& path)
{
  struct stat st {};
  return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

[[nodiscard]] std::string dirname_of(const std::string& path)
{
  const auto pos = path.find_last_of('/');
  if (pos == std::string::npos) {
    return ".";
  }
  if (pos == 0) {
    return "/";
  }
  return path.substr(0, pos);
}

[[nodiscard]] std::string read_file(const std::string& path)
{
  std::ifstream in{path};
  std::ostringstream out;
  out << in.rdbuf();
  return out.str();
}

[[nodiscard]] bool copy_file(const std::string& from, const std::string& to)
{
  std::ifstream in{from, std::ios::binary};
  std::ofstream out{to, std::ios::binary | std::ios::trunc};
  out << in.rdbuf();
  return in.good() && out.good();
}

[[nodiscard]] bool move_file(const std::string& from, const std::string& to)
{
  return std::rename(from.c_str(), to.c_str()) == 0;
}

[[nodiscard]] bool probe_writable_output(const std::string& output_path)
{
  if (output_path == "-") {
    return true;
  }
  if (is_directory(output_path)) {
    return false;
  }

  const auto parent = dirname_of(output_path);
  if (!is_directory(parent)) {
    return false;
  }

  const auto probe = parent + "/.pandapi-parser-write-test." +
                     std::to_string(static_cast<long long>(getpid()));
  std::ofstream out{probe};
  if (!out.good()) {
    return false;
  }
  out.close();
  std::remove(probe.c_str());
  return true;
}

[[nodiscard]] bool contains(std::string_view haystack, std::string_view needle)
{
  return haystack.find(needle) != std::string_view::npos;
}

[[nodiscard]] bool is_legacy_surface_option(std::string_view arg)
{
  static constexpr std::string_view options[] = {
      "-c", "--panda-converter", "-p", "--properties", "-v", "--verify",
      "-V", "--vverify", "-W", "--vvverify", "-l", "--lenient", "-o",
      "--verify-no-order", "-E", "--verification-encoding", "-S", "--shop",
      "-1", "--shop1", "-H", "--hpdl", "-R", "--hppdl", "-h", "--hddl",
      "-P", "--processed-hddl", "-i", "--internal-hddl",
      "--no-domain-constants", "--remove-method-preconditions", "-m",
      "--goal-action", "-g", "--encode-disjunctive-preconditions-in-htn", "-D",
      "--keep-conditional-effects", "-k", "--linear-conditional-effect", "-L",
      "--no-split-parameters", "-s", "--debug", "-d"};

  for (const auto option : options) {
    if (arg == option) {
      return true;
    }
  }
  return false;
}

[[nodiscard]] ProcessStatus make_status(StatusCode code,
                                        SurfaceDisposition disposition =
                                            SurfaceDisposition::Supported)
{
  return ProcessStatus::from_code(code, Component::Parser, disposition);
}

[[nodiscard]] std::string status_surface(StatusCode code)
{
  if (code == StatusCode::LegacySurface) {
    return "parser_legacy_helper";
  }
  return "normal_parse";
}

void emit_status(StatusTarget target, StatusCode code, PartialOutputPolicy policy,
                 const std::vector<pandapi::runtime::StatusField>& fields = {})
{
  if (target == StatusTarget::None) {
    return;
  }

  const auto disposition = code == StatusCode::LegacySurface
                               ? SurfaceDisposition::Legacy
                               : SurfaceDisposition::Supported;
  auto record_result =
      StatusRecord::create(make_status(code, disposition), status_surface(code));
  if (!record_result.has_value()) {
    return;
  }

  auto record = record_result.value();
  record.set_partial_output_policy(policy);
  for (const auto& field : fields) {
    (void)record.add_field(field.key, field.value);
  }

  if (target == StatusTarget::Stdout) {
    (void)pandapi::runtime::write_status_record(std::cout, record);
  } else {
    (void)pandapi::runtime::write_status_record(std::cerr, record);
  }
}

[[noreturn]] void finish(StatusTarget target, StatusCode code,
                         PartialOutputPolicy policy,
                         const std::vector<pandapi::runtime::StatusField>& fields = {})
{
  emit_status(target, code, policy, fields);
  std::exit(pandapi::runtime::exit_code(make_status(
      code, code == StatusCode::LegacySurface ? SurfaceDisposition::Legacy
                                              : SurfaceDisposition::Supported)));
}

[[noreturn]] void usage_error(std::string_view message, StatusTarget target)
{
  std::cerr << "pandapi-parser: " << message << '\n';
  finish(target, StatusCode::CliUsageError, PartialOutputPolicy::Absent);
}

[[noreturn]] void legacy_surface(StatusTarget target)
{
  std::cerr << "pandapi-parser: legacy parser helper/output surface is not "
               "supported by the managed parser CLI\n";
  finish(target, StatusCode::LegacySurface, PartialOutputPolicy::Absent);
}

void set_info_command(Options& options, std::string command)
{
  if (options.info_command != "none") {
    usage_error("only one informational command may be selected",
                options.status_target);
  }
  options.info_command = std::move(command);
}

[[nodiscard]] Options parse_options(int argc, char** argv)
{
  Options options;

  for (int i = 1; i < argc; ++i) {
    const std::string arg{argv[i]};
    if (arg == "--help") {
      set_info_command(options, "help");
    } else if (arg == "--version") {
      set_info_command(options, "version");
    } else if (arg == "--provenance") {
      set_info_command(options, "provenance");
    } else if (arg == "--status" || arg == "--status=stderr") {
      options.status_target = StatusTarget::Stderr;
    } else if (arg == "--status=stdout") {
      options.status_target = StatusTarget::Stdout;
    } else if (arg.rfind("--status=", 0) == 0) {
      usage_error("invalid --status value: " + arg.substr(9),
                  options.status_target);
    } else if (arg == "--output") {
      if (options.output_given) {
        usage_error("--output specified more than once", options.status_target);
      }
      if (i + 1 >= argc) {
        usage_error("--output requires a path or -", options.status_target);
      }
      options.output_path = argv[++i];
      if (options.output_path.empty()) {
        usage_error("--output requires a path or -", options.status_target);
      }
      options.output_given = true;
    } else if (arg.rfind("--output=", 0) == 0) {
      if (options.output_given) {
        usage_error("--output specified more than once", options.status_target);
      }
      options.output_path = arg.substr(9);
      if (options.output_path.empty()) {
        usage_error("--output requires a path or -", options.status_target);
      }
      options.output_given = true;
    } else if (arg == "--supervised") {
      options.supervised = true;
    } else if (arg == "--quiet") {
      options.quiet = true;
    } else if (arg == "--verbose") {
      options.verbose = true;
    } else if (arg == "--color") {
      if (i + 1 >= argc) {
        usage_error("--color requires auto, always, or never",
                    options.status_target);
      }
      options.color_mode = argv[++i];
    } else if (arg.rfind("--color=", 0) == 0) {
      options.color_mode = arg.substr(8);
    } else if (arg == "--no-color" || arg == "--no-colour") {
      options.color_disabled = true;
      options.color_mode = "never";
    } else if (arg == "--") {
      while (++i < argc) {
        options.operands.emplace_back(argv[i]);
      }
      break;
    } else if (!arg.empty() && arg.front() == '-') {
      if (is_legacy_surface_option(arg)) {
        legacy_surface(options.status_target);
      }
      usage_error("unknown option: " + arg, options.status_target);
    } else {
      options.operands.push_back(arg);
    }
  }

  if (options.color_mode != "auto" && options.color_mode != "always" &&
      options.color_mode != "never") {
    usage_error("invalid --color value: " + options.color_mode,
                options.status_target);
  }

  return options;
}

void print_help()
{
  std::cout
      << "Usage: pandapi-parser [COMMON] [--output OUT.htn|-] DOMAIN.hddl "
         "PROBLEM.hddl\n\n"
      << "Supported surface:\n"
      << "  Normal HDDL domain/problem parsing to pandaPI .htn output.\n\n"
      << "Common options:\n"
      << "  --output PATH|-        Write the parser artifact to PATH, or to "
         "stdout with -.\n"
      << "  --status[=stderr]     Emit one final PANDAPI_STATUS record on "
         "stderr.\n"
      << "  --status=stdout       Emit final status on stdout only when stdout "
         "is otherwise empty.\n"
      << "  --supervised          Suppress inherited human prose and ANSI "
         "behavior.\n"
      << "  --quiet               Suppress non-fatal human diagnostics.\n"
      << "  --verbose             Permit additional human diagnostics on "
         "stderr.\n"
      << "  --color=auto|always|never\n"
      << "  --no-color, --no-colour\n"
      << "  --help                Show this help.\n"
      << "  --version             Show version fields.\n"
      << "  --provenance          Show provenance fields.\n\n"
      << "Compatibility:\n"
      << "  Inherited parser helper modes are not supported managed-process "
         "surfaces through pandapi-parser.\n";
}

[[nodiscard]] std::vector<std::string> provenance_lines()
{
  std::vector<std::string> lines;
  std::ifstream in{"provenance.txt"};
  std::string line;
  bool in_parser = false;
  while (std::getline(in, line)) {
    if (line == "component=pandapi-parser" || line == "component=pandaPIparser") {
      in_parser = true;
      continue;
    }
    if (in_parser && line == "---") {
      break;
    }
    if (!in_parser) {
      continue;
    }
    if (line.rfind("chengdu_commit=", 0) == 0 ||
        line.rfind("upstream_sha=", 0) == 0 ||
        line.rfind("import_commit=", 0) == 0 ||
        line.rfind("compiler=", 0) == 0) {
      lines.push_back(line);
    }
  }
  return lines;
}

void print_version()
{
  std::cout << "canonical_command=pandapi-parser\n"
            << "component=parser\n"
            << "chengdu_version=0.3.0\n"
            << "managed_process_contract=0.3.0\n"
            << "upstream_project=pandaPIparser\n"
            << "source_prefix=pandaPI/pandaPIparser\n";
}

void print_provenance()
{
  print_version();
  std::cout << "license=LICENSE\n"
            << "notice=NOTICE\n"
            << "third_party_licenses=THIRD-PARTY-LICENSES\n";
  for (const auto& line : provenance_lines()) {
    std::cout << line << '\n';
  }
}

[[nodiscard]] bool missing_include(const std::string& path, std::string& include_name)
{
  std::ifstream in{path};
  std::string line;
  while (std::getline(in, line)) {
    const auto marker = line.find("(:include");
    if (marker == std::string::npos) {
      continue;
    }
    auto cursor = marker + std::string{"(:include"}.size();
    while (cursor < line.size() &&
           (line[cursor] == ' ' || line[cursor] == '\t')) {
      ++cursor;
    }
    const auto start = cursor;
    while (cursor < line.size() && line[cursor] != ')' && line[cursor] != ' ' &&
           line[cursor] != '\t') {
      ++cursor;
    }
    if (cursor > start) {
      include_name = line.substr(start, cursor - start);
      if (!is_readable_file(include_name + ".hddl")) {
        return true;
      }
    }
  }
  return false;
}

[[nodiscard]] ChildResult run_legacy_child(const std::string& self,
                                           const std::string& domain,
                                           const std::string& problem,
                                           const std::string& legacy_out,
                                           const std::string& stdout_path,
                                           const std::string& stderr_path)
{
  const pid_t pid = fork();
  if (pid < 0) {
    return ChildResult{60, stdout_path, stderr_path};
  }

  if (pid == 0) {
    FILE* out = std::freopen(stdout_path.c_str(), "w", stdout);
    FILE* err = std::freopen(stderr_path.c_str(), "w", stderr);
    if (out == nullptr || err == nullptr) {
      _exit(127);
    }

    std::vector<std::string> args{self, std::string{hidden_driver}, "-C", domain,
                                  problem, legacy_out};
    std::vector<char*> exec_args;
    exec_args.reserve(args.size() + 1);
    for (auto& arg : args) {
      exec_args.push_back(arg.data());
    }
    exec_args.push_back(nullptr);
    execvp(self.c_str(), exec_args.data());
    _exit(127);
  }

  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    return ChildResult{60, stdout_path, stderr_path};
  }
  if (WIFEXITED(status)) {
    return ChildResult{WEXITSTATUS(status), stdout_path, stderr_path};
  }
  if (WIFSIGNALED(status)) {
    return ChildResult{128 + WTERMSIG(status), stdout_path, stderr_path};
  }
  return ChildResult{60, stdout_path, stderr_path};
}

int run_hidden_driver(int argc, char** argv)
{
  std::vector<char*> legacy_argv;
  legacy_argv.reserve(static_cast<std::size_t>(argc));
  legacy_argv.push_back(argv[0]);
  for (int i = 2; i < argc; ++i) {
    legacy_argv.push_back(argv[i]);
  }
  legacy_argv.push_back(nullptr);
  return pandaPIparser_legacy_main(static_cast<int>(legacy_argv.size() - 1),
                                   legacy_argv.data());
}

} // namespace

int main(int argc, char** argv)
{
  std::ios::sync_with_stdio(false);

  if (argc > 1 && std::string_view{argv[1]} == hidden_driver) {
    return run_hidden_driver(argc, argv);
  }

  auto options = parse_options(argc, argv);

  if (options.info_command != "none") {
    if (!options.operands.empty() || options.output_given) {
      usage_error("informational commands do not accept input or output operands",
                  options.status_target);
    }
    if (options.status_target == StatusTarget::Stdout) {
      usage_error("--status=stdout conflicts with informational stdout",
                  options.status_target);
    }
    if (options.info_command == "help") {
      print_help();
    } else if (options.info_command == "version") {
      print_version();
    } else if (options.info_command == "provenance") {
      print_provenance();
    }
    finish(options.status_target, StatusCode::Ok, PartialOutputPolicy::Unknown);
  }

  if (options.operands.size() < 2) {
    usage_error("DOMAIN.hddl and PROBLEM.hddl are required", options.status_target);
  }
  if (options.output_given && options.operands.size() > 2) {
    usage_error("--output conflicts with positional output compatibility",
                options.status_target);
  }

  if (!options.output_given) {
    if (options.operands.size() == 3) {
      options.positional_output_alias = true;
      options.output_path = options.operands[2];
    } else if (options.operands.size() == 2) {
      options.output_path = "-";
    } else {
      usage_error("too many positional arguments", options.status_target);
    }
  } else if (options.operands.size() != 2) {
    usage_error("exactly DOMAIN.hddl and PROBLEM.hddl are required with --output",
                options.status_target);
  }

  if (options.status_target == StatusTarget::Stdout && options.output_path == "-") {
    usage_error("--status=stdout conflicts with stdout artifact output",
                options.status_target);
  }

  if (options.color_mode == "always" && !options.color_disabled &&
      (options.supervised || options.status_target != StatusTarget::None ||
       options.output_path == "-")) {
    usage_error("--color=always conflicts with supervised or machine-owned streams",
                options.status_target);
  }

  if (std::getenv("NO_COLOR") != nullptr) {
    options.color_disabled = true;
    options.color_mode = "never";
  }

  const auto& domain = options.operands[0];
  const auto& problem = options.operands[1];
  if (!is_readable_file(domain)) {
    if (!options.quiet) {
      std::cerr << "pandapi-parser: cannot read domain: " << domain << '\n';
    }
    finish(options.status_target, StatusCode::InputUnavailable,
           PartialOutputPolicy::Absent,
           {{"path_role", "domain"}, {"operation", "open"}});
  }
  if (!is_readable_file(problem)) {
    if (!options.quiet) {
      std::cerr << "pandapi-parser: cannot read problem: " << problem << '\n';
    }
    finish(options.status_target, StatusCode::InputUnavailable,
           PartialOutputPolicy::Absent,
           {{"path_role", "problem"}, {"operation", "open"}});
  }

  std::string include_name;
  if (missing_include(domain, include_name) || missing_include(problem, include_name)) {
    if (!options.quiet) {
      std::cerr << "pandapi-parser: cannot read include: " << include_name
                << ".hddl\n";
    }
    finish(options.status_target, StatusCode::InputUnavailable,
           PartialOutputPolicy::Absent,
           {{"path_role", "include"},
            {"operation", "open"},
            {"include_stack", include_name}});
  }

  if (!probe_writable_output(options.output_path)) {
    if (!options.quiet) {
      std::cerr << "pandapi-parser: cannot write output: " << options.output_path
                << '\n';
    }
    finish(options.status_target, StatusCode::OutputUnavailable,
           PartialOutputPolicy::Absent,
           {{"path_role", "output"}, {"operation", "open"}});
  }

  TemporaryDirectory work;
  if (!work.valid()) {
    finish(options.status_target, StatusCode::InternalError,
           PartialOutputPolicy::Unknown);
  }

  const auto legacy_out = work.file("parser.htn");
  const auto legacy_stdout = work.file("stdout.txt");
  const auto legacy_stderr = work.file("stderr.txt");
  const auto child =
      run_legacy_child(argv[0], domain, problem, legacy_out, legacy_stdout, legacy_stderr);

  if (child.exit_code != 0) {
    if (options.output_path != "-") {
      std::remove(options.output_path.c_str());
    }
    const auto out = read_file(child.stdout_path);
    const auto err = read_file(child.stderr_path);
    if (contains(out, "I can't open") || contains(err, "I can't open")) {
      if (!options.quiet) {
        std::cerr << "pandapi-parser: parser input unavailable\n";
      }
      finish(options.status_target, StatusCode::InputUnavailable,
             PartialOutputPolicy::Absent);
    }
    if (options.verbose && !options.supervised) {
      std::cerr << err;
    } else if (!options.quiet) {
      std::cerr << "pandapi-parser: parser input invalid\n";
    }
    finish(options.status_target, StatusCode::InputInvalid,
           PartialOutputPolicy::Discarded);
  }

  struct stat st {};
  if (stat(legacy_out.c_str(), &st) != 0 || st.st_size == 0) {
    if (!options.quiet) {
      std::cerr << "pandapi-parser: legacy parser produced no output artifact\n";
    }
    finish(options.status_target, StatusCode::InternalError,
           PartialOutputPolicy::Unknown);
  }

  if (options.output_path == "-") {
    std::cout << read_file(legacy_out);
  } else {
    const auto tmp_output =
        options.output_path + ".tmp." + std::to_string(static_cast<long long>(getpid()));
    std::remove(tmp_output.c_str());
    if (!copy_file(legacy_out, tmp_output) || !move_file(tmp_output, options.output_path)) {
      std::remove(tmp_output.c_str());
      std::remove(options.output_path.c_str());
      if (!options.quiet) {
        std::cerr << "pandapi-parser: cannot finalize output: "
                  << options.output_path << '\n';
      }
      finish(options.status_target, StatusCode::OutputUnavailable,
             PartialOutputPolicy::Discarded,
             {{"path_role", "output"}, {"operation", "finalize"}});
    }
  }

  if (options.verbose && !options.supervised) {
    std::cerr << read_file(child.stderr_path);
  }

  std::vector<pandapi::runtime::StatusField> fields{{"artifact",
                                                     options.output_path == "-"
                                                         ? "stdout"
                                                         : "file"}};
  if (options.positional_output_alias) {
    fields.push_back({"positional_output_alias", "true"});
  }
  finish(options.status_target, StatusCode::Ok, PartialOutputPolicy::Complete,
         fields);
}
