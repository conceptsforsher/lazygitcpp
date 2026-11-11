// Origin: pkg/utils/regexp.go

#include "utils/regexp.hpp"

namespace lazygit {
namespace utils {

Regexp CompileRegexp(const std::string& go_pattern) {
  std::string pattern;
  std::vector<std::string> names(1, "");  // names[0] is the whole match

  for (std::size_t i = 0; i < go_pattern.size();) {
    // Escapes are copied verbatim so that a literal "(" never looks like a group.
    if (go_pattern[i] == '\\' && i + 1 < go_pattern.size()) {
      pattern.push_back(go_pattern[i]);
      pattern.push_back(go_pattern[i + 1]);
      i += 2;
      continue;
    }

    if (go_pattern[i] == '(' && i + 1 < go_pattern.size() && go_pattern[i + 1] == '?') {
      std::size_t j = i + 2;
      if (j < go_pattern.size() && go_pattern[j] == 'P') {
        ++j;
      }
      if (j < go_pattern.size() && go_pattern[j] == '<') {
        std::size_t closing = go_pattern.find('>', j);
        if (closing != std::string::npos) {
          names.push_back(go_pattern.substr(j + 1, closing - j - 1));
          pattern.push_back('(');
          i = closing + 1;
          continue;
        }
      }
      // Any other (?...) construct is left alone.
    }

    pattern.push_back(go_pattern[i]);
    ++i;
  }

  return Regexp{std::regex(pattern, std::regex::ECMAScript), std::move(names)};
}

std::unordered_map<std::string, std::string> FindNamedMatches(const Regexp& regex,
                                                              const std::string& str) {
  std::smatch match;
  if (!std::regex_search(str, match, regex.re)) {
    return {};  // upstream: return nil when len(match) == 0
  }

  std::unordered_map<std::string, std::string> results;
  for (std::size_t i = 1; i < match.size(); ++i) {
    const std::string& name = i < regex.subexp_names.size() ? regex.subexp_names[i] : std::string();
    results[name] = match[i].str();
  }

  return results;
}

}  // namespace utils
}  // namespace lazygit
