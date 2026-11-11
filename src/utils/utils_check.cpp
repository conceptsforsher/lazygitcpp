// Origin: (none) - translation harness, this file is not an upstream file.
//
// Headers are not translation units: without this file the g++/clang++ build in
// .github/workflows/build.yml would never compile the translated headers at
// all, and templates would never even be instantiated. It includes every
// translated header and exercises each one, so CI really checks them.

#include "utils/errors.hpp"
#include "utils/history_buffer.hpp"
#include "utils/io.hpp"
#include "utils/once_writer.hpp"
#include "utils/regexp.hpp"
#include "utils/stack.hpp"
#include "utils/string_pool.hpp"
#include "utils/template.hpp"
#include "utils/thread_safe_map.hpp"

#include <map>
#include <optional>
#include <sstream>
#include <string>

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
}

}  // namespace
}  // namespace utils
}  // namespace lazygit
