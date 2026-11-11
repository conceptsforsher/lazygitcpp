// Origin: pkg/utils/project_root.go

#include "utils/project_root.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>  // getcwd; upstream's os.Getwd on a POSIX build

namespace lazygit {
namespace utils {
namespace {

// filepath.Dir, for the parent-directory walk.
std::string Dir(const std::string& path) {
  if (path == "/") {
    return "/";
  }
  const std::size_t slash = path.find_last_of('/');
  if (slash == std::string::npos) {
    return ".";
  }
  if (slash == 0) {
    return "/";
  }
  return path.substr(0, slash);
}

// filepath.Join(dir, "go.mod"), the only join upstream makes.
std::string Join(const std::string& dir, const std::string& name) {
  if (dir.empty() || dir.back() == '/') {
    return dir + name;
  }
  return dir + "/" + name;
}

std::vector<std::string> SplitOn(const std::string& s, char separator) {
  std::vector<std::string> parts;
  std::string current;
  for (char c : s) {
    if (c == separator) {
      parts.push_back(current);
      current.clear();
      continue;
    }
    current.push_back(c);
  }
  parts.push_back(current);
  return parts;
}

// strings.Fields
std::vector<std::string> Fields(const std::string& s) {
  std::vector<std::string> fields;
  std::string current;
  for (char c : s) {
    if (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r') {
      if (!current.empty()) {
        fields.push_back(current);
        current.clear();
      }
      continue;
    }
    current.push_back(c);
  }
  if (!current.empty()) {
    fields.push_back(current);
  }
  return fields;
}

// declaresLazygitModule: upstream lines 55-67, unexported. Upstream comment: "A
// fork is free to rename the module, so we accept any module path with a
// 'lazygit' element in it, e.g. github.com/jesseduffield/lazygit."
bool DeclaresLazygitModule(const std::string& go_mod_path) {
  std::ifstream file(go_mod_path);
  if (!file.is_open()) {
    return false;  // upstream: os.ReadFile error -> false
  }

  std::ostringstream contents;
  contents << file.rdbuf();

  const std::vector<std::string> lines = SplitOn(contents.str(), '\n');
  for (const std::string& line : lines) {
    const std::vector<std::string> fields = Fields(line);
    if (fields.size() >= 2 && fields[0] == "module") {
      // upstream: lo.SomeBy(strings.Split(fields[1], "/"), ... "lazygit")
      const std::vector<std::string> path_elements = SplitOn(fields[1], '/');
      for (const std::string& element : path_elements) {
        if (element == "lazygit") {
          return true;
        }
      }
    }
  }

  return false;
}

std::string NotFoundMessage(const std::string& start_dir) {
  return "failed to find the lazygit root directory: there is no go.mod for a lazygit module in " +
         start_dir + " or any of its parent directories";
}

}  // namespace

std::optional<std::string> FindLazygitRootDirectory(std::string* error_message) {
  char buffer[4096];
  if (getcwd(buffer, sizeof(buffer)) == nullptr) {
    return std::nullopt;  // upstream: return "", err
  }

  const std::string start_dir(buffer);
  std::string dir = start_dir;

  while (true) {
    if (DeclaresLazygitModule(Join(dir, "go.mod"))) {
      return dir;
    }

    const std::string parent = Dir(dir);
    if (parent == dir) {
      if (error_message != nullptr) {
        *error_message = NotFoundMessage(start_dir);
      }
      return std::nullopt;
    }
    dir = parent;
  }
}

std::string MustFindLazygitRootDirectory() {
  std::string error_message;
  const std::optional<std::string> dir = FindLazygitRootDirectory(&error_message);
  if (!dir) {
    // upstream: log.Fatal(err)
    std::fprintf(stderr, "%s\n", error_message.c_str());
    std::exit(1);
  }
  return *dir;
}

}  // namespace utils
}  // namespace lazygit
