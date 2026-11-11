// Origin: pkg/utils/template.go

#pragma once

#include <map>
#include <optional>
#include <string>

namespace lazygit {
namespace utils {

// ResolveTemplate: upstream lines 9-21. Upstream parses with Go's
// text/template and Option("missingkey=error").
//
// This translation covers the subset lazygit feeds to it - plain {{name}} and
// {{.name}} substitutions over a map - and reports a missing key as an error,
// which is what "missingkey=error" does. It is deliberately not a text/template
// clone: TODO(translation) add actions, pipelines and conditionals when a
// command template in pkg/config needs them.
std::optional<std::string> ResolveTemplate(const std::string& template_str,
                                           const std::map<std::string, std::string>& object);

// ResolvePlaceholderString: upstream lines 23-33. Upstream builds an
// oldnew slice of {{key}}, value, {{.key}}, value and lets strings.NewReplacer
// do one pass; this does the same scan in a single pass over the input.
std::string ResolvePlaceholderString(const std::string& str,
                                     const std::map<std::string, std::string>& arguments);

}  // namespace utils
}  // namespace lazygit
