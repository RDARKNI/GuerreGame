#ifndef DTOS
#define DTOS
#include "action_data.hpp"
#include "coords.hpp"

struct UserInput {
  Coord src{};
  ActionID act{};
  OptionID opt{};
  Coord dst{};
  // friend std::ostream& operator<<(std::ostream& os, const UserInput& inp) {
  //   return os << "{" << inp.src << "," << inp.act << "," << inp.opt << ","
  //             << inp.dst << "}";
};
struct SettingsInput {
  Coord src{};
  char act{};
  char opt{};
  Coord dst{};
};
struct DataType {
  union {
    UserInput input;
    SettingsInput settings;
  };
};
static_assert(sizeof(UserInput) == sizeof(SettingsInput));
#endif