#pragma once
#include "tasks.hpp"
#include <chrono>
#include <string>

class Time {
public:
  static std::optional<std::chrono::system_clock::time_point>
  parse_due(const std::string &s);

  std::string age_string(const Task &task) const;
  std::string due_string(const Task &task) const;
};
