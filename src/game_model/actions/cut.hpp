#ifndef CUT_WOOD_ACTION_HPP
#define CUT_WOOD_ACTION_HPP
#include "../../helpers/action_data.hpp"
#include "../board.hpp"
#include "action.hpp"

class Cut final : public Action {
 public:
  ActionID get_type() const override { return CUT; }
  void perform(Coord source, int opt, Coord target) const override {
    Pion* const actor = (*board)[source];
    int harvest = (*board)[target]->get_prod();
    actor->get_player().change_gold(+harvest);
    (*board)[target]->change_pv(-actor->get_puiss());
    actor->change_fatigue(+2);
    if (!(*board)[target]) {
      actor->get_player().change_gold(+harvest * 2);
    }
  }
  bool get_fatigue_condition(Coord source) const override {
    return (*board)[source]->get_fatigue() < 2;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    for (const Coord square : board->get_neighbours(source)) {
      const Pion* const actee{(*board)[square]};
      if (actee && !actee->get_player().get_colour()) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

#endif