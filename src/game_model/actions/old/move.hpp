// #ifndef MOVE_ACTION_HPP
// #define MOVE_ACTION_HPP
// #include <queue>
// #include <unordered_set>
//
// #include "../../helpers/action_data.hpp"
// #include "../player.hpp"
// #include "action.hpp"
//
// template <>
// struct std::hash<Coord> {
//   std::size_t operator()(const Coord& c) const { return (c.y << 8) + c.x; }
// };
//
// class Move final : public Action {
//  public:
//   ActionID get_type() const override { return MOVE; }
//
//   void perform(Coord source, int, Coord dst) const override {
//     (*board)[source]->change_fatigue(+1);
//     (*board)[source]->set_square(dst);
//   }
//
//   const std::vector<Coord> get_poss_squares(Coord source) const override {
//     std::vector<Coord> squares;
//     const Player& player = (*board)[source]->get_player();
//     std::queue<Coord> visited{{source}};  // squares we can move through
//     std::unordered_set<Coord> bord{source};
//     int mov = (*board)[source]->get_depl();
//     while (mov) {
//       for (size_t i{visited.size()}; i; --i) {
//         for (const Coord sq : board->get_neighbours(visited.front())) {
//           if (bord.count(sq)) {
//             continue;
//           }
//           bord.insert(sq);
//           const Pion* const piece{(*board)[sq]};
//           if (!piece) {
//             squares.push_back(sq);
//             visited.push(sq);
//           } else if (player == piece->get_player()) {
//             visited.push(sq);
//           }
//         }
//         visited.pop();
//       }
//       --mov;
//     }
//     return squares;
//   }
//   /*  const std::vector<Coord> get_poss_squares(Coord source) const override
//   {
//     std::vector<Coord> squares;
//     const Player& player = (*board)[source]->get_player();
//     int mov = (*board)[source]->get_depl();
//     std::queue<Coord> visited{{source}};  // squares we can move through
//     const int vstart = std::max(0, source.y - mov);
//     const int hstart = std::max(0, source.x - mov);
//     const int height = std::min(source.y + mov, (int)B_H) - vstart + 1;
//     const int width = std::min(source.x + mov, (int)B_W) - hstart + 1;
//     std::vector<bool> brd(height * width);
//     brd[(source.y - vstart) * width + source.x - hstart] = true;
//     while (mov) {
//       for (int i = visited.size(); i; --i) {
//         for (const Coord sq : board->get_neighbours(visited.front())) {
//           const int index = (sq.y - vstart) * width + sq.x - hstart;
//           if (brd[index]) {
//             continue;
//           }
//           brd[index] = true;
//           if (!(*board)[sq]) {
//             squares.push_back(sq);
//             visited.push(sq);
//           } else if (player == (*board)[sq]->get_player()) {
//             visited.push(sq);
//           }
//         }
//         visited.pop();
//       }
//       --mov;
//     }
//     return squares;
//   }*/
// };
//
// #endif