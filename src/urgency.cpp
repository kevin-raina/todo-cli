#include "../include/urgency.hpp"

double Urgency::contribution(const std::optional<Priority> p) {
  if (!p)
    return 0.0;
  switch (*p) {
  case Priority::H:
    return 6.0;
    break;
  case Priority::M:
    return 3.9;
    break;
  case Priority::L:
    return 1.8;
  default:
    return 0.0;
  }
}
double Urgency::urgency_calc(const Task &task) {

  auto now = std::chrono::system_clock::now();

  double urgency{0.0};

  // Due
  if (task.due) {
    auto due_tp = *task.due;
    auto due_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(due_tp - now).count();
    int days_until_due = due_seconds / 86400;
    urgency += (due_coefficient / std::max(days_until_due, 1));
  }

  // Priority weight
  urgency += Urgency::contribution(task.priority);

  // Age
  auto age_seconds =
      std::chrono::duration_cast<std::chrono::seconds>(now - task.created_at)
          .count();
  int age_in_days = age_seconds / 86400;
  int age_score = std::min(age_in_days, age_max);
  urgency += age_score * age_coefficient;

  return urgency;
}
