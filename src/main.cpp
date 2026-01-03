#include "../include/app.hpp"
#include "../include/token.hpp"
#include <iostream>
#include <string>

int main() {
  App app;
  std::string line;
  std::getline(std::cin, line);
  input(line, app);
  return 0;
}
