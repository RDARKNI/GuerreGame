#ifndef BUILDUNIT_ACTION_HPP
#define BUILDUNIT_ACTION_HPP
#include <memory>
#include <vector>

#include "../../helpers/action_data.hpp"
#include "../board.hpp"
#include "../pion.hpp"
#include "../player.hpp"
#include "action.hpp"

class BuildUnit final : public Action {
 public:
  ActionID get_type() const override { return BUILD_UNIT; }

  void perform(Coord source, int opt, Coord target) const override {
    Player& player = (*board)[source]->get_player();
    Piece_ID type;
    switch (opt) {
      case 0:
        type = PAYSAN;
        break;
      case 1:
        type = SEIGNEUR;
        break;
      case 2:
        type = GUERRIER;
        break;
    }
    player.add_piece(std::make_unique<Pion>(player, type, target));
    player.change_gold(-piece_constants[type].cout);
    (*board)[source]->change_fatigue(+2);
  }

  std::vector<OptionID> get_poss_options(Coord source) const override {
    std::vector<OptionID> opts;
    int gold = (*board)[source]->get_player().get_gold();
    if (gold >= piece_constants[PAYSAN].cout) {
      opts.push_back(CREATE_PAYSAN);
    }
    if (gold >= piece_constants[SEIGNEUR].cout) {
      opts.push_back(CREATE_SEIGNEUR);
    }
    if (gold >= piece_constants[GUERRIER].cout) {
      opts.push_back(CREATE_GUERRIER);
    }
    return opts;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    for (const Coord square : board->get_neighbours(source)) {
      if (!(*board)[square]) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

#endif