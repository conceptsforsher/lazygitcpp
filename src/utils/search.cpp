// Origin: pkg/utils/search.go

#include "utils/search.hpp"

#include <algorithm>
#include <cctype>

namespace lazygit {
namespace utils {
namespace {

// Upstream's strings.Fields splits on any whitespace; this is the same walk.
bool IsSpace(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }

std::vector<std::string> Fields(const std::string& s) {
  std::vector<std::string> fields;
  std::string current;
  for (char c : s) {
    if (IsSpace(c)) {
      if (!current.empty()) {
        fields.push_back(current);
        current.clear();
      }
      continue;
    }
    current.push_back(c);
  }
  if (!current.empty()) {
    fields.push_back(current);
  }
  return fields;
}

std::string ToLower(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  }
  return out;
}

}  // namespace

// stringSource: upstream lines 19-27, an unexported fuzzy.Source over []string.
class StringSource : public fuzzy::Source {
 public:
  explicit StringSource(const std::vector<std::string>& data) : data_(data) {}

  std::string String(int i) const override { return data_[static_cast<std::size_t>(i)]; }

  int Len() const override { return static_cast<int>(data_.size()); }

 private:
  const std::vector<std::string>& data_;
};

std::vector<std::string> FilterStrings(const std::string& needle,
                                       const std::vector<std::string>& haystack,
                                       bool use_fuzzy_search) {
  if (needle.empty()) {
    return std::vector<std::string>();  // upstream: return []string{}
  }

  const fuzzy::Matches matches = Find(needle, haystack, use_fuzzy_search);

  // upstream: lo.Map(matches, func(match fuzzy.Match, _ int) string { return match.Str })
  std::vector<std::string> result;
  result.reserve(matches.size());
  for (const fuzzy::Match& match : matches) {
    result.push_back(match.str);
  }
  return result;
}

fuzzy::Matches FindSubstringsFrom(const std::string& pattern, const fuzzy::Source& data) {
  const std::vector<std::string> substrings = Fields(pattern);  // strings.Fields(pattern)
  fuzzy::Matches result;

  for (int i = 0; i < data.Len(); ++i) {
    const std::string s = data.String(i);

    bool all_present = true;
    for (const std::string& sub : substrings) {
      if (!CaseAwareContains(s, sub)) {
        all_present = false;  // upstream: continue outer
        break;
      }
    }
    if (!all_present) {
      continue;
    }

    fuzzy::Match match;
    match.str = s;
    match.index = i;
    result.push_back(match);
  }

  return result;
}

fuzzy::Matches FindSubstrings(const std::string& pattern, const std::vector<std::string>& data) {
  const StringSource source(data);
  return FindSubstringsFrom(pattern, source);
}

fuzzy::Matches Find(const std::string& pattern, const std::vector<std::string>& data,
                    bool use_fuzzy_search) {
  if (use_fuzzy_search) {
    return fuzzy::Find(pattern, data);
  }
  return FindSubstrings(pattern, data);
}

fuzzy::Matches FindFrom(const std::string& pattern, const fuzzy::Source& data,
                        bool use_fuzzy_search) {
  if (use_fuzzy_search) {
    return fuzzy::FindFrom(pattern, data);
  }
  return FindSubstringsFrom(pattern, data);
}

bool CaseAwareContains(const std::string& haystack, const std::string& needle) {
  if (ContainsUppercase(needle)) {
    return haystack.find(needle) != std::string::npos;  // strings.Contains
  }

  return CaseInsensitiveContains(haystack, needle);
}

bool ContainsUppercase(const std::string& s) {
  for (char c : s) {
    if (c >= 'A' && c <= 'Z') {
      return true;
    }
  }
  return false;
}

bool CaseInsensitiveContains(const std::string& haystack, const std::string& needle) {
  return ToLower(haystack).find(ToLower(needle)) != std::string::npos;
}

}  // namespace utils
}  // namespace lazygit
