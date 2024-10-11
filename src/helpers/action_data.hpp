#ifndef ACTION_ENUMS
#define ACTION_ENUMS
#include <array>
/*
All < 0 are special actions
*/
enum class ActionID : unsigned char {
  action,
  create_unit,
  end_turn,
  move,
  attack,
  build_unit,
  farm,
  to_castle,
  idle,
  convert,
  to_monk,
  cut,
  explode,
  build_tower,
  research_upgrade,
  build_bomb,
  COUNT,
};

template <typename T>
constexpr inline auto index(T id)
  requires(std::is_enum_v<T>)
{
  return static_cast<std::underlying_type_t<T>>(id);
}
enum class OptionID : unsigned char {
  option,
  create_chateau,
  create_paysan,
  create_seigneur,
  create_guerrier,
  create_archer,
  create_arbre,
  create_mine,
  res_fortress,
  COUNT,
};

struct ActionConstants {
  const char name[16];
  std::array<OptionID, 4> actions;
  bool takes_options;
  bool takes_targets;
};

inline constexpr std::array<const char*, index(ActionID::COUNT)> action_names{
    "",        "Create",      "End Turn",      "Move",
    "Attack",  "Build Unit",  "Harvest",       "Build Castle",
    "Idle",    "Convert",     "Become a Monk", "Cut",
    "Explode", "Build Tower", "Research",      "Build Bomb"};

inline constexpr std::array<const char*, index(OptionID::COUNT)> option_names{
    "",       "Chateau", "Paysan", "Seigneur", "Guerrier",
    "Archer", "Foret",   "Mine",   "Fortress"};

inline constexpr OptionID action_options[index(ActionID::COUNT)][4]{
    {},
    {OptionID::create_chateau, OptionID::create_paysan},
    {},
    {},
    {},
    {OptionID::create_paysan, OptionID::create_seigneur,
     OptionID::create_guerrier, OptionID::create_archer},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {OptionID::res_fortress},
    {}};
inline constexpr bool action_takes_squares[index(ActionID::COUNT)]{
    false, true, false, true, true,  true, false, false,
    false, true, false, true, false, true, false, true,
};
inline constexpr bool act_takes_options(ActionID act) {
  return action_options[index(act)][0] != OptionID::option;
}
inline constexpr bool act_takes_squares(ActionID act) {
  return action_takes_squares[index(act)];
}

template <typename T>
struct Data {
  T type;
  unsigned char mods;
};
using ActionData = Data<ActionID>;

using OptionData = Data<OptionID>;

#endif