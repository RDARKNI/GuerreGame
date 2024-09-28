#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>

#include "../helpers/coords.hpp"
#include "pion.hpp"

class Board {
 public:
  explicit Board(Coord dimensions)
      : B_H{dimensions.y},
        B_W{dimensions.x}  //, board(B_H * B_W, Pion())
  {}

  // template <class Self>
  // constexpr auto&& operator[](this Self&& self, Coord coords) {
  //   return std::forward<Self>(self).board_[coords.y * self.B_W + coords.x];
  // }
  Pion& operator[](Coord coords) { return board[coords.y * B_W + coords.x]; }
  const Pion& operator[](Coord coords) const {
    return board[coords.y * B_W + coords.x];
  }
  bool in_bounds(Coord c) const { return c.y < B_H && c.x < B_W; }

  // 2*range
  std::vector<Coord> get_neighbours(Coord square, char range = 1) const {
    std::vector<Coord> neighbours;
    int y{square.y};
    int x{square.x};
    for (int dy = 0; dy <= range; ++dy) {
      for (int dx = 0; dx + dy <= range; ++dx) {
        if (dy == 0 && dx == 0) {
          continue;
        }
        int new_y{y + dy};
        int new_x{x + dx};
        if (new_y < B_H && new_x < B_W) {
          neighbours.emplace_back(new_y, new_x);
        }
        new_y = y - dy;
        if (new_y >= 0 && new_x < B_W) {
          neighbours.emplace_back(new_y, new_x);
        }
        new_x = x - dx;
        if (new_y >= 0 && new_x >= 0) {
          neighbours.emplace_back(new_y, new_x);
        }
        new_y = y + dy;
        if (new_y < B_H && new_x >= 0) {
          neighbours.emplace_back(new_y, new_x);
        }
      }
    }
    return neighbours;
  }

 private:
  /*
  We store pointers to Pions here since otherwise we would need a 'NULL-pion'
  which would make some things awkward compared to using a nullptr
  */
  std::array<Pion, 1024> board{Pion()};
  char B_H;
  char B_W;
  // 1.024
  // std::vector<Pion> board;
};

#endif
