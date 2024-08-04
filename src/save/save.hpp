#ifndef SAVE_HPP
#define SAVE_HPP
#include <fstream>
#include <vector>

#include "../helpers/user_input.hpp"

static int save_game(const std::vector<UserInput>& moves) {
  std::ofstream savefile("TESTSAVE.txt");
  if (!savefile.is_open()) {
    return 1;
  }
  for (const UserInput& move : moves) {
    savefile.write(reinterpret_cast<const char*>(&move), sizeof(move));
  }
  savefile.close();
  return 0;
}

static std::vector<UserInput> load_game(std::string filename) {
  std::ifstream savefile(filename);
  if (!savefile.is_open()) {
    return {};
  }
  std::vector<UserInput> hist;
  UserInput input;
  while (savefile.read(reinterpret_cast<char*>(&input), sizeof(input))) {
    hist.push_back(input);
  }
  return hist;
}

#endif
