#ifndef ACTION_ENUMS
#define ACTION_ENUMS
#include <array>
#include <type_traits>

// #include "coords.hpp"
/*
All < 0 are special actions
*/
enum class ActionID : unsigned char {
  Sentinel,
  CreateUnit,
  EndTurn,
  Move,
  Attack,
  BuildUnit,
  Farm,
  ToCastle,
  Idle,
  Convert,
  ToMonk,
  Cut,
  Explode,
  BuildTower,
  Research,
  BuildBomb,
  BuildPaysan,
  BuildChateau,
  BuildArcher,
  BuildArbre,
  BuildMine,
  BuildGuerrier,
  COUNT,
};
template <class T>
concept scoped_enum = std::is_scoped_enum_v<T>;
template <class T>
concept has_sentinel = requires(T t) { T::Sentinel; } || std::is_pointer_v<T>;

constexpr inline auto index(scoped_enum auto id) {
  return static_cast<std::underlying_type_t<decltype(id)>>(id);
}

template <has_sentinel T>
constexpr inline T sentinelof() {
  if constexpr (std::is_pointer_v<T>) {
    return nullptr;
  } else {
    return T::Sentinel;
  }

  // return T{0};
}

// constexpr inline scoped_enum auto sentinelof() { return auto{0}; }
//  template <typename T>
//  constexpr inline scoped_enum auto sentinelof() {
//    return T{0};
//  }
enum class OptionID : unsigned char {
  Sentinel,
  CreateChateau,
  CreatePaysan,
  CreateSeigneur,
  CreateGuerrier,
  CreateArcher,
  CreateArbre,
  CreateMine,
  ResearchFortress,
  COUNT,
};

inline constexpr std::array<const char*, index(ActionID::COUNT)> action_names{
    "",        "Create",       "End Turn", "Move",       "Attack",        "Build Unit",
    "Harvest", "Build Castle", "Idle",     "Convert",    "Become a Monk", "Cut",
    "Explode", "Build Tower",  "Research", "Build Bomb", "Build Chateau", "Build Paysan"};

inline constexpr std::array<const char*, index(OptionID::COUNT)> option_names{
    "", "Chateau", "Paysan", "Seigneur", "Guerrier", "Archer", "Foret", "Mine", "Fortress"};

inline constexpr std::array<std::array<OptionID, 4>, index(ActionID::COUNT)> action_options{
    {{},
     {OptionID::CreateArbre, OptionID::CreateMine},
     {},
     {},
     {},
     {OptionID::CreatePaysan, OptionID::CreateSeigneur, OptionID::CreateGuerrier, OptionID::CreateArcher},
     {},
     {},
     {},
     {},
     {},
     {},
     {},
     {},
     {OptionID::ResearchFortress},
     {}}};

inline constexpr std::array<bool, index(ActionID::COUNT)> action_takes_squares{
    false, true,  false, true,  true, true,  false, false, false,
    true,  false, true,  false, true, false, true,  false, false};

inline constexpr bool act_takes_options(ActionID act) {
  return action_options[index(act)][0] != OptionID::Sentinel;
}
inline constexpr bool act_takes_dst(ActionID act) { return action_takes_squares[index(act)]; }

template <typename T>
struct Data {
  T type;
  unsigned char mods;
};
using ActionData = Data<ActionID>;

using OptionData = Data<OptionID>;

#endif