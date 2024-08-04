#ifndef MOVE_ACTION_HPP
#define MOVE_ACTION_HPP
#include <queue>

#include "../../helpers/action_data.hpp"
#include "action.hpp"

class Move final : public Action {
 public:
  ActionID get_type() const override { return MOVE; }

  void perform(Coord source, int opt, Coord target) const override {
    (*board)[source]->change_fatigue(+1);
    (*board)[source]->set_square(target);
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    const Pion& actor = *(*board)[source];
    const Player& player = actor.get_player();
    int mov = actor.get_depl();
    const int vstart = std::max(0, source.y - mov);
    const int hstart = std::max(0, source.x - mov);
    std::queue<Coord> visited{};
    std::vector<std::vector<bool>> bord(
        std::min(source.y + mov, (int)B_H) - vstart + 1,
        std::vector<bool>(std::min(source.x + mov, (int)B_W) - hstart + 1));
    visited.push(source);
    bord[source.y - vstart][source.x - hstart] = 1;
    for (; mov; mov--) {
      for (int i = visited.size(); i; --i) {
        for (const Coord sq : board->get_neighbours(visited.front())) {
          const int ny = sq.y - vstart;
          const int nx = sq.x - hstart;
          if (bord[ny][nx]) {
            continue;
          }
          bord[ny][nx] = true;
          if (!(*board)[sq]) {
            squares.push_back(sq);
          }
          if (!(*board)[sq] || player == (*board)[sq]->get_player()) {
            visited.push(sq);
          }
        }
        visited.pop();
      }
    }
    return squares;
  }
};

#endif