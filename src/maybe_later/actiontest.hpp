// #ifndef ACTION_ENUMS
// #define ACTION_ENUMS
// #include <array>
//
// enum class ActionID : unsigned char {
//   senitel,
//   end_turn,
//   idle,
//   farm,
//   place_unit,
//   build_unit,
//   to_unit,
//   move,
//   attack,
//   cut,
//   convert,
//   explode,
//   COUNT,
// };
//
// template <typename T>
// constexpr inline auto index(T id)
//   requires(std::is_enum_v<T>)
//{
//   return static_cast<std::underlying_type_t<T>>(id);
// }
// enum class OptionID : unsigned char {
//   senitel,
//   create_chateau,
//   create_paysan,
//   create_seigneur,
//   create_guerrier,
//   create_archer,
//   create_arbre,
//   create_mine,
//   res_fortress,
//   COUNT,
// };
//
// struct ActionConstants {
//   const char name[16];
//   std::array<OptionID, 4> actions;
//   bool takes_options;
//   bool takes_targets;
// };
// inline constexpr std::array<const char*, index(ActionID::COUNT)>
// action_names{
//     "",          "End Turn", "Idle",   "Farm", "Place Unit", "Build Unit",
//     "Turn Into", "Move",     "Attack", "Cut",  "Convert",    "Explode"};
//
// inline constexpr std::array<const char*, index(OptionID::COUNT)>
// option_names{
//     "",       "Chateau", "Paysan", "Seigneur", "Guerrier",
//     "Archer", "Foret",   "Mine",   "Fortress"};
//
// inline constexpr bool action_takes_squares[index(ActionID::COUNT)]{
//     false, false, false, false, true, true,
//     false, true,  true,  true,  true, false};
//
// inline constexpr bool act_takes_squares(ActionID act) {
//   return action_takes_squares[index(act)];
// }
//
// template <typename T>
// struct Data {
//   T type;
//   unsigned char mods;
// };
// using ActionData = Data<ActionID>;
//
// using OptionData = Data<OptionID>;
//
// #endif