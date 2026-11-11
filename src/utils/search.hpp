// Origin: pkg/utils/search.go
//
// Upstream file: pkg/utils/search.go (imports github.com/sahilm/fuzzy and
// github.com/samber/lo; the fuzzy types come from the local shim in
// src/third_party/fuzzy, and lo.Map is a plain loop here).

#pragma once

#include <string>
#include <vector>

#include "third_party/fuzzy/fuzzy.hpp"

namespace lazygit {
namespace utils {

// FilterStrings: upstream lines 9-17. Upstream comment: returns the strings of
// the matches; an empty needle gives an empty slice, not the whole haystack.
std::vector<std::string> FilterStrings(const std::string& needle,
                                       const std::vector<std::string>& haystack,
                                       bool use_fuzzy_search);

// stringSource: upstream lines 19-27. Upstream comment: "Duplicated from the
// fuzzy package because it's private there".
fuzzy::Matches FindSubstrings(const std::string& pattern, const std::vector<std::string>& data);

// FindSubstringsFrom: upstream lines 33-51. Upstream comment: "Drop-in
// replacement for fuzzy.FindFrom (except that it doesn't fill out
// MatchedIndexes or Score, but we are not using these)".
fuzzy::Matches FindSubstringsFrom(const std::string& pattern, const fuzzy::Source& data);

// Find: upstream lines 53-58.
fuzzy::Matches Find(const std::string& pattern, const std::vector<std::string>& data,
                    bool use_fuzzy_search);

// FindFrom: upstream lines 60-65.
fuzzy::Matches FindFrom(const std::string& pattern, const fuzzy::Source& data,
                        bool use_fuzzy_search);

// CaseAwareContains: upstream lines 67-75. Upstream comment: "if needle
// contains an uppercase letter, we'll do a case sensitive search".
bool CaseAwareContains(const std::string& haystack, const std::string& needle);

// ContainsUppercase: upstream lines 77-85.
bool ContainsUppercase(const std::string& s);

// CaseInsensitiveContains: upstream lines 87-93.
bool CaseInsensitiveContains(const std::string& haystack, const std::string& needle);

}  // namespace utils
}  // namespace lazygit
