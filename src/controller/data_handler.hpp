#ifndef DATAHANDLER
#define DATAHANDLER

#include <vector>

#include "../game_model/game.hpp"
#include "../helpers/user_input.hpp"
#include "../ui/ncurses_setup.hpp"
/*
For communication between the Game and both the Controller and the UI
Requests Data from the Game whenever prompted by the Controller and UI.
*/
struct SquareData {
  int data;
  SquareData() : data{0} {}
  SquareData(int d) : data{d} {}
  Piece_ID type() const { return static_cast<Piece_ID>(data & 0xF); }
  int player() const { return (data >> 4) & 0xF; }
  int fatigue() const { return (data >> 8) & 0xF; }
  int hp() const { return (data >> 12); }
};
struct ActionDat {
  ActionID type;
  int value;
};
struct OptionDat {
  OptionID type;
  int value;
};
struct PlayerData {
  int index;
  int gold;
};
struct MenuItem {
  const char* str;
  int value;
};
class DataHandler {
 public:
  DataHandler(const UserInput& inputs);

  std::vector<MenuItem> get_action_items() const;
  std::vector<MenuItem> get_option_items() const;

  chtype get_sqdat(Coord coords) const;
  SquareData get_piece_data(Coord coords) const;

  const ActionDat& get_curr_action() const;
  const std::vector<ActionDat>& get_curr_actions() const;
  const std::vector<OptionDat>& get_curr_options() const;

  const std::vector<Coord>& get_curr_targets() const;

  int get_current_turn() const;

  const std::vector<UserInput>& get_history() const;

  PlayerData get_current_player_data() const;

  Coord get_next_piece_coords() const;
  Coord get_next_target_coords() const;

  int send_command();

  void start_selection();

  void request_action_data();

 private:
  Game game;
  std::vector<ActionDat> act_data;
  std::vector<std::vector<OptionDat>> act_option_data;
  std::vector<std::vector<Coord>> act_target_data;
  std::vector<UserInput> history;
  const Board& board{game.get_board()};
  const int& curr_turn{game.get_current_turn()};
  const UserInput& inputs;
};

#endif