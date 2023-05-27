#pragma once

#include <string>

bool parse_input(const std::string &line, char &cmd, std::string &err_msg, int &vertex, int &start, int &dest, std::vector<std::pair<int, int>> &edges);
