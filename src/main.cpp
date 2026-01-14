#include <CLI/CLI.hpp>
#include <fmt/core.h>

struct LsOptions {
  bool all = false;                 // -a
  bool long_fmt = false;            // -l
  bool recursive = false;           // -R
  std::vector<std::string> targets; // ls <targets>
};

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

  // print parsing result
  fmt::print("(-a): {}\n", opts.all);
  fmt::print("(-l): {}\n", opts.long_fmt);
  fmt::print("(-R): {}\n", opts.recursive);
  fmt::print("# files: {}\n", opts.targets.size());

  for (const auto& target : opts.targets) {
    fmt::print("\t-> {}\n", target);
  }

  return 0;
}
