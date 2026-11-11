// Origin: pkg/utils/regexp.go
//
// Go's regexp package carries named capture groups in the compiled expression,
// std::regex does not. To keep FindNamedMatches usable with lazygit's patterns
// the translation keeps the same information next to the compiled regexp (the
// sub-expression names, ordered like Go's SubexpNames()) and rewrites Go's
// (?P<name> / (?<name> spelling into a plain group before compiling.

#pragma once

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace lazygit {
namespace utils {

struct Regexp {
  std::regex re;
  // subexp_names[0] is the whole match, exactly like Go's SubexpNames()[0].
  std::vector<std::string> subexp_names;
};

// Compiles a Go-flavoured pattern. Throws std::regex_error where Go's
// regexp.Compile returns an error.
Regexp CompileRegexp(const std::string& go_pattern);

// FindNamedMatches: upstream lines 5-16. An empty result stands for upstream's
// nil map, which is what it returns when nothing matched.
std::unordered_map<std::string, std::string> FindNamedMatches(const Regexp& regex,
                                                              const std::string& str);

}  // namespace utils
}  // namespace lazygit
