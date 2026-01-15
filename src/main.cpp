#include <filesystem>

#include <CLI/CLI.hpp>
#include <fmt/core.h>

namespace fs = std::filesystem;

struct LsOptions {
  bool all = false;                 // -a
  bool long_fmt = false;            // -l
  bool recursive = false;           // -R
  std::vector<std::string> targets; // ls <targets>
};

void list_directory(const fs::path& path, const LsOptions& opts) {
  std::error_code ec;
  if (!fs::exists(path, ec)) {
    fmt::println("ls: {}: No such file or directory", path.string());
    return;
  }

  if (fs::is_regular_file(path, ec)) {
    fmt::println("{}", path.filename().string());
    return;
  }

  // iterate directory
  std::vector<fs::path> entries;

  try {
    for (const auto& entry : fs::directory_iterator(path)) {
      std::string filename = entry.path().filename();

      if (!opts.all && filename.starts_with(".")) {
        // ignore hidden file
        continue;
      }

      entries.push_back(entry.path());
    }
  } catch (const fs::filesystem_error& err) {
    fmt::println(stdout, "ls: cannot open directory {}", path.string());
    return;
  }

  // print entries
  // by default, ls sorts file by name
  std::sort(entries.begin(), entries.end());

  for (const auto& entry : entries) {
    // TODO: list files in grid layout
    fmt::println("{}", entry.filename().string());
  }
}

int main(int argc, char** argv) {
  // initialize CLI11 app
  CLI::App app{"ls rewritten in C++", "lscpp"};
  // convert argv to UTF-8 encoding, only on Windows
  argv = app.ensure_utf8(argv);

  LsOptions opts;

  // parse arguments, bind cli arguments to LsOptions fields
  app.add_flag("-a,--all", opts.all, "Include directory entries whose names begin with a dot ('.')");
  app.add_flag("-l", opts.long_fmt, "(The lowercase letter “ell”.) List files in the long format, as described in the The Long Format subsection below.");
  app.add_flag("-R,--recursive", opts.recursive, "Recursively list subdirectories encountered.");
  // positional arguments, targets
  app.add_option("file", opts.targets, "Files or directories to list");
  CLI11_PARSE(app, argc, argv);

  // if no file specified, use "." by default
  if (opts.targets.empty()) {
    opts.targets.push_back(".");
  }

  bool multi_target = opts.targets.size() > 1;

  for (const auto& target : opts.targets) {
    if (multi_target) {
      fmt::println("{}:", target);
    }

    list_directory(target, opts);

    // TODO: remove extra last line
    if (multi_target) {
      fmt::println("");
    }
  }

  return 0;
}
