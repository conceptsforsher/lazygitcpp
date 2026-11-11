// Origin: pkg/utils/thread_safe_map.go
//
// Line-by-line translation of lazygit's generic ThreadSafeMap.
//
//   Go                          C++
//   --------------------------  -------------------------------------------
//   sync.RWMutex                mutable std::shared_mutex
//   map[K]V                     std::map<K, V>
//   (V, bool)                   std::optional<V>
//
// Upstream iterates a Go map for Keys()/Values(), whose order is random by
// language definition; std::map gives a deterministic order instead, which is
// the only intentional behavioural difference.

#pragma once

#include <map>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>
#include <vector>

namespace lazygit {
namespace utils {

template <typename K, typename V>
class ThreadSafeMap {
 public:
  ThreadSafeMap() = default;

  // Get: upstream lines 17-23.
  std::optional<V> Get(const K& key) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    auto it = inner_map_.find(key);
    if (it == inner_map_.end()) {
      return std::nullopt;
    }
    return it->second;
  }

  // Set: upstream lines 25-30.
  void Set(const K& key, V value) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    inner_map_.insert_or_assign(key, std::move(value));
  }

  // Delete: upstream lines 32-37.
  void Delete(const K& key) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    inner_map_.erase(key);
  }

  // Keys: upstream lines 39-49.
  std::vector<K> Keys() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    std::vector<K> keys;
    keys.reserve(inner_map_.size());
    for (const auto& entry : inner_map_) {
      keys.push_back(entry.first);
    }
    return keys;
  }

  // Values: upstream lines 51-61.
  std::vector<V> Values() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    std::vector<V> values;
    values.reserve(inner_map_.size());
    for (const auto& entry : inner_map_) {
      values.push_back(entry.second);
    }
    return values;
  }

  // Len: upstream lines 63-68.
  std::size_t Len() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    return inner_map_.size();
  }

  // Clear: upstream lines 70-75 (`innerMap = make(map[K]V)`).
  void Clear() {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    inner_map_.clear();
  }

  // IsEmpty: upstream lines 77-82.
  bool IsEmpty() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    return inner_map_.empty();
  }

  // Has: upstream lines 84-90.
  bool Has(const K& key) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    return inner_map_.count(key) != 0;
  }

 private:
  mutable std::shared_mutex mutex_;
  std::map<K, V> inner_map_;
};

}  // namespace utils
}  // namespace lazygit
