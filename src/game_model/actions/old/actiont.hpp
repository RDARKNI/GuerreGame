// #ifndef ACTION_HPP
// #define ACTION_HPP
//
// #include <vector>
//
// #include "../../helpers/coords.hpp"
// #include "../../helpers/piece_data.hpp"
// #include "../board.hpp"
// #include "../game.hpp"
// #include "../pion.hpp"
// #include "../player.hpp"
//
//// Base class for all actions
// class Action {
//  public:
//   Action(Game& game, Board& board) : game{game}, board{board} {}
//   virtual ~Action() = default;
//
//   Game& game;
//   Board& board;
//
//   constexpr virtual ActionID get_type() const = 0;
//   constexpr bool takes_options() const {
//     return action_takes_options(get_type());
//   }
//   constexpr bool takes_squares() const {
//     return action_takes_squares[get_type()];
//   }
//   virtual bool get_fatigue_condition(Coord source) const {
//     return board[source].get_fatigue() < 1;
//   }
//   virtual std::vector<Coord> get_poss_options(Coord source) const { return
//   {}; } virtual std::vector<Coord> get_poss_squares(Coord source) const {
//   return {}; } virtual ActionStatus perform(const UserInput& inputs) = 0;
//
//  protected:
//   inline bool is_in_bounds(Coord c) const { return board.in_bounds(c); }
//   inline bool is_occupied(Coord s) const { return board[s]; }
//   inline bool is_free(Coord s) const { return !board[s]; }
//   inline bool is_valid_player(char i) const {
//     return i >= 0 && i < game.get_players().size();
//   }
//   inline bool is_current_player(const Player& player) const {
//     return game.get_current_player() == player;
//   }
//   inline bool is_valid_unit(char opt) const {
//     return opt >= 0 && opt < static_cast<char>(PieceType::PIECECOUNT);
//   }
//
//   inline bool can_afford(const Player& player, PieceType type) const {
//     return player.get_gold() >= pieces_data[int(type)].cout;
//   }
//
//   ActionStatus helper_create_new_unit(Coord src, PieceType type, Coord dst);
//   std::vector<Coord> helper_get_free_neighbours(Coord src) const;
//   std::vector<Coord> helper_get_foe_neighbours(Coord src) const;
//   bool helper_can_afford(Coord src, PieceType type) const;
// };
//
// #endif
