// #ifndef DATAHANDLER
// #define DATAHANDLER
//
// #include <vector>
//
// #include "../game_model/actions/action.hpp"
// #include "../game_model/game.hpp"
// #include "../helpers/coords.hpp"
// #include "../helpers/user_input.hpp"
//
// class DataHandler {
//  public:
//   DataHandler(UserInput init_settings)
//       : game{init_settings},
//         board{&game.get_board()},
//         curr_turn{game.get_turn()} {}
//
//   const std::vector<ActionData>& get_curr_actions() const {
//     return curr_actions;
//   }
//   const std::vector<OptionData>& get_curr_options() const {
//     return curr_options;
//   }
//   const std::vector<Coord>& get_curr_targets() const { return curr_targets; }
//   const Pion& get_curr_source() const { return curr_source; }
//   const Pion& get_curr_target() const { return curr_target; }
//   const Coord& get_curr_player_data() const { return curr_player_data; }
//   const int& get_curr_turn() const { return curr_turn; }
//   int get_curr_player_count() const { return game.get_curr_player_count(); }
//   const std::array<Player, 9>& get_players() const {
//     return game.get_players();
//   }
//
//   const Board& get_board() { return *board; }
//
//   Coord get_next_piece_coords(Coord dst) const;
//
//   size_t get_next_action_index(ActionID act) const {
//     if (act == ActionID::action) {
//       return 0;
//     }
//     for (size_t i{}; i < curr_actions.size(); ++i) {
//       if (curr_actions[i].y == to_index(act) && curr_actions[i].x) {
//         return i;
//       }
//     }
//     return 0;
//   }
//   size_t get_next_option_index(OptionID opt) const {
//     if (opt == OptionID::option) {
//       return 0;
//     }
//     for (size_t i{}; i < curr_options.size(); ++i) {
//       if (curr_options[i].y == to_index(opt) && curr_options[i].x) {
//         return i;
//       }
//     }
//     return 0;
//   }
//   Coord get_next_target_coords(Coord dst) const;
//
//   void select_source(Coord source);
//   void select_action(int act_index) {
//     const Action&
//     action{game.get_action(ActionID(curr_actions[act_index].y))};
//     curr_options = action.get_opts(curr_source);
//     curr_targets = action.get_dsts(src);
//   }
//   void select_target(Coord target) { curr_target = (*board)[target]; }
//   void refresh_player_data() {
//     curr_player_data.y = game.get_curr_player().get_colour();
//     curr_player_data.x = game.get_curr_player().get_gold();  // todo
//   }
//   void refresh_curr_turn() { curr_turn = game.get_turn(); }
//
//   void start_selection() {
//     curr_options = game.get_action(ActionID::create_unit).get_opts(Pion());
//   }
//   ActionStatus send_command(const UserInput& inputs) {
//     fprintf(stderr, "source: %d,%d\n", inputs.dst.y, inputs.dst.x);
//     return game.receive_command(inputs);
//   }
//   const Game& get_game() { return game; }
//   bool validate_input(const UserInput& inputs) {
//     if (inputs.act < 1 || inputs.act >= ActionID::COUNT) {
//       return false;  // invalid action
//     }
//     const Action& action{game.get_action(ActionID(inputs.act))};
//     if (!board->in_bounds(inputs.src)) {
//       return false;  // invalid source square
//     }
//     const Pion& actor{(*board)[inputs.src]};
//     if (action.get_type() == ActionID::create_unit) {
//       if (curr_turn > 0) {
//         return false;
//       }
//       if (actor) {
//         return false;
//       }
//       for (auto opt : action.get_opts(actor)) {
//         if (inputs.opt == opt.y && !opt.x) {
//           return false;
//         }
//       }
//       return true;
//     }
//     if (action.get_type() == ActionID::end_turn) {
//       if (curr_turn < 1) {
//         return false;
//       }
//       return true;  // Can always end turn
//     }
//     if (actor.get_player_index() != curr_player) {
//       return false;
//     }
//     if (!action.get_fatigue_condition(actor)) {
//       return false;
//     }
//     if (action.takes_options()) {
//       bool valid_opt = false;
//       for (auto opt : action.get_opts(actor)) {
//         if (inputs.opt == opt.y && opt.x) {
//           valid_opt = true;
//           break;
//         }
//       }
//       if (!valid_opt) {
//         return false;
//       }
//     }
//     if (action.takes_squares()) {
//       bool valid_square = false;
//       for (auto sq : action.get_dsts(inputs.src)) {
//         if (inputs.dst == sq) {
//           valid_square = true;
//           break;
//         }
//       }
//       if (!valid_square) {
//         return false;
//       }
//     }
//     return true;
//   }
//
//  private:
//   Game game;
//   std::vector<ActionData> curr_actions;
//   std::vector<OptionData> curr_options;
//   // std::vector<Coord> curr_actions;
//   // std::vector<Coord> curr_options;
//   std::vector<Coord> curr_targets;
//   Pion curr_source;
//   Pion curr_target;
//   Coord src;
//   Coord curr_player_data;
//   const Board* board;
//   int curr_turn;
//   int curr_player;
//   std::vector<int> encode() {
//     std::vector<int> encoded{game.get_curr_player_count()};
//     /*
//     assume 16*16 board
//     player_count (int)
//     player_count * int (gold for each player)
//     16*16*(
//     if(!occupied): 1 bit (0)
//     if(gaia): 2 bits(type) 4 bits (hp)
//     else:
//     3 bits (player, 0 is 1 etc.)
//     5 bits (type)
//     6 bits (hp, 1-65)
//     3 bits (fatigue, 0-8)
//     )
//     */
//     return encoded;
//   }
// };
//
// #endif
