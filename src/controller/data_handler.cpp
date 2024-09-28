#include "data_handler.hpp"

DataHandler::DataHandler(UserInput init_settings) : game{init_settings} {}

SquareData DataHandler::get_square_data(Coord coords) const {
  const Pion& occ = board[coords];
  if (!occ) {
    return SquareData{0};
  }
  return SquareData{static_cast<unsigned int>(occ.get_type()) +
                    (occ.get_player().get_colour() << 4) +
                    (occ.get_fatigue() << 8) +
                    ((occ.get_player().get_colour() != curr_player_data.y ||
                      occ.get_fatigue())
                     << 12) +
                    (occ.get_pv() << 13)};
}
std::vector<SquareData> DataHandler::get_squares_data(
    const std::vector<Coord>& coords) const {
  std::vector<SquareData> data;
  data.reserve(coords.size());
  for (auto coord : coords) {
    data.push_back(get_square_data(coord));
  }
  return data;
}

void DataHandler::select_source(Coord source) {
  src = source;
  curr_source = get_square_data(source);
  curr_actions.clear();
  const Pion& occ = board[source];
  if (!occ || occ.get_player().get_colour() != curr_player_data.y) {
    return;
  }
  for (ActionID action_id : occ.get_actions()) {
    if (action_id == ACTION) {
      break;
    }
    Action& action = *(game.actions[action_id]);
    bool possible{action.get_fatigue_condition(source)};
    if (possible && action.takes_options()) {
      possible &= !action.get_poss_options(source).empty();
    }
    if (possible && action.takes_squares()) {
      possible &= !action.get_poss_squares(source).empty();
    }
    curr_actions.emplace_back(action_id, possible);
  }
}

Coord DataHandler::get_next_piece_coords(Coord src) const {
  if (src.y < 0) {
    return game.get_current_player().get_pieces()[0];
  }
  bool start_found = false;
  for (const auto& piece : game.get_current_player().get_pieces()) {
    if (piece == src) {
      start_found = true;
    } else if (!board[piece].get_fatigue() && start_found) {
      return piece;
    }
  }
  for (const auto& piece : game.get_current_player().get_pieces()) {
    if (!board[piece].get_fatigue()) {
      return piece;
    }
  }
  return src;
}

Coord DataHandler::get_next_target_coords(Coord dst) const {
  bool start_found{false};
  for (const auto& sq : curr_targets) {
    if (sq == dst) {
      start_found = true;
    } else if (start_found) {
      return sq;
    }
  }
  return curr_targets[0];
}

void DataHandler::start_selection() {
  const auto& pieces{game.get_current_player().get_pieces()};
  if (pieces.empty()) {
    curr_options = {{CREATE_CHATEAU, 1}, {CREATE_PAYSAN, 1}};
  } else if (pieces.size() == 2) {
    curr_options = {};
  } else if (board[pieces[0]].get_type() == PieceType::Chateau) {
    curr_options = {{CREATE_CHATEAU, 0}, {CREATE_PAYSAN, 1}};
  } else {
    curr_options = {{CREATE_CHATEAU, 1}, {CREATE_PAYSAN, 0}};
  }
}
