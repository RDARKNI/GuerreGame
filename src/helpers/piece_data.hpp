#ifndef PIECE_DATA
#define PIECE_DATA

#include <array>

#include "action_data.hpp"

enum class PieceType : char {
  Nullpion,
  Chateau,
  Paysan,
  Guerrier,
  Seigneur,
  Moine,
  Foret,
  Mine,
  Chevalier,   // not implemented yet, not sure  how to balance
  Bombardier,  // not implemented yet, not sure  how to balance
  Tour,
  PIECECOUNT
};

struct PieceConstants {
  const char name[16];
  const unsigned char sign;
  const unsigned char max_hp;
  const unsigned char depl;
  const unsigned char puiss;
  const unsigned char atk_ran;
  const unsigned char prod;
  const unsigned char cout;
  std::array<ActionID, 8> actions;
};

inline constexpr PieceConstants
    pieces_data[static_cast<size_t>(PieceType::PIECECOUNT)] = {
        {.name = "",
         .sign = ' ',
         .max_hp = 0,
         .depl = 0,
         .puiss = 0,
         .atk_ran = 0,
         .prod = 0,
         .cout = 0,
         .actions = {}},
        {.name = "Chateau",
         .sign = 'C',
         .max_hp = 20,
         .depl = 0,
         .puiss = 2,
         .atk_ran = 1,
         .prod = 2,
         .cout = 15,
         .actions = {IDLE, BUILD_UNIT}},
        {.name = "Paysan",
         .sign = 'P',
         .max_hp = 6,
         .depl = 2,
         .puiss = 1,
         .atk_ran = 1,
         .prod = 4,
         .cout = 10,
         .actions = {IDLE, MOVE, FARM, CUT, TO_MONK}},
        {.name = "Guerrier",
         .sign = 'G',
         .max_hp = 10,
         .depl = 3,
         .puiss = 5,
         .atk_ran = 1,
         .prod = 0,
         .cout = 15,
         .actions = {IDLE, MOVE, ATTACK}},
        {.name = "Seigneur",
         .sign = 'S',
         .max_hp = 5,
         .depl = 1,
         .puiss = 3,
         .atk_ran = 2,
         .prod = 0,
         .cout = 10,
         .actions = {IDLE, MOVE, ATTACK, TO_CASTLE, BUILD_TOWER}},
        {.name = "Moine",
         .sign = 'M',
         .max_hp = 3,
         .depl = 1,
         .puiss = 0,
         .atk_ran = 2,
         .prod = 0,
         .cout = 0,
         .actions = {IDLE, MOVE, CONVERT}},
        {.name = "Foret",
         .sign = '@',
         .max_hp = 2,
         .depl = 0,
         .puiss = 0,
         .atk_ran = 0,
         .prod = 3,
         .cout = 0,
         .actions = {}},
        {.name = "Mine",
         .sign = '+',
         .max_hp = 5,
         .depl = 0,
         .puiss = 0,
         .atk_ran = 0,
         .prod = 5,
         .cout = 0,
         .actions = {}},
        {.name = "Chevalier",
         .sign = 'K',
         .max_hp = 15,
         .depl = 1,
         .puiss = 5,
         .atk_ran = 4,
         .prod = 0,
         .cout = 25,
         .actions = {IDLE, MOVE, ATTACK}},
        {.name = "Bombardier",
         .sign = 'B',
         .max_hp = 1,
         .depl = 2,
         .puiss = 20,
         .atk_ran = 1,
         .prod = 0,
         .cout = 10,
         .actions = {IDLE, MOVE, EXPLODE}},
        {.name = "Tour",
         .sign = 'T',
         .max_hp = 15,
         .depl = 0,
         .puiss = 3,
         .atk_ran = 2,
         .prod = 2,
         .cout = 15,
         .actions = {IDLE, ATTACK}}};
#endif