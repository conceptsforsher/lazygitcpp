// Origin: pkg/utils/date.go

#pragma once

#include <cstdint>
#include <ctime>
#include <string>

namespace lazygit {
namespace utils {

constexpr std::int64_t kSecondsInSecond = 1;
constexpr std::int64_t kSecondsInMinute = 60;
constexpr std::int64_t kSecondsInHour = 3600;
constexpr std::int64_t kSecondsInDay = 86400;
constexpr std::int64_t kSecondsInWeek = 604800;
constexpr std::int64_t kSecondsInYear = 31536000;
constexpr std::int64_t kSecondsInMonth = kSecondsInYear / 12;

// UnixToTimeAgo: upstream lines 7-10.
std::string UnixToTimeAgo(std::int64_t timestamp);

// UnixToDateSmart: upstream lines 65-75. Upstream comment: "formats the date in
// a smart way, if the date is today, it will show the time, otherwise it will
// show the date".
//
// The formats are Go reference layouts ("02 Jan 06"), not strftime patterns, so
// they are converted first; see GoLayoutToStrftime.
std::string UnixToDateSmart(std::time_t now, std::int64_t timestamp,
                            const std::string& long_time_format,
                            const std::string& short_time_format);

// Translates a Go reference layout into a strftime pattern. Supported: 2006,
// 06, January, Jan, Monday, Mon, 01, 1, 02, 2, 15, 03, 3, 04, 4, 05, 5, PM.
// TODO(translation): add timezone tokens (Z0700, -0700, MST) when a user
// config uses them.
std::string GoLayoutToStrftime(const std::string& layout);

// Formats a timestamp with a Go reference layout in local time.
std::string FormatGoLayout(std::time_t timestamp, const std::string& layout);

}  // namespace utils
}  // namespace lazygit
