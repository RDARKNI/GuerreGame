#ifndef DATAHANDLER
#define DATAHANDLER

#include <vector>

#include "../game_model/game.hpp"
#include "../helpers/coords.hpp"
#include "../helpers/dtos.hpp"
#include "../helpers/user_input.hpp"

class DataHandler {
 public:
  explicit DataHandler(UserInput init_settings);

  SquareData get_square_data(Coord coords) const;

  // design issue
  std::vector<SquareData> get_squares_data(
      const std::vector<Coord>& coords) const;

  const std::vector<Coord>& get_curr_actions() const { return curr_actions; }
  const std::vector<Coord>& get_curr_options() const { return curr_options; }
  const std::vector<Coord>& get_curr_targets() const { return curr_targets; }
  const SquareData& get_curr_source() const { return curr_source; }
  const SquareData& get_curr_target() const { return curr_target; }
  const Coord& get_curr_player_data() const { return curr_player_data; }
  const int& get_curr_turn() const { return curr_turn; }

  Coord get_next_piece_coords(Coord dst) const;
  Coord get_next_target_coords(Coord dst) const;

  void select_source(Coord source);
  void select_action(int act) {
    Action& action = *(game.actions[curr_actions[act].y]);
    curr_options = action.get_poss_options(src);
    curr_targets = action.get_poss_squares(src);
  }
  void select_target(Coord target) { curr_target = get_square_data(target); }
  void refresh_player_data() {
    curr_player_data.y = game.get_current_player().get_colour();
    curr_player_data.x = game.get_current_player().get_gold();
  }
  void refresh_curr_turn() { curr_turn = game.get_current_turn(); }
  void start_selection();
  int send_command(UserInput inputs) { return game.receive_command(inputs); }

 private:
  Game game;
  std::vector<Coord> curr_actions;
  std::vector<Coord> curr_options;
  std::vector<Coord> curr_targets;
  SquareData curr_source;
  SquareData curr_target;
  Coord src;
  Coord curr_player_data;
  const Board& board{game.get_board()};
  int curr_turn{game.get_current_turn()};
};

#endif