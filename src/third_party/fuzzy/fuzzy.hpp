// Origin: (shim for github.com/sahilm/fuzzy)
//
// lazygit reaches for the third-party fuzzy package in pkg/utils/search.go;
// nothing else in the port may pull in a Go module, so this stands in for it.
//
// Faithful in shape (Match/Matches/Source, Find and FindFrom with results
// ordered by score) and an approximation in scoring: upstream scores
// consecutive and camel-boundary matches with a fairly involved heuristic.
// TODO(translation): port that scoring once pkg/gui shows MatchedIndexes.

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace lazygit {
namespace fuzzy {

// upstream: fuzzy.Match
struct Match {
  std::string str;
  int index = -1;
  std::vector<std::size_t> matched_indexes;
  int score = 0;
};

// upstream: fuzzy.Matches
using Matches = std::vector<Match>;

// upstream: fuzzy.Source, an interface rather than a concrete slice.
class Source {
 public:
  virtual ~Source() = default;
  virtual std::string String(int i) const = 0;
  virtual int Len() const = 0;
};

// fuzzy.SourceFromStrings / the []string shortcut upstream uses.
class StringSource : public Source {
 public:
  explicit StringSource(std::vector<std::string> data) : data_(std::move(data)) {}

  std::string String(int i) const override { return data_[static_cast<std::size_t>(i)]; }

  int Len() const override { return static_cast<int>(data_.size()); }

 private:
  std::vector<std::string> data_;
};

// upstream: fuzzy.Find / fuzzy.FindFrom. Both answer matches ordered by
// descending score, exactly like upstream sorts them.
Matches Find(const std::string& pattern, const std::vector<std::string>& data);
Matches FindFrom(const std::string& pattern, const Source& source);

}  // namespace fuzzy
}  // namespace lazygit
