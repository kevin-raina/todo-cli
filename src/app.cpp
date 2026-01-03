#include "../include/app.hpp"
#include "../include/storage.hpp"
#include "../include/time.hpp"
#include "../include/token.hpp"
#include "../include/urgency.hpp"
#include <algorithm>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <unordered_set>
void App::add(std::vector<std::string> &tokens) {

  load();

  Task task;

  task.id = tasks.empty() ? 1 : tasks.back().id + 1;

  // Creation time
  task.created_at = std::chrono::system_clock::now();

  for (size_t i = 2; i < tokens.size(); i++) {

    // Task priority
    if (tokens[i].rfind("pri:", 0) == 0) {
      char p = tokens[i].back();

      if (p == 'H' || p == 'M' || p == 'L') {
        task.priority = static_cast<Priority>(p);
      } else {
        std::cerr << "Invalid priority: " << p << "\n";
      }
    }

    // Due time
    else if (tokens[i].rfind("due:", 0) == 0) {
      task.due = Time::parse_due(tokens[i].substr(4));
    }

    // Project
    else if (tokens[i].rfind("project:", 0) == 0) {
      task.project = tokens[i].substr(8);
    }

    // Tags
    else if (tokens[i].rfind("+", 0) == 0) {
      task.tags.push_back(tokens[i].substr(1));
    }

    // Description
    else if (tokens[i].find(':') == std::string::npos && tokens[i][0] != '+') {
      if (!task.description.empty())
        task.description += ' ';
      task.description += tokens[i];
    }
  }

  tasks.push_back(task);
  save();
}

void App::list() {
  Time time;
  Urgency urgency;

  load();

  std::sort(tasks.begin(), tasks.end(),
            [&urgency](const Task &a, const Task &b) {
              return urgency.urgency_calc(a) > urgency.urgency_calc(b);
            });

  bool show_due = false;
  bool show_project = false;
  bool show_tags = false;

  for (const Task &task : tasks) {
    if (task.due)
      show_due = true;

    if (task.project)
      show_project = true;

    if (!task.tags.empty())
      show_tags = true;
  }

  auto header_style = fmt::fg(fmt::color::white) | fmt::emphasis::bold;

  fmt::print(header_style, "{:<4} {:<4} {:<3}", "ID", "Age", "P");

  if (show_due) {
    fmt::print(header_style, " {:<10}", "Due");
  }
  if (show_project)
    fmt::print(header_style, " {:<12}", "Project");

  if (show_tags)
    fmt::print(header_style, " {:<15}", "Tags");

  fmt::print(header_style, " {:<40}{:>6}\n", "Description", "Urg");

  bool striped = false;

  for (const Task &task : tasks) {
    double urg_val = urgency.urgency_calc(task);

    fmt::text_style style;
    if (urg_val >= 6.0) {
      style = fmt::fg(fmt::color::red);
    } else if (urg_val < 3.0) {
      style = fmt::fg(fmt::color::white) | fmt::emphasis::faint;
    } else {
      style = fmt::fg(fmt::color::white);
    }

    if (striped) {

      style |= fmt::bg(fmt::rgb(0x282a36));
    }

    char pri_char = task.priority ? static_cast<char>(*task.priority) : ' ';

    fmt::print(style, "{:<4} {:<4} {:<3}", task.id, time.age_string(task),
               pri_char);

    if (show_due) {
      fmt::print(style, " {:<10}", task.due ? time.due_string(task) : "");
    }
    if (show_project)
      fmt::print(style, " {:<12}", task.project ? *task.project : "");

    std::string tags_str;
    if (show_tags) {
      for (size_t i = 0; i < task.tags.size(); i++) {
        tags_str += task.tags[i] + ' ';
      }
      fmt::print(style, " {:<15}", tags_str);
    }
    fmt::print(style, " {:<40} {:>6.2f}", task.description, urg_val);

    fmt::print("\n");

    striped = !striped;
  }
}

void App::modify(std::vector<std::string> &tokens) {

  load();

  std::vector<Task *> selected;

  size_t modify_pos = 0;
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i] == "modify") {
      modify_pos = i;
      break;
    }
  }

  if (modify_pos == 0 || modify_pos + 1 >= tokens.size()) {
    std::cerr << "Nothing to modify\n";
    return;
  }

  for (size_t i = 1; i < modify_pos; i++) {

    if (is_number(tokens[i])) {
      auto part = select_by_id(std::stoi(tokens[i]));
      selected.insert(selected.end(), part.begin(), part.end());
    }

    if (tokens[i].rfind("pri:", 0) == 0) {
      char p = tokens[i].back();
      if (p == 'H' || p == 'M' || p == 'L') {
        auto part = select_by_pri(static_cast<Priority>(tokens[i].back()));
        selected.insert(selected.end(), part.begin(), part.end());
      } else {
        std::cerr << "Invalid priority: " << p << "\n";
      }
    }

    if (tokens[i].rfind("+", 0) == 0 || tokens[i].rfind("-", 0) == 0) {
      auto part = select_by_tags(tokens[i].substr(1));
      selected.insert(selected.end(), part.begin(), part.end());
    }
  }

  std::sort(selected.begin(), selected.end());
  selected.erase(std::unique(selected.begin(), selected.end()), selected.end());

  if (selected.empty()) {
    std::cerr << "No tasks matched selection\n";
    return;
  }

  mutate(selected, tokens, modify_pos + 1);
  save();
}

void App::remove(std::vector<std::string> &tokens) {

  load();

  std::vector<Task *> selected;

  size_t delete_pos = 0;
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i] == "delete") {
      delete_pos = i;
      break;
    }
  }

  if (delete_pos == 0) {
    std::cerr << "Nothing to delete\n";
    return;
  }

  for (size_t i = 1; i < delete_pos; i++) {

    if (is_number(tokens[i])) {
      auto part = select_by_id(std::stoi(tokens[i]));
      selected.insert(selected.end(), part.begin(), part.end());
    }

    if (tokens[i].rfind("pri:", 0) == 0) {
      char p = tokens[i].back();
      if (p == 'H' || p == 'M' || p == 'L') {
        auto part = select_by_pri(static_cast<Priority>(tokens[i].back()));
        selected.insert(selected.end(), part.begin(), part.end());
      } else {
        std::cerr << "Invalid priority: " << p << "\n";
      }
    }

    if (tokens[i].rfind("+", 0) == 0 || tokens[i].rfind("-", 0) == 0) {
      auto part = select_by_tags(tokens[i].substr(1));
      selected.insert(selected.end(), part.begin(), part.end());
    }
  }

  std::sort(selected.begin(), selected.end());
  selected.erase(std::unique(selected.begin(), selected.end()), selected.end());

  if (selected.empty()) {
    std::cerr << "No tasks matched selection\n";
    return;
  }
  std::unordered_set<Task *> doomed(selected.begin(), selected.end());
  tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
                             [&](const Task &t) {
                               return doomed.count(const_cast<Task *>(&t));
                             }),
              tasks.end());
  save();
}

void App::load() { tasks = Storage::load(); }

void App::save() { Storage::save(tasks); }

std::vector<Task *> App::select_by_id(int task_id) {
  std::vector<Task *> selected;
  for (Task &t : tasks) {
    if (t.id == task_id) {
      selected.push_back(&t);
    }
  }
  return selected;
}

std::vector<Task *> App::select_by_pri(Priority pri) {
  std::vector<Task *> selected;
  for (Task &t : tasks) {
    if (t.priority && *t.priority == pri) {
      selected.push_back(&t);
    }
  }
  return selected;
}

std::vector<Task *> App::select_by_tags(std::string tags) {
  std::vector<Task *> selected;
  for (Task &t : tasks) {
    for (const std::string &tag : t.tags) {
      if (tag == tags) {
        selected.push_back(&t);
        break;
      }
    }
  }
  return selected;
}

void App::mutate(std::vector<Task *> &selected,
                 const std::vector<std::string> &tokens, size_t start) {
  for (size_t i = start; i < tokens.size(); i++) {
    for (size_t j = 0; j < selected.size(); j++) {

      Task &selected_task = *selected[j];

      size_t last_k{i};

      // Task priority
      if (tokens[i].rfind("pri:", 0) == 0) {
        char p = tokens[i].back();

        if (p == 'H' || p == 'M' || p == 'L') {
          selected_task.priority = static_cast<Priority>(tokens[i].back());

        } else {
          std::cerr << "Invalid priority: " << p << "\n";
        }
      }

      // Due time
      else if (tokens[i].rfind("due:", 0) == 0) {
        selected_task.due = Time::parse_due(tokens[i].substr(4));
      }

      // Project
      else if (tokens[i].rfind("project:", 0) == 0) {
        selected_task.project = tokens[i].substr(8);
      }

      // Add tags
      else if (tokens[i].rfind("+", 0) == 0) {
        auto &tags = selected_task.tags;
        if (std ::find(tags.begin(), tags.end(), tokens[i].substr(1)) ==
            tags.end())
          selected_task.tags.push_back(tokens[i].substr(1));

      }

      // Remove tags
      else if (tokens[i].rfind("-", 0) == 0) {
        auto &tags = selected_task.tags;
        if (std::find(tags.begin(), tags.end(), tokens[i].substr(1)) ==
            tags.end())
          continue;
        tags.erase(std::remove(tags.begin(), tags.end(), tokens[i].substr(1)),
                   tags.end());
      }

      // ---- Description replace ----
      else if (tokens[i].rfind("description:", 0) == 0) {
        std::string desc = tokens[i].substr(12);

        for (size_t k = i + 1; k < tokens.size(); k++) {
          if (tokens[k].find(':') == std::string::npos && tokens[k][0] != '+' &&
              tokens[k][0] != '-') {
            desc += " " + tokens[k];
            last_k = k;
          } else {
            break;
          }
        }
        i = last_k;
        selected_task.description = desc;
      }

      // ---- Description append ----
      else if (tokens[i].rfind("description+=", 0) == 0) {
        std::string extra;

        for (size_t k = i + 1; k < tokens.size(); k++) {
          if (tokens[k].find(':') == std::string::npos && tokens[k][0] != '+' &&
              tokens[k][0] != '-') {
            if (!extra.empty())
              extra += ' ';
            extra += tokens[k];

            last_k = k;

          } else {
            break;
          }
        }
        i = last_k;
        if (!selected_task.description.empty())
          selected_task.description += " ";

        selected_task.description += extra;
      }

      // ---- Description prepend ----
      else if (tokens[i].rfind("description^=", 0) == 0) {
        std::string prefix = tokens[i].substr(13);

        for (size_t k = i + 1; k < tokens.size(); k++) {
          if (tokens[k].find(':') == std::string::npos && tokens[k][0] != '+' &&
              tokens[k][0] != '-') {
            prefix += " " + tokens[k];
            last_k = k;
          } else {
            break;
          }
        }
        i = last_k;
        prefix += " ";
        selected_task.description.insert(0, prefix);
      }

      // Default: modify description
      else if (tokens[i].find(':') == std::string::npos &&
               tokens[i].find("+=") == std::string::npos &&
               tokens[i].find("-=") == std::string::npos &&
               tokens[i][0] != '+' && tokens[i][0] != '-') {

        if (!selected_task.description.empty())
          selected_task.description += ' ';
        selected_task.description += tokens[i];
      }

      else {
        continue;
      }
    }
  }
}
