#pragma once
#include "tasks.hpp"
#include <vector>

class Storage {
public:
  static void save(const std::vector<Task> &tasks);
  static std::vector<Task> load();
};
