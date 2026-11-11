// Origin: pkg/utils/string_pool.go
//
// Upstream comment: "A simple string pool implementation that can help reduce
// memory usage for cases where the same string is used multiple times."
//
// Upstream embeds sync.Map and LoadOrStore's the *string; this translation
// keeps the same interning semantics (one canonical copy per distinct string,
// pointer stable for as long as the pool lives) behind a mutex.

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace lazygit {
namespace utils {

class StringPool {
 public:
  // Add: upstream lines 11-14. Returns the canonical string, so callers can
  // store the pointer instead of their own copy.
  const std::string* Add(const std::string& s) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto it = pool_.find(s);
    if (it != pool_.end()) {
      return it->second.get();
    }

    auto stored = std::make_unique<std::string>(s);
    const std::string* interned = stored.get();
    pool_.emplace(s, std::move(stored));
    return interned;
  }

 private:
  std::mutex mutex_;
  std::unordered_map<std::string, std::unique_ptr<std::string>> pool_;
};

}  // namespace utils
}  // namespace lazygit
