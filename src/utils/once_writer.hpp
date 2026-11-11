// Origin: pkg/utils/once_writer.go
//
// Upstream comment: "This wraps a writer and ensures that before we actually
// write anything we call a given function first."
//
// Go's io.Writer.Write is translated as writing into a std::ostream; sync.Once
// becomes std::call_once.

#pragma once

#include <functional>
#include <mutex>
#include <ostream>
#include <utility>

namespace lazygit {
namespace utils {

class OnceWriter {
 public:
  using BeforeWrite = std::function<void()>;

  // NewOnceWriter: upstream lines 18-23.
  OnceWriter(std::ostream& writer, BeforeWrite f) : writer_(writer), f_(std::move(f)) {}

  // Write: upstream lines 25-31. The wrapper function runs exactly once, no
  // matter how often or from where Write is called.
  std::ostream& Write(const char* data, std::streamsize count) {
    std::call_once(once_, [this]() {
      if (f_) {
        f_();
      }
    });

    return writer_.write(data, count);
  }

 private:
  std::ostream& writer_;
  std::once_flag once_;
  BeforeWrite f_;
};

}  // namespace utils
}  // namespace lazygit
