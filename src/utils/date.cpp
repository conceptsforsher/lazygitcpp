// Origin: pkg/utils/date.go

#include "utils/date.hpp"

#include <array>
#include <cctype>
#include <ctime>
#include <vector>

namespace lazygit {
namespace utils {

namespace {

// upstream lines 12-20: the period table, unexported.
struct Period {
  const char* label;
  std::int64_t seconds_in_period;
};

// upstream lines 22-30.
const std::array<Period, 7>& Periods() {
  static const std::array<Period, 7> periods = {{
      {"s", kSecondsInSecond},
      {"m", kSecondsInMinute},
      {"h", kSecondsInHour},
      {"d", kSecondsInDay},
      {"w", kSecondsInWeek},
      {"M", kSecondsInMonth},
      {"y", kSecondsInYear},
  }};
  return periods;
}

// formatSecondsAgo: upstream lines 32-48, unexported.
std::string FormatSecondsAgo(std::int64_t seconds_ago) {
  const auto& periods = Periods();

  for (std::size_t i = 0; i < periods.size(); ++i) {
    if (i == 0) {
      continue;
    }

    if (seconds_ago < periods[i].seconds_in_period) {
      return std::to_string(seconds_ago / periods[i - 1].seconds_in_period) +
             periods[i - 1].label;
    }
  }

  const Period& last = periods[periods.size() - 1];
  return std::to_string(seconds_ago / last.seconds_in_period) + last.label;
}

bool StartsWith(const std::string& s, std::size_t pos, const char* token) {
  for (std::size_t i = 0; token[i] != '\0'; ++i) {
    if (pos + i >= s.size() || s[pos + i] != token[i]) {
      return false;
    }
  }
  return true;
}

}  // namespace

std::string GoLayoutToStrftime(const std::string& layout) {
  struct Token {
    const char* go;
    const char* strftime;
  };
  static const std::array<Token, 15> kTokens = {{
      {"2006", "%Y"},
      {"January", "%B"},
      {"Jan", "%b"},
      {"Monday", "%A"},
      {"Mon", "%a"},
      {"06", "%y"},
      {"01", "%m"},
      {"1", "%-m"},
      {"02", "%d"},
      {"2", "%-d"},
      {"15", "%H"},
      {"03", "%I"},
      {"3", "%-I"},
      {"04", "%M"},
      {"05", "%S"},
  }};

  std::string pattern;
  for (std::size_t i = 0; i < layout.size();) {
    bool matched = false;
    for (const Token& token : kTokens) {
      if (StartsWith(layout, i, token.go)) {
        std::size_t len = 0;
        while (token.go[len] != '\0') {
          ++len;
        }
        pattern.append(token.strftime);
        i += len;
        matched = true;
        break;
      }
    }
    if (matched) {
      continue;
    }

    if (StartsWith(layout, i, "PM")) {
      pattern.append("%p");
      i += 2;
      continue;
    }

    // Anything else is a literal; '%' has to survive into strftime itself.
    if (layout[i] == '%') {
      pattern.append("%%");
    } else {
      pattern.push_back(layout[i]);
    }
    ++i;
  }

  return pattern;
}

std::string FormatGoLayout(std::time_t timestamp, const std::string& layout) {
  std::tm local{};
  localtime_r(&timestamp, &local);

  const std::string pattern = GoLayoutToStrftime(layout);
  std::vector<char> buffer(256);
  const std::size_t written =
      std::strftime(buffer.data(), buffer.size(), pattern.c_str(), &local);
  if (written == 0) {
    return std::string();
  }
  return std::string(buffer.data(), written);
}

std::string UnixToTimeAgo(std::int64_t timestamp) {
  const std::time_t now = std::time(nullptr);  // upstream: time.Now().Unix()
  return FormatSecondsAgo(static_cast<std::int64_t>(now) - timestamp);
}

std::string UnixToDateSmart(std::time_t now, std::int64_t timestamp,
                            const std::string& long_time_format,
                            const std::string& short_time_format) {
  const std::time_t as_time = static_cast<std::time_t>(timestamp);

  // upstream: time.Unix(timestamp, 0).In(now.Location())
  std::tm date{};
  localtime_r(&as_time, &date);
  std::tm now_local{};
  localtime_r(&now, &now_local);

  if (date.tm_mday == now_local.tm_mday && date.tm_mon == now_local.tm_mon &&
      date.tm_year == now_local.tm_year) {
    return FormatGoLayout(as_time, short_time_format);
  }

  return FormatGoLayout(as_time, long_time_format);
}

}  // namespace utils
}  // namespace lazygit
