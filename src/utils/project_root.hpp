// Origin: pkg/utils/project_root.go

#pragma once

#include <optional>
#include <string>

namespace lazygit {
namespace utils {

// FindLazygitRootDirectory: upstream lines 20-41. Upstream comment: "returns
// the root directory of the lazygit source tree, by searching the working
// directory and its parents for the go.mod file that declares lazygit's
// module. Only development tools use it ... We search upwards rather than
// expect to be called from the root directory".
//
// nullopt stands for upstream's ("" , err); when error_message is given it
// receives upstream's fmt.Errorf text, which MustFindLazygitRootDirectory has
// to report. os.Getwd failing is the other error path.
std::optional<std::string> FindLazygitRootDirectory(std::string* error_message = nullptr);

// MustFindLazygitRootDirectory: upstream lines 44-52. Upstream comment: "is
// FindLazygitRootDirectory for tools that can't do anything useful if the
// directory isn't found." Upstream aborts through log.Fatal(err); because C++
// exceptions would change every caller, this writes to stderr and exits 1.
std::string MustFindLazygitRootDirectory();

}  // namespace utils
}  // namespace lazygit
