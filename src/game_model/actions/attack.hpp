#ifndef ATTACK_ACTION_HPP
#define ATTACK_ACTION_HPP
#include <vector>

#include "../../helpers/action_data.hpp"
#include "../../helpers/coords.hpp"
#include "../board.hpp"
#include "../pion.hpp"
#include "../player.hpp"
#include "action.hpp"

class Attack final : public Action {
 public:
  ActionID get_type() const override { return ATTACK; }
  void perform(Coord source, int opt, Coord target) const override {
    Pion& actor = *(*board)[source];
    Pion* actee = (*board)[target];
    actee->change_pv(-actor.get_puiss());
    if (actee->get_pv() <= 0) {
      actee = nullptr;
    }
    actor.change_fatigue(2);
  }
  bool get_fatigue_condition(Coord source) const override {
    return (*board)[source]->get_fatigue() < 2;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    const Pion& actor = *(*board)[source];
    const Player& player = actor.get_player();
    for (const Coord square :
         board->get_neighbours(source, actor.get_atkran())) {
      const Pion* const occ = (*board)[square];
      if (occ && occ->get_player().get_colour() &&
          occ->get_player() != player) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

#endif