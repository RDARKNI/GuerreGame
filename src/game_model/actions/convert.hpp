#ifndef CONVERT_ACTION_HPP
#define CONVERT_ACTION_HPP
#include "../../helpers/action_data.hpp"
#include "../board.hpp"
#include "../pion.hpp"
#include "../player.hpp"
#include "action.hpp"

class Convert final : public Action {
 public:
  ActionID get_type() const override { return CONVERT; }
  void perform(Coord source, int opt, Coord target) const override {
    (*board)[target]->set_player((*board)[source]->get_player());
    (*board)[source]->change_fatigue(+2);
    (*board)[target]->change_fatigue(+2);
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