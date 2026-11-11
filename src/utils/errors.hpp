// Origin: pkg/utils/errors.go
//
// Upstream comment: "WrapError wraps an error for the sake of showing a stack
// trace at the top level. the go-errors package, for some reason, does not
// return nil when you try to wrap a non-error, so we're just doing it here".
//
// C++17 has no standard stack-trace facility (std::stacktrace arrives in
// C++23), so this translation keeps what the port can honour today: the
// nil-in/nil-out behaviour of upstream, and a payload for the message plus a
// stack trace once the project moves to C++23.
// TODO(C++23): fill stack_trace with std::stacktrace::current().

#pragma once

#include <optional>
#include <string>

namespace lazygit {
namespace utils {

struct Error {
  std::string message;
  std::string stack_trace;
};

// WrapError: upstream lines 8-14.
inline std::optional<Error> WrapError(const std::optional<Error>& err) {
  if (!err.has_value()) {
    return std::nullopt;
  }

  return Error{err->message, err->stack_trace};
}

}  // namespace utils
}  // namespace lazygit
