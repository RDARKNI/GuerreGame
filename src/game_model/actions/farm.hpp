#ifndef PRODUCE_ACTION_HPP
#define PRODUCE_ACTION_HPP
#include "../../helpers/action_data.hpp"
#include "action.hpp"

class Harvest final : public Action {
 public:
  ActionID get_type() const override { return FARM; }
  void perform(Coord source, int opt, Coord target) const override {
    Pion& actor = *(*board)[source];
    actor.get_player().change_gold(actor.get_prod());
    actor.change_fatigue(2);
  }
};

#endif