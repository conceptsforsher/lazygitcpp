// Origin: pkg/utils/slice.go
//
// Upstream file: pkg/utils/slice.go (imports only golang.org/x/exp/slices,
// whose two uses here - Contains and Delete/Insert - are spelled out inline).

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace lazygit {
namespace utils {

// NextIndex: upstream lines 7-14. Upstream comment: "returns the index of the
// element that comes after the given number".
inline int NextIndex(const std::vector<int>& numbers, int current_number) {
  for (std::size_t index = 0; index < numbers.size(); ++index) {
    if (numbers[index] > current_number) {
      return static_cast<int>(index);
    }
  }
  return static_cast<int>(numbers.size()) - 1;
}

// PrevIndex: upstream lines 17-25. Upstream comment: "returns the index that
// comes before the given number, cycling if we reach the end".
inline int PrevIndex(const std::vector<int>& numbers, int current_number) {
  int end = static_cast<int>(numbers.size()) - 1;
  for (int i = end; i >= 0; --i) {
    if (numbers[static_cast<std::size_t>(i)] < current_number) {
      return i;
    }
  }
  return 0;
}

// NextIntInCycle: upstream lines 28-37.
inline int NextIntInCycle(const std::vector<int>& sl, int current) {
  for (std::size_t i = 0; i < sl.size(); ++i) {
    if (sl[i] == current) {
      if (i == sl.size() - 1) {
        return sl[0];
      }
      return sl[i + 1];
    }
  }
  return sl.empty() ? 0 : sl[0];
}

// PrevIntInCycle: upstream lines 40-51.
inline int PrevIntInCycle(const std::vector<int>& sl, int current) {
  for (std::size_t i = 0; i < sl.size(); ++i) {
    if (sl[i] == current) {
      if (i > 0) {
        return sl[i - 1];
      }
      return sl[sl.size() - 1];
    }
  }
  return sl.empty() ? 0 : sl[sl.size() - 1];
}

// StringArraysOverlap: upstream lines 53-61.
inline bool StringArraysOverlap(const std::vector<std::string>& str_arr_a,
                                const std::vector<std::string>& str_arr_b) {
  for (const std::string& first : str_arr_a) {
    if (std::find(str_arr_b.begin(), str_arr_b.end(), first) != str_arr_b.end()) {
      return true;
    }
  }
  return false;
}

// Limit: upstream lines 63-68.
inline std::vector<std::string> Limit(const std::vector<std::string>& values, int limit) {
  if (static_cast<int>(values.size()) > limit) {
    return std::vector<std::string>(values.begin(), values.begin() + limit);
  }
  return values;
}

// LimitStr: upstream lines 70-79. Go ranges over runes and cuts the string at a
// byte offset, so 'limit' counts code points, not bytes: the same walk over
// UTF-8 lead bytes is done here.
inline std::string LimitStr(const std::string& value, int limit) {
  int n = 0;
  for (std::size_t i = 0; i < value.size(); ++i) {
    if (n >= limit) {
      return value.substr(0, i);
    }
    // 0x10xxxxxx and higher are never lead bytes in UTF-8.
    const unsigned char c = static_cast<unsigned char>(value[i]);
    if ((c & 0xC0) != 0x80) {
      ++n;
    }
  }
  return value;
}

// MuiltiGroupBy: upstream lines 82-97 (the typo is upstream's).
template <typename T, typename K, typename F>
std::map<K, std::vector<T>> MuiltiGroupBy(const std::vector<T>& slice, F f) {
  std::map<K, std::vector<T>> result;
  for (const T& item : slice) {
    for (const K& key : f(item)) {
      result[key].push_back(item);
    }
  }
  return result;
}

// MoveElement: upstream lines 100-117. Upstream comment: "Returns a new slice
// with the element at index 'from' moved to index 'to'. Does not mutate
// original slice."
//
// Go's copy() tolerates the overlapping ranges these two calls use; the C++
// loops walk them in the direction that keeps every read ahead of every write.
template <typename T>
std::vector<T> MoveElement(const std::vector<T>& slice, int from, int to) {
  std::vector<T> new_slice(slice);

  if (from == to) {
    return new_slice;
  }

  if (from < to) {
    for (int i = from; i < to; ++i) {
      new_slice[static_cast<std::size_t>(i)] = new_slice[static_cast<std::size_t>(i + 1)];
    }
  } else {
    for (int i = from; i > to; --i) {
      new_slice[static_cast<std::size_t>(i)] = new_slice[static_cast<std::size_t>(i - 1)];
    }
  }

  new_slice[static_cast<std::size_t>(to)] = slice[static_cast<std::size_t>(from)];

  return new_slice;
}

// ValuesAtIndices: upstream lines 119-128. Upstream comment: "gracefully
// handling the situation where the index is out of bounds" - out of range keeps
// the zero value already in the result.
template <typename T>
std::vector<T> ValuesAtIndices(const std::vector<T>& slice, const std::vector<int>& indices) {
  std::vector<T> result(indices.size());
  for (std::size_t i = 0; i < indices.size(); ++i) {
    if (indices[i] >= 0 && static_cast<std::size_t>(indices[i]) < slice.size()) {
      result[i] = slice[static_cast<std::size_t>(indices[i])];
    }
  }
  return result;
}

// Partition: upstream lines 131-144. Upstream comment: "returns two slices: the
// first is for elements that pass the test, the second for those that don't."
// Upstream takes func(T) bool; the predicate type is deduced here so that a
// lambda can be passed straight through (std::function would block deduction).
template <typename T, typename Pred>
std::pair<std::vector<T>, std::vector<T>> Partition(const std::vector<T>& slice, Pred test) {
  std::vector<T> left;
  std::vector<T> right;
  left.reserve(slice.size());
  right.reserve(slice.size());

  for (const T& value : slice) {
    if (test(value)) {
      left.push_back(value);
    } else {
      right.push_back(value);
    }
  }

  return {std::move(left), std::move(right)};
}

// Prepend: upstream lines 147-152.
template <typename T, typename... Vs>
std::vector<T> Prepend(const std::vector<T>& slice, Vs&&... values) {
  std::vector<T> result{std::forward<Vs>(values)...};
  result.insert(result.end(), slice.begin(), slice.end());
  return result;
}

// Remove: upstream lines 155-158 (slices.Delete(slice, index, index+1)).
template <typename T>
std::vector<T> Remove(std::vector<T> slice, std::size_t index) {
  if (index < slice.size()) {
    slice.erase(slice.begin() + static_cast<std::ptrdiff_t>(index));
  }
  return slice;
}

// Move: upstream lines 161-166.
template <typename T>
std::vector<T> Move(std::vector<T> slice, std::size_t from_index, std::size_t to_index) {
  if (from_index >= slice.size()) {
    return slice;
  }
  T item = slice[from_index];  // upstream reads before deleting
  slice = Remove(std::move(slice), from_index);
  if (to_index > slice.size()) {
    to_index = slice.size();
  }
  slice.insert(slice.begin() + static_cast<std::ptrdiff_t>(to_index), std::move(item));
  return slice;
}

// Pop: upstream lines 169-175. Takes and returns the shortened slice because
// upstream expects callers to reassign the result.
template <typename T>
std::pair<T, std::vector<T>> Pop(std::vector<T> slice) {
  const std::size_t index = slice.size() - 1;
  T value = slice[index];
  slice.resize(index);
  return {std::move(value), std::move(slice)};
}

// Shift: upstream lines 178-183.
template <typename T>
std::pair<T, std::vector<T>> Shift(std::vector<T> slice) {
  T value = slice.front();
  slice.erase(slice.begin());
  return {std::move(value), std::move(slice)};
}

}  // namespace utils
}  // namespace lazygit
