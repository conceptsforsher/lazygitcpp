// Origin: (none) - translation harness, this file is not an upstream file.
//
// Headers are not translation units: without this file the g++/clang++ build in
// .github/workflows/build.yml would never compile the translated headers at
// all, and templates would never even be instantiated. It includes every
// translated header and exercises each one, so CI really checks them.

#include "utils/date.hpp"
#include "utils/errors.hpp"
#include "utils/history_buffer.hpp"
#include "utils/io.hpp"
#include "utils/once_writer.hpp"
#include "utils/project_root.hpp"
#include "utils/regexp.hpp"
#include "utils/search.hpp"
#include "utils/slice.hpp"
#include "utils/stack.hpp"
#include "utils/string_pool.hpp"
#include "utils/template.hpp"
#include "utils/thread_safe_map.hpp"

#include <ctime>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace lazygit {
namespace utils {
namespace {

[[maybe_unused]] void InstantiateTranslatedHeaders() {
  ThreadSafeMap<std::string, int> map;
  map.Set("status", 1);
  (void)map.Get("status");
  (void)map.Keys();
  (void)map.Values();
  (void)map.Len();
  (void)map.IsEmpty();
  (void)map.Has("status");
  map.Delete("status");
  map.Clear();

  HistoryBuffer<int> history(3);
  history.Push(1);
  (void)history.PeekAt(0);
  (void)history.PeekAt(-1);
  (void)history.Size();

  Stack<int> stack;
  stack.Push(1);
  (void)stack.Pop();
  (void)stack.IsEmpty();
  stack.Clear();

  StringPool pool;
  const std::string* interned = pool.Add("lazygit");
  (void)interned;

  std::ostringstream sink;
  OnceWriter writer(sink, []() {});
  writer.Write("lazygit", 7);

  (void)WrapError(std::optional<Error>());
  (void)WrapError(Error{"boom", ""});

  (void)ResolveTemplate("git {{.cmd}}", {{"cmd", "status"}});
  (void)ResolvePlaceholderString("{{.who}} {{who}}", {{"who", "lazygit"}});

  (void)ForEachLineInFile("no-such-file", [](const std::string&, int) {});

  (void)FindNamedMatches(CompileRegexp("(?P<name>[a-z]+)"), "lazygit");

  (void)UnixToTimeAgo(1700000000);
  (void)UnixToDateSmart(std::time(nullptr), 1700000000, "02 Jan 06", "3:04PM");
  (void)GoLayoutToStrftime("2006-01-02 15:04:05");
  (void)FormatGoLayout(1700000000, "02 Jan 06");

  (void)NextIndex({1, 3, 5}, 2);
  (void)PrevIndex({1, 3, 5}, 4);
  (void)NextIntInCycle({1, 3, 5}, 5);
  (void)PrevIntInCycle({1, 3, 5}, 1);
  (void)StringArraysOverlap({"a"}, {"b", "a"});
  (void)Limit({"a", "b"}, 1);
  (void)LimitStr("abcdef", 3);
  (void)MuiltiGroupBy<int, std::string>(
      std::vector<int>{1, 2},
      [](int value) { return std::vector<std::string>{std::to_string(value)}; });
  (void)MoveElement(std::vector<int>{1, 2, 3}, 0, 2);
  (void)ValuesAtIndices(std::vector<int>{1, 2, 3}, {0, 5});
  (void)Partition(std::vector<int>{1, 2}, [](int value) { return value % 2 == 0; });
  (void)Prepend(std::vector<int>{2}, 1);
  (void)Remove(std::vector<int>{1, 2}, 0);
  (void)Move(std::vector<int>{1, 2}, 0, 1);
  (void)Pop(std::vector<int>{1, 2});
  (void)Shift(std::vector<int>{1, 2});

  (void)FilterStrings("la", std::vector<std::string>{"lazygit", "git"}, true);
  (void)FilterStrings("la", std::vector<std::string>{"lazygit", "git"}, false);
  (void)Find("la", std::vector<std::string>{"lazygit"}, true);
  (void)FindSubstrings("la", std::vector<std::string>{"lazygit"});
  (void)CaseAwareContains("LazyGit", "Lazy");
  (void)ContainsUppercase("LazyGit");
  (void)CaseInsensitiveContains("LazyGit", "lazygit");
  const fuzzy::StringSource search_source(std::vector<std::string>{"lazygit"});
  (void)FindFrom("la", search_source, true);
  (void)FindSubstringsFrom("la", search_source);

  (void)FindLazygitRootDirectory();
}

}  // namespace
}  // namespace utils
}  // namespace lazygit
