#include <ctime>
#include <filesystem>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>

#include <CLI/CLI.hpp>
#include <fmt/core.h>

namespace fs = std::filesystem;

struct LsOptions {
  bool all = false;                 // -a
  bool long_fmt = false;            // -l
  bool recursive = false;           // -R
  std::vector<std::string> targets; // ls <targets>
};

struct FileInfo {
  bool is_directory;
  fs::perms perms;
  nlink_t nlink;
  uid_t uid;
  std::string owner_name;
  gid_t gid;
  std::string group_name;
  size_t size;
  std::string date_str;
  std::string filename;
};

std::unordered_map<uid_t, std::string> user_cache;
std::unordered_map<gid_t, std::string> group_cache;

std::string format_time(time_t mtime) {
  time_t now = time(nullptr);
  const time_t six_months = 365 / 2 * 86400;
  struct tm *tm_info = localtime(&mtime);

  char buffer[80];
  bool is_old_or_future = (mtime > now) || ((now - mtime) > six_months);

  if (is_old_or_future) {
    strftime(buffer, sizeof(buffer), "%b %e  %Y", tm_info);
  } else {
    strftime(buffer, sizeof(buffer), "%b %e %H:%M", tm_info);
  }

  return std::string(buffer);
}

std::string format_perms(fs::perms perms, bool is_directory) {
  std::string str = "----------";

  if (is_directory)
    str[0] = 'd';

  // user
  if ((perms & fs::perms::owner_read) != fs::perms::none)
    str[1] = 'r';
  if ((perms & fs::perms::owner_write) != fs::perms::none)
    str[2] = 'w';
  if ((perms & fs::perms::owner_exec) != fs::perms::none)
    str[3] = 'x';

  // group
  if ((perms & fs::perms::group_read) != fs::perms::none)
    str[4] = 'r';
  if ((perms & fs::perms::group_write) != fs::perms::none)
    str[5] = 'w';
  if ((perms & fs::perms::group_exec) != fs::perms::none)
    str[6] = 'x';

  // other
  if ((perms & fs::perms::others_read) != fs::perms::none)
    str[7] = 'r';
  if ((perms & fs::perms::others_write) != fs::perms::none)
    str[8] = 'w';
  if ((perms & fs::perms::others_exec) != fs::perms::none)
    str[9] = 'x';

  return str;
}

void print_file_infos(std::vector<struct FileInfo> &infos, bool long_fmt) {
  if (!long_fmt) {
    for (const auto &info : infos) {
      fmt::println("{}", info.filename);
    }
    return;
  }

  // calculate width
  size_t nlink_width = 0;
  size_t owner_width = 0;
  size_t group_width = 0;
  size_t size_width = 0;

  for (auto &info : infos) {
    nlink_width = std::max(nlink_width, std::to_string(info.nlink).size());
    owner_width = std::max(owner_width, info.owner_name.size());
    group_width = std::max(group_width, info.group_name.size());
    size_width = std::max(size_width, std::to_string(info.size).size());
  }

  for (const auto &info : infos) {
    fmt::println("{} {:>{}} {:<{}}  {:<{}}  {:>{}} {:>12} {}",
                 format_perms(info.perms, info.is_directory), info.nlink,
                 nlink_width, info.owner_name, owner_width, info.group_name,
                 group_width, info.size, size_width, info.date_str,
                 info.filename);
  }
}

void list_directory(const fs::path &path, const LsOptions &opts) {
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

  if (opts.all) {
    entries.push_back(".");
    entries.push_back("..");
  }

  try {
    for (const auto &entry : fs::directory_iterator(path)) {
      std::string filename = entry.path().filename();

      if (!opts.all && filename.starts_with(".")) {
        // ignore hidden file
        continue;
      }

      entries.push_back(entry.path());
    }
  } catch (const fs::filesystem_error &err) {
    fmt::println(stdout, "ls: cannot open directory {}", path.string());
    return;
  }

  // print entries
  // by default, ls sorts file by name
  std::sort(entries.begin(), entries.end(), [](const fs::path& a, const fs::path& b) {
    return strcoll(a.filename().c_str(), b.filename().c_str()) < 0;
  });

  std::vector<struct FileInfo> file_infos(entries.size());

  // fill in file info
  for (int i = 0; i < entries.size(); i++) {
    fs::path entry = entries[i];
    struct FileInfo &info = file_infos[i];

    // TODO: list files in grid layout
    info.filename = entry.filename();
    if (opts.long_fmt) {
      struct stat stat;
      if (lstat(entry.string().c_str(), &stat) == -1) {
        perror("lstat");
        continue;
      }

      info.is_directory = S_ISDIR(stat.st_mode);
      info.perms = static_cast<fs::perms>(stat.st_mode);

      info.nlink = stat.st_nlink;
      info.uid = stat.st_uid;
      info.gid = stat.st_gid;
      info.size = stat.st_size;
      info.date_str = format_time(stat.st_mtime);

      if (user_cache.find(info.uid) == user_cache.end()) {
        struct passwd *pw = getpwuid(info.uid);
        user_cache[info.uid] = pw ? pw->pw_name : std::to_string(info.uid);
      }
      info.owner_name = user_cache[info.uid];

      if (group_cache.find(info.gid) == group_cache.end()) {
        struct group *gr = getgrgid(info.gid);
        group_cache[info.gid] = gr ? gr->gr_name : std::to_string(info.gid);
      }
      info.group_name = group_cache[info.gid];
    }
  }

  print_file_infos(file_infos, opts.long_fmt);
}

int main(int argc, char **argv) {
  // initialize locale for filename sorting with strcoll
  std::setlocale(LC_ALL, "");

  // initialize CLI11 app
  CLI::App app{"ls rewritten in C++", "lscpp"};
  // convert argv to UTF-8 encoding, only on Windows
  argv = app.ensure_utf8(argv);

  LsOptions opts;

  // parse arguments, bind cli arguments to LsOptions fields
  app.add_flag("-a,--all", opts.all,
               "Include directory entries whose names begin with a dot ('.')");
  app.add_flag("-l", opts.long_fmt,
               "(The lowercase letter “ell”.) List files in the long format, "
               "as described in the The Long Format subsection below.");
  app.add_flag("-R,--recursive", opts.recursive,
               "Recursively list subdirectories encountered.");
  // positional arguments, targets
  app.add_option("file", opts.targets, "Files or directories to list");
  CLI11_PARSE(app, argc, argv);

  // if no file specified, use "." by default
  if (opts.targets.empty()) {
    opts.targets.push_back(".");
  }

  bool multi_target = opts.targets.size() > 1;

  for (const auto &target : opts.targets) {
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
