// Origin: (shim for github.com/sahilm/fuzzy)

#include "third_party/fuzzy/fuzzy.hpp"

#include <algorithm>
#include <cctype>
#include <optional>

namespace lazygit {
namespace fuzzy {
namespace {

std::string ToLower(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  }
  return out;
}

// uppercased versions are compared case-insensitively, which is what upstream's
// fuzzy matcher does when the pattern itself has no uppercase letter.
std::optional<Match> MatchItem(const std::string& lower_pattern, const std::string& item,
                               int index) {
  const std::string lower_item = ToLower(item);

  Match match;
  match.str = item;
  match.index = index;

  std::size_t item_pos = 0;
  int previous_match = -1;
  for (std::size_t p = 0; p < lower_pattern.size(); ++p) {
    const std::size_t found = lower_item.find(lower_pattern[p], item_pos);
    if (found == std::string::npos) {
      return std::nullopt;  // not every character of the pattern is present
    }

    const int position = static_cast<int>(found);
    match.matched_indexes.push_back(found);
    match.score += 1;
    if (previous_match == position - 1) {
      match.score += 3;  // consecutive characters, like upstream's bonus
    }
    if (position == 0) {
      match.score += 5;  // match at the start of the string
    }
    previous_match = position;
    item_pos = found + 1;
  }

  return match;
}

}  // namespace

Matches FindFrom(const std::string& pattern, const Source& source) {
  Matches matches;

  if (pattern.empty()) {
    return matches;
  }

  const std::string lower_pattern = ToLower(pattern);
  for (int i = 0; i < source.Len(); ++i) {
    if (auto match = MatchItem(lower_pattern, source.String(i), i)) {
      matches.push_back(*match);
    }
  }

  std::stable_sort(matches.begin(), matches.end(),
                   [](const Match& a, const Match& b) { return a.score > b.score; });

  return matches;
}

Matches Find(const std::string& pattern, const std::vector<std::string>& data) {
  const StringSource source(std::vector<std::string>(data));
  return FindFrom(pattern, source);
}

}  // namespace fuzzy
}  // namespace lazygit
