#pragma once
#include "tasks.hpp"
#include <string>
#include <vector>

class App {
public:
  void add(std::vector<std::string> &tokens);
  void list();
  void modify(std::vector<std::string> &tokens);
  void remove(std::vector<std::string> &tokens);

  std::vector<Task *> select_by_id(int task_id);
  std::vector<Task *> select_by_pri(Priority pri);
  std::vector<Task *> select_by_tags(std::string tags);
  void mutate(std::vector<Task *> &selected,
              const std::vector<std::string> &tokens, size_t start);

  void load();
  void save();

private:
  std::vector<Task> tasks;
};
