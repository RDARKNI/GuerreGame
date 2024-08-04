#ifndef PIECE_DATA
#define PIECE_DATA
#include <array>

#include "action_data.hpp"

enum Piece_ID : char {
  CHATEAU = 1,
  PAYSAN = 2,
  GUERRIER = 3,
  SEIGNEUR = 4,
  MOINE = 5,
  FORET = 6,
  MINE = 7,
  CHEVALIER = 8,   // not implemented yet, not sure  how to balance
  BOMBARDIER = 9,  // not implemented yet, not sure  how to balance
  TOUR = 10,
  PIECECOUNT
};

struct PieceConstants {
  const char* name;
  const char sign;
  const int max_hp;
  const int depl;
  const int puiss;
  const int atk_ran;
  const int prod;
  const int cout;
  std::array<ActionID, 5> actions;
};

extern const PieceConstants piece_constants[PIECECOUNT];

#endif