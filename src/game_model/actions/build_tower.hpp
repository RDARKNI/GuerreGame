// #ifndef BUILDTOWER_ACTION_HPP
// #define BUILDTOWER_ACTION_HPP
// #include <vector>
//
// #include "../../helpers/action_data.hpp"
// #include "../board.hpp"
// #include "../pion.hpp"
// #include "../player.hpp"
// #include "action.hpp"
//
// class BuildTower final : public Action {
//  public:
//   ActionID get_type() const override { return BUILD_TOWER; }
//   void perform(Coord source, int, Coord dst) const override {
//     Player& player = (*board)[source]->get_player();
//     player.add_piece(PieceType::Tour, dst);
//     player.change_gold(
//         -piece_constants[static_cast<size_t>(PieceType::Tour)].cout);
//   }
//
//   bool get_fatigue_condition(Coord source) const override {
//     const Pion& actor = *(*board)[source];
//     return !actor.get_fatigue() &&
//            actor.get_player().get_gold() >=
//                piece_constants[static_cast<size_t>(PieceType::Tour)].cout;
//   }
//   const std::vector<Coord> get_poss_squares(Coord source) const override {
//     std::vector<Coord> squares;
//     for (const Coord square : board->get_neighbours(source)) {
//       if (!(*board)[square]) {
//         squares.push_back(square);
//       }
//     }
//     return squares;
//   }
// };
//
// #endif