#pragma once
#include "tasks.hpp"

class Urgency {
private:
  static constexpr double age_coefficient{2.0};
  static constexpr int age_max{365};
  static constexpr double due_coefficient{12.0};

  static double contribution(const std::optional<Priority> p);

public:
  static double urgency_calc(const Task &task);
};
