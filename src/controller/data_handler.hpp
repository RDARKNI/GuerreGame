#ifndef DATAHANDLER
#define DATAHANDLER

#include <vector>

#include "../game_model/actions/action.hpp"
#include "../game_model/game.hpp"
#include "../helpers/coords.hpp"
#include "../helpers/user_input.hpp"
/*
Give all pieces end turn as action ?
Nullpion has create unit as action ?
*/
class DataHandler {
 public:
  DataHandler(UserInput init_settings)
      : game{init_settings}, board{&game.get_board()}, curr_turn{game.get_turn()} {}

  const std::vector<ActionData>& get_curr_actions() const { return curr_actions; }
  const std::vector<OptionData>& get_curr_options() const { return curr_options; }
  const std::vector<Coord>& get_curr_targets() const { return curr_targets; }
  const Pion& get_curr_source() const { return curr_source; }
  const Pion& get_curr_target() const { return curr_target; }
  const Coord& get_curr_player_data() const { return curr_player_data; }
  const int& get_curr_turn() const { return curr_turn; }
  const std::array<Player, 9>& get_players() const { return game.get_players(); }

  const Board& get_board() { return *board; }

  Coord get_next_src_coords(Coord dst) const;

  /*
  If there isn't one

  We iterate until we find act
  Then


  */
  /*
  If it's senitel, find the first valid one
    ->If there is no valid one, return 0
  If not senitel, first find the actionid.
    ->Then, keep looping until you hit a valid
    ->If you hit actionid again, return it (no matches)



  */

  void select_source(Coord source);
  void select_action(size_t act_index) {
    if (curr_actions[act_index].mods) {
      const Action& action{game.get_action(curr_actions[act_index].type)};
      curr_options = action.get_opts(curr_source);
      curr_targets = action.get_dsts(src);
    }
  }
  void select_target(Coord target) { curr_target = (*board)[target]; }
  void refresh_player_data() {
    curr_player_data.y = game.get_curr_player().get_colour();
    curr_player_data.x = game.get_curr_player().get_gold();  // todo
  }
  void refresh_curr_turn() { curr_turn = game.get_turn(); }

  ActionStatus transmit_command(const UserInput& inputs) {
    // fprintf(stderr, "source: %d,%d\n", inputs.dst.y, inputs.dst.x);
    return game.receive_command(inputs);
  }
  const Game& get_game() { return game; }

  bool validate_input(const UserInput& inputs) {
    if (inputs.act <= ActionID::Sentinel || inputs.act >= ActionID::COUNT) {
      return false;  // invalid action
    }
    const Action& action{game.get_action(inputs.act)};
    if (!board->in_bounds(inputs.src)) {
      return false;  // invalid source square
    }
    const Pion& actor{(*board)[inputs.src]};
    if (action.get_type() == ActionID::CreateUnit) {  // todo simplify
      if (curr_turn > 0) { return false; }
      if (actor) { return false; }
      for (auto opt : action.get_opts(actor)) {
        if (inputs.opt == opt.type && !opt.mods) { return false; }
      }
      return true;
    }
    if (action.get_type() == ActionID::EndTurn) {
      if (curr_turn < 1) { return false; }
      return true;  // Can always end turn
    }
    if (actor.get_player_i() != curr_player) { return false; }
    if (!action.other_cond(actor)) { return false; }
    if (action.takes_opts()) {
      bool valid_opt = false;
      for (auto opt : action.get_opts(actor)) {
        if (inputs.opt == opt.type && opt.mods) {
          valid_opt = true;
          break;
        }
      }
      if (!valid_opt) { return false; }
    }
    if (action.takes_dsts()) {
      bool valid_square = false;
      for (auto sq : action.get_dsts(inputs.src)) {
        if (inputs.dst == sq) {
          valid_square = true;
          break;
        }
      }
      if (!valid_square) { return false; }
    }
    return true;
  }
  bool test_can_act(ActionID action, Coord src);

 private:
  Game game;
  // std::vector<Coord> curr_sources;  // list of actionable pieces?
  std::vector<ActionData> curr_actions;
  std::vector<OptionData> curr_options;
  std::vector<Coord> curr_targets;
  Pion curr_source;
  Pion curr_target;
  Coord src;
  Coord curr_player_data;
  const Board* board;
  int curr_turn;
  int curr_player;
  // std::vector<int> encode() {
  //   std::vector<int> encoded{game.get_curr_player_count()};
  //   /*
  //   assume 16*16 board
  //   player_count (int)
  //   player_count * int (gold for each player)
  //   16*16*(
  //   if(!occupied): 1 bit (0)
  //   if(gaia): 2 bits(type) 4 bits (hp)
  //   else:
  //   3 bits (player, 0 is 1 etc.)
  //   5 bits (type)
  //   6 bits (hp, 1-65)
  //   3 bits (fatigue, 0-8)
  //   )
  //   */
  //   return encoded;
  // }
};

int test1();
void test2();
#endif
