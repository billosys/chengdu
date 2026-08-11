#include "pandapi/runtime/status.hpp"
#include "pandapi/runtime/status_io.hpp"

#include <cstdio>
#include <cstdlib>
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

int pandaPIengine_legacy_main(int argc, char** argv);

namespace {

using pandapi::runtime::Component;
using pandapi::runtime::PartialOutputPolicy;
using pandapi::runtime::ProcessStatus;
using pandapi::runtime::StatusCode;
using pandapi::runtime::StatusRecord;
using pandapi::runtime::SurfaceDisposition;

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
  StatusCode fenced_surface{StatusCode::Ok};
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
    char pattern[] = "/tmp/pandapi-engine.XXXXXX";
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
      std::remove(file("stdout.txt").c_str());
      std::remove(file("stderr.txt").c_str());
      rmdir(path_.c_str());
    }
  }

  [[nodiscard]] bool valid() const noexcept { return !path_.empty(); }
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
  std::ifstream in{path, std::ios::binary};
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

  const auto probe = parent + "/.pandapi-engine-write-test." +
                     std::to_string(static_cast<long long>(getpid()));
  std::ofstream out{probe};
  if (!out.good()) {
    return false;
  }
  out.close();
  std::remove(probe.c_str());
  return true;
}

[[nodiscard]] bool looks_like_engine_input(const std::string& path)
{
  std::ifstream in{path};
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }
    return line == ";; #state features";
  }
  return false;
}

[[nodiscard]] bool contains(std::string_view haystack, std::string_view needle)
{
  return haystack.find(needle) != std::string_view::npos;
}

[[nodiscard]] ProcessStatus make_status(StatusCode code,
                                        SurfaceDisposition disposition =
                                            SurfaceDisposition::Supported)
{
  return ProcessStatus::from_code(code, Component::Engine, disposition);
}

[[nodiscard]] SurfaceDisposition surface_disposition(StatusCode code)
{
  if (code == StatusCode::LegacySurface) {
    return SurfaceDisposition::Legacy;
  }
  if (code == StatusCode::ExperimentalSurface) {
    return SurfaceDisposition::Experimental;
  }
  if (code == StatusCode::UnsupportedFeature) {
    return SurfaceDisposition::Unsupported;
  }
  if (code == StatusCode::FutureSurface) {
    return SurfaceDisposition::Future;
  }
  return SurfaceDisposition::Supported;
}

[[nodiscard]] std::string status_surface(StatusCode code)
{
  if (code == StatusCode::LegacySurface) {
    return "engine_interactive";
  }
  if (code == StatusCode::ExperimentalSurface) {
    return "engine_translation";
  }
  if (code == StatusCode::UnsupportedFeature) {
    return "engine_sat";
  }
  if (code == StatusCode::FutureSurface) {
    return "engine_bdd_cudd";
  }
  return "normal_search";
}

void emit_status(StatusTarget target, StatusCode code, PartialOutputPolicy policy,
                 const std::vector<pandapi::runtime::StatusField>& fields = {})
{
  if (target == StatusTarget::None) {
    return;
  }

  auto record_result =
      StatusRecord::create(make_status(code, surface_disposition(code)),
                           status_surface(code));
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
  std::exit(pandapi::runtime::exit_code(
      make_status(code, surface_disposition(code))));
}

[[noreturn]] void usage_error(std::string_view message, StatusTarget target)
{
  std::cerr << "pandapi-engine: " << message << '\n';
  finish(target, StatusCode::CliUsageError, PartialOutputPolicy::Absent);
}

[[noreturn]] void policy_surface(StatusCode code, std::string_view message,
                                 StatusTarget target)
{
  std::cerr << "pandapi-engine: " << message << '\n';
  finish(target, code, PartialOutputPolicy::Absent);
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
    } else if (arg == "--interactive" || arg == "-I") {
      options.fenced_surface = StatusCode::LegacySurface;
    } else if (arg == "--translation" || arg == "-2" ||
               arg == "--writeInputToHDDL") {
      options.fenced_surface = StatusCode::ExperimentalSurface;
    } else if (arg == "--sat" || arg == "-s") {
      options.fenced_surface = StatusCode::UnsupportedFeature;
    } else if (arg == "--bdd" || arg == "-b") {
      options.fenced_surface = StatusCode::FutureSurface;
    } else if (arg == "--") {
      while (++i < argc) {
        options.operands.emplace_back(argv[i]);
      }
      break;
    } else if (!arg.empty() && arg.front() == '-') {
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
      << "Usage: pandapi-engine [COMMON] [--output PLAN|-] INPUT.sas\n\n"
      << "Supported surface:\n"
      << "  Normal search over parser/grounder-produced pandaPI .sas input.\n\n"
      << "Common options:\n"
      << "  --output PATH|-        Write the plan artifact to PATH, or to "
         "stdout with -.\n"
      << "  --status[=stderr]     Emit one final PANDAPI_STATUS record on "
         "stderr.\n"
      << "  --status=stdout       Emit final status on stdout only when stdout "
         "is otherwise empty.\n"
      << "  --supervised          Suppress inherited human prose, progress, "
         "statistics, and ANSI behavior.\n"
      << "  --quiet               Suppress non-fatal human diagnostics.\n"
      << "  --verbose             Permit captured inherited diagnostics on "
         "stderr.\n"
      << "  --color=auto|always|never\n"
      << "  --no-color, --no-colour\n"
      << "  --help                Show this help.\n"
      << "  --version             Show version fields.\n"
      << "  --provenance          Show provenance fields.\n\n"
      << "Surface policy:\n"
      << "  Interactive is legacy, translation is experimental, SAT is "
         "unsupported, and BDD/CUDD is future work; these are not supported "
         "normal search behavior through pandapi-engine.\n";
}

[[nodiscard]] std::vector<std::string> provenance_lines()
{
  std::vector<std::string> lines;
  std::ifstream in{"provenance.txt"};
  std::string line;
  bool in_engine = false;
  while (std::getline(in, line)) {
    if (line == "component=pandapi-engine" || line == "component=pandaPIengine") {
      in_engine = true;
      continue;
    }
    if (in_engine && line == "---") {
      break;
    }
    if (!in_engine) {
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
  std::cout << "canonical_command=pandapi-engine\n"
            << "component=engine\n"
            << "chengdu_version=0.3.0\n"
            << "managed_process_contract=0.3.0\n"
            << "upstream_project=pandaPIengine\n"
            << "source_prefix=pandaPI/pandaPIengine\n";
}

void print_provenance()
{
  print_version();
  std::cout << "license=LICENSE\n"
            << "notice=NOTICE\n"
            << "third_party_licenses=THIRD-PARTY-LICENSES\n"
            << "surface_interactive=legacy\n"
            << "surface_translation=experimental\n"
            << "surface_sat=unsupported\n"
            << "surface_bdd_cudd=future\n";
  for (const auto& line : provenance_lines()) {
    std::cout << line << '\n';
  }
}

[[nodiscard]] ChildResult run_legacy_child(const std::string& input,
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

    std::vector<std::string> args{"pandaPIengine", input};
    std::vector<char*> legacy_args;
    legacy_args.reserve(args.size() + 1);
    for (auto& arg : args) {
      legacy_args.push_back(arg.data());
    }
    legacy_args.push_back(nullptr);

    const int rc = pandaPIengine_legacy_main(
        static_cast<int>(legacy_args.size() - 1), legacy_args.data());
    std::cout.flush();
    std::cerr.flush();
    std::fflush(stdout);
    std::fflush(stderr);
    _exit(rc);
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

[[nodiscard]] StatusCode classify_legacy_result(const ChildResult& child)
{
  if (child.exit_code == 0) {
    return StatusCode::Ok;
  }
  if (child.exit_code == 2) {
    return StatusCode::DomainNoPlan;
  }

  const auto out = read_file(child.stdout_path);
  const auto err = read_file(child.stderr_path);
  if (contains(out, "Unable to open input file") ||
      contains(err, "Unable to open input file")) {
    return StatusCode::InputUnavailable;
  }
  if (child.exit_code >= 128) {
    return StatusCode::InternalError;
  }
  return StatusCode::InputInvalid;
}

} // namespace

int main(int argc, char** argv)
{
  std::ios::sync_with_stdio(false);

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

  if (options.fenced_surface == StatusCode::LegacySurface) {
    policy_surface(StatusCode::LegacySurface,
                   "interactive engine behavior is a legacy surface outside "
                   "the managed engine CLI",
                   options.status_target);
  }
  if (options.fenced_surface == StatusCode::ExperimentalSurface) {
    policy_surface(StatusCode::ExperimentalSurface,
                   "translation engine behavior is experimental and is not "
                   "supported normal search",
                   options.status_target);
  }
  if (options.fenced_surface == StatusCode::UnsupportedFeature) {
    policy_surface(StatusCode::UnsupportedFeature,
                   "SAT engine behavior is unsupported in the managed engine CLI",
                   options.status_target);
  }
  if (options.fenced_surface == StatusCode::FutureSurface) {
    policy_surface(StatusCode::FutureSurface,
                   "BDD/CUDD engine behavior is future work",
                   options.status_target);
  }

  if (options.operands.empty()) {
    usage_error("INPUT.sas is required", options.status_target);
  }
  if (options.operands.size() > 1) {
    usage_error("too many positional arguments", options.status_target);
  }
  if (!options.output_given) {
    options.output_path = "-";
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

  const auto& input = options.operands[0];
  if (!is_readable_file(input)) {
    if (!options.quiet) {
      std::cerr << "pandapi-engine: cannot read input: " << input << '\n';
    }
    finish(options.status_target, StatusCode::InputUnavailable,
           PartialOutputPolicy::Absent,
           {{"path_role", "engine_input"}, {"operation", "open"}});
  }

  if (!looks_like_engine_input(input)) {
    if (!options.quiet) {
      std::cerr << "pandapi-engine: input invalid\n";
    }
    finish(options.status_target, StatusCode::InputInvalid,
           PartialOutputPolicy::Absent,
           {{"path_role", "engine_input"}, {"operation", "parse"}});
  }

  if (!probe_writable_output(options.output_path)) {
    if (!options.quiet) {
      std::cerr << "pandapi-engine: cannot write output: " << options.output_path
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

  const auto legacy_stdout = work.file("stdout.txt");
  const auto legacy_stderr = work.file("stderr.txt");
  const auto child = run_legacy_child(input, legacy_stdout, legacy_stderr);
  const auto status = classify_legacy_result(child);

  if (status == StatusCode::Ok) {
    if (options.output_path == "-") {
      std::cout << read_file(legacy_stdout);
    } else {
      const auto tmp_output =
          options.output_path + ".tmp." +
          std::to_string(static_cast<long long>(getpid()));
      std::remove(tmp_output.c_str());
      if (!copy_file(legacy_stdout, tmp_output) ||
          !move_file(tmp_output, options.output_path)) {
        std::remove(tmp_output.c_str());
        std::remove(options.output_path.c_str());
        if (!options.quiet) {
          std::cerr << "pandapi-engine: cannot finalize output: "
                    << options.output_path << '\n';
        }
        finish(options.status_target, StatusCode::OutputUnavailable,
               PartialOutputPolicy::Discarded,
               {{"path_role", "output"}, {"operation", "finalize"}});
      }
    }
    if (options.verbose && !options.supervised) {
      std::cerr << read_file(legacy_stderr);
    }
    finish(options.status_target, StatusCode::Ok, PartialOutputPolicy::Complete,
           {{"artifact", options.output_path == "-" ? "stdout" : "file"},
            {"outcome", "solved"}});
  }

  if (options.output_path != "-") {
    std::remove(options.output_path.c_str());
  }

  if (status == StatusCode::DomainNoPlan) {
    if (options.verbose && !options.supervised) {
      std::cerr << read_file(legacy_stdout);
      std::cerr << read_file(legacy_stderr);
    } else if (!options.quiet) {
      std::cerr << "pandapi-engine: search completed with no plan\n";
    }
    finish(options.status_target, StatusCode::DomainNoPlan,
           PartialOutputPolicy::Absent, {{"outcome", "no_plan"}});
  }

  if (status == StatusCode::InputUnavailable) {
    if (!options.quiet) {
      std::cerr << "pandapi-engine: input unavailable\n";
    }
    finish(options.status_target, StatusCode::InputUnavailable,
           PartialOutputPolicy::Absent,
           {{"path_role", "engine_input"}, {"operation", "open"}});
  }

  if (status == StatusCode::InputInvalid) {
    if (options.verbose && !options.supervised) {
      std::cerr << read_file(legacy_stdout);
      std::cerr << read_file(legacy_stderr);
    } else if (!options.quiet) {
      std::cerr << "pandapi-engine: input invalid\n";
    }
    finish(options.status_target, StatusCode::InputInvalid,
           PartialOutputPolicy::Discarded);
  }

  if (!options.quiet) {
    std::cerr << "pandapi-engine: internal engine failure\n";
  }
  finish(options.status_target, StatusCode::InternalError,
         PartialOutputPolicy::Unknown);
}
