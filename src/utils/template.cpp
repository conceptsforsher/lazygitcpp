// Origin: pkg/utils/template.go

#include "utils/template.hpp"

#include <cctype>

namespace lazygit {
namespace utils {

namespace {

const std::string kOpen = "{{";
const std::string kClose = "}}";

bool IsSpace(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }

}  // namespace

std::optional<std::string> ResolveTemplate(const std::string& template_str,
                                           const std::map<std::string, std::string>& object) {
  std::string result;
  result.reserve(template_str.size());

  std::size_t i = 0;
  while (i < template_str.size()) {
    std::size_t open = template_str.find(kOpen, i);
    if (open == std::string::npos) {
      result.append(template_str, i, std::string::npos);
      break;
    }

    std::size_t close = template_str.find(kClose, open + kOpen.size());
    if (close == std::string::npos) {
      // Unbalanced braces: Go's parser rejects the template too.
      return std::nullopt;
    }

    result.append(template_str, i, open - i);

    std::string key = template_str.substr(open + kOpen.size(), close - open - kOpen.size());
    // Trim the whitespace Go allows inside the action, then drop the leading
    // dot of the {{.Field}} form.
    while (!key.empty() && IsSpace(key.front())) {
      key.erase(key.begin());
    }
    while (!key.empty() && IsSpace(key.back())) {
      key.pop_back();
    }
    if (!key.empty() && key.front() == '.') {
      key.erase(key.begin());
    }

    // Option("missingkey=error"): an unknown key makes the whole call fail.
    auto it = object.find(key);
    if (it == object.end()) {
      return std::nullopt;
    }

    result.append(it->second);
    i = close + kClose.size();
  }

  return result;
}

std::string ResolvePlaceholderString(const std::string& str,
                                     const std::map<std::string, std::string>& arguments) {
  std::string result;
  result.reserve(str.size());

  std::size_t i = 0;
  while (i < str.size()) {
    std::size_t open = str.find(kOpen, i);
    if (open == std::string::npos) {
      result.append(str, i, std::string::npos);
      break;
    }

    std::size_t close = str.find(kClose, open + kOpen.size());
    if (close == std::string::npos) {
      result.append(str, i, std::string::npos);
      break;
    }

    result.append(str, i, open - i);

    std::string key = str.substr(open + kOpen.size(), close - open - kOpen.size());
    if (!key.empty() && key.front() == '.') {
      key.erase(key.begin());
    }

    auto it = arguments.find(key);
    if (it != arguments.end()) {
      result.append(it->second);  // upstream: "{{"+key+"}}" -> value
    } else {
      result.append(str, open, close + kClose.size() - open);
    }

    i = close + kClose.size();
  }

  return result;
}

}  // namespace utils
}  // namespace lazygit
