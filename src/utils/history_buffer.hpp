// Origin: pkg/utils/history_buffer.go
//
// Line-by-line translation of lazygit's HistoryBuffer: newest item first,
// bounded by maxSize.
//
// Upstream returns (T, error); here a missing value is std::nullopt and each
// upstream error branch is marked at the point where it is returned.

#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

namespace lazygit {
namespace utils {

template <typename T>
class HistoryBuffer {
 public:
  // NewHistoryBuffer: upstream lines 12-17.
  explicit HistoryBuffer(int max_size) : max_size_(max_size) {
    items_.reserve(static_cast<std::size_t>(max_size > 0 ? max_size : 0));
  }

  // Push: upstream lines 19-24. The buffer is full when it holds maxSize
  // items, and then the oldest one is dropped before inserting at the front.
  void Push(T item) {
    if (static_cast<int>(items_.size()) == max_size_) {
      items_.pop_back();
    }
    items_.insert(items_.begin(), std::move(item));
  }

  // PeekAt: upstream lines 26-37. index == -1 asks for nothing at all, which
  // upstream answers with the zero value and no error.
  std::optional<T> PeekAt(int index) const {
    std::optional<T> item;  // upstream: var item T

    if (items_.empty()) {
      return item;  // upstream: errors.New("Buffer is empty")
    }
    if (index < -1 || (index >= 0 && static_cast<std::size_t>(index) >= items_.size())) {
      return item;  // upstream: errors.New("Index out of range")
    }
    if (index == -1) {
      return item;
    }
    return items_[static_cast<std::size_t>(index)];
  }

  std::size_t Size() const { return items_.size(); }

 private:
  int max_size_;
  std::vector<T> items_;
};

}  // namespace utils
}  // namespace lazygit
