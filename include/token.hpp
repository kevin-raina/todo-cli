#pragma once
#include <string>
#include <vector>

class App;

std::vector<std::string> tokenize(const std::string &line);
bool is_number(const std::string &s);
void input(const std::string &line, App &app);
