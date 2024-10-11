#ifndef SAVE_HPP
#define SAVE_HPP
#include <string>
#include <vector>

#include "../helpers/user_input.hpp"
int save_game(const std::vector<UserInput>& moves);
std::vector<UserInput> load_game(const std::string& filename);
std::vector<std::string> get_savefiles();

#endif
