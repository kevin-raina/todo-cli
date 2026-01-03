#include "../include/storage.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using sys_clock = std::chrono::system_clock;

static long long to_unix_seconds(sys_clock::time_point tp) {
  return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch())
      .count();
}

static sys_clock::time_point from_unix_seconds(long long s) {
  return sys_clock::time_point{std::chrono::seconds{s}};
}

void Storage::save(const std::vector<Task> &tasks) {
  std::filesystem::create_directories("data");
  std::ofstream outf("data/tasks.jsonl");
  if (!outf) {
    std::cerr << "Could not open file for writing\n";
    return;
  }

  for (const Task &task : tasks) {
    json j;
    j["id"] = task.id;
    j["created_at"] = to_unix_seconds(task.created_at);
    if (task.priority)
      j["priority"] = std::string(1, static_cast<char>(*task.priority));
    if (task.due)
      j["due"] = to_unix_seconds(*task.due);
    if (task.project)
      j["project"] = *task.project;
    j["tags"] = task.tags;
    j["description"] = task.description;

    outf << j.dump() << '\n';
  }
}

std::vector<Task> Storage::load() {
  std::filesystem::create_directories("data");
  std::vector<Task> tasks;
  std::ifstream inf("data/tasks.jsonl");
  if (!inf) {
    return tasks; // first run, file doesn't exist
  }

  std::string line;
  while (std::getline(inf, line)) {
    try {
      json j = json::parse(line);

      Task task;
      task.id = j.at("id").get<int>();
      task.created_at = from_unix_seconds(j.at("created_at").get<long long>());
      if (j.contains("priority") && !j["priority"].is_null()) {
        auto p = j.at("priority").get<std::string>();
        task.priority = static_cast<Priority>(p[0]);
      }
      if (j.contains("due") && !j["due"].is_null())
        task.due = from_unix_seconds(j.at("due").get<long long>());
      if (j.contains("project") && !j["project"].is_null())
        task.project = j.at("project").get<std::string>();
      task.tags = j.at("tags").get<std::vector<std::string>>();
      task.description = j.at("description").get<std::string>();

      tasks.push_back(task);
    } catch (...) {
      std::cerr << "Skipping bad line\n";
    }
  }
  return tasks;
}
