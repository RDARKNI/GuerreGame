#ifndef TO_MONK_ACTION_HPP
#define TO_MONK_ACTION_HPP
#include <vector>

#include "../../helpers/action_data.hpp"
#include "action.hpp"

class ToMonk final : public Action {
 public:
  ActionID get_type() const override { return TO_MONK; }

  bool get_fatigue_condition(Coord source) const override {
    const Pion& actor = *(*board)[source];
    return !actor.get_fatigue() &&
           actor.get_player().get_gold() >= piece_constants[MOINE].cout;
  }
  void perform(Coord source, int opt, Coord target) const override {
    Player& player = (*board)[source]->get_player();
    player.remove_piece(*(*board)[source]);
    player.add_piece(std::make_unique<Pion>(player, MOINE, source));
    player.change_gold(-piece_constants[MOINE].cout);
  }
};

#endif