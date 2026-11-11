// Origin: pkg/utils/stack.go
//
// Line-by-line translation of lazygit's Stack. Pop() answers with the zero
// value when the stack is empty, which std::optional expresses directly.

#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

namespace lazygit {
namespace utils {

template <typename T>
class Stack {
 public:
  // Push: upstream lines 7-9.
  void Push(T item) { stack_.push_back(std::move(item)); }

  // Pop: upstream lines 11-20.
  std::optional<T> Pop() {
    if (stack_.empty()) {
      return std::nullopt;  // upstream: var zero T; return zero
    }
    T last = std::move(stack_.back());
    stack_.pop_back();
    return last;
  }

  // IsEmpty: upstream lines 22-24.
  bool IsEmpty() const { return stack_.empty(); }

  // Clear: upstream lines 26-28 (`stack = nil`).
  void Clear() { stack_.clear(); }

  std::size_t Size() const { return stack_.size(); }

 private:
  std::vector<T> stack_;
};

}  // namespace utils
}  // namespace lazygit
