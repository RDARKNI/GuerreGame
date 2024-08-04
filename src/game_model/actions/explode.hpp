#ifndef EXPLODE_ACTION_HPP
#define EXPLODE_ACTION_HPP
#include <vector>

#include "../../helpers/action_data.hpp"
#include "../../helpers/coords.hpp"
#include "../board.hpp"
#include "../pion.hpp"
#include "../player.hpp"
#include "action.hpp"

class Explode final : public Action {
 public:
  ActionID get_type() const override { return EXPLODE; }
  void perform(Coord source, int opt, Coord target) const override {
    Pion& actor = *(*board)[source];
    for (Coord sq : board->get_neighbours(source)) {
      (*board)[sq]->change_pv(-actor.get_puiss());
    }
    actor.get_player().remove_piece(actor);
  }
};

#endif