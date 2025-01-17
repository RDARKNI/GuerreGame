// #ifndef PIECE_DATA
// #define PIECE_DATA
//
// #include <array>
//
// #include "actiontest.hpp"
//
// enum class PieceType : unsigned char {
//   Nullpion,
//   Chateau,
//   Paysan,
//   Guerrier,
//   Seigneur,
//   Moine,
//   Arbre,
//   Mine,
//   Chevalier,   // not implemented yet, not sure  how to balance
//   Bombardier,  // not implemented yet, not sure  how to balance
//   Tour,
//   Archer,
//   PIECECOUNT
// };
///*
// Store Actions and their options?
//*/
// struct ActionOptions {
//   ActionID action;
//   std::array<OptionID, 7> options;
// };
// inline constexpr size_t max_actions{8};
// struct PieceConstants {
//   const char name[16];
//   const std::array<ActionOptions, 8> actions;
//   const unsigned char sign;
//   const unsigned char max_hp;
//   const unsigned char depl;
//   const unsigned char puiss;
//   const unsigned char atk_ran;
//   const unsigned char prod;
//   const unsigned char cout;
//   const unsigned char RESERVED;
// };
//
// inline constexpr PieceConstants pieces_data[int(PieceType::PIECECOUNT)] = {
//     {.name = "",
//      .actions = {ActionID::build_chateau, ActionID::build_paysan},
//      .sign = ' ',
//      .max_hp = 0,
//      .depl = 0,
//      .puiss = 0,
//      .atk_ran = 0,
//      .prod = 0,
//      .cout = 0,
//      .RESERVED = 0},
//     {.name = "Chateau",
//      .actions = {ActionID::idle, ActionID::build_unit,
//                  ActionID::research_upgrade, ActionID::attack},
//      .sign = 'C',
//      .max_hp = 20,
//      .depl = 0,
//      .puiss = 2,
//      .atk_ran = 1,
//      .prod = 2,
//      .cout = 15,
//      .RESERVED = 0},
//     {.name = "Paysan",
//      .actions = {ActionID::idle, ActionID::move, ActionID::farm,
//      ActionID::cut,
//                  ActionID::to_monk},
//      .sign = 'P',
//      .max_hp = 6,
//      .depl = 2,
//      .puiss = 1,
//      .atk_ran = 1,
//      .prod = 4,
//      .cout = 10,
//      .RESERVED = 0},
//     {.name = "Guerrier",
//      .actions = {ActionID::idle, ActionID::move, ActionID::attack},
//      .sign = 'G',
//      .max_hp = 10,
//      .depl = 3,
//      .puiss = 5,
//      .atk_ran = 1,
//      .prod = 0,
//      .cout = 15,
//      .RESERVED = 0},
//     {.name = "Seigneur",
//      .actions = {ActionID::idle, ActionID::move, ActionID::attack,
//                  ActionID::to_castle, ActionID::build_tower},
//      .sign = 'S',
//      .max_hp = 5,
//      .depl = 1,
//      .puiss = 3,
//      .atk_ran = 2,
//      .prod = 0,
//      .cout = 10,
//      .RESERVED = 0},
//     {.name = "Moine",
//      .actions = {ActionID::idle, ActionID::move, ActionID::convert},
//      .sign = 'M',
//      .max_hp = 3,
//      .depl = 1,
//      .puiss = 0,
//      .atk_ran = 2,
//      .prod = 0,
//      .cout = 0,
//      .RESERVED = 0},
//     {.name = "Arbre",
//      .actions = {},
//      .sign = '@',
//      .max_hp = 2,
//      .depl = 0,
//      .puiss = 0,
//      .atk_ran = 0,
//      .prod = 3,
//      .cout = 0,
//      .RESERVED = 0},
//     {.name = "Mine",
//      .actions = {},
//      .sign = '+',
//      .max_hp = 5,
//      .depl = 0,
//      .puiss = 0,
//      .atk_ran = 0,
//      .prod = 5,
//      .cout = 0,
//      .RESERVED = 0},
//     {.name = "Chevalier",
//      .actions = {ActionID::idle, ActionID::move, ActionID::attack},
//      .sign = 'K',
//      .max_hp = 15,
//      .depl = 1,
//      .puiss = 5,
//      .atk_ran = 4,
//      .prod = 0,
//      .cout = 25,
//      .RESERVED = 0},
//     {.name = "Bombardier",
//      .actions = {ActionID::idle, ActionID::move, ActionID::explode},
//      .sign = 'B',
//      .max_hp = 1,
//      .depl = 2,
//      .puiss = 20,
//      .atk_ran = 1,
//      .prod = 0,
//      .cout = 10,
//      .RESERVED = 0},
//     {.name = "Tour",
//      .actions = {ActionID::idle, ActionID::attack, ActionID::build_bomb},
//      .sign = 'T',
//      .max_hp = 15,
//      .depl = 0,
//      .puiss = 3,
//      .atk_ran = 2,
//      .prod = 0,
//      .cout = 15,
//      .RESERVED = 0},
//     {.name = "Archer",
//      .actions = {ActionID::idle, ActionID::move, ActionID::attack},
//      .sign = 'A',
//      .max_hp = 6,
//      .depl = 1,
//      .puiss = 4,
//      .atk_ran = 3,
//      .prod = 0,
//      .cout = 16,
//      .RESERVED = 0}};
//
// #endif