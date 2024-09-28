#ifndef GAME_HPP
#define GAME_HPP
#include <array>
#include <memory>
#include <vector>

#include "../helpers/action_data.hpp"
#include "../helpers/user_input.hpp"
#include "actions/action.hpp"
// #include "actions/attack.hpp"
// #include "actions/build_tower.hpp"
// #include "actions/build_unit.hpp"
// #include "actions/convert.hpp"
// #include "actions/cut.hpp"
// #include "actions/do_nothing.hpp"
// #include "actions/explode.hpp"
// #include "actions/farm.hpp"
// #include "actions/move.hpp"
// #include "actions/to_castle.hpp"
// #include "actions/to_monk.hpp"
#include "board.hpp"
#include "pion.hpp"
#include "player.hpp"

class Game {
 public:
  explicit Game(UserInput settings);

  inline static const std::array<std::unique_ptr<Action>, ACTIONCOUNT> actions{
      std::make_unique<Move>(),      std::make_unique<Move>(),
      std::make_unique<Attack>(),    std::make_unique<BuildUnit>(),
      std::make_unique<Harvest>(),   std::make_unique<ToCastle>(),
      std::make_unique<DoNothing>(), std::make_unique<Convert>(),
      std::make_unique<ToMonk>(),    std::make_unique<Cut>(),
      std::make_unique<Explode>(),   std::make_unique<BuildTower>()};

  const Board& get_board() const { return board; }
  const Player& get_current_player() const { return *players[curr_player_i]; }
  int get_current_turn() const { return turn; }

  int receive_command(const UserInput& input);

 private:
  Board board;
  int initial_player_count;  // excludes gaia
  char curr_player_i{0};     // NOT the current player colour
  std::vector<std::unique_ptr<Player>> players;
  int curr_player_count;
  int turn{-1};

  int kill_players();
  int act(const UserInput& input);
  int end_turn();
  int create_unit(const UserInput& inputs);
};

#endif