#include "pion.hpp"

#include <cassert>

#include "board.hpp"
#include "player.hpp"

constexpr const PieceConstants piece_constants[PIECECOUNT] = {
    //  name      s    hp  de pu ra pr co
    {"Pion", ' ', 0, 0, 0, 0, 0, 0, {}},
    {"Chateau", 'C', 20, 0, 2, 1, 2, 15, {IDLE, BUILD_UNIT}},
    {"Paysan", 'P', 6, 2, 1, 1, 4, 10, {IDLE, MOVE, FARM, CUT, TO_MONK}},
    {"Guerrier", 'G', 10, 3, 5, 1, 0, 15, {IDLE, MOVE, ATTACK}},
    {"Seigneur",
     'S',
     5,
     1,
     3,
     2,
     0,
     10,
     {IDLE, MOVE, ATTACK, TO_CASTLE, BUILD_TOWER}},
    {"Moine", 'M', 3, 1, 0, 2, 0, 0, {IDLE, MOVE, CONVERT}},
    {"Foret", '@', 2, 0, 0, 0, 3, 0, {}},
    {"Mine", '+', 5, 0, 0, 0, 5, 0, {}},
    {"Chevalier", 'K', 15, 1, 5, 4, 0, 25, {IDLE, MOVE, ATTACK}},
    {"Bombardier", 'B', 1, 2, 20, 1, 0, 10, {IDLE, MOVE, EXPLODE}},
    {"Tour", 'T', 15, 0, 3, 2, 2, 15, {IDLE, ATTACK}}};

Pion::Pion(Player& p, Piece_ID type, Coord coords)
    : player_{&p},
      pv_{piece_constants[type].max_hp},
      type{type},
      coords_{coords} {}

void Pion::die() { player_->remove_piece(*this); }

const std::array<ActionID, 5>& Pion::get_actions() const {
  return piece_constants[type].actions;
}
void Pion::set_square(Coord coords) {
  (*board)[coords_] = nullptr;
  coords_ = coords;
  (*board)[coords_] = this;
}

void Pion::set_player(Player& new_player) {
  new_player.add_piece(player_->remove_piece(*this));
  player_ = &new_player;
}

void Pion::change_fatigue(int i) {
  fatigue_ += i;
  if (fatigue_ < 0) {
    fatigue_ = 0;
  }
}
void Pion::change_pv(int val) {
  pv_ += val;
  if (pv_ > get_max_pv()) {
    pv_ = get_max_pv();
  }
  if (pv_ <= 0) {
    die();
  }
}

void Pion::start_turn() { this->change_fatigue(-3); }

/*
constexpr const char piece_signs[PIECECOUNT]{' ', 'C', 'P', 'G', 'S', 'M',
                                             '@', '+', 'K', 'B', 'T'};

constexpr const char* piece_names[PIECECOUNT]{
    "Pion",  "Chateau", "Paysan",    "Guerrier",   "Seigneur", "Moine",
    "Foret", "Mine",    "Chevalier", "Bombardier", "Tour"};

constexpr const int piece_max_hp[PIECECOUNT]{0, 20, 6,  10, 5, 3,
                                             2, 5,  15, 1,  15};
constexpr const int piece_depl[PIECECOUNT]{0, 0, 2, 3, 1, 1, 0, 0, 1, 2, 2};

constexpr const int piece_puiss[PIECECOUNT]{0, 2, 1, 5, 3, 0, 0, 0, 5, 20, 3};

constexpr const int piece_atk_ran[PIECECOUNT]{0, 1, 1, 1, 2, 2, 0, 0, 4, 1, 2};

constexpr const int piece_prod[PIECECOUNT]{0, 2, 4, 0, 0, 0, 3, 5, 0, 0, 2};

constexpr const int piece_cout[PIECECOUNT]{0, 15, 10, 15, 10, 0, 0, 0, 25, 10,
15};

constexpr const std::array<ActionID, 5> piece_actions[PIECECOUNT] = {
    {},                  // Assuming the empty action list for "Pion"
    {IDLE, BUILD_UNIT},  // Actions for "Chateau"
    {IDLE, MOVE, FARM, CUT, TO_MONK},              // Actions for "Paysan"
    {IDLE, MOVE, ATTACK},                          // Actions for "Guerrier"
    {IDLE, MOVE, ATTACK, TO_CASTLE, BUILD_TOWER},  // Actions for "Seigneur"
    {IDLE, MOVE, CONVERT},                         // Actions for "Moine"
    {},                                            // Actions for "Foret"
    {},                                            // Actions for "Mine"
    {IDLE, MOVE, ATTACK},                          // Actions for "Chevalier"
    {IDLE, MOVE, EXPLODE},                         // Actions for "Bombardier"
    {IDLE, ATTACK}                                 // Actions for "Tour"
};

*/