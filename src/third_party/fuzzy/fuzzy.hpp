// Origin: (shim for github.com/sahilm/fuzzy)\n//\n// lazygit reaches for the third-party fuzzy package in pkg/utils/search.go;\n// nothing else in the port may pull in a Go module, so this stands in for it.\n//\n// Faithful in shape (Match/Matches/Source, Find and FindFrom with sorted\n// results) and an approximation in scoring: upstream scores consecutive and\n// camel-boundary matches with a fairly involved heuristic. TODO(translation):\n// port that scoring once pkg/gui starts showing MatchedIndexes.\n\n#pragma once\n\n#include <cstddef>\n#include <string>\n#include <vector>\n\nnamespace lazygit {\nnamespace fuzzy {\n\n// upstream: fuzzy.Match\nstruct Match {\n  std::string str;\n  int index = -1;\n  std::vector<std::size_t> matched_indexes;\n  int score = 0;\n};\n\n// upstream: fuzzy.Matches\nusing Matches = std::vector<Match>;\n\n// upstream: fuzzy.Source, an interface rather than a concrete slice.\nclass Source {\n public:\n  virtual ~Source() = default;\n  virtual std::string String(int i) const = 0;\n  virtual int Len() const = 0;\n};\n\n// fuzzy.SourceFromStrings / the []string shortcut upstream uses.\nclass StringSource : public Source {\n public:\n  explicit StringSource(std::vector<std::string> data) : data_(std::move(data)) {}\n\n  std::string String(int i) const override {\n    return data_[static_cast<std::size_t>(i)];\n  }\n\n  int Len() const override { return static_cast<int>(data_.size()); }\n\n private:\n  std::vector<std::string> data_;\n};\n\n// upstream: fuzzy.Find / fuzzy.FindFrom. Both answer the matches ordered by\n// descending score, exactly like upstream sorts them.\nMatches Find(const std::string& pattern, const std::vector<std::string>& data);\nMatches FindFrom(const std::string& pattern, const Source& source);\n\n}  // namespace fuzzy\n}  // namespace lazygit\n","path":"/tmp/opencode/work/lazygitcpp/src/third_party/fuzzy/fuzzy.hpp"})
[Tool error]: Failed: content has escaped newlines; the content is literal text.

Escaped sequences such as \n inside strings are intentional part of file content?  The tool rejected content because it contained backslash-n sequences that look like escapes. If you write files that contain literal \n sequences (for source code with "\n" text), use write with content that has actual line breaks, and any literal backslash-n inside string literals must be written as a real "\n"? The tool rejects content with the two-character sequence backslash + n when it appears as an escape in JSON. The problem: my JSON string contained literal "\n" inside the comment text "MatchedIndexes"? Whatever. I should write file content with real newlines and avoid backslash-n two-char sequences in text.

In the fuzzy.hpp there is no \n in strings... Actually the content I passed had escaped \n because I wrote them as \n within a JSON string where I intended real newlines? I typed literal "\n" sequences in the comment? The first line: `// Origin: (shim for github.com/sahilm/fuzzy)\n//` — yes I included trailing \n escapes as text. I must instead use actual line breaks.

Let me rewrite with real newlines (press enter equivalents). I'll produce content with genuine newlines.</think:6124c78e><tool_calls:6124c78e><tool_call:6124c78e>write<arg_key:6124c78e>content</arg_key:6124c78e><arg_value:6124c78e>// Origin: (shim for github.com/sahilm/fuzzy)
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
