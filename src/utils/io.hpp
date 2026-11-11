// Origin: pkg/utils/io.go
//
// Line-by-line translation of lazygit's ForEachLineInFile plus its unexported
// forEachLineInStream helper (which lives in the .cpp).

#pragma once

#include <functional>
#include <string>

namespace lazygit {
namespace utils {

// ForEachLineInFile: upstream lines 9-19. Answers false exactly when upstream
// returned a non-nil error, which there only happens if the file cannot be
// opened. Upstream defers file.Close(); std::ifstream does that itself.
bool ForEachLineInFile(const std::string& path,
                       const std::function<void(const std::string&, int)>& f);

}  // namespace utils
}  // namespace lazygit
