#include "save.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>

inline const std::filesystem::path dir_path{"guerre_savefiles/"};

int save_game(const std::vector<UserInput>& moves) {
  std::filesystem::create_directory(dir_path);
  std::filesystem::path filepath =
      dir_path /
      (std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) + ".sav");
  std::ofstream savefile{filepath, std::ios::binary | std::ios::out};

  for (const UserInput& move : moves) {
    if (!savefile.write(reinterpret_cast<const char*>(&move), sizeof(move))) {
      return 2;
    }
  }
  return 0;
}

std::vector<UserInput> load_game(const std::string& filename) {
  std::filesystem::path path{dir_path / filename};
  std::string filepath{path.string()};
  std::ifstream savefile{filepath, std::ios::binary | std::ios::in};
  std::vector<UserInput> hist;
  UserInput input;

  while (savefile.read(reinterpret_cast<char*>(&input), sizeof(input))) {
    hist.push_back(input);
  }
  if (savefile.gcount() % sizeof(input)) {
    return {};
  }
  return hist;
}
std::vector<std::string> get_savefiles() {
  if (!std::filesystem::is_directory(dir_path)) {
    return {};
  }
  std::vector<std::string> filenames;
  for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
    if (entry.is_regular_file()) {
      filenames.push_back(entry.path().filename().string());
    }
  }
  std::ranges::sort(filenames, std::greater<std::string>());
  return filenames;
}
