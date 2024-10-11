// #ifndef BUILDUNIT_ACTION_HPP
// #define BUILDUNIT_ACTION_HPP
// #include <vector>
//
// #include "../../helpers/action_data.hpp"
// #include "../board.hpp"
// #include "../pion.hpp"
// #include "../player.hpp"
// #include "action.hpp"
//
// class BuildUnit final : public Action {
//  public:
//   ActionID get_type() const override { return BUILD_UNIT; }
//
//   void perform(Coord source, int opt, Coord dst) const override {
//     Player& player = (*board)[source]->get_player();
//     PieceType type;
//     switch (opt) {
//       case 0:
//         type = PieceType::Paysan;
//         break;
//       case 1:
//         type = PieceType::Seigneur;
//         break;
//       case 2:
//         type = PieceType::Guerrier;
//         break;
//     }
//     player.add_piece(type, dst);
//     player.change_gold(-piece_constants[static_cast<size_t>(type)].cout);
//     (*board)[source]->change_fatigue(+2);
//   }
//
//   std::vector<Coord> get_poss_options(Coord source) const override {
//     std::vector<Coord> opts = {
//         {CREATE_PAYSAN, 0}, {CREATE_SEIGNEUR, 0}, {CREATE_GUERRIER, 0}};
//     int gold = (*board)[source]->get_player().get_gold();
//     if (gold >= piece_constants[static_cast<size_t>(PieceType::Paysan)].cout)
//     {
//       opts[0].x = 1;
//     }
//     if (gold >=
//         piece_constants[static_cast<size_t>(PieceType::Seigneur)].cout) {
//       opts[1].x = 1;
//     }
//     if (gold >=
//         piece_constants[static_cast<size_t>(PieceType::Guerrier)].cout) {
//       opts[2].x = 1;
//     }
//     return opts;
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