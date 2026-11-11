// Origin: pkg/utils/io.go

#include "utils/io.hpp"

#include <fstream>
#include <istream>

namespace lazygit {
namespace utils {

namespace {

// forEachLineInStream: upstream lines 21-29, unexported.
//
// bufio's ReadString('\n') hands the line over with its terminator attached,
// while std::getline strips it, so the terminator is put back to keep the
// callback input identical to upstream's. The loop stops on the first read
// that yields nothing, which is upstream's `len(line) == 0` check.
void ForEachLineInStream(std::istream& reader,
                         const std::function<void(const std::string&, int)>& f) {
  for (int i = 0;; ++i) {
    std::string line;
    if (!std::getline(reader, line)) {
      break;
    }
    line.push_back('\n');
    f(line, i);
  }
}

}  // namespace

bool ForEachLineInFile(const std::string& path,
                       const std::function<void(const std::string&, int)>& f) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return false;  // upstream: return err from os.Open
  }

  ForEachLineInStream(file, f);

  return true;
}

}  // namespace utils
}  // namespace lazygit
