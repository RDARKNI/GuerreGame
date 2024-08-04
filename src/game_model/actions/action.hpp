#ifndef ACTION_HPP
#define ACTION_HPP
#include <vector>

#include "../../helpers/action_data.hpp"
#include "../../helpers/coords.hpp"
#include "../board.hpp"
#include "../pion.hpp"

class Action {
 public:
  inline static Board* board;
  virtual ~Action() = default;
  virtual ActionID get_type() const = 0;
  const char* get_name() const { return action_names[get_type()]; }
  bool takes_options() const { return action_takes_options[get_type()]; }
  bool takes_squares() const { return action_takes_squares[get_type()]; }

  virtual bool get_fatigue_condition(Coord source) const {
    return !(*board)[source]->get_fatigue();
  }
  virtual void perform(Coord source, int opt, Coord target) const = 0;
  virtual std::vector<OptionID> get_poss_options(Coord source) const {
    return {};
  }

  virtual const std::vector<Coord> get_poss_squares(Coord source) const {
    return {};
  };
};

#endif
