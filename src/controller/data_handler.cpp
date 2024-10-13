#include "data_handler.hpp"

#include <algorithm>
#include <ranges>

#include "../signalhandler.hpp"
bool DataHandler::test_can_act(ActionID type, Coord src) {
  const Action& action{game.get_action(type)};
  const Pion& actor{(*board)[src]};
  bool possible{action.other_cond(actor)};
  possible &= !action.takes_opts() ||
              std::ranges::any_of(action.get_opts(actor), [](OptionData opt) { return opt.mods; });
  possible &= !action.takes_dsts() || !action.get_dsts(src).empty();
  return possible;
}

void DataHandler::select_source(Coord source) {
  src = source;
  curr_source = (*board)[source];
  curr_actions.clear();
  if (curr_turn > 0 && (!curr_source || curr_source.get_player_i() != curr_player_data.y)) { return; }
  for (ActionID action_id : curr_source.get_actions()) {
    if (action_id == ActionID::Sentinel) { break; }
    curr_actions.emplace_back(action_id, test_can_act(action_id, src));
  }
}

Coord DataHandler::get_next_src_coords(Coord src) const {
  if (curr_turn < 1) {
    if (src != Coord::sentinel) { return src; }
    for (auto i{0}; i < board->get_height(); ++i) {
      for (auto j{0}; j < board->get_width(); ++j) {
        if (!(*board)[Coord(i, j)]) { return Coord(i, j); }
      }
    }
    return {0, 0};
  }
  const auto& pieces{game.get_curr_player().get_pieces()};
  if (src == Coord::sentinel) { return pieces[0]; }
  bool start_found = false;
  Coord first_found{src};
  for (auto piece : pieces) {
    if (piece == src) {
      start_found = true;
    } else if (!(*board)[piece].get_fat()) {
      if (start_found) { return piece; }
      if (first_found == src) { first_found = piece; }
    }
  }
  return first_found;
}
