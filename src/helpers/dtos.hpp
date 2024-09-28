#ifndef DTOS_FILE__
#define DTOS_FILE__
#include "piece_data.hpp"

struct SquareData {
  unsigned int data;
  // max 16 types
  PieceType type() const { return static_cast<PieceType>(data & 0b1111); }
  unsigned int player() const {
    return (data >> 4) & 0b1111;
  }  // max 16 players
  unsigned int fatigue() const {
    return (data >> 8) & 0b1111;
  }  // max 16 fatigue
  unsigned int cannot_act() const { return (data >> 12) & 0b1; }
  unsigned int hp() const { return (data >> 13); }
};

#endif