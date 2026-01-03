#include "../include/time.hpp"
#include <sstream>

std::string Time::age_string(const Task &task) const {
  auto now = std::chrono::system_clock::now();
  auto diff = now - task.created_at;

  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count();

  const long long minute = 60;
  const long long hour = 60 * minute;
  const long long day = 24 * hour;
  const long long week = 7 * day;

  if (seconds >= week)
    return std::to_string(seconds / week) + "w";
  if (seconds >= day)
    return std::to_string(seconds / day) + "d";
  if (seconds >= hour)
    return std::to_string(seconds / hour) + "h";
  if (seconds >= minute)
    return std::to_string(seconds / minute) + "m";

  return std::to_string(seconds) + "s";
}

std::optional<std::chrono::system_clock::time_point>
Time::parse_due(const std::string &s) {
  using clock = std::chrono::system_clock;
  auto now = clock::now();

  // ---------- Case 1: relative time ----------
  if (!s.empty() && std::isdigit(s[0])) {
    size_t i = 0;
    while (i < s.size() && std::isdigit(s[i]))
      ++i;

    // EXACTLY "<digits><unit>"
    if (i + 1 == s.size()) {
      long long value = std::stoll(s.substr(0, i));
      char unit = s[i];

      using namespace std::chrono;
      switch (unit) {
      case 'm':
        return now + minutes(value);
      case 'h':
        return now + hours(value);
      case 'd':
        return now + hours(24 * value);
      case 'w':
        return now + hours(24 * 7 * value);
      }
    }
  }

  // ---------- Case 2: absolute date ----------
  int y, m, d;
  char dash1, dash2;
  std::istringstream iss(s);

  if (!(iss >> y >> dash1 >> m >> dash2 >> d))
    return std::nullopt;

  if (dash1 != '-' || dash2 != '-')
    return std::nullopt;

  std::tm tm{};
  tm.tm_year = y - 1900;
  tm.tm_mon = m - 1;
  tm.tm_mday = d;

  std::time_t tt = std::mktime(&tm);
  if (tt == -1)
    return std::nullopt;

  return clock::from_time_t(tt);
}

std::string Time::due_string(const Task &task) const {
  if (!task.due) {
    return "";
  }
  auto now = std::chrono::system_clock::now();
  auto diff = *task.due - now;

  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count();

  const long long minute = 60;
  const long long hour = 60 * minute;
  const long long day = 24 * hour;
  const long long week = 7 * day;

  auto abs_seconds = std::llabs(seconds);

  if (abs_seconds >= week)
    return std::to_string(seconds / week) + "w";
  if (abs_seconds >= day)
    return std::to_string(seconds / day) + "d";
  if (abs_seconds >= hour)
    return std::to_string(seconds / hour) + "h";
  if (abs_seconds >= minute)
    return std::to_string(seconds / minute) + "m";

  return std::to_string(seconds) + "s";
}
