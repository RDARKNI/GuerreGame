// #ifndef TO_CASTLE_ACTION_HPP
// #define TO_CASTLE_ACTION_HPP
//
// #include "../../helpers/action_data.hpp"
// #include "../player.hpp"
// #include "action.hpp"
//
// class ToCastle final : public Action {
//  public:
//   ActionID get_type() const override { return TO_CASTLE; }
//
//   void perform(Coord source, int, Coord) const override {
//     Player& player = (*board)[source]->get_player();
//     player.remove_piece(*(*board)[source]);
//     player.add_piece(PieceType::Chateau, source);
//     player.change_gold(
//         -piece_constants[static_cast<size_t>(PieceType::Chateau)].cout);
//   }
//   bool get_fatigue_condition(Coord source) const override {
//     const Pion& actor = *(*board)[source];
//     return !actor.get_fatigue() &&
//            actor.get_player().get_gold() >=
//                piece_constants[static_cast<size_t>(PieceType::Chateau)].cout;
//   }
// };
//
// #endif