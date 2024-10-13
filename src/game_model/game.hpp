#ifndef GAME_HPP
#define GAME_HPP
#include <array>

#include "../game_config.hpp"
#include "../helpers/user_input.hpp"
#include "board.hpp"
#include "pion.hpp"
#include "player.hpp"

enum class ActionStatus {
  Ok,
  KilledPlayer,
  NewTurn,
  // error_input,
};

class Action;
class Game {
 public:
  explicit Game(UserInput settings);

  const Board& get_board() const { return board; }
  Player& get_curr_player() { return players[curr_player_i]; }
  const Player& get_curr_player() const { return players[curr_player_i]; }
  const std::array<Player, max_players + 1>& get_players() const { return players; }
  Player& get_player(unsigned char index) { return players[index]; }
  int get_turn() const { return turn; }
  int get_initial_player_count() const { return initial_player_count; }
  int get_curr_player_count() const { return curr_player_count; }
  const Action& get_action(ActionID id) const { return *actions[index(id)]; }

  ActionStatus receive_command(const UserInput& input);

  void next_turn();
  void next_player() {
    do {
      curr_player_i = (curr_player_i + 1) % (initial_player_count + 1);
    } while (players[curr_player_i].get_status() != PlayerStatus::Alive);
  }

  int get_winner() const {
    if (curr_player_count >= 2) { return -1; }
    if (curr_player_count == 0) { return 0; }
    if (curr_player_count == 1) {
      for (const auto& player : players) {
        if (player.get_status() == PlayerStatus::Alive) { return player.get_colour(); }
      }
    }
    std::unreachable();
  }

  bool kill_player_if(unsigned char index);

 private:
  Board board;
  std::array<Player, max_players + 1> players;
  std::array<std::unique_ptr<Action>, index(ActionID::COUNT)> actions;
  unsigned char initial_player_count;  // excludes gaia
  unsigned char curr_player_count;
  unsigned char curr_player_i{0};
  int turn{-1};
};

#endif