#include "data_handler.hpp"

#include "../signalhandler.hpp"
void DataHandler::select_source(Coord source) {
  src = source;
  curr_source = (*board)[source];
  curr_actions.clear();
  if (!curr_source || curr_source.get_player_i() != curr_player_data.y) {
    return;
  }
  for (ActionID action_id : curr_source.get_actions()) {
    if (action_id == ActionID::action) {
      break;
    }
    const Action& action{game.get_action(action_id)};
    bool possible{action.other_cond(curr_source)};
    if (possible && action.takes_options()) {
      possible &= !action.get_opts(curr_source).empty();
    }
    if (possible && action.takes_squares()) {
      possible &= !action.get_dsts(source).empty();
    }
    curr_actions.emplace_back(action_id, possible);
  }
}

Coord DataHandler::get_next_piece_coords(Coord src) const {
  const auto& pieces{game.get_current_player().get_pieces()};
  massert(pieces.size());
  if (src.y == coord_senitel) {
    return pieces[0];
  }
  bool start_found = false;
  Coord first_found{src};
  for (auto piece : pieces) {
    if (piece == src) {
      start_found = true;
      continue;
    }
    if (!(*board)[piece].get_fat()) {
      if (start_found) {
        return piece;
      }
      if (first_found == src) {
        first_found = piece;
      }
    }
  }
  return first_found;
}

Coord DataHandler::get_next_target_coords(Coord dst) const {
  bool start_found{false};
  for (auto square : curr_targets) {
    if (square == dst) {
      start_found = true;
      continue;
    }
    if (start_found) {
      return square;
    }
  }
  return curr_targets[0];
}
