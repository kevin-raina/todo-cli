#include "../include/token.hpp"
#include "../include/app.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

std::vector<std::string> tokenize(const std::string &line) {
  std::vector<std::string> tokens;
  std::istringstream iss(line);

  std::string word;
  while (iss >> word) {
    tokens.push_back(word);
  }
  return tokens;
}

bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void input(const std::string &line, App &app) {
  auto tokens = tokenize(line);

  if (tokens.size() < 2) {
    std::cerr << "Invalid command\n";
    return;
  }

  if (tokens[1] == "add") {
    app.add(tokens);
  } else if (tokens[1] == "list") {
    app.list();
  } else if (std::any_of(tokens.begin(), tokens.end(),
                         [](const std::string &t) {
                           return t.rfind("modify", 0) == 0;
                         })) {
    app.modify(tokens);
  } else if (std::any_of(tokens.begin(), tokens.end(),
                         [](const std::string &t) {
                           return t.rfind("delete", 0) == 0;
                         })) {
    app.remove(tokens);
  } else {
    std::cerr << "Unknown command\n";
  }
}
