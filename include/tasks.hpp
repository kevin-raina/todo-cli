#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

enum class Priority {
  H = 'H',
  M = 'M',
  L = 'L',
  None = ' ',
};

struct Task {

  int id;
  std::chrono::time_point<std::chrono::system_clock> created_at;
  std::optional<Priority> priority;
  std::optional<std::chrono::time_point<std::chrono::system_clock>> due;
  std::optional<std::string> project;
  std::vector<std::string> tags;
  std::string description;
};
